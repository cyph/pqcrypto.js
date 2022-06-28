/** 
 * \file rqc.c
 * \brief Implementation of rqc.h
 */

#include "ffi_field.h"
#include "ffi_vec.h"
#include "gabidulin.h"
#include "parameters.h"
#include "parsing.h"
#include "rqc.h"


/** 
 * \fn void rqc_pke_keygen(unsigned char* pk, unsigned char* sk)
 * \brief Keygen of the RQC_PKE IND-CPA scheme
 *
 * The public key is composed of the syndrom <b>s</b> as well as the <b>seed</b> used to generate vectors <b>g</b> and <b>h</b>.
 *
 * The secret key is composed of the seed used to generate vectors <b>x</b> and <b>y</b>.
 * As a technicality, the public key is appended to the secret key in order to respect the NIST API.
 *
 * \param[out] pk String containing the public key
 * \param[out] sk String containing the secret key
 */
void rqc_pke_keygen(unsigned char* pk, unsigned char* sk) {

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
  ffi_vec x, y, support_w;
  ffi_vec_set_random_full_rank_with_one(support_w, PARAM_W, sk_seedexpander);
  ffi_vec_set_random_from_support(x, PARAM_N, support_w, PARAM_W, sk_seedexpander);
  ffi_vec_set_random_from_support(y, PARAM_N, support_w, PARAM_W, sk_seedexpander);

  // Compute public key
  ffi_vec g, h, s;
  ffi_vec_set_random_full_rank(g, PARAM_N, pk_seedexpander);
  ffi_vec_set_random(h, PARAM_N, pk_seedexpander); 
  
  ffi_vec_mul(s, h, y, PARAM_N);
  ffi_vec_add(s, s, x, PARAM_N);

  // Parse keys to string
  rqc_public_key_to_string(pk, s, pk_seed);
  rqc_secret_key_to_string(sk, sk_seed, pk);

  #ifdef VERBOSE
    printf("\n\nsk_seed: "); for(int i = 0 ; i < SEEDEXPANDER_SEED_BYTES ; ++i) printf("%02x", sk_seed[i]);
    printf("\n\nsupport_w: "); ffi_vec_print(support_w, PARAM_W);
    printf("\nx: "); ffi_vec_print(x, PARAM_N);
    printf("\ny: "); ffi_vec_print(y, PARAM_N);

    printf("\npk_seed: "); for(int i = 0 ; i < SEEDEXPANDER_SEED_BYTES ; ++i) printf("%02x", pk_seed[i]);
    printf("\n\ng: "); ffi_vec_print(g, PARAM_N);
    printf("\nh: "); ffi_vec_print(h, PARAM_N);
    printf("\ns: "); ffi_vec_print(s, PARAM_N);

    printf("\nsk: "); for(int i = 0 ; i < SECRET_KEY_BYTES ; ++i) printf("%02x", sk[i]);
    printf("\n\npk: "); for(int i = 0 ; i < PUBLIC_KEY_BYTES ; ++i) printf("%02x", pk[i]);
  #endif

  free(pk_seedexpander);
  free(sk_seedexpander);
}



/** 
 * \fn void rqc_pke_encrypt(ffi_vec& u, ffi_vec& v, const ffi_vec& m, unsigned char* theta, const unsigned char* pk)
 * \brief Encryption of the RQC_PKE IND-CPA scheme
 *
 * The ciphertext is composed of the vectors <b>u</b> and <b>v</b>.
 *
 * \param[out] u Vector u (first part of the ciphertext)
 * \param[out] v Vector v (second part of the ciphertext)
 * \param[in] m Vector representing the message to encrypt
 * \param[in] theta Seed used to derive randomness required for encryption
 * \param[in] pk String containing the public key
 */
void rqc_pke_encrypt(ffi_vec& u, ffi_vec& v, const ffi_vec& m, unsigned char* theta, const unsigned char* pk) {

  // Create seed_expander from theta
  AES_XOF_struct* seedexpander = (AES_XOF_struct*) malloc(sizeof(AES_XOF_struct));
  seedexpander_init(seedexpander, theta, theta + 32, SEEDEXPANDER_MAX_LENGTH);

  ffi_field_init();

  // Retrieve g, h and s from public key
  ffi_vec g, h, s;
  rqc_public_key_from_string(g, h, s, pk);

  // Generate r1, r2 and e
  ffi_vec r1, r2, e, support_r;
  ffi_vec_set_random_full_rank(support_r, PARAM_W_R, seedexpander);
  ffi_vec_set_random_from_support(r1, PARAM_N, support_r, PARAM_W_R, seedexpander);
  ffi_vec_set_random_from_support(r2, PARAM_N, support_r, PARAM_W_R, seedexpander);
  ffi_vec_set_random_from_support(e, PARAM_N, support_r, PARAM_W_R, seedexpander);

  // Compute u = r1 + h.r2
  ffi_vec_mul(u, h, r2, PARAM_N);
  ffi_vec_add(u, u, r1, PARAM_N);

  // Compute v = m.G by encoding the message
  gabidulin_code code = gabidulin_code_init(g, PARAM_K, PARAM_N);
  gabidulin_code_encode(v, code, m);

  // Compute v = m.G + s.r2 + e
  ffi_vec tmp;
  ffi_vec_mul(tmp, s, r2, PARAM_N);
  ffi_vec_add(tmp, tmp, e, PARAM_N);
  ffi_vec_add(v, v, tmp, PARAM_N);

  #ifdef VERBOSE
    printf("\n\nh: "); ffi_vec_print(h, PARAM_N);
    printf("\ns: "); ffi_vec_print(s, PARAM_N);
    printf("\nsupport_r: "); ffi_vec_print(support_r, PARAM_W_R);
    printf("\nr1: "); ffi_vec_print(r1, PARAM_N);
    printf("\nr2: "); ffi_vec_print(r2, PARAM_N);
    printf("\ne: "); ffi_vec_print(e, PARAM_N);

    printf("\nu: "); ffi_vec_print(u, PARAM_N);
    printf("\nv: "); ffi_vec_print(v, PARAM_N);
  #endif

  free(seedexpander);
}



/** 
 * \fn void rqc_pke_decrypt(ffi_vec& m, const ffi_vec& u, const ffi_vec& v, const unsigned char* sk)
 * \brief Decryption of the RQC_PKE IND-CPA scheme
 *
 * \param[out] m Vector representing the decrypted message
 * \param[in] u Vector u (first part of the ciphertext)
 * \param[in] v Vector v (second part of the ciphertext)
 * \param[in] sk String containing the secret key
 */
void rqc_pke_decrypt(ffi_vec& m, const ffi_vec& u, const ffi_vec& v, const unsigned char* sk) {

  ffi_field_init();

  // Retrieve x, y, g, h and s from secret key
  unsigned char pk[PUBLIC_KEY_BYTES];
  ffi_vec x, y;
  ffi_vec g, h, s; 

  rqc_secret_key_from_string(x, y, pk, sk);
  rqc_public_key_from_string(g, h, s, pk);

  // Compute v - u.y
  ffi_vec tmp;
  ffi_vec_mul(tmp, u, y, PARAM_N);
  ffi_vec_add(tmp, v, tmp, PARAM_N);

  #ifdef VERBOSE
    printf("\n\nu: "); ffi_vec_print(u, PARAM_N);
    printf("\nv: "); ffi_vec_print(v, PARAM_N);
    printf("\ny: "); ffi_vec_print(y, PARAM_N);
    printf("\nv - u.y: "); ffi_vec_print(tmp, PARAM_N);
  #endif

  // Compute m by decoding v - u.y
  gabidulin_code code = gabidulin_code_init(g, PARAM_K, PARAM_N);
  gabidulin_code_decode(m, code, tmp);
}

