#include <string.h>
#include "crypto_hash.h"
#include "komihash.h"

int crypto_hash(
	unsigned char *out,
	const unsigned char *in,
	unsigned long long inlen )
{
	uint64_t r = komihash( in, inlen, 0 );
	memcpy(out,&r,sizeof r);

	return 0;
}
