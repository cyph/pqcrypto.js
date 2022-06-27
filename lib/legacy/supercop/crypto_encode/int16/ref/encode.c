#include "crypto_encode.h"
#include "crypto_uint16.h"

void crypto_encode(unsigned char *s,const void *x)
{
  crypto_uint16 u = *(const crypto_uint16 *) x;
  s[0] = u;
  s[1] = u >> 8;
}
