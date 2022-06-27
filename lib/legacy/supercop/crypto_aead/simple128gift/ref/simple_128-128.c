#include <string.h>
#include <stdlib.h>
#include "api.h"

#include "blockcipher.h"

//Message-Blocksize
#define MSZ 16


typedef unsigned char u8;		//used for Byte-Arrays
typedef unsigned int u32;		//used for regular counters
typedef unsigned long long ull;	//used for long counters

void E(u8 *ct, const u8 *key, const u8 *pt){
	
	blockcipher_encrypt(ct, pt, key);
	
	return;
}

void derive(u8 *K_E, u8 *K_MAC, u8 *N2, u8 *N1, const u8 *k, const u8 *npub){
	
	//N with first byte for counting
	u8 N_pad[MSZ] = { 0 };
	
	memcpy(&N_pad[1], npub, CRYPTO_NPUBBYTES);
	
	u8 T0[MSZ] = { 0 };
	u8 N2N1[MSZ] = { 0 };

	//(a) K_E
	E(K_E, k, N_pad);
	
	//(b) K_MAC
	N_pad[0] = 1;
	E(K_MAC, k, N_pad);
	
	//(c) [N2, N1]
	N_pad[0] = 2;
	E(N2N1, k, N_pad);
	for(u8 i=0; i<MSZ; i++){
		N2N1[i] ^= T0[i];
	}
	memcpy(N1, N2N1, MSZ/2);
	memcpy(N2, &N2N1[MSZ/2], MSZ/2);


	return;
}

void increase_ctr(u8 *ctr){
	
	for(u32 i=0; i<MSZ; i++){
		ctr[i]++;
		if(ctr[i] != 0){
			break;
		}
	}
	return;
}


int simple_encrypt(
unsigned char *c,unsigned long long *clen,
const unsigned char *m,unsigned long long mlen,
const unsigned char *ad,unsigned long long adlen,
const unsigned char *nsec,
const unsigned char *npub,
const unsigned char *k
){
	*clen = mlen+CRYPTO_ABYTES;
		
	//Don't use nsec
	if(nsec != NULL){
		u32 unused_32[1];
		memcpy(unused_32, nsec, 0);
	}

	//Rounding
	//Calculate number of blocks
	u32 r_M = mlen%MSZ;
	//u32 r_A = adlen%MSZ;
	ull mlen_blocks = r_M ? ((mlen/MSZ)+1) : (mlen/MSZ);
	//complete adlen_blocks to full blocks if last block is incomplete or add additional if last block is complete
	ull adlen_blocks = (adlen/MSZ)+1;
	
	
	u8 *T_A;
	T_A = (u8 *) malloc( MSZ*adlen_blocks * sizeof(u8));
	
	memset(T_A, 0, MSZ*adlen_blocks);
	memcpy(T_A, ad, adlen);
	//forced padding
	memset(&T_A[adlen], 0x01, 1);
	
	
	u8 *T_M;
	T_M = (u8 *) malloc( MSZ*mlen_blocks * sizeof(u8));
		
	memset(T_M, 0, MSZ*mlen_blocks);
	memcpy(T_M, m, mlen);
	
	
	u8 K_E[MSZ] = { 0 };
	u8 K_MAC[MSZ] = { 0 };
	u8 N1[MSZ/2] = { 0 };
	u8 N2[MSZ/2] = { 0 };
	u8 tag[MSZ] = { 0 };
	
	
	/*** Derive keys ***/
	derive(K_E, K_MAC, N2, N1, k, npub);
	
	
	/*** Encrypt ***/
	if(mlen == 0){
		;
	} else {
		u8 j_ctr[MSZ] = { 0 };
		
		//Set higher MSZ/2 Bytes of IV to N1
		memcpy(&j_ctr[MSZ/2], N1, MSZ/2);
		
		//CTRENC
		u8 V_j[MSZ] = { 0 };
		for(ull j=0; j<mlen_blocks; j++){
			
			//Vj = E(K_E, ctr_j)
			E(V_j, K_E, j_ctr);
			
			for(u32 i=0; i<MSZ; i++){
				T_M[j*MSZ+i] ^= V_j[i];
			}
			
			increase_ctr(j_ctr);
		}
	}
	memcpy(c, T_M, mlen);
	
	
	/*** Authenticate ***/
	//Reset T_M to message
	memset(T_M, 0, MSZ*mlen_blocks);
	memcpy(T_M, m, mlen);
	
	//Padding of M if last block is not full
	if(r_M != 0){
		memset(&T_M[mlen], 1, 1);
	}
	
	
	u8 X_temp[MSZ] = { 0 };
	
	u8 TMP[MSZ] = { 0 };
	
	//Set higher MSZ/2 Bytes of TMP to N2
	memcpy(&TMP[MSZ/2], N2, MSZ/2);
	
	
	//CBCMAC-IV
	for(ull j=0; j<mlen_blocks; j++){
		
		memcpy(X_temp, TMP, MSZ);
		
		//TMP XOR Xj
		for(u32 i=0; i<MSZ; i++){
			X_temp[i] ^= T_M[j*MSZ+i];
		}
		
		//TMP = E(K_MAC, TMP XOR Xj)
		E(TMP, K_MAC, X_temp);
	}
	
	//Padding of M if last block is full
	if( r_M == 0 ){
		
		memcpy(X_temp, TMP, MSZ);
		
		//TMP XOR 1
		X_temp[0] ^= 1;
		
		//TMP = E(K_MAC, TMP XOR Xj)
		E(TMP, K_MAC, X_temp);
	}
	
	for(ull j=0; j<adlen_blocks; j++){
		
		memcpy(X_temp, TMP, MSZ);
		
		//TMP XOR Xj
		for(u32 i=0; i<MSZ; i++){
			X_temp[i] ^= T_A[j*MSZ+i];
		}
		
		//TMP = E(K_MAC, TMP XOR Xj)
		E(TMP, K_MAC, X_temp);
	}
	
	//Tag = TMP
	memcpy(tag, TMP, MSZ);
	
	
	//Append Tag
	memcpy(&c[mlen], tag, CRYPTO_ABYTES);
	
	free(T_M);
	free(T_A);
	
	return 0;
}


int simple_decrypt(
unsigned char *m,unsigned long long *mlen,
unsigned char *nsec,
const unsigned char *c, unsigned long long clen,
const unsigned char *ad, unsigned long long adlen,
const unsigned char *npub,
const unsigned char *k
)
{
	*mlen = clen-CRYPTO_ABYTES;
		
	//Don't use nsec
	if(nsec != NULL){
		u32 unused_32[1];
		memcpy(unused_32, nsec, 0);
	}

	//Rounding
	//Calculate number of blocks
	u32 r_C = *mlen%MSZ;
	//u32 r_A = adlen%MSZ;
	ull mlen_blocks = r_C ? ((*mlen/MSZ)+1) : (*mlen/MSZ);
	//complete adlen_blocks to full blocks if last block is incomplete or add additional if last block is complete
	ull adlen_blocks = (adlen/MSZ)+1;
	
	
	u8 *T_A;
	T_A = (u8 *) malloc( MSZ*adlen_blocks * sizeof(u8));
	
	memset(T_A, 0, MSZ*adlen_blocks);
	memcpy(T_A, ad, adlen);
	//forced padding
	memset(&T_A[adlen], 0x01, 1);
	
	
	u8 *T_C;
	T_C = (u8 *) malloc( MSZ*mlen_blocks * sizeof(u8));
	
	memset(T_C, 0, MSZ*mlen_blocks);
	memcpy(T_C, c, *mlen);
	
	
	u8 K_E[MSZ] = { 0 };
	u8 K_MAC[MSZ] = { 0 };
	u8 N1[MSZ/2] = { 0 };
	u8 N2[MSZ/2] = { 0 };
	u8 tag[MSZ] = { 0 };
	
	int is_auth = 0;
	
	
	/*** Derive keys ***/
	derive(K_E, K_MAC, N2, N1, k, npub);
	
	
	/*** Decrypt ***/
	if(*mlen == 0){
		;
	} else {
		u8 j_ctr[MSZ] = { 0 };
		
		//Set higher MSZ/2 Bytes of IV to N1
		memcpy(&j_ctr[MSZ/2], N1, MSZ/2);
		
		//CTRENC
		u8 V_j[MSZ] = { 0 };
		for(ull j=0; j<mlen_blocks; j++){
			
			//Vj = E(K_E, j)
			E(V_j, K_E, j_ctr);
			
			for(u32 i=0; i<MSZ; i++){
				T_C[j*MSZ+i] ^= V_j[i];
			}
			
			increase_ctr(j_ctr);
		}
		if(r_C > 0){
			//Truncate
			memset(&T_C[(mlen_blocks-1)*MSZ+r_C], 0, MSZ-r_C);
		}
	}
	
	
	/*** Authenticate ***/
	
	//Padding of M if last block is not full
	if(r_C != 0){
		memset(&T_C[*mlen], 1, 1);
	}
	
	u8 X_temp[MSZ] = { 0 };
	
	u8 TMP[MSZ] = { 0 };
	
	//Set higher MSZ/2 Bytes of TMP to N2
	memcpy(&TMP[MSZ/2], N2, MSZ/2);
	
	
	//CBCMAC-IV
	for(ull j=0; j<mlen_blocks; j++){
		
		memcpy(X_temp, TMP, MSZ);
		
		//TMP XOR Xj
		for(u32 i=0; i<MSZ; i++){
			X_temp[i] ^= T_C[j*MSZ+i];
		}
		
		//TMP = E(K_MAC, TMP XOR Xj)
		E(TMP, K_MAC, X_temp);
	}
	
	//Padding of M if last block is full
	if( r_C == 0 ){
		
		memcpy(X_temp, TMP, MSZ);
		
		//TMP XOR 1
		X_temp[0] ^= 1;
		
		//TMP = E(K_MAC, TMP XOR Xj)
		E(TMP, K_MAC, X_temp);
	}
	
	for(ull j=0; j<adlen_blocks; j++){
		
		memcpy(X_temp, TMP, MSZ);
		
		//TMP XOR Xj
		for(u32 i=0; i<MSZ; i++){
			X_temp[i] ^= T_A[j*MSZ+i];
		}
		
		//TMP = E(K_MAC, TMP XOR Xj)
		E(TMP, K_MAC, X_temp);
	}
	
	//Tag = TMP
	memcpy(tag, TMP, MSZ);
	
	
	//compare T' == T (0 for equal)
	if( memcmp(tag, &c[*mlen], CRYPTO_ABYTES) == 0 ){
				
		//reverse Padding of M if last block is not full
		if(r_C != 0){
			memset(&T_C[*mlen], 0, 1);
		}
	
		//only copy message if auth succeeded
		//M = (Mm-1,...,M0)
		memcpy(m, T_C, *mlen);
	
		//if T' == T -> is_auth = 1	
		is_auth = 1;
	
	} else {
		
		//if T' =/= T -> is_auth = 0
		is_auth = 0;
		
	}
	
	free(T_C);
	free(T_A);
	
	if(is_auth){
		return 0;
	} else {
		return -1;
	}
}