/**
 * \file rbc_vec.h
 * \brief Interface for finite field elements
 */

#ifndef RBC_ELT_H
#define RBC_ELT_H

#include "rbc.h"
#include "randombytes.h"
#include "seedexpander.h"


/********************
* rbc_elt functions *
********************/

void rbc_field_init();
void rbc_elt_set_zero(rbc_elt o);
void rbc_elt_set_one(rbc_elt o);
void rbc_elt_set(rbc_elt o, const rbc_elt e);
void rbc_elt_set_mask1(rbc_elt o, const rbc_elt e1, const rbc_elt e2, uint32_t mask);
void rbc_elt_set_mask2(rbc_elt o1, rbc_elt o2, const rbc_elt e, uint32_t mask);
void rbc_elt_set_from_uint64(rbc_elt o, const uint64_t* e);
void rbc_elt_set_random(rbc_elt o, AES_XOF_struct* ctx);
void rbc_elt_set_random2(rbc_elt o);

int32_t rbc_elt_get_degree(const rbc_elt e);
uint8_t rbc_elt_get_coefficient(const rbc_elt e, uint32_t index);
void rbc_elt_set_coefficient(rbc_elt o, uint32_t index, uint8_t bit);
uint8_t rbc_elt_is_zero(const rbc_elt e);
uint8_t rbc_elt_is_equal_to(const rbc_elt e1, const rbc_elt e2);
uint8_t rbc_elt_is_greater_than(const rbc_elt e1, const rbc_elt e2);

void rbc_elt_add(rbc_elt o, const rbc_elt e1, const rbc_elt e2);
void rbc_elt_mul(rbc_elt o, const rbc_elt e1, const rbc_elt e2);
void rbc_elt_inv(rbc_elt o, const rbc_elt e);
void rbc_elt_sqr(rbc_elt o, const rbc_elt e);
void rbc_elt_nth_root(rbc_elt o, const rbc_elt e, uint32_t n);
void rbc_elt_reduce(rbc_elt o, const rbc_elt_ur e);

void rbc_elt_print(const rbc_elt e);



/***********************
* rbc_elt_ur functions *
***********************/

void rbc_elt_ur_set_zero(rbc_elt_ur o);
void rbc_elt_ur_set_from_uint64(rbc_elt o, const uint64_t* e);

void rbc_elt_ur_mul(rbc_elt_ur o, const rbc_elt e1, const rbc_elt e2);
void rbc_elt_ur_sqr(rbc_elt_ur o, const rbc_elt e);

void rbc_elt_ur_print(const rbc_elt_ur e);

#endif
