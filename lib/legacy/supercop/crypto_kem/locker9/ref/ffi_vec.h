/**
 * \file ffi_vec.h
 * \brief Interface for vectors over finite field elements
 */

#ifndef FFI_VEC_H
#define FFI_VEC_H

#include "rng.h"
#include "ffi.h"

void ffi_vec_set(ffi_vec& o, const ffi_vec& v, unsigned int size);
void ffi_vec_set_zero(ffi_vec& o, unsigned int size);
void ffi_vec_set_random_using_seedexpander(ffi_vec& o, unsigned int size, AES_XOF_struct* ctx);
void ffi_vec_set_random_full_rank_using_seedexpander(ffi_vec& o, unsigned int size, AES_XOF_struct* ctx);
void ffi_vec_set_random_from_support_using_seedexpander(ffi_vec& o, unsigned int size, const ffi_vec& support, unsigned int rank, AES_XOF_struct* ctx);
void ffi_vec_set_random_full_rank_using_rng(ffi_vec& o, unsigned int size);
void ffi_vec_set_random_from_support_using_rng(ffi_vec& o, unsigned int size, const ffi_vec& support, unsigned int rank);
void ffi_vec_set_coeff(ffi_vec &v, const ffi_elt& e, unsigned int position);
void ffi_vec_get_coeff(ffi_elt& o, const ffi_vec& v, unsigned int position);
int ffi_vec_cmp(const ffi_vec &v1, const ffi_vec &v2);

unsigned int ffi_vec_gauss(ffi_vec &o, unsigned int size);
unsigned int ffi_vec_get_rank(ffi_vec v, unsigned int size);
void ffi_vec_echelonize(ffi_vec &o, unsigned int size);

void ffi_vec_add(ffi_vec &o, const ffi_vec& v1, const ffi_vec& v2, unsigned int size);
void ffi_vec_mul(ffi_vec &o, const ffi_vec& v1, const ffi_vec& v2, unsigned int size);
void ffi_vec_scalar_mul(ffi_vec& o, const ffi_vec& v, const ffi_elt& e, unsigned int size);
void ffi_vec_tensor_mul(ffi_vec& o, const ffi_vec& v1, unsigned int size1, const ffi_vec& v2, unsigned int size2);

void ffi_vec_directsum(ffi_vec& o, const ffi_vec& v1, unsigned int size1, const ffi_vec& v2, unsigned int size2);
void ffi_vec_intersection(ffi_vec& o, unsigned int& size, const ffi_vec& v1, unsigned int size1, const ffi_vec& v2, unsigned int size2);

void ffi_vec_to_string(unsigned char* str, const ffi_vec& v, unsigned int size);
void ffi_vec_from_string(ffi_vec &v, unsigned int size, const unsigned char *str);
void ffi_vec_print(const ffi_vec& v, unsigned int size);

void ffi_vec_init_mulmod();
long ffi_vec_inv(ffi_vec &inv, ffi_vec A);

#endif

