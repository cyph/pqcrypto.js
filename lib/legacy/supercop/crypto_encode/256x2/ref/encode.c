#include "crypto_encode.h"

void crypto_encode(unsigned char *s,const void *v)
{
  const unsigned char *r = v;
  int i;
  for (i = 0;i < 32;++i) s[i] = 0;
  for (i = 0;i < 256;++i) s[i>>3] |= (r[i]&1)<<(i&7);
}
