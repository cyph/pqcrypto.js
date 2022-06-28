/** 
 * \file parsing.h
 * \brief Functions to parse secret key, public key and ciphertext of the RQC scheme
 */

#ifndef PARSING_H
#define PARSING_H

#include "ffi_vec.h"


void rqc_secret_key_to_string(unsigned char* sk, const unsigned char* seed, const unsigned char* pk);
void rqc_secret_key_from_string(ffi_vec& x, ffi_vec& y, unsigned char* pk, const unsigned char* sk);

void rqc_public_key_to_string(unsigned char* pk, const ffi_vec& s, const unsigned char* seed);
void rqc_public_key_from_string(ffi_vec& g, ffi_vec& h, ffi_vec& s, const unsigned char* pk);

void rqc_kem_ciphertext_to_string(unsigned char* ct, const ffi_vec& u, const ffi_vec& v, const unsigned char* d);
void rqc_kem_ciphertext_from_string(ffi_vec& u, ffi_vec& v, unsigned char* d, const unsigned char* ct);

#endif

