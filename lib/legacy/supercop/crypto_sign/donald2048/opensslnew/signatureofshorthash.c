#include <stddef.h>
#include <openssl/dsa.h>
#include <openssl/sha.h>
#include <string.h>
#include "sizes.h"
#include "prime.h"
#include "short.h"

int signatureofshorthash(
    unsigned char sm[SIGNATURE_BYTES],unsigned long long *smlen,
    const unsigned char m[SHORTHASH_BYTES],const unsigned long long mlen,
    const unsigned char sk[SECRETKEY_BYTES],const unsigned long long sklen
    )
{
  int result = 0;
  unsigned char h[20];
  DSA *x = 0;
  DSA_SIG *y = 0;
  BIGNUM *p = 0;
  BIGNUM *q = 0;
  BIGNUM *g = 0;
  BIGNUM *pub_key = 0;
  BIGNUM *priv_key = 0;
  const BIGNUM *r = 0;
  const BIGNUM *s = 0;
  int len;

  if (mlen > SHORTHASH_BYTES) goto error;
  SHA1(m,mlen,h);

  p = BN_new(); if (!p) goto error;
  q = BN_new(); if (!q) goto error;
  g = BN_new(); if (!g) goto error;

  if (!BN_bin2bn(prime,sizeof prime,p)) goto error;
  if (!BN_bin2bn(prime_q,sizeof prime_q,q)) goto error;
  if (!BN_bin2bn(prime_g,sizeof prime_g,g)) goto error;

  x = DSA_new(); if (!x) goto error;

  if (!DSA_set0_pqg(x,p,q,g)) goto error;
  p = 0; q = 0; g = 0;

  pub_key = BN_new(); if (!pub_key) goto error;
  priv_key = BN_new(); if (!priv_key) goto error;

  if (!BN_bin2bn(sk,PUBLICKEY_BYTES,pub_key)) goto error;
  sk += PUBLICKEY_BYTES;
  if (!BN_bin2bn(sk,SECRETKEY_BYTES - PUBLICKEY_BYTES,priv_key)) goto error;

  if (!DSA_set0_key(x,pub_key,priv_key)) goto error;
  pub_key = 0; priv_key = 0;

  y = DSA_do_sign(h,20,x);
  if (!y) goto error;

  memset(sm,0,40);

  DSA_SIG_get0(y,&r,&s);

  len = BN_num_bytes(r);
  if (len > 20) goto error;
  if (!BN_bn2bin(r,sm + 20 - len)) goto error;

  len = BN_num_bytes(s);
  if (len > 20) goto error;
  if (!BN_bn2bin(s,sm + 40 - len)) goto error;
  *smlen = 40;

  goto cleanup;

  error: result = -1;

  cleanup:
  if (x) { DSA_free(x); x = 0; }
  if (y) { DSA_SIG_free(y); y = 0; }
  if (p) { BN_free(p); p = 0; }
  if (q) { BN_free(q); q = 0; }
  if (g) { BN_free(g); g = 0; }
  if (pub_key) { BN_free(pub_key); pub_key = 0; }
  if (priv_key) { BN_free(priv_key); priv_key = 0; }

  return result;
}
