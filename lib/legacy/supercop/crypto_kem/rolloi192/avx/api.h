/**
 * \file api.h
 * \brief NIST KEM API used by the ROLLOI_KEM IND-CCA2 scheme
 */


#ifndef API_H
#define API_H

#define CRYPTO_ALGNAME "ROLLOI-192"

#define CRYPTO_SECRETKEYBYTES 40
#define CRYPTO_PUBLICKEYBYTES 958
#define CRYPTO_BYTES 64
#define CRYPTO_CIPHERTEXTBYTES 958

// As a technicality, the public key is appended to the secret key in order to respect the NIST API. 
// Without this constraint, CRYPTO_SECRETKEYBYTES would be defined as 40


int crypto_kem_keypair(unsigned char* pk, unsigned char* sk);
int crypto_kem_enc(unsigned char* ct, unsigned char* ss, const unsigned char* pk);
int crypto_kem_dec(unsigned char* ss, const unsigned char* ct, const unsigned char* sk);

#endif

