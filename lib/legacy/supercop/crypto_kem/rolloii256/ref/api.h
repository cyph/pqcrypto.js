/**
 * \file api.h
 * \brief NIST KEM API used by the LOCKER scheme
 */






#define CRYPTO_ALGNAME "ROLLO-II-256"

#define CRYPTO_SECRETKEYBYTES 2533
#define CRYPTO_PUBLICKEYBYTES 2493
#define CRYPTO_BYTES 64
#define CRYPTO_CIPHERTEXTBYTES 2621


int crypto_kem_keypair(unsigned char* pk, unsigned char* sk);
int crypto_kem_enc(unsigned char* ct, unsigned char* ss, const unsigned char* pk);
int crypto_kem_dec(unsigned char* ss, const unsigned char* ct, const unsigned char* sk);



