/** 
 * \file kem.cpp
 * \brief Implementation of api.h
 */

#include "string.h"
#include "hash.h"
#include "rng.h"
#include "crypto_kem.h"
#include "parameters.h"
#include "rsr_algorithm.h"
#include "parsing.h"
#include "ffi_field.h"
#include "ffi_vec.h"


/** 
 * \fn int crypto_kem_keypair(unsigned char* pk, unsigned char* sk)
 * \brief Keygen of the Ouroboros-R scheme
 *
 * The public key is composed of the syndrom <b>s</b> as well as the seed used to generate the vector <b>h</b>.
 * The secret key is composed of the seed used to generate the vectors <b>x</b> and <b>y</b>.
 *
 * \param[out] pk String containing the public key
 * \param[out] sk String containing the secret key
 * \return 0 if keygen is sucessfull
 */
int crypto_kem_keypair(unsigned char* pk, unsigned char* sk) {
  #ifdef VERBOSE
    printf("\n\n\n### KEYGEN ###");
  #endif

  // Create seed expanders for public key and secret key
  unsigned char sk_seed[SEEDEXPANDER_SEED_BYTES];
  randombytes(sk_seed, SEEDEXPANDER_SEED_BYTES);
  AES_XOF_struct* sk_seedexpander = (AES_XOF_struct*) malloc(sizeof(AES_XOF_struct));
  seedexpander_init(sk_seedexpander, sk_seed, sk_seed + 32, SEEDEXPANDER_MAX_LENGTH);

  unsigned char pk_seed[SEEDEXPANDER_SEED_BYTES];
  randombytes(pk_seed, SEEDEXPANDER_SEED_BYTES);
  AES_XOF_struct* pk_seedexpander = (AES_XOF_struct*) malloc(sizeof(AES_XOF_struct));
  seedexpander_init(pk_seedexpander, pk_seed, pk_seed + 32, SEEDEXPANDER_MAX_LENGTH);

  ffi_field_init();

  // Compute secret key
  ffi_vec F, x, y;
  ffi_vec_set_random_full_rank_with_one_using_seedexpander(F, PARAM_W, sk_seedexpander);
  ffi_vec_set_random_from_support_using_seedexpander(x, PARAM_N, F, PARAM_W, sk_seedexpander);
  ffi_vec_set_random_from_support_using_seedexpander(y, PARAM_N, F, PARAM_W, sk_seedexpander);

  // Compute public key
  ffi_vec h, s;
  ffi_vec_set_random_using_seedexpander(h, PARAM_N, pk_seedexpander); 
  
  ffi_vec_mul(s, h, y, PARAM_N);
  ffi_vec_add(s, s, x, PARAM_N);

  // Parse keys to string
  ouroborosr_public_key_to_string(pk, s, pk_seed);
  ouroborosr_secret_key_to_string(sk, sk_seed);

  #ifdef VERBOSE
    printf("\n\nsk_seed: "); for(int i = 0 ; i < SEEDEXPANDER_SEED_BYTES ; ++i) printf("%02x", sk_seed[i]);
    printf("\n\nF: "); ffi_vec_print(F, PARAM_W);
    printf("\n\nx: "); ffi_vec_print(x, PARAM_N);
    printf("\n\ny: "); ffi_vec_print(y, PARAM_N);

    printf("\n\npk_seed: "); for(int i = 0 ; i < SEEDEXPANDER_SEED_BYTES ; ++i) printf("%02x", pk_seed[i]);
    printf("\n\nh: "); ffi_vec_print(h, PARAM_N);
    printf("\n\ns: "); ffi_vec_print(s, PARAM_N);

    printf("\n\nsk: "); for(int i = 0 ; i < SECRET_KEY_BYTES ; ++i) printf("%02x", sk[i]);
    printf("\n\npk: "); for(int i = 0 ; i < PUBLIC_KEY_BYTES ; ++i) printf("%02x", pk[i]);
  #endif

  free(pk_seedexpander);
  free(sk_seedexpander);

  return 0;
}



/** 
 * \fn int crypto_kem_enc(unsigned char* ct, unsigned char* ss, const unsigned char* pk)
 * \brief Encapsulation of the Ouroboros-R scheme
 *
 * \param[out] ct String containing the ciphertext
 * \param[out] ss String containing the shared secret
 * \param[in] pk String containing the public key
 * \return 0 if encapsulation is sucessfull
 */
int crypto_kem_enc(unsigned char* ct, unsigned char* ss, const unsigned char* pk) {
  #ifdef VERBOSE
    printf("\n\n\n\n### ENCAPS ###");
  #endif

  ffi_field_init();

  // Retrieve h and s from public key
  ffi_vec h, s;
  ouroborosr_public_key_from_string(h, s, pk);

  // Generate r1, r2 and er
  ffi_vec E, r1, r2, er;
  ffi_vec_set_random_full_rank_using_rng(E, PARAM_W_R);
  ffi_vec_set_random_from_support_using_rng(r1, PARAM_N, E, PARAM_W_R);
  ffi_vec_set_random_from_support_using_rng(r2, PARAM_N, E, PARAM_W_R);
  ffi_vec_set_random_from_support_using_rng(er, PARAM_N, E, PARAM_W_R);

  // Compute sr = r1 + h.r2
  ffi_vec sr;
  ffi_vec_mul(sr, h, r2, PARAM_N);
  ffi_vec_add(sr, sr, r1, PARAM_N);

  // Compute se = s.r2 + er
  ffi_vec se;
  ffi_vec_mul(se, s, r2, PARAM_N);
  ffi_vec_add(se, se, er, PARAM_N);

  // Compute ciphertext
  ouroborosr_ciphertext_to_string(ct, sr, se);

  // Compute shared secret
  ffi_vec_echelonize(E, PARAM_W_R);
  
  unsigned char support[FFI_VEC_R_BYTES];
  ffi_vec_to_string(support, E, PARAM_W_R);
  sha512(ss, support, FFI_VEC_R_BYTES);

  #ifdef VERBOSE
    printf("\n\nh: "); ffi_vec_print(h, PARAM_N);
    printf("\n\ns: "); ffi_vec_print(s, PARAM_N);
    printf("\n\nE: "); ffi_vec_print(E, PARAM_W_R);
    printf("\n\nr1: "); ffi_vec_print(r1, PARAM_N);
    printf("\n\nr2: "); ffi_vec_print(r2, PARAM_N);
    printf("\n\ner: "); ffi_vec_print(er, PARAM_N);
    printf("\n\nsr: "); ffi_vec_print(sr, PARAM_N);
    printf("\n\nse: "); ffi_vec_print(se, PARAM_N);
    printf("\n\nct: "); for(int i = 0 ; i < CIPHERTEXT_BYTES ; ++i) printf("%02x", ct[i]);
    printf("\n\nss: "); for(int i = 0 ; i < SHARED_SECRET_BYTES ; ++i) printf("%02x", ss[i]);
  #endif

  return 0;
}



/** 
 * \fn int crypto_kem_dec(unsigned char* ss, const unsigned char* ct, const unsigned char* sk)
 * \brief Decapsulation of the Ouroboros-R scheme
 *
 * \param[out] ss String containing the shared secret
 * \param[in] ct String containing the ciphertext
 * \param[in] sk String containing the secret key
 * \return 0 if decapsulation is successfull, -1 otherwise
 */
int crypto_kem_dec(unsigned char* ss, const unsigned char* ct, const unsigned char* sk) {
  #ifdef VERBOSE
    printf("\n\n\n\n### DECAPS ###");
  #endif

  ffi_field_init();

  // Retrieve sr and se from ciphertext
  ffi_vec sr, se;
  ouroborosr_ciphertext_from_string(sr, se, ct);

  // Retrieve x, y and their support from secret key
  ffi_vec F, x, y;
  ouroborosr_secret_key_from_string(x, y, F, sk);

  // Compute ec = se - y.sr
  ffi_vec ec;
  ffi_vec_mul(ec, y, sr, PARAM_N);
  // ffi_vec_neg(ec, ec, PARAM_N);
  ffi_vec_add(ec, ec, se, PARAM_N);

  #ifdef VERBOSE
    printf("\n\nsk: "); for(int i = 0 ; i < SECRET_KEY_BYTES ; ++i) printf("%02x", sk[i]);
    printf("\n\nciphertext: "); for(int i = 0 ; i < CIPHERTEXT_BYTES ; ++i) printf("%02x", ct[i]);
    printf("\n\nsr: "); ffi_vec_print(sr, PARAM_N);
    printf("\n\nse: "); ffi_vec_print(se, PARAM_N);
    printf("\n\nF: "); ffi_vec_print(F, PARAM_W);
    printf("\n\nx: "); ffi_vec_print(x, PARAM_N);
    printf("\n\ny: "); ffi_vec_print(y, PARAM_N);
    printf("\n\nec: "); ffi_vec_print(ec, PARAM_N);
  #endif

  // Retrieve E by solving rank support recovery problem
  ffi_vec E;
  unsigned int E_dim;
  rank_support_recover(E, E_dim, PARAM_W_R, F, PARAM_W, ec, PARAM_N);

  // Compute shared secret 
  if(E_dim != 0) {
    unsigned char support[FFI_VEC_R_BYTES];
    ffi_vec_to_string(support, E, PARAM_W_R);
    sha512(ss, support, FFI_VEC_R_BYTES);

    #ifdef VERBOSE
      printf("\n\nE: "); ffi_vec_print(E, PARAM_W_R);
      printf("\n\nss: "); for(int i = 0 ; i < SHARED_SECRET_BYTES ; ++i) printf("%02x", ss[i]);
    #endif

    return 0;
  } else {
    memset(ss, 0, SHARED_SECRET_BYTES);

    #ifdef VERBOSE
      printf("\n\nss: "); for(int i = 0 ; i < SHARED_SECRET_BYTES ; ++i) printf("%02x", ss[i]);
    #endif

    return -1;
  }
}

