#include "crypto_encode.h"
#include <string.h>

void crypto_encode(unsigned char *s,const void *v)
{
  if (v != s)
    memcpy(s,v,crypto_encode_STRBYTES);
}
