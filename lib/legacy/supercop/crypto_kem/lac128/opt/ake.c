#include "api.h"
#include "rand.h"
#include <string.h>

//Alice send: generate pk and sk, and send pk and cca kem ciphertext of pk_b to Bob
int crypto_ake_alice_send(unsigned char *pk,unsigned char *sk, unsigned char *pk_b, unsigned char *sk_a, unsigned char *c, unsigned char *k1)
{
	unsigned char seed[SEED_LEN],buf[CRYPTO_SECRETKEYBYTES+SEED_LEN];
	//check pointer
	if(pk==NULL || sk==NULL || pk_b==NULL || k1==NULL)
	{
		return  -1;
	}
	//call key generation algorithm to get pk and sk
	kg(pk,(char*)sk);
	// compute seed=hash(random_seed|sk_a)
	random_bytes(buf,SEED_LEN);
	memcpy(buf+SEED_LEN,sk_a,CRYPTO_SECRETKEYBYTES);
	gen_seed(buf,CRYPTO_SECRETKEYBYTES+SEED_LEN,seed);
	// call cca secure kem with seed to generate k1
	kem_enc_fo_seed(pk_b,k1,c,seed);
	
	return 0;
}
// Bob receive: receive  pk, randomly choose m, and encryrpt m with pk to generate c, k1. k=HASH(pk_a,pk_b,pk,c3,k1,k2,k3)
int crypto_ake_bob_receive(unsigned char *pk_b, unsigned char *sk_b, unsigned char *pk_a, unsigned char *pk, unsigned char *c_in,unsigned char *c_out , unsigned char *k)
{
	unsigned char k1[MESSAGE_LEN],k2[MESSAGE_LEN],k3[MESSAGE_LEN];
	unsigned char in[3*MESSAGE_LEN+3*PK_LEN+CIPHER_LEN],seed[SEED_LEN];
	unsigned char buf[CRYPTO_SECRETKEYBYTES+SEED_LEN];
	//check pointer
	if(pk_b==NULL || sk_b==NULL|| pk_a==NULL || pk==NULL|| c_in==NULL || c_out==NULL || k==NULL)
	{
		return  -1;
	}
	
	// compute seed=hash(random_seed|sk_b)
	random_bytes(buf,SEED_LEN);
	memcpy(buf+SEED_LEN,sk_b,CRYPTO_SECRETKEYBYTES);
	gen_seed(buf,CRYPTO_SECRETKEYBYTES+SEED_LEN,seed);
	//call cca secure kem to generate k2 
	kem_enc_fo_seed(pk_a,k2,c_out,seed);
	
	//call cpa kem algorithm to generate k3
	random_bytes(k3,MESSAGE_LEN);
	pke_enc(pk,k3,MESSAGE_LEN,c_out+CIPHER_LEN);
	
	//decrypt c_in to get k1
	kem_dec_fo(pk_b,(char  *)sk_b,c_in,k1);
	
	//compy pk_a,pk_b,pk to buf
	memcpy(in,pk_a,PK_LEN);
	memcpy(in+PK_LEN,pk_b,PK_LEN);
	memcpy(in+2*PK_LEN,pk,PK_LEN);
	
	//copy c3 to to buffer
	memcpy(in+3*PK_LEN,c_out+CIPHER_LEN,CIPHER_LEN);
	//copy k1,k2,k3 to buf
	memcpy(in+3*PK_LEN+CIPHER_LEN,k1,MESSAGE_LEN);
	memcpy(in+3*PK_LEN+CIPHER_LEN+MESSAGE_LEN,k2,MESSAGE_LEN);
	memcpy(in+3*PK_LEN+CIPHER_LEN+2*MESSAGE_LEN,k3,MESSAGE_LEN);
	// compute session key k=HASH(pk_a,pk_b,pk,c3,k1,k2,k3)
	hash(in,3*MESSAGE_LEN+3*PK_LEN+CIPHER_LEN,k);
	
	return 0;
}
//Alice receive: receive c, and decrypt to get k2, k3 and comute k=HASH(pk_a,pk_b,pk,c3,k1,k2,k3)
int crypto_ake_alice_receive(unsigned char *pk_a, unsigned char *sk_a, unsigned char *pk_b, unsigned char *pk, unsigned char *sk, unsigned char *c1, unsigned char *c_in, unsigned char *k1, unsigned char *k)
{
	unsigned char k2[MESSAGE_LEN],k3[MESSAGE_LEN];
	unsigned char in[3*MESSAGE_LEN+3*PK_LEN+CIPHER_LEN];
	unsigned char mlen;
	//check pointer
	if(pk_a==NULL || sk_a==NULL|| pk==NULL || sk==NULL || c_in==NULL || k1==NULL || k==NULL)
	{
		return  -1;
	}
	//decrypt c of cca kem to get k2
	kem_dec_fo(pk_a,(char *)sk_a,c_in,k2);
	
	//decrypt c of cpa pke to get k3
	pke_dec((char *)sk,c_in+CIPHER_LEN,k3,&mlen);
	
	//copy pk_a,pk_b,pk to buf
	memcpy(in,pk_a,PK_LEN);
	memcpy(in+PK_LEN,pk_b,PK_LEN);
	memcpy(in+2*PK_LEN,pk,PK_LEN);
	//copy c3 to buf
	memcpy(in+3*PK_LEN,c_in+CIPHER_LEN,CIPHER_LEN);
	// copy k1,k2,k3 to buf
	memcpy(in+3*PK_LEN+CIPHER_LEN,k1,MESSAGE_LEN);
	memcpy(in+3*PK_LEN+CIPHER_LEN+MESSAGE_LEN,k2,MESSAGE_LEN);
	memcpy(in+3*PK_LEN+CIPHER_LEN+2*MESSAGE_LEN,k3,MESSAGE_LEN);
	// compute session key k=HASH(pk_a,pk_b,pk,c3,k1,k2,k3)
	hash(in,3*MESSAGE_LEN+3*PK_LEN+CIPHER_LEN,k);
	
	return 0;
}