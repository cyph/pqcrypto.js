/**
 * \file api.h
 * \brief NIST KEM API used by the LAKE scheme
 */


#ifndef API_H
#define API_H


#define CRYPTO_ALGNAME "LAKE-II"

#define CRYPTO_SECRETKEYBYTES 40
#define CRYPTO_PUBLICKEYBYTES 636
#define CRYPTO_BYTES 64
#define CRYPTO_CIPHERTEXTBYTES 636


int crypto_kem_keypair(unsigned char* pk, unsigned char* sk);
int crypto_kem_enc(unsigned char* ct, unsigned char* ss, const unsigned char* pk);
int crypto_kem_dec(unsigned char* ss, const unsigned char* ct, const unsigned char* sk);

#endif

