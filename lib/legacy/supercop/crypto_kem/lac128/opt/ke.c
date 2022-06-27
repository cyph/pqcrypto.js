#include "api.h"
#include "rand.h"
#include <string.h>

//Alice send: generate pk and sk, and send pk to Bob
int crypto_ke_alice_send(unsigned char *pk,unsigned char *sk)
{
	//check pointer
	if(pk==NULL || sk==NULL)
	{
		return  -1;
	}
	//call key generation algorithm to get pk and sk
	kg(pk, (char *)sk);
	
	return 0;
}
// Bob receive: receive  pk, randomly choose m, and encryrpt m with pk to generate c, k=HASH(pk,m).
int crypto_ke_bob_receive(unsigned char *pk, unsigned char *c, unsigned char *k)
{
	unsigned char temp_key[MESSAGE_LEN];
	unsigned char in[MESSAGE_LEN+DIM_N+SEED_LEN];
	//check pointer
	if(pk==NULL || c==NULL || k==NULL)
	{
		return  -1;
	}
	
	//call cpa secure kem to generate k2 
	random_bytes(temp_key,MESSAGE_LEN);
	pke_enc(pk,temp_key,MESSAGE_LEN,c);
	
	//call hash to compute  k=hash(pk,m)
	memcpy(in,pk,DIM_N+SEED_LEN);
	memcpy(in+DIM_N,temp_key,MESSAGE_LEN);
	hash(in,MESSAGE_LEN+DIM_N,k);
	
	return 0;
}
//Alice receive: receive c, and decrypt to get m and comute k=HASH(pk,m)
int crypto_ke_alice_receive(unsigned char *pk, unsigned char *sk, unsigned char *c, unsigned char *k)
{
	unsigned char temp_key[MESSAGE_LEN];
	unsigned char in[MESSAGE_LEN+DIM_N+SEED_LEN];
	unsigned char mlen;
	//check pointer
	if(pk==NULL || sk==NULL || c==NULL || k==NULL)
	{
		return  -1;
	}
	//decrypt c of cca kem to get k2
	pke_dec((char *)sk,c,temp_key,&mlen);
	
	//call hash to compute  k=hash(pk,m)
	memcpy(in,pk,DIM_N+SEED_LEN);
	memcpy(in+DIM_N,temp_key,MESSAGE_LEN);
	hash(in,MESSAGE_LEN+DIM_N,k);
	
	return 0;
}