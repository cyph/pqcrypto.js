#include <stddef.h>
#include <openssl/bn.h>
#include <openssl/dh.h>
#include <string.h>
#include "prime.h"
#include "crypto_dh.h"

#define SECRETKEY_BYTES crypto_dh_SECRETKEYBYTES
#define PUBLICKEY_BYTES crypto_dh_PUBLICKEYBYTES
#define SHAREDSECRET_BYTES crypto_dh_BYTES

int crypto_dh(
  unsigned char *s,
  const unsigned char *pk,
  const unsigned char *sk
)
{
  int result = 0;
  DH *alice = 0;
  BIGNUM *bob = 0;
  BIGNUM *p = 0;
  BIGNUM *g = 0;
  BIGNUM *pub_key = 0;
  BIGNUM *priv_key = 0;
  int len;

  alice = DH_new(); if (!alice) goto error;

  p = BN_new(); if (!p) goto error;
  g = BN_new(); if (!g) goto error;

  if (!BN_bin2bn(prime,sizeof prime,p)) goto error;
  if (!BN_set_word(g,2)) goto error;

  if (!DH_set0_pqg(alice,p,0,g)) goto error;
  p = 0; g = 0;

  pub_key = BN_new(); if (!pub_key) goto error;
  priv_key = BN_new(); if (!priv_key) goto error;

  if (!BN_bin2bn(sk,PUBLICKEY_BYTES,pub_key)) goto error;
  if (!BN_bin2bn(sk + PUBLICKEY_BYTES,SECRETKEY_BYTES - PUBLICKEY_BYTES,priv_key)) goto error;

  if (!DH_set0_key(alice,pub_key,priv_key)) goto error;
  pub_key = 0; priv_key = 0;

  if (DH_size(alice) > SHAREDSECRET_BYTES) goto error;

  bob = BN_new(); if (!bob) goto error;
  if (!BN_bin2bn(pk,PUBLICKEY_BYTES,bob)) goto error;

  memset(s,0,SHAREDSECRET_BYTES);

  len = DH_compute_key(s,bob,alice);
  if (len < 0) goto error;
  /* OpenSSL documentation claims len is length of shared secret; it's actually 1 */

  goto cleanup;
  error: result = -1;
  cleanup:
  if (alice) { DH_free(alice); alice = 0; }
  if (bob) { BN_free(bob); bob = 0; }
  if (p) { BN_free(p); p = 0; }
  if (g) { BN_free(g); g = 0; }
  if (pub_key) { BN_free(pub_key); pub_key = 0; }
  if (priv_key) { BN_free(priv_key); priv_key = 0; }

  return result;
}
