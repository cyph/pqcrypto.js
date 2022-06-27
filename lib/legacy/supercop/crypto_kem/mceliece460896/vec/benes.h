/*
  This file is for Benes network related functions
*/

#ifndef BENES_H
#define BENES_H
#define benes CRYPTO_NAMESPACE(benes)
#define support_gen CRYPTO_NAMESPACE(support_gen)

#include "gf.h"
#include "vec.h"

void benes(vec *, const unsigned char *, int);

#endif

