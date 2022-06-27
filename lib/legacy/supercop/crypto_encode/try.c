/*
 * crypto_encode/try.c version 20200816
 * D. J. Bernstein
 * Public domain.
 */

#include <string.h>
#include "crypto_encode.h"
#include "try.h"

const char *primitiveimplementation = crypto_encode_IMPLEMENTATION;

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

void preallocate(void)
{
}

void allocate(void)
{
  long long alloclen = crypto_encode_ITEMS * crypto_encode_ITEMBYTES + crypto_encode_ITEMBYTES + crypto_encode_STRBYTES;
  x = alignedcalloc(alloclen);
  y = alignedcalloc(alloclen);
  x2 = alignedcalloc(alloclen);
  y2 = alignedcalloc(alloclen);
}

void unalign(void)
{
  x += crypto_encode_ITEMBYTES;
  x2 += crypto_encode_ITEMBYTES;
  ++y;
  ++y2;
}

void realign(void)
{
  x -= crypto_encode_ITEMBYTES;
  x2 -= crypto_encode_ITEMBYTES;
  --y;
  --y2;
}

void predoit(void)
{
  long long i;
  for (i = 0;i < crypto_encode_ITEMS * crypto_encode_ITEMBYTES;++i) x[i] = myrandom();
}

void doit(void)
{
  crypto_encode(y,x);
}

/* on big-endian machines, flip into little-endian */
/* other types of endianness are not supported */
static void endianness(unsigned char *e,unsigned long long len)
{
  long long i = 1;

  if (1 == *(unsigned char *) &i) return;

  while (len > 0) {
    for (i = 0;2 * i < crypto_encode_ITEMBYTES;++i) {
      long long j = crypto_encode_ITEMBYTES - 1 - i;
      unsigned char ei = e[i];
      e[i] = e[j];
      e[j] = ei;
    }
    e += crypto_encode_ITEMBYTES;
    len -= 1;
  }
}

void test(void)
{
  long long loop;
  long long xbytes = crypto_encode_ITEMS * crypto_encode_ITEMBYTES;
  long long ybytes = crypto_encode_STRBYTES;
  
  for (loop = 0;loop < LOOPS;++loop) {
    input_prepare(x2,x,xbytes);
    output_prepare(y2,y,ybytes);
    endianness(x,crypto_encode_ITEMS);
    poison(x,xbytes);
    poison(y,ybytes);
    crypto_encode(y,x);
    unpoison(x,xbytes);
    unpoison(y,ybytes);
    checksum(y,ybytes);
    output_compare(y2,y,ybytes,"crypto_encode");
    input_compare(x2,x,xbytes,"crypto_encode");

    double_canary(x2,x,xbytes);
    double_canary(y2,y,ybytes);
    poison(x2,xbytes);
    poison(y2,ybytes);
    crypto_encode(y2,x2);
    unpoison(x2,xbytes);
    unpoison(y2,ybytes);
    if (memcmp(y2,y,ybytes) != 0) fail("crypto_encode is nondeterministic");
  }
}
