/*
 * Written by Ming-Shing Chen and Tung Chou
 */

#ifndef _GF2X_MUL_KARATSUBA_H_
#define _GF2X_MUL_KARATSUBA_H_

#if defined(CRYPTO_NAMESPACE)
#define gf2x_mul_4096  CRYPTO_NAMESPACE(gf2x_mul_4096)
#define gf2x_mul_8192  CRYPTO_NAMESPACE(gf2x_mul_8192)
#endif


#include "gf2x_internal.h"

#include "stdint.h"


void gf2x_mul_4096(uint64_t *c, const uint64_t *a, const uint64_t *b);

void gf2x_mul_8192(uint64_t *c, const uint64_t *a, const uint64_t *b);


#endif // _GF2X_MUL_KARATSUBA_H_
