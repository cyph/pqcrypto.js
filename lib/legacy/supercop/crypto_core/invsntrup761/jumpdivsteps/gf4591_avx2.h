#ifndef _GF4591_AVX2_H_
#define _GF4591_AVX2_H_

#include <stdint.h>
#include <stdio.h>
#include <emmintrin.h>
#include <tmmintrin.h>
#include <immintrin.h>

#define v0 _mm256_setzero_si256()
#define v4591_16 _mm256_set1_epi16(4591)  // M
#define v15631_16 _mm256_set1_epi16(15631) // M^{-1} mod R
#define v15631_32 _mm256_set1_epi32(15631) // M^{-1} mod R
#define v4158_16 _mm256_set1_epi16(-433) // R^2 mod M
#define v1_16 _mm256_set1_epi16(1)
#define v1262_16 _mm256_set1_epi16(1262) // R mod M
#define v0_16 _mm256_set1_epi16(0)
#define v7_16 _mm256_set1_epi16(7)
#define v01 _mm256_set_epi16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1)
#define v_m1_0 _mm256_set_epi16(-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0)
#define v29234_16 _mm256_set1_epi16(29234)// floor(2^27/_QQ) 
#define v1024_16 _mm256_set1_epi16(1024)
#define v2295_16 _mm256_set1_epi16(2295)


static inline int smaller_mask(int x,int y)
{
  return (x - y) >> 31;
}

static inline __m256i montproduct(__m256i x,__m256i y)
// note : montproduct with v4158_16 to multiply by R=2^16
{
  __m256i lo,hi,d,e;

  lo = _mm256_mullo_epi16(x,y);
  hi = _mm256_mulhi_epi16(x,y);
  d = _mm256_mullo_epi16(lo,v15631_16);
  e = _mm256_mulhi_epi16(d,v4591_16);
  return _mm256_sub_epi16(hi,e);
}

static inline __m256i mont_divR(__m256i x)
// divide each entry in x by R=2^16
{
  __m256i hi,d,e;

  hi = _mm256_cmpgt_epi16(v0_16,x);
  d = _mm256_mullo_epi16(x,v15631_16);
  e = _mm256_mulhi_epi16(d,v4591_16);
  return _mm256_sub_epi16(hi,e);
}


// size of output < 2520
static inline __m256i barrett_fake(__m256i x) {
  __m256i d = _mm256_mulhrs_epi16 (x, v7_16);
  __m256i e = _mm256_mullo_epi16 (d, v4591_16);
  return _mm256_sub_epi16(x,e);
}

static inline __m256i barrett_real(__m256i x) {
  __m256i d = _mm256_mulhi_epi16 (x, v29234_16);
  __m256i t = _mm256_srai_epi16(_mm256_add_epi16(d,v1024_16),11);
  __m256i e = _mm256_mullo_epi16 (t, v4591_16);
  return _mm256_sub_epi16(x,e);
}

static inline __m256i center_adjust(__m256i x) {
  __m256i x_abs = _mm256_sign_epi16(x,x);
  __m256i p_pm = _mm256_sign_epi16(v4591_16,x);
  __m256i mask = _mm256_cmpgt_epi16(x_abs,v2295_16);
  __m256i diff = _mm256_and_si256(p_pm,mask);
  __m256i res = _mm256_sub_epi16(x,diff);
  return(res);
}


static inline __m256i __attribute__ ((always_inline)) uv_over (__m256i *uv) {
  return(_mm256_broadcastw_epi16(_mm256_castsi256_si128(_mm256_cmpeq_epi16(*uv,v01))));
}

static inline __m256i __attribute__ ((always_inline)) uv_sep (__m256i *uv) {
  __m256i temp = _mm256_broadcastw_epi16(_mm256_castsi256_si128(_mm256_cmpeq_epi16(*uv,v01)));
  *uv = _mm256_and_si256(v_m1_0,*uv);
  return(temp);
}

static inline void __attribute__ ((always_inline)) uv_replace (__m256i *v, __m256i m){
  *v = _mm256_or_si256(_mm256_and_si256(v01,m),*v);
}


//static const uint8_t __mask_coe[32] __attribute__((aligned(32))) = {0,1,8,9,2,3,10,11,4,5,12,13,6,7,14,15,0,1,8,9,2,3,10,11,4,5,12,13,6,7,14,15};
//static const __m256i * _ymm_mask_coe = (__m256i *) &__mask_coe[0];


static inline void print4 (int *f) {
  printf("(%d) + (%d) * x^2 + (%d) * x^4 + (%d) * x^6", f[0],f[1],f[2],f[3]);
}

static inline void print8 (short *b) {
  int i;
  printf("(%-5d) + (%-5d) * x",b[0],b[1]);
  for (i=2; i<8; i++) {
    if ((i&3)==0) printf ("\n ");
    printf ("+(%-5d) * x^%d",b[i],i);
  }
  printf("\n");
}






#endif
