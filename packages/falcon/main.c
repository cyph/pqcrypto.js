#include "crypto_sign/falcon256/ref/api.h"
#include "randombytes.h"


int crypto_sign_falcon_keypair (unsigned char *pk, unsigned char *sk);

int crypto_sign_falcon (
	unsigned char *sm,
	unsigned long long *smlen,
	const unsigned char *m,
	unsigned long long mlen,
	const unsigned char *sk
);

int crypto_sign_falcon_open(
	unsigned char *m,
	unsigned long long *mlen,
	const unsigned char *sm,
	unsigned long long smlen,
	const unsigned char *pk
);


void falconjs_init () {
	randombytes_stir();
}

long falconjs_public_key_bytes () {
	return CRYPTO_PUBLICKEYBYTES;
}

long falconjs_secret_key_bytes () {
	return CRYPTO_SECRETKEYBYTES;
}

long falconjs_signature_bytes () {
	return CRYPTO_BYTES;
}

long falconjs_keypair (
	uint8_t* public_key,
	uint8_t* private_key
) {
	return crypto_sign_falcon_keypair(public_key, private_key);
}

long falconjs_open (
	uint8_t *m,
	unsigned long long *mlen,
	const uint8_t *sm,
	unsigned long smlen,
	const uint8_t *pk
) {
	return crypto_sign_falcon_open(m, mlen, sm, smlen, pk);
}

long falconjs_sign (
	uint8_t *sm,
	unsigned long long *smlen,
	const uint8_t *m,
	unsigned long mlen,
	const uint8_t *sk
) {
	return crypto_sign_falcon(sm, smlen, m, mlen, sk);
}
