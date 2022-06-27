#include "crypto_decode.h"
#include "crypto_uint32.h"

void crypto_decode(void *v,const unsigned char *s)
{
  crypto_uint32 *x = v;
  int i;

  for (i = 0;i < 1277;++i) {
    crypto_uint32 u0 = s[0];
    crypto_uint32 u1 = s[1];
    crypto_uint32 u2 = s[2];
    crypto_uint32 u3 = s[3];
    u1 <<= 8;
    u2 <<= 16;
    u3 <<= 24;
    *x = u0 | u1 | u2 | u3;
    x += 1;
    s += 4;
  }
}
