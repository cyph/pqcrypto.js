/*
  This file is for matrix transposition
*/

#ifndef TRANSPOSE_H
#define TRANSPOSE_H

#include "vec128.h"

void transpose_64x128_sp_asm(vec128 *);

static void transpose_64x128_sp(vec128 *in)
{
        transpose_64x128_sp_asm(in);
}

#endif

