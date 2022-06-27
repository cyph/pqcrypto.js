#include "crypto_hash_sha256.h"
#include "ntl_import.h"
#include "ntl_export.h"
#include "randombytes.h"
#include "crypto_kem.h"

#define NBYTES crypto_kem_PUBLICKEYBYTES

int crypto_kem_enc(
  unsigned char *c,
  unsigned char *k,
  const unsigned char *pk
)
{
  ZZ n = ntl_import(pk,NBYTES);

  unsigned char mstr[NBYTES];
  ZZ m;
  do {
    randombytes(mstr,NBYTES);
    m = ntl_import(mstr,NBYTES);
  } while (m >= n);
  crypto_hash_sha256(k,mstr,NBYTES);

  ntl_export(c,NBYTES,PowerMod(m,3,n));
  return 0;
}
