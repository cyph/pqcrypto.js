/// @file utils_hash.c
/// @brief the adapter for SHA2 families in openssl.
///
///

#include "utils_hash.h"

#include "openssl/sha.h"


#include "hash_len_config.h"

#ifndef _HASH_LEN
#define _HASH_LEN (32)
#endif


#include <rng.h>
// the macro _SUPERCOP_ might be defined in rng.h
#if defined(_SUPERCOP_)
#if 32 == _HASH_LEN
#include "crypto_hash_sha256.h"
#elif 48 == _HASH_LEN
#include "crypto_hash_sha384.h"
#elif 64 == _HASH_LEN
#include "crypto_hash_sha512.h"
#else
error: supercop hash
#endif

#endif



static inline
int _hash( unsigned char * digest , const unsigned char * m , unsigned long long mlen )
{
#if 32 == _HASH_LEN
#if defined(_SUPERCOP_)
	crypto_hash_sha256(digest,m,mlen);
#else
	SHA256_CTX sha256;
	SHA256_Init( &sha256 );
	SHA256_Update( &sha256 , m , mlen );
	SHA256_Final( digest , &sha256 );
#endif
#elif 48 == _HASH_LEN
#if defined(_SUPERCOP_)
	crypto_hash_sha384(digest,m,mlen);
#else
	SHA512_CTX sha384;
	SHA384_Init( &sha384 );
	SHA384_Update( &sha384 , m , mlen );
	SHA384_Final( digest , &sha384 );
#endif
#elif 64 == _HASH_LEN
#if defined(_SUPERCOP_)
	crypto_hash_sha512(digest,m,mlen);
#else
	SHA512_CTX sha512;
	SHA512_Init( &sha512 );
	SHA512_Update( &sha512 , m , mlen );
	SHA512_Final( digest , &sha512 );
#endif
#else
error: un-supported _HASH_LEN
#endif
	return 0;
}





static inline
int expand_hash( unsigned char * digest , unsigned n_digest , const unsigned char * hash )
{
	if( _HASH_LEN >= n_digest ) {
		for(unsigned i=0;i<n_digest;i++) digest[i] = hash[i];
		return 0;
	} else {
		for(unsigned i=0;i<_HASH_LEN;i++) digest[i] = hash[i];
		n_digest -= _HASH_LEN;
	}

	while( _HASH_LEN <= n_digest ) {
		_hash( digest+_HASH_LEN , digest , _HASH_LEN );

		n_digest -= _HASH_LEN;
		digest += _HASH_LEN;
	}
	unsigned char temp[_HASH_LEN];
	if( n_digest ){
		_hash( temp , digest , _HASH_LEN );
		for(unsigned i=0;i<n_digest;i++) digest[_HASH_LEN+i] = temp[i];
	}
	return 0;
}




int hash_msg( unsigned char * digest , unsigned len_digest , const unsigned char * m , unsigned long long mlen )
{
	unsigned char buf[_HASH_LEN];
	_hash( buf , m , mlen );

	return expand_hash( digest , len_digest , buf );
}


