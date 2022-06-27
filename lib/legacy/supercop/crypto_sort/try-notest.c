/*
 * crypto_sort/try-notest.c version 20180810
 * D. J. Bernstein
 * Public domain.
 */

#include "crypto_sort.h"
#include "try.h"

#define TUNE 768

const char *primitiveimplementation = crypto_sort_implementation;

static unsigned char *x;

void preallocate(void)
{
}

void allocate(void)
{
  x = alignedcalloc(crypto_sort_BYTES * TUNE);
}

void unallocate(void)
{
  alignedfree(x);
}

void predoit(void)
{
}

void doit(void)
{
  crypto_sort(x,TUNE);
}
