/**
 * \file rbc_vspace.h
 * \brief Interface for subspaces of Fq^m
 */

#ifndef RBC_VSPACE_H
#define RBC_VSPACE_H

#include "rbc.h"
#include "seedexpander.h"


void rbc_vspace_init(rbc_vspace* vs, uint32_t size);
void rbc_vspace_clear(rbc_vspace vs);

void rbc_vspace_set_zero(rbc_vspace o, uint32_t size);
void rbc_vspace_set(rbc_vspace o, const rbc_vspace vs, uint32_t size);
void rbc_vspace_set_random_full_rank(AES_XOF_struct* ctx, rbc_vspace o, uint32_t size);
void rbc_vspace_set_random_full_rank2(rbc_vspace o, uint32_t size);
void rbc_vspace_set_random_full_rank_with_one(AES_XOF_struct* ctx, rbc_vspace o, uint32_t size);

void rbc_vspace_directsum(rbc_vspace o, const rbc_vspace vs1, const rbc_vspace vs2, uint32_t vs1_size, uint32_t vs2_size);
void rbc_vspace_product(rbc_vspace o, const rbc_vspace vs1, const rbc_vspace vs2, uint32_t vs1_size, uint32_t vs2_size);
uint32_t rbc_vspace_intersection(rbc_vspace o, const rbc_vspace vs1, const rbc_vspace vs2, uint32_t vs1_size, uint32_t vs2_size);
uint32_t rbc_vspace_intersection_constant_time(rbc_vspace o, const rbc_vspace vs1, const rbc_vspace vs2, uint32_t vs1_size, uint32_t vs2_size);

void rbc_vspace_print(const rbc_vspace vs, uint32_t size);

#endif

