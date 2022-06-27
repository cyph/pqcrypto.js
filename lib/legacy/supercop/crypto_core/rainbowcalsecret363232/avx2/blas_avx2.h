/// @file blas_avx2.h
/// @brief Inlined functions for implementing basic linear algebra functions for AVX2 arch.
///

#ifndef _BLAS_AVX2_H_
#define _BLAS_AVX2_H_

#include "gf16.h"

#include <immintrin.h>

#include "blas_config.h"
#include "assert.h"

#include "gf16_avx2.h"

#include "blas_sse.h"


#ifdef  __cplusplus
extern  "C" {
#endif






static inline
__m256i _load_ymm( const uint8_t *a , unsigned _num_byte ) {
	uint8_t temp[32] __attribute__((aligned(32)));
	assert( 32 >= _num_byte );
	assert( 0 < _num_byte );
	for(unsigned i=0;i<_num_byte;i++) temp[i] = a[i];
	return _mm256_load_si256((__m256i*)temp);
}

static inline
void _store_ymm( uint8_t *a , unsigned _num_byte , __m256i data ) {
	uint8_t temp[32] __attribute__((aligned(32)));
	assert( 32 >= _num_byte );
	assert( 0 < _num_byte );
	_mm256_store_si256((__m256i*)temp,data);
	for(unsigned i=0;i<_num_byte;i++) a[i] = temp[i];
}



static inline
void linearmap_8x8_ymm( uint8_t * a , __m256i ml , __m256i mh , __m256i mask , unsigned _num_byte ) {
	unsigned n_32 = _num_byte>>5;
	for(unsigned i=0;i<n_32;i++) {
		__m256i inp = _mm256_loadu_si256( (__m256i*)(a+i*32) );
		__m256i r0 = linear_transform_8x8_256b( ml , mh , inp , mask );
		_mm256_storeu_si256( (__m256i*)(a+i*32) , r0 );
	}
	unsigned rem = _num_byte&31;
	if( rem ) linearmap_8x8_sse( a+n_32*32 , _mm256_castsi256_si128(ml) , _mm256_castsi256_si128(mh) , _mm256_castsi256_si128(mask) , rem );
}


static inline
void linearmap_8x8_accu_ymm( uint8_t * accu_c , const uint8_t * a ,  __m256i ml , __m256i mh , __m256i mask , unsigned _num_byte ) {
	unsigned n_32 = _num_byte>>5;
	for(unsigned i=0;i<n_32;i++) {
		__m256i inp = _mm256_loadu_si256( (__m256i*)(a+i*32) );
		__m256i out = _mm256_loadu_si256( (__m256i*)(accu_c+i*32) );
		__m256i r0 = linear_transform_8x8_256b( ml , mh , inp , mask );
		r0 ^= out;
		_mm256_storeu_si256( (__m256i*)(accu_c+i*32) , r0 );
	}
	unsigned rem = _num_byte&31;
	if( rem ) linearmap_8x8_accu_sse( accu_c+n_32*32 , a+n_32*32 , _mm256_castsi256_si128(ml) , _mm256_castsi256_si128(mh) , _mm256_castsi256_si128(mask) , rem );
}




///////////////////////   basic functions   /////////////////////////////////////


static inline
void gf256v_add_avx2( uint8_t * accu_b, const uint8_t * a , unsigned _num_byte ) {
	//uint8_t temp[32] __attribute__((aligned(32)));
	unsigned n_ymm = (_num_byte)>>5;
	unsigned i=0;
	for(;i<n_ymm;i++) {
		__m256i inp = _mm256_loadu_si256( (__m256i*) (a+i*32) );
		__m256i out = _mm256_loadu_si256( (__m256i*) (accu_b+i*32) );
		out ^= inp;
		_mm256_storeu_si256( (__m256i*) (accu_b+i*32) , out );
	}
	if( 0 != (_num_byte&0x1f) ) {
		n_ymm <<= 5;
		gf256v_add_sse( accu_b + n_ymm , a + n_ymm , _num_byte&0x1f );
	}
}






static inline
void gf16v_mul_scalar_avx2( uint8_t * a, uint8_t gf16_b , unsigned _num_byte ) {
	unsigned b = gf16_b&0xf;
	__m256i m_tab = _mm256_load_si256( (__m256i*) (__gf16_mul + 32*b) );
	__m256i ml = _mm256_permute2x128_si256( m_tab , m_tab , 0 );
	__m256i mh = _mm256_permute2x128_si256( m_tab , m_tab , 0x11 );
	__m256i mask = _mm256_load_si256( (__m256i*) __mask_low );

	linearmap_8x8_ymm( a , ml , mh , mask , _num_byte );
}



static inline
void gf16v_madd_avx2( uint8_t * accu_c, const uint8_t * a , uint8_t gf16_b, unsigned _num_byte ) {
	unsigned b = gf16_b&0xf;
	__m256i m_tab = _mm256_load_si256( (__m256i*) (__gf16_mul + 32*b) );
	__m256i ml = _mm256_permute2x128_si256( m_tab , m_tab , 0 );
	__m256i mh = _mm256_permute2x128_si256( m_tab , m_tab , 0x11 );
	__m256i mask = _mm256_load_si256( (__m256i*) __mask_low );

	linearmap_8x8_accu_ymm( accu_c , a , ml , mh , mask , _num_byte );
}

static inline
void gf16v_madd_multab_avx2( uint8_t * accu_c, const uint8_t * a , const uint8_t * multab , unsigned _num_byte ) {
	__m128i ml_16 = _mm_load_si128( (__m128i*) multab );
	__m128i mh_16 = _mm_slli_epi16( ml_16 , 4 );
	__m256i ml = _mm256_inserti128_si256( _mm256_castsi128_si256(ml_16), ml_16, 1);
	__m256i mh = _mm256_inserti128_si256( _mm256_castsi128_si256(mh_16), mh_16, 1);
	__m256i mask = _mm256_load_si256( (__m256i*) __mask_low );

	linearmap_8x8_accu_ymm( accu_c , a , ml , mh , mask , _num_byte );
}





static inline
void gf256v_mul_scalar_avx2( uint8_t * a, uint8_t _b , unsigned _num_byte ) {
	unsigned b = _b;
	__m256i m_tab = _mm256_load_si256( (__m256i*) (__gf256_mul + 32*b) );
	__m256i ml = _mm256_permute2x128_si256( m_tab , m_tab , 0 );
	__m256i mh = _mm256_permute2x128_si256( m_tab , m_tab , 0x11 );
	__m256i mask = _mm256_load_si256( (__m256i*) __mask_low );

	linearmap_8x8_ymm( a , ml , mh , mask , _num_byte );
}


static inline
void gf256v_madd_multab_avx2( uint8_t * accu_c, const uint8_t * a , const uint8_t * multab, unsigned _num_byte ) {
	__m256i m_tab = _mm256_load_si256( (__m256i*) (multab) );
	__m256i ml = _mm256_permute2x128_si256( m_tab , m_tab , 0 );
	__m256i mh = _mm256_permute2x128_si256( m_tab , m_tab , 0x11 );
	__m256i mask = _mm256_load_si256( (__m256i*) __mask_low );

	linearmap_8x8_accu_ymm( accu_c , a , ml , mh , mask , _num_byte );
}



static inline
void gf256v_madd_avx2( uint8_t * accu_c, const uint8_t * a , uint8_t _b, unsigned _num_byte ) {
	unsigned b = _b;
	__m256i m_tab = _mm256_load_si256( (__m256i*) (__gf256_mul + 32*b) );
	__m256i ml = _mm256_permute2x128_si256( m_tab , m_tab , 0 );
	__m256i mh = _mm256_permute2x128_si256( m_tab , m_tab , 0x11 );
	__m256i mask = _mm256_load_si256( (__m256i*) __mask_low );

	linearmap_8x8_accu_ymm( accu_c , a , ml , mh , mask , _num_byte );
}






#ifdef  __cplusplus
}
#endif



#endif // _BLAS_AVX2_H_

