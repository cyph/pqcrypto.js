/**
 * \file api.h
 * \brief NIST KEM API used by the LAKE scheme
 */






#define CRYPTO_ALGNAME "ROLLO-I-192"

#define CRYPTO_SECRETKEYBYTES 40
#define CRYPTO_PUBLICKEYBYTES 590
#define CRYPTO_BYTES 64
#define CRYPTO_CIPHERTEXTBYTES 590


int crypto_kem_keypair(unsigned char* pk, unsigned char* sk);
int crypto_kem_enc(unsigned char* ct, unsigned char* ss, const unsigned char* pk);
int crypto_kem_dec(unsigned char* ss, const unsigned char* ct, const unsigned char* sk);



