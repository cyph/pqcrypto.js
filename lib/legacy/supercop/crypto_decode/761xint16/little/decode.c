#include "crypto_decode.h"
#include <string.h>

void crypto_decode(void *v,const unsigned char *s)
{
  if (v != s)
    memcpy(v,s,crypto_decode_STRBYTES);
}
