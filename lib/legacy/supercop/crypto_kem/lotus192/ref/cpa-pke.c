/**
 * @file cpa-pke.c
 * @author Takuya HAYASHI (t-hayashi@eedept.kobe-u.ac.jp)
 * @brief An implementaion of LOTUS-CPA-PKE
 */

#include "type.h"
#include "param.h"
#include "crypto.h"
#include "lwe-arithmetics.h"
#include "sampler.h"
#include "randombytes.h"

#include <stdio.h>
#include <string.h>

typedef struct {
  U16 A[_LOTUS_LWE_DIM * _LOTUS_LWE_DIM];
  U16 P[_LOTUS_LWE_DIM * _LOTUS_LWE_PT];
} pubkey_t;

typedef struct {
  U16 S[_LOTUS_LWE_DIM * _LOTUS_LWE_PT];
  pubkey_t pk;
} prvkey_t;

typedef struct {
  U16 c1[_LOTUS_LWE_DIM];
  U16 c2[_LOTUS_LWE_PT];
} ct_t;


/**
 * @param[out] pk a public key
 * @param[out] sk a secret key
 */
void lotus_cpa_pke_keypair(U16 *pk, U16 *sk){
  U8 A_seed[_LOTUS_SEED_BYTES];
  pubkey_t *pkt = (pubkey_t*)pk;
  prvkey_t *skt = (prvkey_t*)sk;
  /* A_seed is a seed for sampling the uniform matrix A, which means A can be compressed 
     to A_seed for transmission then be extracted via sampling, i.e., sample_uniform(). 
     However, sample_uniform() is relatively slow and it is better to be called just once 
     (after transmission), but current API does not match the case since API does not allow
     precomputation. 
     Therefore in this implementation we just store A and P to the public key. */
  randombytes(A_seed, _LOTUS_SEED_BYTES);
  sampler_set_seed(A_seed);
  sample_uniform(pkt->A, _LOTUS_LWE_DIM * _LOTUS_LWE_DIM);

  /* Sampler should be initialized again for sampling independently from the matrix A. */
  sampler_init();
  sample_discrete_gaussian(skt->S, _LOTUS_LWE_DIM * _LOTUS_LWE_PT);
  sample_discrete_gaussian(pkt->P, _LOTUS_LWE_DIM * _LOTUS_LWE_PT); /* P = R */
  
  submul(pkt->P, pkt->A, skt->S); /* P -= A * S, so P = R - AS. */
  redc(pkt->P, _LOTUS_LWE_DIM * _LOTUS_LWE_PT);
}


/**
 * @param[out] ct a ciphertext corresponding to sigma
 * @param[in] sigma a message to be encrypted
 * @param[in] pk a public key
 */
void lotus_cpa_pke_enc(U16 *ct, const U8 *sigma, const U16 *pk){
  const pubkey_t *pkt = (pubkey_t*)pk;
  ct_t *c = (ct_t*)ct;
  U16 e[_LOTUS_LWE_DIM];

  sample_discrete_gaussian(e, _LOTUS_LWE_DIM);
  sample_discrete_gaussian((U16*)c, _LOTUS_LWE_DIM + _LOTUS_LWE_PT); /* c = [e2|e3] */

  addmul_concat((U16*)c, e, pkt->A, pkt->P); /* c += e * [A|P] */

  add_sigma(c->c2, sigma); /* c2 += sigma * \lceil q / 2 \rceil */

  redc((U16*)c, _LOTUS_LWE_DIM + _LOTUS_LWE_PT); /* c = c mod q */

  OPENSSL_cleanse(e, _LOTUS_LWE_DIM * sizeof(U16)); /* cleanse e */
}

/**
 * @param[out] sigma a message corresponding to ct
 * @param[in] ct a ciphertext to be decrypted
 * @param[in] sk a secret key
 */
void lotus_cpa_pke_dec(U8 *sigma, const U16 *ct, const U16 *sk){
  const prvkey_t *skt = (prvkey_t*)sk;
  ct_t *c = (ct_t*)ct;
  U16 t[_LOTUS_LWE_PT];

  memcpy(t, c->c2, _LOTUS_LWE_PT * sizeof(U16)); /* t = c2 */
  addmul(t, (U16*)c, skt->S); /* t += c1 * S */

  reconstruct(sigma, t); /* compute sigma from t */

  OPENSSL_cleanse(t, _LOTUS_LWE_PT * sizeof(U16)); /* cleanse t */
}
