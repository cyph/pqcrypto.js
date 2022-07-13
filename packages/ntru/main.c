#include "pqclean/crypto_kem/sntrup1277/clean/api.h"
#include "randombytes.h"


void ntrujs_init () {
	randombytes_stir();
}

long ntrujs_public_key_bytes () {
	return PQCLEAN_SNTRUP1277_CLEAN_CRYPTO_PUBLICKEYBYTES;
}

long ntrujs_private_key_bytes () {
	return PQCLEAN_SNTRUP1277_CLEAN_CRYPTO_SECRETKEYBYTES;
}

long ntrujs_cyphertext_bytes () {
	return PQCLEAN_SNTRUP1277_CLEAN_CRYPTO_CIPHERTEXTBYTES;
}

long ntrujs_secret_bytes () {
	return PQCLEAN_SNTRUP1277_CLEAN_CRYPTO_BYTES;
}

long ntrujs_keypair (
	uint8_t* public_key,
	uint8_t* private_key
) {
	return PQCLEAN_SNTRUP1277_CLEAN_crypto_kem_keypair(public_key, private_key);
}

long ntrujs_encrypt (
	const uint8_t* public_key,
	uint8_t* cyphertext,
	uint8_t* secret
) {
	return PQCLEAN_SNTRUP1277_CLEAN_crypto_kem_enc(
		cyphertext,
		secret,
		public_key
	);
}

long ntrujs_decrypt (
	const uint8_t* cyphertext,
	const uint8_t* private_key,
	uint8_t* secret
) {
	return PQCLEAN_SNTRUP1277_CLEAN_crypto_kem_dec(
		secret,
		cyphertext,
		private_key
	);
}
