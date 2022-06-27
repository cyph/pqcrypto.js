#include <stddef.h>
#include <openssl/dsa.h>
#include <string.h>
#include "sizes.h"
#include "prime.h"

int crypto_sign_keypair(unsigned char *pk,unsigned char *sk)
{
  int result = 0;
  DSA *x = 0;
  BIGNUM *p = 0;
  BIGNUM *q = 0;
  BIGNUM *g = 0;
  const BIGNUM *z;
  int len;

  memset(sk,0,SECRETKEY_BYTES);
  memset(pk,0,PUBLICKEY_BYTES);

  x = DSA_new(); if (!x) goto error;
  p = BN_new(); if (!p) goto error;
  q = BN_new(); if (!q) goto error;
  g = BN_new(); if (!g) goto error;

  if (!BN_bin2bn(prime,sizeof prime,p)) goto error;
  if (!BN_bin2bn(prime_q,sizeof prime_q,q)) goto error;
  if (!BN_bin2bn(prime_g,sizeof prime_g,g)) goto error;

  if (!DSA_set0_pqg(x,p,q,g)) goto error;
  p = 0; q = 0; g = 0;

  if (!DSA_generate_key(x)) goto error;

  z = DSA_get0_pub_key(x);
  len = BN_num_bytes(z);
  if (len > PUBLICKEY_BYTES) goto error;
  if (!BN_bn2bin(z,pk + PUBLICKEY_BYTES - len)) goto error;
  if (!BN_bn2bin(z,sk + PUBLICKEY_BYTES - len)) goto error;

  z = DSA_get0_priv_key(x);
  len = BN_num_bytes(z);
  if (len > SECRETKEY_BYTES - PUBLICKEY_BYTES) goto error;
  if (!BN_bn2bin(z,sk + SECRETKEY_BYTES - len)) goto error;

  goto cleanup;

  error:
  result = -1;

  cleanup:
  if (x) { DSA_free(x); x = 0; }
  if (p) { BN_free(p); p = 0; }
  if (q) { BN_free(q); q = 0; }
  if (g) { BN_free(g); g = 0; }
  return result;
}
