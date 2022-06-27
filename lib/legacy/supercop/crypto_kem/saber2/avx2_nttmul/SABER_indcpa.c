/*
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "api.h"
#include "SABER_indcpa.h"
#include "pack_unpack.h"
//#include "randombytes.h"
#include "rng.h"
#include "cbd.h"
#include "SABER_params.h"
#include "polyvec.h"
#include "fips202.h"
*/

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "SABER_params.h"
#include "SABER_indcpa.h"
#include "polyvec.h"
#include "pack_unpack.h"
#include "cbd.h"
#include "randombytes.h"
#include "fips202.h"


#define h1 4 			//2^(EQ-EP-1)

#define h2 ( (1<<(SABER_EP-2)) - (1<<(SABER_EP-SABER_ET-1)) + (1<<(SABER_EQ-SABER_EP-1)) )



void GenMatrix(polyvec *a, const unsigned char *seed){
  unsigned int one_vector=13*SABER_N/8;
  unsigned int byte_bank_length=SABER_K*SABER_K*one_vector;
  unsigned char buf[byte_bank_length];

  uint16_t temp_ar[SABER_N];

  int i,j,k;
  uint16_t mod = (SABER_Q-1);

  shake128(buf,byte_bank_length,seed,SABER_SEEDBYTES);
  
  for(i=0;i<SABER_K;i++){
    for(j=0;j<SABER_K;j++){
			BS2POLq(buf+(i*SABER_K+j)*one_vector,temp_ar);
			for(k=0;k<SABER_N;k++){
				a[i].vec[j].coeffs[k] = (temp_ar[k])& mod ;
			}
    }
  }
}


void GenSecret(polyvec *r,const unsigned char *seed){

	uint32_t i;

	int32_t buf_size= SABER_MU*SABER_N*SABER_K/8;

	uint8_t buf[buf_size];

	shake128(buf, buf_size, seed,SABER_NOISESEEDBYTES);

	for(i=0;i<SABER_K;i++){
		cbd(&r->vec[i].coeffs[0],buf+i*SABER_MU*SABER_N/8);
	}
}



void indcpa_kem_keypair(unsigned char *pk, unsigned char *sk)
{
 
  polyvec a[SABER_K];
  polyvec skpv2;

  unsigned char seed[SABER_SEEDBYTES];
  unsigned char noiseseed[SABER_COINBYTES];
  int32_t i,j;

	randombytes(seed, SABER_SEEDBYTES);
 
  shake128(seed, SABER_SEEDBYTES, seed, SABER_SEEDBYTES); // for not revealing system RNG state

  randombytes(noiseseed, SABER_COINBYTES);

	GenMatrix(a, seed); //sample matrix A

	GenSecret(&skpv2,noiseseed);

	polyvec u;

	polyvec_matrix_vector_mul(&u, a, &skpv2, 1);


	//------------------Pack sk into byte string-------
		
	POLVEC2BS(sk,&skpv2,SABER_Q);

	for (i = 0; i < SABER_K; i++)
		for (j = 0; j < SABER_N; j++)
			u.vec[i].coeffs[j] = (uint16_t) (u.vec[i].coeffs[j] + h1) >> (SABER_EQ - SABER_EP);

	
	POLVEC2BS(pk,&u,SABER_P); // load the public-key into pk byte string 	


	for(i=0;i<SABER_SEEDBYTES;i++) // now load the seedbytes in PK. Easy since seed bytes are kept in byte format.
		pk[SABER_POLYVECCOMPRESSEDBYTES + i]=seed[i]; 


}


void indcpa_kem_enc(unsigned char *message_received, unsigned char *noiseseed, const unsigned char *pk, unsigned char *ciphertext)
{ 

	uint32_t i,j;

	polyvec a[SABER_K];		// skpv;

	unsigned char seed[SABER_SEEDBYTES];

	polyvec skpv2;

	poly r;

	polyvec pkcl_vec;

	uint16_t temp[SABER_N];

	uint16_t message[SABER_KEYBYTES*8];

	nttpolyvec shat0, shat1;	// They store NTT of [s]

	polyvec u;
      
	for(i=0;i<SABER_SEEDBYTES;i++) // Load the seedbytes in the client seed from PK.
		seed[i]=pk[ SABER_POLYVECCOMPRESSEDBYTES + i]; 

	GenMatrix(a, seed);
				
	GenSecret(&skpv2,noiseseed);

	polyvec_matrix_vector_mul2(&u, a, &skpv2, &shat0, &shat1, 1, 0);	// Compute [A]*[s] and also store NTT(s) in {shat0,shat1}

	for (i = 0; i < SABER_K; i++)
		for (j = 0; j < SABER_N; j++)
			u.vec[i].coeffs[j] = (uint16_t) (u.vec[i].coeffs[j] + h1) >> (SABER_EQ - SABER_EP);


	POLVEC2BS(ciphertext, &u, SABER_P);		// Pack b_prime into ciphertext byte string

	BS2POLVEC(pk, &pkcl_vec, SABER_P);

	polyvec_iprod2(&r, &pkcl_vec, &skpv2, &shat0, &shat1, 0);


	// unpack message_received;
	for(j=0; j<SABER_KEYBYTES; j++)
		for(i=0; i<8; i++)
			message[8*j+i] = ((message_received[j]>>i) & 0x01);


	for (j = 0; j < SABER_N; j++){
		temp[j] = r.coeffs[j] & (SABER_P-1);
		temp[j] =  (temp[j] - (message[j] << (SABER_EP - 1)) + h1) >> (SABER_EP - SABER_ET);
	}

	POLT2BS(ciphertext + SABER_CIPHERTEXTBYTES, temp);
	
}


void indcpa_kem_dec(const unsigned char *sk, const unsigned char *ciphertext, unsigned char message_dec[])
{

	uint32_t i;
	uint16_t message_dec_unpacked[SABER_KEYBYTES*8];	// one element containes on decrypted bit;
	uint16_t op[SABER_N];

	poly message_poly;
	polyvec sksv_vec, pksv_vec;

	BS2POLVEC(sk, &sksv_vec, SABER_Q); //sksv is the secret-key
	BS2POLVEC(ciphertext, &pksv_vec, SABER_P); //pksv is the ciphertext

	polyvec_iprod(&message_poly, &pksv_vec, &sksv_vec);

	
	BS2POLT(ciphertext + SABER_CIPHERTEXTBYTES, op);

	for(i=0;i<SABER_N;i++)
		message_dec_unpacked[i]= ( ( message_poly.coeffs[i] + h2 - (op[i]<<(SABER_EP-SABER_ET)) ) & (SABER_P-1) ) >> (SABER_EP-1);


	POL2MSG(message_dec_unpacked, message_dec);
}


