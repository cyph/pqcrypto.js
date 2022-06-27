#include "rand.h"
#include "rng.h"
#include "lac_param.h"
#include <string.h>
#include <openssl/rand.h>
#include <openssl/aes.h>
#include <openssl/sha.h>
#include <openssl/crypto.h>
#include <openssl/evp.h>
//random bytes
int random_bytes(unsigned char *r, unsigned int len)
{
	//check parameter
	if(r==NULL)
	{
		return 1;
	}
	// call the random function 
//	RAND_bytes(r,len);
	randombytes(r,len);
	return 0;
}

//pseudo-random bytes
int pseudo_random_bytes(unsigned char *r, unsigned int len, const unsigned char *seed)
{
	int c_len;
	unsigned char data[AES_BLOCK_SIZE],c[AES_BLOCK_SIZE];
//	unsigned int *p=(unsigned int *)data;
	int i,loop=len/AES_BLOCK_SIZE;
	//check  parameter
	if(r==NULL || seed==NULL)
	{
		return 1;
	}
	EVP_CIPHER_CTX *ctx;
	ctx = EVP_CIPHER_CTX_new();
	EVP_EncryptInit_ex(ctx, EVP_aes_256_ctr(), NULL, seed, NULL);
	memset(data,0,AES_BLOCK_SIZE);
	for(i=0;i<loop;i++)
	{
	//	*p=i;//set counter
		EVP_EncryptUpdate(ctx, r+i*AES_BLOCK_SIZE, &c_len, data, AES_BLOCK_SIZE);
	}
	//check tail
	if(len%AES_BLOCK_SIZE>0)
	{
	//	*p=loop;
		EVP_EncryptUpdate(ctx, c, &c_len, data, AES_BLOCK_SIZE);
	}
	memcpy(r+loop-1,c,len%AES_BLOCK_SIZE);
	EVP_CIPHER_CTX_free(ctx);
	
	return 0;
}

//hash
int hash(const unsigned char *in, unsigned int len_in, unsigned char * out)
{
	//check  parameter
	if(in==NULL || out==NULL)
	{
		return 1;
	}
	
	#if defined LAC128
	SHA256(in,len_in,out);
	#endif
	
	#if defined LAC192
	SHA384(in,len_in,out);
	#endif

	#if defined LAC256
	SHA512(in,len_in,out);
	#endif

	
	return 0;
}

//generate seed
int gen_seed(unsigned char *in, unsigned int len_in, unsigned char * out)
{
	//check  parameter
	if(in==NULL || out==NULL)
	{
		return 1;
	}
	
	SHA256(in,len_in,out);
	return 0;
}

