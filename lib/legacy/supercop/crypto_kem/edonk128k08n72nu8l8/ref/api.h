#define CRYPTO_SECRETKEYBYTES 32
#define CRYPTO_PUBLICKEYBYTES 840
#define CRYPTO_CIPHERTEXTBYTES 1184
#define CRYPTO_BYTES 32
#define CRYPTO_ALGNAME "EdonK128r08c072e8"
#define CRYPTO_VERSION "1.0"

int crypto_kem_keypair(unsigned char *pk, unsigned char *sk); 

int crypto_kem_enc(unsigned char *ct, unsigned char *ss, const unsigned char *pk);

int crypto_kem_dec(unsigned char *ss, const unsigned char *ct, const unsigned char *sk);
