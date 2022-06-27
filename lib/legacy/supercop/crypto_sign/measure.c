#include <stdlib.h>
#include "randombytes.h"
#include "cpucycles.h"
#include "crypto_sign.h"
#include "measure.h"

const char *primitiveimplementation = crypto_sign_IMPLEMENTATION;
const char *implementationversion = crypto_sign_VERSION;
const char *sizenames[] = { "outputbytes", "publickeybytes", "secretkeybytes", 0 };
const long long sizes[] = { crypto_sign_BYTES, crypto_sign_PUBLICKEYBYTES, crypto_sign_SECRETKEYBYTES };

#define MAXTEST_BYTES 100000

#define TIMINGS 15

static unsigned char *pk[TIMINGS + 1];
static unsigned char *sk[TIMINGS + 1];
static unsigned char *m[TIMINGS + 1]; unsigned long long mlen;
static unsigned char *sm[TIMINGS + 1]; unsigned long long smlen[TIMINGS + 1];
static unsigned char *t[TIMINGS + 1]; unsigned long long tlen;

void preallocate(void)
{
#ifdef RAND_R_PRNG_NOT_SEEDED
  RAND_status();
#endif
}

void allocate(void)
{
  int i;
  for (i = 0;i <= TIMINGS;++i) {
    pk[i] = alignedcalloc(crypto_sign_PUBLICKEYBYTES);
    sk[i] = alignedcalloc(crypto_sign_SECRETKEYBYTES);
    m[i] = alignedcalloc(MAXTEST_BYTES + crypto_sign_BYTES);
    sm[i] = alignedcalloc(MAXTEST_BYTES + crypto_sign_BYTES);
    t[i] = alignedcalloc(MAXTEST_BYTES + crypto_sign_BYTES);
  }
}

static long long cycles[TIMINGS + 1];
static long long bytes[TIMINGS + 1];
static long long rbytes[TIMINGS + 1];
static long long rcalls[TIMINGS + 1];

void measure(void)
{
  int i;
  int loop;

  for (loop = 0;loop < LOOPS;++loop) {
    for (i = 0;i <= TIMINGS;++i) {
      cycles[i] = cpucycles();
      rbytes[i] = randombytes_bytes;
      rcalls[i] = randombytes_calls;
      crypto_sign_keypair(pk[i],sk[i]);
    }
    for (i = 0;i < TIMINGS;++i) cycles[i] = cycles[i + 1] - cycles[i];
    for (i = 0;i < TIMINGS;++i) rbytes[i] = rbytes[i + 1] - rbytes[i];
    for (i = 0;i < TIMINGS;++i) rcalls[i] = rcalls[i + 1] - rcalls[i];
    printentry(-1,"keypair_cycles",cycles,TIMINGS);
    printentry(-1,"keypair_randombytes",rbytes,TIMINGS);
    printentry(-1,"keypair_randomcalls",rcalls,TIMINGS);

    for (mlen = 0;mlen <= MAXTEST_BYTES;mlen += 1 + mlen / 4) {
      for (i = 0;i <= TIMINGS;++i)
        randombytes(m[i],mlen);

      for (i = 0;i <= TIMINGS;++i) {
        cycles[i] = cpucycles();
        rbytes[i] = randombytes_bytes;
        rcalls[i] = randombytes_calls;
        bytes[i] = crypto_sign(sm[i],&smlen[i],m[i],mlen,sk[i]);
	if (bytes[i] == 0) bytes[i] = smlen[i];
      }
      for (i = 0;i < TIMINGS;++i) cycles[i] = cycles[i + 1] - cycles[i];
      for (i = 0;i < TIMINGS;++i) rbytes[i] = rbytes[i + 1] - rbytes[i];
      for (i = 0;i < TIMINGS;++i) rcalls[i] = rcalls[i + 1] - rcalls[i];
      printentry(mlen,"cycles",cycles,TIMINGS);
      printentry(mlen,"bytes",bytes,TIMINGS);
      printentry(mlen,"randombytes",rbytes,TIMINGS);
      printentry(mlen,"randomcalls",rcalls,TIMINGS);

      for (i = 0;i <= TIMINGS;++i) {
        cycles[i] = cpucycles();
        rbytes[i] = randombytes_bytes;
        rcalls[i] = randombytes_calls;
        bytes[i] = crypto_sign_open(t[i],&tlen,sm[i],smlen[i],pk[i]);
	if (bytes[i] == 0) bytes[i] = tlen;
      }
      for (i = 0;i < TIMINGS;++i) cycles[i] = cycles[i + 1] - cycles[i];
      for (i = 0;i < TIMINGS;++i) rbytes[i] = rbytes[i + 1] - rbytes[i];
      for (i = 0;i < TIMINGS;++i) rcalls[i] = rcalls[i + 1] - rcalls[i];
      printentry(mlen,"open_cycles",cycles,TIMINGS);
      printentry(mlen,"open_bytes",bytes,TIMINGS);
      printentry(mlen,"open_randombytes",rbytes,TIMINGS);
      printentry(mlen,"open_randomcalls",rcalls,TIMINGS);
    }
  }
}
