/** 
 * \file parsing.h
 * \brief Functions to parse secret key, public key and ciphertext of the Ouroboros scheme
 */

#ifndef PARSING_H
#define PARSING_H

#include "ffi_vec.h"


void ouroborosr_secret_key_to_string(unsigned char* sk, const unsigned char* seed);
void ouroborosr_secret_key_from_string(ffi_vec& x, ffi_vec& y, ffi_vec& F, const unsigned char* sk);

void ouroborosr_public_key_to_string(unsigned char* pk, const ffi_vec& s, const unsigned char* seed);
void ouroborosr_public_key_from_string(ffi_vec& h, ffi_vec& s, const unsigned char* pk);

void ouroborosr_ciphertext_to_string(unsigned char* ct, const ffi_vec& sr, const ffi_vec& se);
void ouroborosr_ciphertext_from_string(ffi_vec& sr, ffi_vec& se, const unsigned char* ct);

#endif

