#define CRYPTO_SECRETKEYBYTES 32
#define CRYPTO_PUBLICKEYBYTES 2576
#define CRYPTO_CIPHERTEXTBYTES 2336
#define CRYPTO_BYTES 32
#define CRYPTO_ALGNAME "EdonK128REF"
#define CRYPTO_VERSION "1.0"

int crypto_kem_keypair(unsigned char *pk, unsigned char *sk); 

int crypto_kem_enc(unsigned char *ct, unsigned char *ss, const unsigned char *pk);

int crypto_kem_dec(unsigned char *ss, const unsigned char *ct, const unsigned char *sk);
