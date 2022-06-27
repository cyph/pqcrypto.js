/** 
 * \file parsing.c
 * \brief Implementation of parsing.h
 */

#include "string.h"
#include "rbc_vspace.h"
#include "rbc_qre.h"
#include "parameters.h"
#include "parsing.h"
#include "seedexpander.h"


/** 
 * \fn void rqc_secret_key_to_string(uint8_t* sk, const uint8_t* seed, const uint8_t* pk)
 * \brief This function parses a secret key into a string
 *
 * The secret key is composed of the seed used to generate vectors <b>x</b> and <b>y</b>.
 * As a technicality, the public key is appended to the secret key in order to respect the NIST API.
 *
 * \param[out] sk String containing the secret key
 * \param[in] seed Seed used to generate the vectors x and y
 * \param[in] pk String containing the public key
 */
void rqc_secret_key_to_string(uint8_t* sk, const uint8_t* seed, const uint8_t* pk) {
  memcpy(sk, seed, SEEDEXPANDER_SEED_BYTES);
  memcpy(sk + SEEDEXPANDER_SEED_BYTES, pk, RQC_PUBLIC_KEY_BYTES);
}



/** 
* \fn void rqc_secret_key_from_string(rbc_qre x, rbc_qre y, uint8_t* pk, const uint8_t* sk)
* \brief This function parses a secret key from a string
*
* The secret key is composed of the seed used to generate vectors <b>x</b> and <b>y</b>.
* As a technicality, the public key is appended to the secret key in order to respect the NIST API.
*
* \param[out] x rbc_qre representation of vector x
* \param[out] y rbc_qre representation of vector y
* \param[out] pk String containing the public key
* \param[in] sk String containing the secret key
*/
void rqc_secret_key_from_string(rbc_qre x, rbc_qre y, uint8_t* pk, const uint8_t* sk) {
  uint8_t sk_seed[SEEDEXPANDER_SEED_BYTES] = {0};
  AES_XOF_struct sk_seedexpander;

  rbc_vspace support_w;
  rbc_vspace_init(&support_w, RQC_PARAM_W);

  memcpy(sk_seed, sk, SEEDEXPANDER_SEED_BYTES);
  seedexpander_init(&sk_seedexpander, sk_seed, sk_seed + 32, SEEDEXPANDER_MAX_LENGTH);

  rbc_vspace_set_random_full_rank_with_one(&sk_seedexpander, support_w, RQC_PARAM_W);
  rbc_qre_set_random_pair_from_support(&sk_seedexpander, x, y, support_w, RQC_PARAM_W);

  memcpy(pk, sk + SEEDEXPANDER_SEED_BYTES, RQC_PUBLIC_KEY_BYTES);

  rbc_vspace_clear(support_w);
}



/** 
 * \fn void rqc_public_key_to_string(uint8_t* pk, const rbc_qre s, const uint8_t* seed)
 * \brief This function parses a public key into a string
 *
 * The public key is composed of the syndrom <b>s</b> as well as the seed used to generate vectors <b>g</b> and <b>h</b>.
 *
 * \param[out] pk String containing the public key
 * \param[in] s rbc_qre representation of vector s
 * \param[in] seed Seed used to generate the public key
 */
void rqc_public_key_to_string(uint8_t* pk, const rbc_qre s, const uint8_t* seed) {
  rbc_qre_to_string(pk, s);
  memcpy(pk + RQC_VEC_N_BYTES, seed, SEEDEXPANDER_SEED_BYTES);
}



/** 
 * \fn void rqc_public_key_from_string(rbc_qre g, rbc_qre h, rbc_qre s, const uint8_t* pk)
 * \brief This function parses a public key from a string
 *
 * The public key is composed of the syndrom <b>s</b> as well as the seed used to generate vectors <b>g</b> and <b>h</b>.
 *
 * \param[out] g rbc_qre representation of vector g
 * \param[out] h rbc_qre representation of vector h
 * \param[out] s rbc_qre representation of vector s
 * \param[in] pk String containing the public key
 */
void rqc_public_key_from_string(rbc_qre g, rbc_qre h, rbc_qre s, const uint8_t* pk) {
  uint8_t pk_seed[SEEDEXPANDER_SEED_BYTES] = {0};
  AES_XOF_struct pk_seedexpander;

  rbc_qre_from_string(s, pk);

  memcpy(pk_seed, pk + RQC_VEC_N_BYTES, SEEDEXPANDER_SEED_BYTES);
  seedexpander_init(&pk_seedexpander, pk_seed, pk_seed + 32, SEEDEXPANDER_MAX_LENGTH);

  rbc_qre_set_random_full_rank(&pk_seedexpander, g);
  rbc_qre_set_random(&pk_seedexpander, h); 
}



/** 
 * \fn void rqc_kem_ciphertext_to_string(uint8_t* ct, const rbc_qre u, const rbc_qre v, const uint8_t* d) {
 * \brief This function parses a ciphertext into a string
 *
 * The ciphertext is composed of vectors <b>u</b>, <b>v</b> and the hash <b>d</b>.
 *
 * \param[out] ct String containing the ciphertext
 * \param[in] u rbc_qre representation of vector u
 * \param[in] v rbc_qre representation of vector v
 * \param[in] d String containing the hash d
 */
void rqc_kem_ciphertext_to_string(uint8_t* ct, const rbc_qre u, const rbc_qre v, const uint8_t* d) {
  rbc_qre_to_string(ct, u);
  rbc_qre_to_string(ct + RQC_VEC_N_BYTES, v);
  memcpy(ct + 2 * RQC_VEC_N_BYTES, d, SHA512_BYTES);
}



/** 
 * \fn void rqc_kem_ciphertext_from_string(rbc_qre u, rbc_qre v, uint8_t* d, const uint8_t* ct)
 * \brief This function parses a ciphertext from a string
 *
 * The ciphertext is composed of vectors <b>u</b>, <b>v</b> and the hash <b>d</b>.
 *
 * \param[out] u rbc_qre representation of vector u
 * \param[out] v rbc_qre representation of vector v
 * \param[out] d String containing the hash d
 * \param[in] ct String containing the ciphertext
 */
void rqc_kem_ciphertext_from_string(rbc_qre u, rbc_qre v, uint8_t* d, const uint8_t* ct) {
  rbc_qre_from_string(u, ct);
  rbc_qre_from_string(v, ct + RQC_VEC_N_BYTES);
  memcpy(d, ct + 2 * RQC_VEC_N_BYTES, SHA512_BYTES);
}

