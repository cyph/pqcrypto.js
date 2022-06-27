// Yarara-128 implementation
// Target: Optimized for 64 bit platforms and GCC
// Copyright (c) 2019, Miguel Montes <mmontes@iua.edu.ar>
#include <stdint.h>
#include "api.h"
#include "crypto_aead.h"

#define SBOX_SIZE 4
#define STATE_SIZE 32
#define RATE 8
#define PADDING_BYTE 0x80
#define INITIAL_ROUNDS 10
#define ROUNDS 6
#define FINAL_ROUNDS ROUNDS
#define ROTR(x, n) (((x) >> (n)) | ((x) << (64 - (n))))
#define MIX(x, a, b) x ^= ROTR(x, a) ^ ROTR(x, b)


#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define load64(x,s) (x) = *(uint64_t*)(s)
#define store64(s,x) *(uint64_t*)(s) = (x)
#elif defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ && defined(__GNUC__)
#define load64(x,s) x = __builtin_bswap64(*(uint64_t*)(s))
#define store64(s,x) *(uint64_t*)(s) = __builtin_bswap64(x)
#else
void _load64(uint64_t *x, const unsigned char *S) {
    *x = (uint64_t)S[0];
    for (int i = 1; i < 8; ++i) {
        *x <<= 8;
        *x |= (uint64_t)S[i];
    }
}

void _store64(unsigned char *S, uint64_t x) {
     S[7] = (unsigned char)x;
    for (int i = 6; i >= 0; --i) {
        x >>= 8;
        S[i] = (unsigned char)x;
    }
}
#define load64(x,s) _load64(&(x), (s))
#define store64(s,x) _store64((s),(x)
#endif



#define ROUND(rconst) {\
        x1 ^= (rconst);\
        t0 = x0 ^ (x1 & x2) ^ (x1 & x3) ^ (x2 & x3) ^ (x0 & x2 & x3) ^ (x1 & x2 & x3);\
        t1 = ~(x1 ^ x2 ^ (x0 & x2) ^ (x0 & x3) ^ (x2 & x3) ^ (x0 & x1 & x2) ^ (x0 & x2 & x3));\
        t2 = x1 ^ x3 ^ (x0 & x2) ^ (x2 & x3) ^ (x0 & x1 & x3) ^ (x0 & x2 & x3);\
        t3 = x0 ^ x2 ^ x3 ^ (x1 & x3) ^ (x0 & x1 & x2) ^ (x0 & x1 & x3);\
        MIX(t0, 19, 28);\
        MIX(t1, 61, 39);\
        MIX(t2, 1, 6);\
        MIX(t3, 10, 17);\
        x0 = t1 ^ t2 ^ t3;\
        x1 = t0 ^ t2 ^ t3;\
        x2 = t0 ^ t1 ^ t3;\
        x3 = t0 ^ t1 ^ t2;\
        MIX(x0, 19, 28);\
        MIX(x1, 61, 39);\
        MIX(x2, 1, 6);\
        MIX(x3, 10, 17);\
}


#define initial_permutation() {\
    uint64_t t0, t1, t2, t3;\
    ROUND(0xbe);\
    ROUND(0xb1);\
    ROUND(0xa0);\
    ROUND(0x93);\
    ROUND(0x82);\
    ROUND(0xf5);\
    ROUND(0xe4);\
    ROUND(0xd7);\
    ROUND(0xc6);\
    ROUND(0x39);\
}

#define m_permutation() {\
    uint64_t t0, t1, t2, t3;\
    ROUND(0xae);\
    ROUND(0xa1);\
    ROUND(0xb0);\
    ROUND(0x83);\
    ROUND(0x92);\
    ROUND(0xe5);\
 }


#define ad_permutation() {\
    uint64_t t0, t1, t2, t3;\
    ROUND(0xad);\
    ROUND(0xa2);\
    ROUND(0xb3);\
    ROUND(0x80);\
    ROUND(0x91);\
    ROUND(0xe6);\
}

#define final_permutation() {\
    uint64_t t0, t1, t2, t3;\
    ROUND(0xeb);\
    ROUND(0xe4);\
    ROUND(0xf5);\
    ROUND(0xc6);\
    ROUND(0xd7);\
    ROUND(0xa0);\
}

#define initialize(k,npub){\
    load64(x0, k);\
    load64(x1, k+8);\
    load64(x2,npub);\
    load64(x3,npub+8);\
    initial_permutation();\
}


#define process_ad(ad,adlen){\
    unsigned char one[] = {0,0,0,0,0,0,0,1};\
    uint64_t word;\
    if (adlen) {\
        int remaining = (adlen) % RATE;\
        unsigned long long full_blocks = (adlen) - remaining;\
        for (unsigned long long i = 0; i < full_blocks; i += RATE) {\
            load64(word,(ad)+i);\
            x0 ^= word;\
            ad_permutation();\
        }\
        unsigned char block[RATE] = {0};\
        for (int i = 0; i < remaining; i++) {\
            block[i] = (ad)[full_blocks + i];\
        }\
        block[remaining] = PADDING_BYTE;\
        load64(word,block);\
        x0 ^= word;\
        ad_permutation();\
    }\
    load64(word, one);\
    x3 ^= word;\
}

#define encrypt(c, m, mlen) {\
    int remaining = (mlen) % RATE;\
    unsigned long long full_blocks = (mlen) - remaining;\
    uint64_t word;\
    for (unsigned long long i = 0; i < full_blocks; i += RATE) {\
        load64(word, (m)+i);\
        x0 ^= word;\
        store64((c)+i,x0);\
        m_permutation();\
    }\
    unsigned char block[RATE] = {0};\
    for (int j = 0; j < remaining; j++) {\
        block[j] = (m)[full_blocks + j];\
    }\
    block[remaining] = PADDING_BYTE;\
    load64(word,block);\
    x0 ^= word;\
    store64(block, x0);\
    for (int j = 0; j < remaining; j++){\
        (c)[full_blocks + j] = block[j];\
    }\
    m_permutation();\
}

#define decrypt(m, c, mlen) {\
    int remaining = (mlen) % RATE;\
    unsigned long long full_blocks = (mlen) - remaining;\
    uint64_t word;\
    for (unsigned long long i = 0; i < full_blocks; i += RATE) {\
        word = x0;\
        load64(x0, (c)+i);\
        word ^= x0;\
        store64((m)+i, word);\
        m_permutation();\
    }\
    unsigned char block[RATE];\
    store64(block,x0);\
    for (int j = 0; j < remaining; j++) {\
        (m)[full_blocks + j] = (c)[full_blocks + j] ^ block[j];\
        block[j] = (c)[full_blocks + j];\
    }\
    block[remaining] ^= PADDING_BYTE;\
    load64(x0, block);\
    m_permutation();\
}

#define finalize(tag) {\
    int l = CRYPTO_ABYTES;\
    int i = 0;\
    while (l >= RATE) {\
        final_permutation();\
        store64((tag)+i, x0);\
        l -= RATE;\
        i += RATE;\
    }\
    if (l > 0) {\
        final_permutation();\
        unsigned char block[RATE];\
        store64(block,x0);\
        for (int j = 0; j < l; j++) {\
            (tag)[i++] = block[j];\
        }\
    }\
}

int crypto_aead_encrypt(
    unsigned char *c, unsigned long long *clen,
    const unsigned char *m, unsigned long long mlen,
    const unsigned char *ad, unsigned long long adlen,
    const unsigned char *nsec,
    const unsigned char *npub,
    const unsigned char *k) {
    (void)nsec;
    uint64_t x0,x1,x2,x3;

    initialize(k, npub);
    process_ad(ad, adlen);
    encrypt(c, m, mlen);
    finalize(c + mlen);
    *clen = mlen + CRYPTO_ABYTES;
    return 0;
}

int crypto_aead_decrypt(
    unsigned char *m, unsigned long long *mlen,
    unsigned char *nsec,
    const unsigned char *c, unsigned long long clen,
    const unsigned char *ad, unsigned long long adlen,
    const unsigned char *npub,
    const unsigned char *k) {
    (void)nsec;
    unsigned char tag[CRYPTO_ABYTES];
    *mlen = 0;
    if (clen < CRYPTO_ABYTES)
        return -1;
    uint64_t x0,x1,x2,x3;

    initialize(k, npub);
    process_ad(ad, adlen);
    decrypt(m, c, clen - CRYPTO_ABYTES);
    finalize(tag);
    *mlen = clen - CRYPTO_ABYTES;
    int result = 0;
    for (int j = 0; j < CRYPTO_ABYTES; j++) {
        result |= tag[j] ^ c[(*mlen) + j];
    }
    return -result;
}
