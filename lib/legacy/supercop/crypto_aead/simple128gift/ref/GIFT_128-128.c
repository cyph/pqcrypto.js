
#define ROUNDS 40

typedef unsigned char u8;
typedef unsigned int u32;

//Sbox
const u8 GIFT_S[16] = { 1,10, 4,12, 6,15, 3, 9, 2,13,11, 7, 5, 0, 8,14};
const u8 GIFT_S_inv[16] = {13, 0, 8, 6, 2,12, 4,11,14, 7, 1,10, 3, 9,15, 5};


//bit permutation
const u8 GIFT_P[]={
/* Block size = 128 */
  0, 33, 66, 99, 96,  1, 34, 67, 64, 97,  2, 35, 32, 65, 98,  3,
  4, 37, 70,103,100,  5, 38, 71, 68,101,  6, 39, 36, 69,102,  7,
  8, 41, 74,107,104,  9, 42, 75, 72,105, 10, 43, 40, 73,106, 11,
 12, 45, 78,111,108, 13, 46, 79, 76,109, 14, 47, 44, 77,110, 15,
 16, 49, 82,115,112, 17, 50, 83, 80,113, 18, 51, 48, 81,114, 19,
 20, 53, 86,119,116, 21, 54, 87, 84,117, 22, 55, 52, 85,118, 23,
 24, 57, 90,123,120, 25, 58, 91, 88,121, 26, 59, 56, 89,122, 27,
 28, 61, 94,127,124, 29, 62, 95, 92,125, 30, 63, 60, 93,126, 31
};

const u8 GIFT_P_inv[]={
/* Block size = 128 */
  0,  5, 10, 15, 16, 21, 26, 31, 32, 37, 42, 47, 48, 53, 58, 63,
 64, 69, 74, 79, 80, 85, 90, 95, 96,101,106,111,112,117,122,127,
 12,  1,  6, 11, 28, 17, 22, 27, 44, 33, 38, 43, 60, 49, 54, 59,
 76, 65, 70, 75, 92, 81, 86, 91,108, 97,102,107,124,113,118,123,
  8, 13,  2,  7, 24, 29, 18, 23, 40, 45, 34, 39, 56, 61, 50, 55,
 72, 77, 66, 71, 88, 93, 82, 87,104,109, 98,103,120,125,114,119,
  4,  9, 14,  3, 20, 25, 30, 19, 36, 41, 46, 35, 52, 57, 62, 51,
 68, 73, 78, 67, 84, 89, 94, 83,100,105,110, 99,116,121,126,115
};


// round constants
const u8 GIFT_RC[62] = {
    0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3E, 0x3D, 0x3B, 0x37, 0x2F,
    0x1E, 0x3C, 0x39, 0x33, 0x27, 0x0E, 0x1D, 0x3A, 0x35, 0x2B,
    0x16, 0x2C, 0x18, 0x30, 0x21, 0x02, 0x05, 0x0B, 0x17, 0x2E,
    0x1C, 0x38, 0x31, 0x23, 0x06, 0x0D, 0x1B, 0x36, 0x2D, 0x1A,
    0x34, 0x29, 0x12, 0x24, 0x08, 0x11, 0x22, 0x04, 0x09, 0x13,
    0x26, 0x0c, 0x19, 0x32, 0x25, 0x0a, 0x15, 0x2a, 0x14, 0x28,
    0x10, 0x20
};


void blockcipher_encrypt(u8 *ct, const u8 *pt, const u8 *masterkey){

	//convert input data from bytes to halfbytes
    u8 key[32];
    for(u8 i=0; i<16; i++){
        key[2*i] = masterkey[i]&0xF;
        key[2*i+1] = (masterkey[i]&0xF0)>>4;
    }
	
	u8 input[32];
    for(u8 i=0; i<16; i++){
        input[2*i] = pt[i]&0xF;
        input[2*i+1] = (pt[i]&0xF0)>>4;
    }
	
	
    //input = MSB [15][14]...[1][0] LSB
    //key = MSB [31][30]...[1][0] LSB

	u8 bits[128];
	u8 perm_bits[128];
	u8 key_bits[128];
	u8 temp_key[32];
	
	for(u8 r=0; r<ROUNDS; r++){

		//SubCells
		for(u8 i=0; i<32; i++){
			input[i] = GIFT_S[input[i]];
		}
		

		//PermBits
		//input to bits
		for(u8 i=0; i<32; i++){
			for(u8 j=0; j<4; j++){
				bits[4*i+j] = (input[i] >> j) & 0x1;
			}
		}
		
		//permute the bits
		for(u8 i=0; i<128; i++){
			perm_bits[GIFT_P[i]] = bits[i];
		}
		
		//perm_bits to input
		for(u8 i=0; i<32; i++){
			input[i]=0;
			for(u8 j=0; j<4; j++){
				 input[i] ^= perm_bits[4*i+j] << j;
			}
		}


		//AddRoundKey
		//input to bits
		for(u8 i=0; i<32; i++){
			for(u8 j=0; j<4; j++){
				bits[4*i+j] = (input[i] >> j) & 0x1;
			}
		}
		
		//key to key_bits
		for(u8 i=0; i<32; i++){
			for(u8 j=0; j<4; j++){
				key_bits[4*i+j] = (key[i] >> j) & 0x1;
			}
		}

		//add round key
		u8 kbc=0;  //key_bit_counter
		for(u8 i=0; i<32; i++){
			bits[4*i+1] ^= key_bits[kbc];
			bits[4*i+2] ^= key_bits[kbc+64];
			kbc++;
		}

		//add constant
		bits[3] ^= GIFT_RC[r] & 0x1;
		bits[7] ^= (GIFT_RC[r]>>1) & 0x1;
		bits[11] ^= (GIFT_RC[r]>>2) & 0x1;
		bits[15] ^= (GIFT_RC[r]>>3) & 0x1;
		bits[19] ^= (GIFT_RC[r]>>4) & 0x1;
		bits[23] ^= (GIFT_RC[r]>>5) & 0x1;
		bits[127] ^= 1;

		//bits to input
		for(u8 i=0; i<32; i++){
			input[i]=0;
			for(u8 j=0; j<4; j++){
				 input[i] ^= bits[4*i+j] << j;
			}
		}
		

		//key update
		//entire key>>32
		for(u8 i=0; i<32; i++){
			temp_key[i] = key[(i+8)%32];
		}
		for(u8 i=0; i<24; i++){
			key[i] = temp_key[i];
		}
		
		//k0>>12
		key[24] = temp_key[27];
		key[25] = temp_key[24];
		key[26] = temp_key[25];
		key[27] = temp_key[26];
		
		//k1>>2
		key[28] = ((temp_key[28]&0xc)>>2) ^ ((temp_key[29]&0x3)<<2);
		key[29] = ((temp_key[29]&0xc)>>2) ^ ((temp_key[30]&0x3)<<2);
		key[30] = ((temp_key[30]&0xc)>>2) ^ ((temp_key[31]&0x3)<<2);
		key[31] = ((temp_key[31]&0xc)>>2) ^ ((temp_key[28]&0x3)<<2);
	}
	
	//convert back from half-bytes
	for(u8 i=0; i<16; i++){
		ct[i] = input[2*i+1]<<4 | input[2*i];
	}
	
	return;
}