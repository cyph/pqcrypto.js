#include "crypto_encode.h"
#include "crypto_uint32.h"

void crypto_encode(unsigned char *s,const void *x)
{
  crypto_uint32 u = *(const crypto_uint32 *) x;
  s[0] = u;
  s[1] = u >> 8;
  s[2] = u >> 16;
  s[3] = u >> 24;
}
