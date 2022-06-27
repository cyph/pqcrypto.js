#include "crypto_hash_sha256.h"
#include "gmp_import.h"
#include "gmp_export.h"
#include "crypto_kem.h"

#define NBYTES crypto_kem_PUBLICKEYBYTES
#define PBYTES (NBYTES / 2)

int crypto_kem_dec(
  unsigned char *k,
  const unsigned char *cstr,
  const unsigned char *sk
)
{
  mpz_t x, p, q, pinv, mp, mq;
  unsigned char mstr[NBYTES];

  mpz_init(x);
  mpz_init(p);
  mpz_init(q);
  mpz_init(pinv);
  mpz_init(mp);
  mpz_init(mq);

  gmp_import(x,cstr,NBYTES);
  gmp_import(p,sk,PBYTES);
  gmp_import(q,sk + PBYTES,PBYTES);
  gmp_import(pinv,sk + 2 * PBYTES,PBYTES);

  mpz_add(mp,p,p);
  mpz_sub_ui(mp,mp,1);
  mpz_fdiv_q_ui(mp,mp,3);
  mpz_powm(mp,x,mp,p);

  mpz_add(mq,q,q);
  mpz_sub_ui(mq,mq,1);
  mpz_fdiv_q_ui(mq,mq,3);
  mpz_powm(mq,x,mq,q);

  mpz_sub(x,mq,mp);
  mpz_mul(x,x,pinv);
  mpz_mod(x,x,q);
  mpz_mul(x,x,p);
  mpz_add(x,x,mp);

  gmp_export(mstr,NBYTES,x);
  crypto_hash_sha256(k,mstr,NBYTES);

  mpz_clear(mq);
  mpz_clear(mp);
  mpz_clear(pinv);
  mpz_clear(q);
  mpz_clear(p);
  mpz_clear(x);

  return 0;
}
