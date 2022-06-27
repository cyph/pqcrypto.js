#include "crypto_core.h"

#include "crypto_int16.h"
#define int16 crypto_int16
#include "crypto_decode_int16.h"

#include "params.h"

/* out = in if bottom bits of in have weight w */
/* otherwise out = (1,1,...,1,0,0,...,0) */
int crypto_core(unsigned char *out,const unsigned char *in,const unsigned char *kbytes,const unsigned char *cbytes)
{
  int16 weight;
  int16 mask;
  int i;

  crypto_core_weight((unsigned char *) &weight,in,0,0);
  crypto_decode_int16(&weight,(unsigned char *) &weight);

  mask = (weight-w)|(w-weight);
  mask >>= 15;
  mask = ~mask;

  for (i = 0;i < w;++i) out[i] = ((in[i]^1)&mask)^1;
  for (i = w;i < p;++i) out[i] = in[i]&mask;
  return 0;
}
