/*
  This file is for the Gao-Mateer FFT
  sse http://www.math.clemson.edu/~sgao/papers/GM10.pdf
*/

#ifndef FFT_H
#define FFT_H
#define fft CRYPTO_NAMESPACE(fft)

#include <stdint.h>
#include "params.h"
#include "vec128.h"

void fft(vec128 [][GFBITS], uint64_t *);

#endif

