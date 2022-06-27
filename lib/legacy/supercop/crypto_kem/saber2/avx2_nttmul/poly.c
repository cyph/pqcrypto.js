#include <stdint.h>
#include <assert.h>
#include <immintrin.h>
#include "poly.h"
#include "consts256.h"

static inline __m256i mulmod(const __m256i a, const __m256i b_pinv, const __m256i b, const __m256i p) {
  __m256i t,u;
  t = _mm256_mullo_epi16(a,b_pinv);
  u = _mm256_mulhi_epi16(a,b);
  t = _mm256_mulhi_epi16(t,p);
  t = _mm256_sub_epi16(u,t);
  return t;
}

static inline __m256i reduce(const __m256i a, const __m256i p, const int16_t *pdata) {
  __m256i t;
  const __m256i v = _mm256_load_si256((__m256i *)&pdata[_16XV]);
  const __m256i shift = _mm256_load_si256((__m256i *)&pdata[_16XSHIFT]);
  t = _mm256_mulhi_epi16(a,v);
  t = _mm256_mulhrs_epi16(t,shift);
  t = _mm256_mullo_epi16(t,p);
  t = _mm256_sub_epi16(a,t);
  return t;
}

#if NTT_N == KEM_N
void poly_crt(poly *r, const nttpoly *a, const nttpoly *b) {
  unsigned int i;
  __m256i f0, f1;
  const __m256i u_pinv = _mm256_set1_epi16(CRT_U_PINV);
  const __m256i u = _mm256_set1_epi16(CRT_U);
  const __m256i p0 = _mm256_load_si256((__m256i *)&PDATA0[_16XP]);
  const __m256i p1 = _mm256_load_si256((__m256i *)&PDATA1[_16XP]);
  const __m256i mod = _mm256_set1_epi16(KEM_Q-1);
  const __m256i mont0_pinv = _mm256_load_si256((__m256i *)&PDATA0[_16XMONT_PINV]);
  const __m256i mont0 = _mm256_load_si256((__m256i *)&PDATA0[_16XMONT]);

  for(i=0;i<KEM_N/16;i++) {
    f0 = _mm256_load_si256((__m256i *)&a->coeffs[16*i]);
    f1 = _mm256_load_si256((__m256i *)&b->coeffs[16*i]);
    f0 = mulmod(f0,mont0_pinv,mont0,p0);  // extra reduction
    f1 = _mm256_sub_epi16(f1,f0);
    f1 = mulmod(f1,u_pinv,u,p1);
    f1 = _mm256_mullo_epi16(f1,p0);
    f0 = _mm256_add_epi16(f0,f1);
    f0 = _mm256_and_si256(f0,mod);
    _mm256_store_si256((__m256i *)&r->coeffs[16*i],f0);
  }
}
#else
void poly_crt(poly *r, const nttpoly *a, const nttpoly *b) {
  unsigned int i;
  __m256i f0, f1, g0, g1;
  const __m256i u_pinv = _mm256_set1_epi16(CRT_U_PINV);
  const __m256i u = _mm256_set1_epi16(CRT_U);
  const __m256i p0 = _mm256_load_si256((__m256i *)&PDATA0[_16XP]);
  const __m256i p1 = _mm256_load_si256((__m256i *)&PDATA1[_16XP]);
  const __m256i mod = _mm256_set1_epi16(KEM_Q-1);
#if NTT_N == 1536
  const __m256i mont0_pinv = _mm256_load_si256((__m256i *)&PDATA0[_16XMONT_PINV]);
  const __m256i mont0 = _mm256_load_si256((__m256i *)&PDATA0[_16XMONT]);
#endif

  for(i=0;i<(KEM_N+15)/16;i++) {
    f0 = _mm256_load_si256((__m256i *)&a->coeffs[16*i]);
    f1 = _mm256_load_si256((__m256i *)&b->coeffs[16*i]);
    g0 = _mm256_loadu_si256((__m256i *)&a->coeffs[KEM_N+16*i]);
    g1 = _mm256_loadu_si256((__m256i *)&b->coeffs[KEM_N+16*i]);
#if NTT_N == 1024
    f0 = reduce(f0,p0,PDATA0);  // extra reduction
    g0 = reduce(g0,p0,PDATA0);  // extra reduction
#elif NTT_N == 1536
    f0 = mulmod(f0,mont0_pinv,mont0,p0);  // extra reduction
    g0 = mulmod(g0,mont0_pinv,mont0,p0);  // extra reduction
#endif
    f1 = _mm256_sub_epi16(f1,f0);
    g1 = _mm256_sub_epi16(g1,g0);
    f1 = mulmod(f1,u_pinv,u,p1);
    g1 = mulmod(g1,u_pinv,u,p1);
    f1 = _mm256_mullo_epi16(f1,p0);
    g1 = _mm256_mullo_epi16(g1,p0);
    f0 = _mm256_add_epi16(f0,g0);
    f0 = _mm256_add_epi16(f0,f1);
    f0 = _mm256_add_epi16(f0,g1);
    f0 = _mm256_and_si256(f0,mod);
    _mm256_store_si256((__m256i *)&r->coeffs[16*i],f0);
  }
}
#endif

void poly_add(poly *r, const poly *a, const poly *b) {
  unsigned int i;
  __m256i f0, f1;

  for(i=0;i<(KEM_N+15)/16;i++) {
    f0 = _mm256_load_si256((__m256i *)&a->coeffs[16*i]);
    f1 = _mm256_load_si256((__m256i *)&b->coeffs[16*i]);
    f0 = _mm256_add_epi16(f0, f1);
    _mm256_store_si256((__m256i *)&r->coeffs[16*i], f0);
  }
}

void poly_sub(poly *r, const poly *a, const poly *b) {
  unsigned int i;
  __m256i f0, f1;

  for(i=0;i<(KEM_N+15)/16;i++) {
    f0 = _mm256_load_si256((__m256i *)&a->coeffs[16*i]);
    f1 = _mm256_load_si256((__m256i *)&b->coeffs[16*i]);
    f0 = _mm256_sub_epi16(f0, f1);
    _mm256_store_si256((__m256i *)&r->coeffs[16*i], f0);
  }
}

void poly_mul(poly *r, const poly *a, const poly *b) {
  unsigned int i;
  poly a2, b2;
  nttpoly ahat, bhat, chat;
  const __m256i mod = _mm256_set1_epi16(KEM_Q-1);
  const __m256i q = _mm256_set1_epi16(KEM_Q);
  const __m256i hq = _mm256_set1_epi16(KEM_Q/2-1);
  __m256i f, g, t, u;

#if 0
  for(int k=0;k<KEM_N;k++)
    assert(a->coeffs[k] < KEM_Q/2 && a->coeffs[k] >= -KEM_Q/2);

  for(int k=0;k<KEM_N;k++)
    assert(b->coeffs[k] <= 6  && b->coeffs[k] >= -6);
#endif

  for(i=0;i<(KEM_N+15)/16;i++) {
    f = _mm256_load_si256((__m256i *)&a->coeffs[16*i]);
    g = _mm256_load_si256((__m256i *)&b->coeffs[16*i]);
    f = _mm256_and_si256(f,mod);
    g = _mm256_and_si256(g,mod);
    t = _mm256_cmpgt_epi16(f,hq);
    u = _mm256_cmpgt_epi16(g,hq);
    t = _mm256_and_si256(t,q);
    u = _mm256_and_si256(u,q);
    f = _mm256_sub_epi16(f,t);
    g = _mm256_sub_epi16(g,u);
    _mm256_store_si256((__m256i *)&a2.coeffs[16*i],f);
    _mm256_store_si256((__m256i *)&b2.coeffs[16*i],g);
  }

  f = _mm256_setzero_si256();
  while(i < POLY_N/16) {
    _mm256_store_si256((__m256i *)&a2.coeffs[16*i],f);
    _mm256_store_si256((__m256i *)&b2.coeffs[16*i],f);
    i += 1;
  }

  for(i=KEM_N;i<(KEM_N+15)/16*16;i++)
    a2.coeffs[i] = b2.coeffs[i] = 0;

  poly_ntt(&ahat,&a2,PDATA0);
  poly_ntt(&bhat,&b2,PDATA0);
  poly_basemul_montgomery(&ahat,&ahat,&bhat,PDATA0);
  poly_invntt_tomont(&ahat,&ahat,PDATA0);
  poly_ntt(&bhat,&a2,PDATA1);
  poly_ntt(&chat,&b2,PDATA1);
  poly_basemul_montgomery(&bhat,&bhat,&chat,PDATA1);
  poly_invntt_tomont(&bhat,&bhat,PDATA1);
  poly_crt(r,&ahat,&bhat);
}

#if defined(LAC128) || defined(LAC192)
void polysmall_mul(uint8_t *r, const uint8_t *a, const int8_t *b) {
  unsigned int i;
  poly a2, b2;
  nttpoly ahat, bhat, chat;
  __m256i f, g;

  for(i=0;i<KEM_N/16;i++) {
    f = _mm256_cvtepu8_epi16(_mm_loadu_si128((__m128i *)&a[16*i]));
    g = _mm256_cvtepi8_epi16(_mm_loadu_si128((__m128i *)&b[16*i]));
    _mm256_store_si256((__m256i *)&a2.coeffs[16*i],f);
    _mm256_store_si256((__m256i *)&b2.coeffs[16*i],g);
  }

  f = _mm256_setzero_si256();
  while(i < POLY_N/16) {
    _mm256_store_si256((__m256i *)&a2.coeffs[16*i],f);
    _mm256_store_si256((__m256i *)&b2.coeffs[16*i],f);
    i += 1;
  }

  poly_ntt(&ahat,&a2,PDATA0);
  poly_ntt(&bhat,&b2,PDATA0);
  poly_basemul_montgomery(&ahat,&ahat,&bhat,PDATA0);
  poly_invntt_tomont(&ahat,&ahat,PDATA0);
  poly_ntt(&bhat,&a2,PDATA1);
  poly_ntt(&chat,&b2,PDATA1);
  poly_basemul_montgomery(&bhat,&bhat,&chat,PDATA1);
  poly_invntt_tomont(&bhat,&bhat,PDATA1);
  poly_crt(&a2,&ahat,&bhat);

  for(i=0;i<KEM_N/32;i++) {
    f = _mm256_load_si256((__m256i *)&a2.coeffs[32*i+ 0]);
    g = _mm256_load_si256((__m256i *)&a2.coeffs[32*i+16]);
    f = _mm256_packus_epi16(f,g);
    f = _mm256_permute4x64_epi64(f,0xD8);
    _mm256_storeu_si256((__m256i *)&r[32*i],f);
  }
}
#endif
