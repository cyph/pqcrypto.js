/**
 * @file gf.c
 * Galois field implementation with multiplication using the pclmulqdq instruction
 */

#include "gf.h"
#include "parameters.h"
#include <emmintrin.h>
#include <wmmintrin.h>
#include <stdint.h>

uint16_t gf_reduce(uint64_t x, size_t deg_x);
uint16_t gf_quad(uint64_t a);


/**
 * @brief Generates exp and log lookup tables of GF(2^m).
 *
 * The logarithm of 0 is defined as 2^PARAM_M by convention. <br>
 * The last two elements of the exp table are needed by the gf_mul function from gf_lutmul.c
 * (for example if both elements to multiply are zero).
 * @param[out] exp Array of size 2^PARAM_M + 2 receiving the powers of the primitive element
 * @param[out] log Array of size 2^PARAM_M receiving the logarithms of the elements of GF(2^m)
 * @param[in] m Parameter of Galois field GF(2^m)
 */
void gf_generate(uint16_t *exp, uint16_t *log, const int16_t m) {
	uint16_t elt = 1;
	uint16_t alpha = 2; // primitive element of GF(2^PARAM_M)
	uint16_t gf_poly = PARAM_GF_POLY;

	for (size_t i = 0 ; i < (1U << m) - 1 ; ++i){
		exp[i] = elt;
		log[elt] = i;

		elt *= alpha;
		if(elt >= 1 << m)
			elt ^= gf_poly;
	}

	exp[(1 << m) - 1] = 1;
	exp[1 << m] = 2;
	exp[(1 << m) + 1] = 4;
	log[0] = 1 << m; // by convention
}



/**
 * Returns the requested power of the primitive element of GF(2^GF_M).
 * @returns a^i
 */
uint16_t gf_exp(uint16_t i) {
	return exp[i];
}



/**
 * Returns the integer i such that elt = a^i
 * where a is the primitive element of GF(2^GF_M).
 *@returns the logarithm of the given element
 */
uint16_t gf_log(uint16_t elt) {
	return log[elt];
}



/**
 * Reduces polynomial x modulo primitive polynomial GF_POLY.
 * @returns x mod GF_POLY
 * @param[in] x Polynomial of degree less than 64
 * @param[in] deg_x The degree of polynomial x
 */
uint16_t gf_reduce(uint64_t x, size_t deg_x) {  
	// Compute the distance between the primitive polynomial first two set bits
	size_t lz1 = __builtin_clz(PARAM_GF_POLY);
	size_t lz2 = __builtin_clz(PARAM_GF_POLY ^ 1<<PARAM_M);
	size_t dist = lz2 - lz1;

	// Deduce the number of steps of reduction
	size_t steps = CEIL_DIVIDE(deg_x - (PARAM_M-1), dist);

	// Reduce
	for (size_t i = 0; i < steps; ++i) {
		uint64_t mod = x >> PARAM_M;
		x &= (1<<PARAM_M) - 1;
		x ^= mod;

		size_t tz1 = 0;
		uint16_t rmdr = PARAM_GF_POLY ^ 1;
		for (size_t j = __builtin_popcount(PARAM_GF_POLY)-2; j; --j) {      
			size_t tz2 = __builtin_ctz(rmdr);
			size_t shift = tz2 - tz1;
			mod <<= shift;
			x ^= mod;
			rmdr ^= 1 << tz2;
			tz1 = tz2;
		}
	}

	return x;
}



/**
 * Multiplies two elements of GF(2^GF_M).
 * @returns the product a*b
 * @param[in] a Element of GF(2^GF_M)
 * @param[in] b Element of GF(2^GF_M)
 */
uint16_t gf_mul(uint16_t a, uint16_t b) {
	__m128i va = _mm_cvtsi32_si128(a);
	__m128i vb = _mm_cvtsi32_si128(b);
	__m128i vab = _mm_clmulepi64_si128(va, vb, 0);
	uint32_t ab = _mm_cvtsi128_si32(vab);

	return gf_reduce(ab, 2*(PARAM_M-1));
}



/**
 * Squares an element of GF(2^GF_M).
 * @returns a^2
 * @param[in] a Element of GF(2^GF_M)
 */
uint16_t gf_square(uint16_t a) {
	uint32_t b = a;
	uint32_t s = b & 1;     
	for (size_t i = 1; i < PARAM_M; ++i) {
		b <<= 1;
		s ^= b & (1 << 2*i);
	}

	return gf_reduce(s, 2*(PARAM_M-1));
}


/**
 * Computes the inverse of an element of GF(2^GF_M) by fast exponentiation.
 * @returns the inverse of a
 * @param[in] a Element of GF(2^GF_M)
 */
uint16_t gf_inverse(uint16_t a) {
	size_t pow = (1 << PARAM_M) - 2;
	uint16_t inv = 1;

	do {
		if (pow & 1)
			inv = gf_mul(inv, a);
		a = gf_square(a);
		pow >>= 1;
	} while (pow);

	return inv;
}



/**
 * Returns i modulo 2^GF_M-1.
 * i must be less than 2*(2^GF_M-1).
 * Therefore, the return value is either i or i-2^GF_M+1.
 * @returns i mod (2^GF_M-1)
 * @param[in] i The integer whose modulo is taken
 */
uint16_t gf_mod(uint16_t i) {
	uint16_t tmp = i - PARAM_GF_MUL_ORDER;

	// mask = 0xffff if (i < GF_MUL_ORDER)
	int16_t mask = -(tmp >> 15);

	return tmp + (mask & PARAM_GF_MUL_ORDER);
}