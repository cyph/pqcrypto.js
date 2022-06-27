#include <stddef.h>
#include <openssl/rand.h>
#include <openssl/bn.h>
#include <openssl/rsa.h>
#include <openssl/rc4.h>
#include <string.h>
#include "sizes.h"
#include "short.h"

int shortciphertext(
  unsigned char c[ENCRYPTION_BYTES],unsigned long long *clen,
  const unsigned char m[SHORTPLAINTEXT_BYTES],const unsigned long long mlen,
  const unsigned char pk[PUBLICKEY_BYTES],const unsigned long long pklen
)
{
  int result = 0;
  BIGNUM *n = BN_new();
  BIGNUM *e = BN_new();
  RSA *r = RSA_new();

  if (!n) result = -1;
  if (!e) result = -1;
  if (!r) result = -1;

  if (result == 0) if (!BN_bin2bn(pk,MODULUS_BYTES,n)) result = -1;
  if (result == 0) if (!BN_set_word(e,3)) result = -1;
  if (result == 0) if (!RSA_set0_key(r,n,e,0)) result = -1;
  if (result == 0) { n = 0; e = 0; }
  if (result == 0) if (RSA_size(r) > MODULUS_BYTES) result = -1;
  if (result == 0) if (RSA_public_encrypt(mlen,m,c,r,RSA_PKCS1_PADDING) != MODULUS_BYTES) result = -1;
  if (result == 0) *clen = MODULUS_BYTES;

  if (n) { BN_free(n); n = 0; }
  if (e) { BN_free(e); e = 0; }
  if (r) { RSA_free(r); r = 0; }

  return result;
}
