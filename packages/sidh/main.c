#include "P503_api.h"
#include "sodium.h"


void sidhjs_init () {
	randombytes_stir();
}

long sidhjs_public_key_bytes () {
	return CRYPTO_PUBLICKEYBYTES;
}

long sidhjs_private_key_bytes () {
	return CRYPTO_SECRETKEYBYTES;
}

long sidhjs_encrypted_bytes () {
	return CYPHERTEXT_LEN + CRYPTO_CIPHERTEXTBYTES;
}

long sidhjs_decrypted_bytes () {
	return CYPHERTEXT_LEN - crypto_aead_chacha20poly1305_IETF_ABYTES;
}

long sidhjs_keypair (
	uint8_t* public_key,
	uint8_t* private_key
) {
	return crypto_kem_keypair_SIKEp503(public_key, private_key);
}

long sidhjs_encrypt (
	uint8_t* message,
	long message_len,
	uint8_t* public_key,
	uint8_t* cyphertext
) {
	uint8_t sike_secret[CRYPTO_BYTES];

	long status	= crypto_kem_enc_SIKEp503(cyphertext, sike_secret, public_key);

	if (status != 0) {
		return status;
	}

	uint8_t key[crypto_aead_chacha20poly1305_KEYBYTES];

	status	= crypto_generichash(
		key,
		crypto_aead_chacha20poly1305_KEYBYTES,
		sike_secret,
		CRYPTO_BYTES,
		NULL,
		0
	);

	sodium_memzero(sike_secret, CRYPTO_BYTES);

	if (status != 0) {
		return status;
	}

	unsigned long long cyphertext_len;
	uint8_t nonce[crypto_aead_chacha20poly1305_IETF_NPUBBYTES] = {0};

	status	= crypto_aead_chacha20poly1305_ietf_encrypt(
		cyphertext + CRYPTO_CIPHERTEXTBYTES,
		&cyphertext_len,
		message,
		sidhjs_decrypted_bytes(),
		cyphertext,
		CRYPTO_CIPHERTEXTBYTES,
		NULL,
		nonce,
		key
	);

	sodium_memzero(key, CRYPTO_BYTES);

	if (status != 0) {
		sodium_memzero(cyphertext, sidhjs_encrypted_bytes());
	}

	return status;
}

long sidhjs_decrypt (
	uint8_t* cyphertext,
	uint8_t* private_key,
	uint8_t* decrypted
) {
	uint8_t sike_secret[CRYPTO_BYTES];

	long status	= crypto_kem_dec_SIKEp503(sike_secret, cyphertext, private_key);

	if (status != 0) {
		return status;
	}

	uint8_t key[crypto_aead_chacha20poly1305_KEYBYTES];

	status	= crypto_generichash(
		key,
		crypto_aead_chacha20poly1305_KEYBYTES,
		sike_secret,
		CRYPTO_BYTES,
		NULL,
		0
	);

	sodium_memzero(sike_secret, CRYPTO_BYTES);

	if (status != 0) {
		return status;
	}

	unsigned long long decrypted_len;
	uint8_t nonce[crypto_aead_chacha20poly1305_IETF_NPUBBYTES] = {0};

	status	= crypto_aead_chacha20poly1305_ietf_decrypt(
		decrypted,
		&decrypted_len,
		NULL,
		cyphertext + CRYPTO_CIPHERTEXTBYTES,
		CYPHERTEXT_LEN,
		cyphertext,
		CRYPTO_CIPHERTEXTBYTES,
		nonce,
		key
	);

	sodium_memzero(key, CRYPTO_BYTES);

	if (status != 0) {
		sodium_memzero(decrypted, sidhjs_decrypted_bytes());
	}

	return status;
}
