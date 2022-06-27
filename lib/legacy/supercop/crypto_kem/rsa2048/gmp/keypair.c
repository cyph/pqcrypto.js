#include "gmp_import.h"
#include "gmp_export.h"
#include "randombytes.h"
#include "crypto_kem.h"

#if 2 * crypto_kem_SECRETKEYBYTES != 3 * crypto_kem_PUBLICKEYBYTES
#error "SECRETKEYBYTES must be 1.5 * PUBLICKEYBYTES"
#endif
#define NBYTES crypto_kem_PUBLICKEYBYTES
#define PBYTES (NBYTES / 2)

static void randomprime(mpz_t p)
{
  for (;;) {
    unsigned char s[PBYTES];
    randombytes(s,sizeof s);
    s[0] |= 1; /* set bottom bit; i.e., force odd */
    s[sizeof s - 1] |= 0xc0; /* set top two bits */
    gmp_import(p,s,sizeof s);
    if (mpz_fdiv_ui(p,3) == 2)
      if (mpz_probab_prime_p(p,1) > 0)
        return;
  }
}

int crypto_kem_keypair(unsigned char *pk,unsigned char *sk)
{
  mpz_t p, q, g, s, t;
  int result;

  mpz_init(p);
  mpz_init(q);
  mpz_init(g);
  mpz_init(s);
  mpz_init(t);

  for (;;) {
    randomprime(p);
    randomprime(q);
    mpz_gcdext(g,s,t,p,q);
    /* g = ps+qt, and g is gcd{p,q} */
    if (mpz_cmp_ui(g,1) == 0) break;
  }

  result = 0;
  mpz_mod(s,s,q);
  mpz_mul(t,p,q);

  if (gmp_export(sk,PBYTES,p) < 0) result = -1;
  if (gmp_export(sk + PBYTES,PBYTES,q) < 0) result = -1;
  if (gmp_export(sk + PBYTES * 2,PBYTES,s) < 0) result = -1;
  if (gmp_export(pk,NBYTES,t) < 0) result = -1;

  mpz_clear(t);
  mpz_clear(s);
  mpz_clear(g);
  mpz_clear(q);
  mpz_clear(p);

  return result;
}
