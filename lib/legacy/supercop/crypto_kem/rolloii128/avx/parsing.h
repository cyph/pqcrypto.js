/** 
 * \file parsing.h
 * \brief Functions to parse secret key, public key and ciphertext of the ROLLO scheme
 */

#ifndef PARSING_H
#define PARSING_H

#include "types.h"

void rolloII_secret_key_to_string(uint8_t* skString, const uint8_t* seed);
void rolloII_secret_key_from_string(rolloII_secretKey* sk, const uint8_t* skString);


void rolloII_public_key_to_string(uint8_t* pkString, rolloII_publicKey* pk);
void rolloII_public_key_from_string(rolloII_publicKey* pk, const uint8_t* pkString);


void rolloII_ciphertext_to_string(uint8_t* ctString, rolloII_ciphertext* ct);
void rolloII_ciphertext_from_string(rolloII_ciphertext* ct, const uint8_t* ctString);

#endif
