// api.h
// 2017-09-23  Markku-Juhani O. Saarinen <mjos@iki.fi>

#ifndef _API_H_
#define _API_H_

// Definitions for HILA5 as a KEM

#define CRYPTO_ALGNAME "HILA5"
#define CRYPTO_SECRETKEYBYTES 1824
#define CRYPTO_PUBLICKEYBYTES 1824
#define CRYPTO_BYTES 32
#define CRYPTO_CIPHERTEXTBYTES 2012

// == Prototypes ===========================================================

// Generates a keypair - pk is the public key and sk is the secret key.
int crypto_kem_keypair(unsigned char *pk, unsigned char *sk);

// Encrypt - pk is the public key, ct is a key encapsulation message
// (ciphertext), ss is the shared secret.
int crypto_kem_enc(unsigned char *ct, unsigned char *ss,
    const unsigned char *pk);

// Decrypt - ct is a key encapsulation message (ciphertext),
// sk is the private key, ss is the shared secret
int crypto_kem_dec(unsigned char *ss, const unsigned char *ct,
    const unsigned char *sk);

#endif
