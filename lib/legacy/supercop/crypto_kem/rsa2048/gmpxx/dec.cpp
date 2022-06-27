#include "crypto_hash_sha256.h"
#include "gmpxx_import.h"
#include "gmpxx_export.h"
#include "crypto_kem.h"

#define NBYTES crypto_kem_PUBLICKEYBYTES
#define PBYTES (NBYTES / 2)

int crypto_kem_dec(
  unsigned char *k,
  const unsigned char *cstr,
  const unsigned char *sk
)
{
  mpz_class c = gmpxx_import(cstr,NBYTES);

  mpz_class p = gmpxx_import(sk,PBYTES);
  mpz_class q = gmpxx_import(sk + PBYTES,PBYTES);
  mpz_class pinv = gmpxx_import(sk + 2 * PBYTES,PBYTES);

  mpz_class dp = (2 * p - 1)/3;
  mpz_class mp; 
  mpz_powm(mp.get_mpz_t(),c.get_mpz_t(),dp.get_mpz_t(),p.get_mpz_t());

  mpz_class dq = (2 * q - 1)/3;
  mpz_class mq;
  mpz_powm(mq.get_mpz_t(),c.get_mpz_t(),dq.get_mpz_t(),q.get_mpz_t());

  mpz_class offset = mq - mp;
  while (offset < 0) offset += q;
  mpz_class m = mp + p * ((pinv * offset) % q);

  unsigned char mstr[NBYTES];
  gmpxx_export(mstr,NBYTES,m);

  crypto_hash_sha256(k,mstr,NBYTES);
  return 0;
}
