#include <openssl/objects.h>
#include <openssl/ecdsa.h>
#include <openssl/sha.h>
#include <string.h>
#include "sizes.h"
#include "short.h"

int verification(
    const unsigned char m[SHORTHASH_BYTES],const unsigned long long mlen,
    const unsigned char sm[SIGNATURE_BYTES],const unsigned long long smlen,
    const unsigned char pk[PUBLICKEY_BYTES],const unsigned long long pklen
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
  BIGNUM *r = 0;
  BIGNUM *s = 0;
  int len;

  if (smlen != SIGNATURE_BYTES) goto error;
  if (mlen > SHORTHASH_BYTES) goto error;
  SHA1(m,mlen,h);

  group = EC_GROUP_new_by_curve_name(NID);
  if (!group) goto error;

  kx = BN_new(); if (!kx) goto error;
  ky = BN_new(); if (!ky) goto error;
  kxy = EC_POINT_new(group); if (!kxy) goto error;
  k = EC_KEY_new(); if (!k) goto error;

  if (!EC_KEY_set_group(k,group)) goto error;
  if (!BN_bin2bn(pk,PRIME_BYTES,kx)) goto error; pk += PRIME_BYTES;
  if (!BN_bin2bn(pk,PRIME_BYTES,ky)) goto error;
#ifdef PRIME_FIELD
  if (!EC_POINT_set_affine_coordinates_GFp(group,kxy,kx,ky,0)) goto error;
#else
  if (!EC_POINT_set_affine_coordinates_GF2m(group,kxy,kx,ky,0)) goto error;
#endif
  if (!EC_KEY_set_public_key(k,kxy)) goto error;

  r = BN_new(); if (!r) goto error;
  s = BN_new(); if (!s) goto error;
  if (!BN_bin2bn(sm,PRIME_BYTES,r)) goto error; sm += PRIME_BYTES;
  if (!BN_bin2bn(sm,PRIME_BYTES,s)) goto error;

  rs = ECDSA_SIG_new(); if (!rs) goto error;
  if (!ECDSA_SIG_set0(rs,r,s)) goto error;
  r = 0; s = 0;

  len = ECDSA_do_verify(h,20,rs,k);
  if (len != 1) {
    if (len != 0) goto error;
    result = -100;
  }
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
  if (r) { BN_free(r); r = 0; }
  if (s) { BN_free(s); s = 0; }
  return result;
}
