/**
 * \file parsing.cpp
 * \brief Functions to parse secret key, public key and ciphertext of the HQC scheme
 */

#include "parsing.h"

/**
 *\fn void hqc_secret_key_to_string(unsigned char* sk, const unsigned char* sk_seed, const unsigned char* pk)
 *\brief Parse a secret key into a string
 *
 * The secret key is composed of the seed used to generate vectors <b>x</b> and <b>y</b>.
 * As technicality, the public key is appended to the secret key in order to respect NIST API.
 * 
 * \param[out] sk String containing the secret key
 * \param[in] sk_seed Seed used to generate the secret key
 * \param[in] pk String containing the public key
 */
void hqc_secret_key_to_string(unsigned char* sk, const unsigned char* sk_seed, const unsigned char* pk) {
  memcpy(sk, sk_seed, SEED_BYTES);
  memcpy(sk + SEED_BYTES, pk, PUBLIC_KEY_BYTES);
}

/**
 *\fn void hqc_secret_key_from_string(uint8_t* x, uint8_t* y, unsigned char* pk, const unsigned char* sk)
 *\brief Parse a secret key from a string
 *
 * The secret key is composed of the seed used to generate vectors <b>x</b> and <b>y</b>.
 * As technicality, the public key is appended to the secret key in order to respect NIST API.
 * 
 * \param[out] x uint8_t representation of vector x 
 * \param[out] y uint8_t representation of vector y
 * \param[out] pk String containing the public key
 * \param[in] sk String containing the secret key
 */
void hqc_secret_key_from_string(uint8_t* x, uint8_t* y, unsigned char* pk, const unsigned char* sk) {
  unsigned char sk_seed[SEED_BYTES];
  memcpy(sk_seed, sk, SEED_BYTES);
  AES_XOF_struct* sk_seedexpander = (AES_XOF_struct*) malloc(sizeof(AES_XOF_struct));
  seedexpander_init(sk_seedexpander, sk_seed, sk_seed + 32, SEEDEXPANDER_MAX_LENGTH);

  vect_fixed_weight(x, PARAM_OMEGA, sk_seedexpander);
  vect_fixed_weight(y, PARAM_OMEGA, sk_seedexpander);
  memcpy(pk, sk + SEED_BYTES, PUBLIC_KEY_BYTES);

  free(sk_seedexpander);
}

/**
 *\fn void hqc_public_key_to_string(unsigned char* pk, const unsigned char* pk_seed, uint8_t* s)
 *\brief Parse a public key into a string
 * 
 * The public key is composed of the syndrome <b>s</b> as well as the seed used to generate the vector <b>h</b>
 *
 * \param[out] pk String containing the public key 
 * \param[in] pk_seed Seed used to generate the public key
 * \param[in] s uint8_t representation of vector s
 */
void hqc_public_key_to_string(unsigned char* pk, const unsigned char* pk_seed, uint8_t* s) {
  memcpy(pk, pk_seed, SEED_BYTES);
  memcpy(pk + SEED_BYTES, s, VEC_N_SIZE_BYTES);
}


/**
 *\fn void hqc_public_key_from_string(uint8_t* h, uint8_t* s, const unsigned char* pk)
 *\brief Parse a public key from a string
 * 
 * The public key is composed of the syndrome <b>s</b> as well as the seed used to generate the vector <b>h</b>
 *
 * \param[out] h uint8_t representation of vector h
 * \param[out] s uint8_t representation of vector s
 * \param[in] pk String containing the public key
 */
void hqc_public_key_from_string(uint8_t* h, uint8_t* s, const unsigned char* pk) {
  unsigned char pk_seed[SEED_BYTES];
  memcpy(pk_seed, pk, SEED_BYTES);
  AES_XOF_struct* pk_seedexpander = (AES_XOF_struct*) malloc(sizeof(AES_XOF_struct));
  seedexpander_init(pk_seedexpander, pk_seed, pk_seed + 32, SEEDEXPANDER_MAX_LENGTH);
  vect_set_random(h, pk_seedexpander);

  memcpy(s, pk + SEED_BYTES, VEC_N_SIZE_BYTES);

  free(pk_seedexpander);
}

/**
 *\fn void hqc_ciphertext_to_string(unsigned char* ct, uint8_t* u, uint8_t* v, const unsigned char* d)
 *\brief Parse a ciphertext into a string 
 * 
 * The ciphertext is composed of vectors <b>u</b>, <b>v</b> and hash <b>d</b>.
 *
 * \param[out] ct String containing the ciphertext
 * \param[in] u uint8_t representation of vector u
 * \param[in] v uint8_t representation of vector v
 * \param[in] d String containing the hash d
 */
void hqc_ciphertext_to_string(unsigned char* ct, uint8_t* u, uint8_t* v, const unsigned char* d) {
  memcpy(ct, u, VEC_N_SIZE_BYTES);
  memcpy(ct + VEC_N_SIZE_BYTES, v, VEC_N1N2_SIZE_BYTES);
  memcpy(ct + VEC_N_SIZE_BYTES + VEC_N1N2_SIZE_BYTES, d, SHA512_BYTES);
}

/**
 *\fn void hqc_ciphertext_from_string(uint8_t* u, uint8_t* v, unsigned char* d, const unsigned char* ct)
 *\brief Parse a ciphertext from a string 
 * 
 * The ciphertext is composed of vectors <b>u</b>, <b>v</b> and hash <b>d</b>.
 *
 * \param[out] u uint8_t representation of vector u
 * \param[out] v uint8_t representation of vector v
 * \param[out] d String containing the hash d
 * \param[in] ct String containing the ciphertext
 */
void hqc_ciphertext_from_string(uint8_t* u, uint8_t* v, unsigned char* d, const unsigned char* ct) {
  memcpy(u, ct, VEC_N_SIZE_BYTES);
  memcpy(v, ct + VEC_N_SIZE_BYTES, VEC_N1N2_SIZE_BYTES);
  memcpy(d, ct + VEC_N_SIZE_BYTES + VEC_N1N2_SIZE_BYTES, SHA512_BYTES);
}