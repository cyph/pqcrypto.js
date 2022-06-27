#ifndef NTT_H
#define NTT_H

#include "inttypes.h"

void ntt(int16_t *poly);
void invntt(int16_t *poly);
void basemul(int16_t *r, const int16_t *a, const int16_t *b, int16_t offset);

#endif
