/** 
 * \file parsing.cpp
 * \brief Implementation of parsing.h
 */

#include "parameters.h"
#include "parsing.h"
#include "ffi_vec.h"


/** 
 * \fn void locker_secret_key_to_string(unsigned char* sk, const unsigned char* seed)
 * \brief This function parses a secret key into a string
 *
 * The secret key is composed of the seed used to generate vectors <b>x</b> and <b>y</b>.
 *
 * \param[out] sk String containing the secret key
 * \param[in] seed Seed used to generate the vectors x and y
 */
void locker_secret_key_to_string(unsigned char* sk, const unsigned char* seed) {
  memcpy(sk, seed, SEEDEXPANDER_SEED_BYTES);
}



/** 
 * \fn void locker_secret_key_from_string(ffi_vec& x, ffi_vec& y, ffi_vec& support_w, const unsigned char* sk)
 * \brief This function parses a secret key from a string
 *
 * The secret key is composed of the seed used to generate vectors <b>x</b> and <b>y</b>.
 *
 * \param[out] x ffi_vec representation of vector x
 * \param[out] y ffi_vec representation of vector y
 * \param[out] support_w ffi_vec representation of the support of <b>x</b> and <b>y</b>
 * \param[in] sk String containing the secret key
 */
void locker_secret_key_from_string(secretKey &sk, const unsigned char* skString) {
  unsigned char sk_seed[SEEDEXPANDER_SEED_BYTES];
  memcpy(sk_seed, skString, SEEDEXPANDER_SEED_BYTES);
  AES_XOF_struct* sk_seedexpander = (AES_XOF_struct*) malloc(sizeof(AES_XOF_struct));
  seedexpander_init(sk_seedexpander, sk_seed, sk_seed + 32, SEEDEXPANDER_MAX_LENGTH);

  //Computes the support of the parity check matrix
  ffi_vec_set_random_full_rank_using_seedexpander(sk.F, PARAM_D, sk_seedexpander);
  //Computes two polynomials A and B defining the two cyclic parts of the parity check matrix
  //These are parts of the secret key. See the parsing part of the supporting documentation for more details

  ffi_vec_set_random_from_support_using_seedexpander(sk.x, PARAM_N, sk.F, PARAM_D, sk_seedexpander);
  ffi_vec_set_random_from_support_using_seedexpander(sk.y, PARAM_N, sk.F, PARAM_D, sk_seedexpander);

  free(sk_seedexpander);
}



/** 
 * \fn void locker_public_key_to_string(unsigned char* pk, const ffi_vec& s, const unsigned char* seed)
 * \brief This function parses a public key into a string
 *
 * The public key is composed of the syndrom <b>s</b> as well as the seed used to generate vector <b>h</b>.
 *
 * \param[out] pk String containing the public key
 * \param[in] s ffi_vec representation of vector <b>s</b>
 * \param[in] seed Seed used to generate the public key
 */
void locker_public_key_to_string(unsigned char* pkString, publicKey pk) {
  ffi_vec_to_string(pkString, pk.h, PARAM_N);
}



/** 
 * \fn void locker_public_key_from_string(ffi_vec& h, ffi_vec& s, const unsigned char* pk)
 * \brief This function parses a public key from a string
 *
 * The public key is composed of the syndrom <b>s</b> as well as the seed used to generate vector <b>h</b>.
 *
 * \param[out] h representation of vector <b>h</b>
 * \param[out] s representation of vector <b>s</b>
 * \param[in] pk String containing the public key
 */
void locker_public_key_from_string(publicKey &pk, const unsigned char *pkString) {
  ffi_vec_from_string(pk.h, PARAM_N, pkString);
}



/** 
 * \fn void locker_ciphertext_to_string(unsigned char* ct, const ffi_vec& sr, const ffi_vec& se) {
 * \brief This function parses a ciphertext into a string
 *
 * The ciphertext is composed of vectors <b>sr</b> and <b>se</b>.
 *
 * \param[out] ct String containing the ciphertext
 * \param[in] sr ffi_vec representation of vector <b>sr</b>
 * \param[in] se ffi_vec representation of vector <b>se</b>
 */
void locker_ciphertext_to_string(unsigned char* ct, const ciphertext &c) {
  ffi_vec_to_string(ct, c.u, PARAM_N);
  memcpy(ct + PARAM_N * GF2MBYTES, c.v, SHA512_BYTES);
  memcpy(ct + PARAM_N * GF2MBYTES + SHA512_BYTES, c.d, SHA512_BYTES);
}



/** 
 * \fn void locker_ciphertext_from_string(ffi_vec& sr, ffi_vec& se, const unsigned char* ct)
 * \brief This function parses a ciphertext from a string
 *
 * The ciphertext is composed of vectors <b>sr</b> and <b>se</b>.
 *
 * \param[out] sr ffi_vec representation of vector <b>sr</b>
 * \param[out] se ffi_vec representation of vector <b>se</b>
 * \param[in] ct String containing the ciphertext
 */
void locker_ciphertext_from_string(const unsigned char* ct, ciphertext &c) {
  ffi_vec_from_string(c.u, PARAM_N, ct);
  memcpy(c.v, ct + PARAM_N * GF2MBYTES, SHA512_BYTES);
  memcpy(c.d, ct + PARAM_N * GF2MBYTES + SHA512_BYTES, SHA512_BYTES);
}

