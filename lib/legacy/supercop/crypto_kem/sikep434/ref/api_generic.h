//
// Supersingular Isogeny Key Encapsulation Ref. Library
//
// InfoSec Global Inc., 2017-2020
// Basil Hess <basil.hess@infosecglobal.com>
//

#ifndef ISOGENY_REF_SIDH_API_GENERIC_H
#define ISOGENY_REF_SIDH_API_GENERIC_H


#include <sike_params.h>

// SIKE's key generation
// It produces a private key sk and computes the public key pk.
// Outputs: secret key sk
//          public key pk
int crypto_kem_keypair_generic(const sike_params_t *params, unsigned char *pk, unsigned char *sk);

// SIKE's encapsulation
// Input:   public key pk
// Outputs: shared secret ss
//          ciphertext message ct
int crypto_kem_enc_generic(const sike_params_t *params, unsigned char *ct, unsigned char *ss, const unsigned char *pk);

// SIKE's decapsulation
// Input:   secret key sk
//          ciphertext message ct
// Outputs: shared secret ss
int crypto_kem_dec_generic(const sike_params_t *params, unsigned char *ss, const unsigned char *ct,
                           const unsigned char *sk);

#endif //ISOGENY_REF_SIDH_API_GENERIC_H
