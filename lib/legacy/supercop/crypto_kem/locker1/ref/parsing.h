/** 
 * \file parsing.h
 * \brief Functions to parse secret key, public key and ciphertext of the LOCKER scheme
 */

#ifndef PARSING_H
#define PARSING_H

#include "ffi_vec.h"
#include "locker_types.h"

void locker_secret_key_to_string(unsigned char* sk, const unsigned char* seed);
void locker_secret_key_from_string(secretKey &sk, const unsigned char* skString);

void locker_public_key_to_string(unsigned char* pkString, publicKey pk);
void locker_public_key_from_string(publicKey &pk, const unsigned char* pkString);

void locker_ciphertext_to_string(unsigned char* ct, const ciphertext &c);
void locker_ciphertext_from_string(const unsigned char* ct, ciphertext &c);

#endif

