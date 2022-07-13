#include "pqclean/crypto_kem/mceliece8192128/clean/api.h"
#include "randombytes.h"


void mceliecejs_init () {
	randombytes_stir();
}

long mceliecejs_public_key_bytes () {
	return PQCLEAN_MCELIECE8192128_CLEAN_CRYPTO_PUBLICKEYBYTES;
}

long mceliecejs_private_key_bytes () {
	return PQCLEAN_MCELIECE8192128_CLEAN_CRYPTO_SECRETKEYBYTES;
}

long mceliecejs_cyphertext_bytes () {
	return PQCLEAN_MCELIECE8192128_CLEAN_CRYPTO_CIPHERTEXTBYTES;
}

long mceliecejs_secret_bytes () {
	return PQCLEAN_MCELIECE8192128_CLEAN_CRYPTO_BYTES;
}

long mceliecejs_keypair (
	uint8_t* public_key,
	uint8_t* private_key
) {
	return PQCLEAN_MCELIECE8192128_CLEAN_crypto_kem_keypair(public_key, private_key);
}

long mceliecejs_encrypt (
	const uint8_t* public_key,
	uint8_t* cyphertext,
	uint8_t* secret
) {
	return PQCLEAN_MCELIECE8192128_CLEAN_crypto_kem_enc(
		cyphertext,
		secret,
		public_key
	);
}

long mceliecejs_decrypt (
	const uint8_t* cyphertext,
	const uint8_t* private_key,
	uint8_t* secret
) {
	return PQCLEAN_MCELIECE8192128_CLEAN_crypto_kem_dec(
		secret,
		cyphertext,
		private_key
	);
}
