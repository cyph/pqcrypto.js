/*
  This file is for Benes network related functions
*/

#ifndef BENES_H
#define BENES_H
#define benes CRYPTO_NAMESPACE(benes)
#define support_gen CRYPTO_NAMESPACE(support_gen)

#include "gf.h"

void benes(uint64_t *, const unsigned char *, int);
void support_gen(gf *, const unsigned char *);

#endif

