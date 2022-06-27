#include <stddef.h>
#include <openssl/dsa.h>
#include <openssl/sha.h>
#include "sizes.h"
#include "prime.h"
#include "short.h"

int verification(
    const unsigned char m[SHORTHASH_BYTES],const unsigned long long mlen,
    const unsigned char sm[SIGNATURE_BYTES],const unsigned long long smlen,
    const unsigned char pk[PUBLICKEY_BYTES],const unsigned long long pklen
    )
{
  int result = 0;
  unsigned char h[20];
  BIGNUM *p = 0;
  BIGNUM *q = 0;
  BIGNUM *g = 0;
  BIGNUM *pub_key = 0;
  BIGNUM *r = 0;
  BIGNUM *s = 0;
  DSA *x = 0;
  DSA_SIG *y = 0;

  if (smlen != 40) goto error;
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
  if (!BN_bin2bn(pk,PUBLICKEY_BYTES,pub_key)) goto error;

  if (!DSA_set0_key(x,pub_key,0)) goto error;
  pub_key = 0;

  r = BN_new(); if (!r) goto error;
  s = BN_new(); if (!s) goto error;

  if (!BN_bin2bn(sm,20,r)) goto error;
  if (!BN_bin2bn(sm + 20,20,s)) goto error;

  y = DSA_SIG_new(); if (!y) goto error;

  if (!DSA_SIG_set0(y,r,s)) goto error;
  r = 0; s = 0;

  switch(DSA_do_verify(h,20,y,x)) {
    case 0: result = -100; break;
    case 1: break;
    default: goto error;
  }

  goto cleanup;

  error:
  result = -1;

  cleanup:
  if (p) { BN_free(p); p = 0; }
  if (q) { BN_free(q); q = 0; }
  if (g) { BN_free(g); g = 0; }
  if (pub_key) { BN_free(pub_key); pub_key = 0; }
  if (r) { BN_free(r); r = 0; }
  if (s) { BN_free(s); s = 0; }
  if (x) { DSA_free(x); x = 0; }
  if (y) { DSA_SIG_free(y); y = 0; }

  return result;
}
