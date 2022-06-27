#include "crypto_core.h"

#include "mult3_32x32.h"

#include "const_inline.h"

#include "immintrin.h"


#define ALIGNED __attribute((aligned(32)))

//
// unsigned input < 128
// unsigned output <= 9
//
//inline __m256i reduce_high( __m256i a ) { return _mm256_sub_epi8(a, _mm256_shuffle_epi8(reduce_3_7, _mm256_srli_epi16(a,3)&v_0xf  ) ); }


// input:
// a: 0 <= values <= 2
// b: 0 <= values <= 2
// output:
// h: 0 <= vlues <= 2
static inline
void mult3_16x16_x2( __m256i * h0 , __m256i* h1 , __m256i a , __m256i b ) {
  __m256i aodd = _mm256_srli_si256(a,4);
  __m256i b_br;

  b_br = _mm256_shuffle_epi32(b  ,0  ); // 00,00,00,00  // b0
  __m256i aeve_b0 = _mm256_mul_epi32(   a, b_br); // <= 16
  __m256i aodd_b0 = _mm256_mul_epi32(aodd, b_br); // <= 16

  b_br = _mm256_shuffle_epi32(b  ,0x55); // 01,01,01,01 // b1
  __m256i aeve_b1 = _mm256_mul_epi32(   a, b_br); // <= 16
  __m256i aodd_b1 = _mm256_mul_epi32(aodd, b_br); // <= 16

  b_br = _mm256_shuffle_epi32(b  ,0xaa); // 10,10,10,10 // b2
  __m256i aeve_b2 = _mm256_mul_epi32(   a, b_br); // <= 16
  __m256i aodd_b2 = _mm256_mul_epi32(aodd, b_br); // <= 16

  b_br = _mm256_shuffle_epi32(b  ,0xff); // 11,11,11,11 // b3
  __m256i aeve_b3 = _mm256_mul_epi32(   a, b_br); // <= 16
  __m256i aodd_b3 = _mm256_mul_epi32(aodd, b_br); // <= 16

//aeve_b0
  aeve_b1 = _mm256_add_epi8( aeve_b1 , aodd_b0 ); // <= 32
  aeve_b2 = _mm256_add_epi8( aeve_b2 , aodd_b1 ); // <= 32
  aeve_b3 = _mm256_add_epi8( aeve_b3 , aodd_b2 ); // <= 32
//aodd_b3

  aeve_b0 = _mm256_add_epi8( aeve_b0 , _mm256_slli_si256(aeve_b1, 4) ); // <= 48
  aodd_b3 = _mm256_add_epi8( aodd_b3 , _mm256_srli_si256(aeve_b1,12) ); // <= 48
  aeve_b0 = _mm256_add_epi8( aeve_b0 , _mm256_slli_si256(aeve_b2, 8) ); // <= 80
  aodd_b3 = _mm256_add_epi8( aodd_b3 , _mm256_srli_si256(aeve_b2, 8) ); // <= 80
  aeve_b0 = _mm256_add_epi8( aeve_b0 , _mm256_slli_si256(aeve_b3,12) ); // <= 112
  aodd_b3 = _mm256_add_epi8( aodd_b3 , _mm256_srli_si256(aeve_b3, 4) ); // <= 112

  aeve_b0 = reduce_high( aeve_b0 ); // <= 9
  aodd_b3 = reduce_high( aodd_b3 ); // <= 9
  aeve_b0 = reduce_low( aeve_b0 ); // <= 2
  aodd_b3 = reduce_low( aodd_b3 ); // <= 2

  _mm256_store_si256( h0 , aeve_b0 );
  _mm256_store_si256( h1 , aodd_b3 );
}


//
// input:
// f: 32 uint8_t,  0 <= values <= 2
// g: 32 uint8_t,  0 <= values <= 2
// output:
// h: 64 uint8_t,  0 <= vlues <= 2
//

static inline
void mult3_32x32_0( __m256i * h0 , __m256i* h1 , __m256i a , __m256i b )
{
  __m256i aeve_b0,aodd_b3;
  mult3_16x16_x2( &aeve_b0 , &aodd_b3 , a , b );

  __m256i aeve_b4,aodd_b7;
  mult3_16x16_x2( &aeve_b4 , &aodd_b7 , a , _mm256_permute4x64_epi64(b,0x4e ) );  // reverse up/down 128-bits

  __m256i c_mid = _mm256_permute2x128_si256(aeve_b0,aodd_b3, 0x12 );
  __m256i c_mid0 = _mm256_permute2x128_si256(aeve_b4,aodd_b7, 0x20 );
  __m256i c_mid1 = _mm256_permute2x128_si256(aeve_b4,aodd_b7, 0x31 );
  c_mid = reduce_low( _mm256_add_epi8( c_mid1 , _mm256_add_epi8(c_mid0,c_mid) ) );

  aeve_b0 = _mm256_permute2x128_si256(aeve_b0,c_mid, 0x20 );
  aodd_b3 = _mm256_permute2x128_si256(c_mid,aodd_b3, 0x31 );

  _mm256_store_si256( h0 , aeve_b0 );
  _mm256_store_si256( h1 , aodd_b3 );
}

void mult3_32x32( unsigned char * h , const unsigned char * f , const unsigned char * g )
{

  __m256i a = _mm256_loadu_si256((__m256i*) f);
  __m256i b = _mm256_loadu_si256((__m256i*) g);
  __m256i c0,c1;
  mult3_32x32_0(&c0 , &c1 , a , b );

  _mm256_storeu_si256( (__m256i*) h , c0 );
  _mm256_storeu_si256( (__m256i*) (h+32) , c1 );
}



static inline
void mult3x2_32x32( __m256i * c0 , __m256i* c1 , __m256i * c2 , __m256i * c3 , __m256i a0 , __m256i a1 , __m256i b0 , __m256i b1 )
{
  __m256i a0a1 = add_r3(a0,a1);
  __m256i b0b1 = add_r3(b0,b1);

  __m256i t0,t1;
  mult3_16x16_x2( &t0 , &t1 , a0 , b0 );

  __m256i t2,t3;
  mult3_16x16_x2( &t2 , &t3 , a1 , b1 );

  __m256i mid0, mid1;
  mult3_16x16_x2( &mid0 , &mid1 , a0a1 , b0b1 );

  mid0 = _mm256_add_epi8( mid0 , reduce_neg(_mm256_add_epi8(t0,t2)) );
  mid1 = _mm256_add_epi8( mid1 , reduce_neg(_mm256_add_epi8(t1,t3)) );
  t1 = add_r3(mid0,t1);
  t2 = add_r3(mid1,t2);

  _mm256_store_si256( c0 , t0 );
  _mm256_store_si256( c1 , t1 );
  _mm256_store_si256( c2 , t2 );
  _mm256_store_si256( c3 , t3 );
}



static
void mult3x2_64x64_inplace_karatsuba( __m256i * a ) {
  __m256i *a0 = a;
  __m256i *b0 = a+2;
  __m256i *a1 = a+4;
  __m256i *b1 = a+6;
  __m256i tmp;

  tmp = b0[0];
  b0[0] = a1[0];
  a1[0] = tmp;
  tmp = b0[1];
  b0[1] = a1[1];
  a1[1] = tmp;

  __m256i ab0,ab1,ab2,ab3;
  ab0 = add_r3( a0[0] , a1[0] );
  ab1 = add_r3( a0[1] , a1[1] );
  ab2 = add_r3( b0[0] , b1[0] );
  ab3 = add_r3( b0[1] , b1[1] );

  mult3x2_32x32( a0 , a0+1 , b0 , b0+1 , a0[0] , a0[1] , b0[0] , b0[1] );
  mult3x2_32x32( a1 , a1+1 , b1 , b1+1 , a1[0] , a1[1] , b1[0] , b1[1] );
  mult3x2_32x32( &ab0 , &ab1 , &ab2 , &ab3 , ab0 , ab1 , ab2 , ab3 );

  ab0 = add_r3( _mm256_add_epi8(ab0,b0[0]) , reduce_neg( _mm256_add_epi8(a0[0],a1[0]) ) );
  ab1 = add_r3( _mm256_add_epi8(ab1,b0[1]) , reduce_neg( _mm256_add_epi8(a0[1],a1[1]) ) );
  ab2 = add_r3( _mm256_add_epi8(ab2,a1[0]) , reduce_neg( _mm256_add_epi8(b0[0],b1[0]) ) );
  ab3 = add_r3( _mm256_add_epi8(ab3,a1[1]) , reduce_neg( _mm256_add_epi8(b0[1],b1[1]) ) );

  a[2] = ab0;
  a[3] = ab1;
  a[4] = ab2;
  a[5] = ab3;
}

static
void mult3_64x64_inplace_karatsuba( __m256i * a ) {

  __m256i ab0,ab1;
  __m256i * b = a + 2;
  ab0 = add_r3(a[0],a[1]);
  ab1 = add_r3(b[0],b[1]);
  mult3_32x32_0( &ab0 , &ab1 , ab0 , ab1 );

  __m256i al = _mm256_permute2x128_si256( a[0] , a[1] , 0x20 );
  __m256i ah = _mm256_permute2x128_si256( a[0] , a[1] , 0x31 );
  __m256i bl = _mm256_permute2x128_si256( b[0] , b[1] , 0x20 );
  __m256i bh = _mm256_permute2x128_si256( b[0] , b[1] , 0x31 );
  mult3x2_32x32( &al , &ah , &bl , &bh , al , ah , bl , bh );
  a[0] = _mm256_permute2x128_si256( al , ah , 0x20 );
  a[1] = _mm256_permute2x128_si256( bl , bh , 0x20 );
  a[2] = _mm256_permute2x128_si256( al , ah , 0x31 );
  a[3] = _mm256_permute2x128_si256( bl , bh , 0x31 );

  ab0 = _mm256_add_epi8( ab0 , reduce_neg( _mm256_add_epi8(a[0],b[0]) ) ); // <= 4
  ab1 = _mm256_add_epi8( ab1 , reduce_neg( _mm256_add_epi8(a[1],b[1]) ) ); // <= 4

  a[1] = add_r3( a[1] , ab0 );
  a[2] = add_r3( a[2] , ab1 );
}



static inline
void cvt_2vecs_to_vecx2_1( __m256i * r , const __m256i * v0 , const __m256i * v1 )
{
  r[0] = _mm256_permute2x128_si256( v0[0] , v1[0] , 0x20 );
  r[1] = _mm256_permute2x128_si256( v0[0] , v1[0] , 0x31 );
}

static inline
void cvt_2vecs_to_vecx2_2( __m256i * r , const __m256i * v0 , const __m256i * v1 )
{
  r[0] = _mm256_permute2x128_si256( v0[0] , v1[0] , 0x20 );
  r[1] = _mm256_permute2x128_si256( v0[0] , v1[0] , 0x31 );
  r[2] = _mm256_permute2x128_si256( v0[1] , v1[1] , 0x20 );
  r[3] = _mm256_permute2x128_si256( v0[1] , v1[1] , 0x31 );
}

static inline
void cvt_2vecs_to_vecx2_4( __m256i * r , const __m256i * v0 , const __m256i * v1 )
{
  r[0] = _mm256_permute2x128_si256( v0[0] , v1[0] , 0x20 );
  r[1] = _mm256_permute2x128_si256( v0[0] , v1[0] , 0x31 );
  r[2] = _mm256_permute2x128_si256( v0[1] , v1[1] , 0x20 );
  r[3] = _mm256_permute2x128_si256( v0[1] , v1[1] , 0x31 );
  r[4] = _mm256_permute2x128_si256( v0[2] , v1[2] , 0x20 );
  r[5] = _mm256_permute2x128_si256( v0[2] , v1[2] , 0x31 );
  r[6] = _mm256_permute2x128_si256( v0[3] , v1[3] , 0x20 );
  r[7] = _mm256_permute2x128_si256( v0[3] , v1[3] , 0x31 );
}

static inline
void cvt_vecx2_to_2vecs_2( __m256i * r0 , __m256i * r1 , const __m256i * vec )
{
  r0[0] = _mm256_permute2x128_si256( vec[0] , vec[1] , 0x20 );
  r1[0] = _mm256_permute2x128_si256( vec[0] , vec[1] , 0x31 );
  r0[1] = _mm256_permute2x128_si256( vec[2] , vec[3] , 0x20 );
  r1[1] = _mm256_permute2x128_si256( vec[2] , vec[3] , 0x31 );
}

static inline
void cvt_vecx2_to_2vecs_4( __m256i * r0 , __m256i * r1 , const __m256i * vec )
{
  r0[0] = _mm256_permute2x128_si256( vec[0] , vec[1] , 0x20 );
  r1[0] = _mm256_permute2x128_si256( vec[0] , vec[1] , 0x31 );
  r0[1] = _mm256_permute2x128_si256( vec[2] , vec[3] , 0x20 );
  r1[1] = _mm256_permute2x128_si256( vec[2] , vec[3] , 0x31 );
  r0[2] = _mm256_permute2x128_si256( vec[4] , vec[5] , 0x20 );
  r1[2] = _mm256_permute2x128_si256( vec[4] , vec[5] , 0x31 );
  r0[3] = _mm256_permute2x128_si256( vec[6] , vec[7] , 0x20 );
  r1[3] = _mm256_permute2x128_si256( vec[6] , vec[7] , 0x31 );
}

static inline
void mult3_64x64_inplace_karatsuba_x2( __m256i * a0 , __m256i * a1) {
  __m256i f2_f0[8];

  cvt_2vecs_to_vecx2_4( f2_f0 , a0 , a1 );
  mult3x2_64x64_inplace_karatsuba( f2_f0 );
  cvt_vecx2_to_2vecs_4( a0 , a1 , f2_f0 );
}




static
void mult3_128x128_refined_karatsuba( __m256i * a ) {
  __m256i * f = a;
  __m256i * g = a + 4;

  __m256i f0g0[2];
  __m256i f1g1[2];
  __m256i f2g2[2];
  __m256i f3g3[2];
  __m256i f0f1g0g1[2];
  __m256i f2f3g2g3[2];

  {
  __m256i f2_f0[4];
  __m256i *g2_g0 = f2_f0 + 2;
  __m256i f3_f1[4];
  __m256i *g3_g1 = f3_f1 + 2;
  __m256i f2f3_f0f1[4];
  __m256i *g2g3_g0g1 = f2f3_f0f1 + 2;

  cvt_2vecs_to_vecx2_1( f2_f0 , f , f+2 );
  cvt_2vecs_to_vecx2_1( g2_g0 , g , g+2 );
  cvt_2vecs_to_vecx2_1( f3_f1 , f+1 , f+3 );
  cvt_2vecs_to_vecx2_1( g3_g1 , g+1 , g+3 );

  f2f3_f0f1[0] = add_r3( f2_f0[0] , f3_f1[0] );
  f2f3_f0f1[1] = add_r3( f2_f0[1] , f3_f1[1] );
  g2g3_g0g1[0] = add_r3( g2_g0[0] , g3_g1[0] );
  g2g3_g0g1[1] = add_r3( g2_g0[1] , g3_g1[1] );

  mult3x2_32x32( f2_f0 , f2_f0+1 , g2_g0 , g2_g0+1 , f2_f0[0] , f2_f0[1] , g2_g0[0] , g2_g0[1] );
  mult3x2_32x32( f3_f1 , f3_f1+1 , g3_g1 , g3_g1+1 , f3_f1[0] , f3_f1[1] , g3_g1[0] , g3_g1[1] );
  mult3x2_32x32( f2f3_f0f1 , f2f3_f0f1+1 , g2g3_g0g1 , g2g3_g0g1+1 , f2f3_f0f1[0] , f2f3_f0f1[1] , g2g3_g0g1[0] , g2g3_g0g1[1] );

  cvt_vecx2_to_2vecs_2( f0g0 , f2g2 , f2_f0 );
  cvt_vecx2_to_2vecs_2( f1g1 , f3g3 , f3_f1 );
  cvt_vecx2_to_2vecs_2( f0f1g0g1 , f2f3g2g3 , f2f3_f0f1 );
  }

  __m256i f0f2_f1f3_g0g2_g1g3[4];
  f0f2_f1f3_g0g2_g1g3[0] = add_r3(f[0],f[2]);
  f0f2_f1f3_g0g2_g1g3[1] = add_r3(f[1],f[3]);
  f0f2_f1f3_g0g2_g1g3[2] = add_r3(g[0],g[2]);
  f0f2_f1f3_g0g2_g1g3[3] = add_r3(g[1],g[3]);

  mult3_64x64_inplace_karatsuba( f0f2_f1f3_g0g2_g1g3 );



  // U = f0g0 - t1 f1g1 - t2 f2g2 + t3 f3g3
  a[0] = f0g0[0];  // <= 2
  a[1] = _mm256_add_epi8( f0g0[1] , reduce_neg(f1g1[0]) ); // <= 4
  a[2] = reduce_neg( _mm256_add_epi8(f1g1[1],f2g2[0]) ); // <= 2
  a[3] = _mm256_add_epi8( f3g3[0] , reduce_neg(f2g2[1]) ); // <= 4
  a[4] = f3g3[1]; // <= 2

  // V = (1-t1)U + t1(f0f1)(g0g1) - t3(f2f3)(g2g3)
  a[5] = reduce_neg( a[4] ); // <= 2
  a[4] = _mm256_add_epi8( a[4] , reduce_neg( a[3] ) ); // <= 4
  a[3] = _mm256_add_epi8( a[3] , reduce_neg( a[2] ) ); // <= 6
  a[2] = _mm256_add_epi8( a[2] , reduce_neg( a[1] ) ); // <= 4
  a[1] = _mm256_add_epi8( a[1] , reduce_neg( a[0] ) ); // <= 6

  a[1] = add_r3( a[1] , f0f1g0g1[0] ); // <= 2
  a[2] = _mm256_add_epi8( a[2] , f0f1g0g1[1] ); // <= 6
  a[3] = _mm256_add_epi8( a[3] , reduce_neg(f2f3g2g3[0]) ); // <= 8
  a[4] = _mm256_add_epi8( a[4] , reduce_neg(f2f3g2g3[1]) ); // <= 6

  // (1-t2)V + t2(f0f2_f1f3_g0g2_g1g3)
  a[7] = reduce_neg(a[5]); // <= 2
  a[6] = reduce_neg(a[4]); // <= 2
  a[5] = _mm256_add_epi8( a[5] , reduce_neg( a[3] ) ); // <= 4
  a[4] = _mm256_add_epi8( a[4] , reduce_neg( a[2] ) ); // <= 8
  a[3] = _mm256_add_epi8( a[3] , reduce_neg( a[1] ) ); // <= 10
  a[2] = _mm256_add_epi8( a[2] , reduce_neg( a[0] ) ); // <= 8

  a[2] = add_r3( a[2] , f0f2_f1f3_g0g2_g1g3[0] );
  a[3] = add_r3( a[3] , f0f2_f1f3_g0g2_g1g3[1] );
  a[4] = add_r3( a[4] , f0f2_f1f3_g0g2_g1g3[2] );
  a[5] = add_r3( a[5] , f0f2_f1f3_g0g2_g1g3[3] );

}


static
void mult3x2_128x128_refined_karatsuba( __m256i * a ) {
  __m256i * f = a;
  __m256i * g = a + 8;

  __m256i f0g0[4];
  __m256i f1g1[4];
  __m256i f2g2[4];
  __m256i f3g3[4];
  __m256i f0f1g0g1[4];
  __m256i f2f3g2g3[4];

  f0f1g0g1[0] = add_r3( f[0] , f[2] );
  f0f1g0g1[1] = add_r3( f[1] , f[3] );
  f0f1g0g1[2] = add_r3( g[0] , g[2] );
  f0f1g0g1[3] = add_r3( g[1] , g[3] );

  f2f3g2g3[0] = add_r3( f[4] , f[6] );
  f2f3g2g3[1] = add_r3( f[5] , f[7] );
  f2f3g2g3[2] = add_r3( g[4] , g[6] );
  f2f3g2g3[3] = add_r3( g[5] , g[7] );

  mult3x2_32x32( f0g0 , f0g0+1 , f0g0+2 , f0g0+3 , f[0] , f[1] , g[0] , g[1] );
  mult3x2_32x32( f1g1 , f1g1+1 , f1g1+2 , f1g1+3 , f[2] , f[3] , g[2] , g[3] );
  mult3x2_32x32( f2g2 , f2g2+1 , f2g2+2 , f2g2+3 , f[4] , f[5] , g[4] , g[5] );
  mult3x2_32x32( f3g3 , f3g3+1 , f3g3+2 , f3g3+3 , f[6] , f[7] , g[6] , g[7] );
  mult3x2_32x32( f0f1g0g1, f0f1g0g1+1 , f0f1g0g1+2 , f0f1g0g1+3 , f0f1g0g1[0] , f0f1g0g1[1] , f0f1g0g1[2] , f0f1g0g1[3] );
  mult3x2_32x32( f2f3g2g3, f2f3g2g3+1 , f2f3g2g3+2 , f2f3g2g3+3 , f2f3g2g3[0] , f2f3g2g3[1] , f2f3g2g3[2] , f2f3g2g3[3] );

  __m256i f0f2_f1f3_g0g2_g1g3[8];
  f0f2_f1f3_g0g2_g1g3[0] = add_r3(f[0],f[4]);
  f0f2_f1f3_g0g2_g1g3[1] = add_r3(f[1],f[5]);
  f0f2_f1f3_g0g2_g1g3[2] = add_r3(f[2],f[6]);
  f0f2_f1f3_g0g2_g1g3[3] = add_r3(f[3],f[7]);
  f0f2_f1f3_g0g2_g1g3[4] = add_r3(g[0],g[4]);
  f0f2_f1f3_g0g2_g1g3[5] = add_r3(g[1],g[5]);
  f0f2_f1f3_g0g2_g1g3[6] = add_r3(g[2],g[6]);
  f0f2_f1f3_g0g2_g1g3[7] = add_r3(g[3],g[7]);

  mult3x2_64x64_inplace_karatsuba( f0f2_f1f3_g0g2_g1g3 );

  // U = f0g0 - t1 f1g1 - t2 f2g2 + t3 f3g3
  a[0] = f0g0[0];  // <= 2
  a[1] = f0g0[1];  // <= 2
  a[2] = _mm256_add_epi8( f0g0[2] , reduce_neg(f1g1[0]) ); // <= 4
  a[3] = _mm256_add_epi8( f0g0[3] , reduce_neg(f1g1[1]) ); // <= 4
  a[4] = reduce_neg( _mm256_add_epi8(f1g1[2],f2g2[0]) ); // <= 2
  a[5] = reduce_neg( _mm256_add_epi8(f1g1[3],f2g2[1]) ); // <= 2
  a[6] = _mm256_add_epi8( f3g3[0] , reduce_neg(f2g2[2]) ); // <= 4
  a[7] = _mm256_add_epi8( f3g3[1] , reduce_neg(f2g2[3]) ); // <= 4
  a[8] = f3g3[2]; // <= 2
  a[9] = f3g3[3]; // <= 2

  // V = (1-t1)U + t1(f0f1)(g0g1) - t3(f2f3)(g2g3)
  a[10] = reduce_neg( a[8] ); // <= 2
  a[11] = reduce_neg( a[9] ); // <= 2
  a[8] = _mm256_add_epi8( a[8] , reduce_neg( a[6] ) ); // <= 4
  a[9] = _mm256_add_epi8( a[9] , reduce_neg( a[7] ) ); // <= 4
  a[6] = _mm256_add_epi8( a[6] , reduce_neg( a[4] ) ); // <= 6
  a[7] = _mm256_add_epi8( a[7] , reduce_neg( a[5] ) ); // <= 6
  a[4] = _mm256_add_epi8( a[4] , reduce_neg( a[2] ) ); // <= 4
  a[5] = _mm256_add_epi8( a[5] , reduce_neg( a[3] ) ); // <= 4
  a[2] = _mm256_add_epi8( a[2] , reduce_neg( a[0] ) ); // <= 6
  a[3] = _mm256_add_epi8( a[3] , reduce_neg( a[1] ) ); // <= 6

  a[2] = add_r3( a[2] , f0f1g0g1[0] ); // <= 2
  a[3] = add_r3( a[3] , f0f1g0g1[1] ); // <= 2
  a[4] = _mm256_add_epi8( a[4] , f0f1g0g1[2] ); // <= 6
  a[5] = _mm256_add_epi8( a[5] , f0f1g0g1[3] ); // <= 6
  a[6] = _mm256_add_epi8( a[6] , reduce_neg(f2f3g2g3[0]) ); // <= 8
  a[7] = _mm256_add_epi8( a[7] , reduce_neg(f2f3g2g3[1]) ); // <= 8
  a[8] = _mm256_add_epi8( a[8] , reduce_neg(f2f3g2g3[2]) ); // <= 6
  a[9] = _mm256_add_epi8( a[9] , reduce_neg(f2f3g2g3[3]) ); // <= 6

  // (1-t2)V + t2(f0f2_f1f3_g0g2_g1g3)
  a[14] = reduce_neg(a[10]); // <= 2
  a[15] = reduce_neg(a[11]); // <= 2
  a[12] = reduce_neg(a[8]); // <= 2
  a[13] = reduce_neg(a[9]); // <= 2
  a[10] = _mm256_add_epi8( a[10] , reduce_neg( a[6] ) ); // <= 4
  a[11] = _mm256_add_epi8( a[11] , reduce_neg( a[7] ) ); // <= 4
  a[8] = _mm256_add_epi8( a[8] , reduce_neg( a[4] ) ); // <= 8
  a[9] = _mm256_add_epi8( a[9] , reduce_neg( a[5] ) ); // <= 8
  a[6] = _mm256_add_epi8( a[6] , reduce_neg( a[2] ) ); // <= 10
  a[7] = _mm256_add_epi8( a[7] , reduce_neg( a[3] ) ); // <= 10
  a[4] = _mm256_add_epi8( a[4] , reduce_neg( a[0] ) ); // <= 8
  a[5] = _mm256_add_epi8( a[5] , reduce_neg( a[1] ) ); // <= 8

  a[4] = add_r3( a[4] , f0f2_f1f3_g0g2_g1g3[0] );
  a[5] = add_r3( a[5] , f0f2_f1f3_g0g2_g1g3[1] );
  a[6] = add_r3( a[6] , f0f2_f1f3_g0g2_g1g3[2] );
  a[7] = add_r3( a[7] , f0f2_f1f3_g0g2_g1g3[3] );
  a[8] = add_r3( a[8] , f0f2_f1f3_g0g2_g1g3[4] );
  a[9] = add_r3( a[9] , f0f2_f1f3_g0g2_g1g3[5] );
  a[10] = add_r3( a[10] , f0f2_f1f3_g0g2_g1g3[6] );
  a[11] = add_r3( a[11] , f0f2_f1f3_g0g2_g1g3[7] );
}




static
void mult3_256x256_refined_karatsuba( __m256i * a ) {
  __m256i * f = a;
  __m256i * g = a + 8;

  __m256i f0g0[4];
  __m256i f1g1[4];
  __m256i f2g2[4];
  __m256i f3g3[4];
  __m256i f0f1g0g1[4];
  __m256i f2f3g2g3[4];

  {
  __m256i f2_f0[8];
  __m256i *g2_g0 = f2_f0 + 4;
  __m256i f3_f1[8];
  __m256i *g3_g1 = f3_f1 + 4;
  __m256i f2f3_f0f1[8];
  __m256i *g2g3_g0g1 = f2f3_f0f1 + 4;

  cvt_2vecs_to_vecx2_2( f2_f0 , f , f+4 );
  cvt_2vecs_to_vecx2_2( f3_f1 , f+2 , f+6 );
  cvt_2vecs_to_vecx2_2( g2_g0 , g , g+4 );
  cvt_2vecs_to_vecx2_2( g3_g1 , g+2 , g+6 );

  f2f3_f0f1[0] = add_r3( f2_f0[0] , f3_f1[0] );
  f2f3_f0f1[1] = add_r3( f2_f0[1] , f3_f1[1] );
  f2f3_f0f1[2] = add_r3( f2_f0[2] , f3_f1[2] );
  f2f3_f0f1[3] = add_r3( f2_f0[3] , f3_f1[3] );
  g2g3_g0g1[0] = add_r3( g2_g0[0] , g3_g1[0] );
  g2g3_g0g1[1] = add_r3( g2_g0[1] , g3_g1[1] );
  g2g3_g0g1[2] = add_r3( g2_g0[2] , g3_g1[2] );
  g2g3_g0g1[3] = add_r3( g2_g0[3] , g3_g1[3] );

  mult3x2_64x64_inplace_karatsuba( f2_f0 );
  mult3x2_64x64_inplace_karatsuba( f3_f1 );
  mult3x2_64x64_inplace_karatsuba( f2f3_f0f1 );

  cvt_vecx2_to_2vecs_4( f0g0 , f2g2 , f2_f0 );
  cvt_vecx2_to_2vecs_4( f1g1 , f3g3 , f3_f1 );
  cvt_vecx2_to_2vecs_4( f0f1g0g1 , f2f3g2g3 , f2f3_f0f1 );
  }

  __m256i f0f2_f1f3_g0g2_g1g3[8];
  f0f2_f1f3_g0g2_g1g3[0] = add_r3(f[0],f[4]);
  f0f2_f1f3_g0g2_g1g3[1] = add_r3(f[1],f[5]);
  f0f2_f1f3_g0g2_g1g3[2] = add_r3(f[2],f[6]);
  f0f2_f1f3_g0g2_g1g3[3] = add_r3(f[3],f[7]);
  f0f2_f1f3_g0g2_g1g3[4] = add_r3(g[0],g[4]);
  f0f2_f1f3_g0g2_g1g3[5] = add_r3(g[1],g[5]);
  f0f2_f1f3_g0g2_g1g3[6] = add_r3(g[2],g[6]);
  f0f2_f1f3_g0g2_g1g3[7] = add_r3(g[3],g[7]);

  mult3_128x128_refined_karatsuba( f0f2_f1f3_g0g2_g1g3 );

  // U = f0g0 - t1 f1g1 - t2 f2g2 + t3 f3g3
  a[0] = f0g0[0];  // <= 2
  a[1] = f0g0[1];  // <= 2
  a[2] = _mm256_add_epi8( f0g0[2] , reduce_neg(f1g1[0]) ); // <= 4
  a[3] = _mm256_add_epi8( f0g0[3] , reduce_neg(f1g1[1]) ); // <= 4
  a[4] = reduce_neg( _mm256_add_epi8(f1g1[2],f2g2[0]) ); // <= 2
  a[5] = reduce_neg( _mm256_add_epi8(f1g1[3],f2g2[1]) ); // <= 2
  a[6] = _mm256_add_epi8( f3g3[0] , reduce_neg(f2g2[2]) ); // <= 4
  a[7] = _mm256_add_epi8( f3g3[1] , reduce_neg(f2g2[3]) ); // <= 4
  a[8] = f3g3[2]; // <= 2
  a[9] = f3g3[3]; // <= 2

  // V = (1-t1)U + t1(f0f1)(g0g1) - t3(f2f3)(g2g3)
  a[10] = reduce_neg( a[8] ); // <= 2
  a[11] = reduce_neg( a[9] ); // <= 2
  a[8] = _mm256_add_epi8( a[8] , reduce_neg( a[6] ) ); // <= 4
  a[9] = _mm256_add_epi8( a[9] , reduce_neg( a[7] ) ); // <= 4
  a[6] = _mm256_add_epi8( a[6] , reduce_neg( a[4] ) ); // <= 6
  a[7] = _mm256_add_epi8( a[7] , reduce_neg( a[5] ) ); // <= 6
  a[4] = _mm256_add_epi8( a[4] , reduce_neg( a[2] ) ); // <= 4
  a[5] = _mm256_add_epi8( a[5] , reduce_neg( a[3] ) ); // <= 4
  a[2] = _mm256_add_epi8( a[2] , reduce_neg( a[0] ) ); // <= 6
  a[3] = _mm256_add_epi8( a[3] , reduce_neg( a[1] ) ); // <= 6

  a[2] = add_r3( a[2] , f0f1g0g1[0] ); // <= 2
  a[3] = add_r3( a[3] , f0f1g0g1[1] ); // <= 2
  a[4] = _mm256_add_epi8( a[4] , f0f1g0g1[2] ); // <= 6
  a[5] = _mm256_add_epi8( a[5] , f0f1g0g1[3] ); // <= 6
  a[6] = _mm256_add_epi8( a[6] , reduce_neg(f2f3g2g3[0]) ); // <= 8
  a[7] = _mm256_add_epi8( a[7] , reduce_neg(f2f3g2g3[1]) ); // <= 8
  a[8] = _mm256_add_epi8( a[8] , reduce_neg(f2f3g2g3[2]) ); // <= 6
  a[9] = _mm256_add_epi8( a[9] , reduce_neg(f2f3g2g3[3]) ); // <= 6

  // (1-t2)V + t2(f0f2_f1f3_g0g2_g1g3)
  a[14] = reduce_neg(a[10]); // <= 2
  a[15] = reduce_neg(a[11]); // <= 2
  a[12] = reduce_neg(a[8]); // <= 2
  a[13] = reduce_neg(a[9]); // <= 2
  a[10] = _mm256_add_epi8( a[10] , reduce_neg( a[6] ) ); // <= 4
  a[11] = _mm256_add_epi8( a[11] , reduce_neg( a[7] ) ); // <= 4
  a[8] = _mm256_add_epi8( a[8] , reduce_neg( a[4] ) ); // <= 8
  a[9] = _mm256_add_epi8( a[9] , reduce_neg( a[5] ) ); // <= 8
  a[6] = _mm256_add_epi8( a[6] , reduce_neg( a[2] ) ); // <= 10
  a[7] = _mm256_add_epi8( a[7] , reduce_neg( a[3] ) ); // <= 10
  a[4] = _mm256_add_epi8( a[4] , reduce_neg( a[0] ) ); // <= 8
  a[5] = _mm256_add_epi8( a[5] , reduce_neg( a[1] ) ); // <= 8

  a[4] = add_r3( a[4] , f0f2_f1f3_g0g2_g1g3[0] );
  a[5] = add_r3( a[5] , f0f2_f1f3_g0g2_g1g3[1] );
  a[6] = add_r3( a[6] , f0f2_f1f3_g0g2_g1g3[2] );
  a[7] = add_r3( a[7] , f0f2_f1f3_g0g2_g1g3[3] );
  a[8] = add_r3( a[8] , f0f2_f1f3_g0g2_g1g3[4] );
  a[9] = add_r3( a[9] , f0f2_f1f3_g0g2_g1g3[5] );
  a[10] = add_r3( a[10] , f0f2_f1f3_g0g2_g1g3[6] );
  a[11] = add_r3( a[11] , f0f2_f1f3_g0g2_g1g3[7] );

}




static
void mult3x2_256x256_refined_karatsuba( __m256i * a ) {
  __m256i * f = a;
  __m256i * g = a + 16;

  __m256i f0g0[8];
  __m256i f1g1[8];
  __m256i f2g2[8];
  __m256i f3g3[8];
  __m256i f0f1g0g1[8];
  __m256i f2f3g2g3[8];

  for(int i=0;i<4;i++) f0g0[i]   = f[i];
  for(int i=0;i<4;i++) f0g0[4+i] = g[i];
  for(int i=0;i<4;i++) f1g1[i]   = f[4+i];
  for(int i=0;i<4;i++) f1g1[4+i] = g[4+i];
  for(int i=0;i<4;i++) f2g2[i]   = f[8+i];
  for(int i=0;i<4;i++) f2g2[4+i] = g[8+i];
  for(int i=0;i<4;i++) f3g3[i]   = f[12+i];
  for(int i=0;i<4;i++) f3g3[4+i] = g[12+i];

  for(int i=0;i<4;i++) f0f1g0g1[i]   = add_r3( f[i] , f[4+i] );
  for(int i=0;i<4;i++) f0f1g0g1[4+i] = add_r3( g[i] , g[4+i] );
  for(int i=0;i<4;i++) f2f3g2g3[i]   = add_r3( f[8+i] , f[12+i] );
  for(int i=0;i<4;i++) f2f3g2g3[4+i] = add_r3( g[8+i] , g[12+i] );

  mult3x2_64x64_inplace_karatsuba( f0g0 );
  mult3x2_64x64_inplace_karatsuba( f1g1 );
  mult3x2_64x64_inplace_karatsuba( f2g2 );
  mult3x2_64x64_inplace_karatsuba( f3g3 );
  mult3x2_64x64_inplace_karatsuba( f0f1g0g1 );
  mult3x2_64x64_inplace_karatsuba( f2f3g2g3 );

  __m256i f0f2_f1f3_g0g2_g1g3[16];
  for(int i=0;i<4;i++) f0f2_f1f3_g0g2_g1g3[i]    = add_r3(f[i],f[8+i]);
  for(int i=0;i<4;i++) f0f2_f1f3_g0g2_g1g3[4+i]  = add_r3(f[4+i],f[12+i]);
  for(int i=0;i<4;i++) f0f2_f1f3_g0g2_g1g3[8+i]  = add_r3(g[i],g[8+i]);
  for(int i=0;i<4;i++) f0f2_f1f3_g0g2_g1g3[12+i] = add_r3(g[4+i],g[12+i]);

  mult3x2_128x128_refined_karatsuba( f0f2_f1f3_g0g2_g1g3 );

  // U = f0g0 - t1 f1g1 - t2 f2g2 + t3 f3g3
  for(int i=0;i<4;i++) a[0+i] = f0g0[0+i];  // <= 2
  for(int i=0;i<4;i++) a[4+i] = _mm256_add_epi8( f0g0[4+i] , reduce_neg(f1g1[0+i]) ); // <= 4
  for(int i=0;i<4;i++) a[8+i] = reduce_neg( _mm256_add_epi8(f1g1[4+i],f2g2[0+i]) ); // <= 2
  for(int i=0;i<4;i++) a[12+i] = _mm256_add_epi8( f3g3[0+i] , reduce_neg(f2g2[4+i]) ); // <= 4
  for(int i=0;i<4;i++) a[16+i] = f3g3[4+i]; // <= 2

  // V = (1-t1)U + t1(f0f1)(g0g1) - t3(f2f3)(g2g3)
  for(int i=0;i<4;i++) a[20+i] = reduce_neg( a[16+i] ); // <= 2
  for(int i=0;i<4;i++) a[16+i] = _mm256_add_epi8( a[16+i] , reduce_neg( a[12+i] ) ); // <= 4
  for(int i=0;i<4;i++) a[12+i] = _mm256_add_epi8( a[12+i] , reduce_neg( a[8+i] ) ); // <= 6
  for(int i=0;i<4;i++) a[8+i] = _mm256_add_epi8( a[8+i] , reduce_neg( a[4+i] ) ); // <= 4
  for(int i=0;i<4;i++) a[4+i] = _mm256_add_epi8( a[4+i] , reduce_neg( a[0+i] ) ); // <= 6
  for(int i=0;i<4;i++) a[4+i] = add_r3( a[4+i] , f0f1g0g1[0+i] ); // <= 2
  for(int i=0;i<4;i++) a[8+i] = _mm256_add_epi8( a[8+i] , f0f1g0g1[4+i] ); // <= 6
  for(int i=0;i<4;i++) a[12+i] = _mm256_add_epi8( a[12+i] , reduce_neg(f2f3g2g3[0+i]) ); // <= 8
  for(int i=0;i<4;i++) a[16+i] = _mm256_add_epi8( a[16+i] , reduce_neg(f2f3g2g3[4+i]) ); // <= 6

  // (1-t2)V + t2(f0f2_f1f3_g0g2_g1g3)
  for(int i=0;i<4;i++) a[28+i] = reduce_neg(a[20+i]); // <= 2
  for(int i=0;i<4;i++) a[24+i] = reduce_neg(a[16+i]); // <= 2
  for(int i=0;i<4;i++) a[20+i] = _mm256_add_epi8( a[20+i] , reduce_neg( a[12+i] ) ); // <= 4
  for(int i=0;i<4;i++) a[16+i] = _mm256_add_epi8( a[16+i] , reduce_neg( a[8+i] ) ); // <= 8
  for(int i=0;i<4;i++) a[12+i] = _mm256_add_epi8( a[12+i] , reduce_neg( a[4+i] ) ); // <= 10
  for(int i=0;i<4;i++) a[8+i] = _mm256_add_epi8( a[8+i] , reduce_neg( a[0+i] ) ); // <= 8

  for(int i=0;i<16;i++) a[8+i] = add_r3( a[8+i] , f0f2_f1f3_g0g2_g1g3[i] );
}



static inline
void cvt_2vecs_to_vecx2( __m256i * r , const __m256i * v0 , const __m256i * v1 , int len ) {
  for(int i=0;i<len;i++) {
    r[i*2+0] = _mm256_permute2x128_si256( v0[i] , v1[i] , 0x20 );
    r[i*2+1] = _mm256_permute2x128_si256( v0[i] , v1[i] , 0x31 );
  }
}

static inline
void cvt_vecx2_to_2vecs( __m256i * r0 , __m256i * r1 , const __m256i * vec , int len ) {
  for(int i=0;i<len;i++) {
    r0[i] = _mm256_permute2x128_si256( vec[i*2+0] , vec[i*2+1] , 0x20 );
    r1[i] = _mm256_permute2x128_si256( vec[i*2+0] , vec[i*2+1] , 0x31 );
  }
}


static inline
void mult3_256x256_refined_karatsuba_x2( __m256i * a , __m256i * b)
{
  __m256i vec[32];
  cvt_2vecs_to_vecx2( vec , a , b , 16 );
  mult3x2_256x256_refined_karatsuba( vec );
  cvt_vecx2_to_2vecs( a , b , vec , 16 );
}



static
void mult3_256x256_inplace_karatsuba( __m256i * a ) {
  static const unsigned len = 8;
  static const unsigned len_2 = 4;
  __m256i * b = a + len;

  for(unsigned i=0;i<len_2;i++){
    __m256i tmp = a[len_2+i];
    a[len_2+i] = b[i];
    b[i] = tmp;
  }
  __m256i ab[len];
  for(unsigned i=0;i<len;i++) {
    ab[i] = add_r3(a[i],b[i]);
  }

  mult3_128x128_refined_karatsuba( a );
  mult3_128x128_refined_karatsuba( b );
  mult3_128x128_refined_karatsuba( ab );

  for(unsigned i=0;i<len;i++){
    ab[i] = _mm256_add_epi8( ab[i] , reduce_neg( _mm256_add_epi8(a[i],b[i]) ) ); // <= 4
  }
  for(unsigned i=0;i<len;i++){
    a[len_2+i] = add_r3( a[len_2+i] , ab[i] );
  }
}



//////////////////////////////////////////////////////////////////////////////


#define _div_xsq_1_4 _mm256_set_epi8( -1,-1,15,14, -1,-1,11,10, -1,-1,7,6, -1,-1,3,2,   -1,-1,15,14, -1,-1,11,10, -1,-1,7,6, -1,-1,3,2 )
#define _div_xsq_1_8 _mm256_set_epi8( -1,-1,-1,-1, 13,12,13,12, -1,-1,-1,-1, 5,4,5,4,   -1,-1,-1,-1, 13,12,13,12, -1,-1,-1,-1, 5,4,5,4 )
#define _div_xsq_1_16 _mm256_set_epi8( -1,-1,-1,-1, -1,-1,-1,-1, 9,8,9,8, 9,8,9,8,   -1,-1,-1,-1, -1,-1,-1,-1, 9,8,9,8, 9,8,9,8 )
#define v_0x0100 _mm256_set1_epi16( 0x100 )

static inline
__m256i parallel32_div_xsq_1( __m256i a0 )
{
  __m256i b0 = _mm256_add_epi8(a0,_mm256_shuffle_epi8(a0,_div_xsq_1_4)); // <= 4
  __m256i c0 = reduce_low( _mm256_add_epi8(b0,_mm256_shuffle_epi8(b0,_div_xsq_1_8)) ); // <= 2
  __m256i d0 = _mm256_add_epi8(c0,_mm256_shuffle_epi8(c0,_div_xsq_1_16)); // <= 4
  __m256i _0_d0h = _mm256_permute2x128_si256( d0, d0, 0x81 );
  __m256i e0 = reduce_low( _mm256_add_epi8(d0,_mm256_shuffle_epi8(_0_d0h,v_0x0100)) ); // <= 2
  return e0;
}

static
void poly3_512_div_xsq_1_mult_xsq( __m256i* a )
{
  __m256i *st = a;
  __m256i a0,a1,a2,a3;

  for( int i=12;i>=0;i-=4) {
    a0 = _mm256_load_si256( a+i );
    a1 = _mm256_load_si256( a+i+1 );
    a2 = _mm256_load_si256( a+i+2 );
    a3 = _mm256_load_si256( a+i+3 );

    a0 = parallel32_div_xsq_1( a0 );
    a1 = parallel32_div_xsq_1( a1 );
    a2 = parallel32_div_xsq_1( a2 );
    a3 = parallel32_div_xsq_1( a3 );

    _mm256_store_si256(a+i,a0);
    _mm256_store_si256(a+i+1,a1);
    _mm256_store_si256(a+i+2,a2);
    _mm256_store_si256(a+i+3,a3);
  }

  // unit = 1<<(i+1), unit_2 = 1<<i
  for(int i=0;i<=3;i++) {
    unsigned unit_2 = 1<<i;
    for( a = st; a != (st+16); a += (unit_2<<1)) {
      __m256i br = _mm256_broadcastw_epi16( _mm256_castsi256_si128(_mm256_load_si256(a+unit_2)) );
      for(unsigned j=0;j<unit_2;j++) {
        __m256i l0 = _mm256_load_si256(a+j);
        l0 = add_r3( br , l0 );
        _mm256_store_si256(a+j,l0);
      }
    }
  }
}


/////////////////////////////////////////////////////////////////////////////


static inline
__m256i right_shift_1_low( __m256i a1 , __m256i a0 )
{
  __m256i _a1l_a0h = _mm256_permute2x128_si256( a0, a1 , 0x21 );
  return _mm256_alignr_epi8(_a1l_a0h,a0,1);
}

static inline
__m256i right_shift_1( __m256i a0 )
{
  __m256i _a1l_a0h = _mm256_permute2x128_si256( a0, a0 , 0x81 );
  return _mm256_alignr_epi8(_a1l_a0h,a0,1);
}

static inline
__m256i right_shift_2_low( __m256i a1 , __m256i a0 )
{
  __m256i _a1l_a0h = _mm256_permute2x128_si256( a0, a1 , 0x21 );
  return _mm256_alignr_epi8(_a1l_a0h,a0,2);
}

static inline
__m256i right_shift_2( __m256i a0 )
{
  __m256i _a1l_a0h = _mm256_permute2x128_si256( a0, a0 , 0x81 );
  return _mm256_alignr_epi8(_a1l_a0h,a0,2);
}

static inline
__m256i left_shift_1_high( __m256i a1 , __m256i a0 )
{
  __m256i _a1l_a0h = _mm256_permute2x128_si256( a0, a1 , 0x21 );
  return _mm256_alignr_epi8(a1,_a1l_a0h,15);
}

static inline
__m256i left_shift_1( __m256i a1 )
{
  __m256i _a1l_a0h = _mm256_permute2x128_si256( a1, a1 , 0x28 );
  return _mm256_alignr_epi8(a1,_a1l_a0h,15);
}

static inline
__m256i left_shift_2_high( __m256i a1 , __m256i a0 )
{
  __m256i _a1l_a0h = _mm256_permute2x128_si256( a0, a1 , 0x21 );
  return _mm256_alignr_epi8(a1,_a1l_a0h,14);
}

static inline
__m256i left_shift_2( __m256i a1 )
{
  __m256i _a1l_a0h = _mm256_permute2x128_si256( a1, a1 , 0x28 );
  return _mm256_alignr_epi8(a1,_a1l_a0h,14);
}

///////////////////////////////////////////////////////////////////



#include "string.h"


// assume: a[512+254]=a[512+255]=0  b[512+254]=b[512+255]=0
void mult3_768_refined_karatsuba3( unsigned char * c , const unsigned char * a , const unsigned char * b )
{
  const unsigned char * a0 = a;
  const unsigned char * a1 = a + 256;
  const unsigned char * a2 = a + 512;
  const unsigned char * b0 = b;
  const unsigned char * b1 = b + 256;
  const unsigned char * b2 = b + 512;

  ALIGNED unsigned char h0[512];
  ALIGNED unsigned char h1[512];
  ALIGNED unsigned char h2[512];
  ALIGNED unsigned char hx[512];
  ALIGNED unsigned char hinf[512];

  //h0 = a0*b0
  //h1 = (a0+a1+a2)*(b0+b1+b2)
  //h2 = (a0-a1+a2)*(b0-b1+b2)
  //hx = (a0+a1*x+a2*x^2)*(b0+b1*x+b2*x^2)
  //hinf = a2*b2

  memcpy( h0 , a0 , 256 ); memcpy( h0+256 , b0 , 256 );
  memcpy( h1 , a1 , 256 ); memcpy( h1+256 , b1 , 256 );
  memcpy( hinf , a2 , 256 ); memcpy( hinf+256 , b2 , 256 );

  for(unsigned i=0;i<512;i+=32) {
    __m256i m0 = _mm256_load_si256((__m256i*)(h0+i));
    __m256i m1 = _mm256_load_si256((__m256i*)(h1+i));
    __m256i m2 = _mm256_load_si256((__m256i*)(hinf+i));

    _mm256_store_si256( (__m256i*)(h2+i) ,  reduce_low( _mm256_add_epi8( reduce_neg(m1) , _mm256_add_epi8(m0,m2) ) ) );
  }

  // h(x) =  hx * x^2 + V[255:0] * x + a0[0]*b0[0]
  {
    unsigned i=0;
    __m256i m0 = _mm256_loadu_si256((__m256i*)(a0+1+i));
    __m256i m1 = _mm256_load_si256((__m256i*)(h1+i));
    __m256i m2 = _mm256_loadu_si256((__m256i*)(a2-1+i));
    m2 &= _mm256_set_epi32(0xffffffff,0xffffffff,0xffffffff,0xffffffff, 0xffffffff,0xffffffff,0xffffffff,0xffffff00 );
    _mm256_store_si256( (__m256i*)(hx+i) ,  reduce_low( _mm256_add_epi8( m1 , _mm256_add_epi8(m0,m2) ) ) );
  }
  for(unsigned i=32;i<256-32;i+=32) {
    __m256i m0 = _mm256_loadu_si256((__m256i*)(a0+1+i));
    __m256i m1 = _mm256_load_si256((__m256i*)(h1+i));
    __m256i m2 = _mm256_loadu_si256((__m256i*)(a2-1+i));
    _mm256_store_si256( (__m256i*)(hx+i) ,  reduce_low( _mm256_add_epi8( m1 , _mm256_add_epi8(m0,m2) ) ) );
  }
  {
    unsigned i=256-32;
    __m256i m0 = _mm256_loadu_si256((__m256i*)(a0+1+i));
    m0 &= _mm256_set_epi32(0x00ffffff,0xffffffff,0xffffffff,0xffffffff, 0xffffffff,0xffffffff,0xffffffff,0xffffffff );
    __m256i m1 = _mm256_load_si256((__m256i*)(h1+i));
    __m256i m2 = _mm256_loadu_si256((__m256i*)(a2-1+i));
    _mm256_store_si256( (__m256i*)(hx+i) ,  reduce_low( _mm256_add_epi8( m1 , _mm256_add_epi8(m0,m2) ) ) );
  }

  {
    unsigned i=0;
    __m256i m0 = _mm256_loadu_si256((__m256i*)(b0+1+i));
    __m256i m1 = _mm256_load_si256((__m256i*)(h1+256+i));
    __m256i m2 = _mm256_loadu_si256((__m256i*)(b2-1+i));
    m2 &= _mm256_set_epi32(0xffffffff,0xffffffff,0xffffffff,0xffffffff, 0xffffffff,0xffffffff,0xffffffff,0xffffff00 );
    _mm256_store_si256( (__m256i*)(hx+256+i) ,  reduce_low( _mm256_add_epi8( m1 , _mm256_add_epi8(m0,m2) ) ) );
  }
  for(unsigned i=32;i<256-32;i+=32) {
    __m256i m0 = _mm256_loadu_si256((__m256i*)(b0+1+i));
    __m256i m1 = _mm256_load_si256((__m256i*)(h1+256+i));
    __m256i m2 = _mm256_loadu_si256((__m256i*)(b2-1+i));
    _mm256_store_si256( (__m256i*)(hx+256+i) ,  reduce_low( _mm256_add_epi8( m1 , _mm256_add_epi8(m0,m2) ) ) );
  }
  {
    unsigned i=256-32;
    __m256i m0 = _mm256_loadu_si256((__m256i*)(b0+1+i));
    m0 &= _mm256_set_epi32(0x00ffffff,0xffffffff,0xffffffff,0xffffffff, 0xffffffff,0xffffffff,0xffffffff,0xffffffff );
    __m256i m1 = _mm256_load_si256((__m256i*)(h1+256+i));
    __m256i m2 = _mm256_loadu_si256((__m256i*)(b2-1+i));
    _mm256_store_si256( (__m256i*)(hx+256+i) ,  reduce_low( _mm256_add_epi8( m1 , _mm256_add_epi8(m0,m2) ) ) );
  }

  for(unsigned i=0;i<512;i+=32) {
    __m256i m0 = _mm256_load_si256((__m256i*)(h0+i));
    __m256i m1 = _mm256_load_si256((__m256i*)(h1+i));
    __m256i m2 = _mm256_load_si256((__m256i*)(hinf+i));

    _mm256_store_si256( (__m256i*)(h1+i) ,  reduce_low( _mm256_add_epi8( m1, _mm256_add_epi8(m0,m2) ) ) );
  }

  // h(x) =  hx * x^2 + V[255:0] * x + a0[0]*b0[0]
  ALIGNED unsigned char V[512];
  __m256i lm1, lm2;
  lm1 = _mm256_set1_epi16( a0[0] );
  lm2 = _mm256_set1_epi16( b0[0] );
  for(int i=0;i<256;i+=32) {
    __m256i m1 = _mm256_load_si256((__m256i*)(hx+i));
    __m256i m2 = _mm256_load_si256((__m256i*)(hx+256+i));
    __m256i m3 = _mm256_add_epi8( _mm256_mullo_epi16(lm2,m1),_mm256_mullo_epi16(lm1,m2) ); // <= 8
    _mm256_store_si256((__m256i*)(V+i),m3);
  }

  mult3_256x256_refined_karatsuba_x2( (__m256i * )h0 , (__m256i * )h1 );
  mult3_256x256_refined_karatsuba_x2( (__m256i * )h2 , (__m256i * )hx );
  mult3_256x256_refined_karatsuba( (__m256i * )hinf );

  // h1,h2 = (h1-h2,h1+h2)
  for(unsigned i=0;i<512;i+=32) {
    __m256i h1i = _mm256_load_si256( (__m256i*)(h1+i) );
    __m256i h2i = _mm256_load_si256( (__m256i*)(h2+i) );

    _mm256_store_si256( (__m256i*)(h1+i) , _mm256_add_epi8(h1i,reduce_neg(h2i)) ); // <= 4
    _mm256_store_si256( (__m256i*)(h2+i) , _mm256_add_epi8(h1i,h2i) );  // <= 4
  }


  //
  // V = ( (h1+h2)*x + (h1-h2) +(hx//x) ) // (x^2-1)\n",
  // U = V + ( h0//x) - hinf*x\n",
  // h = U*(y^3) - (U + h1-h2)*y   +   hinf*y^4 - (h0+(h1+h2+hinf)*y^2 + h0\n",
  //


  //V = ( (h1+h2)*x + (h1-h2) +(hx//x) ) // (x^2-1)
  // h(x) =  hx * x^2 + V[255:0] * x + a0[0]*b0[0]
  lm1 = _mm256_setzero_si256();
  lm2 = _mm256_setzero_si256();
  for(int i=512-32;i>=256;i-=32) {
    __m256i m1 = _mm256_load_si256((__m256i*)(h2+i)); // <= 4
    __m256i m2 = _mm256_load_si256((__m256i*)(h1+i)); // <= 4
    __m256i m3 = _mm256_load_si256((__m256i*)(hx+i)); // <= 2

    __m256i m10 = _mm256_add_epi8(m1,m3);
    __m256i m1_ = right_shift_1_low( lm1, m10 );
    __m256i m2_ = right_shift_2_low( lm2 , m2 );

    lm1 = m10;
    lm2 = m2;
    _mm256_store_si256( (__m256i*)(V+i) , reduce_low(_mm256_add_epi8(m1_,m2_)) );
  }
  for(int i=256-32;i>=0;i-=32) {
    __m256i m0 = _mm256_load_si256((__m256i*)(V+i));  // <= 8
    __m256i m1 = _mm256_load_si256((__m256i*)(h2+i)); // <= 4
    __m256i m2 = _mm256_load_si256((__m256i*)(h1+i)); // <= 4
    __m256i m3 = _mm256_load_si256((__m256i*)(hx+i)); // <= 2

    __m256i m10 = _mm256_add_epi8(m1,m3);
    __m256i m1_ = right_shift_1_low( lm1, m10 );
    __m256i m20 = reduce_low(_mm256_add_epi8(m2,m0));
    __m256i m2_ = right_shift_2_low( lm2 , m20 );

    lm1 = m10;
    lm2 = m20;
    _mm256_store_si256( (__m256i*)(V+i) , reduce_low(_mm256_add_epi8(m1_,m2_)) );
  }
  poly3_512_div_xsq_1_mult_xsq( (__m256i*)V );

  //U = V + ( h0//x) - hinf*x
  lm1 = _mm256_setzero_si256();
  for(int i=512-32;i>=0;i-=32) {
    __m256i m0 = _mm256_load_si256((__m256i*)(V+i)); // <= 2
    __m256i m1 = _mm256_load_si256((__m256i*)(h0+i)); // <= 2
    __m256i m1_ = right_shift_1_low( lm1, m1 );
    lm1 = m1;
    _mm256_store_si256( (__m256i*)(V+i) , _mm256_add_epi8(m0,m1_) ); // <= 4
  }
  lm1 = _mm256_setzero_si256();
  for(int i=0;i<512;i+=32) {
    __m256i m0 = _mm256_load_si256((__m256i*)(V+i)); // <= 4
    __m256i m1 = _mm256_load_si256((__m256i*)(hinf+i)); // <= 2
    __m256i m1_ = left_shift_1_high( m1 , lm1 );
    lm1 = m1;
    _mm256_store_si256( (__m256i*)(V+i) , _mm256_add_epi8(m0,reduce_neg(m1_)) ); // <= 6
  }

  //h = U*(y^3) - (U + (h1-h2))*y   +   hinf*y^4 - (h0+(h1+h2)+hinf)*y^2 + h0
  memcpy( c , h0 , 512 );
  for(int i=0;i<512;i+=32) {
    __m256i m0 = _mm256_load_si256((__m256i*)(h0+i)); // <= 2
    __m256i m1 = _mm256_load_si256((__m256i*)(hinf+i)); // <= 2
    __m256i m2 = _mm256_load_si256((__m256i*)(h2+i)); // <= 4

    _mm256_storeu_si256( (__m256i*)(c+512+i) , reduce_neg(_mm256_add_epi8(m0,_mm256_add_epi8(m1,m2))) ); // <= 2
  }
  memcpy( c+1024, hinf , 512 );

  // - (U+(h1-h2))*y
  for(int i=0;i<512;i+=32) {
    __m256i ci = _mm256_loadu_si256((__m256i*)(c+256+i)); // <= 2

    __m256i m1 = _mm256_load_si256((__m256i*)(V+i));  // <= 6
    __m256i m2 = _mm256_load_si256((__m256i*)(h1+i)); // <= 4

    __m256i m3 = _mm256_add_epi8( reduce_neg(ci), _mm256_add_epi8(m1,m2) ); // <= 12

    _mm256_storeu_si256( (__m256i*)(c+256+i) , reduce_neg(m3) ); // <= 2
  }
  // U*(y^3)
  for(int i=0;i<512;i+=32) {
    __m256i ci = _mm256_loadu_si256((__m256i*)(c+768+i)); // <= 2
    __m256i ui = _mm256_load_si256((__m256i*)(V+i));  // <= 7
    _mm256_storeu_si256( (__m256i*)(c+768+i) , add_r3(ci,ui) ); // <= 2
  }

}






void mult3_768_karatsuba3( unsigned char * c , const unsigned char * a , const unsigned char * b )
{
  const unsigned char * a0 = a;
  const unsigned char * a1 = a + 256;
  const unsigned char * a2 = a + 512;
  const unsigned char * b0 = b;
  const unsigned char * b1 = b + 256;
  const unsigned char * b2 = b + 512;

  ALIGNED unsigned char a0b0[512];
  ALIGNED unsigned char a1b1[512];
  ALIGNED unsigned char a2b2[512];
  ALIGNED unsigned char a01b01[512];
  ALIGNED unsigned char a02b02[512];
  ALIGNED unsigned char a012b012[512];

  memcpy( a0b0 , a0 , 256 ); memcpy( a0b0+256 , b0 , 256 );
  memcpy( a1b1 , a1 , 256 ); memcpy( a1b1+256 , b1 , 256 );
  memcpy( a2b2 , a2 , 256 ); memcpy( a2b2+256 , b2 , 256 );

  for(unsigned i=0;i<512;i+=32) {
    _mm256_store_si256( (__m256i*)(a01b01+i) , add_r3( _mm256_load_si256( (__m256i*)(a0b0+i) ),_mm256_load_si256( (__m256i*)(a1b1+i) ) ) );
  }
  for(unsigned i=0;i<512;i+=32) {
    _mm256_store_si256( (__m256i*)(a02b02+i) , add_r3( _mm256_load_si256( (__m256i*)(a0b0+i) ),_mm256_load_si256( (__m256i*)(a2b2+i) ) ) );
  }
  for(unsigned i=0;i<512;i+=32) {
    _mm256_store_si256( (__m256i*)(a012b012+i) , add_r3( _mm256_load_si256( (__m256i*)(a02b02+i) ),_mm256_load_si256( (__m256i*)(a1b1+i) ) ) );
  }

  mult3_256x256_refined_karatsuba_x2( (__m256i * )a0b0 , (__m256i * )a1b1 );
  mult3_256x256_refined_karatsuba_x2( (__m256i * )a2b2 , (__m256i * )a012b012 );
  mult3_256x256_refined_karatsuba_x2( (__m256i * )a01b01 , (__m256i * )a02b02 );
  //mult3_256x256_inplace_karatsuba( (__m256i * )a0b0 );
  //mult3_256x256_inplace_karatsuba( (__m256i * )a1b1 );
  //mult3_256x256_inplace_karatsuba( (__m256i * )a2b2 );
  //mult3_256x256_inplace_karatsuba( (__m256i * )a01b01 );
  //mult3_256x256_inplace_karatsuba( (__m256i * )a02b02 );
  //mult3_256x256_inplace_karatsuba( (__m256i * )a012b012 );

//   (a0 + a1 X + a2 X2 )( b0 + b1 X + b2 X2 )
// = a0b0 ( 1 - X - X2 + X3 )
// + a1b1 ( -X + X2 )
// + a2b2 ( -X2 + X4 )
// + a01b01 ( X - X3 )
// + a02b02 ( X2 - X3 )
// + a012b012 ( X3 )

  memcpy( c , a0b0 , 512 );          // 1
  memcpy( c + 512 , a1b1 , 512 );  // X2
  memcpy( c + 1024, a2b2 , 512 );    // X4
  // X
  for(unsigned i=0;i<512;i+=32) {
    __m256i ci = _mm256_loadu_si256( (__m256i*)(c+256+i) );
    __m256i a0b0i = _mm256_load_si256( (__m256i*)(a0b0+i) );
    __m256i a1b1i = _mm256_load_si256( (__m256i*)(a1b1+i) );
    __m256i a01b01i = _mm256_load_si256( (__m256i*)(a01b01+i) );
    ci = add_r3( _mm256_add_epi8(ci,a01b01i) , reduce_neg(_mm256_add_epi8(a0b0i,a1b1i)) );
    _mm256_storeu_si256( (__m256i*)(c+256+i), ci );
  }
  // X2
  for(unsigned i=0;i<512;i+=32) {
    __m256i ci = _mm256_loadu_si256( (__m256i*)(c+512+i) );
    __m256i a0b0i = _mm256_load_si256( (__m256i*)(a0b0+i) );
    __m256i a2b2i = _mm256_load_si256( (__m256i*)(a2b2+i) );
    __m256i a02b02i = _mm256_load_si256( (__m256i*)(a02b02+i) );
    ci = add_r3( _mm256_add_epi8(ci,a02b02i) , reduce_neg(_mm256_add_epi8(a0b0i,a2b2i)) );
    _mm256_storeu_si256( (__m256i*)(c+512+i), ci );
  }
  // X3
  for(unsigned i=0;i<512;i+=32) {
    __m256i ci = _mm256_loadu_si256( (__m256i*)(c+768+i) );
    __m256i a0b0i = _mm256_load_si256( (__m256i*)(a0b0+i) );
    __m256i a01b01i = _mm256_load_si256( (__m256i*)(a01b01+i) );
    __m256i a02b02i = _mm256_load_si256( (__m256i*)(a02b02+i) );
    __m256i a012b012i = _mm256_load_si256( (__m256i*)(a012b012+i) );
    ci = add_r3( _mm256_add_epi8(ci,_mm256_add_epi8(a0b0i,a012b012i)) , reduce_neg(_mm256_add_epi8(a01b01i,a02b02i)) );
    _mm256_storeu_si256( (__m256i*)(c+768+i), ci );
  }
}




