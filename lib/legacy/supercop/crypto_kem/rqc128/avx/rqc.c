/** 
 * \file rqc.c
 * \brief Implementation of rqc.h
 */

#include "rqc.h"
#include "parameters.h"
#include "rbc_vspace.h"
#include "rbc_qre.h"
#include "gabidulin.h"
#include "parsing.h"

/** 
 * \fn void rqc_pke_keygen(uint8_t* pk, uint8_t* sk)
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
void rqc_pke_keygen(uint8_t* pk, uint8_t* sk) {
  AES_XOF_struct sk_seedexpander;
  AES_XOF_struct pk_seedexpander;
  uint8_t sk_seed[SEEDEXPANDER_SEED_BYTES] = {0};
  uint8_t pk_seed[SEEDEXPANDER_SEED_BYTES] = {0};
  rbc_vspace support_w;
  rbc_qre x, y;
  rbc_qre g, h, s;

  rbc_field_init();
  rbc_qre_init_modulus(RQC_PARAM_N);

  rbc_vspace_init(&support_w, RQC_PARAM_W);
  rbc_qre_init(&x);
  rbc_qre_init(&y);
  rbc_qre_init(&g);
  rbc_qre_init(&h);
  rbc_qre_init(&s);

  // Create seed expanders for public key and secret key
  randombytes(sk_seed, SEEDEXPANDER_SEED_BYTES);
  randombytes(pk_seed, SEEDEXPANDER_SEED_BYTES);
  seedexpander_init(&sk_seedexpander, sk_seed, sk_seed + 32, SEEDEXPANDER_MAX_LENGTH);
  seedexpander_init(&pk_seedexpander, pk_seed, pk_seed + 32, SEEDEXPANDER_MAX_LENGTH);

  // Compute secret key
  rbc_vspace_set_random_full_rank_with_one(&sk_seedexpander, support_w, RQC_PARAM_W);
  rbc_qre_set_random_pair_from_support(&sk_seedexpander, x, y, support_w, RQC_PARAM_W);

  // Compute public key
  rbc_qre_set_random_full_rank(&pk_seedexpander, g);
  rbc_qre_set_random(&pk_seedexpander, h); 
  
  rbc_qre_mul(s, h, y);
  rbc_qre_add(s, s, x);

  // Parse keys to string
  rqc_public_key_to_string(pk, s, pk_seed);
  rqc_secret_key_to_string(sk, sk_seed, pk);

  #ifdef VERBOSE
    printf("\n\nsk_seed: "); for(int i = 0 ; i < SEEDEXPANDER_SEED_BYTES ; ++i) printf("%02x", sk_seed[i]);
    printf("\n\npk_seed: "); for(int i = 0 ; i < SEEDEXPANDER_SEED_BYTES ; ++i) printf("%02x", pk_seed[i]);
    printf("\n\nsupport_w: "); rbc_vspace_print(support_w, RQC_PARAM_W);
    printf("\n\nx: "); rbc_qre_print(x);
    printf("\n\ny: "); rbc_qre_print(y);
    printf("\n\ng: "); rbc_qre_print(g);
    printf("\n\nh: "); rbc_qre_print(h);
    printf("\n\ns: "); rbc_qre_print(s);
    printf("\n\nsk: "); for(int i = 0 ; i < RQC_SECRET_KEY_BYTES ; ++i) printf("%02x", sk[i]);
    printf("\n\npk: "); for(int i = 0 ; i < RQC_PUBLIC_KEY_BYTES ; ++i) printf("%02x", pk[i]);
  #endif

  rbc_vspace_clear(support_w);
  rbc_qre_clear(x);
  rbc_qre_clear(y);
  rbc_qre_clear(g);
  rbc_qre_clear(h);
  rbc_qre_clear(s);
  rbc_qre_clear_modulus();
}



/** 
 * \fn void rqc_pke_encrypt(rbc_qre u, rbc_qre v, const rbc_vec m, uint8_t* theta, const uint8_t* pk)
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
void rqc_pke_encrypt(rbc_qre u, rbc_qre v, const rbc_vec m, uint8_t* theta, const uint8_t* pk) {
  AES_XOF_struct seedexpander;
  rbc_qre g, h, s;
  rbc_vspace support_e, support_r;
  rbc_qre r1, r2, e;
  rbc_qre tmp;
  rbc_gabidulin code;

  rbc_field_init();
  rbc_qre_init_modulus(RQC_PARAM_N);

  rbc_qre_init(&g);
  rbc_qre_init(&h);
  rbc_qre_init(&s);
  rbc_vspace_init(&support_e, RQC_PARAM_W_E);
  rbc_vspace_init(&support_r, RQC_PARAM_W_R);
  rbc_qre_init(&r1);
  rbc_qre_init(&r2);
  rbc_qre_init(&e);
  rbc_qre_init(&tmp);

  // Create seed_expander from theta
  seedexpander_init(&seedexpander, theta, theta + 32, SEEDEXPANDER_MAX_LENGTH);

  // Retrieve g, h and s from public key
  rqc_public_key_from_string(g, h, s, pk);

  // Generate r1, r2 and e
  rbc_vspace_set_random_full_rank(&seedexpander, support_e, RQC_PARAM_W_E);
  rbc_vspace_set(support_r, support_e, RQC_PARAM_W_R);

  rbc_qre_set_random_pair_from_support(&seedexpander, r1, r2, support_r, RQC_PARAM_W_R);
  rbc_qre_set_random_from_support(&seedexpander, e, support_e, RQC_PARAM_W_E);

  // Compute u = r1 + h.r2
  rbc_qre_mul(u, h, r2);
  rbc_qre_add(u, u, r1);

  // Compute v = m.G by encoding the message
  rbc_gabidulin_init(&code, g, RQC_PARAM_K, RQC_PARAM_N);
  rbc_gabidulin_encode(v, code, m);

  // Compute v = m.G + s.r2 + e
  rbc_qre_mul(tmp, s, r2);
  rbc_qre_add(tmp, tmp, e);
  rbc_qre_add(v, v, tmp);

  #ifdef VERBOSE
    printf("\n\ng: "); rbc_qre_print(g);
    printf("\n\nh: "); rbc_qre_print(h);
    printf("\n\ns: "); rbc_qre_print(s);
    printf("\n\nsupport_r: "); rbc_vspace_print(support_r, RQC_PARAM_W_R);
    printf("\n\nsupport_e: "); rbc_vspace_print(support_e, RQC_PARAM_W_E);
    printf("\n\nr1: "); rbc_qre_print(r1);
    printf("\n\nr2: "); rbc_qre_print(r2);
    printf("\n\ne: "); rbc_qre_print(e);
    printf("\n\nu: "); rbc_qre_print(u);
    printf("\n\nv: "); rbc_qre_print(v);
  #endif

  rbc_qre_clear(g);
  rbc_qre_clear(h);
  rbc_qre_clear(s);
  rbc_vspace_clear(support_r);
  rbc_vspace_clear(support_e);
  rbc_qre_clear(r1);
  rbc_qre_clear(r2);
  rbc_qre_clear(e);
  rbc_qre_clear(tmp);
  rbc_qre_clear_modulus();
}



/** 
 * \fn void rqc_pke_decrypt(rbc_vec m, const rbc_qre u, const rbc_qre v, const uint8_t* sk)
 * \brief Decryption of the RQC_PKE IND-CPA scheme
 *
 * \param[out] m Vector representing the decrypted message
 * \param[in] u Vector u (first part of the ciphertext)
 * \param[in] v Vector v (second part of the ciphertext)
 * \param[in] sk String containing the secret key
 */
void rqc_pke_decrypt(rbc_vec m, const rbc_qre u, const rbc_qre v, const uint8_t* sk) {
  uint8_t pk[RQC_PUBLIC_KEY_BYTES] = {0};
  rbc_qre x, y, g, h, s; 
  rbc_qre tmp;
  rbc_gabidulin code;

  rbc_field_init();
  rbc_qre_init_modulus(RQC_PARAM_N);

  rbc_qre_init(&x);
  rbc_qre_init(&y);
  rbc_qre_init(&g);
  rbc_qre_init(&h);
  rbc_qre_init(&s);
  rbc_qre_init(&tmp);

  // Retrieve x, y, g, h and s from secret key
  rqc_secret_key_from_string(x, y, pk, sk);
  rqc_public_key_from_string(g, h, s, pk);

  // Compute v - u.y
  rbc_qre_mul(tmp, u, y);
  rbc_qre_add(tmp, v, tmp);

  // Compute m by decoding v - u.y
  rbc_gabidulin_init(&code, g, RQC_PARAM_K, RQC_PARAM_N);
  rbc_gabidulin_decode(m, code, tmp);

  #ifdef VERBOSE
    printf("\n\nu: "); rbc_qre_print(u);
    printf("\n\nv: "); rbc_qre_print(v);
    printf("\n\ny: "); rbc_qre_print(y);
    printf("\n\nv - u.y: "); rbc_qre_print(tmp);
  #endif

  rbc_qre_clear(x);
  rbc_qre_clear(y);
  rbc_qre_clear(g);
  rbc_qre_clear(h);
  rbc_qre_clear(s);
  rbc_qre_clear(tmp);
  rbc_qre_clear_modulus();
}

