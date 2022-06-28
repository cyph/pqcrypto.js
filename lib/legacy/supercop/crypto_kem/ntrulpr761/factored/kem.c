#include "params.h"

#include "randombytes.h"
#include "crypto_hash_sha512.h"
#include "crypto_stream_aes256ctr.h"
#include "crypto_sort_uint32.h"

#include "crypto_int8.h"
#include "crypto_int16.h"
#include "crypto_int32.h"
#include "crypto_uint16.h"
#include "crypto_uint32.h"
#include "crypto_uint64.h"
#define int8 crypto_int8
#define int16 crypto_int16
#define int32 crypto_int32
#define uint16 crypto_uint16
#define uint32 crypto_uint32
#define uint64 crypto_uint64

/* ----- masks */

/* return -1 if x<0; otherwise return 0 */
static int int16_negative_mask(int16 x)
{
  uint16 u = x;
  u >>= 15;
  return -(int) u;
  /* alternative with gcc -fwrapv: */
  /* x>>15 compiles to CPU's arithmetic right shift */
}

/* ----- arithmetic mod 3 */

typedef int8 small;
/* F3 is always represented as -1,0,1 */

/* ----- arithmetic mod q */

#define q12 ((q-1)/2)
typedef int16 Fq;

/* works for -14000000 < x < 14000000 if q in 4591, 4621, 5167 */
/* assumes twos complement; use, e.g., gcc -fwrapv */
static Fq Fq_freeze(int32 x)
{
  x -= q*((q18*x)>>18);
  x -= q*((q27*x+67108864)>>27);
  return x;
}

/* works for all uint32 x */
static Fq Fq_bigfreeze(uint32 x)
{
  x -= q*((x*(uint64)q31)>>31);
  x -= q*((x*(uint64)q31)>>31);
  x -= q;
  x += (-(x>>31))&(uint32)q;
  return x;
}

/* ----- Top and Right */

static int8 Top(Fq C)
{
  return (tau1*(int32)(C+tau0)+16384)>>15;
}

static Fq Right(int8 T)
{
  return Fq_freeze(tau3*(int32)T-tau2);
}

/* ----- polynomials mod q */

/* h = f*g in the ring Rq */
static void Rq_mult_small(Fq *h,const Fq *f,const small *g)
{
  unsigned char hbytes[2*p];
  unsigned char fbytes[2*p];
  crypto_encode_pxint16(fbytes,f);
  crypto_core_mult(hbytes,fbytes,(const unsigned char *) g,0);
  crypto_decode_pxint16(h,hbytes);
}

/* ----- sorting to generate short polynomial */

static void Short_fromlist(small *out,const uint32 *in)
{
  uint32 L[p];
  int i;

  for (i = 0;i < w;++i) L[i] = in[i]&(uint32)-2;
  for (i = w;i < p;++i) L[i] = (in[i]&(uint32)-3)|1;
  crypto_sort_uint32(L,p);
  for (i = 0;i < p;++i) out[i] = (L[i]&3)-1;
}

/* ----- underlying hash function */

#define Hash_bytes 32

static void Hash(unsigned char *out,const unsigned char *in,int inlen)
{
  unsigned char h[64];
  int i;
  crypto_hash_sha512(h,in,inlen);
  for (i = 0;i < 32;++i) out[i] = h[i];
}

/* ----- higher-level randomness */

static void Short_random(small *out)
{
  uint32 L[p];

  randombytes((unsigned char *) L,sizeof L);
  crypto_decode_pxint32(L,(unsigned char *) L);
  Short_fromlist(out,L);
}

/* ----- Inputs, Expand, Generator */

typedef int8 Inputs[I]; /* passed by reference */

static const unsigned char aes_nonce[16] = {0};

static void Expand(uint32 *L,const unsigned char *k)
{
  crypto_stream_aes256ctr((unsigned char *) L,4*p,aes_nonce,k);
  crypto_decode_pxint32(L,(unsigned char *) L);
}

/* G = Generator(k) */
static void Generator(Fq *G,const unsigned char *k)
{
  uint32 L[p];
  int i;

  Expand(L,k);
  for (i = 0;i < p;++i) G[i] = Fq_bigfreeze(L[i])-q12;
}

/* ----- NTRU LPRime */

#define Seeds_bytes 32
#define Ciphertexts_bytes (Rounded_bytes+Top_bytes)
#define SecretKeys_bytes Small_bytes
#define PublicKeys_bytes (Seeds_bytes+Rounded_bytes)
#define Confirm_bytes 32

/* c,r_enc[1:] = Hide(r,pk,cache); cache is Hash4(pk) */
static void Hide(unsigned char *c,unsigned char *r_enc,const Inputs r,const unsigned char *pk,const unsigned char *cache)
{
  Fq A[p],G[p],bG[p],bA[p];
  int8 T[I];
  small b[p];
  int i;
  unsigned char s[1+Inputs_bytes];
  unsigned char h[Hash_bytes];
  uint32 L[p];
  unsigned char x[1+Inputs_bytes+Hash_bytes];

  Inputs_encode(r_enc+1,r);
  Rounded_decode(A,pk+Seeds_bytes);
  Generator(G,pk);
  Inputs_encode(s+1,r);
  s[0] = 5;
  Hash(h,s,sizeof s);
  Expand(L,h);
  Short_fromlist(b,L);
  Rq_mult_small(bG,G,b);
  Rq_mult_small(bA,A,b);
  for (i = 0;i < I;++i) T[i] = Top(Fq_freeze(bA[i]+r[i]*q12));
  Round_and_encode(c,bG); c += Rounded_bytes;
  Top_encode(c,T); c += Top_bytes;
  for (i = 0;i < Inputs_bytes;++i) x[1+i] = r_enc[1+i];
  for (i = 0;i < Hash_bytes;++i) x[1+Inputs_bytes+i] = cache[i];
  x[0] = 2;
  Hash(c,x,sizeof x);
}

/* 0 if matching ciphertext+confirm, else -1 */
static int Ciphertexts_diff_mask(const unsigned char *c,const unsigned char *c2)
{
  uint16 differentbits = 0;
  int len = Ciphertexts_bytes+Confirm_bytes;

  while (len-- > 0) differentbits |= (*c++)^(*c2++);
  return (1&((differentbits-1)>>8))-1;
}

#include "crypto_kem.h"

int crypto_kem_keypair(unsigned char *pk,unsigned char *sk)
{
  Fq G[p],aG[p];
  small a[p];
  int i;

  randombytes(pk,Seeds_bytes);
  Generator(G,pk);
  Short_random(a);
  Rq_mult_small(aG,G,a);
  Round_and_encode(pk+Seeds_bytes,aG);
  for (i = 0;i < PublicKeys_bytes;++i) sk[SecretKeys_bytes+i] = pk[i];
  sk[SecretKeys_bytes-1] = 4;
  Hash(sk+SecretKeys_bytes+PublicKeys_bytes+Inputs_bytes,sk+SecretKeys_bytes-1,1+PublicKeys_bytes);
  randombytes(sk+SecretKeys_bytes+PublicKeys_bytes,Inputs_bytes);
  Small_encode(sk,a);
  return 0;
}

int crypto_kem_enc(unsigned char *c,unsigned char *k,const unsigned char *pk)
{
  unsigned char s[Inputs_bytes];
  Inputs r;
  unsigned char cache[Hash_bytes];
  int i;
  unsigned char x[1+Inputs_bytes+Ciphertexts_bytes+Confirm_bytes];
  unsigned char y[1+PublicKeys_bytes];

  for (i = 0;i < PublicKeys_bytes;++i) y[1+i] = pk[i];
  y[0] = 4;
  Hash(cache,y,sizeof y);
  randombytes(s,sizeof s);
  Inputs_decode(r,s);
  Hide(c,x,r,pk,cache);
  for (i = 0;i < Ciphertexts_bytes+Confirm_bytes;++i) x[1+Inputs_bytes+i] = c[i];
  x[0] = 1;
  Hash(k,x,sizeof x);
  return 0;
}

int crypto_kem_dec(unsigned char *k,const unsigned char *c,const unsigned char *sk)
{
  const unsigned char *pk = sk+SecretKeys_bytes;
  const unsigned char *rho = pk+PublicKeys_bytes;
  const unsigned char *cache = rho+Inputs_bytes;
  Inputs r;
  unsigned char cnew[Ciphertexts_bytes+Confirm_bytes];
  int mask;
  int i;
  small a[p];
  Fq B[p],aB[p];
  int8 T[I];
  unsigned char x[1+Inputs_bytes+Ciphertexts_bytes+Confirm_bytes];

  Small_decode(a,sk);
  Rounded_decode(B,c);
  Top_decode(T,c+Rounded_bytes);
  Rq_mult_small(aB,B,a);
  for (i = 0;i < I;++i)
    r[i] = -int16_negative_mask(Fq_freeze(Right(T[i])-aB[i]+4*w+1));
  Hide(cnew,x,r,pk,cache);
  mask = Ciphertexts_diff_mask(c,cnew);
  for (i = 0;i < Inputs_bytes;++i) x[1+i] ^= mask&(x[1+i]^rho[i]);
  for (i = 0;i < Ciphertexts_bytes+Confirm_bytes;++i) x[1+Inputs_bytes+i] = c[i];
  x[0] = 1+mask;
  Hash(k,x,sizeof x);
  return 0;
}
