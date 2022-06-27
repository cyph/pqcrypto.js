#include <string.h>
#include "kernelrandombytes.h"
#include "cpucycles.h"
#include "crypto_encode.h"
#include "measure.h"

const char *primitiveimplementation = crypto_encode_IMPLEMENTATION;
const char *implementationversion = crypto_encode_VERSION;
const char *sizenames[] = { "strbytes", "itembytes", "items", 0 };
const long long sizes[] = { crypto_encode_STRBYTES, crypto_encode_ITEMBYTES, crypto_encode_ITEMS };

static unsigned char *x;
static unsigned char *y;

void preallocate(void)
{
}

void allocate(void)
{
  x = alignedcalloc(crypto_encode_ITEMS * crypto_encode_ITEMBYTES);
  y = alignedcalloc(crypto_encode_STRBYTES);
}

#define TIMINGS 31
static long long cycles[TIMINGS + 1];

void measure(void)
{
  long long loop,i;

  for (loop = 0;loop < LOOPS;++loop) {
    kernelrandombytes(x,crypto_encode_ITEMS * crypto_encode_ITEMBYTES);
    for (i = 0;i <= TIMINGS;++i) {
      cycles[i] = cpucycles();
      crypto_encode(y,x);
    }
    for (i = 0;i < TIMINGS;++i) cycles[i] = cycles[i + 1] - cycles[i];
    printentry(-1,"cycles",cycles,TIMINGS);
  }
}
