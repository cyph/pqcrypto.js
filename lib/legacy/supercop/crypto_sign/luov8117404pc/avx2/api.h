#ifndef API_H
#define API_H

#include "parameters.h"

#ifdef PRECOMPUTE
	/* Number of bytes it takes to encode the precomputed secret key */
	#define CRYPTO_SECRETKEYBYTES BIG_SECRET_KEY_BYTES    
	/* Number of bytes it takes to encode the precomputed public key */ 
	#define CRYPTO_PUBLICKEYBYTES BIG_PUBLIC_KEY_BYTES                   
#else
	/* Number of bytes it takes to encode the secret key */
	#define CRYPTO_SECRETKEYBYTES SECRET_KEY_BYTES    
	/* Number of bytes it takes to encode the public key */ 
	#define CRYPTO_PUBLICKEYBYTES PUBLIC_KEY_BYTES                         
#endif


/* Number of bytes it takes to encode a signature */
#define CRYPTO_BYTES ( VARS )        

#define CRYPTO_ALGNAME "LUOV"

int crypto_sign_keypair(unsigned char *pk, unsigned char *sk);
int crypto_sign(unsigned char *sm, unsigned long long *smlen, const unsigned char *m, unsigned long long mlen, const unsigned char *sk);
int crypto_sign_open(unsigned char *m, unsigned long long *mlen, const unsigned char *sm, unsigned long long smlen, const unsigned char *pk);

#endif 
