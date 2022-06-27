#include "crypto_decode.h"
#include "Decode.h"
#define uint16 crypto_uint16

#define q 4621
#define q12 2310
#define p 653

void crypto_decode(void *v,const unsigned char *s)
{
  uint16 *r = v;
  uint16 R[p],M[p];
  int i;
  
  for (i = 0;i < p;++i) M[i] = q;
  Decode(R,s,M,p);
  for (i = 0;i < p;++i) r[i] = R[i]-q12;
}
