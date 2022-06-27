#include <string.h>
#include "crypto_hash.h"
#include "prvhash64s.h"

int crypto_hash(
	unsigned char *out,
	const unsigned char *in,
	unsigned long long inlen )
{
	prvhash64s_oneshot( in, inlen, out, 64 );

	return 0;
}
