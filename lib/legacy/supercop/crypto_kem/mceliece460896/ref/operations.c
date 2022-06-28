#include "operations.h"

#include "aes256ctr.h"
#include "controlbits.h"
#include "randombytes.h"
#include "crypto_hash.h"
#include "encrypt.h"
#include "decrypt.h"
#include "params.h"
#include "sk_gen.h"
#include "pk_gen.h"
#include "util.h"

#include <stdint.h>
#include <string.h>

int crypto_kem_enc(
       unsigned char *c,
       unsigned char *key,
       const unsigned char *pk
)
{
	unsigned char two_e[ 1 + SYS_N/8 ] = {2};
	unsigned char *e = two_e + 1;
	unsigned char one_ec[ 1 + SYS_N/8 + (SYND_BYTES + 32) ] = {1};

	//

	encrypt(c, pk, e);

	crypto_hash_32b(c + SYND_BYTES, two_e, sizeof(two_e)); 

	memcpy(one_ec + 1, e, SYS_N/8);
	memcpy(one_ec + 1 + SYS_N/8, c, SYND_BYTES + 32);

	crypto_hash_32b(key, one_ec, sizeof(one_ec));

	return 0;
}

int crypto_kem_dec(
       unsigned char *key,
       const unsigned char *c,
       const unsigned char *sk
)
{
	int i;

	unsigned char ret_confirm = 0;
	unsigned char ret_decrypt = 0;

	uint16_t m;

	unsigned char conf[32];
	unsigned char two_e[ 1 + SYS_N/8 ] = {2};
	unsigned char *e = two_e + 1;
	unsigned char preimage[ 1 + SYS_N/8 + (SYND_BYTES + 32) ];
	unsigned char *x = preimage;

	//

	ret_decrypt = decrypt(e, sk + SYS_N/8, c);

	crypto_hash_32b(conf, two_e, sizeof(two_e)); 

	for (i = 0; i < 32; i++) ret_confirm |= conf[i] ^ c[SYND_BYTES + i];

	m = ret_decrypt | ret_confirm;
	m -= 1;
	m >>= 8;

	                                      *x++ = (~m &     0) | (m &    1);
	for (i = 0; i < SYS_N/8;         i++) *x++ = (~m & sk[i]) | (m & e[i]);
	for (i = 0; i < SYND_BYTES + 32; i++) *x++ = c[i];

	crypto_hash_32b(key, preimage, sizeof(preimage)); 

	return 0;
}

int crypto_kem_keypair
(
       unsigned char *pk,
       unsigned char *sk 
)
{
	int i;
	unsigned char seed[ 32 ];
	unsigned char r[ SYS_T*2 + (1 << GFBITS)*sizeof(uint32_t) + SYS_N/8 + 32 ];
	unsigned char nonce[ 16 ] = {0};
	unsigned char *rp;

	gf f[ SYS_T ]; // element in GF(2^mt)
	gf irr[ SYS_T ]; // Goppa polynomial
	uint32_t perm[ 1 << GFBITS ]; // random permutation 

	randombytes(seed, sizeof(seed));

	while (1)
	{
		rp = r;
		aes256ctr(r, sizeof(r), nonce, seed);
		memcpy(seed, &r[ sizeof(r)-32 ], 32);

		for (i = 0; i < SYS_T; i++) f[i] = load2(rp + i*2); rp += sizeof(f);
		if (genpoly_gen(irr, f)) continue;

		for (i = 0; i < (1 << GFBITS); i++) perm[i] = load4(rp + i*4); rp += sizeof(perm);
		if (perm_check(perm)) continue;

		for (i = 0; i < SYS_T;   i++) store2(sk + SYS_N/8 + i*2, irr[i]);
		if (pk_gen(pk, sk + SYS_N/8, perm)) continue;

		memcpy(sk, rp, SYS_N/8);
		controlbits(sk + SYS_N/8 + IRR_BYTES, perm);

		break;
	}

	return 0;
}

