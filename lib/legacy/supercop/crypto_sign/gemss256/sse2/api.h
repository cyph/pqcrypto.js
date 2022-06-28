#ifndef _API_H
#define _API_H

#include "config_HFE.h"

#define CRYPTO_SECRETKEYBYTES SIZE_SK_BYTES
#define CRYPTO_PUBLICKEYBYTES SIZE_PK_BYTES
#define CRYPTO_BYTES SIZE_SIGNATURE_BYTES

#define CRYPTO_ALGNAME "GeMSS"

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

#endif
