/**
 * \file hqc.h
 * \brief Functions of the HQC_PKE IND_CPA scheme
 */

#ifndef HQC_H
#define HQC_H

#include "parameters.h"
#include "vector.h"
#include "tensor.h"
#include "parsing.h"
#include "gf2x.h"

void hqc_pke_keygen(unsigned char* pk, unsigned char* sk);
void hqc_pke_encrypt(uint8_t* u, uint8_t* v, uint8_t* m, unsigned char* theta, const unsigned char* pk);
void hqc_pke_decrypt(uint8_t* m, uint8_t* u, uint8_t* v, const unsigned char* sk);

#endif