#include "ntl_import.h"
#include "ntl_export.h"
#include "randombytes.h"
#include "crypto_kem.h"

#if 2 * crypto_kem_SECRETKEYBYTES != 3 * crypto_kem_PUBLICKEYBYTES
#error "SECRETKEYBYTES must be 1.5 * PUBLICKEYBYTES"
#endif
#define NBYTES crypto_kem_PUBLICKEYBYTES
#define PBYTES (NBYTES / 2)

ZZ randomprime(void)
{
  for (;;) {
    unsigned char s[PBYTES];
    randombytes(s,sizeof s);
    s[0] |= 1; /* set bottom bit; i.e., force odd */
    s[sizeof s - 1] |= 0xc0; /* set top two bits */
    ZZ p = ntl_import(s,sizeof s);
    if (p % 3 == 2)
      if (ProbPrime(p,1)) return p;
  }
}

int crypto_kem_keypair(unsigned char *pk,unsigned char *sk)
{
  for (;;) {
    ZZ p = randomprime();
    ZZ q = randomprime();
    ZZ g;
    ZZ s;
    ZZ t;
    XGCD(g,s,t,p,q);
    /* g = ps+qt, and g is gcd{p,q} */
    if (g == 1) {
      s %= q;
      if (s < 0) s += q;
      ntl_export(sk,PBYTES,p);
      ntl_export(sk + PBYTES,PBYTES,q);
      ntl_export(sk + PBYTES * 2,PBYTES,s);
      ntl_export(pk,NBYTES,p * q);
      return 0;
    }
  }
}
