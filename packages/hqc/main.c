#include "pqclean/crypto_kem/hqc1024/clean/api.h"
#include "randombytes.h"


void hqcjs_init () {
	randombytes_stir();
}

long hqcjs_public_key_bytes () {
	return PQCLEAN_HQC1024_CLEAN_CRYPTO_PUBLICKEYBYTES;
}

long hqcjs_private_key_bytes () {
	return PQCLEAN_HQC1024_CLEAN_CRYPTO_SECRETKEYBYTES;
}

long hqcjs_cyphertext_bytes () {
	return PQCLEAN_HQC1024_CLEAN_CRYPTO_CIPHERTEXTBYTES;
}

long hqcjs_secret_bytes () {
	return PQCLEAN_HQC1024_CLEAN_CRYPTO_BYTES;
}

long hqcjs_keypair (
	uint8_t* public_key,
	uint8_t* private_key
) {
	return PQCLEAN_HQC1024_CLEAN_crypto_kem_keypair(public_key, private_key);
}

long hqcjs_encrypt (
	const uint8_t* public_key,
	uint8_t* cyphertext,
	uint8_t* secret
) {
	return PQCLEAN_HQC1024_CLEAN_crypto_kem_enc(
		cyphertext,
		secret,
		public_key
	);
}

long hqcjs_decrypt (
	const uint8_t* cyphertext,
	const uint8_t* private_key,
	uint8_t* secret
) {
	return PQCLEAN_HQC1024_CLEAN_crypto_kem_dec(
		secret,
		cyphertext,
		private_key
	);
}
