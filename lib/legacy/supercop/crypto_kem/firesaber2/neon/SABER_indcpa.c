/*=============================================================================
This file has been adapted from the implementation 
(available at, Public Domain https://github.com/KULeuven-COSIC/SABER) 
of "Saber: Module-LWR based key exchange, CPA-secure encryption and CCA-secure KEM"
by : Jan-Pieter D'Anvers, Angshuman Karmakar, Sujoy Sinha Roy, and Frederik Vercauteren
Jose Maria Bermudo Mera, Michiel Van Beirendonck, Andrea Basso. 

 * Copyright (c) 2020 by Cryptographic Engineering Research Group (CERG)
 * ECE Department, George Mason University
 * Fairfax, VA, U.S.A.
 * Author: Duc Tri Nguyen

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

=============================================================================*/


#include <arm_neon.h>

#include "pack_unpack.h"
#include "randombytes.h"
#include "cbd.h"
#include "SABER_params.h"
#include "fips202.h"
#include "polymul/toom_cook_4/asimd_toom_cook_4way_neon.c"


#define h1 4 //2^(EQ-EP-1)

#define h2 ( (1<<(SABER_EP-2)) - (1<<(SABER_EP-SABER_ET-1)) + (1<<(SABER_EQ-SABER_EP-1)) )

// c = a + b
#define vadd1(c, a, b) \
	c.val[0] = vaddq_u16(a.val[0], b); \
	c.val[1] = vaddq_u16(a.val[1], b);

// c = a & b
#define vand(c, a, b) \
	c.val[0] = vandq_u16(a.val[0], b); \
	c.val[1] = vandq_u16(a.val[1], b);

// c = 0 
#define vzero(c, zero) \
	vxor(zero, zero, zero); \
	vstore(c, zero);


void POL2MSG(uint16_t *message_dec_unpacked, unsigned char *message_dec){

	int32_t i,j;

	for(j=0; j<SABER_KEYBYTES; j++)
	{
		message_dec[j] = 0;
		for(i=0; i<8; i++)
		message_dec[j] = message_dec[j] | (message_dec_unpacked[j*8 + i] <<i);
	} 

}

/*-----------------------------------------------------------------------------------
	This routine generates a=[Matrix K x K] of 256-coefficient polynomials 
-------------------------------------------------------------------------------------*/



void BS2POLq(const unsigned char *bytes, uint16_t data[SABER_N]){
	
	uint32_t j;
	uint32_t offset_data=0,offset_byte=0;	
	
	offset_byte=0;

		for(j=0;j<SABER_N/8;j++){
			offset_byte=13*j;
			offset_data=8*j;
			data[offset_data + 0]= ( bytes[ offset_byte + 0 ] & (0xff)) | ((bytes[offset_byte + 1] & 0x1f)<<8);
			data[offset_data + 1]= ( bytes[ offset_byte + 1 ]>>5 & (0x07)) | ((bytes[offset_byte + 2] & 0xff)<<3) | ((bytes[offset_byte + 3] & 0x03)<<11);
			data[offset_data + 2]= ( bytes[ offset_byte + 3 ]>>2 & (0x3f)) | ((bytes[offset_byte + 4] & 0x7f)<<6);
			data[offset_data + 3]= ( bytes[ offset_byte + 4 ]>>7 & (0x01)) | ((bytes[offset_byte + 5] & 0xff)<<1) | ((bytes[offset_byte + 6] & 0x0f)<<9);
			data[offset_data + 4]= ( bytes[ offset_byte + 6 ]>>4 & (0x0f)) | ((bytes[offset_byte + 7] & 0xff)<<4) | ((bytes[offset_byte + 8] & 0x01)<<12);
			data[offset_data + 5]= ( bytes[ offset_byte + 8]>>1 & (0x7f)) | ((bytes[offset_byte + 9] & 0x3f)<<7);
			data[offset_data + 6]= ( bytes[ offset_byte + 9]>>6 & (0x03)) | ((bytes[offset_byte + 10] & 0xff)<<2) | ((bytes[offset_byte + 11] & 0x07)<<10);
			data[offset_data + 7]= ( bytes[ offset_byte + 11]>>3 & (0x1f)) | ((bytes[offset_byte + 12] & 0xff)<<5);
		}


}


void GenMatrix(polyvec *a, const unsigned char *seed) 
{
  unsigned int one_vector=13*SABER_N/8;
  unsigned int byte_bank_length=SABER_K*SABER_K*one_vector;
  unsigned char buf[byte_bank_length];

  uint16_t temp_ar[SABER_N];

  int i,j,k;
  uint16_t mod = (SABER_Q-1);

  shake128(buf,byte_bank_length,seed,SABER_SEEDBYTES);
  
  for(i=0;i<SABER_K;i++)
  {
    for(j=0;j<SABER_K;j++)
    {
	BS2POLq(buf+(i*SABER_K+j)*one_vector,temp_ar);
	for(k=0;k<SABER_N;k++){
		a[i].vec[j].coeffs[k] = (temp_ar[k])& mod ;
	}
    }
  }
}

void GenSecret(uint16_t r[SABER_K][SABER_N],const unsigned char *seed){


		uint32_t i;

		int32_t buf_size= SABER_MU*SABER_N*SABER_K/8;

		uint8_t buf[buf_size];

		shake128(buf, buf_size, seed,SABER_NOISESEEDBYTES);

		for(i=0;i<SABER_K;i++)
		{
			cbd(r[i],buf+i*SABER_MU*SABER_N/8);
		}
}


void printArray(uint16_t *M, char *string, size_t length)
{
	for (size_t i = 0; i < length; i++)
	{
		printf("%d, ", M[i]);
	}
	printf("\n");
}

void indcpa_kem_keypair(unsigned char *pk, unsigned char *sk)
{
 
  polyvec a[SABER_K];

  uint16_t skpv1[SABER_K][SABER_N];

  
 
  unsigned char seed[SABER_SEEDBYTES];
  unsigned char noiseseed[SABER_COINBYTES];
  int32_t i,j,k;


//--------------NEON declaration------------------

  uint16x8_t mod   =vdupq_n_u16(SABER_Q-1);
  uint16x8_t H1_avx=vdupq_n_u16(h1);
  uint16x8x2_t acc_neon;
  uint16x8x2_t res_neon;

//--------------NEON declaration ends------------------
  uint16_t res_avx[SABER_K][SABER_N] = {0};
  uint16_t acc[SABER_N];

  randombytes(seed, SABER_SEEDBYTES);

  shake128(seed, SABER_SEEDBYTES, seed, SABER_SEEDBYTES); // for not revealing system RNG state
  randombytes(noiseseed, SABER_COINBYTES);
  
  GenMatrix(a, seed); //sample matrix A
  
  GenSecret(skpv1,noiseseed);

//   printf("skvp1:\n");
//   for (i = 0; i < SABER_K; i++)
//   {
// 	  printArray(skpv1[i], "skvp1", SABER_N);
//   }

  //------------------------do the matrix vector multiplication and rounding------------

	// Matrix-vector multiplication; Matrix in transposed order
	for(i=0;i<SABER_K;i++){
		for(j=0;j<SABER_K;j++){
			toom_cook_4way_neon(a[j].vec[i].coeffs, skpv1[j], SABER_Q, acc);

			for(k=0;k<SABER_N/16;k++){
				vload(res_neon, &res_avx[i][k*16]);
				vload(acc_neon, &acc[k*16]);
				// res_avx[i][k] += acc[k]
				vadd(res_neon, res_neon, acc_neon);
				// res_avx[i][k] &= mod 
				vand(res_neon, res_neon, mod);
				// acc[k] = 0
				// vzero(&acc[k*16], acc_neon); // No Need
				vstore(&res_avx[i][k*16], res_neon);
			}
		}
	}
	
	// Now truncation
	for(i=0;i<SABER_K;i++){ //shift right EQ-EP bits
		for(j=0;j<SABER_N/16;j++){
			vload(res_neon, &res_avx[i][16*j]);
			// res_avx[i][j] += H1_avx
			vadd1(res_neon, res_neon, H1_avx);
			// res_avx[i][j] >>= (SABER_EQ-SABER_EP)
			vsr(res_neon, res_neon, (SABER_EQ-SABER_EP));
			// res_avx[i][j] &= mod
			vand(res_neon, res_neon, mod);
			vstore(&res_avx[i][16*j], res_neon);
		}
	}

	//------------------Pack sk into byte string-------
		
	POLVEC2BS(sk,skpv1,SABER_Q);

	//------------------Pack pk into byte string-------
	POLVEC2BS(pk,res_avx,SABER_P); // load the public-key into pk byte string 	

	for(i=0;i<SABER_SEEDBYTES;i++){ // now load the seedbytes in PK. Easy since seed bytes are kept in byte format.
		pk[SABER_POLYVECCOMPRESSEDBYTES + i]=seed[i]; 
	}

}



void indcpa_kem_enc(unsigned char *message_received, 
					unsigned char *noiseseed, 
					const unsigned char *pk, 
					unsigned char *ciphertext)
{ 

	uint32_t i,j,k;
	polyvec a[SABER_K];		// skpv;
	unsigned char seed[SABER_SEEDBYTES];
	uint16_t pkcl[SABER_K][SABER_N]; 	//public key of received by the client


	uint16_t skpv1[SABER_K][SABER_N];
	uint16_t temp[SABER_K][SABER_N];
	uint16_t message[SABER_KEYBYTES*8];

	unsigned char msk_c[SABER_SCALEBYTES_KEM];
	//--------------NEON declaration------------------
	  uint16x8_t mod   = vdupq_n_u16 (SABER_Q-1);
	  uint16x8_t mod_p = vdupq_n_u16 (SABER_P-1);
	  uint16x8_t H1_avx= vdupq_n_u16(h1);
	  
	  uint16x8x2_t acc_neon;
	  uint16x8x2_t res_neon;
 
	//--------------NEON declaration ends------------------
	uint16_t acc[SABER_N];
	uint16_t res_avx[SABER_K][SABER_N] = {0}; 
	uint16_t vprime_avx[SABER_N] = {0};
      
	for(i=0;i<SABER_SEEDBYTES;i++){ // Load the seedbytes in the client seed from PK.
		seed[i]=pk[ SABER_POLYVECCOMPRESSEDBYTES + i]; 
	}

	GenMatrix(a, seed);				

	GenSecret(skpv1,noiseseed);

	//-------unpack the public_key
	BS2POLVEC(pk, pkcl, SABER_P);


	//-----------------matrix-vector multiplication and rounding

	// Matrix-vector multiplication; 
	for(i=0;i<SABER_K;i++){
		for(j=0;j<SABER_K;j++){

			// toom_cook_4way_avx(a_avx[i][j], sk_avx[j], SABER_Q, acc);
			toom_cook_4way_neon(a[i].vec[j].coeffs, skpv1[j], SABER_Q, acc);

			for(k=0;k<SABER_N/16;k++){
				vload(res_neon, &res_avx[i][k*16]);
				vload(acc_neon, &acc[k*16]);
				// res_avx[i][k] += acc[k]
				vadd(res_neon, res_neon, acc_neon);
				// res_avx[i][k] &= mod 
				vand(res_neon, res_neon, mod);
				// acc[k] = 0 
				// vzero(&acc[k*16], acc_neon); // No need
				vstore(&res_avx[i][k*16], res_neon);
				
			}
			
		}
	}
	
	// Now truncation

	for(i=0;i<SABER_K;i++){ //shift right EQ-EP bits
		for(j=0;j<SABER_N/16;j++){
			vload(res_neon, &res_avx[i][j*16]);
			// res_avx[i][j] += H1_avx
			vadd1(res_neon, res_neon, H1_avx);
			// res_avx[i][j] >>= (SABER_EQ-SABER_EP)
			vsr(res_neon, res_neon, (SABER_EQ-SABER_EP));
			// res_avx[i][j] &= mod 
			vand(res_neon, res_neon, mod);
			
			//-----this result should be put in b_prime for later use in server.
			vstore(&temp[i][j*16], res_neon);
		}
	}
	
	POLVEC2BS(ciphertext,temp, SABER_P); // Pack b_prime into ciphertext byte string

//**************client matrix-vector multiplication ends******************//

	//------now calculate the v'

	// vector-vector scalar multiplication with mod p
	
	
	for(j=0;j<SABER_K;j++){
		// toom_cook_4way_avx(pkcl_avx[j], sk_avx[j], SABER_P, acc);
		toom_cook_4way_neon(pkcl[j], skpv1[j], SABER_P, acc);

		for(k=0;k<SABER_N/16;k++){
			vload(acc_neon, &acc[k*16]);
			vload(res_neon, &vprime_avx[k*16]);
			// vprime_avx[k] += acc[k]
			vadd(res_neon, res_neon, acc_neon);
			// vprime_avx[k] &= mod_p
			vand(res_neon, res_neon, mod_p);
			// acc[k] = 0
			// vzero(&acc[k*16], acc_neon); // No need
			vstore(&vprime_avx[k*16], res_neon);
		}
	}
	
	// unpack message_received;
	for(j=0; j<SABER_KEYBYTES; j++)
	{
		for(i=0; i<8; i++)
		{
			message[8*j+i] = ((message_received[j]>>i) & 0x01);
		}
	}
	
	// message encoding
	for(k=0; k<SABER_N/16; k++)
	{
		vload(acc_neon, &message[k*16]);
		vsl(acc_neon, acc_neon, SABER_EP-1);
		
		// Computation of v'+h1 
		vload(res_neon, &vprime_avx[k*16]);
		vadd1(res_neon, res_neon, H1_avx); //adding h1

		// SHIFTRIGHT(v'+h1-m mod p, EP-ET)
		vsub( res_neon, res_neon, acc_neon);
		vand( res_neon, res_neon, mod_p);
		vsr(res_neon, res_neon, (SABER_EP-SABER_ET));
		
		// Unpack avx
		vstore(&temp[0][k*16], res_neon);
	}

	#if Saber_type == 1
		SABER_pack_3bit(msk_c, temp[0]);
	#elif Saber_type == 2
		SABER_pack_4bit(msk_c, temp[0]);
	#elif Saber_type == 3
		SABER_pack_6bit(msk_c, temp[0]);
	#endif


	for(j=0;j<SABER_SCALEBYTES_KEM;j++){
		ciphertext[SABER_CIPHERTEXTBYTES + j] = msk_c[j];
	}
}


void indcpa_kem_dec(const unsigned char *sk, 
					const unsigned char *ciphertext, 
					unsigned char message_dec[])
{
	uint32_t i,j,k;
	
	
	uint16_t sksv[SABER_K][SABER_N]; //secret key of the server
	

	uint16_t pksv[SABER_K][SABER_N];
	
	uint16_t message_dec_unpacked[SABER_KEYBYTES*8];	// one element containes on decrypted bit;

	uint8_t scale_ar[SABER_SCALEBYTES_KEM];
	uint16_t op[SABER_N];

	//--------------NEON declaration------------------
	  uint16x8_t mod_p=vdupq_n_u16(SABER_P-1); 
	  uint16x8x2_t acc_neon;
	  uint16x8x2_t v_neon;
	  uint16x8x2_t vh2;
	  vh2.val[0] = vdupq_n_u16(h2);
	  vh2.val[1] = vdupq_n_u16(h2);
	//--------------NEON declaration ends------------------
	uint16_t v_avx[SABER_N] = {0};
	uint16_t acc[SABER_N];
	
	//-------unpack the public_key

	BS2POLVEC(sk, sksv, SABER_Q); //sksv is the secret-key
	BS2POLVEC(ciphertext, pksv, SABER_P); //pksv is the ciphertext

	for(i=0;i<SABER_SCALEBYTES_KEM;i++){
		scale_ar[i]=ciphertext[SABER_CIPHERTEXTBYTES+i];
	}

	#if Saber_type == 1
		SABER_un_pack3bit(scale_ar, op);
	#elif Saber_type == 2
		SABER_un_pack4bit(scale_ar, op);
	#elif Saber_type == 3
		SABER_un_pack6bit(scale_ar, op);
	#endif

	// InnerProduct(b', s, mod p)
	for(j=0;j<SABER_K;j++){

		// toom_cook_4way_avx(pksv_avx[j], sksv_avx[j], SABER_P, acc);
		toom_cook_4way_neon(pksv[j], sksv[j], SABER_P, acc);

		for(k=0;k<SABER_N/16;k++){
			vload(v_neon, &v_avx[k*16]);
			vload(acc_neon, &acc[k*16]);
			// v_avx[k] += acc[k]
			vadd(v_neon, v_neon, acc_neon);
			// v_avx[k] &= mod_p 
			vand(v_neon, v_neon, mod_p);
			// acc[k] = 0
			// vzero(&acc[k*16], acc_neon); // No need
			vstore(&v_avx[k*16], v_neon);
		}
	}

	for(i=0; i<SABER_N/16; i++){
		//addition of h2
		vload(v_neon, &v_avx[i*16]);
		vadd(v_neon, v_neon, vh2);

		vload(acc_neon, &op[i*16]);
		vsl(acc_neon, acc_neon, (SABER_EP-SABER_ET));

		vsub(v_neon, v_neon, acc_neon);
		vand(v_neon, v_neon, mod_p);
		vsr(v_neon, v_neon, (SABER_EP-1));

		vstore(&message_dec_unpacked[i*16], v_neon);	
	}

	POL2MSG(message_dec_unpacked, message_dec);
}

