/* ****************************** *
 * Titanium_CCA_toy               *
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

/* mapping between the results of NTT_512 and NTT_1024
 * because omega_1024^2=omega_512, we can directly get the results of NTT_512 from NTT_1024 for the same input */
static const uint32_t mapping_r[D + K1 + 1]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255,512,513,514,515,516,517,518,519,520,521,522,523,524,525,526,527,528,529,530,531,532,533,534,535,536,537,538,539,540,541,542,543,544,545,546,547,548,549,550,551,552,553,554,555,556,557,558,559,560,561,562,563,564,565,566,567,568,569,570,571,572,573,574,575,576,577,578,579,580,581,582,583,584,585,586,587,588,589,590,591,592,593,594,595,596,597,598,599,600,601,602,603,604,605,606,607,608,609,610,611,612,613,614,615,616,617,618,619,620,621,622,623,624,625,626,627,628,629,630,631,632,633,634,635,636,637,638,639,640,641,642,643,644,645,646,647,648,649,650,651,652,653,654,655,656,657,658,659,660,661,662,663,664,665,666,667,668,669,670,671,672,673,674,675,676,677,678,679,680,681,682,683,684,685,686,687,688,689,690,691,692,693,694,695,696,697,698,699,700,701,702,703,704,705,706,707,708,709,710,711,712,713,714,715,716,717,718,719,720,721,722,723,724,725,726,727,728,729,730,731,732,733,734,735,736,737,738,739,740,741,742,743,744,745,746,747,748,749,750,751,752,753,754,755,756,757,758,759,760,761,762,763,764,765,766,767};

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
		sampler_zq(a[i], N_act, ZQ_BYTPCA); /* a_i <-- U(Z_q^{<n}[x]) */
		memset(a[i] + N_act, 0, sizeof(uint64_t) * (N - N_act)); /* padding with 0 */
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
		sampler_zq(a[i], N_act, ZQ_BYTPCA); /* reconstruct a_i */
		memset(a[i] + N_act, 0, sizeof(uint64_t) * (N - N_act)); /* padding with 0 */
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
	for (i = 0; i < T; i++)
	{
		NTT_K_NK(r[i]); /* transform r_i here */
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
			v = _mm256_set_epi64x(r[i][mapping_r[j + 3]], r[i][mapping_r[j + 2]], r[i][mapping_r[j + 1]], r[i][mapping_r[j]]);
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
