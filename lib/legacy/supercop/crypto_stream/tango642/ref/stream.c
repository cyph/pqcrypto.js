#include <string.h>
#include "crypto_stream.h"
#include "tango642.h"

int crypto_stream(
	unsigned char *c, unsigned long long clen,
	const unsigned char *n,
	const unsigned char *k
)
{
	TANGO642_CTX ctx;
	tango642_init( &ctx, k, 32, n, 8 );

	memset( c, 0, clen );
	tango642_xor( &ctx, c, clen );
	tango642_final( &ctx );

	return 0;
}
