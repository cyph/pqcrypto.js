/*
 * Copyright (c) 2018, PQShield
 * Markku-Juhani O. Saarinen
 */

//  Generic prototypes for error correction code

#ifndef _XEF_H_
#define _XEF_H_

#include <stdint.h>
#include <stddef.h>

//  Parametrized versions. f = 0..5, number of errors fixed

//  Computes the parity code, XORs it at the end of payload
//  len = payload (bytes). Returns (payload | xef) length in *bits*.
size_t xef_compute(void *block, size_t len, unsigned f);

//  Fixes errors based on parity code. Call xef_compute() first to get delta.
//  len = payload (bytes). Returns (payload | xef) length in *bits*.
size_t xef_fixerr(void *block, size_t len, unsigned f);


// specific code from optimized implementations

void xe2_53_compute(void *block);       // xe2_c16.c
void xe2_53_fixerr(void *block);

void xe4_163_compute(void *block);      // xe4_c64.c
void xe4_163_fixerr(void *block);

void xe5_190_compute(void *block);      // xe5_c64.c
void xe5_190_fixerr(void *block);
void xe5_218_compute(void *block);
void xe5_218_fixerr(void *block);
void xe5_234_compute(void *block);
void xe5_234_fixerr(void *block);

#endif /* _XEF_H_ */
