/* ****************************** *
 * Titanium_CCA_std               *
 * Implemented by Raymond K. ZHAO *
 *                                *
 * CPA Encryption                 *
 * ****************************** */

#include "encrypt.h"
#include "param.h"
#include "fastrandombytes.h"
#include "sampler.h"
#include "ntt.h"
#include "product.h"
#include "littleendian.h"
#include "pack.h"
#include <string.h>
#include <stdint.h>

int crypto_encrypt_keypair(unsigned char *pk, unsigned char *sk, const unsigned char *randomness)
{
	uint32_t s[N + D + K + 1];
	uint32_t a[T][N + D + K + 1];
	uint32_t b[T][D + K + 1];
	uint32_t e[T][D + K];
	unsigned char seed_sk[CRYPTO_RANDOMBYTES], seed_pk[CRYPTO_RANDOMBYTES];
	uint32_t i, j;
	
	fastrandombytes_setseed(randomness);
	
	/* generate seed_pk and seed_sk */
	fastrandombytes(seed_sk, CRYPTO_RANDOMBYTES);
	fastrandombytes(seed_pk, CRYPTO_RANDOMBYTES);
		
	fastrandombytes_setseed(seed_pk); /* use seed_pk to sample a_i */
	for (i = 0; i < T; i++)
	{
		sampler_zq(a[i], N, ZQ_BYTPCA); /* a_i <-- U(Z_q^{<n}[x]) */
	}
	
	fastrandombytes_setseed(seed_sk); /* change to seed_sk */
	
	/* sample s <-- NTT(lambda_s) over Z_q^{<n+d+k+1)[x] */
	sampler_zq(s, N + D + K + 1, ZQ_BYTPCS);
	/* transform s back */
	INTT_NDK_NDK(s);
	
	sampler_binomial(e); /* sample e_i <-- lambda_e over Z_q^{<d+k}[x] */
	
	for (i = 0; i < T; i++)
	{
		/* a_i <-- a_i mp_{d+k} s */
		mp(b[i], D + K, a[i], N, s, N + D + K - 1);

		/* b_i <-- as_i + e_i */
		for (j = 0; j < D + K; j++)
		{
			b[i][j] = (b[i][j] + e[i][j]) % Q;
		}

		/* b_i pre-transform */ 
		b[i][D + K] = 0;
		NTT_DK_DK_INV(b[i]);
	}
	
	/* encode pk and sk to binary string */
	/* sk <-- seed_sk */
	memcpy(sk, seed_sk, CRYPTO_RANDOMBYTES);
	
	/* pk <-- (seed_pk, b_i) */
	memcpy(pk, seed_pk, CRYPTO_RANDOMBYTES); 
	for (i = 0; i < T; i++)
	{
		poly_encode(pk + CRYPTO_RANDOMBYTES + i * (D + K + 1) / 8 * Q_BITS, b[i], D + K + 1);
	}
	
	return 0;
}

int crypto_encrypt(unsigned char *c, unsigned long long *clen, const unsigned char *m, unsigned long long mlen, const unsigned char *pk, const unsigned char *randomness)
{
	uint32_t mu[D];
	uint32_t r[T][N + K + 1];
	uint32_t a[T][N + K + 1];
	uint32_t b[T][D + K + 1];
	uint32_t c1[N + K + 1];
	uint32_t c2[D + K + 1];
	uint32_t i, j;
	uint32_t c1_t[N + K + 1];
	uint32_t c2_t[D];
	
	/* reconstruct a_i by the seed saved in pk */
	fastrandombytes_setseed(pk);
	
	for(i = 0; i < T; i++)
	{
		sampler_zq(a[i], N, ZQ_BYTPCA); /* reconstruct a_i */
	}
	
	/* take hash(m) as the seed */
	fastrandombytes_setseed(randomness);
	
	/* decode pk to polynomials */
	for (i = 0; i < T; i++) 
	{
		poly_decode(b[i], pk + CRYPTO_RANDOMBYTES + i * (D + K + 1) / 8 * Q_BITS, D + K + 1);
		INTT_DK_DK(b[i]); /* transform b_i back */
	}
	
	/* convert the message to a bit string and map {0,1}^D-->{0,Q/2}^D */
	memset(mu, 0, sizeof(mu));
	for (i = 0; i < mlen; i++)
	{
		for (j = 0; j < 8; j++)
		{
			mu[i * 8 + j] = ((m[i] >> j) & 0x1) * ((Q + 1) / 2);
		}
	}
	
	/* sample r_i <-- lambda_r over Z_q^{<k+1}[x] */
	sampler_zb(r);
	
	/* c1 <-- NTT(sum(r_i * a_i)) */
	memset(c1, 0, sizeof(c1));
	for (i = 0; i < T; i++)
	{
		product(c1_t, r[i], K + 1, a[i], N);
		for (j = 0; j < N + K; j++)
		{
			c1[j] = (c1[j] + c1_t[j]) % Q;
		}
	}
	NTT_NK_NK(c1);
	
	/* c2 <-- sum(r_i mp_d b_i) + mu */
	memset(c2, 0, sizeof(c2));
	for (i = 0; i < T; i++) 
	{
		mp(c2_t, D, r[i], K + 1, b[i], D + K);
		for (j = 0; j < D; j++)
		{
			c2[j] = (c2[j] + c2_t[j]) % Q;
		}
	}
	
	for (i = 0; i < D; i++)
	{
		c2[i] = c2[i] + mu[i]; /* will be reduced by encoding */
	}
	
	/* encode c <-- (c1,c2) */
	poly_encode(c, c1, N + K + 1);
	poly_encode_c2(c + (N + K + 1) / 8 * Q_BITS, c2, D);
	
	*clen = (N + K + 1) / 8 * Q_BITS + D * C2_COMPRESSION_BYTE;
	
	return 0;
}

int crypto_encrypt_open(unsigned char *m, unsigned long long *mlen, const unsigned char *c, unsigned long long clen, const unsigned char *sk)
{
	uint32_t s[N + D + K + 1];
	uint32_t c1[N + D + K + 1];
	uint32_t c2[D];
	uint32_t c_prime[D];
	uint32_t i, j;
	
	/* recover sk */
	fastrandombytes_setseed(sk);
	sampler_zq(s, N + D + K + 1, ZQ_BYTPCS);
	INTT_NDK_NDK(s);
	
	/* decode c to polynomials c1,c2*/
	poly_decode(c1, c, N + K + 1);
	poly_decode_c2(c2, c + (N + K + 1) / 8 * Q_BITS, D);
	
	/* transform c1 back */
	INTT_NK_NK_INV(c1);
	
	mp(c_prime, D, c1, N + K, s, N + D + K - 1);
	/* c^prime <-- c2 - c1 mp_d s */	
	for (i = 0; i < D; i++)
	{
		c_prime[i] = Q2 + c2[i] - c_prime[i];
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
