/**
 * \file ffi_vec.h
 * \brief Interface for vectors over finite field elements
 */

#ifndef FFI_VEC_H
#define FFI_VEC_H

#include "ffi.h"
#include "rng.h"

ffi_elt ffi_vec_get_coeff(const ffi_vec& v, unsigned int position);
void ffi_vec_set_coeff(ffi_vec& o, const ffi_elt& e, unsigned int position);
void ffi_vec_set_length(ffi_vec& o, unsigned int size);

void ffi_vec_set(ffi_vec& o, const ffi_vec& v, unsigned int size);
void ffi_vec_set_zero(ffi_vec& o, unsigned int size);
void ffi_vec_set_random(ffi_vec& o, unsigned int size, AES_XOF_struct* ctx);
void ffi_vec_set_random2(ffi_vec& o, unsigned int size);
void ffi_vec_set_random_full_rank(ffi_vec& o, unsigned int size, AES_XOF_struct* ctx);
void ffi_vec_set_random_full_rank_with_one(ffi_vec& o, unsigned int size, AES_XOF_struct* ctx);
void ffi_vec_set_random_from_support(ffi_vec& o, unsigned int size, const ffi_vec& support, unsigned int rank, AES_XOF_struct* ctx);

int ffi_vec_is_equal_to(const ffi_vec& v1, const ffi_vec& v2, unsigned int position);

void ffi_vec_add(ffi_vec &o, const ffi_vec& v1, const ffi_vec& v2, unsigned int size);
void ffi_vec_mul(ffi_vec &o, const ffi_vec& v1, const ffi_vec& v2, unsigned int size);

void ffi_vec_to_string(unsigned char* str, const ffi_vec& v, unsigned int size);
void ffi_vec_to_string_compact(unsigned char* str, const ffi_vec& v, unsigned int size);
void ffi_vec_from_string(ffi_vec &o, unsigned int size, const unsigned char *str);
void ffi_vec_from_string_compact(ffi_vec &o, unsigned int size, const unsigned char *str);

void ffi_vec_print(const ffi_vec& v, unsigned int size);

#endif

