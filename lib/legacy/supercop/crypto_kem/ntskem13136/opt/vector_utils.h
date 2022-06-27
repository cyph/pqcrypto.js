/**
 *  vector_utils.h
 *  NTS-KEM
 *
 *  Parameter: NTS-KEM(13, 136)
 *  Platform: Intel 64-bit
 *
 *  This file is part of the optimized implemention of NTS-KEM
 *  submitted as part of NIST Post-Quantum Cryptography
 *  Standardization Process.
 **/

#ifndef __NTSKEM_VECTOR_UTILS_H
#define __NTSKEM_VECTOR_UTILS_H

#include <stdint.h>

#define PARAM_M     13

uint64_t vector_ff_or_64(const uint64_t *in);

void vector_load_1d_64(uint64_t b[PARAM_M],   const uint16_t* a, int32_t size);
void vector_load_2d_64(uint64_t (*b)[PARAM_M], const uint16_t* a, int32_t size);

void vector_store_1d_64(uint16_t* b, const uint64_t a[PARAM_M],   int32_t size);
void vector_store_2d_64(uint16_t* b, const uint64_t (*a)[PARAM_M], int32_t size);

#endif /* __NTSKEM_VECTOR_UTILS_H */
