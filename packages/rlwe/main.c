#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "LatticeCrypto_priv.h"
#include "LatticeCrypto.h"
#include "crypto_stream_chacha20.h"
#include "randombytes.h"
#include "utils.h"


PLatticeCryptoStruct lattice;

long public_key_bytes	= PKB_BYTES;
long private_key_length	= PARAMETER_N;


CRYPTO_STATUS rlwejs_randombytes (unsigned int nbytes, unsigned char* random_array) {
	randombytes_buf(random_array, nbytes);
	return CRYPTO_SUCCESS;
}

CRYPTO_STATUS rlwejs_streamoutput (
	const unsigned char* seed,
	unsigned int seed_nbytes,
	unsigned char* nonce,
	unsigned int nonce_nbytes,
	unsigned int array_nbytes,
	unsigned char* stream_array
) {
	unsigned char* k	= (unsigned char*) seed;
	if (seed_nbytes < crypto_stream_chacha20_KEYBYTES) {
		k	= calloc(crypto_stream_chacha20_KEYBYTES, 1);
		if (seed_nbytes > 0) {
			memcpy(k, seed, seed_nbytes);
		}
	}

	unsigned char* n	= nonce;
	if (nonce_nbytes < crypto_stream_chacha20_NONCEBYTES) {
		n	= calloc(crypto_stream_chacha20_NONCEBYTES, 1);
		if (nonce_nbytes > 0) {
			memcpy(n, nonce, nonce_nbytes);
		}
	}

	int status	= crypto_stream_chacha20(stream_array, array_nbytes, n, k);

	if (k != seed) {
		sodium_memzero(k, crypto_stream_chacha20_KEYBYTES);
		free(k);
	}

	if (n != nonce) {
		sodium_memzero(n, crypto_stream_chacha20_NONCEBYTES);
		free(n);
	}

	if (status != 0) {
		return CRYPTO_ERROR_UNKNOWN;
	}

	return CRYPTO_SUCCESS;
}

CRYPTO_STATUS rlwejs_extendableoutput (
	const unsigned char* seed,
	unsigned int seed_nbytes,
	unsigned int array_ndigits,
	uint32_t extended_array[]
) {
	CRYPTO_STATUS status	= rlwejs_streamoutput(
		seed,
		seed_nbytes,
		NULL,
		0,
		array_ndigits * 4,
		(unsigned char*) extended_array
	);

	if (status != CRYPTO_SUCCESS) {
		return status;
	}

	for (int i = 0 ; i < array_ndigits ; ++i) {
		extended_array[i]	= extended_array[i] % PARAMETER_Q;
	}

	return CRYPTO_SUCCESS;
}

CRYPTO_STATUS rlwejs_init () {
	randombytes_stir();

	lattice	= LatticeCrypto_allocate();

	return LatticeCrypto_initialize(
		lattice,
		rlwejs_randombytes,
		rlwejs_extendableoutput,
		rlwejs_streamoutput
	);
}

long rlwejs_public_key_bytes () {
	return public_key_bytes + 1;
}

long rlwejs_private_key_bytes () {
	return (private_key_length + 1) * 4;
}

long rlwejs_secret_bytes () {
	return SHAREDKEY_BYTES;
}

CRYPTO_STATUS rlwejs_keypair_alice (
	uint8_t public_key[],
	int32_t private_key[]
) {
	CRYPTO_STATUS status	= KeyGeneration_A(private_key, public_key, lattice);

	public_key[public_key_bytes]	= 1;
	private_key[private_key_length]	= 1;

	return status;
}

CRYPTO_STATUS rlwejs_secret_alice (
	uint8_t public_key[],
	int32_t private_key[],
	uint8_t* secret
) {
	if (public_key[public_key_bytes] || !private_key[private_key_length]) {
		return CRYPTO_ERROR_INVALID_PARAMETER;
	}

	return SecretAgreement_A(public_key, private_key, secret);
}

CRYPTO_STATUS rlwejs_secret_bob (
	uint8_t public_key_alice[],
	uint8_t public_key_bob[],
	uint8_t* secret
) {
	if (!public_key_alice[public_key_bytes]) {
		return CRYPTO_ERROR_INVALID_PARAMETER;
	}

	CRYPTO_STATUS status	= SecretAgreement_B(public_key_alice, secret, public_key_bob, lattice);

	public_key_bob[public_key_bytes]	= 0;

	return status;
}
