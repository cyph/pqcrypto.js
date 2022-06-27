#include <string.h>
#include "crypto_verify.h"

int crypto_verify(const unsigned char *x,const unsigned char *y)
{
  if (memcmp(x,y,127)) return -1;
  return 0;
}
