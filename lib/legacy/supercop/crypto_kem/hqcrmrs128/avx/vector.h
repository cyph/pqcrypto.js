#ifndef VECTOR_H
#define VECTOR_H

/**
 * @file vector.h
 * @brief Header file for vector.c
 */

#include "rng.h"
#include "randombytes.h"
#include <stdint.h>

void vect_set_random_fixed_weight(AES_XOF_struct *ctx, uint64_t *v, uint16_t weight);
void vect_set_random(AES_XOF_struct *ctx, uint64_t *v);
void vect_set_random_from_randombytes(uint64_t *v);

void vect_add(uint64_t *o, const uint64_t *v1, const uint64_t *v2, uint32_t size);
int vect_compare(const uint64_t *v1, const uint64_t *v2, uint32_t size);
void vect_resize(uint64_t *o, uint32_t size_o, const uint64_t *v, uint32_t size_v);

void vect_print(const uint64_t *v, const uint32_t size);
void vect_print_sparse(const uint32_t *v, const uint16_t weight);

#endif
