#include <stdio.h>
#include <string.h>
#include "api.h"
#include "hanmre.h"
#include "permutation.h"
#include <unistd.h>

const unsigned char HANMRE_IV_1[] = {0x01,HANMRE_K,HANMRE_rH,HANMRE_rB,HANMRE_sH,HANMRE_sB,HANMRE_sE,HANMRE_sK};

const unsigned char HANMRE_IV_2[] = {0x02,HANMRE_K,HANMRE_rH,HANMRE_rB,HANMRE_sH,HANMRE_sB,HANMRE_sE,HANMRE_sK};

const unsigned char HANMRE_IV_3[] = {0x03,HANMRE_K,HANMRE_rH,HANMRE_rB,HANMRE_sH,HANMRE_sB,HANMRE_sE,HANMRE_sK};

void hanmre_mac(
	const unsigned char *ad, const unsigned long long adlen,
	const unsigned char *m, const unsigned long long mlen,
	unsigned char *tag
){
	// Init State
	unsigned char state[HANMRE_STATE_SZ];
	Permutation_Initialize(state);
	Permutation_AddBytes(state,HANMRE_IV_2,0,HANMRE_IV_SZ);
	Permutation_AddBytes(state,HANMRE_IV_3,HANMRE_IV_SZ,HANMRE_IV_SZ);
	
	Permutation_Permute_Nrounds(state,HANMRE_sH);

	// Absorb AD
	size_t rate_bytes_avail = HANMRE_rH_SZ;

	unsigned char cur_ad;

	for (unsigned long long i = 0; i < adlen; i++){
		if(rate_bytes_avail == 0){
			Permutation_Permute_Nrounds(state,HANMRE_sH);
			rate_bytes_avail=HANMRE_rH_SZ;
		}
		cur_ad = ad[i];
		Permutation_AddBytes(state,&cur_ad,HANMRE_rH_SZ-rate_bytes_avail,1);
		
		rate_bytes_avail--;
	}

	// Absorb Padding: 0x80
	if(rate_bytes_avail == 0){
		Permutation_Permute_Nrounds(state,HANMRE_sH);
		rate_bytes_avail = HANMRE_rH_SZ;
	}
	unsigned char pad = 0x80;
	Permutation_AddBytes(state,&pad,HANMRE_rH_SZ-rate_bytes_avail,1);
	Permutation_Permute_Nrounds(state,HANMRE_sH);

	// Domain Seperation ##\n"); 
	unsigned char dom_sep = 0x01;
	Permutation_AddBytes(state,&dom_sep,HANMRE_STATE_SZ-1,1);

	// Absorb P
	rate_bytes_avail = HANMRE_rH_SZ;
	unsigned char cur_m;
	for (unsigned long long i = 0; i < mlen; i++){
		cur_m = m[i];
		Permutation_AddBytes(state,&cur_m,HANMRE_rH_SZ-rate_bytes_avail,1);
		rate_bytes_avail--;
		if(rate_bytes_avail == 0){
			Permutation_Permute_Nrounds(state,HANMRE_sH);
			rate_bytes_avail = HANMRE_rH_SZ;
		}
	}

	// Absorb Padding: 0x80
	pad = 0x80;
	Permutation_AddBytes(state,&pad,HANMRE_rH_SZ-rate_bytes_avail,1);
	Permutation_Permute_Nrounds(state,HANMRE_sH);

	unsigned char y[CRYPTO_KEYBYTES];
	Permutation_ExtractBytes(state,y,0,CRYPTO_KEYBYTES);

	// Squeezing Tag
	Permutation_OverwriteBytes(state,y,0,CRYPTO_KEYBYTES);
	Permutation_Permute_Nrounds(state,HANMRE_sH);
	Permutation_ExtractBytes(state,tag,0,CRYPTO_KEYBYTES);
}


void hanmre_rk(
	const unsigned char *k,
	const unsigned char *iv,
	const unsigned char *in,
	const unsigned long long inlen,
	unsigned char *out,
	const unsigned long long outlen
){
	// Init State
	unsigned char state[HANMRE_STATE_SZ];
	Permutation_Initialize(state);
	Permutation_AddBytes(state,k,0,CRYPTO_KEYBYTES);
	Permutation_AddBytes(state,iv,CRYPTO_KEYBYTES,HANMRE_IV_SZ);
	Permutation_Permute_Nrounds(state,HANMRE_sK);
	
	// Absorb
	for (size_t i = 0; i < inlen*8-1; i++){
		size_t cur_byte_pos = i/8;
		size_t cur_bit_pos = 7-(i%8);
		unsigned char cur_bit = ((in[cur_byte_pos] >> (cur_bit_pos)) & 0x01) << 7;
		Permutation_AddBytes(state,(const unsigned char*)&cur_bit,0,1);
		Permutation_Permute_Nrounds(state,HANMRE_sB);
	}
	unsigned char cur_bit = ((in[inlen-1]) & 0x01) << 7;
	Permutation_AddBytes(state,(const unsigned char*)&cur_bit,0,1);
	Permutation_Permute_Nrounds(state,HANMRE_sK);

	// Squeeze K*
	Permutation_ExtractBytes(state,out,0,outlen);
}


void hanmre_enc(
	const unsigned char *k,
	const unsigned char *tag,
	const unsigned char *m, const unsigned long long mlen,
	unsigned char *c
){	
	// Derive K*
	unsigned char state[HANMRE_STATE_SZ];
	hanmre_rk(k,HANMRE_IV_1,tag,CRYPTO_KEYBYTES,state,HANMRE_STATE_SZ-CRYPTO_KEYBYTES);
	Permutation_OverwriteBytes(state,tag,HANMRE_STATE_SZ-CRYPTO_KEYBYTES,CRYPTO_KEYBYTES);

	Permutation_Permute_Nrounds(state,HANMRE_sB);
	Permutation_Permute_Nrounds(state,HANMRE_sH);

	// Squeeze Keystream
	size_t key_bytes_avail = 0;
	for (unsigned long long i = 0; i < mlen; i++) {
		if(key_bytes_avail == 0){
			Permutation_Permute_Nrounds(state,HANMRE_sE);
			key_bytes_avail = HANMRE_rH_SZ;
		}
		unsigned char keybyte;
		Permutation_ExtractBytes(state,&keybyte,i%HANMRE_rH_SZ,1);
		c[i] = m[i] ^ keybyte;
		key_bytes_avail--;
	}

}
