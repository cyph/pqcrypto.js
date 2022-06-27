#include "crypto_hash_sha256.h"
#include "ntl_import.h"
#include "ntl_export.h"
#include "crypto_kem.h"

#define NBYTES crypto_kem_PUBLICKEYBYTES
#define PBYTES (NBYTES / 2)

int crypto_kem_dec(
  unsigned char *k,
  const unsigned char *cstr,
  const unsigned char *sk
)
{
  ZZ c = ntl_import(cstr,NBYTES);

  ZZ p = ntl_import(sk,PBYTES);
  ZZ q = ntl_import(sk + PBYTES,PBYTES);
  ZZ pinv = ntl_import(sk + 2 * PBYTES,PBYTES);

  ZZ dp = (2 * p - 1)/3;
  ZZ mp; 
  mp = PowerMod(c % p,dp,p);

  ZZ dq = (2 * q - 1)/3;
  ZZ mq;
  mq = PowerMod(c % q,dq,q);

  ZZ offset = mq - mp;
  ZZ m = mp + p * ((pinv * offset) % q);

  unsigned char mstr[NBYTES];
  ntl_export(mstr,NBYTES,m);

  crypto_hash_sha256(k,mstr,NBYTES);
  return 0;
}
