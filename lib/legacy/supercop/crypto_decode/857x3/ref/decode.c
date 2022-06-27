#include "crypto_decode.h"
#include "crypto_uint8.h"
#define uint8 crypto_uint8

#define p 857

void crypto_decode(void *v,const unsigned char *s)
{
  uint8 *f = v;
  uint8 x;
  int i;

  for (i = 0;i < p/4;++i) {
    x = *s++;
    *f++ = ((uint8)(x&3))-1; x >>= 2;
    *f++ = ((uint8)(x&3))-1; x >>= 2;
    *f++ = ((uint8)(x&3))-1; x >>= 2;
    *f++ = ((uint8)(x&3))-1;
  }
  x = *s++;
  *f++ = ((uint8)(x&3))-1;
}
