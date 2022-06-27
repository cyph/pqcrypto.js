#include "crypto_onetimeauth.h"
#include "kernelrandombytes.h"
#include "cpucycles.h"
#include "measure.h"

const char *primitiveimplementation = crypto_onetimeauth_IMPLEMENTATION;
const char *implementationversion = crypto_onetimeauth_VERSION;
const char *sizenames[] = { "outputbytes", "keybytes", 0 };
const long long sizes[] = { crypto_onetimeauth_BYTES, crypto_onetimeauth_KEYBYTES };

#define MAXTEST_BYTES 4096
#ifdef SUPERCOP
#define MGAP 8192
#else
#define MGAP 8
#endif

static unsigned char *k;
static unsigned char *m;
static unsigned char *h;

void preallocate(void)
{
}

void allocate(void)
{
  k = alignedcalloc(crypto_onetimeauth_KEYBYTES);
  m = alignedcalloc(MAXTEST_BYTES);
  h = alignedcalloc(crypto_onetimeauth_BYTES);
}

#define TIMINGS 15
static long long cycles[TIMINGS + 1];

void measure(void)
{
  int i;
  int loop;
  int mlen;

  for (loop = 0;loop < LOOPS;++loop) {
    for (mlen = 0;mlen <= MAXTEST_BYTES;mlen += 1 + mlen / MGAP) {
      kernelrandombytes(k,crypto_onetimeauth_KEYBYTES);
      kernelrandombytes(m,mlen);
      kernelrandombytes(h,crypto_onetimeauth_BYTES);
      for (i = 0;i <= TIMINGS;++i) {
        cycles[i] = cpucycles();
	crypto_onetimeauth(h,m,mlen,k);
      }
      for (i = 0;i < TIMINGS;++i) cycles[i] = cycles[i + 1] - cycles[i];
      printentry(mlen,"cycles",cycles,TIMINGS);
      for (i = 0;i <= TIMINGS;++i) {
        cycles[i] = cpucycles();
	crypto_onetimeauth_verify(h,m,mlen,k);
      }
      for (i = 0;i < TIMINGS;++i) cycles[i] = cycles[i + 1] - cycles[i];
      printentry(mlen,"verify_cycles",cycles,TIMINGS);
    }
  }
}
