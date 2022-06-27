/*
  This file is for the inversion-free Berlekamp-Massey algorithm
  see https://ieeexplore.ieee.org/document/87857
*/

#ifndef BM_H
#define BM_H
#define bm CRYPTO_NAMESPACE(bm)

#include <stdint.h>

#include "params.h"
#include "vec.h"

void bm(vec *, vec [][ GFBITS ]);

#endif

