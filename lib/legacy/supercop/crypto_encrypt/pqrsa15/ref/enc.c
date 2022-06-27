#include <stdlib.h>
#include <string.h>
#include <libkeccak.a.headers/KeccakSpongeWidth1600.h>
#include "gmp_import.h"
#include "gmp_export.h"
#include "randombytes.h"

#include "crypto_aead_aes256gcmv1.h"
#define crypto_aead_encrypt crypto_aead_aes256gcmv1_encrypt
#define crypto_aead_KEYBYTES crypto_aead_aes256gcmv1_KEYBYTES
#define crypto_aead_NPUBBYTES crypto_aead_aes256gcmv1_NPUBBYTES
#define crypto_aead_ABYTES crypto_aead_aes256gcmv1_ABYTES

#include "crypto_encrypt.h"
#include "internal.h"

#define MAXNBYTES crypto_encrypt_PUBLICKEYBYTES

static const unsigned char nonce[crypto_aead_NPUBBYTES];
static const unsigned char nsec[1];
static const unsigned char ad[1];

int crypto_encrypt(
  unsigned char *c,unsigned long long *clen,
  const unsigned char *m,const unsigned long long mlen,
  const unsigned char *pk
)
{
  mpz_t N, xint;
  long long nbytes, alpha, i;
  unsigned char k[32];
  unsigned char r[33];
  unsigned char *x;

  x = calloc(1,MAXNBYTES); if (!x) abort();
  /* with more work can do everything in place, even with c = m */

  mpz_init(N);
  mpz_init(xint);

  gmp_import(N,pk,MAXNBYTES);
  nbytes = mpz_sizeinbase(N,256);
  alpha = nbytes - 65;

  if (mlen < alpha) {
    memcpy(x,m,mlen);
    x[mlen] = 1;
    *clen = MAXNBYTES;
  } else {
    randombytes(k,32);
    memcpy(x,m,alpha - 33);
    memcpy(x + alpha - 33,k,32);
    x[alpha - 1] = 2;
    crypto_aead_encrypt(c + MAXNBYTES,clen,m + alpha - 33,mlen - (alpha - 33),ad,0,nsec,nonce,k);
    *clen += MAXNBYTES;
  }

  randombytes(r,32);

  memcpy(x + alpha,r,32);
  x[alpha + 32] = 1;
  KeccakWidth1600_Sponge(1088,512,x,alpha + 33,0x1F,x + alpha,32);

  r[32] = 0;
  KeccakWidth1600_Sponge(1088,512,r,33,0x1F,c,alpha);

  for (i = 0;i < alpha;++i) x[i] ^= c[i];

  x[alpha + 32] = 2;
  KeccakWidth1600_Sponge(1088,512,x,alpha + 33,0x1F,x + alpha + 32,32);

  for (i = 0;i < 32;++i) x[alpha + 32 + i] ^= r[i];

  gmp_import(xint,x,alpha + 64);
  mpz_pow_ui(xint,xint,3);
  mpz_mod(xint,xint,N);

  gmp_export(c,MAXNBYTES,xint);

  mpz_clear(N);
  mpz_clear(xint);
  free(x);

  return 0;
}
