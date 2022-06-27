//  encrypt.c
//  2019-02-23  Markku-Juhani O. Saarinen <mjos@pqshield.com>
//  Copyright (C) 2019, PQShield Ltd. Please see LICENSE.

//  "Optimized" (self-contained) AEAD

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "api.h"
#include "crypto_aead.h"
#include "sneik_param.h"

#define BLNK_LAST   0x01                    // Last (padded) block of domain
#define BLNK_FULL   0x02                    // Full state input (0: RATE input)
#define BLNK_AD     0x10                    // Authenticated data (in)
#define BLNK_ADF    (BLNK_AD | BLNK_FULL)   // Full-state assoc. data (in)
#define BLNK_KEY    0x20                    // Secret key (in)
#define BLNK_KEYF   (BLNK_KEY | BLNK_FULL)  // Secret key/nonce block (in)
#define BLNK_HASH   0x40                    // Hash/Authentication tag (out)
#define BLNK_PTCT   0x70                    // Plaintext or Ciphertext (in/out)

//  The common ID / Keying / Nonce / AD part

static void sneiken_key_ad(uint8_t s[BLNK_BLOCK],
    const uint8_t *ad, size_t adlen,        // AD and its size
    const uint8_t *npub, const uint8_t *k)  // nonce and secret key
{
    const uint8_t id[6] = { 'a', 'e',
        SNEIKEN_RATE, CRYPTO_KEYBYTES, CRYPTO_NPUBBYTES, CRYPTO_ABYTES };
    size_t i;

    //  Key block: id | k | iv

    memcpy(s, id, sizeof(id));
    memcpy(s + sizeof(id), k, CRYPTO_KEYBYTES);
    memcpy(s + sizeof(id) + CRYPTO_KEYBYTES, npub, CRYPTO_NPUBBYTES);
    s[sizeof(id) + CRYPTO_KEYBYTES + CRYPTO_NPUBBYTES] = 0x01;
    memset(s + sizeof(id) + CRYPTO_KEYBYTES + CRYPTO_NPUBBYTES + 1, 0x00,
        BLNK_BLOCK - (sizeof(id) + CRYPTO_KEYBYTES + CRYPTO_NPUBBYTES + 1));
    sneik_f512(s, BLNK_KEYF | BLNK_LAST, SNEIKEN_ROUNDS);

    //  Associated Data (full state)

    while (adlen >= BLNK_BLOCK) {
        for (i = 0; i < BLNK_BLOCK; i++) {
            s[i] ^= ad[i];
        }
        sneik_f512(s, BLNK_ADF, SNEIKEN_ROUNDS);
        ad += BLNK_BLOCK;
        adlen -= BLNK_BLOCK;
    }
    for (i = 0; i < adlen; i++)
        s[i] ^= ad[i];
    s[adlen] ^= 0x01;                       //  full-state "last" padding
    sneik_f512(s, BLNK_ADF | BLNK_LAST, SNEIKEN_ROUNDS);
}

// Encryption

int crypto_aead_encrypt(
    unsigned char *c, unsigned long long *clen,         // Ciphertext out
    const unsigned char *m, unsigned long long mlen,    // Plaintext in
    const unsigned char *ad, unsigned long long adlen,  // AAD in
    const unsigned char *nsec,                          // Secret Nonce in
    const unsigned char *npub,                          // Public Nonce in
    const unsigned char *k)                             // Secret Key in
{
    uint8_t s[BLNK_BLOCK];                  //  Local state
    size_t  i, l;

    (void)(nsec);                           //  (Supress warning)

    //  Keying and AD

    sneiken_key_ad(s, ad, (size_t) adlen, npub, k);

    //  Encrypt Message (this version can handle only c >= m overlap)

    l = (size_t) mlen;
    *clen = l + CRYPTO_ABYTES;              //  store length

    while (l >= SNEIKEN_RATE) {
        for (i = 0; i < SNEIKEN_RATE; i++) {
            c[i] = m[i] ^ s[i];
            s[i] = c[i];
        }
        sneik_f512(s, BLNK_PTCT, SNEIKEN_ROUNDS);
        m += SNEIKEN_RATE;
        c += SNEIKEN_RATE;
        l -= SNEIKEN_RATE;
    }
    for (i = 0; i < l; i++) {
        c[i] = m[i] ^ s[i];
        s[i] = c[i];
    }
    s[l] ^= 0x01;                           //  "last" padding
    s[SNEIKEN_RATE - 1] ^= 0x80;               //  rate padding
    c += l;
    sneik_f512(s, BLNK_PTCT | BLNK_LAST, SNEIKEN_ROUNDS);

    memcpy(c, s, CRYPTO_ABYTES);            //  Get MAC

    return 0;                               //  Success.
}


// Decryption and authentication

int crypto_aead_decrypt(
    unsigned char *m, unsigned long long *outputmlen,   // Plaintext out
    unsigned char *nsec,                                // Secret Nonce out
    const unsigned char *c, unsigned long long clen,    // Ciphertext in
    const unsigned char *ad, unsigned long long adlen,  // AAD in
    const unsigned char *npub,                          // Public Nonce in
    const unsigned char *k)                             // Secret Key in
{
    uint8_t s[BLNK_BLOCK];                  //  Local state
    size_t  i, l;
    uint8_t t;

    (void)(nsec);                           //  (Supress warning)

    //  Keying and AD

    sneiken_key_ad(s, ad, (size_t) adlen, npub, k);

    //  Output length

    l = (size_t) clen;
    if (l < CRYPTO_ABYTES)                  //  Invalid length
        return -1;
    l -= CRYPTO_ABYTES;                     //  same as mlen now
    *outputmlen = l;

    //  Decrypt Message (this version can handle only m >= c overlap)

    while (l >= SNEIKEN_RATE) {
        for (i = 0; i < SNEIKEN_RATE; i++) {
            t = c[i];
            m[i] = t ^ s[i];
            s[i] = t;
        }
        sneik_f512(s, BLNK_PTCT, SNEIKEN_ROUNDS);
        m += SNEIKEN_RATE;
        c += SNEIKEN_RATE;
        l -= SNEIKEN_RATE;
    }
    for (i = 0; i < l; i++) {
        t = c[i];
        m[i] = t ^ s[i];
        s[i] = t;
    }
    s[l] ^= 0x01;                           //  "last" padding
    s[SNEIKEN_RATE - 1] ^= 0x80;               //  rate padding
    c += l;
    sneik_f512(s, BLNK_PTCT | BLNK_LAST, SNEIKEN_ROUNDS);

    t = 0;                                  //  constant time compare
    for (i = 0; i < CRYPTO_ABYTES; i++)
        t |= s[i] ^ c[i];

    if (t != 0)                             //  Authentication failure
        return -1;

    return 0;                               //  Success
}

