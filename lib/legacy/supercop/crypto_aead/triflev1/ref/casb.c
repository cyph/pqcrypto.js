/* CASB Block cipher*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "casb.h"
/* No. of block cipher rounds to be used. */ 


/* Generate encryption round keys. */

/* Generate decryption round keys. */
#define _casb_DEC_ROUND_KEY_GEN(round_keys, key)		(generate_round_keys(round_keys, key))



/* sbox */ 
const u8 _casb_sbox[16] = { 0, 12, 9, 7, 3, 5, 14, 4, 6, 11, 10, 2, 13, 1, 8, 15 };

/* bit permutation and its inverse*/ 
const u8 _casb_perm[] = {
	          	0,32,64,96,1,33,65,97,2,34,66,98,3,35,67,99,4,36,68,100,
                        5,37,69,101,6,38,70,102,7,39,71,103,8,40,72,104,9,41,73,105,10,
                        42,74,106,11,43,75,107,12,44,76,108,13,45,77,109,   
                        14,46,78,110,15,47,79,111,16,48,80,112,17,49,81,113,18,50,82,114,19,51,83,115,
                        20,52,84,116,21,53,85,117,22,54,86,118,23,55,87,119,24,56,88,120,25,57,89,121,26,58,90,122,27,
                        59,91,123,28,60,92,124,29,61,93,125,30,62,94,126,31,63,95,127
};

/* round constants */ 
const u8 _casb_round_constants[62] = {
    0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3E, 0x3D, 0x3B, 0x37, 0x2F,
    0x1E, 0x3C, 0x39, 0x33, 0x27, 0x0E, 0x1D, 0x3A, 0x35, 0x2B,
    0x16, 0x2C, 0x18, 0x30, 0x21, 0x02, 0x05, 0x0B, 0x17, 0x2E,
    0x1C, 0x38, 0x31, 0x23, 0x06, 0x0D, 0x1B, 0x36, 0x2D, 0x1A,
    0x34, 0x29, 0x12, 0x24, 0x08, 0x11, 0x22, 0x04, 0x09, 0x13,
    0x26, 0x0c, 0x19, 0x32, 0x25, 0x0a, 0x15, 0x2a, 0x14, 0x28,
    0x10, 0x20
};

/**********************************************************************
 * 
 * bytes_to_nibbles:	Convert byte oriented "src" to nibble oriented "dest".
 * 
 **********************************************************************/
void bytes_to_nibbles(u8 *dest, const u8 *src, u8 src_len)
{
    for(u8 i=0; i<src_len; i++)
    {
        dest[2*i] = src[i]&0xF;
        dest[2*i+1] = (src[i]&0xF0)>>4;
    }
}

/**********************************************************************
 * 
 * nibbles_to_bytes: Convert nibble oriented "src" to byte oriented "dest".
 * 
 **********************************************************************/
void nibbles_to_bytes(u8 *dest, const u8 *src, u8 dest_len)
{
    for(u8 i=0; i<dest_len; i++)
	{
		dest[i] = src[2*i+1]<<4 | src[2*i];
	}
}

/**********************************************************************
 * 
 * nibbles_to_bits: Convert nibble oriented "src" to bit oriented "dest".
 * 
 **********************************************************************/
void nibbles_to_bits(u8 *dest, const u8 *src, u8 src_len)
{
	for(u8 i=0; i<src_len; i++)
	{
		for(u8 j=0; j<4; j++)
		{
			dest[4*i+j] = (src[i] >> j) & 0x1;
		}
	}
}

/**********************************************************************
 * 
 * bits_to_nibbles: Convert bit oriented "src" to nibble oriented "dest".
 * 
 **********************************************************************/
void bits_to_nibbles(u8 *dest, const u8 *src, u8 dest_len)
{
    for(u8 i=0; i<dest_len; i++)
	{
		dest[i]=0;
		for(u8 j=0; j<4; j++)
		{
			 dest[i] ^= src[4*i+j] << j;
		}
	}
}



/**********************************************************************
 * 
 * generate_round_keys:	Generate and store the round key nibbles using the
 * 			master key.
 * 
 **********************************************************************/
void generate_round_keys(u8 (*round_key_nibbles)[32], const u8 *key_bytes)
{
	u8 key_nibbles[32];
	
	// convert master key from byte-oriented to nibble-oriented
	bytes_to_nibbles(key_nibbles, key_bytes, 16);
	
	//copy the first round key
	for (u8 i=0; i<32; i++)
	{
		round_key_nibbles[0][i] = key_nibbles[i];
	}
	
	// update round key and store the rest of the round keys
	u8 temp[32];
	for (u8 r=1; r<CRYPTO_BC_NUM_ROUNDS; r++)
	{
		//key update entire key>>32
		for(u8 i=0; i<32; i++)
		{
			temp[i] = key_nibbles[(i+8)%32];
		}
		for(u8 i=0; i<24; i++)
		{
			key_nibbles[i] = temp[i];
		}
		//k0>>12
		key_nibbles[24] = temp[27];
		key_nibbles[25] = temp[24];
		key_nibbles[26] = temp[25];
		key_nibbles[27] = temp[26];
		//k1>>2
		key_nibbles[28] = ((temp[28]&0xc)>>2) ^ ((temp[29]&0x3)<<2);
		key_nibbles[29] = ((temp[29]&0xc)>>2) ^ ((temp[30]&0x3)<<2);
		key_nibbles[30] = ((temp[30]&0xc)>>2) ^ ((temp[31]&0x3)<<2);
		key_nibbles[31] = ((temp[31]&0xc)>>2) ^ ((temp[28]&0x3)<<2);
		
		//copy the key state
		for (u8 i=0; i<32; i++)
		{
			round_key_nibbles[r][i] = key_nibbles[i];
		}
	}
}

/**********************************************************************
 * 
 * sub_cells:	SubCells operation.
 * 
 **********************************************************************/
void sub_cells(u8 *state_nibbles)
{
	for(u8 i=0; i<32; i++)
	{
		state_nibbles[i] = _casb_sbox[state_nibbles[i]];
	}
}


/**********************************************************************
 * 
 * perm_bits:	PermBits operation.
 * 
 **********************************************************************/
void perm_bits(u8 *state_bits)
{	
	//nibbles to bits
	u8 bits[128];
	
	//permute the bits
	for(u8 i=0; i<128; i++)
	{
		bits[_casb_perm[i]] = state_bits[i];
	}
	memcpy(&state_bits[0], &bits[0], 128);
}


/**********************************************************************
 * 
 * add_round_key:	Add round key operation.
 * 
 **********************************************************************/
void add_round_key(u8 *state_bits, const u8 *round_key_nibbles)
{		
	//round key nibbles to bits
	u8 key_bits[128];
	nibbles_to_bits(&key_bits[0], &round_key_nibbles[0], 32);

	//add round key
	for (u8 i=0; i<32; i++)
	{
		state_bits[4*i+1] ^= key_bits[i];
		state_bits[4*i+2] ^= key_bits[i+64];
	}
}


/**********************************************************************
 * 
 * add_round_constant:	Add round constant operation.
 * 
 **********************************************************************/
void add_round_constants(u8 *state_bits, u8 round)
{
	//add constant
	state_bits[3] ^= _casb_round_constants[round] & 0x1;
	state_bits[7] ^= (_casb_round_constants[round]>>1) & 0x1;
	state_bits[11] ^= (_casb_round_constants[round]>>2) & 0x1;
	state_bits[15] ^= (_casb_round_constants[round]>>3) & 0x1;
	state_bits[19] ^= (_casb_round_constants[round]>>4) & 0x1;
	state_bits[23] ^= (_casb_round_constants[round]>>5) & 0x1;
	state_bits[127] ^= 1;
}

/**********************************************************************
 * 
 * casb_enc:	casb encryption function.
 * 
 **********************************************************************/
void casb_enc(u8 *ct, const u8 (*round_keys)[32], const u8 *pt)
{
    u8 in[32];
    
	// convert input plaintext from byte-oriented to nibble-oriented
	bytes_to_nibbles(&in[0], &pt[0], 16);
    	
	u8 in_bits[128];
	
	for(u8 r=0; r < CRYPTO_BC_NUM_ROUNDS; r++)
	{
		// subnibbles operation
		sub_cells(&in[0]);

		// bitpermutation operation
		nibbles_to_bits(&in_bits[0], &in[0], 32);
		perm_bits(&in_bits[0]);
		
		// addroundkey operation
		add_round_key(&in_bits[0], round_keys[r]);
		
	
		// addroundconstant operation
		add_round_constants(&in_bits[0], r);
		
		// input bits to nibbles
		bits_to_nibbles(&in[0], &in_bits[0], 32);
	}
	
	//convert ciphertext from nibble-oriented to byte-oriented
	nibbles_to_bytes(ct, in, 16);
}
