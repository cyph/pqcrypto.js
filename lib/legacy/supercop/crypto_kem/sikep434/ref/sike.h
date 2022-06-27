//
// Supersingular Isogeny Key Encapsulation Ref. Library
//
// InfoSec Global Inc., 2017-2020
// Basil Hess <basil.hess@infosecglobal.com>
//

/** @file
SIKE KEM and PKE functions:
 PKE encryption
 PKE decryption
 KEM encapsulation
 KEM decapsulation
*/

#ifndef ISOGENY_REF_SIKE_H
#define ISOGENY_REF_SIKE_H

#include <fp.h>

/**
 * ALICE or BOB
 */
typedef enum {ALICE, BOB} party_t;

/**
 * A private key can be represented by a multi-precision value.
 */
typedef mp sike_private_key;

/**
 * A message m
 */
typedef unsigned char sike_msg;

/**
 * SIKE PKE encryption
 *
 * For B:
 * - c_0 == PK_3 <- B's keygen function using PK_3, SK_3
 * - j <- Shared secret (j-invariant) using PK_2, SK_3
 * - h <- F(j)
 * - c_1 <- h + m
 * - return (c_0, c_1)
 * -
 * @param params SIKE parameters
 * @param pk3 Public key of the other party
 * @param m message
 * @param sk2 Own private key
 * @param c0 First component of encryption
 * @param c1 Second component of encryption
 */
void sike_pke_enc(const sike_params_t* params,
                  const sike_public_key_t* pk3,
                  const sike_msg* m,
                  const sike_private_key sk2,
                  sike_public_key_t* c0,
                  unsigned char* c1 );

/**
 * SIKE PKE decryption
 *
 * For B:
 * - B's keygen function using PK_2, SK_3, evaluating on B's curve
 * - Shared secret (j-invariant),
 *
 * @param params SIKE parameters
 * @param sk3 Own private key
 * @param c0 First component of encryption
 * @param c1 Second component of encryption
 * @param m Recovered message
 */
void sike_pke_dec(const sike_params_t* params,
                  const sike_private_key sk3,
                  const sike_public_key_t* c0,
                  const unsigned char* c1,
                  sike_msg* m);

/**
 * SIKE KEM key generation (KeyGen)
 *
 * @param params SIKE parameters
 * @param pk3 public key
 * @param sk3 private key
 * @param s   SIKE parameter s
 */
void sike_kem_keygen(const sike_params_t *params,
                     sike_public_key_t *pk3,
                     sike_private_key sk3,
                     unsigned char *s);

/**
 * SIKE KEM Encapsulation
 *
 * For B:
 * - m <- random(0,1)^l
 * - r <- G(m || pk3)
 * - (c0, c1) <- Enc(pk3, m, r)
 * - K <- H(m || (c0, c1))
 *
 * @param params SIKE parameters
 * @param pk3 Other party's public key
 * @param c0 First component of encryption
 * @param c1 Second component of encryption
 * @param K key (do not share with other party)
 */
void sike_kem_encaps(const sike_params_t *params,
                     const sike_public_key_t *pk3,
                     sike_public_key_t *c0,
                     unsigned char *c1,
                     unsigned char *K);

/**
 * SIKE KEM Decapsulation
 *
 * For B:
 * - m'  <- Dec(sk3, (c0, c1))
 * - r'  <- G(m' || pk3)
 * - c0' <- isogen_2(r')
 * - if (c0' == c0) K <- H(m' || (c0, c1))
 * - else           K <- H(s || (c0, c1))
 * @param params SIKE parameters
 * @param pk3 Own public key
 * @param sk3 Own private key
 * @param c0 First component of the encryption
 * @param c1 Second component of the encrytion
 * @param s SIKE parameter `s`
 * @param K decapsulated keys
 *
 */
void sike_kem_decaps(const sike_params_t *params,
                     const sike_public_key_t *pk3,
                     const sike_private_key sk3,
                     const sike_public_key_t *c0,
                     const unsigned char *c1,
                     const unsigned char *s,
                     unsigned char *K);


#endif //ISOGENY_REF_SIKE_H
