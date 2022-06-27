#include "crypto_verify.h"

int crypto_verify(const unsigned char *x,const unsigned char *y)
{
  unsigned int differentbits = 0;
  int i;

  for (i = 0;i < crypto_verify_BYTES;++i)
    differentbits |= x[i] ^ y[i];

  return (1 & ((differentbits - 1) >> 8)) - 1;
}
