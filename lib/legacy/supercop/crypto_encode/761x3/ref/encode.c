#include "crypto_encode.h"
#include "crypto_uint8.h"
#define uint8 crypto_uint8

#define p 761

void crypto_encode(unsigned char *s,const void *v)
{
  const uint8 *f = v;
  uint8 x;
  int i;

  for (i = 0;i < p/4;++i) {
    x = *f++ + 1;
    x += (*f++ + 1)<<2;
    x += (*f++ + 1)<<4;
    x += (*f++ + 1)<<6;
    *s++ = x;
  }
  x = *f++ + 1;
  *s++ = x;
}
