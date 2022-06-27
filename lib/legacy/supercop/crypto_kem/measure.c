#include <stdlib.h>
#include "randombytes.h"
#include "cpucycles.h"
#include "crypto_kem.h"
#include "measure.h"

const char *primitiveimplementation = crypto_kem_IMPLEMENTATION;
const char *implementationversion = crypto_kem_VERSION;
const char *sizenames[] = { "publickeybytes", "secretkeybytes", "outputbytes", "ciphertextbytes", 0 };
const long long sizes[] = { crypto_kem_PUBLICKEYBYTES, crypto_kem_SECRETKEYBYTES, crypto_kem_BYTES, crypto_kem_CIPHERTEXTBYTES };

static unsigned char *p;
static unsigned char *s;
static unsigned char *k;
static unsigned char *c;
static unsigned char *t;

void preallocate(void)
{
#ifdef RAND_R_PRNG_NOT_SEEDED
  RAND_status();
#endif
}

void allocate(void)
{
  p = alignedcalloc(crypto_kem_PUBLICKEYBYTES);
  s = alignedcalloc(crypto_kem_SECRETKEYBYTES);
  k = alignedcalloc(crypto_kem_BYTES);
  c = alignedcalloc(crypto_kem_CIPHERTEXTBYTES);
  t = alignedcalloc(crypto_kem_BYTES);
}

#define TIMINGS 31
static long long cycles[TIMINGS + 1];
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
      crypto_kem_keypair(p,s);
    }
    for (i = 0;i < TIMINGS;++i) cycles[i] = cycles[i + 1] - cycles[i];
    for (i = 0;i < TIMINGS;++i) rbytes[i] = rbytes[i + 1] - rbytes[i];
    for (i = 0;i < TIMINGS;++i) rcalls[i] = rcalls[i + 1] - rcalls[i];
    printentry(-1,"keypair_cycles",cycles,TIMINGS);
    printentry(-1,"keypair_randombytes",rbytes,TIMINGS);
    printentry(-1,"keypair_randomcalls",rcalls,TIMINGS);
    for (i = 0;i <= TIMINGS;++i) {
      cycles[i] = cpucycles();
      rbytes[i] = randombytes_bytes;
      rcalls[i] = randombytes_calls;
      crypto_kem_enc(c,k,p);
    }
    for (i = 0;i < TIMINGS;++i) cycles[i] = cycles[i + 1] - cycles[i];
    for (i = 0;i < TIMINGS;++i) rbytes[i] = rbytes[i + 1] - rbytes[i];
    for (i = 0;i < TIMINGS;++i) rcalls[i] = rcalls[i + 1] - rcalls[i];
    printentry(-1,"enc_cycles",cycles,TIMINGS);
    printentry(-1,"enc_randombytes",rbytes,TIMINGS);
    printentry(-1,"enc_randomcalls",rcalls,TIMINGS);
    for (i = 0;i <= TIMINGS;++i) {
      cycles[i] = cpucycles();
      rbytes[i] = randombytes_bytes;
      rcalls[i] = randombytes_calls;
      crypto_kem_dec(t,c,s);
    }
    for (i = 0;i < TIMINGS;++i) cycles[i] = cycles[i + 1] - cycles[i];
    for (i = 0;i < TIMINGS;++i) rbytes[i] = rbytes[i + 1] - rbytes[i];
    for (i = 0;i < TIMINGS;++i) rcalls[i] = rcalls[i + 1] - rcalls[i];
    printentry(-1,"dec_cycles",cycles,TIMINGS);
    printentry(-1,"dec_randombytes",rbytes,TIMINGS);
    printentry(-1,"dec_randomcalls",rcalls,TIMINGS);
  }
}
