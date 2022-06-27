#include <cryptopp/salsa.h>
#include "crypto_stream.h"

int crypto_stream(
  unsigned char *out,
  unsigned long long outlen,
  const unsigned char *n,
  const unsigned char *k
)
{
  CryptoPP::Salsa20::Encryption e;
  e.SetKeyWithIV(k,32,n,8);
  e.GenerateBlock(out,outlen);
  return 0;
}

int crypto_stream_xor(
  unsigned char *out,
  const unsigned char *in,
  unsigned long long inlen,
  const unsigned char *n,
  const unsigned char *k
)
{
  CryptoPP::Salsa20::Encryption e;
  e.SetKeyWithIV(k,32,n,8);
  e.ProcessString(out,in,inlen);
  return 0;
}
