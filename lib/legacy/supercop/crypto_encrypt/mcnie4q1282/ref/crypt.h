#ifndef crypt_h
#define crypt_h

#include "matrix.h"
#include "bin_vec.h"

#define MESSAGE_BYTES  264
#define CIPHER_BYTES   505

/* Low rank parity check matrix */
struct LRP
{
	gf *basis;      /* basis */
	Bvec *bin;      /* coefficient */
	Gmat M;         /* matrix representation */
};

gf *syndrome_decode(gf *syn, struct LRP *H, short r);


int crypto_encrypt_keypair(unsigned char *pk, unsigned char *sk);
int crypto_encrypt(
	unsigned char *c, unsigned long long *clen,
	const unsigned char *m, const unsigned long long mlen,
	const unsigned char *pk);
int crypto_encrypt_open(
	unsigned char *m, unsigned long long *mlen,
	const unsigned char *c, unsigned long long clen,
	const unsigned char *sk);

#endif
