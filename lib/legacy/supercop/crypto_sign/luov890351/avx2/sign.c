#include "LUOV.h"
#include "crypto_sign.h"

#ifdef KAT
	#define printIntermediateValue(A) printf(A)
#else
	#define printIntermediateValue(A) 
#endif

/*
	Generates a new keypair

	pk : char array that receives the new public key
	sk : char array that receives the new secret key
*/
int crypto_sign_keypair(unsigned char *pk, unsigned char *sk) 
{
	printIntermediateValue("--- Start keygen ---\n");
	
#ifdef PRECOMPUTE
	unsigned char *small_pk = aligned_alloc(32,PUBLIC_KEY_BYTES);
	generateBigKeyPair(small_pk , pk , sk);
	free(small_pk);
#else
	generateKeyPair(pk , sk);
#endif
	
	printIntermediateValue("--- End keygen ---\n");
	return 0;
}

/*
	Signs a document

	sm : char array that receives the signed message
	smlen : receives the length of the signed message
	m  : char array that contains the original message
	mlen : length of original message
	sk : char array containing the secret key
*/
int crypto_sign(unsigned char *sm, unsigned long long *smlen, const unsigned char *m, unsigned long long mlen, const unsigned char *sk)
{
	printIntermediateValue("--- Start signing ---\n");

	// If not the entire mesage can be recovered from a signature, we copy the first part to sm.
	if( mlen > RECOVERED_PART_MESSAGE ){
		memcpy(sm,m,mlen - RECOVERED_PART_MESSAGE);
		sm += mlen - RECOVERED_PART_MESSAGE;
	}
	// Produce a signature
#ifdef PRECOMPUTE
	sign_fast(sm, sk, m , mlen);
#else
	sign(sm , sk, m, mlen);
#endif

	*smlen = ((mlen > RECOVERED_PART_MESSAGE)? mlen - RECOVERED_PART_MESSAGE : 0) + CRYPTO_BYTES;

	printIntermediateValue("--- End signing ---\n");
	return 0;
}

/*
	Verify a signature

	m :  char array that receives the original message
	mlen : receives the length of the original message
	sm : char array that contains the signed message
	smlen : the length of the signed message
	pk : char array containing the public key

	returns : 0 if the signature is accepted, -1 otherwise
*/
int crypto_sign_open(unsigned char *m, unsigned long long *mlen, const unsigned char *sm, unsigned long long smlen, const unsigned char *pk)
{
	int valid;

	if(smlen<CRYPTO_BYTES){
	    return -1;
	}

	unsigned char *message = malloc(sizeof(unsigned char)*(smlen));
	
	printIntermediateValue("--- Start verifying ---\n");
	
	// Copy the part of the message that cannot be recovered from the signature into m
	memcpy(message,sm,smlen - CRYPTO_BYTES);
	sm += smlen - CRYPTO_BYTES;
	*mlen = smlen - CRYPTO_BYTES;
	
	// Verify signature
#ifdef PRECOMPUTE
	valid = verify_fast(pk, sm, message, mlen);
#else
	valid = verify(pk, sm, message, mlen);
#endif
	
	memcpy(m,message,*mlen);
	
	// Free up memory
	free(message);
	
	printIntermediateValue("--- End verifying ---\n");
	return valid;
}
