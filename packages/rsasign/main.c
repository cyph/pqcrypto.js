#include "openssl/asn1.h"
#include "openssl/asn1t.h"
#include "openssl/bn.h"
#include "openssl/rand.h"
#include "openssl/rsa.h"
#include "openssl/sha.h"
#include "openssl/x509.h"
#include "randombytes.h"


void rsasignjs_init () {
	randombytes_stir();
}

long rsasignjs_public_key_bytes () {
	return RSASIGNJS_PUBLEN;
}

long rsasignjs_secret_key_bytes () {
	return RSASIGNJS_PRIVLEN;
}

long rsasignjs_signature_bytes () {
	return RSASIGNJS_SIGLEN;
}

long rsasignjs_keypair (
	uint8_t* public_key,
	uint8_t* private_key
) {
	BIGNUM* prime	= BN_new();
	RSA* rsa		= RSA_new();

	BN_add_word(prime, RSA_F4);

	if (RSA_generate_key_ex(rsa, RSASIGNJS_BITS, prime, NULL) != 1) {
		return 1;
	}
	
	i2d_RSA_PUBKEY(rsa, &public_key);
	i2d_RSAPrivateKey(rsa, &private_key);

	RSA_free(rsa);
	BN_free(prime);

	return 0;
}

long rsasignjs_sign (
	uint8_t* signature,
	uint8_t* message,
	long message_len,
	const uint8_t* private_key,
	long private_key_len
) {
	RSA* rsa	= RSA_new();

	if (d2i_RSAPrivateKey(&rsa, &private_key, private_key_len) == NULL) {
		return -1;
	}

	uint8_t hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	SHA256_Update(&sha256, message, message_len);
	SHA256_Final(hash, &sha256);

	long status	= RSA_sign(
		NID_sha256,
		hash,
		SHA256_DIGEST_LENGTH,
		signature,
		NULL,
		rsa
	);

	RSA_free(rsa);

	return status;
}

long rsasignjs_verify (
	uint8_t* signature,
	uint8_t* message,
	long message_len,
	const uint8_t* public_key,
	long public_key_len
) {
	RSA* rsa	= RSA_new();

	if (d2i_RSAPublicKey(&rsa, &public_key, public_key_len) == NULL) {
		return -1;
	}

	uint8_t hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	SHA256_Update(&sha256, message, message_len);
	SHA256_Final(hash, &sha256);

	long status	= RSA_verify(
		NID_sha256,
		hash,
		SHA256_DIGEST_LENGTH,
		signature,
		RSASIGNJS_SIGLEN,
		rsa
	);

	RSA_free(rsa);

	return status;
}


void RAND_add (const void *buf, int num, double entropy) {
	randombytes_stir();
}

int RAND_bytes (unsigned char *buf, int num) {
	randombytes_buf(buf, num);
	return 1;
}

int RAND_pseudo_bytes (unsigned char *buf, int num) {
	return RAND_bytes(buf, num);
}

void RAND_seed (const void *buf, int num) {
	randombytes_stir();
}

int RAND_status () {
	return 1;
}

void rand_cleanup_int () {}
