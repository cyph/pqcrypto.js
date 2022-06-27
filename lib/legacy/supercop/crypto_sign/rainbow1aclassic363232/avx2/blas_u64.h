/// @file blas_u64.h
/// @brief Inlined functions for implementing basic linear algebra functions for uint64 arch.
///

#ifndef _BLAS_U64_H_
#define _BLAS_U64_H_

#include <string.h>
#include <stdint.h>
#include <stdio.h>



#include <stdint.h>

#include "gf16.h"

#include "gf16_u64.h"

#include "blas_u32.h"


#ifdef  __cplusplus
extern  "C" {
#endif




static inline
void _gf256v_add_u64( uint8_t * accu_b, const uint8_t * a , unsigned _num_byte ) {
	unsigned n_u64 = _num_byte >> 3;
	for(unsigned i=0;i<n_u64;i++) {
	  uint64_t bx;
	  uint64_t ax;
	  memcpy(&bx,accu_b+8*i,8);
	  memcpy(&ax,a+8*i,8);
	  bx ^= ax;
	  memcpy(accu_b+8*i,&bx,8);
	}

	a += (n_u64<<3);
	accu_b += (n_u64<<3);
	unsigned rem = _num_byte & 7;
	if( rem )_gf256v_add_u32( accu_b , a , rem );
}


static inline
void _gf256v_conditional_add_u64( uint8_t * accu_b, uint8_t condition , const uint8_t * a , unsigned _num_byte ) {
	uint64_t pr_u64 = ((uint64_t)0)-((uint64_t)condition);

	unsigned n_u64 = _num_byte >> 3;
	for(unsigned i=0;i<n_u64;i++) {
	  uint64_t bx;
	  uint64_t ax;
	  memcpy(&bx,accu_b+8*i,8);
	  memcpy(&ax,a+8*i,8);
	  bx ^= ax&pr_u64;
	  memcpy(accu_b+8*i,&bx,8);
	}

	a += (n_u64<<3);
	accu_b += (n_u64<<3);
	unsigned rem = _num_byte & 7;
	if( rem ) _gf256v_conditional_add_u32( accu_b , condition , a , rem );
}



///////////////////////////////////////////////////



static inline
void _gf16v_mul_scalar_u64( uint8_t * a, uint8_t b , unsigned _num_byte ) {
	if( (size_t)a&7 ) { _gf16v_mul_scalar_u32(a,b,_num_byte); return; }
	unsigned _num = _num_byte>>3;
	for(unsigned i=0;i<_num;i++) {
	  uint64_t ax;
	  memcpy(&ax,a+8*i,8);
	  ax = gf16v_mul_u64(ax,b);
	  memcpy(a+8*i,&ax,8);
	}

	unsigned _num_b = _num_byte&0x7;
	unsigned st = _num<<3;
	a += st;
	if( _num_b ) _gf16v_mul_scalar_u32( a , b , _num_b );
}


static inline
void _gf256v_mul_scalar_u64( uint8_t *a, uint8_t b, unsigned _num_byte ) {
	if( (size_t)a&7 ) { _gf256v_mul_scalar_u32(a,b,_num_byte); return; }
	unsigned _num = _num_byte>>3;
	for(unsigned i=0;i<_num;i++) {
	  uint64_t ax;
	  memcpy(&ax,a+8*i,8);
	  ax = gf256v_mul_u64(ax,b);
	  memcpy(a+8*i,&ax,8);
	}
	unsigned _num_b = _num_byte&0x7;
	unsigned st = _num<<3;
	if( _num_b ) _gf256v_mul_scalar_u32( a+st , b , _num_b );
}


//////////////////////////////////////////////

static inline
void _gf16v_madd_u64( uint8_t * accu_c, const uint8_t * a , uint8_t b, unsigned _num_byte ) {
	if( ((size_t)a&7)||((size_t)accu_c&7) ) { _gf16v_madd_u32(accu_c,a,b,_num_byte); return; }
	unsigned _num = _num_byte>>3;
	for(unsigned i=0;i<_num;i++) {
	  uint64_t cx;
	  uint64_t ax;
	  memcpy(&cx,accu_c+8*i,8);
	  memcpy(&ax,a+8*i,8);
	  cx ^= gf16v_mul_u64(ax,b);
	  memcpy(accu_c+8*i,&cx,8);
	}

	unsigned _num_b = _num_byte&0x7;
	unsigned st = _num<<3;
	if( _num_b ) _gf16v_madd_u32( accu_c + st , a + st , b , _num_b );
}



static inline
void _gf256v_madd_u64( uint8_t * accu_c, const uint8_t * a , uint8_t b, unsigned _num_byte ) {
	if( ((size_t)a&7)||((size_t)accu_c&7) ) { _gf256v_madd_u32(accu_c,a,b,_num_byte); return; }
	unsigned _num = _num_byte>>3;
	for(unsigned i=0;i<_num;i++) {
	  uint64_t cx;
	  uint64_t ax;
	  memcpy(&cx,accu_c+8*i,8);
	  memcpy(&ax,a+8*i,8);
	  cx ^= gf256v_mul_u64(ax,b);
	  memcpy(accu_c+8*i,&cx,8);
	}
	unsigned _num_b = _num_byte&0x7;
	unsigned st = _num<<3;
	if( _num_b ) _gf256v_madd_u32( accu_c + st , a + st , b , _num_b );
}









#ifdef  __cplusplus
}
#endif



#endif // _BLAS_U64_H_


