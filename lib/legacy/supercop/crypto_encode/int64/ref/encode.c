#include "crypto_encode.h"
#include "crypto_uint64.h"

void crypto_encode(unsigned char *s,const void *x)
{
  crypto_uint64 u = *(const crypto_uint64 *) x;
  s[0] = u;
  s[1] = u >> 8;
  s[2] = u >> 16;
  s[3] = u >> 24;
  s[4] = u >> 32;
  s[5] = u >> 40;
  s[6] = u >> 48;
  s[7] = u >> 56;
}
