#ifndef UTIL_H
#define UTIL_H

#include "vec256.h"

#include <stdint.h>

void store8(unsigned char *, uint64_t);
uint64_t load8(const unsigned char *);

vec128 load16(const unsigned char *);

void store32(unsigned char *, vec256);

#endif

