/**
 * \file rbc_vec.h
 * \brief Interface for vectors of finite field elements
 */

#ifndef RBC_VEC_H
#define RBC_VEC_H

#include "rbc.h"
#include "rbc_elt.h"
#include "seedexpander.h"


void rbc_vec_init(rbc_vec* v, uint32_t size);
void rbc_vec_clear(rbc_vec v);

void rbc_vec_set_zero(rbc_vec v, uint32_t size);
void rbc_vec_set(rbc_vec o, const rbc_vec v, uint32_t size);
void rbc_vec_set_random(AES_XOF_struct* ctx, rbc_vec o, uint32_t size);
void rbc_vec_set_random2(rbc_vec o, uint32_t size);
void rbc_vec_set_random_full_rank(AES_XOF_struct*, rbc_vec o, uint32_t size);
void rbc_vec_set_random_full_rank2(rbc_vec o, uint32_t size);
void rbc_vec_set_random_full_rank_with_one(AES_XOF_struct* ctx, rbc_vec o, uint32_t size);
void rbc_vec_set_random_from_support(AES_XOF_struct* ctx, rbc_vec o, uint32_t size, const rbc_vec support, uint32_t support_size);
void rbc_vec_set_random_pair_from_support(AES_XOF_struct* ctx, rbc_vec o1, rbc_vec o2, uint32_t size, const rbc_vec support, uint32_t support_size);
void rbc_vec_set_random_pair_from_support2(rbc_vec o1, rbc_vec o2, uint32_t size, const rbc_vec support, uint32_t support_size);

uint32_t rbc_vec_gauss(rbc_vec o, uint32_t size);
uint32_t rbc_vec_gauss_constant_time(rbc_vec v, uint32_t size, rbc_vec *other_matrices, uint32_t nMatrices);
uint32_t rbc_vec_gauss_other_matrices(rbc_vec v, uint32_t size, rbc_vec *other_matrices, uint32_t nMatrices);
uint32_t rbc_vec_get_rank(const rbc_vec v, uint32_t size);
uint32_t rbc_vec_echelonize(rbc_vec o, uint32_t size);

void rbc_vec_add(rbc_vec o, const rbc_vec v1, const rbc_vec v2, uint32_t size);
void rbc_vec_scalar_mul(rbc_vec o, const rbc_vec v, const rbc_elt e, uint32_t size);

void rbc_vec_to_string(uint8_t* str, const rbc_vec v, uint32_t size);
void rbc_vec_from_string(rbc_vec v, uint32_t size, const uint8_t* str);
void rbc_vec_print(const rbc_vec v, uint32_t size);

#endif

