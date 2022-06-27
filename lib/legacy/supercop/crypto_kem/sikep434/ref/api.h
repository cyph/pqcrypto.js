//
//  api.h
//
//  Created by Bassham, Lawrence E (Fed) on 9/6/17.
//  Copyright © 2017 Bassham, Lawrence E (Fed). All rights reserved.
//


//   This is a sample 'api.h' for use 'sign.c'

#ifndef api_h
#define api_h

#define CRYPTO_SECRETKEYBYTES     374    // MSG_BYTES + SECRETKEY_B_BYTES + CRYPTO_PUBLICKEYBYTES bytes
#define CRYPTO_PUBLICKEYBYTES     330
#define CRYPTO_BYTES               16
#define CRYPTO_CIPHERTEXTBYTES    346    // CRYPTO_PUBLICKEYBYTES + MSG_BYTES bytes

// Change the algorithm name
#define CRYPTO_ALGNAME "SIKEp434"

// SIKE's key generation
// It produces a private key sk and computes the public key pk.
// Outputs: secret key sk (CRYPTO_SECRETKEYBYTES = 374 bytes)
//          public key pk (CRYPTO_PUBLICKEYBYTES = 330 bytes)
int crypto_kem_keypair(unsigned char *pk, unsigned char *sk);

// SIKE's encapsulation
// Input:   public key pk         (CRYPTO_PUBLICKEYBYTES = 330 bytes)
// Outputs: shared secret ss      (CRYPTO_BYTES = 16 bytes)
//          ciphertext message ct (CRYPTO_CIPHERTEXTBYTES = 346 bytes)
int crypto_kem_enc(unsigned char *ct, unsigned char *ss, const unsigned char *pk);

// SIKE's decapsulation
// Input:   secret key sk         (CRYPTO_SECRETKEYBYTES = 374 bytes)
//          ciphertext message ct (CRYPTO_CIPHERTEXTBYTES = 346 bytes)
// Outputs: shared secret ss      (CRYPTO_BYTES = 16 bytes)
int crypto_kem_dec(unsigned char *ss, const unsigned char *ct, const unsigned char *sk);

#endif /* api_h */
