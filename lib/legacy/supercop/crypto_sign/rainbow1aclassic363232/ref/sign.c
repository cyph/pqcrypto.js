///  @file  sign.c
///  @brief the implementations for functions in api.h
///
///
#include <stdlib.h>
#include <string.h>

#include "rainbow_config.h"
#include "rainbow_keypair.h"
#include "rainbow.h"

#include "api.h"

#include "utils_hash.h"

#include <rng.h>
// the macro _SUPERCOP_ might be defined in rng.h
#if defined(_SUPERCOP_)
#include "crypto_sign.h"
#endif


int
crypto_sign_keypair(unsigned char *pk, unsigned char *sk)
{
    unsigned char sk_seed[LEN_SKSEED] = {0};
    randombytes( sk_seed , LEN_SKSEED );

#if defined _RAINBOW_CLASSIC

    int r = generate_keypair( (pk_t*) pk , (sk_t*) sk , sk_seed );

#elif defined _RAINBOW_CYCLIC

    unsigned char pk_seed[LEN_PKSEED] = {0};
    randombytes( pk_seed , LEN_PKSEED );
    int r = generate_keypair_cyclic( (cpk_t*) pk , (sk_t*) sk , pk_seed , sk_seed );

    for(int i=0;i<LEN_PKSEED;i++) pk_seed[i]=0;
#elif defined _RAINBOW_CYCLIC_COMPRESSED

    unsigned char pk_seed[LEN_PKSEED] = {0};
    randombytes( pk_seed , LEN_PKSEED );
    int r = generate_compact_keypair_cyclic( (cpk_t*) pk , (csk_t*) sk , pk_seed , sk_seed );

    for(int i=0;i<LEN_PKSEED;i++) pk_seed[i]=0;
#else
error here
#endif
    for(int i=0;i<LEN_SKSEED;i++) sk_seed[i]=0;
    return r;
}





int
crypto_sign(unsigned char *sm, unsigned long long *smlen, const unsigned char *m, unsigned long long mlen, const unsigned char *sk)
{
	unsigned char digest[_HASH_LEN];

	hash_msg( digest , _HASH_LEN , m , mlen );

	int r = -1;
#if defined _RAINBOW_CLASSIC

	r = rainbow_sign( sm + mlen , (const sk_t*)sk , digest );

#elif defined _RAINBOW_CYCLIC

	r = rainbow_sign( sm + mlen , (const sk_t*)sk , digest );

#elif defined _RAINBOW_CYCLIC_COMPRESSED

	r = rainbow_sign_cyclic( sm + mlen , (const csk_t*)sk , digest );

#else
error here
#endif
	memcpy( sm , m , mlen );
	smlen[0] = mlen + _SIGNATURE_BYTE;

	return r;
}






int
crypto_sign_open(unsigned char *m, unsigned long long *mlen,const unsigned char *sm, unsigned long long smlen,const unsigned char *pk)
{
	if( _SIGNATURE_BYTE > smlen ) return -1;

	unsigned char digest[_HASH_LEN];
	hash_msg( digest , _HASH_LEN , sm , smlen-_SIGNATURE_BYTE );

	int r = -1;

#if defined _RAINBOW_CLASSIC

	r = rainbow_verify( digest , sm + smlen-_SIGNATURE_BYTE , (const pk_t *)pk );

#elif defined _RAINBOW_CYCLIC

	r = rainbow_verify_cyclic( digest , sm + smlen-_SIGNATURE_BYTE , (const cpk_t *)pk );

#elif defined _RAINBOW_CYCLIC_COMPRESSED

	r = rainbow_verify_cyclic( digest , sm + smlen-_SIGNATURE_BYTE , (const cpk_t *)pk );

#else
error here
#endif

	memcpy( m , sm , smlen-_SIGNATURE_BYTE );
	mlen[0] = smlen-_SIGNATURE_BYTE;

	return r;
}

