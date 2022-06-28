/** 
 * \file parsing.cpp
 * \brief Implementation of parsing.h
 */

#include "string.h"
#include "parameters.h"
#include "ffi_vec.h"


/** 
 * \fn void ouroborosr_secret_key_to_string(unsigned char* sk, const unsigned char* seed)
 * \brief This function parses a secret key into a string
 *
 * The secret key is composed of the seed used to generate vectors <b>x</b> and <b>y</b>.
 *
 * \param[out] sk String containing the secret key
 * \param[in] seed Seed used to generate the vectors x and y
 */
void ouroborosr_secret_key_to_string(unsigned char* sk, const unsigned char* seed) {
  memcpy(sk, seed, SEEDEXPANDER_SEED_BYTES);
}



/** 
 * \fn void ouroborosr_secret_key_from_string(ffi_vec& x, ffi_vec& y, ffi_vec& F, const unsigned char* sk)
 * \brief This function parses a secret key from a string
 *
 * The secret key is composed of the seed used to generate vectors <b>x</b> and <b>y</b>.
 *
 * \param[out] x ffi_vec representation of vector x
 * \param[out] y ffi_vec representation of vector y
 * \param[out] F ffi_vec representation of the support of <b>x</b> and <b>y</b>
 * \param[in] sk String containing the secret key
 */
void ouroborosr_secret_key_from_string(ffi_vec& x, ffi_vec& y, ffi_vec& F, const unsigned char* sk) {
  unsigned char sk_seed[SEEDEXPANDER_SEED_BYTES];
  memcpy(sk_seed, sk, SEEDEXPANDER_SEED_BYTES);
  AES_XOF_struct* sk_seedexpander = (AES_XOF_struct*) malloc(sizeof(AES_XOF_struct));
  seedexpander_init(sk_seedexpander, sk_seed, sk_seed + 32, SEEDEXPANDER_MAX_LENGTH);

  ffi_vec_set_random_full_rank_with_one_using_seedexpander(F, PARAM_W, sk_seedexpander);
  ffi_vec_set_random_from_support_using_seedexpander(x, PARAM_N, F, PARAM_W, sk_seedexpander);
  ffi_vec_set_random_from_support_using_seedexpander(y, PARAM_N, F, PARAM_W, sk_seedexpander);

  free(sk_seedexpander);
}



/** 
 * \fn void ouroborosr_public_key_to_string(unsigned char* pk, const ffi_vec& s, const unsigned char* seed)
 * \brief This function parses a public key into a string
 *
 * The public key is composed of the syndrom <b>s</b> as well as the seed used to generate vector <b>h</b>.
 *
 * \param[out] pk String containing the public key
 * \param[in] s ffi_vec representation of vector <b>s</b>
 * \param[in] seed Seed used to generate the public key
 */
void ouroborosr_public_key_to_string(unsigned char* pk, const ffi_vec& s, const unsigned char* seed) {
  ffi_vec_to_string(pk, s, PARAM_N);
  memcpy(pk + FFI_VEC_N_BYTES, seed, SEEDEXPANDER_SEED_BYTES);
}



/** 
 * \fn void ouroborosr_public_key_from_string(ffi_vec& h, ffi_vec& s, const unsigned char* pk)
 * \brief This function parses a public key from a string
 *
 * The public key is composed of the syndrom <b>s</b> as well as the seed used to generate vector <b>h</b>.
 *
 * \param[out] h representation of vector <b>h</b>
 * \param[out] s representation of vector <b>s</b>
 * \param[in] pk String containing the public key
 */
void ouroborosr_public_key_from_string(ffi_vec& h, ffi_vec& s, const unsigned char* pk) {
  ffi_vec_from_string(s, PARAM_N, pk);

  unsigned char pk_seed[SEEDEXPANDER_SEED_BYTES];
  memcpy(pk_seed, pk + FFI_VEC_N_BYTES, SEEDEXPANDER_SEED_BYTES);
  AES_XOF_struct* pk_seedexpander = (AES_XOF_struct*) malloc(sizeof(AES_XOF_struct));
  seedexpander_init(pk_seedexpander, pk_seed, pk_seed + 32, SEEDEXPANDER_MAX_LENGTH);

  ffi_vec_set_random_using_seedexpander(h, PARAM_N, pk_seedexpander); 

  free(pk_seedexpander);
}



/** 
 * \fn void ouroborosr_ciphertext_to_string(unsigned char* ct, const ffi_vec& sr, const ffi_vec& se) {
 * \brief This function parses a ciphertext into a string
 *
 * The ciphertext is composed of vectors <b>sr</b> and <b>se</b>.
 *
 * \param[out] ct String containing the ciphertext
 * \param[in] sr ffi_vec representation of vector <b>sr</b>
 * \param[in] se ffi_vec representation of vector <b>se</b>
 */
void ouroborosr_ciphertext_to_string(unsigned char* ct, const ffi_vec& sr, const ffi_vec& se) {
  ffi_vec_to_string(ct, sr, PARAM_N);
  ffi_vec_to_string(ct + FFI_VEC_N_BYTES, se, PARAM_N);
}



/** 
 * \fn void ouroborosr_ciphertext_from_string(ffi_vec& sr, ffi_vec& se, const unsigned char* ct)
 * \brief This function parses a ciphertext from a string
 *
 * The ciphertext is composed of vectors <b>sr</b> and <b>se</b>.
 *
 * \param[out] sr ffi_vec representation of vector <b>sr</b>
 * \param[out] se ffi_vec representation of vector <b>se</b>
 * \param[in] ct String containing the ciphertext
 */
void ouroborosr_ciphertext_from_string(ffi_vec& sr, ffi_vec& se, const unsigned char* ct) {
  ffi_vec_from_string(sr, PARAM_N, ct);
  ffi_vec_from_string(se, PARAM_N, ct + FFI_VEC_N_BYTES);
}

