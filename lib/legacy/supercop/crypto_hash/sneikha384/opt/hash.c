//  hash.c
//  2019-02-24  Markku-Juhani O. Saarinen <mjos@pqshield.com>
//  Copyright (C) 2019, PQShield Ltd. Please see LICENSE.

//  "Optimized" (self-contained) hash

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "api.h"
#include "crypto_hash.h"
#include "sneik_param.h"

#define BLNK_LAST   0x01                    //  Last (padded) block of domain
#define BLNK_AD     0x10                    //  Authenticated data (in)
#define BLNK_HASH   0x40                    //  Hash/Authentication tag (out)

//  Single-call NIST interface

int crypto_hash(
    unsigned char *out,
    const unsigned char *in,
    unsigned long long inlen)
{
    uint8_t s[BLNK_BLOCK];                  //  Local state
    size_t  i, l;

    memset(s, 0x00, BLNK_BLOCK);            //  Initialize state

    l = (size_t) inlen;                     //  Use a natural-sized type
    while (l >= SNEIKHA_RATE) {             //  Absorb blocks
        for (i = 0; i < SNEIKHA_RATE; i++) {
            s[i] ^= in[i];
        }
        sneik_f512(s, BLNK_AD, SNEIKHA_ROUNDS);
        in += SNEIKHA_RATE;
        l -= SNEIKHA_RATE;
    }
    for (i = 0; i < l; i++) {               //  Last block
        s[i] ^= in[i];
    }
    s[l] ^= 0x01;                           //  "last" padding
    s[SNEIKHA_RATE - 1] ^= 0x80;            //  rate padding
    in += l;
    sneik_f512(s, BLNK_AD | BLNK_LAST, SNEIKHA_ROUNDS);

    i = CRYPTO_BYTES;
    while (i > SNEIKHA_RATE) {
        memcpy(out, s, SNEIKHA_RATE);
        out += SNEIKHA_RATE;
        i -= SNEIKHA_RATE;
        sneik_f512(s, BLNK_HASH, SNEIKHA_ROUNDS);
    }
    memcpy(out, s, i);                      //  last partial block

    return 0;                               //  Success
}

