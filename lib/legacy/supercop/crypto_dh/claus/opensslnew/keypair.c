#include <stddef.h>
#include <openssl/bn.h>
#include <openssl/dh.h>
#include <string.h>
#include "prime.h"
#include "crypto_dh.h"

#define SECRETKEY_BYTES crypto_dh_SECRETKEYBYTES
#define PUBLICKEY_BYTES crypto_dh_PUBLICKEYBYTES

int crypto_dh_keypair(
  unsigned char *pk,
  unsigned char *sk
)
{
  int result = 0;
  BIGNUM *p = 0;
  BIGNUM *g = 0;
  DH *dh = 0;
  const BIGNUM *z;
  int len;

  memset(sk,0,SECRETKEY_BYTES);
  memset(pk,0,PUBLICKEY_BYTES);

  p = BN_new(); if (!p) goto error;
  g = BN_new(); if (!g) goto error;

  if (!BN_bin2bn(prime,sizeof prime,p)) goto error;
  if (!BN_set_word(g,2)) goto error;

  dh = DH_new(); if (!dh) goto error;

  if (!DH_set0_pqg(dh,p,0,g)) goto error;
  p = 0; g = 0;

  if (!DH_generate_key(dh)) goto error;

  z = DH_get0_pub_key(dh);
  if (!z) goto error;
  len = BN_num_bytes(z);
  if (len > PUBLICKEY_BYTES) goto error;
  if (!BN_bn2bin(z,pk + PUBLICKEY_BYTES - len)) goto error;
  if (!BN_bn2bin(z,sk + PUBLICKEY_BYTES - len)) goto error;

  z = DH_get0_priv_key(dh);
  if (!z) goto error;
  len = BN_num_bytes(z);
  if (len > SECRETKEY_BYTES - PUBLICKEY_BYTES) goto error;
  if (!BN_bn2bin(z,sk + SECRETKEY_BYTES - len)) goto error;

  goto cleanup;
  error: result = -1;
  cleanup:

  if (p) { BN_free(p); p = 0; }
  if (g) { BN_free(g); g = 0; }
  if (dh) { DH_free(dh); dh = 0; }

  return result;
}
