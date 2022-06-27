#include <stdlib.h>
#include <libkeccak.a.headers/KeccakSpongeWidth1600.h>
#include "gmp_import.h"
#include "gmp_export.h"
#include "randombytes.h"
#include "crypto_kem.h"
#include "internal.h"

#define MAXNBYTES crypto_kem_PUBLICKEYBYTES

int crypto_kem_enc(
  unsigned char *c,
  unsigned char *k,
  const unsigned char *pk
)
{
  mpz_t N, r;
  unsigned char *rstr;
  int result;
  long long nbytes;
  long long i;

  mpz_init(N);
  mpz_init(r);
  rstr = malloc(MAXNBYTES);

  gmp_import(N,pk,MAXNBYTES);
  nbytes = mpz_sizeinbase(N,256);

  randombytes(rstr,MAXNBYTES);
  for (i = nbytes - 1;i < MAXNBYTES;++i) rstr[i] = 0;

  KeccakWidth1600_Sponge(1088,512,rstr,MAXNBYTES,0x1F,k,32);

  gmp_import(r,rstr,MAXNBYTES);
  mpz_pow_ui(r,r,3);
  mpz_mod(r,r,N);

  result = gmp_export(c,MAXNBYTES,r);

  mpz_clear(r);
  mpz_clear(N);
  free(rstr);

  return result;
}
