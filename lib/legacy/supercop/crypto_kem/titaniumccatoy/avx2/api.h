/* ****************************** *
 * Titanium_CCA_toy               *
 * Implemented by Raymond K. ZHAO *
 * ****************************** */

#ifndef api_h
#define api_h

#define CRYPTO_SECRETKEYBYTES 12224
#define CRYPTO_PUBLICKEYBYTES 12192
#define CRYPTO_BYTES 32
#define CRYPTO_CIPHERTEXTBYTES 2720

#define CRYPTO_ALGNAME "Titanium CCA toy"

int crypto_kem_keypair(
unsigned char *pk,
unsigned char *sk
);

int crypto_kem_enc(
unsigned char *ct,
unsigned char *ss,
const unsigned char *pk
);

int crypto_kem_dec(
unsigned char *ss,
const unsigned char *ct,
const unsigned char *sk
);

#endif /* api_h */
