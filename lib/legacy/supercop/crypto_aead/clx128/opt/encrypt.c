/*   
     CLX-128: 128-bit key, 96-bit IV, 160-bit state  
     Optimized implementation of CLX-128 on 32-bit processor.   
     The state consists of five 32-bit registers
     state[4] || state[3] || state[2] || state[1] || state[0]
*/   

#include <string.h>
#include <stdio.h>
#include "crypto_aead.h"

#define FrameBitsIV  0x10  
#define FrameBitsAD  0x30  
#define FrameBitsPC  0x50  //Framebits for plaintext/ciphertext      
#define FrameBitsFinalization 0x70       

#define NROUND1 384 
#define NROUND2 1152 

/*optimized state update function*/
void state_update(unsigned int *state, unsigned int number_of_steps)
{
	unsigned int i, j, temp;
	unsigned int t1, t2, t3, t4;
	for (i = 0; i < number_of_steps; i = i + 128)
	{
		t1 = (state[1] >> 3)  | (state[2] << 29);    //35   = 1*32 + 3   
		t2 = (state[2] >> 29) | (state[3] << 3);     //93   = 2*32 + 29   
		t3 = (state[3] >> 10) | (state[4] << 22);    //106  = 3*32 + 10  
		t4 = (state[3] >> 31) | (state[4] << 1);     //127  = 3*32 + 31  
		state[0] ^= t1 ^ (~(t2 & t3)) ^ t4;

		t1 = (state[2] >> 3)  | (state[3] << 29);       
		t2 = (state[3] >> 29) | (state[4] << 3);        
		t3 = (state[4] >> 10) | (state[0] << 22);      
		t4 = (state[4] >> 31) | (state[0] << 1);       
		state[1] ^= t1 ^ (~(t2 & t3)) ^ t4;

		t1 = (state[3] >> 3)  | (state[4] << 29);
		t2 = (state[4] >> 29) | (state[0] << 3);
		t3 = (state[0] >> 10) | (state[1] << 22);
		t4 = (state[0] >> 31) | (state[1] << 1);
		state[2] ^= t1 ^ (~(t2 & t3)) ^ t4;

		t1 = (state[4] >> 3)  | (state[0] << 29);
		t2 = (state[0] >> 29) | (state[1] << 3);
		t3 = (state[1] >> 10) | (state[2] << 22);
		t4 = (state[1] >> 31) | (state[2] << 1);
		state[3] ^= t1 ^ (~(t2 & t3)) ^ t4;

		// rotate 32 bit positions
		temp = state[4]; 
		for (j = 4; j >= 1; j--) state[j] = state[j - 1]; 
		state[0] = temp;  
	}
}


// The initialization  
// The input to initialization is the key and IV; 
void initialization(const unsigned char *key, const unsigned char *iv, unsigned int *state)
{
	int i;

	//initialize the state as key
	state[0] = 0x80000000; 
	for (i = 0; i < 4; i++) state[i+1] = ((unsigned int*)key)[i]; 

	//update the state 
	state_update(state, NROUND2);

	//introduce iv into the state  
	for (i = 0;  i < 3; i++)  
	{
		state[2] ^= FrameBitsIV;
		state_update(state, NROUND1); 
		state[4] ^= ((unsigned int*)iv)[i]; 
	}    
}

//process the associated data     
void process_ad(const unsigned char *ad, unsigned long long adlen, unsigned int *state)
{
	unsigned long long i;
	unsigned int j;

	for (i = 0; i < (adlen >> 2); i++)
	{
		state[2] ^= FrameBitsAD;
		state_update(state, NROUND1);
		state[4] ^= ((unsigned int*)ad)[i];
	}

	// if adlen is not a multiple of 4, we process the remaining bytes
	if ((adlen & 3) > 0)
	{
		state[2] ^= FrameBitsAD;
		state_update(state, NROUND1);
		for (j = 0; j < (adlen & 3); j++)  ((unsigned char*)state)[16 + j] ^= ad[(i << 2) + j];
		state[2] ^= adlen & 3;
	}
}

//encrypt a message
int crypto_aead_encrypt(
	unsigned char *c,unsigned long long *clen,
	const unsigned char *m,unsigned long long mlen,
	const unsigned char *ad,unsigned long long adlen,
	const unsigned char *nsec,
	const unsigned char *npub,
	const unsigned char *k
	)
{
	unsigned long long i;
	unsigned int j; 
	unsigned char mac[8]; 
	unsigned int state[5]; 

	//initialization stage
	initialization(k, npub, state);

	//process the associated data   
	process_ad(ad, adlen, state); 

	//process the plaintext    
	for (i = 0; i < (mlen >> 2); i++)
	{
		state[2] ^= FrameBitsPC; 
		state_update(state, NROUND2);
		state[4] ^= ((unsigned int*)m)[i];
		((unsigned int*)c)[i] = state[4]; 
	}
	// if mlen is not a multiple of 4, we process the remaining bytes
	if ((mlen & 3) > 0)
	{
		state[2] ^= FrameBitsPC;
		state_update(state, NROUND2);  
		for (j = 0; j < (mlen & 3); j++)
		{
			((unsigned char*)state)[16 + j] ^= m[(i << 2) + j];
			c[(i << 2) + j] = ((unsigned char*)state)[16 + j];
		}  
		state[2] ^= (mlen & 3);  
	}

	//finalization stage, we assume that the tag length is a multiple of bytes
	for (i = 0; i < 2; i++)
	{
		state[2] ^= FrameBitsFinalization;
		state_update(state, NROUND2);
		((unsigned int*)mac)[i] = state[4];
	}

	*clen = mlen + 8; 
	memcpy(c+mlen, mac, 8);  

	return 0;
}

//decrypt a message
int crypto_aead_decrypt(
	unsigned char *m,unsigned long long *mlen,
	unsigned char *nsec,
	const unsigned char *c,unsigned long long clen,
	const unsigned char *ad,unsigned long long adlen,
	const unsigned char *npub,
	const unsigned char *k
	)
{
	unsigned long long i;
	unsigned int j, check = 0;
 	unsigned char mac[8];
	unsigned int state[5];

	*mlen = clen - 8; 

	//initialization stage
	initialization(k, npub, state);

	//process the associated data   
	process_ad(ad, adlen, state);

	//process the ciphertext    
	for (i = 0; i < (*mlen >> 2); i++)
	{
		state[2] ^= FrameBitsPC;       
		state_update(state, NROUND2);
		((unsigned int*)m)[i] = state[4] ^ ((unsigned int*)c)[i];
		state[4] = ((unsigned int*)c)[i];
	}
	// if mlen is not a multiple of 4, we process the remaining bytes
	if ((*mlen & 3) > 0) 
	{
		state[2] ^= FrameBitsPC;
		state_update(state, NROUND2);
		for (j = 0; j < (*mlen & 3); j++)
		{
			m[(i << 2) + j] = ((unsigned char*)state)[16 + j] ^ c[(i << 2) + j];
			((unsigned char*)state)[16 + j] = c[(i << 2) + j];
		}   
		state[2] ^= *mlen & 3;  
	}

	//finalization stage, we assume that the tag length is a multiple of bytes
	for (i = 0; i < 2; i++)
	{
		state[2] ^= FrameBitsFinalization;
		state_update(state, NROUND2);
		((unsigned int*)mac)[i] = state[4];
	}
			
	for (i = 0; i  < 8; i++) check |= (mac[i] ^ c[clen - 8 + i]);
	if (check == 0) return 0;
	else return -1;
}
