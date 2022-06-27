#include <gmp.h>
#include "gmpxx_import.h"
#include "gmpxx_export.h"
#include "randombytes.h"
#include "crypto_kem.h"

#if 2 * crypto_kem_SECRETKEYBYTES != 3 * crypto_kem_PUBLICKEYBYTES
#error "SECRETKEYBYTES must be 1.5 * PUBLICKEYBYTES"
#endif
#define NBYTES crypto_kem_PUBLICKEYBYTES
#define PBYTES (NBYTES / 2)

mpz_class randomprime(void)
{
  for (;;) {
    unsigned char s[PBYTES];
    randombytes(s,sizeof s);
    s[0] |= 1; /* set bottom bit; i.e., force odd */
    s[sizeof s - 1] |= 0xc0; /* set top two bits */
    mpz_class p = gmpxx_import(s,sizeof s);
    if (p % 3 == 2)
      if (mpz_probab_prime_p(p.get_mpz_t(),1) > 0) return p;
  }
}

int crypto_kem_keypair(unsigned char *pk,unsigned char *sk)
{
  for (;;) {
    mpz_class p = randomprime();
    mpz_class q = randomprime();
    mpz_class g;
    mpz_class s;
    mpz_class t;
    mpz_gcdext(g.get_mpz_t(),s.get_mpz_t(),t.get_mpz_t(),p.get_mpz_t(),q.get_mpz_t());
    /* g = ps+qt, and g is gcd{p,q} */
    if (g == 1) {
      s %= q;
      if (s < 0) s += q;
      gmpxx_export(sk,PBYTES,p);
      gmpxx_export(sk + PBYTES,PBYTES,q);
      gmpxx_export(sk + PBYTES * 2,PBYTES,s);
      gmpxx_export(pk,NBYTES,p * q);
      return 0;
    }
  }
}
