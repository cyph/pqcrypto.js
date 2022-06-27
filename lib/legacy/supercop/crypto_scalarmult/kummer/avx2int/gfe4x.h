#ifndef GFE4X_H
#define GFE4X_H

#include "crypto_int32.h"
#include <immintrin.h>

typedef __m256i vec;

typedef struct {
  vec v[5];
} gfe4x;

#define gfe4x_mul CRYPTO_NAMESPACE(gfe4x_mul)
#define gfe4x_precompute CRYPTO_NAMESPACE(gfe4x_precompute)
#define gfe4x_mulprecomputed CRYPTO_NAMESPACE(gfe4x_mulprecomputed)
#define gfe4x_square CRYPTO_NAMESPACE(gfe4x_square)
#define gfe4x_mulconst CRYPTO_NAMESPACE(gfe4x_mulconst)
#define gfe4x_hadamard CRYPTO_NAMESPACE(gfe4x_hadamard)
#define gfe4x_select CRYPTO_NAMESPACE(gfe4x_select)
#define gfe4x_from_gfe CRYPTO_NAMESPACE(gfe4x_from_gfe)
#define gfe4x_to_gfe CRYPTO_NAMESPACE(gfe4x_to_gfe)

void gfe4x_mul(gfe4x *,const gfe4x *,const gfe4x *);
void gfe4x_precompute(gfe4x *,const gfe4x *);
void gfe4x_mulprecomputed(gfe4x *,const gfe4x *,const gfe4x *,const gfe4x *);
void gfe4x_square(gfe4x *,const gfe4x *);
void gfe4x_mulconst(gfe4x *,const gfe4x *,const vec *);
void gfe4x_hadamard(gfe4x *,gfe4x *);
void gfe4x_select(gfe4x *,const gfe4x *,const gfe4x *,int);

#endif
