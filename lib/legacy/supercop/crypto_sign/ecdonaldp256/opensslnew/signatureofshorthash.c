#include <stddef.h>
#include <openssl/objects.h>
#include <openssl/ecdsa.h>
#include <openssl/sha.h>
#include <string.h>
#include "sizes.h"
#include "short.h"

int signatureofshorthash(
    unsigned char sm[SIGNATURE_BYTES],unsigned long long *smlen,
    const unsigned char m[SHORTHASH_BYTES],const unsigned long long mlen,
    const unsigned char sk[SECRETKEY_BYTES],const unsigned long long sklen
    )
{
  int result = 0;
  unsigned char h[20];
  EC_GROUP *group = 0;
  BIGNUM *kx = 0;
  BIGNUM *ky = 0;
  EC_POINT *kxy = 0;
  EC_KEY *k = 0;
  ECDSA_SIG *rs = 0;
  const BIGNUM *z;
  int len;

  memset(sm,0,SIGNATURE_BYTES); *smlen = SIGNATURE_BYTES;

  if (mlen > SHORTHASH_BYTES) goto error;
  SHA1(m,mlen,h);

  group = EC_GROUP_new_by_curve_name(NID);
  if (!group) goto error;

  kx = BN_new(); if (!kx) goto error;
  ky = BN_new(); if (!ky) goto error;
  kxy = EC_POINT_new(group); if (!kxy) goto error;
  k = EC_KEY_new(); if (!k) goto error;

  if (!EC_KEY_set_group(k,group)) goto error;

  if (!BN_bin2bn(sk,PRIME_BYTES,kx)) goto error; sk += PRIME_BYTES;
  if (!EC_KEY_set_private_key(k,kx)) goto error;

  if (!BN_bin2bn(sk,PRIME_BYTES,kx)) goto error; sk += PRIME_BYTES;
  if (!BN_bin2bn(sk,PRIME_BYTES,ky)) goto error;
#ifdef PRIME_FIELD
  if (!EC_POINT_set_affine_coordinates_GFp(group,kxy,kx,ky,0)) goto error;
#else
  if (!EC_POINT_set_affine_coordinates_GF2m(group,kxy,kx,ky,0)) goto error;
#endif
  if (!EC_KEY_set_public_key(k,kxy)) goto error;

  rs = ECDSA_do_sign(h,20,k);
  if (!rs) goto error;

  z = ECDSA_SIG_get0_r(rs);
  len = BN_num_bytes(z); if (len > PRIME_BYTES) goto error;
  if (!BN_bn2bin(z,sm + PRIME_BYTES - len)) goto error;
  sm += PRIME_BYTES;

  z = ECDSA_SIG_get0_s(rs);
  len = BN_num_bytes(z); if (len > PRIME_BYTES) goto error;
  if (!BN_bn2bin(z,sm + PRIME_BYTES - len)) goto error;

  goto cleanup;

  error:
  result = -1;

  cleanup:
  if (group) { EC_GROUP_free(group); group = 0; }
  if (kx) { BN_free(kx); kx = 0; }
  if (ky) { BN_free(ky); ky = 0; }
  if (kxy) { EC_POINT_free(kxy); kxy = 0; }
  if (k) { EC_KEY_free(k); k = 0; }
  if (rs) { ECDSA_SIG_free(rs); rs = 0; }
  return result;
}
