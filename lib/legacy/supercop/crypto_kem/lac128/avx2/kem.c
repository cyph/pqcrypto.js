#include <string.h>
#include "api.h"
#include "rand.h"
#include "bch.h"
#include "ecc.h"
#include "crypto_kem.h"

//generate keypair
int crypto_kem_keypair(unsigned char *pk, unsigned char *sk)
{
	//call the key generation algorithm of pke
	crypto_encrypt_keypair(pk, sk);
	return 0;
}
int crypto_kem_enc( unsigned char *ct, unsigned char *ss, const unsigned char *pk)
{
	kem_enc_fo(pk,ss,ct);
	return 0;
}
int crypto_kem_dec( unsigned char *ss, const unsigned char *ct, const unsigned char *sk)
{
	const unsigned char *pk=sk+DIM_N;
	kem_dec_fo(pk,(const char *)sk,ct,ss);
	return 0;
}
// fo encryption for cca security 
int kem_enc_fo(const unsigned char *pk, unsigned char *k, unsigned char *c)
{
	unsigned char buf[MESSAGE_LEN+CIPHER_LEN],seed[SEED_LEN];
	
	//check parameter
	if(pk==NULL || k==NULL || c==NULL)
	{
		return -1;
	}
	
	//generate random message m, stored in buf
	random_bytes(buf,MESSAGE_LEN);
	//compute seed=gen_seed(m)
	gen_seed(buf,MESSAGE_LEN,seed);
	//encrypt m with seed
	pke_enc_seed(pk,buf,MESSAGE_LEN,c,seed);
	
	//compute k=hash(m|c)
	memcpy(buf+MESSAGE_LEN,c,CIPHER_LEN);
	hash(buf,MESSAGE_LEN+CIPHER_LEN,k);
	
	return 0;
}

// fo encryption for cca security with seed
int kem_enc_fo_seed(const unsigned char *pk, unsigned char *k, unsigned char *c, unsigned char *seed)
{
	unsigned char buf[MESSAGE_LEN+CIPHER_LEN],local_seed[SEED_LEN];
	
	//check parameter
	if(pk==NULL || k==NULL || c==NULL)
	{
		return -1;
	}
	
	//generate random message m from seed, stored in buf
	pseudo_random_bytes(buf,MESSAGE_LEN,seed);
	//compute loacal_seed=gen_seed(m)
	gen_seed(buf,MESSAGE_LEN,local_seed);
	//encrypt m with local_seed
	pke_enc_seed(pk,buf,MESSAGE_LEN,c,local_seed);
	
	//compute k=hash(m|c)
	memcpy(buf+MESSAGE_LEN,c,CIPHER_LEN);
	hash(buf,MESSAGE_LEN+CIPHER_LEN,k);
	
	return 0;
}

// decrypt of fo mode
int kem_dec_fo(const unsigned char *pk, const char *sk, const unsigned char *c, unsigned char *k)
{
	unsigned char buf[MESSAGE_LEN+CIPHER_LEN],seed[SEED_LEN];
	unsigned char c_v[CIPHER_LEN];
        unsigned char k2[CRYPTO_BYTES];
	unsigned char mlen;
	
	//check parameter
	if(pk==NULL || sk==NULL || k==NULL || c==NULL)
	{
		return -1;
	}
	
	//compute m from c
	pke_dec(sk,c,buf,&mlen);
	//compte k=hash(m|c)
	memcpy(buf+MESSAGE_LEN,c,CIPHER_LEN);
	hash(buf,MESSAGE_LEN+CIPHER_LEN,k2);
	//re-encryption with seed=gen_seed(m)
	gen_seed(buf,MESSAGE_LEN,seed);
	pke_enc_seed(pk,buf,MESSAGE_LEN,c_v,seed);
	
	//verify 
	if(memcmp(c,c_v,CIPHER_LEN)!=0)
	{
		//k=hash(hash(sk)|c)
		hash((unsigned char*)sk,DIM_N,buf);
		hash(buf,MESSAGE_LEN+CIPHER_LEN,k2);
	}
        memcpy(k,k2,sizeof k2);
	
	return 0;
}

