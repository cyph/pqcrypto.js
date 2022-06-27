#ifndef _GF4591_H_
#define _GF4591_H_

#include <stdint.h>

// q61 = 765; q = 6*q61+1;


#ifdef  __cplusplus
extern  "C" {
#endif


typedef uint16_t sto_t;


static inline sto_t gf_is_nonzero( sto_t a )
{
	sto_t aa = a-1;
	aa = ~aa;
	return (aa>>15)&1;
}

static inline sto_t _gf_reduce( sto_t a )
{
	uint16_t aa = a-4591;
	aa = ~aa;
	aa >>= 15; /// sign bit
	uint16_t mask = 0-aa;
	return a-(mask&4591);
}

static inline sto_t gf_neg( sto_t a )
{
	sto_t mask = 0 - gf_is_nonzero(a);
	return (mask&4591)-a;
}

static inline sto_t gf_add( sto_t a , sto_t b )
{
	sto_t r = a+b;
	return _gf_reduce(r);
}

static inline sto_t gf_sub( sto_t a , sto_t b )
{
	sto_t r = 4591+a-b;
	return _gf_reduce(r);
}


/// 14617 = (2^10/4591)* 2^16
#define _REDUC (14617)


static inline sto_t gf_mul( sto_t _a , sto_t _b )
{
	//return ((a*b)%4591);

	uint32_t a = _a;
	uint32_t b = _b;
	uint32_t c = a*b;

	/// estimate c = ?*4591
	uint32_t d = (c>>10)*_REDUC;
	uint32_t e = d>>16;
	/// reduce
	uint32_t f = e*4591;
	uint32_t _r = c-f;
	sto_t r = _r;
	return _gf_reduce(r);
}

static inline sto_t gf_squ( sto_t a )
{
	return gf_mul(a,a);
}




static inline sto_t gf_inv( sto_t a )
{
	/// 4591 = 0x11ef <-- 1,0001,1110,1111
	/// 4549 : 1,0001,1110,1101
	sto_t x10 = gf_squ( a );
	sto_t x11 = gf_mul( x10 , a );
	sto_t x100 = gf_squ( x10 );
	sto_t x1000 = gf_squ( x100 );
	sto_t r = x1000;
	for(unsigned i=0;i<2;i++) r = gf_squ(r);
	r = gf_mul( r , x11 );
	for(unsigned i=0;i<2;i++) r = gf_squ(r);
	r = gf_mul( r , x11 );
	for(unsigned i=0;i<3;i++) r = gf_squ(r);
	r = gf_mul( r , x11 );
	for(unsigned i=0;i<2;i++) r = gf_squ(r);
	r = gf_mul( r , a );

	return r;
}


#ifdef  __cplusplus
}
#endif


#endif
