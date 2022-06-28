#include "params.h"

#include "randombytes.h"
#include "crypto_hash_sha512.h"
#include "crypto_sort_uint32.h"

#include "crypto_int8.h"
#include "crypto_int16.h"
#include "crypto_int32.h"
#include "crypto_uint16.h"
#include "crypto_uint32.h"
#define int8 crypto_int8
#define int16 crypto_int16
#define int32 crypto_int32
#define uint16 crypto_uint16
#define uint32 crypto_uint32

/* ----- masks */

/* return -1 if x!=0; else return 0 */
static int int16_nonzero_mask(int16 x)
{
  uint16 u = x; /* 0, else 1...65535 */
  uint32 v = u; /* 0, else 1...65535 */
  v = -v; /* 0, else 2^32-65535...2^32-1 */
  v >>= 31; /* 0, else 1 */
  return -v; /* 0, else -1 */
}

/* ----- arithmetic mod 3 */

typedef int8 small;
/* F3 is always represented as -1,0,1 */

/* works for -16384 <= x < 16384 */
static small F3_freeze(int16 x)
{
  return x-3*((10923*x+16384)>>15);
}

/* ----- arithmetic mod q */

typedef int16 Fq;
/* always represented as -(q-1)/2...(q-1)/2 */

/* works for -14000000 < x < 14000000 if q in 4591, 4621, 5167 */
static Fq Fq_freeze(int32 x)
{
  x -= q*((q18*x)>>18);
  x -= q*((q27*x+67108864)>>27);
  return x;
}

/* ----- small polynomials */

/* 0 if Weightw_is(r), else -1 */
static int Weightw_mask(small *r)
{
  int weight = 0;
  int i;

  for (i = 0;i < p;++i) weight += r[i]&1;
  return int16_nonzero_mask(weight-w);
}

/* R3_fromR(R_fromRq(r)) */
static void R3_fromRq(small *out,const Fq *r)
{
  int i;
  for (i = 0;i < p;++i) out[i] = F3_freeze(r[i]);
}

/* h = f*g in the ring R3 */
static void R3_mult(small *h,const small *f,const small *g)
{
  crypto_core_mult3((unsigned char *) h,(const unsigned char *) f,(const unsigned char *) g,0);
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

/* h = 3f in Rq */
static void Rq_mult3(Fq *h,const Fq *f)
{
  int i;
  
  for (i = 0;i < p;++i) h[i] = Fq_freeze(3*f[i]);
}

/* out = 1/(3*in) in Rq */
static void Rq_recip3(Fq *out,const small *in)
{ 
  unsigned char sout[2*p+1];
  crypto_core_inv(sout,(const unsigned char *) in,0,0);
  /* could check sout[2*p] for failure but, in context, inv always works */
  crypto_decode_pxint16(out,sout);
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
  int i;

  randombytes((unsigned char *) L,sizeof L);
  crypto_decode_pxint32(L,(unsigned char *) L);
  for (i = 0;i < w;++i) L[i] = L[i]&(uint32)-2;
  for (i = w;i < p;++i) L[i] = (L[i]&(uint32)-3)|1;
  crypto_sort_uint32(L,p);
  for (i = 0;i < p;++i) out[i] = (L[i]&3)-1;
}

static void Small_random(small *out)
{
  uint32 L[p];
  int i;

  randombytes((unsigned char *) L,sizeof L);
  crypto_decode_pxint32(L,(unsigned char *) L);
  for (i = 0;i < p;++i) out[i] = (((L[i]&0x3fffffff)*3)>>30)-1;
}

/* ----- Streamlined NTRU Prime */

typedef small Inputs[p]; /* passed by reference */
#define Ciphertexts_bytes Rounded_bytes
#define SecretKeys_bytes (2*Small_bytes)
#define PublicKeys_bytes Rq_bytes
#define Confirm_bytes 32

/* c,r_enc[1:] = Hide(r,pk,cache); cache is Hash4(pk) */
/* also overwrite r_enc[0] */
static void Hide(unsigned char *c,unsigned char *r_enc,const Inputs r,const unsigned char *pk,const unsigned char *cache)
{
  Fq h[p],hr[p];
  unsigned char x[1+Hash_bytes*2];
  int i;

  Small_encode(r_enc+1,r);
  Rq_decode(h,pk);
  Rq_mult_small(hr,h,r);
  Round_and_encode(c,hr);
  r_enc[0] = 3;
  Hash(x+1,r_enc,1+Small_bytes);
  for (i = 0;i < Hash_bytes;++i) x[1+Hash_bytes+i] = cache[i];
  x[0] = 2;
  Hash(c+Ciphertexts_bytes,x,sizeof x);
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
  Fq finv[p],h[p];
  small f[p],g[p],v[p+1];
  int i;

  for (;;) {
    Small_random(g);
    crypto_core_inv3((unsigned char *) v,(const unsigned char *) g,0,0);
    if (v[p] == 0) break;
  }
  Short_random(f);
  Rq_recip3(finv,f); /* always works */
  Rq_mult_small(h,finv,g);
  Rq_encode(pk,h);

  for (i = 0;i < PublicKeys_bytes;++i) sk[SecretKeys_bytes+i] = pk[i];
  sk[SecretKeys_bytes-1] = 4;
  Hash(sk+SecretKeys_bytes+PublicKeys_bytes+Small_bytes,sk+SecretKeys_bytes-1,1+PublicKeys_bytes);
  Small_encode(sk,f);
  Small_encode(sk+Small_bytes,v);
  randombytes(sk+SecretKeys_bytes+PublicKeys_bytes,Small_bytes);
  return 0;
}

int crypto_kem_enc(unsigned char *c,unsigned char *k,const unsigned char *pk)
{
  Inputs r;
  unsigned char r_enc[Small_bytes+1];
  unsigned char cache[Hash_bytes];
  unsigned char x[1+Hash_bytes+Ciphertexts_bytes+Confirm_bytes];
  unsigned char y[1+PublicKeys_bytes];
  int i;

  for (i = 0;i < PublicKeys_bytes;++i) y[1+i] = pk[i];
  y[0] = 4;
  Hash(cache,y,sizeof y);
  Short_random(r);
  Hide(c,r_enc,r,pk,cache);

  r_enc[0] = 3;
  Hash(x+1,r_enc,1+Small_bytes);
  for (i = 0;i < Ciphertexts_bytes+Confirm_bytes;++i) x[1+Hash_bytes+i] = c[i];
  x[0] = 1;
  Hash(k,x,sizeof x);

  return 0;
}

int crypto_kem_dec(unsigned char *k,const unsigned char *c,const unsigned char *sk)
{
  const unsigned char *pk = sk+SecretKeys_bytes;
  const unsigned char *rho = pk+PublicKeys_bytes;
  const unsigned char *cache = rho+Small_bytes;
  Inputs r;
  unsigned char r_enc[1+Small_bytes];
  unsigned char cnew[Ciphertexts_bytes+Confirm_bytes];
  small f[p],v[p],e[p],ev[p];
  Fq d[p],cf[p],cf3[p];
  unsigned char x[1+Hash_bytes+Ciphertexts_bytes+Confirm_bytes];
  int mask,i;

  Small_decode(f,sk);
  Small_decode(v,sk+Small_bytes);
  Rounded_decode(d,c);
  Rq_mult_small(cf,d,f);
  Rq_mult3(cf3,cf);
  R3_fromRq(e,cf3);
  R3_mult(ev,e,v);
  mask = Weightw_mask(ev); /* 0 if weight w, else -1 */
  for (i = 0;i < w;++i) r[i] = ((ev[i]^1)&~mask)^1;
  for (i = w;i < p;++i) r[i] = ev[i]&~mask;
  Hide(cnew,r_enc,r,pk,cache);
  mask = Ciphertexts_diff_mask(c,cnew);
  for (i = 0;i < Small_bytes;++i) r_enc[i+1] ^= mask&(r_enc[i+1]^rho[i]);
  r_enc[0] = 3;
  Hash(x+1,r_enc,1+Small_bytes);
  for (i = 0;i < Ciphertexts_bytes+Confirm_bytes;++i) x[1+Hash_bytes+i] = c[i];
  x[0] = 1+mask;
  Hash(k,x,sizeof x);
  return 0;
}
