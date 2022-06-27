// Coral 256 implementation
// Target: Optimized for 64 bit platforms and GCC
// Copyright (c) 2019, Miguel Montes <mmontes@iua.edu.ar>
#include <stdint.h>
#include "api.h"
#include "crypto_hash.h"

#define SBOX_SIZE 4
#define STATE_SIZE 32
#define RATE 4
#define PADDING_BYTE 0x80
#define INITIAL_ROUNDS 10
#define ROUNDS 6
#define FINAL_ROUNDS ROUNDS
#define ROTR(x, n) (((x) >> (n)) | ((x) << (64 - (n))))
#define MIX(x, a, b) x ^= ROTR(x, a) ^ ROTR(x, b)

#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define load64(x, s) (x) = *(uint64_t *)(s)
#define store64(s, x) *(uint64_t *)(s) = (x)
#define load32(x, s) (x) = *(uint32_t *)(s)
#define store32(s, x) *(uint32_t *)(s) = (x)
#elif defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ && defined(__GNUC__)
#define load32(x, s) x = __builtin_bswap32(*(uint32_t *)(s))
#define store32(s, x) *(uint32_t *)(s) = __builtin_bswap32(x)
#define load64(x, s) x = __builtin_bswap64(*(uint64_t *)(s))
#define store64(s, x) *(uint64_t *)(s) = __builtin_bswap64(x)
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
#define load64(x, s) _load64(&(x), (s))
#define store64(s, x) _store64((s),(x)

void _load32(uint32_t *x, const unsigned char *S) {
    *x = (uint32_t)S[0];
    for (int i = 1; i < 4; ++i) {
        *x <<= 8;
        *x |= (uint32_t)S[i];
    }
}

void _store32(unsigned char *S, uint32_t x) {
    S[3] = (unsigned char)x;
    for (int i = 2; i >= 0; --i) {
        x >>= 8;
        S[i] = (unsigned char)x;
    }
}
#define load32(x, s) _load32(&(x), (s))
#define store32(s, x) _store32((s),(x)
#endif

#define ROUND(rconst)                                                                          \
    {                                                                                          \
        x1 ^= (rconst);                                                                        \
        t0 = x0 ^ (x1 & x2) ^ (x1 & x3) ^ (x2 & x3) ^ (x0 & x2 & x3) ^ (x1 & x2 & x3);         \
        t1 = ~(x1 ^ x2 ^ (x0 & x2) ^ (x0 & x3) ^ (x2 & x3) ^ (x0 & x1 & x2) ^ (x0 & x2 & x3)); \
        t2 = x1 ^ x3 ^ (x0 & x2) ^ (x2 & x3) ^ (x0 & x1 & x3) ^ (x0 & x2 & x3);                \
        t3 = x0 ^ x2 ^ x3 ^ (x1 & x3) ^ (x0 & x1 & x2) ^ (x0 & x1 & x3);                       \
        MIX(t0, 19, 28);                                                                       \
        MIX(t1, 61, 39);                                                                       \
        MIX(t2, 1, 6);                                                                         \
        MIX(t3, 10, 17);                                                                       \
        x0 = t1 ^ t2 ^ t3;                                                                     \
        x1 = t0 ^ t2 ^ t3;                                                                     \
        x2 = t0 ^ t1 ^ t3;                                                                     \
        x3 = t0 ^ t1 ^ t2;                                                                     \
        MIX(x0, 19, 28);                                                                       \
        MIX(x1, 61, 39);                                                                       \
        MIX(x2, 1, 6);                                                                         \
        MIX(x3, 10, 17);                                                                       \
    }

#define permutation() {\
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

int crypto_hash(unsigned char *out, const unsigned char *in, unsigned long long inlen) {
    uint64_t x0 = 0, x1 = 0, x2 = 0, x3 = 0;
    uint32_t word;
    int remaining = inlen % RATE;
    unsigned long long full_blocks = inlen - remaining;
    for (unsigned long long i = 0; i < full_blocks; i += RATE) {
        load32(word, in + i);
        x0 ^= ((uint64_t)word) << 32;
        permutation();
    }
    unsigned char block[RATE] = {0};
    for (int i = 0; i < remaining; i++) {
        block[i] = in[full_blocks + i];
    }
    block[remaining] = PADDING_BYTE;
    load32(word, block);
    x0 ^= ((uint64_t)word) << 32;
    for (int i = 0; i < CRYPTO_BYTES; i += RATE) {
        permutation();
        store32(out + i, (x0 >> 32));
    }

    return 0;
}
