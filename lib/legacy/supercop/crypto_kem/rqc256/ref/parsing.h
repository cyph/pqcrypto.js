/** 
 * \file parsing.h
 * \brief Functions to parse secret key, public key and ciphertext of the RQC scheme
 */

#ifndef RQC_PARSING_H
#define RQC_PARSING_H

#include "rbc_qre.h"


void rqc_secret_key_to_string(uint8_t* sk, const uint8_t* seed, const uint8_t* pk);
void rqc_secret_key_from_string(rbc_qre x, rbc_qre y, uint8_t* pk, const uint8_t* sk);

void rqc_public_key_to_string(uint8_t* pk, const rbc_qre s, const uint8_t* seed);
void rqc_public_key_from_string(rbc_qre g, rbc_qre h, rbc_qre s, const uint8_t* pk);

void rqc_kem_ciphertext_to_string(uint8_t* ct, const rbc_qre u, const rbc_qre v, const uint8_t* d);
void rqc_kem_ciphertext_from_string(rbc_qre u, rbc_qre v, uint8_t* d, const uint8_t* ct);

#endif

