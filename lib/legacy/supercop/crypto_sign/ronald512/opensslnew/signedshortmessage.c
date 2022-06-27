#include <stddef.h>
#include <openssl/objects.h>
#include <openssl/md5.h>
#include <openssl/rsa.h>
#include <string.h>
#include "sizes.h"
#include "short.h"

int signedshortmessage(
  unsigned char sm[SIGNATURE_BYTES],unsigned long long *smlen,
  const unsigned char m[SHORTMESSAGE_BYTES],const unsigned long long mlen,
  const unsigned char sk[SECRETKEY_BYTES],const unsigned long long sklen
)
{
  int result = 0;
  BIGNUM *n = BN_new();
  BIGNUM *e = BN_new();
  BIGNUM *d = BN_new();
  BIGNUM *p = BN_new();
  BIGNUM *q = BN_new();
  BIGNUM *dmp1 = BN_new();
  BIGNUM *dmq1 = BN_new();
  BIGNUM *iqmp = BN_new();
  RSA *r = RSA_new();
  int len;

  if (!n) result = -1;
  if (!e) result = -1;
  if (!d) result = -1;
  if (!p) result = -1;
  if (!q) result = -1;
  if (!dmp1) result = -1;
  if (!dmq1) result = -1;
  if (!iqmp) result = -1;
  if (!r) result = -1;

  if (!result) { if (!BN_bin2bn(sk,MODULUS_BYTES,n)) result = -1; sk += MODULUS_BYTES; }
  if (!result) { if (!BN_bin2bn(sk,MODULUS_BYTES,e)) result = -1; sk += MODULUS_BYTES; }
  if (!result) { if (!BN_bin2bn(sk,MODULUS_BYTES,d)) result = -1; sk += MODULUS_BYTES; }
  if (!result) { if (!BN_bin2bn(sk,MODULUS_BYTES,p)) result = -1; sk += MODULUS_BYTES; }
  if (!result) { if (!BN_bin2bn(sk,MODULUS_BYTES,q)) result = -1; sk += MODULUS_BYTES; }
  if (!result) { if (!BN_bin2bn(sk,MODULUS_BYTES,dmp1)) result = -1; sk += MODULUS_BYTES; }
  if (!result) { if (!BN_bin2bn(sk,MODULUS_BYTES,dmq1)) result = -1; sk += MODULUS_BYTES; }
  if (!result) { if (!BN_bin2bn(sk,MODULUS_BYTES,iqmp)) result = -1; sk += MODULUS_BYTES; }

  if (!result) if (!RSA_set0_key(r,n,e,d)) result = -1;
  if (!result) { n = 0; e = 0; d = 0; }
  if (!result) if (!RSA_set0_factors(r,p,q)) result = -1;
  if (!result) { p = 0; q = 0; }
  if (!result) if (!RSA_set0_crt_params(r,dmp1,dmq1,iqmp)) result = -1;
  if (!result) { dmp1 = 0; dmq1 = 0; iqmp = 0; }
  if (!result) if (RSA_size(r) != MODULUS_BYTES) result = -1;

  if (!result) if (mlen > 1000000000) result = -1;
  if (!result) if (RSA_private_encrypt(mlen,m,sm,r,RSA_PKCS1_PADDING) != MODULUS_BYTES) result = -1;
  if (!result) *smlen = MODULUS_BYTES;

  if (n) { BN_free(n); n = 0; }
  if (e) { BN_free(e); e = 0; }
  if (d) { BN_free(d); d = 0; }
  if (p) { BN_free(p); p = 0; }
  if (q) { BN_free(q); q = 0; }
  if (dmp1) { BN_free(dmp1); dmp1 = 0; }
  if (dmq1) { BN_free(dmq1); dmq1 = 0; }
  if (iqmp) { BN_free(iqmp); iqmp = 0; }
  if (r) { RSA_free(r); r = 0; }

  return result;
}
