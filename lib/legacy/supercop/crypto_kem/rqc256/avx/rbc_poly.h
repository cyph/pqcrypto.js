/**
 * \file rbc_poly.h
 * \brief Interface for polynomials over a finite field
 */

#ifndef RBC_POLY_H
#define RBC_POLY_H

#include "rbc.h"
#include "rbc_vspace.h"
#include "seedexpander.h"


void rbc_poly_init(rbc_poly* p, int32_t degree);
void rbc_poly_resize(rbc_poly p, int32_t degree);
void rbc_poly_clear(rbc_poly p);

void rbc_poly_sparse_init(rbc_poly_sparse* p, uint32_t coeff_nb, uint32_t *coeffs);
void rbc_poly_sparse_clear(rbc_poly_sparse p);

void rbc_poly_set_zero(rbc_poly o, int32_t degree);
void rbc_poly_set(rbc_poly o, const rbc_poly p);
void rbc_poly_set_random(AES_XOF_struct* ctx, rbc_poly o, int32_t degree);
void rbc_poly_set_random2(rbc_poly o, int32_t degree);
void rbc_poly_set_random_full_rank(AES_XOF_struct* ctx, rbc_poly o, int32_t degree);
void rbc_poly_set_random_full_rank_with_one(AES_XOF_struct* ctx, rbc_poly o, int32_t degree);
void rbc_poly_set_random_from_support(AES_XOF_struct* ctx, rbc_poly o, int32_t degree, const rbc_vspace support, uint32_t support_size);
void rbc_poly_set_random_pair_from_support(AES_XOF_struct* ctx, rbc_poly o1, rbc_poly o2, int32_t degree, const rbc_vspace support, uint32_t support_size);
void rbc_poly_set_random_pair_from_support2(rbc_poly o1, rbc_poly o2, int32_t degree, const rbc_vspace support, uint32_t support_size);

void rbc_poly_update_degree(rbc_poly p, int32_t max_degree);
uint8_t rbc_poly_is_equal_to(const rbc_poly p1, const rbc_poly p2);

void rbc_poly_add(rbc_poly o, const rbc_poly p1, const rbc_poly p2);
void rbc_poly_add2(rbc_poly o, const rbc_poly p1, const rbc_poly p2, int32_t p1_degree, int32_t p2_degree);
void rbc_poly_mul(rbc_poly o, const rbc_poly p1, const rbc_poly p2);
void rbc_poly_mul2(rbc_poly o, const rbc_poly p1, const rbc_poly p2, int32_t p1_degree, int32_t p2_degree);
void rbc_poly_mulmod_sparse(rbc_poly o, const rbc_poly p1, const rbc_poly p2, const rbc_poly_sparse modulus);
void rbc_poly_div(rbc_poly q, rbc_poly r, const rbc_poly a, const rbc_poly b);
void rbc_poly_inv(rbc_poly o, const rbc_poly p, const rbc_poly modulus);

void rbc_poly_to_string(uint8_t* str, const rbc_poly p);
void rbc_poly_from_string(rbc_poly p, const uint8_t* str);

void rbc_poly_print(const rbc_poly p);
void rbc_poly_sparse_print(const rbc_poly_sparse p);

#endif

