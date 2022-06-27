#include "crypto_hash_sha256.h"
#include "gmpxx_import.h"
#include "gmpxx_export.h"
#include "randombytes.h"
#include "crypto_kem.h"

#define NBYTES crypto_kem_PUBLICKEYBYTES

int crypto_kem_enc(
  unsigned char *c,
  unsigned char *k,
  const unsigned char *pk
)
{
  mpz_class n = gmpxx_import(pk,NBYTES);

  unsigned char mstr[NBYTES];
  mpz_class m;
  do {
    randombytes(mstr,NBYTES);
    m = gmpxx_import(mstr,NBYTES);
  } while (m >= n);
  crypto_hash_sha256(k,mstr,NBYTES);

  gmpxx_export(c,NBYTES,(m * m * m) % n);
  return 0;
}
