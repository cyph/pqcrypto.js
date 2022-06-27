#include "crypto_core.h"
#include "rq.h"

#include "params.h"



int crypto_core(unsigned char *outbytes,const unsigned char *inbytes,const unsigned char *kbytes,const unsigned char *cbytes)
{
  int delta = rq_recip3((modq*)outbytes, (const small *)inbytes);
  crypto_encode_pxint16(outbytes,outbytes);
  outbytes[2*p] = delta;
  return 0;
}
