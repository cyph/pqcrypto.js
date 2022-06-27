/// @file gf16_u64.h
/// @brief Inlined functions for implementing GF arithmetics for uint64 arch.
///

#ifndef _GF16_U64_H_
#define _GF16_U64_H_

#include <stdint.h>


#include "gf16.h"



#ifdef  __cplusplus
extern  "C" {
#endif




static inline uint64_t gf4v_mul_2_u64( uint64_t a )
{
	uint64_t bit0 = a&0x5555555555555555ull;
	uint64_t bit1 = a&0xaaaaaaaaaaaaaaaaull;
	return (bit0<<1)^bit1^(bit1>>1);
}


static inline uint64_t gf4v_mul_3_u64( uint64_t a )
{
	uint64_t bit0 = a&0x5555555555555555ull;
	uint64_t bit1 = a&0xaaaaaaaaaaaaaaaaull;
	return (bit0<<1)^bit0^(bit1>>1);
}


static inline uint64_t gf4v_mul_u64( uint64_t a , unsigned char b )
{
	uint64_t bit0_b = ((uint64_t)0)-((uint64_t)(b&1));
	uint64_t bit1_b = ((uint64_t)0)-((uint64_t)((b>>1)&1));
	return (a&bit0_b)^(bit1_b&gf4v_mul_2_u64(a));
}


static inline uint64_t _gf4v_mul_u64_u64( uint64_t a0 , uint64_t a1 , uint64_t b0 , uint64_t b1 )
{
	uint64_t c0 = a0&b0;
	uint64_t c2 = a1&b1;
	uint64_t c1_ = (a0^a1)&(b0^b1);
	return ((c1_^c0)<<1)^c0^c2;
}

static inline uint64_t gf4v_mul_u64_u64( uint64_t a , uint64_t b )
{
	uint64_t a0 = a&0xaaaaaaaaaaaaaaaaull;
	uint64_t a1 = (a>>1)&0xaaaaaaaaaaaaaaaaull;
	uint64_t b0 = b&0xaaaaaaaaaaaaaaaaull;
	uint64_t b1 = (b>>1)&0xaaaaaaaaaaaaaaaaull;

	return _gf4v_mul_u64_u64( a0 , a1 , b0 , b1 );
}



static inline uint64_t gf4v_squ_u64( uint64_t a )
{
	uint64_t bit1 = a&0xaaaaaaaaaaaaaaaaull;
	return a^(bit1>>1);
}



//////////////////////////////////////////////////////////////////////////////////


// gf16 := gf4[y]/y^2+y+x


static inline uint64_t gf16v_mul_u64( uint64_t a , unsigned char b )
{
	uint64_t axb0 = gf4v_mul_u64( a , b );
	uint64_t axb1 = gf4v_mul_u64( a , b>>2 );
	uint64_t a0b1 = (axb1<<2)&0xccccccccccccccccull;
	uint64_t a1b1 = axb1&0xccccccccccccccccull;
	uint64_t a1b1_2 = a1b1 >>2;

	return axb0 ^ a0b1 ^ a1b1 ^ gf4v_mul_2_u64( a1b1_2 );
}



static inline uint64_t _gf16v_mul_u64_u64( uint64_t a0 , uint64_t a1 , uint64_t a2 , uint64_t a3 , uint64_t b0 , uint64_t b1 , uint64_t b2 , uint64_t b3 )
{
	uint64_t c0 = _gf4v_mul_u64_u64( a0 , a1 , b0 , b1 );
	uint64_t c1_ =  _gf4v_mul_u64_u64( a0^a2 , a1^a3 , b0^b2 , b1^b3 );

	uint64_t c2_0 = a2&b2;
	uint64_t c2_2 = a3&b3;
	uint64_t c2_1_ = (a2^a3)&(b2^b3);
	uint64_t c2_r0 = c2_0^c2_2;
	uint64_t c2_r1 = c2_0^c2_1_;
	//uint64_t c2 = c2_r0^(c2_r1<<1);
	// GF(4) x2: (bit0<<1)^bit1^(bit1>>1);
	return ((c1_^c0)<<2) ^c0^ (c2_r0<<1)^c2_r1^(c2_r1<<1);
}

static inline uint64_t gf16v_mul_u64_u64( uint64_t a , uint64_t b )
{
	uint64_t a0 = a&0x1111111111111111ull;
	uint64_t a1 = (a>>1)&0x1111111111111111ull;
	uint64_t a2 = (a>>2)&0x1111111111111111ull;
	uint64_t a3 = (a>>3)&0x1111111111111111ull;
	uint64_t b0 = b&0x1111111111111111ull;
	uint64_t b1 = (b>>1)&0x1111111111111111ull;
	uint64_t b2 = (b>>2)&0x1111111111111111ull;
	uint64_t b3 = (b>>3)&0x1111111111111111ull;

	return _gf16v_mul_u64_u64( a0 , a1 , a2 , a3 , b0 , b1 , b2 , b3 );
}

static inline uint8_t gf256v_reduce_u64( uint64_t a )
{
	uint32_t * aa = (uint32_t *)(&a);
	uint32_t r = aa[0]^aa[1];
	return gf256v_reduce_u32( r );
}

static inline uint8_t gf16v_reduce_u64( uint64_t a )
{
	uint8_t r256 = gf256v_reduce_u64( a );
	return (r256&0xf)^(r256>>4);
}



static inline uint64_t gf16v_squ_u64( uint64_t a )
{
	uint64_t a2 = gf4v_squ_u64( a );

	return a2 ^ gf4v_mul_2_u64( (a2>>2)& 0x3333333333333333ull );
}

static inline uint64_t gf16v_mul_8_u64( uint64_t a )
{
	uint64_t a1 = a&0xccccccccccccccccull;
	uint64_t a0 = (a<<2)&0xccccccccccccccccull;
	return gf4v_mul_2_u64(a0^a1)|gf4v_mul_3_u64(a1>>2);
}





//////////////////////////////////////////////////////////


static inline uint64_t gf256v_mul_u64( uint64_t a , unsigned char b )
{
	uint64_t axb0 = gf16v_mul_u64( a , b );
	uint64_t axb1 = gf16v_mul_u64( a , b>>4 );
	uint64_t a0b1 = (axb1<<4) & 0xf0f0f0f0f0f0f0f0ull;
	uint64_t a1b1 = axb1&0xf0f0f0f0f0f0f0f0ull;
	uint64_t a1b1_4 = a1b1 >>4;

	return axb0 ^ a0b1 ^ a1b1 ^ gf16v_mul_8_u64( a1b1_4 );
}


static inline uint64_t gf256v_squ_u64( uint64_t a )
{
	uint64_t a2 = gf16v_squ_u64( a );
	uint64_t ar = (a2>>4)&0x0f0f0f0f0f0f0f0full;

	return a2 ^ gf16v_mul_8_u64( ar );
}


static inline uint64_t gf256v_mul_gf16_u64( uint64_t a , unsigned char gf16_b )
{
	return gf16v_mul_u64( a , gf16_b );
}





#ifdef  __cplusplus
}
#endif


#endif // _GF16_U64_H_

