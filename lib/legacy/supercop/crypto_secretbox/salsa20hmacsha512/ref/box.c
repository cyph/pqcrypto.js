#include "crypto_auth_hmacsha512256.h"
#include "crypto_stream_salsa20.h"
#include "crypto_secretbox.h"
#include "crypto_declassify.h"

int crypto_secretbox(
  unsigned char *c,
  const unsigned char *m,unsigned long long mlen,
  const unsigned char *n,
  const unsigned char *k
)
{
  if (mlen < 32) return -1;
  crypto_stream_salsa20_xor(c,m,mlen,n,k);
  return crypto_auth_hmacsha512256(c,c + 32,mlen - 32,c);
}

int crypto_secretbox_open(
  unsigned char *m,
  const unsigned char *c,unsigned long long clen,
  const unsigned char *n,
  const unsigned char *k
)
{
  int i;
  int result;
  unsigned char subkey[32];

  if (clen < 32) return -1;
  crypto_stream_salsa20(subkey,32,n,k);
  result = crypto_auth_hmacsha512256_verify(c,c + 32,clen - 32,subkey);
  crypto_declassify(&result,sizeof result);
  if (result != 0) return -1;
  crypto_stream_salsa20_xor(m,c,clen,n,k);
  for (i = 0;i < 32;++i) m[i] = 0;
  return 0;
}
