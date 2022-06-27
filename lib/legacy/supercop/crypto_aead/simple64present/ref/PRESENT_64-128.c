#include <stdio.h>

#define ROUNDS 31

//State-size in Half-Bytes
#define SSZ 16

typedef unsigned char u8;
typedef unsigned int u32;

//SBOX
const u8 PRESENT_SBOX[16] = {0xC, 5, 6, 0xB, 9, 0, 0xA, 0xD, 3, 0xE, 0xF, 8, 4, 7, 1, 2};

//bit permutation
const u8 PRESENT_PERM[64] = {
   0, 16, 32, 48,  1, 17, 33, 49,  2, 18, 34, 50,  3, 19, 35, 51,
   4, 20, 36, 52,  5, 21, 37, 53,  6, 22, 38, 54,  7, 23, 39, 55,
   8, 24, 40, 56,  9, 25, 41, 57, 10, 26, 42, 58, 11, 27, 43, 59,
  12, 28, 44, 60, 13, 29, 45, 61, 14, 30, 46, 62, 15, 31, 47, 63
};

void blockcipher_encrypt(u8 *ct, const u8 *pt, const u8 *masterkey){

	//convert input data from bytes to Half-Bytes
    u8 k_register[32];
    for(u8 i=0; i<16; i++){
        k_register[2*i] = masterkey[i]&0xF;
        k_register[2*i+1] = (masterkey[i]&0xF0)>>4;
    }
	
	u8 state[SSZ];
    for(u8 i=0; i<SSZ/2; i++){
        state[2*i] = pt[i]&0xF;
        state[2*i+1] = (pt[i]&0xF0)>>4;
    }
	
    //state = MSB [15][14]...[1][0] LSB
    //key = MSB [31][30]...[1][0] LSB

	u8 round_key[ROUNDS+1][SSZ] = { 0 };

	
	//generateRoundKeys
	
	u8 r_counter = 1;
	u8 temp_k_reg[32] = { 0 };
	
	for(u8 r=0; r<ROUNDS+1; r++){
	
		//Extract roundkey k127...k64
		for(u8 i=0; i<SSZ; i++){
			round_key[r][i] = k_register[16+i];
		}
		
		//ROL 61
		for(u8 i=0; i<32; i++){
			temp_k_reg[i] = (k_register[(i+32-15)%32]<<1 | k_register[(i+32-16)%32]>>3)&0xF;
		}
		
		//SBOX-Substitution
		temp_k_reg[31] = PRESENT_SBOX[temp_k_reg[31]];
		temp_k_reg[30] = PRESENT_SBOX[temp_k_reg[30]];
		
		//~copy temp register
		for(u8 i=0; i<32; i++){
			k_register[i] = temp_k_reg[i];
		}
		
		//Round Counter Addition
		k_register[15] ^= (r_counter<<2)&0xF;
		k_register[16] ^= r_counter>>2;
		
		
		r_counter++;
	}
	
		
	u8 bits [64] = { 0 };
	u8 perm_bits [64] = { 0 };

	for(u8 r=0; r<ROUNDS; r++){
		
		
		//addRoundKey
		for(u8 i=0; i<SSZ; i++){
			state[i] = state[i] ^ round_key[r][i];
		}
		
		
		//sBoxLayer
		for(u8 i=0; i<SSZ; i++){
			state[i] = PRESENT_SBOX[state[i]];
		}
		
		
		//pLayer
		//convert state to bits
		for(u8 i=0; i<SSZ; i++){
			for(u8 j=0; j<4; j++){
				bits[4*i+j] = (state[i] >> j) & 0x1;
			}
		}
		
		//permute the bits
		for(u8 i=0; i<64; i++){
			perm_bits[PRESENT_PERM[i]] = bits[i];
		}
		
		//convert permutated bits to state
		for(u8 i=0; i<SSZ; i++){
			state[i]=0;
			for(u8 j=0; j<4; j++){
				state[i] ^= perm_bits[4*i+j] << j;
			}
		}
		
	}
	
	//last addRoundKey
	for(u8 i=0; i<SSZ; i++){
		state[i] = state[i] ^ round_key[ROUNDS][i];
	}
	
	
	//convert back from half-bytes
	for(u8 i=0; i<SSZ/2; i++){
		ct[i] = state[2*i+1]<<4 | state[2*i];
	}
	
	return;
}