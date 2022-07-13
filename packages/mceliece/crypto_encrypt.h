int crypto_encrypt(
	unsigned char *c,
	unsigned long long *clen,
	const unsigned char *m,
	unsigned long long mlen,
	const unsigned char *pk
);

int crypto_encrypt_keypair(
	unsigned char *pk,
	unsigned char *sk
);

int crypto_encrypt_open(
	unsigned char *m,
	unsigned long long *mlen,
	const unsigned char *c,
	unsigned long long clen,
	const unsigned char *sk
);
