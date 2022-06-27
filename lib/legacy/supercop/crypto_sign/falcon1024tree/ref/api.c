/*
 * Wrapper for implementing the SUPERCOP API.
 */

#include <stddef.h>
#include <string.h>

#include "api.h"
#include "crypto_sign.h"
#include "inner.h"

#define NONCELEN   40
#define SEEDLEN    48

int randombytes(unsigned char *x, unsigned long long xlen);

int
crypto_sign_keypair(unsigned char *pk, unsigned char *sk)
{
	union {
		uint8_t b[48 * 1024];
		uint64_t dummy_u64;
		fpr dummy_fpr;
	} tmp;
	int8_t f[1024], g[1024], F[1024], G[1024];
	uint16_t h[1024];
	unsigned char seed[SEEDLEN];
	inner_shake256_context rng;
	union {
		fpr f[(10 + 5) * 1024];
		uint64_t u[(10 + 5) * 1024];
	} esk;
	size_t u, v;


	/*
	 * Generate key pair.
	 */
	randombytes(seed, sizeof seed);
	inner_shake256_init(&rng);
	inner_shake256_inject(&rng, seed, sizeof seed);
	inner_shake256_flip(&rng);
	falcon1024tree_ref_keygen(
		&rng, f, g, F, G, h, 10, tmp.b);

	/*
	 * Expand private key.
	 */
	falcon1024tree_ref_expand_privkey(
		esk.f, f, g, F, G, 10, tmp.b);


	/*
	 * Copy private key into destination buffer. We enforce
	 * little-endian encoding for reproducibility of test cases
	 * across platforms.
	 */
#if FALCON_LE
	(void)u;
	memcpy(sk, esk.f, sizeof esk.f);
#else
	for (u = 0; u < (sizeof esk.u) / sizeof esk.u[0]; u ++) {
		uint64_t w;

		w = esk.u[u];
		sk[(u << 3) + 0] = (unsigned char)w;
		sk[(u << 3) + 1] = (unsigned char)(w >> 8);
		sk[(u << 3) + 2] = (unsigned char)(w >> 16);
		sk[(u << 3) + 3] = (unsigned char)(w >> 24);
		sk[(u << 3) + 4] = (unsigned char)(w >> 32);
		sk[(u << 3) + 5] = (unsigned char)(w >> 40);
		sk[(u << 3) + 6] = (unsigned char)(w >> 48);
		sk[(u << 3) + 7] = (unsigned char)(w >> 56);
	}
#endif

	/*
	 * Convert public key to NTT + Montgomery representation (like for
	 * the private key, we precompute values that depend only on the
	 * keys during keygen).
	 */
	falcon1024tree_ref_to_ntt_monty(h, 10);

	/*
	 * Encode public key.
	 */
	pk[0] = 0x80 + 10;
	v = falcon1024tree_ref_modq_encode(
		pk + 1, CRYPTO_PUBLICKEYBYTES - 1, h, 10);
	if (v != CRYPTO_PUBLICKEYBYTES - 1) {
		return -1;
	}

	return 0;
}

int
crypto_sign(unsigned char *sm, unsigned long long *smlen,
	const unsigned char *m, unsigned long long mlen,
	const unsigned char *sk)
{
	union {
		uint8_t b[48 * 1024];
		uint64_t dummy_u64;
		fpr dummy_fpr;
	} tmp;
	int8_t f[1024], g[1024], F[1024], G[1024];
	union {
		int16_t sig[1024];
		uint16_t hm[1024];
	} r;
	unsigned char seed[SEEDLEN], nonce[NONCELEN];
	unsigned char esig[CRYPTO_BYTES - 2 - sizeof nonce];
	inner_shake256_context sc;
	size_t u, sig_len;
	union {
		fpr f[(10 + 5) * 1024];
		uint64_t u[(10 + 5) * 1024];
	} esk;

	/*
	 * Copy private key into the aligned stack buffer (handling
	 * endianness if necessary).
	 */
#if FALCON_LE
	(void)u;
	memcpy(esk.f, sk, sizeof esk.f);
#else
	for (u = 0; u < (sizeof esk.u) / sizeof esk.u[0]; u ++) {
		uint64_t w;

		w = (uint64_t)sk[(u << 3) + 0]
			| ((uint64_t)sk[(u << 3) + 1] << 8)
			| ((uint64_t)sk[(u << 3) + 2] << 16)
			| ((uint64_t)sk[(u << 3) + 3] << 24)
			| ((uint64_t)sk[(u << 3) + 4] << 32)
			| ((uint64_t)sk[(u << 3) + 5] << 40)
			| ((uint64_t)sk[(u << 3) + 6] << 48)
			| ((uint64_t)sk[(u << 3) + 7] << 56);
		esk.u[u] = w;
	}
#endif


	/*
	 * Create a random nonce (40 bytes).
	 */
	randombytes(nonce, sizeof nonce);

	/*
	 * Hash message nonce + message into a vector.
	 */
	inner_shake256_init(&sc);
	inner_shake256_inject(&sc, nonce, sizeof nonce);
	inner_shake256_inject(&sc, m, mlen);
	inner_shake256_flip(&sc);
	falcon1024tree_ref_hash_to_point_vartime(
		&sc, r.hm, 10);

	/*
	 * Initialize a RNG.
	 */
	randombytes(seed, sizeof seed);
	inner_shake256_init(&sc);
	inner_shake256_inject(&sc, seed, sizeof seed);
	inner_shake256_flip(&sc);

	/*
	 * Compute the signature.
	 */
	falcon1024tree_ref_sign_tree(
		r.sig, &sc, esk.f, r.hm, 10, tmp.b);


	/*
	 * Encode the signature and bundle it with the message. Format is:
	 *   signature length     2 bytes, big-endian
	 *   nonce                40 bytes
	 *   message              mlen bytes
	 *   signature            slen bytes
	 */
	esig[0] = 0x20 + 10;
	sig_len = falcon1024tree_ref_comp_encode(
		esig + 1, (sizeof esig) - 1, r.sig, 10);
	if (sig_len == 0) {
		return -1;
	}
	sig_len ++;
	memmove(sm + 2 + sizeof nonce, m, mlen);
	sm[0] = (unsigned char)(sig_len >> 8);
	sm[1] = (unsigned char)sig_len;
	memcpy(sm + 2, nonce, sizeof nonce);
	memcpy(sm + 2 + (sizeof nonce) + mlen, esig, sig_len);
	*smlen = 2 + (sizeof nonce) + mlen + sig_len;
	return 0;
}

int
crypto_sign_open(unsigned char *m, unsigned long long *mlen,
	const unsigned char *sm, unsigned long long smlen,
	const unsigned char *pk)
{
	union {
		uint8_t b[2 * 1024];
		uint64_t dummy_u64;
		fpr dummy_fpr;
	} tmp;
	const unsigned char *esig;
	uint16_t h[1024], hm[1024];
	int16_t sig[1024];
	inner_shake256_context sc;
	size_t sig_len, msg_len;

	/*
	 * Decode public key.
	 */
	if (pk[0] != 0x80 + 10) {
		return -1;
	}
	if (falcon1024tree_ref_modq_decode(
		h, 10, pk + 1, CRYPTO_PUBLICKEYBYTES - 1)
		!= CRYPTO_PUBLICKEYBYTES - 1)
	{
		return -1;
	}

	/*
	 * Find nonce, signature, message length.
	 */
	if (smlen < 2 + NONCELEN) {
		return -1;
	}
	sig_len = ((size_t)sm[0] << 8) | (size_t)sm[1];
	if (sig_len > (smlen - 2 - NONCELEN)) {
		return -1;
	}
	msg_len = smlen - 2 - NONCELEN - sig_len;

	/*
	 * Decode signature.
	 */
	esig = sm + 2 + NONCELEN + msg_len;
	if (sig_len < 1 || esig[0] != 0x20 + 10) {
		return -1;
	}
	if (falcon1024tree_ref_comp_decode(
		sig, 10, esig + 1, sig_len - 1) != sig_len - 1)
	{
		return -1;
	}

	/*
	 * Hash nonce + message into a vector.
	 */
	inner_shake256_init(&sc);
	inner_shake256_inject(&sc, sm + 2, NONCELEN + msg_len);
	inner_shake256_flip(&sc);
	falcon1024tree_ref_hash_to_point_vartime(
		&sc, hm, 10);

	/*
	 * Verify signature.
	 */
	if (!falcon1024tree_ref_verify_raw(
		hm, sig, h, 10, tmp.b))
	{
		return -1;
	}

	/*
	 * Return plaintext.
	 */
	memmove(m, sm + 2 + NONCELEN, msg_len);
	*mlen = msg_len;
	return 0;
}
