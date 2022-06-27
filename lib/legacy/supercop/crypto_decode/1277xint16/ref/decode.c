#include "crypto_decode.h"
#include "crypto_uint16.h"

void crypto_decode(void *v,const unsigned char *s)
{
  crypto_uint16 *x = v;
  int i;

  for (i = 0;i < 1277;++i) {
    crypto_uint16 u0 = s[0];
    crypto_uint16 u1 = s[1];
    u1 <<= 8;
    *x = u0 | u1;
    x += 1;
    s += 2;
  }
}
