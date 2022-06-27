/* ****************************** *
 * Titanium_CCA_super             *
 * Implemented by Raymond K. ZHAO *
 *                                *
 * Samplers                       *
 * ****************************** */
 
#include "sampler.h"
#include "param.h"
#include "fastrandombytes.h"
#include "littleendian.h"
#include "fastmodulo.h"
#include <stdint.h>
#include <x86intrin.h>

/* rejection sampling in the range of ZQ_T * Q < 2^24, 
 * then mod the sample by Q 
 * (to reduce the rejection rate) */
#define ZQ_T ((1 << BARRETT_BITSHIFT_ZQ) / Q)
#define ZQ_Q (Q * ZQ_T)

#define B1_T (NUM_B1 / (K1 + 1)) /* the number of r_i exclusively sampling from B_1 */
#define B1_REMAINING (NUM_B1 - B1_T * (K1 + 1)) /* the number of B_1 samples in r_t which has both B_1 and B_2 */

static const __m256i V_B_1_BITMASK = {B_1_BITMASK, B_1_BITMASK, B_1_BITMASK, B_1_BITMASK};
static const __m256i V_B_2_BITMASK = {B_2_BITMASK, B_2_BITMASK, B_2_BITMASK, B_2_BITMASK};
static const __m256i V_1_1_1_1 = {1, 1, 1, 1};
static const __m256i V_0_0_0_0 = {0, 0, 0, 0};
static const __m256i V_fe_fe_fe_fe = {0xfffffffffffffffeLL, 0xfffffffffffffffeLL, 0xfffffffffffffffeLL, 0xfffffffffffffffeLL};
static const __m256i V_Q_Q_Q_Q = {Q, Q, Q, Q};
static const __m256i V_B_ADDMASK = {BINOMIAL_ADDMASK, BINOMIAL_ADDMASK, BINOMIAL_ADDMASK, BINOMIAL_ADDMASK};
static const __m256i V_B_MASK = {BINOMIAL_MASK, BINOMIAL_MASK, BINOMIAL_MASK, BINOMIAL_MASK};

/* Sampler of r_i
 * generate Ndec1 samples by B_1 and T*(K+1)-Ndec1 samples by B_2
 * then sample and apply the sign bit to each sample */
void sampler_zb(uint64_t sample[T][N + K1 + 1])
{
	unsigned char r[B_BYTE * T * (K1 + 1) + T * (K1 + 1) / 8];
	uint32_t i;
	uint32_t t;
	unsigned char *rr;
	__m256i y, x;
	
	fastrandombytes(r, B_BYTE * T * (K1 + 1) + T * (K1 + 1) / 8);
	
	/* sample B1_T r_i full of B_1 */
	for (t = 0; t < B1_T; t++)
	{
		rr = r + B_BYTE * t * (K1 + 1);
		for (i = 0; i < K1 + 1; i += 4)
		{
			y = _mm256_set_epi64x(LOAD_B(rr + (i + 3) * B_BYTE), LOAD_B(rr + (i + 2) * B_BYTE), LOAD_B(rr + (i + 1) * B_BYTE), LOAD_B(rr + i * B_BYTE));
			y = _mm256_and_si256(y, V_B_1_BITMASK);
			y = _mm256_add_epi64(y, V_1_1_1_1);
			
			_mm256_storeu_si256((__m256i *)(sample[t] + i), y);
		}
	}
	
	/* for r_t, sample the B_1 samples first */
	rr = r + B_BYTE * B1_T * (K1 + 1);
	for (i = 0; i < B1_REMAINING; i += 4)
	{
		y = _mm256_set_epi64x(LOAD_B(rr + (i + 3) * B_BYTE), LOAD_B(rr + (i + 2) * B_BYTE), LOAD_B(rr + (i + 1) * B_BYTE), LOAD_B(rr + i * B_BYTE));
		y = _mm256_and_si256(y, V_B_1_BITMASK);
		y = _mm256_add_epi64(y, V_1_1_1_1);
			
		_mm256_storeu_si256((__m256i *)(sample[B1_T] + i), y);
	}
	
	/* sample the remaining part of B_2 for r_t */
	for (i = B1_REMAINING; i < K1 + 1; i += 4)
	{
		y = _mm256_set_epi64x(LOAD_B(rr + (i + 3) * B_BYTE), LOAD_B(rr + (i + 2) * B_BYTE), LOAD_B(rr + (i + 1) * B_BYTE), LOAD_B(rr + i * B_BYTE));
		y = _mm256_and_si256(y, V_B_2_BITMASK);
		y = _mm256_add_epi64(y, V_1_1_1_1);
			
		_mm256_storeu_si256((__m256i *)(sample[B1_T] + i), y);
	}
	
	/* sample those r_i full of B_2 */
	for (t = B1_T + 1; t < T; t++)
	{
		rr = r + B_BYTE * t * (K1 + 1);
		for (i = 0; i < K1 + 1; i += 4)
		{
			y = _mm256_set_epi64x(LOAD_B(rr + (i + 3) * B_BYTE), LOAD_B(rr + (i + 2) * B_BYTE), LOAD_B(rr + (i + 1) * B_BYTE), LOAD_B(rr + i * B_BYTE));
			y = _mm256_and_si256(y, V_B_2_BITMASK);
			y = _mm256_add_epi64(y, V_1_1_1_1);
			
			_mm256_storeu_si256((__m256i *)(sample[t] + i), y);
		}
	}

	/* apply the sign bits in constant time
	 * each random byte will generate 8 sign bits
	 * each sample will become [-B,-1] U [1,B] after applying the sign bit */
	for (t = 0; t < T; t++)
	{
		rr = r + (B_BYTE * T * (K1 + 1)) + (t * (K1 + 1) / 8);
		for (i = 0; i < K1 + 1; i += 8)
		{
			x = _mm256_set_epi64x(rr[i / 8] >> 3, rr[i / 8] >> 2, rr[i / 8] >> 1, rr[i / 8]);
			x = _mm256_and_si256(x, V_1_1_1_1);
			x = _mm256_sub_epi64(V_0_0_0_0, x);
			x = _mm256_and_si256(x, V_fe_fe_fe_fe);
			x = _mm256_xor_si256(x, V_1_1_1_1);
			y = _mm256_loadu_si256((__m256i *)(sample[t] + i));
			y = _mm256_mul_epi32(y, x);
			y = _mm256_add_epi64(V_Q_Q_Q_Q, y);
			_mm256_storeu_si256((__m256i *)(sample[t] + i), y);
			
			x = _mm256_set_epi64x(rr[i / 8] >> 7, rr[i / 8] >> 6, rr[i / 8] >> 5, rr[i / 8] >> 4);
			x = _mm256_and_si256(x, V_1_1_1_1);
			x = _mm256_sub_epi64(V_0_0_0_0, x);
			x = _mm256_and_si256(x, V_fe_fe_fe_fe);
			x = _mm256_xor_si256(x, V_1_1_1_1);
			y = _mm256_loadu_si256((__m256i *)(sample[t] + i + 4));
			y = _mm256_mul_epi32(y, x);
			y = _mm256_add_epi64(V_Q_Q_Q_Q, y);
			_mm256_storeu_si256((__m256i *)(sample[t] + i + 4), y);
		}	
	}
}

/* return (x >= y) */
static inline int ct_ge_u32(uint32_t x, uint32_t y)
{
    return 1 ^ ((x^((x^y)|((x-y)^y)))>>31);
}

/* Sample slen uniforms in Z_q 
 * 
 * Generate a little bit more random bytes, since re-run the PRG if running out of the randomness would be costly (also for the constant time implementation)
 * 
 * also, to reduce the rejection rate, do the rejection in the range of ZQ_T * Q, which is "just" smaller than 2^24, 
 * and mod each sample by Q */
void sampler_zq(uint64_t *sample, uint32_t slen, uint32_t bytpc)
{
	unsigned char r[ZQ_BYTES * ZQ_BYTPCS];
	uint32_t i = 0, j = 0;
	uint32_t x;
	
	fastrandombytes(r, ZQ_BYTES * bytpc);
	
	while (j < slen)
	{
		do
		{
			if (i == bytpc)
			{
				fastrandombytes(r, ZQ_BYTES * bytpc);
				i = 0;
			}
			
			x = LOAD_ZQ(r + ZQ_BYTES * (i++));
		} while (ct_ge_u32(x, ZQ_Q)); /* rejection in the range of ZQ_T * Q */
		
		sample[j++] = barrett_zq(x); /* barrett reduction here */
	}
}

/* Binomial Sampler */
void sampler_binomial(uint64_t sample[T][D + K1 + 1])
{
	unsigned char r[T * (D + K1 + 1) * BINOMIAL_BYTE];
	uint32_t i, j;
	uint32_t t;
	unsigned char *rr;
	__m256i x, s, xx, s1, s2;
	
	fastrandombytes(r, T * (D + K1 + 1) * BINOMIAL_BYTE);
	
	for (t = 0; t < T; t++)
	{
		rr = r + t * (D + K1 + 1) * BINOMIAL_BYTE;
		
		/* x = (r1,r2)
		 * s1 = bit_sum(r1 & bitmask), s2 = bit_sum(r2 & bitmask), where bit_sum is the number of "1" bits, and bitmask is the mask to select k bits
		 * sample <-- s1 - s2 */
		for (i = 0; i < D + K1 + 1; i += 4)
		{
			x = _mm256_set_epi64x(LOAD_BINOMIAL(rr + (i + 3) * BINOMIAL_BYTE), LOAD_BINOMIAL(rr + (i + 2) * BINOMIAL_BYTE), LOAD_BINOMIAL(rr + (i + 1) * BINOMIAL_BYTE), LOAD_BINOMIAL(rr + i * BINOMIAL_BYTE));
		
			s = V_0_0_0_0;
			for (j = 0; j < BINOMIAL_K; j++)
			{
				xx = _mm256_srli_epi64(x, j);
				xx = _mm256_and_si256(xx, V_B_ADDMASK);
				s = _mm256_add_epi64(s, xx);
			}
			
			s1 = _mm256_and_si256(s, V_B_MASK);
			s2 = _mm256_srli_epi64(s, BINOMIAL_SHIFT);
			s = _mm256_add_epi64(V_Q_Q_Q_Q, s1);
			s = _mm256_sub_epi64(s, s2);
			
			_mm256_storeu_si256((__m256i *)(sample[t] + i), s);
		}
	}
}
