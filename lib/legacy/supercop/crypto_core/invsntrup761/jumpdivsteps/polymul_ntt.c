
#include <immintrin.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "gf4591_avx2.h"

#include "polymul_ntt.h"

#include "crt3modq.h"

#include "ntt.h"



typedef int16_t int16;



////////////////////////////////////////////

#if 0
static inline void pv( const int16_t * v, int len )
{
  printf("\n");
  for(int i=0;i<len;i++) {
    printf("%d,",v[i]);
    if(31==(i%32)) printf("\n");
  }
  printf("\n");
}
#endif

///////////////////////////////////////////////////////



#define v7681_16 _mm256_set1_epi16(7681)  // M
#define v57857_16 _mm256_set1_epi16(-7679) // M^{-1} mod R
#define v5569_16 _mm256_set1_epi16(-2112) // R^2 mod M
#define v3840_16 _mm256_set1_epi16(3840)

#define v10753_16 _mm256_set1_epi16(10753)  // M
#define v54785_16 _mm256_set1_epi16(-10751) // M^{-1} mod R
#define v4036_16 _mm256_set1_epi16(4036) // R^2 mod M
#define v5376_16 _mm256_set1_epi16(5376)

#define v12289_16 _mm256_set1_epi16(12289)  // M
#define v53249_16 _mm256_set1_epi16(-12287) // M^{-1} mod R
#define v10952_16 _mm256_set1_epi16(-1337) // R^2 mod M
#define v6144_16 _mm256_set1_epi16(6144)


static inline __m256i montproduct2(__m256i x,__m256i y, __m256i v_q_1_mod_r, __m256i v_q )
{
  __m256i lo,hi,d,e;

  lo = _mm256_mullo_epi16(x,y);
  hi = _mm256_mulhi_epi16(x,y);
  d = _mm256_mullo_epi16(lo,v_q_1_mod_r);
  e = _mm256_mulhi_epi16(d,v_q);
  return _mm256_sub_epi16(hi,e);
}

static inline __m256i center_adjust2(__m256i x, __m256i v_q, __m256i v_q_2 )
{
  __m256i x_abs = _mm256_sign_epi16(x,x);
  __m256i p_pm = _mm256_sign_epi16(v_q,x);
  __m256i mask = _mm256_cmpgt_epi16(x_abs,v_q_2);
  __m256i diff = _mm256_and_si256(p_pm,mask);
  return _mm256_sub_epi16(x,diff);
}

////////////////////////////////////////////////


void polymul_ntt512_mul(__m256i *h, __m256i *f, __m256i *g )
{
  unsigned n_ele = 512;
  unsigned n_ymm = n_ele>>4;
  __m256i *f1 = f;
  __m256i *f2 = f + n_ymm;
  __m256i *f3 = f + 2*n_ymm;
  __m256i *g1 = g;
  __m256i *g2 = g + n_ymm;
  __m256i *g3 = g + 2*n_ymm;
  __m256i *h1 = h;
  __m256i *h2 = h + n_ymm;
  __m256i *h3 = h + 2*n_ymm;

  for(int i=0;i<n_ymm;i++) h1[i] = montproduct2(f1[i],g1[i],v57857_16,v7681_16);
  for(int i=0;i<n_ymm;i++) h2[i] = montproduct2(f2[i],g2[i],v54785_16,v10753_16);
  for(int i=0;i<n_ymm;i++) h3[i] = montproduct2(f3[i],g3[i],v53249_16,v12289_16);
}


void polymul_ntt512_add(__m256i *h, __m256i *f, __m256i *g )
{
  unsigned n_ele = 512;
  unsigned n_ymm = n_ele>>4;
  __m256i *f1 = f;
  __m256i *f2 = f + n_ymm;
  __m256i *f3 = f + 2*n_ymm;
  __m256i *g1 = g;
  __m256i *g2 = g + n_ymm;
  __m256i *g3 = g + 2*n_ymm;
  __m256i *h1 = h;
  __m256i *h2 = h + n_ymm;
  __m256i *h3 = h + 2*n_ymm;

  for(int i=0;i<n_ymm;i++) h1[i] = center_adjust2(_mm256_add_epi16(f1[i],g1[i]),v7681_16,v3840_16);
  for(int i=0;i<n_ymm;i++) h2[i] = center_adjust2(_mm256_add_epi16(f2[i],g2[i]),v10753_16,v5376_16);
  for(int i=0;i<n_ymm;i++) h3[i] = center_adjust2(_mm256_add_epi16(f3[i],g3[i]),v12289_16,v6144_16);
}




//////////////////////////////////////////////////



void polymul_ntt512_256_prepare(__m256i *b, __m256i *a )
{
  int n_ele_input = 256;
  //memset(b,0,3*512*2);
  __m256i *b1p = b;
  __m256i *b2p = b + (512/16);
  __m256i *b3p = b + 2*(512/16);
  memcpy(b1p,a,2*n_ele_input);
  memcpy(b2p,a,2*n_ele_input);
  memcpy(b3p,a,2*n_ele_input);

  ntt512_256_7681((int16_t*)b1p,1);
  ntt512_256_10753((int16_t*)b2p,1);
  ntt512_256_12289((int16_t*)b3p,1);
}


void polymul_ntt512_finale(__m256i *b, __m256i *a)
{
  __m256i tt[3*512/16];
  memcpy( tt , a , sizeof(tt) );
  __m256i *u1p = tt;
  __m256i *u2p = &tt[512/16];
  __m256i *u3p = &tt[2*512/16];

  for(int i=0;i<512/16;i++) u1p[i] = montproduct2(u1p[i],v5569_16,v57857_16,v7681_16);
  invntt512_7681((int16_t*)u1p,1);

  for(int i=0;i<512/16;i++) u2p[i] = montproduct2(u2p[i],v4036_16,v54785_16,v10753_16);
  invntt512_10753((int16_t*)u2p,1);

  for(int i=0;i<512/16;i++) u3p[i] = montproduct2(u3p[i],v10952_16,v53249_16,v12289_16);
  invntt512_12289((int16_t*)u3p,1);

  for(int i=0;i<512/16;i++) {
    b[i] = ecrt3modq(u1p[i], u2p[i], u3p[i]);
  }
}



static void polymulntt512_256x256_test(__m256i *h, __m256i *f, __m256i *g)
{
  __m256i tt1[3*512/16];
  __m256i tt2[3*512/16];
  __m256i tt3[3*512/16];

  polymul_ntt512_256_prepare( tt1 , f );
  polymul_ntt512_256_prepare( tt2 , g );
  polymul_ntt512_mul( tt3 , tt1 , tt2 );
  polymul_ntt512_finale( h , tt3 );
}



////////////////////////////////////////////////////////






void polymul_ntt768_256_prepare(__m256i *b, __m256i *a )
{
  int16_t * a16 = (int16_t *)a;
  int16_t * b16 = (int16_t *)b;

  memcpy(b16,a16,256*2);
  ntt768_256_769(b16,1);
  b16 += 768;

  memcpy(b16,a16,256*2);
  ntt768_256_7681(b16,1);
  b16 += 768;

  memcpy(b16,a16,256*2);
  ntt768_256_10753(b16,1);
}

void polymul_ntt768_512_prepare(__m256i *b, __m256i *a )
{
  int16_t * a16 = (int16_t *)a;
  int16_t * b16 = (int16_t *)b;

  memcpy(b16,a16,512*2);
  ntt768_512_769(b16,1);
  b16 += 768;

  memcpy(b16,a16,512*2);
  ntt768_512_7681(b16,1);
  b16 += 768;

  memcpy(b16,a16,512*2);
  ntt768_512_10753(b16,1);
}

#if 0
static inline
void polymul_ntt768_prepare(__m256i *b, __m256i *a, int n_ele_input)
{
  if( 256 == n_ele_input ) return polymul_ntt768_256_prepare(b,a);
  if( 512 == n_ele_input ) return polymul_ntt768_512_prepare(b,a);

  int16_t * a16 = (int16_t *)a;
  int16_t * b16 = (int16_t *)b;

  memset(b16,0,768*2);
  memcpy(b16,a16,n_ele_input*2);
  ntt768_769(b16,1);
  b16 += 768;

  memset(b16,0,768*2);
  memcpy(b16,a16,n_ele_input*2);
  ntt768_7681(b16,1);
  b16 += 768;

  memset(b16,0,768*2);
  memcpy(b16,a16,n_ele_input*2);
  ntt768_10753(b16,1);
}
#endif

#define v4591_16 _mm256_set1_epi16(4591)
#define v2295_16 _mm256_set1_epi16(2295)

void polymul_ntt768_finale(__m256i *b, __m256i *a)
{
  __m256i _t[3*768/16];
  int16_t * t = (int16_t * )&_t[0];
  int16_t * a16 = (int16_t *)a;

  memcpy(t,a16,768*2);
  invntt768_769(t,1);

  memcpy(t+768,a16+768,768*2);
  invntt768_7681(t+768,1);

  memcpy(t+768*2,a16+768*2,768*2);
  invntt768_10753(t+768*2,1);

  crt768(t);
  for(int i=0;i<768/16;i++) _t[i] = center_adjust2(_t[i],v4591_16,v2295_16);
  memcpy(b,t,768*2);
}



void polymul_ntt768_mul(__m256i *c, __m256i *b, __m256i *a )
{
  int16_t * a16 = (int16_t *)a;
  int16_t * b16 = (int16_t *)b;
  int16_t _c16[768*3];
  int16_t * c16 = _c16;

  memcpy(c16,b16,768*2);
  pointwise768_769(c16,a16);
  a16+=768;
  b16+=768;
  c16+=768;

  memcpy(c16,b16,768*2);
  pointwise768_7681(c16,a16);
  a16+=768;
  b16+=768;
  c16+=768;

  memcpy(c16,b16,768*2);
  pointwise768_10753(c16,a16);

  memcpy(c,_c16,768*3*2);
}

#define v769_16 _mm256_set1_epi16(769)  // M
#define v384_16 _mm256_set1_epi16(384)


void polymul_ntt768_add(__m256i *h, __m256i *g, __m256i *f )
{
  unsigned n_ymm = 48;
  __m256i *f1 = f;
  __m256i *f2 = f + n_ymm;
  __m256i *f3 = f + 2*n_ymm;
  __m256i *g1 = g;
  __m256i *g2 = g + n_ymm;
  __m256i *g3 = g + 2*n_ymm;
  __m256i *h1 = h;
  __m256i *h2 = h + n_ymm;
  __m256i *h3 = h + 2*n_ymm;

  for(int i=0;i<n_ymm;i++) h1[i] = center_adjust2(_mm256_add_epi16(f1[i],g1[i]),v769_16,v384_16);
  for(int i=0;i<n_ymm;i++) h2[i] = center_adjust2(_mm256_add_epi16(f2[i],g2[i]),v7681_16,v3840_16);
  for(int i=0;i<n_ymm;i++) h3[i] = center_adjust2(_mm256_add_epi16(f3[i],g3[i]),v10753_16,v5376_16);
}



////////////////////////////////////////////////////////


static void polymulntt768_256x256_test(__m256i *h, __m256i *f, __m256i *g)
{
  __m256i tt1[3*768/16];
  __m256i tt2[3*768/16];
  __m256i tt3[3*768/16];

  polymul_ntt768_256_prepare( tt1 , f );
  polymul_ntt768_256_prepare( tt2 , g );
  polymul_ntt768_mul( tt3 , tt1 , tt2 );
  polymul_ntt768_finale( tt1 , tt3 );
  memcpy( h , tt1 , 512*2 );
}



////////////////////////////////////////////////////////


