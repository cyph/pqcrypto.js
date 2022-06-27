#include "crypto_decode.h"
#include "crypto_uint64.h"

void crypto_decode(void *x,const unsigned char *s)
{
  crypto_uint64 u0 = s[0];
  crypto_uint64 u1 = s[1];
  crypto_uint64 u2 = s[2];
  crypto_uint64 u3 = s[3];
  crypto_uint64 u4 = s[4];
  crypto_uint64 u5 = s[5];
  crypto_uint64 u6 = s[6];
  crypto_uint64 u7 = s[7];
  u1 <<= 8;
  u2 <<= 16;
  u3 <<= 24;
  u4 <<= 32;
  u5 <<= 40;
  u6 <<= 48;
  u7 <<= 56;
  *(crypto_uint64 *) x = u0 | u1 | u2 | u3 | u4 | u5 | u6 | u7;
}
