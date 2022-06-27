#ifndef _POLYMUL_AVX2_H_
#define _POLYMUL_AVX2_H_

#include <stdint.h>
#include <stdio.h>
#include <emmintrin.h>
#include <tmmintrin.h>
#include <immintrin.h>




///////////////

// 8x8 polynomial multiplication without reduction for parallel 128-bit lanes.
// results stored in 32-bits form.
// input:
// a * b : (a0 + a1 x + ... + a7 x^7 ) * (b0 + b1 x + ... + b7 x^7 )
// output:
// -->  rc0:  ( c0 + c1 x + c2 x^2 + c3 x^3 )  +
//      rc1:  ( c4 + c5 x + c6 x^2 + c7 x^3 ) x^4 +
//      rc2:  ( c8 + c9 x + c10 x^2 + c11 x^3 ) x^8 +
//      rc3:  ( c12 + c13 x + c14 x^2         ) x^12
//
// note: school book multiplication
//
static inline void polymul_8x8_32( __m256i *rc0, __m256i *rc1, __m256i *rc2, __m256i *rc3, __m256i a , __m256i b )
{
	__m256i a_sl_2 = _mm256_slli_si256( a , 2 );
	__m256i al = _mm256_unpacklo_epi16( a , a_sl_2 );
	__m256i ah = _mm256_unpackhi_epi16( a , a_sl_2 );

	__m256i a7xbodd = _mm256_madd_epi16( _mm256_shuffle_epi32(a,0xff) , b&_mm256_set1_epi32(0xffff0000) );

	__m256i b_br = _mm256_shuffle_epi32( b , 0 );  /// b0,b1
	__m256i c0 = _mm256_madd_epi16( al , b_br );
	__m256i c1 = _mm256_madd_epi16( ah , b_br );

	b_br = _mm256_shuffle_epi32( b , 0x55 );  /// b2,b3
	__m256i d0 = _mm256_madd_epi16( al , b_br );
	__m256i d1 = _mm256_madd_epi16( ah , b_br );

	b_br = _mm256_shuffle_epi32( b , 0xaa );  /// b4,b5
	c1 = _mm256_add_epi32( c1 , _mm256_madd_epi16( al , b_br ) );
	__m256i e1 = _mm256_madd_epi16( ah , b_br );

	b_br = _mm256_shuffle_epi32( b , 0xff );  /// b7,b8
	d1 = _mm256_add_epi32( d1 , _mm256_madd_epi16( al , b_br ) );
	__m256i f1 = _mm256_madd_epi16( ah , b_br );

	c0 = _mm256_add_epi32( c0 , _mm256_alignr_epi8(d0,_mm256_setzero_si256(),8) );  /// rc0
	c1 = _mm256_add_epi32( c1 , _mm256_alignr_epi8(d1,d0,8) );  /// rc1

	__m256i d1h = _mm256_add_epi32( _mm256_srli_si256(d1,8) , _mm256_unpacklo_epi32(a7xbodd,_mm256_setzero_si256()) ); /// rc2
	e1 = _mm256_add_epi32( e1 , _mm256_add_epi32( d1h , _mm256_slli_si256(f1,8) ) );

	__m256i f1h = _mm256_add_epi32( _mm256_srli_si256(f1,8) , _mm256_unpackhi_epi32(a7xbodd,_mm256_setzero_si256()) );  /// rc3

	_mm256_store_si256( rc0 , c0 );
	_mm256_store_si256( rc1 , c1 );
	_mm256_store_si256( rc2 , e1 );
	_mm256_store_si256( rc3 , f1h );
}





// 16x16 polynomial multiplication without reduction for parallel 128-bit lanes.
// results stored in 32-bits form.
// input:
// (a0 + a1*x^8) * (b0 + b1*x^8) : (a0 + a1 x + ... + a15 x^15 ) * (b0 + b1 x + ... + b15 x^15 )
// output:
// -->  rc[0]:  ( c0 + c1 x + c2 x^2 + c3 x^3 )  +
//      rc[1]:  ( c4 + c5 x + c6 x^2 + c7 x^3 ) x^4 +
//                           .....
//      rc[7]:  ( c28 + c29 x + c30 x^2         ) x^28
static inline void polymul_16x16_x2_32( __m256i *rc, __m256i a0 , __m256i a1 , __m256i b0 , __m256i b1 )
{
	__m256i a0_p_a1 = _mm256_add_epi16( a0 , a1 );
	__m256i b0_p_b1 = _mm256_add_epi16( b0 , b1 );
        polymul_8x8_32( rc   , rc+1 , rc+2 , rc+3 , a0 , b0 );
        polymul_8x8_32( rc+4 , rc+5 , rc+6 , rc+7 , a1 , b1 );
        __m256i t0,t1,t2,t3;
        polymul_8x8_32( &t0 , &t1, &t2, &t3 , a0_p_a1 , b0_p_b1 );

        t0 = _mm256_sub_epi32( t0 , rc[0] );
        t0 = _mm256_sub_epi32( t0 , rc[4] );
        t1 = _mm256_sub_epi32( t1 , rc[1] );
        t1 = _mm256_sub_epi32( t1 , rc[5] );
        t2 = _mm256_sub_epi32( t2 , rc[2] );
        t2 = _mm256_sub_epi32( t2 , rc[6] );
        t3 = _mm256_sub_epi32( t3 , rc[3] );
        t3 = _mm256_sub_epi32( t3 , rc[7] );

        rc[2] = _mm256_add_epi32( rc[2] , t0);
        rc[3] = _mm256_add_epi32( rc[3] , t1);
        rc[4] = _mm256_add_epi32( rc[4] , t2);
        rc[5] = _mm256_add_epi32( rc[5] , t3);
}




////////////////


//
// 16x16 polynomial multiplication without reduction.
// input:   a: [0-7][8-15]    b: [0-7][8-15]
// Results stored in 32-bits form
//            and in the order c0: [0-3][8-11]  c1: [4-7][12-15] c2:[16-19][24-27] c3: [20-23][28-31]
//  ,which has to be adjusted.
// Presuming size of inputs < 4591*2
//
// note: school book multiplication
//
static inline void polymul_16x16_32(__m256i *c0, __m256i *c1, __m256i *c2, __m256i *c3, __m256i a, __m256i b){
  __m256i t0e,t0o,t1e,t1o;
  __m256i a_flip = _mm256_permute4x64_epi64(a, 0x4e);
  polymul_8x8_32(&t0e, &t0o, &t1e, &t1o, a_flip, b);
  __m256i a0b1e = _mm256_permute2x128_si256(t0e,t1e,0x20);
  __m256i a1b0e = _mm256_permute2x128_si256(t0e,t1e,0x31);
  __m256i a0b1o = _mm256_permute2x128_si256(t0o,t1o,0x20);
  __m256i a1b0o = _mm256_permute2x128_si256(t0o,t1o,0x31);
  a0b1e = _mm256_add_epi32(a0b1e,a1b0e);
  a0b1o = _mm256_add_epi32(a0b1o,a1b0o);
  polymul_8x8_32(&t0e,&t0o,&t1e,&t1o,a,b);
  a0b1e = _mm256_add_epi32( a0b1e , _mm256_permute2x128_si256(t1e,t0e,0x30) );
  a0b1o = _mm256_add_epi32( a0b1o , _mm256_permute2x128_si256(t1o,t0o,0x30) );
  __m256i a0b0e = _mm256_permute2x128_si256(t0e,a0b1e,0x20);
  __m256i a0b0o = _mm256_permute2x128_si256(t0o,a0b1o,0x20);
  __m256i a1b1e = _mm256_permute2x128_si256(a0b1e,t1e,0x31);
  __m256i a1b1o = _mm256_permute2x128_si256(a0b1o,t1o,0x31);

  *c0 = a0b0e; *c1 = a0b0o; *c2 = a1b1e; *c3 = a1b1o;
}


/////////////////



//
// 32x32 polynomial multiplication with recursive karatsuba.
// input:   a: [0-7][8-15][16-23][24-32]    b: [0-7][8-15][16-23][24-32]
// Results stored in 32-bits form
//            and in the order:
//   [0-3][8-11] [4-7][12-15] , [16-19][24-27] [20-23][28-31] , .....
//  ,which has to be adjusted.
//
static inline void polymul_32x32( __m256i *c, __m256i *a, __m256i *b ){

// 16x16 level   1. mid term
  __m256i a01 = _mm256_add_epi16(a[0],a[1]);
  __m256i b01 = _mm256_add_epi16(b[0],b[1]);
  __m256i tc_0,tc_1,tc_2,tc_3;
  polymul_16x16_32(&tc_0,&tc_1,&tc_2,&tc_3,a01,b01);

// 8x8 level start
// split the data into high-128bits and low-128bits lanes.
  __m256i a0l_a1l = _mm256_permute2x128_si256( a[0],a[1], 0x20 );
  __m256i a0h_a1h = _mm256_permute2x128_si256( a[0],a[1], 0x31 );
  __m256i b0l_b1l = _mm256_permute2x128_si256( b[0],b[1], 0x20 );
  __m256i b0h_b1h = _mm256_permute2x128_si256( b[0],b[1], 0x31 );

  __m256i a0m_a1m = _mm256_add_epi16( a0l_a1l , a0h_a1h );
  __m256i b0m_b1m = _mm256_add_epi16( b0l_b1l , b0h_b1h );

  __m256i m32_0,m32_1,m32_2,m32_3;
  polymul_8x8_32(&m32_0, &m32_1, &m32_2, &m32_3, a0m_a1m, b0m_b1m );

  __m256i l32_0,l32_1,l32_2,l32_3;
  polymul_8x8_32(&l32_0, &l32_1, &l32_2, &l32_3, a0l_a1l, b0l_b1l );

  __m256i h32_0,h32_1,h32_2,h32_3;
  polymul_8x8_32(&h32_0, &h32_1, &h32_2, &h32_3, a0h_a1h, b0h_b1h );

  m32_0 = _mm256_sub_epi32( m32_0 , l32_0 );
  m32_1 = _mm256_sub_epi32( m32_1 , l32_1 );
  m32_2 = _mm256_sub_epi32( m32_2 , h32_2 );
  m32_3 = _mm256_sub_epi32( m32_3 , h32_3 );
  l32_2 = _mm256_sub_epi32( l32_2 , h32_0 );
  l32_3 = _mm256_sub_epi32( l32_3 , h32_1 );
  h32_0 = _mm256_sub_epi32( m32_2 , l32_2 );
  h32_1 = _mm256_sub_epi32( m32_3 , l32_3 );
  l32_2 = _mm256_add_epi32( l32_2 , m32_0 );
  l32_3 = _mm256_add_epi32( l32_3 , m32_1 );

// 8x8 level end
  __m256i a0b0_0 = _mm256_permute2x128_si256( l32_0 , l32_2 , 0x20 );
  __m256i a0b0_1 = _mm256_permute2x128_si256( l32_1 , l32_3 , 0x20 );
  __m256i a0b0_2 = _mm256_permute2x128_si256( h32_0 , h32_2 , 0x20 );
  __m256i a0b0_3 = _mm256_permute2x128_si256( h32_1 , h32_3 , 0x20 );

  __m256i a1b1_0 = _mm256_permute2x128_si256( l32_0 , l32_2 , 0x31 );
  __m256i a1b1_1 = _mm256_permute2x128_si256( l32_1 , l32_3 , 0x31 );
  __m256i a1b1_2 = _mm256_permute2x128_si256( h32_0 , h32_2 , 0x31 );
  __m256i a1b1_3 = _mm256_permute2x128_si256( h32_1 , h32_3 , 0x31 );

// 16x16 level, the add/sub computations

  tc_0 = _mm256_sub_epi32( tc_0 , a0b0_0 );
  tc_1 = _mm256_sub_epi32( tc_1 , a0b0_1 );
  tc_2 = _mm256_sub_epi32( tc_2 , a1b1_2 );
  tc_3 = _mm256_sub_epi32( tc_3 , a1b1_3 );
  a0b0_2 = _mm256_sub_epi32(a0b0_2,a1b1_0);
  a0b0_3 = _mm256_sub_epi32(a0b0_3,a1b1_1);
  a1b1_0 = _mm256_sub_epi32( tc_2, a0b0_2 );
  a1b1_1 = _mm256_sub_epi32( tc_3, a0b0_3 );
  a0b0_2 = _mm256_add_epi32( a0b0_2 , tc_0 );
  a0b0_3 = _mm256_add_epi32( a0b0_3 , tc_1 );

  c[0] = a0b0_0;
  c[1] = a0b0_1;
  c[2] = a0b0_2;
  c[3] = a0b0_3;
  c[4] = a1b1_0;
  c[5] = a1b1_1;
  c[6] = a1b1_2;
  c[7] = a1b1_3;
}





#endif
