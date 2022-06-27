/*
  This file is for matrix transposition
*/

#ifndef TRANSPOSE_H
#define TRANSPOSE_H
#define transpose_64x128_sp_asm CRYPTO_NAMESPACE(transpose_64x128_sp_asm)
#define transpose_64x64_asm CRYPTO_NAMESPACE(transpose_64x64_asm)

#include "vec128.h"

#include <stdint.h>

extern void transpose_64x64_asm(uint64_t *);

static inline void transpose_64x64(uint64_t *in)
{
	transpose_64x64_asm(in);
}

extern void transpose_64x128_sp_asm(vec128 *);

static inline void transpose_64x128_sp(vec128 *in)
{
	transpose_64x128_sp_asm(in);
}

#endif

