#include "P751_api.h"
#include "randombytes.h"


void sidhjs_init () {
	randombytes_stir();
}

long sidhjs_public_key_bytes () {
	return CRYPTO_PUBLICKEYBYTES;
}

long sidhjs_private_key_bytes () {
	return CRYPTO_SECRETKEYBYTES;
}

long sidhjs_cyphertext_bytes () {
	return CRYPTO_CIPHERTEXTBYTES;
}

long sidhjs_secret_bytes () {
	return CRYPTO_BYTES;
}

long sidhjs_keypair (
	uint8_t* public_key,
	uint8_t* private_key
) {
	return crypto_kem_keypair_SIKEp751(public_key, private_key);
}

long sidhjs_encrypt (
	const uint8_t* public_key,
	uint8_t* cyphertext,
	uint8_t* secret
) {
	return crypto_kem_enc_SIKEp751(
		cyphertext,
		secret,
		public_key
	);
}

long sidhjs_decrypt (
	const uint8_t* cyphertext,
	const uint8_t* private_key,
	uint8_t* secret
) {
	return crypto_kem_dec_SIKEp751(
		secret,
		cyphertext,
		private_key
	);
}
