#ifndef _CONST_INLINE_H_
#define _CONST_INLINE_H_


#include "immintrin.h"


#define v_0xf  _mm256_set1_epi8( 0xf )
#define v_0x1  _mm256_set1_epi8( 0x1 )
#define v_0x3  _mm256_set1_epi8( 0x3 )
#define v_0x6  _mm256_set1_epi8( 0x6 )
#define v_0x9  _mm256_set1_epi8( 0x9 )
#define v_0XX0 _mm256_set_epi32(0,-1,-1,0,0,-1,-1,0)

#define _reduce_low _mm256_set_epi8( 0,2,1,0,2,1,0,2,1,0,2,1,0,2,1,0, 0,2,1,0,2,1,0,2,1,0,2,1,0,2,1,0 )
static inline __m256i reduce_low( __m256i a ) { return _mm256_shuffle_epi8(_reduce_low,a); }

#define _reduce_neg _mm256_set_epi8( 0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0, 0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0 )
static inline __m256i reduce_neg( __m256i a ) { return _mm256_shuffle_epi8(_reduce_neg,a); }

static inline __m256i add_r3( __m256i a , __m256i b ) { return reduce_low(_mm256_add_epi8(a,b)); }

static inline __m256i sub_r3( __m256i a , __m256i b ) { return add_r3(a, reduce_neg(b) ); }


//
// unsigned input < 128
// unsigned output <= 9
//
#define reduce_3_7 _mm256_set_epi8( 120,111,102,96,87,78,72,63,54,48,39,30,24,15,6,0,  120,111,102,96,87,78,72,63,54,48,39,30,24,15,6,0 )
static inline __m256i reduce_high( __m256i a ) { return _mm256_sub_epi8(a, _mm256_shuffle_epi8(reduce_3_7, _mm256_srli_epi16(a,3)&v_0xf  ) ); }


//
// 00->0, 01->1, 10->0, 11->2, others->0
//
#define _cvt_to_unsigned _mm256_set_epi8( 0,0,0,0, 0,0,0,0, 0,0,0,0, 2,0,1,0,  0,0,0,0, 0,0,0,0, 0,0,0,0, 2,0,1,0 )
static inline __m256i cvt_to_unsigned( __m256i a ) { return _mm256_shuffle_epi8(_cvt_to_unsigned,a&v_0x3); }


#define _cvt_to_int _mm256_set_epi8( 0,-1,1,0, -1,1,0,-1, 1,0,-1,1, 0,-1,1,0,  0,-1,1,0, -1,1,0,-1, 1,0,-1,1, 0,-1,1,0 )
static inline __m256i cvt_to_int( __m256i a ) { return _mm256_shuffle_epi8(_cvt_to_int,a); }


#endif
