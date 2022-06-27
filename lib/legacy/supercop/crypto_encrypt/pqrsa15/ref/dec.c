#include <stdlib.h>
#include <libkeccak.a.headers/KeccakSpongeWidth1600.h>
#include "randombytes.h"
#include "gmp_import.h"
#include "gmp_export.h"
#include "crypto_verify_32.h"

#include "crypto_aead_aes256gcmv1.h"
#define crypto_aead_decrypt crypto_aead_aes256gcmv1_decrypt
#define crypto_aead_KEYBYTES crypto_aead_aes256gcmv1_KEYBYTES
#define crypto_aead_NPUBBYTES crypto_aead_aes256gcmv1_NPUBBYTES
#define crypto_aead_ABYTES crypto_aead_aes256gcmv1_ABYTES

#include "crypto_encrypt.h"
#include "internal.h"

#if crypto_encrypt_PUBLICKEYBYTES != K * PBYTES
#error "PUBLICKEYBYTES must be K * PBYTES"
#endif
#if crypto_encrypt_SECRETKEYBYTES != 3 * K * PBYTES
#error "SECRETKEYBYTES must be 3 * K * PBYTES"
#endif
#if crypto_encrypt_BYTES != K * PBYTES
#error "BYTES must be K * PBYTES"
#endif

#define MAXNBYTES crypto_encrypt_PUBLICKEYBYTES

/* num/den = x[0]/p[0] + x[1]/p[1] + ... */
/* except if x==0: num/den = 1/p[0] + 1/p[1] + ... */
static void sumfrac(mpz_t num,mpz_t den,const mpz_t *x,const mpz_t *p,long long plen)
{
  long long mid;
  mpz_t numleft,denleft,numright,denright;

  if (plen < 1) {
    mpz_set_ui(num,0);
    mpz_set_ui(den,1);
    return;
  }
  if (plen == 1) {
    if (x)
      mpz_set(num,x[0]);
    else
      mpz_set_ui(num,1);
    mpz_set(den,p[0]);
    return;
  }

  mpz_init(numleft);
  mpz_init(denleft);
  mpz_init(numright);
  mpz_init(denright);

  mid = plen / 2;
  sumfrac(numleft,denleft,x,p,mid);

  if (x) x += mid;
  p += mid;
  plen -= mid;
  sumfrac(numright,denright,x,p,plen);

  mpz_mul(numleft,numleft,denright);
  mpz_mul(numright,numright,denleft);
  mpz_add(num,numleft,numright);
  mpz_mul(den,denleft,denright);

  mpz_clear(numleft);
  mpz_clear(denleft);
  mpz_clear(numright);
  mpz_clear(denright);
}

/* for i in range(plen): xmodp[i] = x % p[i] */
static void remainders_via_tree(mpz_t *xmodp,const mpz_t x,const mpz_t *p,long long plen)
{
  mpz_t *q; /* p[0]p[1]; p[2]p[3]; etc. */
  mpz_t *xmodq;
  long long qlen, i;

  if (plen < 1) return;
  if (plen == 1) {
    mpz_mod(xmodp[0],x,p[0]);
    return;
  }

  qlen = (plen + 1) / 2;
  q = malloc(qlen * sizeof(mpz_t));
  if (!q) abort();
  xmodq = malloc(qlen * sizeof(mpz_t));
  if (!xmodq) abort();

  for (i = 0;i < qlen;++i) mpz_init(q[i]);
  for (i = 0;i < qlen;++i) mpz_init(xmodq[i]);

  for (i = 0;i < qlen;++i)
    if (i * 2 + 1 < plen)
      mpz_mul(q[i],p[i * 2],p[i * 2 + 1]);
    else
      mpz_set(q[i],p[i * 2]);

  remainders_via_tree(xmodq,x,q,qlen);

  for (i = 0;i < qlen;++i) {
    mpz_mod(xmodp[i * 2],xmodq[i],p[i * 2]);
    if (i * 2 + 1 < plen)
      mpz_mod(xmodp[i * 2 + 1],xmodq[i],p[i * 2 + 1]);
  }

  for (i = 0;i < qlen;++i) mpz_clear(q[i]);
  for (i = 0;i < qlen;++i) mpz_clear(xmodq[i]);
  free(q);
  free(xmodq);
}

static void remainders(mpz_t *xmodp,const mpz_t x,const mpz_t *p)
{
#ifdef SIMPLE
  long long i;
  for (i = 0;i < K;++i)
    mpz_mod(xmodp[i],x,p[i]);
#else
  long long plen = K;
  while (plen > 0) {
    long long tree = K / 8;
    if (plen < tree) tree = plen;
    remainders_via_tree(xmodp,x,p,tree);
    xmodp += tree;
    p += tree;
    plen -= tree;
  }
#endif
}

/* input: x */
/* input: p[0],p[1],...,p[K-1] */
/* input: pinv[0],pinv[1],...,pinv[K-1] */
/* input: N */
/* assumes: N = p[0] p[1] ... p[K-1] */
/* assumes: pinv[i] is reciprocal of N/p[i] mod p[i] */
/* output: x = cube root of original x mod N */
static int cuberoot(mpz_t x,const mpz_t *p,const mpz_t *pinv,const mpz_t N)
{
  mpz_t t;
  mpz_t *xmodp;
  long long i;
  int result = -1;

  xmodp = malloc(K * sizeof(mpz_t)); if (!xmodp) abort();

  mpz_init(t);
  for (i = 0;i < K;++i) mpz_init(xmodp[i]);

  remainders(xmodp,x,p);

#ifdef KAT
  for (i = 0;i < K;++i) {
    printf("intermediate p[%lld] = ",i); mpz_out_str(stdout,10,p[i]); printf("\n");
  }
  for (i = 0;i < K;++i) {
    printf("intermediate pinv[%lld] = ",i); mpz_out_str(stdout,10,pinv[i]); printf("\n");
  }
  printf("intermediate cube = "); mpz_out_str(stdout,10,x); printf("\n");
  for (i = 0;i < K;++i) {
    printf("intermediate cube mod p[%lld] = ",i); mpz_out_str(stdout,10,xmodp[i]); printf("\n");
  }
#endif

  for (i = 0;i < K;++i) {
    mpz_add(t,p[i],p[i]);
    mpz_sub_ui(t,t,1);
    mpz_divexact_ui(t,t,3);
    mpz_powm(xmodp[i],xmodp[i],t,p[i]);
  }

#ifdef KAT
  for (i = 0;i < K;++i) {
    printf("intermediate root mod p[%lld] = ",i); mpz_out_str(stdout,10,xmodp[i]); printf("\n");
  }
#endif

  for (i = 0;i < K;++i) mpz_mul(xmodp[i],xmodp[i],pinv[i]);
  for (i = 0;i < K;++i) mpz_mod(xmodp[i],xmodp[i],p[i]);

  sumfrac(x,t,xmodp,p,K);
  mpz_mod(x,x,N);

#ifdef KAT
  printf("intermediate root = "); mpz_out_str(stdout,10,x); printf("\n");
#endif

  mpz_clear(t);
  for (i = 0;i < K;++i) mpz_clear(xmodp[i]);

  free(xmodp);

  return result;
}

static const unsigned char nonce[crypto_aead_NPUBBYTES];
static const unsigned char ad[1];

int crypto_encrypt_open(
  unsigned char *m,unsigned long long *mlen,
  const unsigned char *c,unsigned long long clen,
  const unsigned char *sk
)
{
  mpz_t xint, N;
  mpz_t *p;
  mpz_t *pinv;
  unsigned char *x;
  long long i, nbytes, alpha;
  int result = 0;
  unsigned char r[33];
  unsigned char check[32];
  unsigned char k[32];
  unsigned char nsec[1];

  if (clen < MAXNBYTES) return -1;

  p = malloc(K * sizeof(mpz_t)); if (!p) abort();
  pinv = malloc(K * sizeof(mpz_t)); if (!pinv) abort();
  x = calloc(1,MAXNBYTES); if (!x) abort();

  mpz_init(xint);
  mpz_init(N);
  for (i = 0;i < K;++i) mpz_init(p[i]);
  for (i = 0;i < K;++i) mpz_init(pinv[i]);

  for (i = 0;i < K;++i) {
    gmp_import(p[i],sk,PBYTES);
    sk += PBYTES;
  }
  for (i = 0;i < K;++i) {
    gmp_import(pinv[i],sk,PBYTES);
    sk += PBYTES;
  }
  gmp_import(N,sk,MAXNBYTES);
  nbytes = mpz_sizeinbase(N,256);
  alpha = nbytes - 65;

  gmp_import(xint,c,MAXNBYTES);
  if (mpz_cmp(xint,N) >= 0) result = -1;

  cuberoot(xint,p,pinv,N);
  if (mpz_sizeinbase(xint,256) >= nbytes) result = -1;

  gmp_export(x,MAXNBYTES,xint);

  memcpy(check,x + alpha,32);

  for (i = 0;i < 32;++i) r[i] = x[alpha + 32 + i];
  x[alpha + 32] = 2;

  KeccakWidth1600_Sponge(1088,512,x,alpha + 33,0x1F,x + alpha + 32,32);
  for (i = 0;i < 32;++i) r[i] ^= x[alpha + 32 + i];

  r[32] = 0;
  KeccakWidth1600_Sponge(1088,512,r,33,0x1F,m,alpha);

  for (i = 0;i < alpha;++i) m[i] ^= x[i];
  for (i = 0;i < 32;++i) m[alpha + i] = r[i];
  m[alpha + 32] = 1;

  KeccakWidth1600_Sponge(1088,512,m,alpha + 33,0x1F,m + alpha + 32,32);
  result |= crypto_verify_32(check,m + alpha + 32);

  if (clen == MAXNBYTES) {
    for (i = alpha - 1;i >= 0;--i)
      if (m[i] != 0) {
        if (m[i] == 1) {
	  *mlen = i;
	  break;
	}
        if (result == 0) result = -1;
      }
    if (i < 0) result = -1;
  } else {
    if (m[alpha - 1] != 2) result = -1;
    memcpy(k,m + alpha - 33,32);
    if (crypto_aead_decrypt(m + alpha - 33,mlen,nsec,c + MAXNBYTES,clen - MAXNBYTES,ad,0,nonce,k) == -1) result = -1;
    *mlen += alpha - 33;
  }

  mpz_clear(xint);
  mpz_clear(N);
  for (i = 0;i < K;++i) mpz_clear(p[i]);
  for (i = 0;i < K;++i) mpz_clear(pinv[i]);

  free(p);
  free(pinv);
  free(x);

  return result;
}

static void randomprime(mpz_t p)
{
  for (;;) {
    unsigned char s[PBYTES];
    randombytes(s,sizeof s);
    s[0] |= 1; /* set bottom bit; i.e., force odd */
    s[sizeof s - 1] |= 0x80; /* set top bit */
    gmp_import(p,s,sizeof s);
    if (mpz_fdiv_ui(p,3) == 2)
      if (mpz_probab_prime_p(p,1) > 0)
        return;
  }
}

int crypto_encrypt_keypair(unsigned char *pk,unsigned char *sk)
{
  mpz_t q, N;
  mpz_t *p;
  mpz_t *qmodp;
  long long i;
  int result = 0;

  p = malloc(K * sizeof(mpz_t)); if (!p) abort();
  qmodp = malloc(K * sizeof(mpz_t)); if (!qmodp) abort();

  mpz_init(q);
  mpz_init(N);
  for (i = 0;i < K;++i) mpz_init(p[i]);
  for (i = 0;i < K;++i) mpz_init(qmodp[i]);

  for (i = 0;i < K;++i) {
    randomprime(p[i]);
    if (gmp_export(sk,PBYTES,p[i]) < 0) result = -1;
    sk += PBYTES;
  }

  sumfrac(q,N,0,p,K);
  remainders(qmodp,q,p);
  /* now qmodp[i] = (N/p[i]) mod p[i] */

  for (i = 0;i < K;++i) {
    mpz_invert(qmodp[i],qmodp[i],p[i]);
    if (gmp_export(sk,PBYTES,qmodp[i]) < 0) result = -1;
    sk += PBYTES;
  }

  if (gmp_export(pk,MAXNBYTES,N) < 0) result = -1;
  if (gmp_export(sk,MAXNBYTES,N) < 0) result = -1;
  sk += MAXNBYTES;

  mpz_clear(q);
  mpz_clear(N);
  for (i = 0;i < K;++i) mpz_clear(p[i]);
  for (i = 0;i < K;++i) mpz_clear(qmodp[i]);

  free(p);
  free(qmodp);

  return result;
}
