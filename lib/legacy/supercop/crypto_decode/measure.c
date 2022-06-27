#include <string.h>
#include "kernelrandombytes.h"
#include "cpucycles.h"
#include "crypto_decode.h"
#include "measure.h"

const char *primitiveimplementation = crypto_decode_IMPLEMENTATION;
const char *implementationversion = crypto_decode_VERSION;
const char *sizenames[] = { "strbytes", "itembytes", "items", 0 };
const long long sizes[] = { crypto_decode_STRBYTES, crypto_decode_ITEMBYTES, crypto_decode_ITEMS };

static unsigned char *x;
static unsigned char *y;

void preallocate(void)
{
}

void allocate(void)
{
  x = alignedcalloc(crypto_decode_STRBYTES);
  y = alignedcalloc(crypto_decode_ITEMS * crypto_decode_ITEMBYTES);
}

#define TIMINGS 31
static long long cycles[TIMINGS + 1];

void measure(void)
{
  long long loop,i;

  for (loop = 0;loop < LOOPS;++loop) {
    kernelrandombytes(x,crypto_decode_STRBYTES);
    for (i = 0;i <= TIMINGS;++i) {
      cycles[i] = cpucycles();
      crypto_decode(y,x);
    }
    for (i = 0;i < TIMINGS;++i) cycles[i] = cycles[i + 1] - cycles[i];
    printentry(-1,"cycles",cycles,TIMINGS);
  }
}
