#include <stdlib.h>
#include <libkeccak.a.headers/KeccakSpongeWidth1600.h>
#include "gmp_import.h"
#include "gmp_export.h"
#include "crypto_sign.h"
#include "internal.h"

#define MAXNBYTES crypto_sign_PUBLICKEYBYTES

int crypto_sign_open(
  unsigned char *m,unsigned long long *mlen,
  const unsigned char *sm,unsigned long long smlen,
  const unsigned char *pk
)
{
  mpz_t N, X, Y;
  int result = -1;
  long long nbytes;
  unsigned char *ystr;

  *mlen = -1LL;
  if (smlen < MAXNBYTES + 32) return result;

  mpz_init(N);
  mpz_init(X);
  mpz_init(Y);
  ystr = malloc(MAXNBYTES); if (!ystr) abort();

  gmp_import(N,pk,MAXNBYTES);
  nbytes = mpz_sizeinbase(N,256);

  KeccakWidth1600_Sponge(1088,512,sm + MAXNBYTES,smlen - MAXNBYTES,0x1F,ystr,nbytes - 1);

  gmp_import(X,sm,MAXNBYTES);
  gmp_import(Y,ystr,nbytes - 1);

  if (mpz_cmp(X,N) < 0) {
    mpz_pow_ui(X,X,3);
    mpz_mod(X,X,N);
    if (mpz_cmp(X,Y) == 0) {
      result = 0;
      sm += MAXNBYTES + 32;
      smlen -= MAXNBYTES + 32;
      memmove(m,sm,smlen);
      *mlen = smlen;
    }
  }

  mpz_clear(N);
  mpz_clear(X);
  mpz_clear(Y);
  free(ystr);
  
  return result;
}
