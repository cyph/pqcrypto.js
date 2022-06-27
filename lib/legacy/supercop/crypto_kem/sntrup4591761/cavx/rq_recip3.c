#include "crypto_kem.h" /* for namespacing */
#include <immintrin.h>
#include "params.h"
#include "rq.h"

#define v4591_16 _mm256_set1_epi16(4591)
#define v15631_16 _mm256_set1_epi16(15631)

/* caller must ensure that x-y does not overflow */
static inline int smaller_mask(int x,int y)
{
  return (x - y) >> 31;
}

static inline __m256i montproduct(__m256i x,__m256i y)
{
  __m256i lo,hi,d,e;

  lo = _mm256_mullo_epi16(x,y);
  hi = _mm256_mulhi_epi16(x,y);
  d = _mm256_mullo_epi16(lo,v15631_16);
  e = _mm256_mulhi_epi16(d,v4591_16);
  return _mm256_sub_epi16(hi,e);
}

static inline void vectormodq_swapeliminate(modq *f,modq *g,int len,const modq f0,const modq g0,int mask)
{
  __m256i f0vec = _mm256_set1_epi16(f0);
  __m256i g0vec = _mm256_set1_epi16(g0);
  __m256i maskvec = _mm256_set1_epi32(mask);
  
  while (len > 0) {
    __m256i fi = _mm256_loadu_si256((__m256i *) f);
    __m256i gi = _mm256_loadu_si256((__m256i *) g);
    __m256i finew = _mm256_blendv_epi8(fi,gi,maskvec);
    __m256i ginew = _mm256_blendv_epi8(gi,fi,maskvec);
    ginew = _mm256_sub_epi16(montproduct(ginew,f0vec),montproduct(finew,g0vec));
    _mm256_storeu_si256((__m256i *) f,finew);
    _mm256_storeu_si256((__m256i *) (g-1),ginew);
    f += 16;
    g += 16;
    len -= 16;
  }
}

static inline void vectormodq_xswapeliminate(modq *f,modq *g,int len,const modq f0,const modq g0,int mask)
{
  __m256i f0vec = _mm256_set1_epi16(f0);
  __m256i g0vec = _mm256_set1_epi16(g0);
  __m256i maskvec = _mm256_set1_epi32(mask);
  
  f += len + (-len & 15);
  g += len + (-len & 15);
  while (len > 0) {
    f -= 16;
    g -= 16;
    len -= 16;
    __m256i fi = _mm256_loadu_si256((__m256i *) f);
    __m256i gi = _mm256_loadu_si256((__m256i *) g);
    __m256i finew = _mm256_blendv_epi8(fi,gi,maskvec);
    __m256i ginew = _mm256_blendv_epi8(gi,fi,maskvec);
    ginew = _mm256_sub_epi16(montproduct(ginew,f0vec),montproduct(finew,g0vec));
    _mm256_storeu_si256((__m256i *) (f+1),finew);
    _mm256_storeu_si256((__m256i *) g,ginew);
  }
}

/*
out = (3s)^(-1) mod m, returning 0, if s is invertible mod m
or returning -1 if s is not invertible mod m
out,s are polys of degree <p
m is x^p-x-1
*/
int rq_recip3(modq *out,const small *s)
{
  int loop;
  modq f[769];
  modq g[769];
  modq v[769];
  modq r[769];
  modq f0;
  modq g0;
  modq c;
  int i;
  int delta = 1;
  int minusdelta;
  int fgflip;
  int swapmask;

  for (i = 0;i < 769;++i) f[i] = 0;
  f[0] = 1;
  f[p-1] = -1;
  f[p] = -1;
  /* generalization: initialize f to reversal of any deg-p polynomial m */

  for (i = 0;i < p;++i) g[i] = 3 * s[p-1-i];
  for (i = p;i < 769;++i) g[i] = 0;

  for (i = 0;i < 769;++i) r[i] = 0;
  r[0] = 1;

  for (i = 0;i < 769;++i) v[i] = 0;

  for (loop = 0;loop < p;++loop) {
    g0 = modq_freeze_short(g[0]);
    f0 = f[0];

    minusdelta = -delta;
    swapmask = smaller_mask(minusdelta,0) & modq_nonzero_mask(g0);

    delta ^= swapmask & (delta ^ minusdelta);
    delta += 1;

    fgflip = swapmask & (f0 ^ g0);
    f0 ^= fgflip;
    g0 ^= fgflip;

    f[0] = f0;

    vectormodq_swapeliminate(f+1,g+1,p,f0,g0,swapmask);
    vectormodq_xswapeliminate(v,r,loop+1,f0,g0,swapmask);
  }

  for (loop = p-1;loop > 0;--loop) {
    g0 = modq_freeze_short(g[0]);
    f0 = f[0];

    minusdelta = -delta;
    swapmask = smaller_mask(minusdelta,0) & modq_nonzero_mask(g0);

    delta ^= swapmask & (delta ^ minusdelta);
    delta += 1;

    fgflip = swapmask & (f0 ^ g0);
    f0 ^= fgflip;
    g0 ^= fgflip;

    f[0] = f0;

    vectormodq_swapeliminate(f+1,g+1,loop,f0,g0,swapmask);
    vectormodq_xswapeliminate(v,r,p,f0,g0,swapmask);
  }

  c = modq_reciprocal(modq_freeze(f[0]));
  for (i = 0;i < p;++i) out[i] = modq_product(modq_freeze(v[p-i]),c);
  for (i = p;i < 768;++i) out[i] = 0;
  return smaller_mask(0,delta);
}
