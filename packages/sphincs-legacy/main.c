#include "crypto_sign/sphincs256/ref/api.h"
#include "randombytes.h"


int crypto_sign_sphincs_keypair (unsigned char *pk, unsigned char *sk);

int crypto_sign_sphincs (
	unsigned char *sm,
	unsigned long long *smlen,
	const unsigned char *m,
	unsigned long long mlen,
	const unsigned char *sk
);

int crypto_sign_sphincs_open(
	unsigned char *m,
	unsigned long long *mlen,
	const unsigned char *sm,
	unsigned long long smlen,
	const unsigned char *pk
);


void sphincsjs_init () {
	randombytes_stir();
}

long sphincsjs_public_key_bytes () {
	return CRYPTO_PUBLICKEYBYTES;
}

long sphincsjs_secret_key_bytes () {
	return CRYPTO_SECRETKEYBYTES;
}

long sphincsjs_signature_bytes () {
	return CRYPTO_BYTES;
}

long sphincsjs_keypair (
	uint8_t* public_key,
	uint8_t* private_key
) {
	return crypto_sign_sphincs_keypair(public_key, private_key);
}

long sphincsjs_open (
	uint8_t *m,
	unsigned long long *mlen,
	const uint8_t *sm,
	unsigned long smlen,
	const uint8_t *pk
) {
	return crypto_sign_sphincs_open(m, mlen, sm, smlen, pk);
}

long sphincsjs_sign (
	uint8_t *sm,
	unsigned long long *smlen,
	const uint8_t *m,
	unsigned long mlen,
	const uint8_t *sk
) {
	return crypto_sign_sphincs(sm, smlen, m, mlen, sk);
}
