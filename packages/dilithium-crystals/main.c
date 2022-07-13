#include "pqclean/crypto_sign/dilithium5/clean/api.h"
#include "randombytes.h"


void dilithiumjs_init () {
	randombytes_stir();
}

long dilithiumjs_public_key_bytes () {
	return PQCLEAN_DILITHIUM5_CLEAN_CRYPTO_PUBLICKEYBYTES;
}

long dilithiumjs_secret_key_bytes () {
	return PQCLEAN_DILITHIUM5_CLEAN_CRYPTO_SECRETKEYBYTES;
}

long dilithiumjs_signature_bytes () {
	return PQCLEAN_DILITHIUM5_CLEAN_CRYPTO_BYTES + sizeof(unsigned short);
}

long dilithiumjs_keypair (
	uint8_t* public_key,
	uint8_t* private_key
) {
	return PQCLEAN_DILITHIUM5_CLEAN_crypto_sign_keypair(
		public_key,
		private_key
	);
}

long dilithiumjs_sign (
	uint8_t *sig,
	const uint8_t *m,
	unsigned long mlen,
	const uint8_t *sk
) {
	size_t siglen;

	int status	= PQCLEAN_DILITHIUM5_CLEAN_crypto_sign_signature(
		sig + sizeof(unsigned short),
		&siglen,
		m,
		mlen,
		sk
	);

	if (status == 0) {
		*((unsigned short*) sig)	= (unsigned short) siglen;
	}

	return status;
}

long dilithiumjs_verify (
	const uint8_t *m,
	unsigned long mlen,
	const uint8_t *sig,
	const uint8_t *pk
) {
	size_t siglen	= (size_t) *((unsigned short*) sig);

	return PQCLEAN_DILITHIUM5_CLEAN_crypto_sign_verify(
		sig + sizeof(unsigned short),
		siglen,
		m,
		mlen,
		pk
	);
}
