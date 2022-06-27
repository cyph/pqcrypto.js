/** 
 * \file parsing.h
 * \brief Functions to parse secret key, public key and ciphertext of the ROLLO scheme
 */

#ifndef PARSING_H
#define PARSING_H

#include "types.h"

void rolloI_secret_key_to_string(uint8_t* skString, const uint8_t* seed);
void rolloI_secret_key_from_string(rolloI_secretKey* sk, const uint8_t* skString);


void rolloI_public_key_to_string(uint8_t* pkString, rolloI_publicKey* pk);
void rolloI_public_key_from_string(rolloI_publicKey* pk, const uint8_t* pkString);


void rolloI_rolloI_ciphertext_to_string(uint8_t* ctString, rolloI_ciphertext* ct);
void rolloI_rolloI_ciphertext_from_string(rolloI_ciphertext* ct, const uint8_t* ctString);

#endif
