#include <cryptopp/chacha.h>
#include <cryptopp/algparam.h>
#include "crypto_stream.h"

using namespace CryptoPP;

int crypto_stream(
  unsigned char *out,
  unsigned long long outlen,
  const unsigned char *n,
  const unsigned char *k
)
{
  ChaCha::Encryption e;
  const AlgorithmParameters params =
    MakeParameters
      (Name::Rounds(),8)
      (Name::IV(),ConstByteArrayParameter(n,8));
  e.SetKey(k,32,params);
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
  ChaCha::Encryption e;
  const AlgorithmParameters params =
    MakeParameters
      (Name::Rounds(),8)
      (Name::IV(),ConstByteArrayParameter(n,8));
  e.SetKey(k,32,params);
  e.ProcessString(out,in,inlen);
  return 0;
}
