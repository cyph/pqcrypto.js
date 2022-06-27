#include <stddef.h>
#include <openssl/objects.h>
#include <openssl/ecdsa.h>
#include <string.h>
#include "sizes.h"

int crypto_sign_keypair(unsigned char *pk,unsigned char *sk)
{
  int result = 0;
  BIGNUM *kx = 0;
  BIGNUM *ky = 0;
  EC_KEY *k = 0;
  EC_GROUP *group = 0;
  int len;

  memset(sk,0,SECRETKEY_BYTES);
  memset(pk,0,PUBLICKEY_BYTES);

  kx = BN_new();
  if (!kx) goto error;
  ky = BN_new();
  if (!ky) goto error;
  k = EC_KEY_new();
  if (!k) goto error;
  group = EC_GROUP_new_by_curve_name(NID);
  if (!group) goto error;

  if (!EC_KEY_set_group(k,group)) goto error;
  if (!EC_KEY_generate_key(k)) goto error;

  len = BN_num_bytes(EC_KEY_get0_private_key(k));
  if (len > PRIME_BYTES) goto error;
  if (!BN_bn2bin(EC_KEY_get0_private_key(k),sk + PRIME_BYTES - len)) goto error;
  sk += PRIME_BYTES;

#ifdef PRIME_FIELD
  if (!EC_POINT_get_affine_coordinates_GFp(group,EC_KEY_get0_public_key(k),kx,ky,0)) goto error;
#else
  if (!EC_POINT_get_affine_coordinates_GF2m(group,EC_KEY_get0_public_key(k),kx,ky,0)) goto error;
#endif

  len = BN_num_bytes(kx);
  if (len > PRIME_BYTES) goto error;
  if (!BN_bn2bin(kx,sk + PRIME_BYTES - len)) goto error;
  sk += PRIME_BYTES;
  if (!BN_bn2bin(kx,pk + PRIME_BYTES - len)) goto error;
  pk += PRIME_BYTES;

  len = BN_num_bytes(ky);
  if (len > PRIME_BYTES) goto error;
  if (!BN_bn2bin(ky,sk + PRIME_BYTES - len)) goto error;
  sk += PRIME_BYTES;
  if (!BN_bn2bin(ky,pk + PRIME_BYTES - len)) goto error;
  pk += PRIME_BYTES;

  goto cleanup;

  error:
  result = -1;

  cleanup:

  if (kx) { BN_free(kx); kx = 0; }
  if (ky) { BN_free(ky); ky = 0; }
  if (k) { EC_KEY_free(k); k = 0; }
  if (group) { EC_GROUP_free(group); group = 0; }
  return result;
}
