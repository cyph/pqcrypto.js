#ifndef _POLYMUL_32X32_H_
#define _POLYMUL_32X32_H_

#include <immintrin.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "polymul_8x8.h"

#define gf_polymul_32x32_avx2_divR CRYPTO_NAMESPACE(gf_polymul_32x32_avx2_divR)
#define gf_polymul_32x32_avx2  CRYPTO_NAMESPACE(gf_polymul_32x32_avx2)
#define gf_polymul_32x32_avx2_uvqr_fg  CRYPTO_NAMESPACE(gf_polymul_32x32_avx2_uvqr_fg)
#define gf_polymul_32x32_avx2_uvqr_vr  CRYPTO_NAMESPACE(gf_polymul_32x32_avx2_uvqr_vr)

#if 1

// recurisive karatsuba.
// 656k haswell cycles for a full polyinv().
static inline
void gf_polymul_32x32_avx2_divR(__m256i *c, __m256i *a, __m256i *b){

  __m256i cc[8];
  polymul_32x32( cc, a, b );

// transforming the 32bits data to 16 bits data in montgomery form.
  gf_mont_4x32_2x16(   c, c+1, cc[0], cc[1], cc[2], cc[3] );
  gf_mont_4x32_2x16( c+2, c+3, cc[4], cc[5], cc[6], cc[7] );

}

#else

// 674k haswell cycles for a full polyinv() with _KA_16_TO_32_ defined.
static inline
void gf_polymul_32x32_avx2_divR(__m256i *c, __m256i *a, __m256i *b){

  __m256i t0e, t0o, t1e, t1o, t2e, t2o, t3e, t3o;
  __m256i s0e, s0o, s1e, s1o, r0e, r0o, r1e, r1o;

#define _KA_16_TO_32_
#ifndef _KA_16_TO_32_
  // exclusive bound on entries: 7900 = ceil ( ((2^31-4591*2^15)/32)^(1/2) )
  polymul_16x16_32(&t0e,&t0o,&t1e,&t1o,a[0],b[0]);
  polymul_16x16_32(&t2e,&t2o,&t3e,&t3o,a[1],b[1]);
  polymul_16x16_32(&s0e,&s0o,&s1e,&s1o,a[0],b[1]);
  polymul_16x16_32(&r0e,&r0o,&r1e,&r1o,a[1],b[0]);
  r0e = _mm256_add_epi32(r0e,s0e);
  r0o = _mm256_add_epi32(r0o,s0o);
  r1e = _mm256_add_epi32(r1e,s1e);
  r1o = _mm256_add_epi32(r1o,s1o);
  t1e = _mm256_add_epi32(r0e,t1e);
  t1o = _mm256_add_epi32(r0o,t1o);
  t2e = _mm256_add_epi32(r1e,t2e);
  t2o = _mm256_add_epi32(r1o,t2o);
  
#else
  // exclusive bound on entries: 5762 = ceil( ((2^31)/16)^(1/2)/2 )
  __m256i a01 = _mm256_add_epi16(a[0],a[1]);
  __m256i b01 = _mm256_add_epi16(b[0],b[1]);

  polymul_16x16_32(&s0e,&s0o,&s1e,&s1o,a01,b01);
  polymul_16x16_32(&t0e,&t0o,&t1e,&t1o,a[0],b[0]);
  polymul_16x16_32(&t2e,&t2o,&t3e,&t3o,a[1],b[1]);
  r0e = _mm256_sub_epi32(t1e,t2e);
  r0o = _mm256_sub_epi32(t1o,t2o);
  r1e = _mm256_add_epi32(r0e,t3e);
  r1o = _mm256_add_epi32(r0o,t3o);
  r0e = _mm256_sub_epi32(r0e,t0e);
  r0o = _mm256_sub_epi32(r0o,t0o);
  t2e = _mm256_sub_epi32(s1e,r1e);
  t2o = _mm256_sub_epi32(s1o,r1o);
  t1e = _mm256_add_epi32(s0e,r0e);
  t1o = _mm256_add_epi32(s0o,r0o);

#endif
  gf_mont_4x32_2x16(c,c+1,t0e,t0o,t1e,t1o);
  gf_mont_4x32_2x16(c+2,c+3,t2e,t2o,t3e,t3o);
  
}

#endif

static inline
void gf_polymul_32x32_avx2 (__m256i *h, __m256i *f, __m256i *g){
  gf_polymul_32x32_avx2_divR(h,f,g);
  h[0]=montproduct(h[0],v4158_16); 
  h[1]=montproduct(h[1],v4158_16); 
  h[2]=montproduct(h[2],v4158_16); 
  h[3]=montproduct(h[3],v4158_16); 
}

static inline
void gf_polymul_32x32_avx2_2x2_divR (__m256i *ff, __m256i *gg, __m256i *uvqr, __m256i *f, __m256i *g){
  __m256i t[4];

  gf_polymul_32x32_avx2_divR(ff,uvqr,f);
  gf_polymul_32x32_avx2_divR(t,uvqr+2,g);
  ff[0]=_mm256_add_epi16(ff[0],t[0]);
  ff[1]=_mm256_add_epi16(ff[1],t[1]);
  ff[2]=_mm256_add_epi16(ff[2],t[2]);
  ff[3]=_mm256_add_epi16(ff[3],t[3]);

  gf_polymul_32x32_avx2_divR(gg,uvqr+4,f);
  gf_polymul_32x32_avx2_divR(t,uvqr+6,g);
  gg[0]=_mm256_add_epi16(gg[0],t[0]);
  gg[1]=_mm256_add_epi16(gg[1],t[1]);
  gg[2]=_mm256_add_epi16(gg[2],t[2]);
  gg[3]=_mm256_add_epi16(gg[3],t[3]);

  ff[0] = barrett_fake(ff[0]); // <2520
  gg[0] = barrett_fake(gg[0]); // <2520
  ff[1] = barrett_fake(ff[1]); // <2520
  gg[1] = barrett_fake(gg[1]); // <2520
  ff[2] = barrett_fake(ff[2]); // <2520
  gg[2] = barrett_fake(gg[2]); // <2520
  ff[3] = barrett_fake(ff[3]); // <2520
  gg[3] = barrett_fake(gg[3]); // <2520
}

static inline
void gf_polymul_32x32_avx2_2x2 (__m256i *ff, __m256i *gg, __m256i *uvqr, __m256i *f, __m256i *g){
  __m256i t[4];

  gf_polymul_32x32_avx2_divR(ff,uvqr,f);
  gf_polymul_32x32_avx2_divR(t,uvqr+2,g);
  ff[0]=_mm256_add_epi16(ff[0],t[0]);
  ff[1]=_mm256_add_epi16(ff[1],t[1]);
  ff[2]=_mm256_add_epi16(ff[2],t[2]);
  ff[3]=_mm256_add_epi16(ff[3],t[3]);

  gf_polymul_32x32_avx2_divR(gg,uvqr+4,f);
  gf_polymul_32x32_avx2_divR(t,uvqr+6,g);
  gg[0]=_mm256_add_epi16(gg[0],t[0]);
  gg[1]=_mm256_add_epi16(gg[1],t[1]);
  gg[2]=_mm256_add_epi16(gg[2],t[2]);
  gg[3]=_mm256_add_epi16(gg[3],t[3]);

  ff[0] = montproduct(ff[0],v4158_16); // <2336
  gg[0] = montproduct(gg[0],v4158_16); // <2336
  ff[1] = montproduct(ff[1],v4158_16); // <2336
  gg[1] = montproduct(gg[1],v4158_16); // <2336
  ff[2] = montproduct(ff[2],v4158_16); // <2336
  gg[2] = montproduct(gg[2],v4158_16); // <2336
  ff[3] = montproduct(ff[3],v4158_16); // <2336
  gg[3] = montproduct(gg[3],v4158_16); // <2336
}

static inline
void gf_polymul_32x32_avx2_uvqr_fg (__m256i *ff, __m256i *gg, __m256i *uvqr, __m256i *f, __m256i *g){
  __m256i t[4];
  __m256i u_mask = uv_over(uvqr);
  __m256i v_mask = uv_over(uvqr+2);
  __m256i uv_mask = _mm256_or_si256(u_mask,v_mask);

  gf_polymul_32x32_avx2_divR(ff,uvqr,f);
  gf_polymul_32x32_avx2_divR(t,uvqr+2,g);
  ff[0]=_mm256_add_epi16(ff[0],t[0]);
  ff[1]=_mm256_add_epi16(ff[1],t[1]);
  ff[2]=_mm256_add_epi16(ff[2],t[2]);
  ff[3]=_mm256_add_epi16(ff[3],t[3]);

  ff[2]=_mm256_or_si256(_mm256_and_si256(uv_mask,ff[0]),ff[2]);
  ff[0]=_mm256_andnot_si256(uv_mask,ff[0]);
  ff[3]=_mm256_or_si256(_mm256_and_si256(uv_mask,ff[1]),ff[3]);
  ff[1]=_mm256_andnot_si256(uv_mask,ff[1]);

  gf_polymul_32x32_avx2_divR(gg,uvqr+4,f);
  gf_polymul_32x32_avx2_divR(t,uvqr+6,g);

  gg[0]=_mm256_add_epi16(gg[0],t[0]);
  gg[1]=_mm256_add_epi16(gg[1],t[1]);
  gg[2]=_mm256_add_epi16(gg[2],t[2]);
  gg[3]=_mm256_add_epi16(gg[3],t[3]);

  ff[0] = montproduct(ff[0],v4158_16); // <2336
  gg[0] = montproduct(gg[0],v4158_16); // <2336
  ff[1] = montproduct(ff[1],v4158_16); // <2336
  gg[1] = montproduct(gg[1],v4158_16); // <2336
  ff[2] = montproduct(ff[2],v4158_16); // <2336
  gg[2] = montproduct(gg[2],v4158_16); // <2336
  ff[3] = montproduct(ff[3],v4158_16); // <2336
  gg[3] = montproduct(gg[3],v4158_16); // <2336
}

static inline
void gf_polymul_32x32_avx2_uvqr_vr (__m256i *ff, __m256i *gg, __m256i *uvqr, __m256i *f, __m256i *g){
  __m256i t[4];
  __m256i u_mask = uv_over(uvqr);
  __m256i v_mask = uv_over(uvqr+2);
  __m256i uv_mask = _mm256_or_si256(u_mask,v_mask);
  __m256i f_mask = uv_over(f);

  gf_polymul_32x32_avx2_divR(ff,uvqr,f);
  ff[2]=_mm256_or_si256(_mm256_and_si256(f_mask,ff[0]),ff[2]);
  ff[0]=_mm256_andnot_si256(f_mask,ff[0]);
  ff[3]=_mm256_or_si256(_mm256_and_si256(f_mask,ff[1]),ff[3]);
  ff[1]=_mm256_andnot_si256(f_mask,ff[1]);
  gf_polymul_32x32_avx2_divR(t,uvqr+2,g);
  ff[0]=_mm256_add_epi16(ff[0],t[0]);
  ff[1]=_mm256_add_epi16(ff[1],t[1]);
  ff[2]=_mm256_add_epi16(ff[2],t[2]);
  ff[3]=_mm256_add_epi16(ff[3],t[3]);

  ff[2]=_mm256_or_si256(_mm256_and_si256(uv_mask,ff[0]),ff[2]);
  ff[0]=_mm256_andnot_si256(uv_mask,ff[0]);
  ff[3]=_mm256_or_si256(_mm256_and_si256(uv_mask,ff[1]),ff[3]);
  ff[1]=_mm256_andnot_si256(uv_mask,ff[1]);

  gf_polymul_32x32_avx2_divR(gg,uvqr+4,f);
  gg[2]=_mm256_or_si256(_mm256_and_si256(f_mask,gg[0]),gg[2]);
  gg[0]=_mm256_andnot_si256(f_mask,gg[0]);
  gg[3]=_mm256_or_si256(_mm256_and_si256(f_mask,gg[1]),gg[3]);
  gg[1]=_mm256_andnot_si256(f_mask,gg[1]);
  gf_polymul_32x32_avx2_divR(t,uvqr+6,g);
  gg[0]=_mm256_add_epi16(gg[0],t[0]);
  gg[1]=_mm256_add_epi16(gg[1],t[1]);
  gg[2]=_mm256_add_epi16(gg[2],t[2]);
  gg[3]=_mm256_add_epi16(gg[3],t[3]);

  ff[0] = montproduct(ff[0],v4158_16); // <2336
  gg[0] = montproduct(gg[0],v4158_16); // <2336
  ff[1] = montproduct(ff[1],v4158_16); // <2336
  gg[1] = montproduct(gg[1],v4158_16); // <2336
  ff[2] = montproduct(ff[2],v4158_16); // <2336
  gg[2] = montproduct(gg[2],v4158_16); // <2336
  ff[3] = montproduct(ff[3],v4158_16); // <2336
  gg[3] = montproduct(gg[3],v4158_16); // <2336

  ff[0] = _mm256_or_si256(_mm256_and_si256(_mm256_and_si256(u_mask,f_mask),v01),ff[0]);
}

#endif
