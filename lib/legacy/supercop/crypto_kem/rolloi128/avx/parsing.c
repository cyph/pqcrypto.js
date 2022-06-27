/** 
 * \file parsing.c
 * \brief Implementation of parsing.h
 */

#include "parsing.h"
#include "string.h"
#include "parameters.h"

void rolloI_secret_key_to_string(uint8_t* skString, const uint8_t* seed) {
	memcpy(skString, seed, SEEDEXPANDER_SEED_BYTES);
}

void rolloI_secret_key_from_string(rolloI_secretKey* sk, const uint8_t* skString) {
  uint8_t sk_seed[SEEDEXPANDER_SEED_BYTES] = {0};
  AES_XOF_struct* sk_seedexpander;
  
  memcpy(sk_seed, skString, SEEDEXPANDER_SEED_BYTES);
  sk_seedexpander = (AES_XOF_struct*) malloc(sizeof(AES_XOF_struct));
  seedexpander_init(sk_seedexpander, sk_seed, sk_seed + 32, SEEDEXPANDER_MAX_LENGTH);

  rbc_vspace_init(&(sk->F), ROLLOI_PARAM_D);
  rbc_qre_init(&(sk->x));
  rbc_qre_init(&(sk->y));

  rbc_vspace_set_random_full_rank(sk_seedexpander, sk->F, ROLLOI_PARAM_D);
  rbc_qre_set_random_pair_from_support(sk_seedexpander, sk->x, sk->y, sk->F, ROLLOI_PARAM_D);

  free(sk_seedexpander);
}


void rolloI_public_key_to_string(uint8_t* pkString, rolloI_publicKey* pk) {
	rbc_qre_to_string(pkString, pk->h);
}

void rolloI_public_key_from_string(rolloI_publicKey* pk, const uint8_t* pkString) {
	rbc_qre_init(&(pk->h));
	rbc_qre_from_string(pk->h, pkString);
}


void rolloI_rolloI_ciphertext_to_string(uint8_t* ctString, rolloI_ciphertext* ct) {
	rbc_qre_to_string(ctString, ct->syndrom);
}

void rolloI_rolloI_ciphertext_from_string(rolloI_ciphertext* ct, const uint8_t* ctString) {
	rbc_qre_init(&(ct->syndrom));
	rbc_qre_from_string(ct->syndrom, ctString);
}
