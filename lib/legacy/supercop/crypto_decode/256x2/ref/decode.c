#include "crypto_decode.h"

void crypto_decode(void *v,const unsigned char *s)
{
  unsigned char *r = v;
  int i;
  for (i = 0;i < 256;++i) r[i] = 1&(s[i>>3]>>(i&7));
}
