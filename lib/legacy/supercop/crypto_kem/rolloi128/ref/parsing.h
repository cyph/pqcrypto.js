/** 
 * \file parsing.h
 * \brief Functions to parse secret key, public key and ciphertext of the LAKE scheme
 */

#ifndef PARSING_H
#define PARSING_H

#include "ffi_vec.h"
#include "lake_types.h"

void lake_secret_key_to_string(unsigned char* sk, const unsigned char* seed);
void lake_secret_key_from_string(secretKey &sk, const unsigned char* skString);

void lake_public_key_to_string(unsigned char* pkString, publicKey pk);
void lake_public_key_from_string(publicKey &pk, const unsigned char* pkString);

void lake_ciphertext_to_string(unsigned char* ct, const ffi_vec& c);
void lake_ciphertext_from_string(const unsigned char* ct, ffi_vec &c);

#endif

