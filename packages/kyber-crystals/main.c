#include "pqclean/crypto_kem/kyber1024/clean/indcpa.h"
#include "randombytes.h"


void kyberjs_init () {
	randombytes_stir();
}

long kyberjs_public_key_bytes () {
	return KYBER_INDCPA_PUBLICKEYBYTES;
}

long kyberjs_private_key_bytes () {
	return KYBER_INDCPA_SECRETKEYBYTES;
}

long kyberjs_encrypted_bytes () {
	return KYBER_INDCPA_BYTES;
}

long kyberjs_decrypted_bytes () {
	return KYBER_INDCPA_MSGBYTES;
}

long kyberjs_keypair (
	uint8_t* public_key,
	uint8_t* private_key
) {
	PQCLEAN_KYBER1024_CLEAN_indcpa_keypair(public_key, private_key);
	return 0;
}

long kyberjs_encrypt (
	const uint8_t* message,
	long message_len,
	const uint8_t* public_key,
	uint8_t* cyphertext
) {
	uint8_t coins[KYBER_SYMBYTES];
	randombytes_buf(coins, KYBER_SYMBYTES);

	PQCLEAN_KYBER1024_CLEAN_indcpa_enc(
		cyphertext,
		message,
		public_key,
		coins
	);

	return 0;
}

long kyberjs_decrypt (
	const uint8_t* cyphertext,
	const uint8_t* private_key,
	uint8_t* decrypted
) {
	PQCLEAN_KYBER1024_CLEAN_indcpa_dec(
		decrypted,
		cyphertext,
		private_key
	);

	return 0;
}
