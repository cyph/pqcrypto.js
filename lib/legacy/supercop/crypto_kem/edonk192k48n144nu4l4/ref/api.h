#define CRYPTO_SECRETKEYBYTES 32
#define CRYPTO_PUBLICKEYBYTES 3672
#define CRYPTO_CIPHERTEXTBYTES 3504
#define CRYPTO_BYTES 48
#define CRYPTO_ALGNAME "EdonK192Span4r48c144e4"
#define CRYPTO_VERSION "1.0"

int crypto_kem_keypair(unsigned char *pk, unsigned char *sk); 

int crypto_kem_enc(unsigned char *ct, unsigned char *ss, const unsigned char *pk);

int crypto_kem_dec(unsigned char *ss, const unsigned char *ct, const unsigned char *sk);
