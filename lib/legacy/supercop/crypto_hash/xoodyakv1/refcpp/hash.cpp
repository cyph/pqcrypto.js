/*
Implementation by Ronny Van Keer, hereby denoted as "the implementer".

For more information, feedback or questions, please refer to our website:
https://keccak.team/

To the extent possible under law, the implementer has waived all copyright
and related or neighboring rights to the source code in this file.
http://creativecommons.org/publicdomain/zero/1.0/
*/

#include "crypto_hash.h"

#ifndef crypto_hash_BYTES
    #define crypto_hash_BYTES 32
#endif

#include "Xoodyak.h"

int crypto_hash(unsigned char *out, const unsigned char *in, unsigned long long inlen)
{
	Xoodyak	instance = Xoodyak(BitString(), BitString(), BitString());

    instance.Absorb(BitString(in, 8 * (size_t)inlen));
    BitString outString = instance.Squeeze(crypto_hash_BYTES);
    if (outString.size() != 0) std::copy(outString.array(), outString.array() + (outString.size() + 7) / 8, out);
	#if 0
	{
		unsigned int i;
		for (i = 0; i < crypto_hash_BYTES; ++i )
		{
			printf("\\x%02x", out[i] );
		}
		printf("\n");
	}
	#endif
	return 0;
}
