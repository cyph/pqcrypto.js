#include <string.h>
#include "crypto_stream.h"
#include "tango642.h"

int crypto_stream_xor(
	unsigned char *c,
	const unsigned char *m, unsigned long long mlen,
	const unsigned char *n,
	const unsigned char *k
)
{
	TANGO642_CTX ctx;
	tango642_init( &ctx, k, 32, n, 8 );

	if( (void*) c != (void*) m )
	{
		memcpy( c, m, mlen );
	}

	tango642_xor( &ctx, c, mlen );
	tango642_final( &ctx );

	return 0;
}
