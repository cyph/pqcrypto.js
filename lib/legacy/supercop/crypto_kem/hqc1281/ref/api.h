/**
 * \file api.h
 * \brief NIST KEM API used by the HQC_KEM IND-CCA2 scheme
 */




#define CRYPTO_ALGNAME                      "HQC_128_1"

#define CRYPTO_SECRETKEYBYTES               3165
#define CRYPTO_PUBLICKEYBYTES               3125
#define CRYPTO_BYTES                        64
#define CRYPTO_CIPHERTEXTBYTES              6234

// As a technicality, the public key is appended to the secret key in order to respect the NIST API. 
// Without this constraint, CRYPTO_SECRETKEYBYTES would be defined as 32

int crypto_kem_keypair(unsigned char* pk, unsigned char* sk);
int crypto_kem_enc(unsigned char* ct, unsigned char* ss, const unsigned char* pk);
int crypto_kem_dec(unsigned char* ss, const unsigned char* ct, const unsigned char* sk);
  
