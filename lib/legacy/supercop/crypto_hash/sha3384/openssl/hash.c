#include <stddef.h>
#include <openssl/evp.h>
#include "crypto_hash.h"

int crypto_hash(unsigned char *out,const unsigned char *in,unsigned long long inlen)
{
  int ok = 1;
  EVP_MD_CTX *x;
  unsigned int outlen;

  x = EVP_MD_CTX_create();
  if (!x) return -111;

  if (EVP_MD_size(EVP_sha3_384()) != crypto_hash_BYTES) ok = 0;
  if (ok) ok = EVP_DigestInit_ex(x,EVP_sha3_384(),NULL);
  if (ok) ok = EVP_DigestUpdate(x,in,inlen);
  if (ok) ok = EVP_DigestFinal(x,out,&outlen);

  EVP_MD_CTX_destroy(x);
  if (!ok) return -111;
  return 0;
}
