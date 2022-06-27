#include <stdlib.h>
#include "kernelrandombytes.h"
#include "cpucycles.h"
#include "crypto_rng.h"
#include "measure.h"

const char *primitiveimplementation = crypto_rng_IMPLEMENTATION;
const char *implementationversion = crypto_rng_VERSION;
const char *sizenames[] = { "outputbytes", "keybytes", 0 };
const long long sizes[] = { crypto_rng_OUTPUTBYTES, crypto_rng_KEYBYTES };

static unsigned char *k;
static unsigned char *r;

void preallocate(void)
{
}

void allocate(void)
{
  k = alignedcalloc(crypto_rng_KEYBYTES);
  r = alignedcalloc(crypto_rng_OUTPUTBYTES);
}

#define TIMINGS 63
static long long cycles[TIMINGS + 1];

void measure(void)
{
  int i;
  int loop;

  for (loop = 0;loop < LOOPS;++loop) {
    kernelrandombytes(k,crypto_rng_KEYBYTES);
    for (i = 0;i <= TIMINGS;++i) {
      cycles[i] = cpucycles();
      crypto_rng(r,k,k);
    }
    for (i = 0;i < TIMINGS;++i) cycles[i] = cycles[i + 1] - cycles[i];
    printentry(-1,"cycles",cycles,TIMINGS);
  }
}
