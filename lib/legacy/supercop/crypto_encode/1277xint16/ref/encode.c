#include "crypto_encode.h"
#include "crypto_uint16.h"

void crypto_encode(unsigned char *s,const void *v)
{
  const crypto_uint16 *x = v;
  int i;

  for (i = 0;i < 1277;++i) {
    crypto_uint16 u = *x++;
    *s++ = u;
    *s++ = u >> 8;
  }
}
