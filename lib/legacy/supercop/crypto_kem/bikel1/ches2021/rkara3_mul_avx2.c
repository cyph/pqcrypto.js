/*
 * Written by Ming-Shing Chen and Tung Chou
 */

#include "stdint.h"
#include "rkara3_mul.h"

#include <immintrin.h>

static inline
__m256i msbyte( __m256i a ) { return _mm256_permute4x64_epi64(_mm256_srli_si256(a,15),0xfe); } // 11,11,11,10

static inline
__m256i shl_1( __m256i a, __m256i a_minus_1 ) { return _mm256_slli_epi16(a,1)|_mm256_srli_epi16(a_minus_1,7); }

static inline
__m256i shl_2( __m256i a, __m256i a_minus_1 ) { return _mm256_slli_epi16(a,2)|_mm256_srli_epi16(a_minus_1,6); }

static
void shl_1_test( uint8_t *b , const uint8_t *a , int len )
{
  __m256i _a0 = _mm256_loadu_si256((const __m256i*)a);
  __m256i a15 = _mm256_permute4x64_epi64(_mm256_srli_si256(_a0,15),0x4f); // 01,00,11,11
  __m256i _a_1 = _mm256_slli_si256(_a0,1)|a15;
  _mm256_storeu_si256((__m256i*)b, shl_1(_a0,_a_1) );

  for(int i=32;i<len;i+=32) {
    __m256i a0 = _mm256_loadu_si256((const __m256i*)(a+i));
    __m256i a_1 = _mm256_loadu_si256((const __m256i*)(a+i-1));
    _mm256_storeu_si256((__m256i*)(b+i), shl_1(a0,a_1) );
  }
}

static
void shl_2_test( uint8_t *b , const uint8_t *a , int len )
{
  __m256i _a0 = _mm256_loadu_si256((const __m256i*)a);
  __m256i a15 = _mm256_permute4x64_epi64(_mm256_srli_si256(_a0,15),0x4f); // 01,00,11,11
  __m256i _a_1 = _mm256_slli_si256(_a0,1)|a15;
  _mm256_storeu_si256((__m256i*)b, shl_2(_a0,_a_1) );

  for(int i=32;i<len;i+=32) {
    __m256i a0 = _mm256_loadu_si256((const __m256i*)(a+i));
    __m256i a_1 = _mm256_loadu_si256((const __m256i*)(a+i-1));
    _mm256_storeu_si256((__m256i*)(b+i), shl_2(a0,a_1) );
  }
}

////////////////////////

static inline
void add( uint8_t *c , const uint8_t *a , const uint8_t *b, int len )
{
  for(int i=0;i<len;i+=32) {
    __m256i a0 = _mm256_loadu_si256((const __m256i*)(a+i));
    __m256i b0 = _mm256_loadu_si256((const __m256i*)(b+i));
    _mm256_storeu_si256((__m256i*)(c+i), a0^b0 );
  }
}

static inline
void cpy( uint8_t *c , const uint8_t *a , int len ) {
  for(int i=0;i<len;i+=32) _mm256_storeu_si256((__m256i*)(c+i), _mm256_loadu_si256((const __m256i*)(a+i)) );
}

//////////////////////////

static inline
__m256i shr_1( __m256i a, __m256i a_plus_1 ) { return _mm256_srli_epi16(a,1)|_mm256_slli_epi16(a_plus_1,7); }

static
void shr_1_test( uint8_t *b , const uint8_t *a , int len )
{
  for(int i=0;i<len;i+=32) {
    __m256i a0 = _mm256_loadu_si256((const __m256i*)(a+i));
    __m256i a_1 = _mm256_loadu_si256((const __m256i*)(a+i+1));
    _mm256_storeu_si256((__m256i*)(b+i), shr_1(a0,a_1) );
  }
}

static
void add_shr1( uint8_t *a , int len )
{
  for(int i=0;i<len;i+=32) {
    __m256i a0 = _mm256_loadu_si256((const __m256i*)(a+i));
    __m256i a_1 = _mm256_loadu_si256((const __m256i*)(a+i+1));
    _mm256_storeu_si256((__m256i*)(a+i), a0^shr_1(a0,a_1) );
  }
}

static inline
__m256i shr_2( __m256i a, __m256i a_plus_1 ) { return _mm256_srli_epi16(a,2)|_mm256_slli_epi16(a_plus_1,6); }

static
void add_shr2( uint8_t *a , int len )
{
  for(int i=0;i<len;i+=32) {
    __m256i a0 = _mm256_loadu_si256((const __m256i*)(a+i));
    __m256i a_1 = _mm256_loadu_si256((const __m256i*)(a+i+1));
    _mm256_storeu_si256((__m256i*)(a+i), a0^shr_2(a0,a_1) );
  }
}

static
void shr_2_test( uint8_t *b , const uint8_t *a , int len )
{
  for(int i=0;i<len;i+=32) {
    __m256i a0 = _mm256_loadu_si256((const __m256i*)(a+i));
    __m256i a_1 = _mm256_loadu_si256((const __m256i*)(a+i+1));
    _mm256_storeu_si256((__m256i*)(b+i), shr_2(a0,a_1) );
  }
}


static inline
__m256i shr_4( __m256i a, __m256i a_plus_1 ) { return _mm256_srli_epi16(a,4)|_mm256_slli_epi16(a_plus_1,4); }

void add_shr4( uint8_t *a , int len )
{
  for(int i=0;i<len;i+=32) {
    __m256i a0 = _mm256_loadu_si256((const __m256i*)(a+i));
    __m256i a_1 = _mm256_loadu_si256((const __m256i*)(a+i+1));
    _mm256_storeu_si256((__m256i*)(a+i), a0^shr_4(a0,a_1) );
  }
}


static inline
void add_shr_jbyte( uint8_t *a , int len , int j)
{
  for(int i=0;i+j<len;i+=32) {
    __m256i a0 = _mm256_loadu_si256((const __m256i*)(a+i));
    _mm256_storeu_si256((__m256i*)(a+i), a0^_mm256_loadu_si256((const __m256i*)(a+i+j)) );
  }
}

static
void div_t2_t_test( uint8_t * poly , int len ) {
  add_shr1( poly , len );
  add_shr2( poly , len );
  add_shr4( poly , len );
  for(int i=1;i<len;i<<=1 ) { add_shr_jbyte( poly , len , i ); }
}




///////////////////////////

static
void _madd_2bits_test( uint8_t *c , const uint8_t *a , uint8_t b , int len )
{
  __m256i b_0 = _mm256_sub_epi8(_mm256_setzero_si256(),_mm256_set1_epi8( b&1 ));
  __m256i b_1 = _mm256_sub_epi8(_mm256_setzero_si256(),_mm256_set1_epi8( (b>>1)&1 ));

  __m256i _a0 = _mm256_loadu_si256((const __m256i*)a);
  __m256i a15 = _mm256_permute4x64_epi64(_mm256_srli_si256(_a0,15),0x4f); // 01,00,11,11
  __m256i _a_1 = _mm256_slli_si256(_a0,1)|a15;
  _mm256_storeu_si256((__m256i*)c, _mm256_loadu_si256((const __m256i*)c)^(shl_1(_a0,_a_1)&b_1)^(_a0&b_0) );

  for(int i=32;i<len;i+=32) {
    __m256i a0 = _mm256_loadu_si256((const __m256i*)(a+i));
    __m256i a_1 = _mm256_loadu_si256((const __m256i*)(a+i-1));
    _mm256_storeu_si256((__m256i*)(c+i), _mm256_loadu_si256((const __m256i*)(c+i))^(shl_1(a0,a_1)&b_1)^(a0&b_0) );
  }
}

static
void mul_2bits_test( uint8_t *c , const uint8_t *a , uint8_t b , int len )
{
  for(int i=0;i<len;i++) c[i]=0;
  _madd_2bits_test( c , a , b , len );
  c[len] = (a[len-1]>>7)&(b>>1);
}


//////////////////////////////////////

#include "gf2x_karatsuba.h"

// 512x512bit multiplication performed by Karatsuba algorithm
// where a and b are considered as having 8 digits of size 64 bits.
//void gf2x_mul_base(OUT uint64_t *c, IN const uint64_t *a, IN const uint64_t *b);


void rkara3_mul_1536(uint64_t *c, const uint64_t *a, const uint64_t *b)
{
#if 1
  const uint64_t * f0 = a;
  const uint64_t * f1 = a+8;
  const uint64_t * f2 = a+16;
  const uint64_t * g0 = b;
  const uint64_t * g1 = b+8;
  const uint64_t * g2 = b+16;

  uint64_t h0[16];
  uint64_t h1[16];
  uint64_t hinf[20]; hinf[16]=0; // for shr_1
  uint64_t ht[16];
  uint64_t ht1[16];

///
  uint64_t f012[8];
  uint64_t g012[8];
  add( (uint8_t*)f012 , (const uint8_t*)f0 , (const uint8_t*)f1 , 64 );
  add( (uint8_t*)f012 , (const uint8_t*)f012 , (const uint8_t*)f2 , 64 ); // f0+f1+f2
  add( (uint8_t*)g012 , (const uint8_t*)g0 , (const uint8_t*)g1 , 64 );
  add( (uint8_t*)g012 , (const uint8_t*)g012 , (const uint8_t*)g2 , 64 ); // g0+g1+g2

///
  uint64_t tf[8];
  uint64_t tg[8];
  shl_1_test( (uint8_t*)tf , (const uint8_t *)f1 , 64 );
  shl_2_test( (uint8_t*)ht , (const uint8_t *)f2 , 64 );
  add( (uint8_t*)tf , (const uint8_t*)tf , (const uint8_t*)ht , 64 );
  uint8_t tf1 = (f1[7]>>63)^(f2[7]>>62); // 2bits

  shl_1_test( (uint8_t*)tg , (const uint8_t *)g1 , 64 );
  shl_2_test( (uint8_t*)ht , (const uint8_t *)g2 , 64 );
  add( (uint8_t*)tg , (const uint8_t*)tg , (const uint8_t*)ht , 64 );
  uint8_t tg1 = (g1[7]>>63)^(g2[7]>>62); // 2bits

  uint8_t tf1_mul_tg1 = (tf1*(tg1&1))^(tf1*(tg1&2)); // 3bits

// ht
  uint64_t tmp_f[8];
  uint64_t tmp_g[8];

  add( (uint8_t*)tmp_f , (const uint8_t*)tf , (const uint8_t*)f0 , 64 );
  add( (uint8_t*)tmp_g , (const uint8_t*)tg , (const uint8_t*)g0 , 64 );
  gf2x_mul_base( ht , tmp_f , tmp_g );
  _madd_2bits_test( (uint8_t*)(ht+8) , (const uint8_t*)tmp_f , tg1 , 64 );
  _madd_2bits_test( (uint8_t*)(ht+8) , (const uint8_t*)tmp_g , tf1 , 64 );
  uint8_t ht_high = tf1_mul_tg1 ^ ((tmp_f[7]>>63)&(tg1>>1)) ^ ((tmp_g[7]>>63)&(tf1>>1));

// ht1
  add( (uint8_t*)tmp_f , (const uint8_t*)tf , (const uint8_t*)f012 , 64 );
  add( (uint8_t*)tmp_g , (const uint8_t*)tg , (const uint8_t*)g012 , 64 );
  gf2x_mul_base( ht1 , tmp_f , tmp_g );
  _madd_2bits_test( (uint8_t*)(ht1+8) , (const uint8_t*)tmp_f , tg1 , 64 );
  _madd_2bits_test( (uint8_t*)(ht1+8) , (const uint8_t*)tmp_g , tf1 , 64 );
  uint8_t ht1_high = tf1_mul_tg1 ^ ((tmp_f[7]>>63)&(tg1>>1)) ^ ((tmp_g[7]>>63)&(tf1>>1));

// ht_ht1n
  add( (uint8_t*)ht1 , (const uint8_t*)ht1 , (const uint8_t*)ht , 128 );
  ht1_high ^= ht_high;  // 1bit

// h1
  gf2x_mul_base( h1 , f012 , g012 );
// h0
  gf2x_mul_base( h0 , f0 , g0 );
// hinf
  gf2x_mul_base( hinf , f2 , g2 );

// U
  cpy( (uint8_t*)c , (const uint8_t*)h0 , 64 );
  add( (uint8_t*)(c+8) , (const uint8_t*)h0 , (const uint8_t*)h1 , 64*2 );
  add( (uint8_t*)(c+8) , (const uint8_t*)(c+8) , (const uint8_t*)(h0+8) , 64 );

// V
  uint64_t V[24+4] = {0};
  cpy( (uint8_t*)V , (const uint8_t*)ht , 128 );
  V[16] = ht_high;
  add( (uint8_t*)(V+8) , (const uint8_t*)(V+8) , (const uint8_t*)ht1 , 128 );
//  V[24] = ht1_high;
  uint8_t v_high = ht1_high;

  uint64_t tmp[16];
  shl_1_test( (uint8_t*)tmp , (const uint8_t*)ht1 , 128 ); // (ht+ht1)<<1
  add( (uint8_t*)V , (const uint8_t*)V , (const uint8_t*)tmp , 128 );
  V[16] ^= ((ht1[15]>>63)^(ht1_high<<1));

// W
  add( (uint8_t*)V , (const uint8_t*)V , (const uint8_t*)c , 192 );

  shl_2_test( (uint8_t*)tmp , (const uint8_t*)hinf , 128 );
  uint64_t hinf_shl2_high = (hinf[15]>>62);
  uint64_t tmp2[16];
  shr_1_test( (uint8_t*)tmp2 , (const uint8_t*)hinf , 128 );
  add( (uint8_t*)tmp , (const uint8_t*)tmp , (const uint8_t*)tmp2 , 128 );

  shr_2_test( (uint8_t*)V , (const uint8_t*)V , 192 );
  V[23] ^= (((uint64_t)v_high)<<62);

  add( (uint8_t*)V , (const uint8_t*)V , (const uint8_t*)tmp , 128 );
  V[16] ^= hinf_shl2_high;

  div_t2_t_test( (uint8_t*)V , 192 );

// output
  for(int i=0;i<8;i++) c[24+i]=0;
  cpy( (uint8_t*)(c+32) , (const uint8_t*)hinf , 128 );
  add( (uint8_t*)(c+8) , (const uint8_t*)(c+8) , (const uint8_t*)hinf , 128 );
  add( (uint8_t*)(c+16) , (const uint8_t*)(c+16) , (const uint8_t*)V , 192 );
  add( (uint8_t*)(c+8) , (const uint8_t*)(c+8) , (const uint8_t*)V , 192 );

#else
  gf2x_mul_1536( c , a , b );
#endif
}


///////////////




void rkara3_mul_12288(uint64_t *c, const uint64_t *a, const uint64_t *b)
{
#if 1
  const uint64_t * f0 = a;
  const uint64_t * f1 = a+64;
  const uint64_t * f2 = a+128;
  const uint64_t * g0 = b;
  const uint64_t * g1 = b+64;
  const uint64_t * g2 = b+128;

  uint64_t h0[128];
  uint64_t h1[128];
  uint64_t hinf[128+4]; hinf[128]=0; // for shr_1
  uint64_t ht[128];
  uint64_t ht1[128];

///
  uint64_t f012[64];
  uint64_t g012[64];
  add( (uint8_t*)f012 , (const uint8_t*)f0 , (const uint8_t*)f1 , 512 );
  add( (uint8_t*)f012 , (const uint8_t*)f012 , (const uint8_t*)f2 , 512 ); // f0+f1+f2
  add( (uint8_t*)g012 , (const uint8_t*)g0 , (const uint8_t*)g1 , 512 );
  add( (uint8_t*)g012 , (const uint8_t*)g012 , (const uint8_t*)g2 , 512 ); // g0+g1+g2

///
  uint64_t tf[64];
  uint64_t tg[64];
  shl_1_test( (uint8_t*)tf , (const uint8_t *)f1 , 512 );
  shl_2_test( (uint8_t*)ht , (const uint8_t *)f2 , 512 );
  add( (uint8_t*)tf , (const uint8_t*)tf , (const uint8_t*)ht , 512 );
  uint8_t tf1 = (f1[64-1]>>63)^(f2[64-1]>>62); // 2bits

  shl_1_test( (uint8_t*)tg , (const uint8_t *)g1 , 512 );
  shl_2_test( (uint8_t*)ht , (const uint8_t *)g2 , 512 );
  add( (uint8_t*)tg , (const uint8_t*)tg , (const uint8_t*)ht , 512 );
  uint8_t tg1 = (g1[64-1]>>63)^(g2[64-1]>>62); // 2bits

  uint8_t tf1_mul_tg1 = (tf1*(tg1&1))^(tf1*(tg1&2)); // 3bits

// ht
  uint64_t tmp_f[64];
  uint64_t tmp_g[64];

  add( (uint8_t*)tmp_f , (const uint8_t*)tf , (const uint8_t*)f0 , 512 );
  add( (uint8_t*)tmp_g , (const uint8_t*)tg , (const uint8_t*)g0 , 512 );
  gf2x_mul_4096( ht , tmp_f , tmp_g );
  _madd_2bits_test( (uint8_t*)(ht+64) , (const uint8_t*)tmp_f , tg1 , 512 );
  _madd_2bits_test( (uint8_t*)(ht+64) , (const uint8_t*)tmp_g , tf1 , 512 );
  uint8_t ht_high = tf1_mul_tg1 ^ ((tmp_f[64-1]>>63)&(tg1>>1)) ^ ((tmp_g[64-1]>>63)&(tf1>>1));

// ht1
  add( (uint8_t*)tmp_f , (const uint8_t*)tf , (const uint8_t*)f012 , 512 );
  add( (uint8_t*)tmp_g , (const uint8_t*)tg , (const uint8_t*)g012 , 512 );
  gf2x_mul_4096( ht1 , tmp_f , tmp_g );
  _madd_2bits_test( (uint8_t*)(ht1+64) , (const uint8_t*)tmp_f , tg1 , 512 );
  _madd_2bits_test( (uint8_t*)(ht1+64) , (const uint8_t*)tmp_g , tf1 , 512 );
  uint8_t ht1_high = tf1_mul_tg1 ^ ((tmp_f[64-1]>>63)&(tg1>>1)) ^ ((tmp_g[64-1]>>63)&(tf1>>1));

// ht_ht1n
  add( (uint8_t*)ht1 , (const uint8_t*)ht1 , (const uint8_t*)ht , 1024 );
  ht1_high ^= ht_high;  // 1bit

// h1
  gf2x_mul_4096( h1 , f012 , g012 );
// h0
  gf2x_mul_4096( h0 , f0 , g0 );
// hinf
  gf2x_mul_4096( hinf , f2 , g2 );

// U
  cpy( (uint8_t*)c , (const uint8_t*)h0 , 512 );
  add( (uint8_t*)(c+64) , (const uint8_t*)h0 , (const uint8_t*)h1 , 512*2 );
  add( (uint8_t*)(c+64) , (const uint8_t*)(c+64) , (const uint8_t*)(h0+64) , 512 );

// V
  uint64_t V[192+4] = {0};
  cpy( (uint8_t*)V , (const uint8_t*)ht , 1024 );
  V[128] = ht_high;
  add( (uint8_t*)(V+64) , (const uint8_t*)(V+64) , (const uint8_t*)ht1 , 1024 );
//  V[24] = ht1_high;
  uint8_t v_high = ht1_high;

  uint64_t tmp[128];
  shl_1_test( (uint8_t*)tmp , (const uint8_t*)ht1 , 1024 ); // (ht+ht1)<<1
  add( (uint8_t*)V , (const uint8_t*)V , (const uint8_t*)tmp , 1024 );
  V[128] ^= ((ht1[128-1]>>63)^(ht1_high<<1));

// W
  add( (uint8_t*)V , (const uint8_t*)V , (const uint8_t*)c , 1536 );

  shl_2_test( (uint8_t*)tmp , (const uint8_t*)hinf , 1024 );
  uint64_t hinf_shl2_high = (hinf[128-1]>>62);
  uint64_t tmp2[128];
  shr_1_test( (uint8_t*)tmp2 , (const uint8_t*)hinf , 1024 );
  add( (uint8_t*)tmp , (const uint8_t*)tmp , (const uint8_t*)tmp2 , 1024 );

  shr_2_test( (uint8_t*)V , (const uint8_t*)V , 1536 );
  V[192-1] ^= (((uint64_t)v_high)<<62);

  add( (uint8_t*)V , (const uint8_t*)V , (const uint8_t*)tmp , 1024 );
  V[128] ^= hinf_shl2_high;

  div_t2_t_test( (uint8_t*)V , 1536 );

// output
  for(int i=0;i<64;i++) c[192+i]=0;
  cpy( (uint8_t*)(c+256) , (const uint8_t*)hinf , 1024 );
  add( (uint8_t*)(c+64) , (const uint8_t*)(c+64) , (const uint8_t*)hinf , 1024 );
  add( (uint8_t*)(c+128) , (const uint8_t*)(c+128) , (const uint8_t*)V , 1536 );
  add( (uint8_t*)(c+64) , (const uint8_t*)(c+64) , (const uint8_t*)V , 1536 );

#else
  gf2x_mul_12288( c , a , b );
#endif
}


////////////////////////



void rkara3_mul_24576(uint64_t *c, const uint64_t *a, const uint64_t *b)
{
#if 1
  const uint64_t * f0 = a;
  const uint64_t * f1 = a+128;
  const uint64_t * f2 = a+256;
  const uint64_t * g0 = b;
  const uint64_t * g1 = b+128;
  const uint64_t * g2 = b+256;

  uint64_t h0[256];
  uint64_t h1[256];
  uint64_t hinf[256+4]; hinf[256]=0; // for shr_1
  uint64_t ht[256];
  uint64_t ht1[256];

///
  uint64_t f012[128];
  uint64_t g012[128];
  add( (uint8_t*)f012 , (const uint8_t*)f0 , (const uint8_t*)f1 , 1024 );
  add( (uint8_t*)f012 , (const uint8_t*)f012 , (const uint8_t*)f2 , 1024 ); // f0+f1+f2
  add( (uint8_t*)g012 , (const uint8_t*)g0 , (const uint8_t*)g1 , 1024 );
  add( (uint8_t*)g012 , (const uint8_t*)g012 , (const uint8_t*)g2 , 1024 ); // g0+g1+g2

///
  uint64_t tf[128];
  uint64_t tg[128];
  shl_1_test( (uint8_t*)tf , (const uint8_t *)f1 , 1024 );
  shl_2_test( (uint8_t*)ht , (const uint8_t *)f2 , 1024 );
  add( (uint8_t*)tf , (const uint8_t*)tf , (const uint8_t*)ht , 1024 );
  uint8_t tf1 = (f1[128-1]>>63)^(f2[128-1]>>62); // 2bits

  shl_1_test( (uint8_t*)tg , (const uint8_t *)g1 , 1024 );
  shl_2_test( (uint8_t*)ht , (const uint8_t *)g2 , 1024 );
  add( (uint8_t*)tg , (const uint8_t*)tg , (const uint8_t*)ht , 1024 );
  uint8_t tg1 = (g1[128-1]>>63)^(g2[128-1]>>62); // 2bits

  uint8_t tf1_mul_tg1 = (tf1*(tg1&1))^(tf1*(tg1&2)); // 3bits

// ht
  uint64_t tmp_f[128];
  uint64_t tmp_g[128];

  add( (uint8_t*)tmp_f , (const uint8_t*)tf , (const uint8_t*)f0 , 1024 );
  add( (uint8_t*)tmp_g , (const uint8_t*)tg , (const uint8_t*)g0 , 1024 );
  gf2x_mul_8192( ht , tmp_f , tmp_g );
  _madd_2bits_test( (uint8_t*)(ht+128) , (const uint8_t*)tmp_f , tg1 , 1024 );
  _madd_2bits_test( (uint8_t*)(ht+128) , (const uint8_t*)tmp_g , tf1 , 1024 );
  uint8_t ht_high = tf1_mul_tg1 ^ ((tmp_f[128-1]>>63)&(tg1>>1)) ^ ((tmp_g[128-1]>>63)&(tf1>>1));

// ht1
  add( (uint8_t*)tmp_f , (const uint8_t*)tf , (const uint8_t*)f012 , 1024 );
  add( (uint8_t*)tmp_g , (const uint8_t*)tg , (const uint8_t*)g012 , 1024 );
  gf2x_mul_8192( ht1 , tmp_f , tmp_g );
  _madd_2bits_test( (uint8_t*)(ht1+128) , (const uint8_t*)tmp_f , tg1 , 1024 );
  _madd_2bits_test( (uint8_t*)(ht1+128) , (const uint8_t*)tmp_g , tf1 , 1024 );
  uint8_t ht1_high = tf1_mul_tg1 ^ ((tmp_f[128-1]>>63)&(tg1>>1)) ^ ((tmp_g[128-1]>>63)&(tf1>>1));

// ht_ht1n
  add( (uint8_t*)ht1 , (const uint8_t*)ht1 , (const uint8_t*)ht , 2048 );
  ht1_high ^= ht_high;  // 1bit

// h1
  gf2x_mul_8192( h1 , f012 , g012 );
// h0
  gf2x_mul_8192( h0 , f0 , g0 );
// hinf
  gf2x_mul_8192( hinf , f2 , g2 );

// U
  cpy( (uint8_t*)c , (const uint8_t*)h0 , 1024 );
  add( (uint8_t*)(c+128) , (const uint8_t*)h0 , (const uint8_t*)h1 , 1024*2 );
  add( (uint8_t*)(c+128) , (const uint8_t*)(c+128) , (const uint8_t*)(h0+128) , 1024 );

// V
  uint64_t V[384+4] = {0};
  cpy( (uint8_t*)V , (const uint8_t*)ht , 2048 );
  V[256] = ht_high;
  add( (uint8_t*)(V+128) , (const uint8_t*)(V+128) , (const uint8_t*)ht1 , 2048 );
//  V[24] = ht1_high;
  uint8_t v_high = ht1_high;

  uint64_t tmp[256];
  shl_1_test( (uint8_t*)tmp , (const uint8_t*)ht1 , 2048 ); // (ht+ht1)<<1
  add( (uint8_t*)V , (const uint8_t*)V , (const uint8_t*)tmp , 2048 );
  V[256] ^= ((ht1[256-1]>>63)^(ht1_high<<1));

// W
  add( (uint8_t*)V , (const uint8_t*)V , (const uint8_t*)c , 3072 );

  shl_2_test( (uint8_t*)tmp , (const uint8_t*)hinf , 2048 );
  uint64_t hinf_shl2_high = (hinf[256-1]>>62);
  uint64_t tmp2[256];
  shr_1_test( (uint8_t*)tmp2 , (const uint8_t*)hinf , 2048 );
  add( (uint8_t*)tmp , (const uint8_t*)tmp , (const uint8_t*)tmp2 , 2048 );

  shr_2_test( (uint8_t*)V , (const uint8_t*)V , 3072 );
  V[384-1] ^= (((uint64_t)v_high)<<62);

  add( (uint8_t*)V , (const uint8_t*)V , (const uint8_t*)tmp , 2048 );
  V[256] ^= hinf_shl2_high;

  div_t2_t_test( (uint8_t*)V , 3072 );

// output
  for(int i=0;i<128;i++) c[384+i]=0;
  cpy( (uint8_t*)(c+512) , (const uint8_t*)hinf , 2048 );
  add( (uint8_t*)(c+128) , (const uint8_t*)(c+128) , (const uint8_t*)hinf , 2048 );
  add( (uint8_t*)(c+256) , (const uint8_t*)(c+256) , (const uint8_t*)V , 3072 );
  add( (uint8_t*)(c+128) , (const uint8_t*)(c+128) , (const uint8_t*)V , 3072 );

#else
  gf2x_mul_24576( c , a , b );
#endif
}


///////////////////////


static inline
void mul_64( uint8_t *c, const uint8_t *a, const uint8_t *b)
{
  __m128i a128 = _mm_loadu_si128((const __m128i*)a);
  __m128i b128 = _mm_loadu_si128((const __m128i*)b);
  __m128i c128 = _mm_clmulepi64_si128( a128 , b128 , 0 );
  _mm_storeu_si128((__m128i*)c,c128);
}

static inline
void madd_64( uint8_t *c, const uint8_t *a, const uint8_t* b, int len_b)
{
  __m128i a128 = _mm_loadu_si128((const __m128i*)a);
  __m128i carry = _mm_setzero_si128();

  for(int i=0;i<len_b;i+=16){
    __m128i b128 = _mm_loadu_si128((const __m128i*)(b+i));
    __m128i c128 = _mm_loadu_si128((const __m128i*)(c+i));
    __m128i c0 = _mm_clmulepi64_si128( b128 , a128 , 0 );
    __m128i c1 = _mm_clmulepi64_si128( b128 , a128 , 1 );
    c128 ^= c0^carry^_mm_slli_si128(c1,8);
    carry = _mm_srli_si128(c1,8);
    _mm_storeu_si128((__m128i*)(c+i),c128);
  }
  __m128i c128 = _mm_loadu_si128((const __m128i*)(c+len_b));
  c128 ^= carry;
  _mm_storeu_si128((__m128i*)(c+len_b),c128);
}

void rkara3_mul_12352(uint64_t *c, const uint64_t *a, const uint64_t *b)
{
#if 1
  rkara3_mul_12288(c,a,b);
  mul_64( (uint8_t*)(c+384), (const uint8_t*)(a+192) , (const uint8_t*)(b+192) );
  madd_64( (uint8_t*)(c+192), (const uint8_t*)(a+192) , (const uint8_t*)b , 1536 );
  madd_64( (uint8_t*)(c+192), (const uint8_t*)(b+192) , (const uint8_t*)a , 1536 );
#else
  gf2x_mul_12352(c,a,b);
#endif
}


static inline
void madd_128( uint8_t *c, const uint8_t *a, const uint8_t* b, int len_b)
{
  __m128i a128 = _mm_loadu_si128((const __m128i*)a);
  __m128i carry = _mm_setzero_si128();

  for(int i=0;i<len_b;i+=16){
    __m128i b128 = _mm_loadu_si128((const __m128i*)(b+i));
    __m128i c128 = _mm_loadu_si128((const __m128i*)(c+i));
    __m128i c0 = _mm_clmulepi64_si128( b128 , a128 , 0 );
    __m128i c1 = _mm_clmulepi64_si128( b128 , a128 , 1 )^_mm_clmulepi64_si128( b128 , a128 , 0x10 );
    c128 ^= c0^carry^_mm_slli_si128(c1,8);
    carry = _mm_srli_si128(c1,8)^_mm_clmulepi64_si128( b128 , a128 , 0x11 );
    _mm_storeu_si128((__m128i*)(c+i),c128);
  }
  __m128i c128 = _mm_loadu_si128((const __m128i*)(c+len_b));
  c128 ^= carry;
  _mm_storeu_si128((__m128i*)(c+len_b),c128);
}


void rkara3_mul_24704(uint64_t *c, const uint64_t *a, const uint64_t *b)
{
#if 1
  rkara3_mul_24576(c,a,b);
  for(int i=0;i<4;i++) c[768+i] = 0;
  madd_128( (uint8_t*)(c+768), (const uint8_t*)(a+384) , (const uint8_t*)(b+384) , 16 );
  madd_128( (uint8_t*)(c+384), (const uint8_t*)(a+384) , (const uint8_t*)b , 3072 );
  madd_128( (uint8_t*)(c+384), (const uint8_t*)(b+384) , (const uint8_t*)a , 3072 );
#else
  gf2x_mul_24704(c,a,b);
#endif
}



