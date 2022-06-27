#define CRYPTO_SECRETKEYBYTES 32
#define CRYPTO_PUBLICKEYBYTES 840
#define CRYPTO_BYTES 232
#define CRYPTO_ALGNAME "EdonS128K08N72nu8L4mu3"
#define CRYPTO_VERSION "1.0"

int crypto_sign_keypair(unsigned char *pk, unsigned char *sk); 

int crypto_sign( 
    unsigned char *sm, unsigned long long *smlen, 
    const unsigned char *m, unsigned long long mlen, 
    const unsigned char *sk
);

int crypto_sign_open(
    unsigned char *m, unsigned long long *mlen, 
    const unsigned char *sm, unsigned long long smlen, 
    const unsigned char *pk
);
