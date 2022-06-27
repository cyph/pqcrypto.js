#include <stddef.h>
#include <openssl/objects.h>
#include <openssl/md5.h>
#include <openssl/rsa.h>
#include <string.h>
#include "sizes.h"
#include "short.h"

int shortmessagesigned(
  unsigned char m[SHORTMESSAGE_BYTES],unsigned long long *mlen,
  const unsigned char sm[SIGNATURE_BYTES],const unsigned long long smlen,
  const unsigned char pk[PUBLICKEY_BYTES],const unsigned long long pklen
)
{
  int recoveredlen;
  RSA *r = RSA_new();
  BIGNUM *n = BN_new();
  BIGNUM *e = BN_new();
  int result = 0;

  if (!r) result = -1;
  if (!n) result = -1;
  if (!e) result = -1;

  if (!result) if (!BN_bin2bn(pk,MODULUS_BYTES,n)) result = -1;
  if (!result) if (!BN_set_word(e,3)) result = -1;
  if (!result) if (!RSA_set0_key(r,n,e,0)) result = -1;
  if (!result) { n = 0; e = 0; }
  if (!result) if (RSA_size(r) > MODULUS_BYTES) result = -1;
  if (!result) recoveredlen = RSA_public_decrypt(smlen,sm,m,r,RSA_PKCS1_PADDING);
  if (!result) if (recoveredlen < 0) result = -100;
  if (!result) *mlen = recoveredlen;

  if (n) { BN_free(n); n = 0; }
  if (e) { BN_free(e); e = 0; }
  if (r) { RSA_free(r); r = 0; }
  return result;
}
