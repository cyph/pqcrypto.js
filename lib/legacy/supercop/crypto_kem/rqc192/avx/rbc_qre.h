/**
 * \file rbc_qre.h
 * \brief Interface for quotient ring elements 
 * */

#ifndef RBC_QRE_H
#define RBC_QRE_H

#include "rbc.h"
#include "seedexpander.h"


void rbc_qre_init_modulus(uint32_t degree);
rbc_poly_sparse rbc_qre_get_modulus(uint32_t degree);
void rbc_qre_clear_modulus();

void rbc_qre_init(rbc_qre* p);
void rbc_qre_clear(rbc_qre p);

void rbc_qre_set_zero(rbc_qre o);
void rbc_qre_set_random(AES_XOF_struct* ctx, rbc_qre o);
void rbc_qre_set_random2(rbc_qre o);
void rbc_qre_set_random_full_rank(AES_XOF_struct* ctx, rbc_qre o);
void rbc_qre_set_random_full_rank_with_one(AES_XOF_struct* ctx, rbc_qre o);
void rbc_qre_set_random_from_support(AES_XOF_struct* ctx, rbc_qre o, const rbc_vspace support, uint32_t support_size);
void rbc_qre_set_random_pair_from_support(AES_XOF_struct* ctx, rbc_qre o1, rbc_qre o2, const rbc_vspace support, uint32_t support_size);
void rbc_qre_set_random_pair_from_support2(rbc_qre o1, rbc_qre o2, const rbc_vspace support, uint32_t support_size);

uint8_t rbc_qre_is_equal_to(const rbc_qre p1, const rbc_qre p2);

void rbc_qre_add(rbc_qre o, const rbc_qre p1, const rbc_qre p2);
void rbc_qre_mul(rbc_qre o, const rbc_qre p1, const rbc_qre p2);
void rbc_qre_div(rbc_qre q, rbc_qre r, const rbc_qre a, const rbc_qre b);
void rbc_qre_inv(rbc_qre o, const rbc_qre p);

void rbc_qre_to_string(uint8_t* str, const rbc_qre p);
void rbc_qre_from_string(rbc_qre o, const uint8_t* str);

void rbc_qre_print(const rbc_qre p);

#endif

