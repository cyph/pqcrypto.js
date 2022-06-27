#include "crypto_core.h"

#include "crypto_int8.h"
#include "crypto_int16.h"
#define int8 crypto_int8
#define int16 crypto_int16
#include "crypto_encode_int16.h"

#include "params.h"

/* out = little-endian weight of bottom bits of in */
int crypto_core(unsigned char *outbytes,const unsigned char *inbytes,const unsigned char *kbytes,const unsigned char *cbytes)
{
  int8 *in = (void *) inbytes;
  int16 weight = 0;
  int i;

  for (i = 0;i < p;++i) weight += in[i]&1;
  crypto_encode_int16(outbytes,&weight);
  return 0;
}
