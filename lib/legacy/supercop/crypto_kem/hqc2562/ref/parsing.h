/**
 * \file parsing.h
 * \brief Header file for parsing.cpp
 */

#ifndef PARSING_H
#define PARSING_H

#include "vector.h"

void hqc_secret_key_to_string(unsigned char* sk, const unsigned char* sk_seed, const unsigned char* pk);
void hqc_secret_key_from_string(uint8_t* x, uint8_t* y, unsigned char* pk, const unsigned char* sk);

void hqc_public_key_to_string(unsigned char* pk, const unsigned char* pk_seed, uint8_t* s);
void hqc_public_key_from_string(uint8_t* h, uint8_t* s, const unsigned char* pk);

void hqc_ciphertext_to_string(unsigned char* ct, uint8_t* u, uint8_t* v, const unsigned char* d);
void hqc_ciphertext_from_string(uint8_t* u, uint8_t* v, unsigned char* d, const unsigned char* ct);

#endif