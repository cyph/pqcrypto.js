#include "pqclean/crypto_sign/falcon-1024/clean/api.h"
#include "randombytes.h"


void falconjs_init () {
	randombytes_stir();
}

long falconjs_public_key_bytes () {
	return PQCLEAN_FALCON1024_CLEAN_CRYPTO_PUBLICKEYBYTES;
}

long falconjs_secret_key_bytes () {
	return PQCLEAN_FALCON1024_CLEAN_CRYPTO_SECRETKEYBYTES;
}

long falconjs_signature_bytes () {
	return PQCLEAN_FALCON1024_CLEAN_CRYPTO_BYTES + sizeof(unsigned short);
}

long falconjs_keypair (
	uint8_t* public_key,
	uint8_t* private_key
) {
	return PQCLEAN_FALCON1024_CLEAN_crypto_sign_keypair(
		public_key,
		private_key
	);
}

long falconjs_sign (
	uint8_t *sig,
	const uint8_t *m,
	unsigned long mlen,
	const uint8_t *sk
) {
	size_t siglen;

	int status	= PQCLEAN_FALCON1024_CLEAN_crypto_sign_signature(
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

long falconjs_verify (
	const uint8_t *m,
	unsigned long mlen,
	const uint8_t *sig,
	const uint8_t *pk
) {
	size_t siglen	= (size_t) *((unsigned short*) sig);

	return PQCLEAN_FALCON1024_CLEAN_crypto_sign_verify(
		sig + sizeof(unsigned short),
		siglen,
		m,
		mlen,
		pk
	);
}
