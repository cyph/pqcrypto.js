#include <string.h>
#include "kernelrandombytes.h"
#include "cpucycles.h"
#include "crypto_sort.h"
#include "measure.h"

const char *primitiveimplementation = crypto_sort_IMPLEMENTATION;
const char *implementationversion = crypto_sort_VERSION;
const char *sizenames[] = { "bytes", 0 };
const long long sizes[] = { crypto_sort_BYTES };

static unsigned char *x;
static unsigned char *y;

#define MAXTEST 65536

void preallocate(void)
{
}

void allocate(void)
{
  x = alignedcalloc(MAXTEST * crypto_sort_BYTES);
  y = alignedcalloc(MAXTEST * crypto_sort_BYTES);
}

#define TIMINGS 31
static long long cycles[TIMINGS + 1];

void measure(void)
{
  long long loop,len,i;

  for (loop = 0;loop < LOOPS;++loop) {
    for (len = 1;len <= MAXTEST;len += len) {
      kernelrandombytes(y,len * crypto_sort_BYTES);
      for (i = 0;i <= TIMINGS;++i) {
        cycles[i] = cpucycles();
        memcpy(x,y,len * crypto_sort_BYTES);
      }
      for (i = 0;i < TIMINGS;++i) cycles[i] = cycles[i + 1] - cycles[i];
      printentry(len,"copy_cycles",cycles,TIMINGS);
      for (i = 0;i <= TIMINGS;++i) {
        cycles[i] = cpucycles();
        memcpy(x,y,len * crypto_sort_BYTES);
        crypto_sort(x,len);
      }
      for (i = 0;i < TIMINGS;++i) cycles[i] = cycles[i + 1] - cycles[i];
      printentry(len,"cycles",cycles,TIMINGS);
    }
  }
}
