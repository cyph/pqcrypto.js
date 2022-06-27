//
//  api.h
//
//  Created by Bassham, Lawrence E (Fed) on 9/6/17.
//  Copyright © 2017 Bassham, Lawrence E (Fed). All rights reserved.
//


//   This is a sample 'api.h' for use 'sign.c'

#ifndef api_h
#define api_h

#include "config_HFE.h"

/** Set to 1 if you want to use MQsoft in SUPERCOP. */
#define SUPERCOP 1

//  Set these three values apropriately for your algorithm
#define CRYPTO_SECRETKEYBYTES SIZE_SK_BYTES
#define CRYPTO_PUBLICKEYBYTES SIZE_PK_BYTES
#define CRYPTO_BYTES SIZE_SIGN_HFE

/** Name of the current used cryptosystem. */
#ifdef GeMSS
    #define CRYPTO_ALGNAME "GeMSS"
#elif defined(BlueGeMSS)
    #define CRYPTO_ALGNAME "BlueGeMSS"
#elif defined(RedGeMSS)
    #define CRYPTO_ALGNAME "RedGeMSS"
#elif defined(FGeMSS)
    #define CRYPTO_ALGNAME "FGeMSS"
#elif defined (DualModeMS)
    #define CRYPTO_ALGNAME "Inner_DualModeMS"
#else
    #define CRYPTO_ALGNAME "MQsoft"
#endif

int
crypto_sign_keypair(unsigned char *pk, unsigned char *sk);

int
crypto_sign(unsigned char *sm, unsigned long long *smlen,
            const unsigned char *m, unsigned long long mlen,
            const unsigned char *sk);

int
crypto_sign_open(unsigned char *m, unsigned long long *mlen,
                 const unsigned char *sm, unsigned long long smlen,
                 const unsigned char *pk);

#endif /* api_h */
