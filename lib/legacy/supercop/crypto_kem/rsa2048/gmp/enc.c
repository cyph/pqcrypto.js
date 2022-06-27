#include "crypto_hash_sha256.h"
#include "gmp_import.h"
#include "gmp_export.h"
#include "randombytes.h"
#include "crypto_kem.h"

#define NBYTES crypto_kem_PUBLICKEYBYTES

int crypto_kem_enc(
  unsigned char *c,
  unsigned char *k,
  const unsigned char *pk
)
{
  mpz_t n, m;
  unsigned char mstr[NBYTES];
  int result;

  mpz_init(n);
  mpz_init(m);
  gmp_import(n,pk,NBYTES);

  do {
    randombytes(mstr,NBYTES);
    gmp_import(m,mstr,NBYTES);
  } while (mpz_cmp(m,n) >= 0);
  crypto_hash_sha256(k,mstr,NBYTES);

  mpz_pow_ui(m,m,3);
  mpz_mod(m,m,n);

  result = gmp_export(c,NBYTES,m);
  mpz_clear(m);
  mpz_clear(n);

  return result;
}
