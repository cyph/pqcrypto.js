/* ****************************** *
 * Titanium_CCA_med               *
 * Implemented by Raymond K. ZHAO *
 *                                *
 * CPA Encryption                 *
 * ****************************** */

#include "encrypt.h"
#include "param.h"
#include "fastrandombytes.h"
#include "sampler.h"
#include "ntt.h"
#include "fastmodulo.h"
#include "littleendian.h"
#include "pack.h"
#include <string.h>
#include <stdint.h>
#include <x86intrin.h>

static const __m256i V_B4Q_B4Q_B4Q_B4Q = {BARRETT_FACTOR_4Q, BARRETT_FACTOR_4Q, BARRETT_FACTOR_4Q, BARRETT_FACTOR_4Q};
static const __m256i V_Q_Q_Q_Q = {Q, Q, Q, Q};
static const __m256i V_Q2_Q2_Q2_Q2 = {Q2, Q2, Q2, Q2};
static const __m256i V_1_1_1_1 = {1, 1, 1, 1};
static const __m256i V_ENC = {(Q + 1) / 2, (Q + 1) / 2, (Q + 1) / 2, (Q + 1) / 2};

int crypto_encrypt_keypair(unsigned char *pk, unsigned char *sk, const unsigned char *randomness)
{
	uint64_t s[N + D + K1 + 1];
	uint64_t a[T][N + D + K1 + 1];
	uint64_t b[T][D + K1 + 1];
	uint64_t e[T][D + K1 + 1];
	unsigned char seed_sk[CRYPTO_RANDOMBYTES], seed_pk[CRYPTO_RANDOMBYTES];
	uint32_t i, j;
	__m256i u, v, t, t1;
	
	fastrandombytes_setseed(randomness);
	
	/* generate seed_pk and seed_sk */
	fastrandombytes(seed_sk, CRYPTO_RANDOMBYTES);
	fastrandombytes(seed_pk, CRYPTO_RANDOMBYTES);
		
	fastrandombytes_setseed(seed_pk); /* use seed_pk to sample a_i */
	for (i = 0; i < T; i++)
	{
		sampler_zq(a[i], N, ZQ_BYTPCA); /* a_i <-- U(Z_q^{<n}[x]) */
		NTT_N_NDK(a[i]); /* transform a_i here */
	}
	
	fastrandombytes_setseed(seed_sk); /* change to seed_sk */
	
	/* sample s <-- NTT(lambda_s) over Z_q^{<n+d+k+1)[x] 
	 * generate two more samples, which wouldn't affect the result of middle-product a_i mp_{d+k} s
	 * (can save the adjustment of s to make the most significant bits of lambda_s be 0) */
	sampler_zq(s, N + D + K1 + 1, ZQ_BYTPCS);
	
	sampler_binomial(e); /* sample e_i <-- lambda_e over Z_q^{<d+k}[x] */
	
	for (i = 0; i < T; i++)
	{
		/* a_i <-- a_i mp_{d+k} s */
		for (j = 0; j < N + D + K1 + 1; j += 4)
		{
			u = _mm256_loadu_si256((__m256i *)(a[i] + j));
			v = _mm256_loadu_si256((__m256i *)(s + j));
			t = _mm256_mul_epu32(u, v);
			_mm256_storeu_si256((__m256i *)(a[i] + j), t);
			a[i][j] %= Q;
			a[i][j + 1] %= Q;
			a[i][j + 2] %= Q;
			a[i][j + 3] %= Q;
		}
		
		INTT_NDK_DK(a[i]);
		
		/* b_i <-- as_i + e_i */
		for (j = 0; j < D + K1 + 1; j += 4)
		{
			u = _mm256_loadu_si256((__m256i *)(a[i] + j));
			v = _mm256_loadu_si256((__m256i *)(e[i] + j));
			t = _mm256_add_epi64(u, v);
			t1 = _mm256_mul_epu32(t, V_B4Q_B4Q_B4Q_B4Q);
			t1 = _mm256_srli_epi64(t1, BARRETT_BITSHIFT_4Q);
			t1 = _mm256_mul_epu32(t1, V_Q_Q_Q_Q);
			t = _mm256_sub_epi64(t, t1);
			_mm256_storeu_si256((__m256i *)(b[i] + j), t);
		}

		/* pre-transform b_i here to make Enc and Dec faster */ 
		b[i][D + K1] = 0;
		NTT_DK_DK_INV(b[i]);
	}
	
	/* encode pk and sk to binary string */
	/* sk <-- seed_sk */
	memcpy(sk, seed_sk, CRYPTO_RANDOMBYTES);
	
	/* pk <-- (seed_pk, b_i) */
	memcpy(pk, seed_pk, CRYPTO_RANDOMBYTES); 
	for (i = 0; i < T; i++)
	{
		poly_encode(pk + CRYPTO_RANDOMBYTES + i * (D + K1 + 1) / 8 * Q_BITS, b[i], D + K1 + 1);
	}
	
	return 0;
}

int crypto_encrypt(unsigned char *c, unsigned long long *clen, const unsigned char *m, unsigned long long mlen, const unsigned char *pk, const unsigned char *randomness)
{
	uint64_t mu[D];
	uint64_t r[T][N + K1 + 1];
	uint64_t r2[T][N + K1 + 1];
	uint64_t a[T][N + K1 + 1];
	uint64_t b[T][D + K1 + 1];
	uint64_t c1[N + K1 + 1];
	uint64_t c2[D + K1 + 1];
	uint32_t i, j;
	__m256i u, v, t;
	
	/* reconstruct a_i by the seed saved in pk */
	fastrandombytes_setseed(pk);
	
	for(i = 0; i < T; i++)
	{
		sampler_zq(a[i], N, ZQ_BYTPCA); /* reconstruct a_i */
		NTT_N_NK(a[i]); /* transform a_i here */
	}
	
	/* take hash(m) as the seed */
	fastrandombytes_setseed(randomness);
	
	/* decode pk to polynomials */
	for (i = 0; i < T; i++) 
	{
		poly_decode(b[i], pk + CRYPTO_RANDOMBYTES + i * (D + K1 + 1) / 8 * Q_BITS, D + K1 + 1);
	}
	
	/* convert the message to a bit string and map {0,1}^D-->{0,Q/2}^D */
	memset(mu, 0, sizeof(mu));
	for (i = 0; i < mlen; i++)
	{
		t = _mm256_set_epi64x(m[i] >> 3, m[i] >> 2, m[i] >> 1, m[i]);
		t = _mm256_and_si256(t, V_1_1_1_1);
		t = _mm256_mul_epu32(t, V_ENC);
		_mm256_storeu_si256((__m256i *)(mu + i * 8), t);

		t = _mm256_set_epi64x(m[i] >> 7, m[i] >> 6, m[i] >> 5, m[i] >> 4);
		t = _mm256_and_si256(t, V_1_1_1_1);
		t = _mm256_mul_epu32(t, V_ENC);
		_mm256_storeu_si256((__m256i *)(mu + i * 8 + 4), t);
	}
	
	/* sample r_i <-- lambda_r over Z_q^{<k+1}[x] */
	sampler_zb(r);
	memcpy(r2, r, sizeof(r));
	for (i = 0; i < T; i++)
	{
		/* transform r_i here */
		NTT_K_NK(r[i]); 
		NTT_K_DK(r2[i]);
	}
	
	/* c1 <-- NTT(sum(r_i * a_i)) */
	memset(c1, 0, sizeof(c1));
	for (i = 0; i < T; i++)
	{
		for (j = 0; j < N + K1 + 1; j += 4)
		{
			u = _mm256_loadu_si256((__m256i *)(c1 + j));
			v = _mm256_loadu_si256((__m256i *)(r[i] + j));
			t = _mm256_loadu_si256((__m256i *)(a[i] + j));
			v = _mm256_mul_epu32(v, t);
			t = _mm256_add_epi64(u, v);
			_mm256_storeu_si256((__m256i *)(c1 + j), t);
			c1[j] %= Q;
			c1[j + 1] %= Q;
			c1[j + 2] %= Q;
			c1[j + 3] %= Q;
		}
	}
	
	/* c2 <-- sum(r_i mp_d b_i) + mu */
	memset(c2, 0, sizeof(c2));
	for (i = 0; i < T; i++) 
	{
		for (j = 0; j < D + K1 + 1; j += 4)
		{
			u = _mm256_loadu_si256((__m256i *)(c2 + j));
			v = _mm256_loadu_si256((__m256i *)(r2[i] + j));
			t = _mm256_loadu_si256((__m256i *)(b[i] + j));
			v = _mm256_mul_epu32(v, t);
			t = _mm256_add_epi64(u, v);
			_mm256_storeu_si256((__m256i *)(c2 + j), t);
			c2[j] %= Q;
			c2[j + 1] %= Q;
			c2[j + 2] %= Q;
			c2[j + 3] %= Q;
		}
	}
	
	INTT_DK_D(c2);
	
	for (i = 0; i < D; i += 4)
	{
		u = _mm256_loadu_si256((__m256i *)(c2 + i));
		v = _mm256_loadu_si256((__m256i *)(mu + i));
		t = _mm256_add_epi64(u, v);
		_mm256_storeu_si256((__m256i *)(c2 + i), t);
	}
	
	/* encode c <-- (c1,c2) */
	poly_encode(c, c1, N + K1 + 1);
	poly_encode_c2(c + (N + K1 + 1) / 8 * Q_BITS, c2, D);
	
	*clen = (N + K1 + 1) / 8 * Q_BITS + D * C2_COMPRESSION_BYTE;
	
	return 0;
}

int crypto_encrypt_open(unsigned char *m, unsigned long long *mlen, const unsigned char *c, unsigned long long clen, const unsigned char *sk)
{
	uint64_t s[N + D + K1 + 1];
	uint64_t c1[N + D + K1 + 1];
	uint64_t c2[D];
	uint64_t c_prime[D];
	uint32_t i, j;
	__m256i u, v, t;

	/* recover sk */
	fastrandombytes_setseed(sk);
	sampler_zq(s, N + D + K1 + 1, ZQ_BYTPCS);
	
	/* decode c to polynomials c1,c2*/
	poly_decode(c1, c, N + K1 + 1);
	poly_decode_c2(c2, c + (N + K1 + 1) / 8 * Q_BITS, D);
	
	/* transform c1 back here (will save 1 NTT in the Encap) */
	INTT_NK_NK_INV(c1);
	
	/* c1 <-- c1 mp_d s*/
	NTT_NK_NDK(c1);
	
	for (i = 0; i < N + D + K1 + 1; i += 4)
	{
		u = _mm256_loadu_si256((__m256i *)(c1 + i));
		v = _mm256_loadu_si256((__m256i *)(s + i));
		t = _mm256_mul_epu32(u, v);
		_mm256_storeu_si256((__m256i *)(c1 + i), t);
		c1[i] %= Q;
		c1[i + 1] %= Q;
		c1[i + 2] %= Q;
		c1[i + 3] %= Q;
	}
	
	INTT_NDK_D(c1);

	/* c^prime <-- c2 - c1 mp_d s */	
	for (i = 0; i < D; i += 4)
	{
		u = _mm256_loadu_si256((__m256i *)(c2 + i));
		v = _mm256_loadu_si256((__m256i *)(c1 + i));
		t = _mm256_add_epi64(V_Q2_Q2_Q2_Q2, u);
		t = _mm256_sub_epi64(t, v);
		_mm256_storeu_si256((__m256i *)(c_prime + i), t);
	}

	/* decode the message (still work for x+kQ) */
	memset(m, 0, D_BYTES);
	for (i = 0; i < D_BYTES; i++)
	{
		for (j = 0; j < 8; j++)
		{
			m[i] |= ((((c_prime[i * 8 + j] << 1) + Q / 2) / Q) & 0x1) << j;
		}
	}
	
	*mlen = D_BYTES;
	return 0;
}
