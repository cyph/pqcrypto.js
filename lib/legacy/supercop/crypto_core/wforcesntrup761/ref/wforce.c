#include "crypto_core.h"

#include "crypto_int8.h"
#include "crypto_int16.h"
#include "crypto_uint16.h"
#include "crypto_uint32.h"
#define int8 crypto_int8
#define int16 crypto_int16
#define uint16 crypto_uint16
#define uint32 crypto_uint32

typedef int8 small;

#include "params.h"

/* return -1 if x!=0; else return 0 */
static int int16_nonzero_mask(int16 x)
{
  uint16 u = x; /* 0, else 1...65535 */
  uint32 v = u; /* 0, else 1...65535 */
  v = -v; /* 0, else 2^32-65535...2^32-1 */
  v >>= 31; /* 0, else 1 */
  return -v; /* 0, else -1 */
}

/* 0 if Weightw_is(r), else -1 */
static int Weightw_mask(const small *r)
{
  int weight = 0;
  int i;

  for (i = 0;i < p;++i) weight += r[i]&1;
  return int16_nonzero_mask(weight-w);
}

/* out = in if bottom bits of in have weight w */
/* otherwise out = (1,1,...,1,0,0,...,0) */
int crypto_core(unsigned char *outbytes,const unsigned char *inbytes,const unsigned char *kbytes,const unsigned char *cbytes)
{
  small *out = (void *) outbytes;
  const small *in = (const void *) inbytes;
  int i,mask;

  mask = Weightw_mask(in); /* 0 if weight w, else -1 */
  for (i = 0;i < w;++i) out[i] = ((in[i]^1)&~mask)^1;
  for (i = w;i < p;++i) out[i] = in[i]&~mask;
  return 0;
}
