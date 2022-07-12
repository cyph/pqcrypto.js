#include "pqclean/crypto_kem/kyber1024/clean/api.h"
#include "randombytes.h"


void kyberjs_init () {
	randombytes_stir();
}

long kyberjs_public_key_bytes () {
	return PQCLEAN_KYBER1024_CLEAN_CRYPTO_PUBLICKEYBYTES;
}

long kyberjs_private_key_bytes () {
	return PQCLEAN_KYBER1024_CLEAN_CRYPTO_SECRETKEYBYTES;
}

long kyberjs_cyphertext_bytes () {
	return PQCLEAN_KYBER1024_CLEAN_CRYPTO_CIPHERTEXTBYTES;
}

long kyberjs_secret_bytes () {
	return PQCLEAN_KYBER1024_CLEAN_CRYPTO_BYTES;
}

long kyberjs_keypair (
	uint8_t* public_key,
	uint8_t* private_key
) {
	return PQCLEAN_KYBER1024_CLEAN_crypto_kem_keypair(public_key, private_key);
}

long kyberjs_encrypt (
	const uint8_t* public_key,
	uint8_t* cyphertext,
	uint8_t* secret
) {
	return PQCLEAN_KYBER1024_CLEAN_crypto_kem_enc(
		cyphertext,
		secret,
		public_key
	);
}

long kyberjs_decrypt (
	const uint8_t* cyphertext,
	const uint8_t* private_key,
	uint8_t* secret
) {
	return PQCLEAN_KYBER1024_CLEAN_crypto_kem_dec(
		secret,
		cyphertext,
		private_key
	);
}
