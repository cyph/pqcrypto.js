#include "api.h"
#include "crypto_encrypt.h"
#include "randombytes.h"


void kyberjs_init () {
	randombytes_stir();
}

long kyberjs_public_key_bytes () {
	return CRYPTO_PUBLICKEYBYTES;
}

long kyberjs_private_key_bytes () {
	return CRYPTO_SECRETKEYBYTES;
}

long kyberjs_encrypted_bytes () {
	return CYPHERTEXT_LEN;
}

long kyberjs_decrypted_bytes () {
	return CYPHERTEXT_LEN - CRYPTO_BYTES;
}

long kyberjs_keypair (
	uint8_t* public_key,
	uint8_t* private_key
) {
	return crypto_encrypt_keypair(public_key, private_key);
}

long kyberjs_encrypt (
	uint8_t* message,
	long message_len,
	uint8_t* public_key,
	uint8_t cyphertext[]
) {
	unsigned long long cyphertext_len;

	long status	= crypto_encrypt(
		cyphertext,
		&cyphertext_len,
		message,
		kyberjs_decrypted_bytes(),
		public_key
	);

	return status;
}

long kyberjs_decrypt (
	uint8_t* cyphertext,
	uint8_t* private_key,
	uint8_t* decrypted
) {
	unsigned long long decrypted_len;

	long status	= crypto_encrypt_open(
		decrypted,
		&decrypted_len,
		cyphertext,
		CYPHERTEXT_LEN,
		private_key
	);

	return status;
}
