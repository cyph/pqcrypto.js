/*
 * crypto_verify/try.c version 20200809
 * D. J. Bernstein
 * Public domain.
 */

#include "crypto_verify.h"
#include "kernelrandombytes.h"
#include "try.h"

#ifdef TIMECOP
#define LOOPS TIMECOP_LOOPS
#else
#ifdef SMALL
#define LOOPS 10000
#else
#define LOOPS 1000000
#endif
#endif

const char *primitiveimplementation = crypto_verify_IMPLEMENTATION;

static unsigned char *x;
static unsigned char *y;

void preallocate(void)
{
}

void allocate(void)
{
  x = alignedcalloc(crypto_verify_BYTES);
  y = alignedcalloc(crypto_verify_BYTES);
}

void unalign(void)
{
  ++x;
  ++y;
}

void realign(void)
{
  --x;
  --y;
}

void predoit(void)
{
}

void doit(void)
{
  crypto_verify(x,y);
}

static void check(void)
{
  int r;
  
  poison(x,crypto_verify_BYTES);
  poison(y,crypto_verify_BYTES);
  r = crypto_verify(x,y);
  unpoison(x,crypto_verify_BYTES);
  unpoison(y,crypto_verify_BYTES);
  unpoison(&r,sizeof r);

  if (r == 0) {
    if (memcmp(x,y,crypto_verify_BYTES)) fail("different strings pass verify");
  } else if (r == -1) {
    if (!memcmp(x,y,crypto_verify_BYTES)) fail("equal strings fail verify");
  } else {
    fail("weird return value from verify");
  }
}

void test(void)
{
  long long tests;

  for (tests = 0;tests < LOOPS;++tests) {
    kernelrandombytes(x,crypto_verify_BYTES);
    kernelrandombytes(y,crypto_verify_BYTES);
    check();
    memcpy(y,x,crypto_verify_BYTES);
    check();
    y[myrandom() % crypto_verify_BYTES] = myrandom();
    check();
    y[myrandom() % crypto_verify_BYTES] = myrandom();
    check();
    y[myrandom() % crypto_verify_BYTES] = myrandom();
    check();
  }
}
