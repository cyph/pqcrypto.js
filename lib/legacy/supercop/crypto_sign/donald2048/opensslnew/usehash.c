#include "crypto_hash_sha256.h"
#include "crypto_sign.h"
#include "sizes.h"
#include "short.h"
#include <string.h>

int crypto_sign(
  unsigned char *sm,unsigned long long *smlen,
  const unsigned char *m,unsigned long long mlen,
  const unsigned char *sk
)
{
  unsigned char h[32];
  int i;

  crypto_hash_sha256(h,m,mlen);
  memmove(sm + SIGNATURE_BYTES,m,mlen);

  if (SHORTHASH_BYTES < 32) return -1;
  i = signatureofshorthash(sm,smlen,h,32,sk,SECRETKEY_BYTES);
  if (i < 0) return i;

  if (*smlen != SIGNATURE_BYTES) return -1;
  *smlen += mlen;
  return 0;
}

int crypto_sign_open(
  unsigned char *m,unsigned long long *mlen,
  const unsigned char *sm,unsigned long long smlen,
  const unsigned char *pk
)
{
  unsigned char h[32];
  int v;

  if (SHORTHASH_BYTES < 32) return -1;
  if (smlen < SIGNATURE_BYTES) return -100;
  crypto_hash_sha256(h,sm + SIGNATURE_BYTES,smlen - SIGNATURE_BYTES);
  v = verification(h,32,sm,SIGNATURE_BYTES,pk,PUBLICKEY_BYTES);
  if (v != 0) return v;
  memmove(m,sm + SIGNATURE_BYTES,smlen - SIGNATURE_BYTES);
  *mlen = smlen - SIGNATURE_BYTES;
  return 0;
}
