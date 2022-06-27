#ifndef _API_H
#define _API_H

/** Set to 1 if you want to use MQsoft in SUPERCOP. */
#define SUPERCOP 1

#include "prefix_name.h"
#include "parameters_HFE.h"
#include "sizes_HFE.h"
#include "choice_crypto.h"
#include "sizes_crypto.h"


/** Size of the secret key in bytes. */
#define CRYPTO_SECRETKEYBYTES SIZE_SK
/** Size of the public key in bytes. */
#define CRYPTO_PUBLICKEYBYTES SIZE_PK
/** Size of the signature (without the document) in bytes. */
#define CRYPTO_BYTES SIZE_SIGN


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


#if SUPERCOP
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
#else
int
PREFIX_NAME(crypto_sign_keypair)(unsigned char *pk, unsigned char *sk);

int
PREFIX_NAME(crypto_sign)(unsigned char *sm, unsigned long long *smlen,
            const unsigned char *m, unsigned long long mlen,
            const unsigned char *sk);

int
PREFIX_NAME(crypto_sign_open)(unsigned char *m, unsigned long long *mlen,
                 const unsigned char *sm, unsigned long long smlen,
                 const unsigned char *pk);

#define crypto_sign_keypair PREFIX_NAME(crypto_sign_keypair)
#define crypto_sign PREFIX_NAME(crypto_sign)
#define crypto_sign_open PREFIX_NAME(crypto_sign_open)

#endif


#endif
