/** 
 * \file parsing.c
 * \brief Implementation of parsing.h
 */

#include "parsing.h"
#include "string.h"
#include "parameters.h"
#include "rbc_qre.h"

void rolloII_secret_key_to_string(uint8_t* skString, const uint8_t* seed) {
	memcpy(skString, seed, SEEDEXPANDER_SEED_BYTES);
}

void rolloII_secret_key_from_string(rolloII_secretKey* sk, const uint8_t* skString) {
  uint8_t sk_seed[SEEDEXPANDER_SEED_BYTES] = {0};
  AES_XOF_struct* sk_seedexpander;

  memcpy(sk_seed, skString, SEEDEXPANDER_SEED_BYTES);
  sk_seedexpander = (AES_XOF_struct*) malloc(sizeof(AES_XOF_struct));
  seedexpander_init(sk_seedexpander, sk_seed, sk_seed + 32, SEEDEXPANDER_MAX_LENGTH);

  rbc_vspace_init(&(sk->F), ROLLOII_PARAM_D);
  rbc_qre_init(&(sk->x));
  rbc_qre_init(&(sk->y));

  rbc_vspace_set_random_full_rank(sk_seedexpander, sk->F, ROLLOII_PARAM_D);
  rbc_qre_set_random_pair_from_support(sk_seedexpander, sk->x, sk->y, sk->F, ROLLOII_PARAM_D);

  free(sk_seedexpander);
}


void rolloII_public_key_to_string(uint8_t* pkString, rolloII_publicKey* pk) {
	rbc_qre_to_string(pkString, pk->h);
}

void rolloII_public_key_from_string(rolloII_publicKey* pk, const uint8_t* pkString) {
	rbc_qre_init(&(pk->h));
	rbc_qre_from_string(pk->h, pkString);
}


void rolloII_ciphertext_to_string(uint8_t* ctString, rolloII_ciphertext* ct) {
	rbc_qre_to_string(ctString, ct->syndrom);
  memcpy(ctString + ROLLOII_RBC_VEC_N_BYTES, ct->v, SHA512_BYTES);
  memcpy(ctString + ROLLOII_RBC_VEC_N_BYTES + SHA512_BYTES, ct->d, SHA512_BYTES);
}

void rolloII_ciphertext_from_string(rolloII_ciphertext* ct, const uint8_t* ctString) {
	rbc_qre_init(&(ct->syndrom));
	rbc_qre_from_string(ct->syndrom, ctString);
  memcpy(ct->v, ctString + ROLLOII_RBC_VEC_N_BYTES, SHA512_BYTES);
  memcpy(ct->d, ctString + ROLLOII_RBC_VEC_N_BYTES + SHA512_BYTES, SHA512_BYTES);
}
