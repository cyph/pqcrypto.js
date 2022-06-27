/*
Implementation by Ronny Van Keer, hereby denoted as "the implementer".

For more information, feedback or questions, please refer to our website:
https://keccak.team/

To the extent possible under law, the implementer has waived all copyright
and related or neighboring rights to the source code in this file.
http://creativecommons.org/publicdomain/zero/1.0/
*/

#include "crypto_aead.h"
#include "api.h"
#include "Xoodyak.h"
#include <string.h>

#if	!defined(CRYPTO_KEYBYTES)
	#define CRYPTO_KEYBYTES     16
#endif
#if	!defined(CRYPTO_NPUBBYTES)
	#define CRYPTO_NPUBBYTES    16
#endif

#define	TAGLEN		16

int crypto_aead_encrypt(
	unsigned char *c, unsigned long long *clen,
	const unsigned char *m, unsigned long long mlen,
	const unsigned char *ad, unsigned long long adlen,
	const unsigned char *nsec,
	const unsigned char *npub,
	const unsigned char *k)
{
	Xoodyak	instance(BitString(k, 8 * CRYPTO_KEYBYTES), BitString(), BitString());

	(void)nsec;

	instance.Absorb(BitString(npub, 8 * CRYPTO_NPUBBYTES));
	instance.Absorb(BitString(ad, 8 * (size_t)adlen));
	BitString cryptString = instance.Encrypt(BitString(m, 8 * (size_t)mlen));
    if (cryptString.size() != 0) std::copy(cryptString.array(), cryptString.array() + (cryptString.size() + 7) / 8, c);
	BitString tagString = instance.Squeeze(TAGLEN);
    if (tagString.size() != 0) std::copy(tagString.array(), tagString.array() + (tagString.size() + 7) / 8, c + mlen);
    *clen = mlen + TAGLEN;
	#if 0
	{
		unsigned int i;
		for (i = 0; i < *clen; ++i )
		{
			printf("\\x%02x", c[i] );
		}
		printf("\n");
	}
	#endif
	return 0;
}

int crypto_aead_decrypt(
	unsigned char *m, unsigned long long *mlen,
	unsigned char *nsec,
	const unsigned char *c, unsigned long long clen,
	const unsigned char *ad, unsigned long long adlen,
	const unsigned char *npub,
	const unsigned char *k)
{
	Xoodyak	instance(BitString(k, 8 * CRYPTO_KEYBYTES), BitString(), BitString());
	unsigned char		tag[TAGLEN];
	unsigned long long	mlen_;

	(void)nsec;

	*mlen = 0;
    if (clen < TAGLEN) {
        return -1;
	}
	mlen_ = clen - TAGLEN;
	instance.Absorb(BitString(npub, 8 * CRYPTO_NPUBBYTES));
	instance.Absorb(BitString(ad, 8 * (size_t)adlen));
	BitString decryptString = instance.Decrypt(BitString(c, 8 * (size_t)mlen_));
	if (decryptString.size() != 0) std::copy(decryptString.array(), decryptString.array() + (decryptString.size() + 7) / 8, m);
	BitString tagString = instance.Squeeze(TAGLEN);
    if (tagString.size() != 0) std::copy(tagString.array(), tagString.array() + (tagString.size() + 7) / 8, tag);
	if (memcmp(tag, c + mlen_, TAGLEN) != 0) {
		memset(m, 0, (size_t)mlen_);
		return -1;
	}
	*mlen = mlen_;
	return 0;
}
