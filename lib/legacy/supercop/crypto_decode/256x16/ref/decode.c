#include "crypto_decode.h"

void crypto_decode(void *v,const unsigned char *s)
{
  unsigned char *T = v;
  int i;
  for (i = 0;i < 128;++i) {
    T[2*i] = s[i]&15;
    T[2*i+1] = s[i]>>4;
  }
}
