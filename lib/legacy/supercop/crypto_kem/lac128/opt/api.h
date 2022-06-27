#ifndef api_h
#define api_h

#include  "lac_param.h"
//  Set these three values apropriately for your algorithm
#define CRYPTO_SECRETKEYBYTES DIM_N+PK_LEN
#define CRYPTO_PUBLICKEYBYTES PK_LEN
#define CRYPTO_BYTES MESSAGE_LEN
#define CRYPTO_CIPHERTEXTBYTES CIPHER_LEN

// Change the algorithm name
#define CRYPTO_ALGNAME STRENGTH
//functions for pke
int crypto_encrypt_keypair( unsigned char *pk, unsigned char *sk);
int crypto_encrypt( unsigned char *c, unsigned long long *clen, const unsigned char *m, unsigned long long mlen, const unsigned char *pk);
int crypto_encrypt_open(unsigned char *m, unsigned long long *mlen,const unsigned char *c, unsigned long long clen,const unsigned char *sk);
//key generation
int kg(unsigned char *pk, char *sk);
//key generation with seed
int kg_seed(unsigned char *pk, char *sk, unsigned char *seed);
// encryption
int pke_enc(const unsigned char *pk, const unsigned char *m, unsigned char mlen, unsigned char *c);
// encryption with seed
int pke_enc_seed(const unsigned char *pk, const unsigned char *m, unsigned char mlen, unsigned char *c, unsigned char *seed);
// decrypt
int pke_dec(const char *sk, const unsigned char *c, unsigned char *m, unsigned char *mlen);

//functions for kem
int crypto_kem_keypair( unsigned char *pk, unsigned char *sk);
int crypto_kem_enc( unsigned char *ct, unsigned char *ss, const unsigned char *pk);
int crypto_kem_dec( unsigned char *ss, const unsigned char *ct, const unsigned char *sk);

int kem_enc_fo(const unsigned char *pk, unsigned char *k, unsigned char *c);
// fo encryption for cca security with seed
int kem_enc_fo_seed(const unsigned char *pk, unsigned char *k, unsigned char *c, unsigned char *seed);
// decrypt of fo mode
int kem_dec_fo(const unsigned char *pk, const char *sk, const  unsigned char *c, unsigned char *k);

//functions for ke
//Alice send: generate pk and sk, and send pk to Bob
int crypto_ke_alice_send(unsigned char *pk,unsigned char *sk);
// Bob receive: receive  pk, randomly choose m, and encryrpt m with pk to generate c, k=HASH(pk,m).
int crypto_ke_bob_receive(unsigned char *pk, unsigned char *c, unsigned char *k);
//Alice receive: receive c, and decrypt to get m and comute k=HASH(pk,m)
int crypto_ke_alice_receive(unsigned char *pk, unsigned char *sk, unsigned char *c, unsigned char *k);

//functions for ake
//Alice send: generate pk and sk, and send pk to Bob
int crypto_ake_alice_send(unsigned char *pk,unsigned char *sk, unsigned char *pk_b, unsigned char *sk_a, unsigned char *c, unsigned char *k1);
// Bob receive: receive  pk, randomly choose m, and encryrpt m with pk to generate c1 c2, k=HASH(pk,m).
int crypto_ake_bob_receive(unsigned char *pk_b, unsigned char *sk_b, unsigned char *pk_a, unsigned char *pk, unsigned char *c_in, unsigned char *c_out, unsigned char *k);
//Alice receive: receive c1,c2, and decrypt to get m and comute k=HASH(pk,m)
int crypto_ake_alice_receive(unsigned char *pk_a, unsigned char *sk_a,unsigned char *pk_b, unsigned char *pk, unsigned char *sk, unsigned char *c1, unsigned char *c_in, unsigned char *k1, unsigned char *k);

#endif /* api_h */
