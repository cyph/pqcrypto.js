/*
 * crypto_sort/try.c version 20200809
 * D. J. Bernstein
 * Public domain.
 */

#include <string.h>
#include "crypto_sort.h"
#include "try.h"

const char *primitiveimplementation = crypto_sort_IMPLEMENTATION;

#define TUNE 768
#ifdef SMALL
#define MAXTEST 128
#else
#define MAXTEST 4096
#endif

#ifdef TIMECOP
#define LOOPS TIMECOP_LOOPS
#else
#ifdef SMALL
#define LOOPS 1024
#else
#define LOOPS 4096
#endif
#endif

static unsigned char *x;
static unsigned char *y;
static unsigned char *x2;
static unsigned char *y2;
unsigned long long len;

void preallocate(void)
{
}

void allocate(void)
{
  unsigned long long alloclen = 0;
  if (alloclen < TUNE) alloclen = TUNE;
  if (alloclen < MAXTEST) alloclen = MAXTEST;
  x = alignedcalloc(crypto_sort_BYTES * alloclen);
  y = alignedcalloc(crypto_sort_BYTES * alloclen);
  x2 = alignedcalloc(crypto_sort_BYTES * alloclen);
  y2 = alignedcalloc(crypto_sort_BYTES * alloclen);
}

void unalign(void)
{
  x += crypto_sort_BYTES;
  y += crypto_sort_BYTES;
  x2 += crypto_sort_BYTES;
  y2 += crypto_sort_BYTES;
}

void realign(void)
{
  x -= crypto_sort_BYTES;
  y -= crypto_sort_BYTES;
  x2 -= crypto_sort_BYTES;
  y2 -= crypto_sort_BYTES;
}

void predoit(void)
{
  long long i;
  for (i = 0;i < crypto_sort_BYTES * TUNE;++i) y[i] = myrandom();
}

void doit(void)
{
  memcpy(x,y,crypto_sort_BYTES * TUNE);
  crypto_sort(x,TUNE);
}

/* on big-endian machines, flip into little-endian */
/* other types of endianness are not supported */
static void endianness(unsigned char *e,unsigned long long len)
{
  long long i = 1;

  if (1 == *(unsigned char *) &i) return;

  while (len > 0) {
    for (i = 0;2 * i < crypto_sort_BYTES;++i) {
      long long j = crypto_sort_BYTES - 1 - i;
      unsigned char ei = e[i];
      e[i] = e[j];
      e[j] = ei;
    }
    e += crypto_sort_BYTES;
    len -= 1;
  }
}

void test(void)
{
  long long loop,bytes;
  
  for (loop = 0;loop < LOOPS;++loop) {
    len = myrandom() % (MAXTEST + 1);
    bytes = crypto_sort_BYTES * len;

    input_prepare(x2,x,bytes);
    output_prepare(y2,y,bytes);
    memcpy(y,x,bytes);
    endianness(y,len);
    poison(y,bytes);
    crypto_sort(y,len);
    unpoison(y,bytes);
    endianness(y,len);
    checksum(y,bytes);
    output_compare(y2,y,bytes,"crypto_sort");
    input_compare(x2,x,bytes,"crypto_sort");

    double_canary(y2,y,bytes);
    memcpy(y2,x,bytes);
    endianness(y2,len);
    poison(y2,bytes);
    crypto_sort(y2,len);
    unpoison(y2,bytes);
    endianness(y2,len);
    if (memcmp(y2,y,bytes) != 0) fail("crypto_sort is nondeterministic");

    double_canary(y2,y,bytes);
    endianness(y2,len);
    poison(y2,bytes);
    crypto_sort(y2,len);
    unpoison(y2,bytes);
    endianness(y2,len);
    if (memcmp(y2,y,bytes) != 0) fail("crypto_sort is not idempotent");
  }
}
