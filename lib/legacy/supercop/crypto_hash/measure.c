#include <stdlib.h>
#include "kernelrandombytes.h"
#include "cpucycles.h"
#include "crypto_hash.h"
#include "measure.h"

const char *primitiveimplementation = crypto_hash_IMPLEMENTATION;
const char *implementationversion = crypto_hash_VERSION;
const char *sizenames[] = { "outputbytes", 0 };
const long long sizes[] = { crypto_hash_BYTES };

#define MAXTEST_BYTES 65536
#define MSTEP 1008 /* 16*9*7 */
#ifdef SUPERCOP
#define MTRANSITION 4096
#define MGAP 8192
#else
#define MTRANSITION 65536
#define MGAP 8
#endif

static unsigned char *h;
static unsigned char *m;

void preallocate(void)
{
}

void allocate(void)
{
  h = alignedcalloc(crypto_hash_BYTES);
  m = alignedcalloc(MAXTEST_BYTES);
}

#define TIMINGS 15
static long long cycles[TIMINGS + 1];

static void printcycles(long long mlen)
{
  int i;
  for (i = 0;i < TIMINGS;++i) cycles[i] = cycles[i + 1] - cycles[i];
  printentry(mlen,"cycles",cycles,TIMINGS);
}

void measure(void)
{
  int i;
  int loop;
  int mlen;

  for (loop = 0;loop < LOOPS;++loop) {
    mlen = 0;
    while (mlen <= MAXTEST_BYTES) {
      kernelrandombytes(m,mlen);
      for (i = 0;i <= TIMINGS;++i) {
        cycles[i] = cpucycles();
	crypto_hash(h,m,mlen);
      }
      printcycles(mlen);
      if (mlen < MTRANSITION) {
        mlen += 1 + mlen / MGAP;
        if (mlen >= MTRANSITION) mlen = MTRANSITION;
      } else {
        mlen += MSTEP;
      }
    }
  }
}
