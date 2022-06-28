/*
  This file is for Benes network related functions
*/

#ifndef BENES_H
#define BENES_H

#include "gf.h"

void load_bits(uint64_t [][32], const unsigned char *);
void benes(uint64_t *, uint64_t [][32], int);
void support_gen(gf *, const unsigned char *);

#endif

