/** 
 * \file kem.c
 * \brief Implementation of api.h
 */

#include "crypto_kem.h"
#include "string.h"
#include "hash.h"
#include "rqc.h"
#include "api.h"
#include "rbc_qre.h"
#include "parameters.h"
#include "parsing.h"

/** 
 * \fn int crypto_kem_keypair(uint8_t* pk, uint8_t* sk)
 * \brief Keygen of the RQC_KEM IND-CCA2 scheme
 *
 * The public key is composed of the syndrom <b>s</b> as well as the seed used to generate vectors <b>g</b> and <b>h</b>.
 *
 * The secret key is composed of the seed used to generate the vectors <b>x</b> and <b>y</b>.
 * As a technicality, the public key is appended to the secret key in order to respect the NIST API.
 *
 * \param[out] pk String containing the public key
 * \param[out] sk String containing the secret key
 * \return 0 if keygen is sucessfull
 */
int crypto_kem_keypair(uint8_t* pk, uint8_t* sk) {
  #ifdef VERBOSE
    printf("\n\n\n### KEYGEN ###");
  #endif

  rqc_pke_keygen(pk, sk);

  return 0;
}



/** 
 * \fn int crypto_kem_enc(uint8_t* ct, uint8_t* ss, const uint8_t* pk)
 * \brief Encapsulation of the RQC_KEM IND-CCA2 scheme
 *
 * \param[out] ct String containing the ciphertext
 * \param[out] ss String containing the shared secret
 * \param[in] pk String containing the public key
 * \return 0 if encapsulation is sucessfull
 */
int crypto_kem_enc(uint8_t* ct, uint8_t* ss, const uint8_t* pk) {
  #ifdef VERBOSE
    printf("\n\n\n\n### ENCAPS ###");
  #endif

  uint8_t theta[SHA512_BYTES] = {0};
  uint8_t mc[RQC_VEC_K_BYTES + 2 * RQC_VEC_N_BYTES] = {0};
  uint8_t d[SHA512_BYTES] = {0};
  rbc_vec m;
  rbc_qre u, v;

  rbc_field_init();
  rbc_qre_init_modulus(RQC_PARAM_N);
  rbc_vec_init(&m, RQC_PARAM_K);
  rbc_qre_init(&u);
  rbc_qre_init(&v);

  // Computing m
  rbc_vec_set_random2(m, RQC_PARAM_K);

  // Computing theta
  rbc_vec_to_string(mc, m, RQC_PARAM_K);
  sha3_512(theta, mc, RQC_VEC_K_BYTES);

  // Encrypting m
  rqc_pke_encrypt(u, v, m, theta, pk);

  // Computing d
  sha512(d, mc, RQC_VEC_K_BYTES);

  // Computing ciphertext
  rqc_kem_ciphertext_to_string(ct, u, v, d);
  
  // Computing shared secret
  rbc_qre_to_string(mc + RQC_VEC_K_BYTES, u);
  rbc_qre_to_string(mc + RQC_VEC_K_BYTES + RQC_VEC_N_BYTES, v);
  sha512(ss, mc, RQC_VEC_K_BYTES + 2 * RQC_VEC_N_BYTES);

  #ifdef VERBOSE
    printf("\n\nm: "); rbc_vec_print(m, RQC_PARAM_K);
    printf("\n\ntheta: "); for(int i = 0 ; i < SHA512_BYTES ; ++i) printf("%02x", theta[i]);
    printf("\n\nciphertext: "); for(int i = 0 ; i < RQC_CIPHERTEXT_BYTES ; ++i) printf("%02x", ct[i]);
    printf("\n\nd: "); for(int i = 0 ; i < SHA512_BYTES ; ++i) printf("%02x", d[i]);
    printf("\n\nsecret 1: "); for(int i = 0 ; i < RQC_SHARED_SECRET_BYTES ; ++i) printf("%02x", ss[i]);
  #endif
  
  rbc_vec_clear(m);
  rbc_qre_clear(u);
  rbc_qre_clear(v);
  rbc_qre_clear_modulus();

  return 0;
}



/** 
 * \fn int crypto_kem_dec(uint8_t* ss, const uint8_t* ct, const uint8_t* sk)
 * \brief Decapsulation of the RQC_KEM IND-CCA2 scheme
 *
 * \param[out] ss String containing the shared secret
 * \param[in] ct String containing the ciphertext
 * \param[in] sk String containing the secret key
 * \return 0 if decapsulation is successfull, -1 otherwise
 */
int crypto_kem_dec(uint8_t* ss, const uint8_t* ct, const uint8_t* sk) {
  #ifdef VERBOSE
    printf("\n\n\n\n### DECAPS ###");
  #endif

  int8_t result = -1;
  uint8_t pk[RQC_PUBLIC_KEY_BYTES] = {0};
  uint8_t theta[SHA512_BYTES] = {0};
  uint8_t d[SHA512_BYTES] = {0};
  uint8_t d2[SHA512_BYTES] = {0};
  uint8_t mc[RQC_VEC_K_BYTES + 2 * RQC_VEC_N_BYTES] = {0};
  rbc_vec m;
  rbc_qre u, v, u2, v2;

  rbc_field_init();
  rbc_qre_init_modulus(RQC_PARAM_N);

  rbc_vec_init(&m, RQC_PARAM_K);
  rbc_qre_init(&u);
  rbc_qre_init(&v);
  rbc_qre_init(&u2);
  rbc_qre_init(&v2);

  // Retrieving u, v and d from ciphertext
  rqc_kem_ciphertext_from_string(u, v, d, ct);

  // Retrieving pk from sk
  memcpy(pk, sk + SEEDEXPANDER_SEED_BYTES, RQC_PUBLIC_KEY_BYTES);

  // Decrypting
  rqc_pke_decrypt(m, u, v, sk);

  // Computing theta
  rbc_vec_to_string(mc, m, RQC_PARAM_K);
  sha3_512(theta, mc, RQC_VEC_K_BYTES);

  // Encrypting m'
  rqc_pke_encrypt(u2, v2, m, theta, pk);

  // Computing d'
  sha512(d2, mc, RQC_VEC_K_BYTES);

  // Computing shared secret
  rbc_qre_to_string(mc + RQC_VEC_K_BYTES, u);
  rbc_qre_to_string(mc + RQC_VEC_K_BYTES + RQC_VEC_N_BYTES, v);
  sha512(ss, mc, RQC_VEC_K_BYTES + 2 * RQC_VEC_N_BYTES);

  // Abort if c != c' or d != d'
  result = (rbc_qre_is_equal_to(u, u2) == 1 && rbc_qre_is_equal_to(v, v2) == 1 && memcmp(d, d2, SHA512_BYTES) == 0);
  for(size_t i = 0 ; i < RQC_SHARED_SECRET_BYTES ; i++) ss[i] = result * ss[i];
  result--;

  #ifdef VERBOSE
    printf("\n\npk: "); for(int i = 0 ; i < RQC_PUBLIC_KEY_BYTES ; ++i) printf("%02x", pk[i]);
    printf("\n\nsk: "); for(int i = 0 ; i < RQC_SECRET_KEY_BYTES ; ++i) printf("%02x", sk[i]);
    printf("\n\nciphertext: "); for(int i = 0 ; i < RQC_CIPHERTEXT_BYTES ; ++i) printf("%02x", ct[i]);
    printf("\n\nm: "); rbc_vec_print(m, RQC_PARAM_K);
    printf("\n\ntheta: "); for(int i = 0 ; i < SHA512_BYTES ; ++i) printf("%02x", theta[i]);
    printf("\n\n\n# Checking Ciphertext- Begin #");
    printf("\n\nu2: "); rbc_qre_print(u2);
    printf("\n\nv2: "); rbc_qre_print(v2);
    printf("\n\nd2: "); for(int i = 0 ; i < SHA512_BYTES ; ++i) printf("%02x", d2[i]);
    printf("\n\n# Checking Ciphertext - End #\n");
  #endif

  rbc_vec_clear(m);
  rbc_qre_clear(u);
  rbc_qre_clear(v);
  rbc_qre_clear(u2);
  rbc_qre_clear(v2);
  rbc_qre_clear_modulus();

  return result;
}

