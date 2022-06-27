#include "crypto_decode.h"
#include "crypto_uint16.h"

void crypto_decode(void *x,const unsigned char *s)
{
  crypto_uint16 u0 = s[0];
  crypto_uint16 u1 = s[1];
  u1 <<= 8;
  *(crypto_uint16 *) x = u0 | u1;
}
