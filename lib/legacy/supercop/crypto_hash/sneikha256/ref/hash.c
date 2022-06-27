//  hash.c
//  2019-02-20  Markku-Juhani O. Saarinen <mjos@pqshield.com>
//  Copyright (C) 2019, PQShield Ltd. Please see LICENSE.

//  Reference SNEIKHA hash using the BLNK calls

#include "api.h"
#include "crypto_hash.h"
#include "blnk.h"

//  Single-call NIST interface

int crypto_hash(
    unsigned char *out,
    const unsigned char *in,
    unsigned long long inlen)
{
    blnk_t  ctx;                        //  Local state

    //  Clear state, set parameters
    blnk_clr(&ctx, SNEIKHA_RATE, SNEIKHA_ROUNDS);

    //  Process input
    blnk_put(&ctx, BLNK_AD, in, (size_t) inlen);
    blnk_fin(&ctx, BLNK_AD);

    //  Get the hash
    blnk_get(&ctx, BLNK_HASH, out, CRYPTO_BYTES);

    //  blnk_fin(&ctx, BLNK_HASH);      //  For intermediate hashes

    return 0;                           //  Success
}

