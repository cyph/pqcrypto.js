/**
 * \file api.h
 * \brief NIST KEM API used by the LOCKER scheme
 */


#ifndef API_H
#define API_H


#define CRYPTO_ALGNAME "LOCKER-I"

#define CRYPTO_SECRETKEYBYTES 787
#define CRYPTO_PUBLICKEYBYTES 747
#define CRYPTO_BYTES 64
#define CRYPTO_CIPHERTEXTBYTES 875


int crypto_kem_keypair(unsigned char* pk, unsigned char* sk);
int crypto_kem_enc(unsigned char* ct, unsigned char* ss, const unsigned char* pk);
int crypto_kem_dec(unsigned char* ss, const unsigned char* ct, const unsigned char* sk);

#endif

