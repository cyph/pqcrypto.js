/*   
	CLX-192Q: 192-bit key, 96-bit IV, 256-bit state   
	Optimized implementation on 32-bit processor.   
	The state consists of 8 32-bit registers       
	state[7] || state[6] || state[5] || state[4] || state[3] || state[2] || state[1] || state[0]

	Implemented by Hongjun Wu 
*/   

#include <string.h>
#include <stdio.h>
#include "crypto_aead.h"

#define FrameBitsIV  0x10  
#define FrameBitsAD  0x30  
#define FrameBitsPC  0x50  //Framebits for plaintext/ciphertext      
#define FrameBitsFinalization 0x70       

#define NROUND1 384   
#define NROUND2 768    
#define NROUND3 1408  
#define NROUND4 768   

#define X  3       // The state size is 160 + 32*X bits 
#define KEYSIZE 6  // The key size is 32*6 bits 

/*optimized state update function*/ 
void state_update(unsigned int *state, unsigned int number_of_steps) 
{
	unsigned int i, temp;
	unsigned int t1, t2, t3, t4;
	for (i = 0; i < number_of_steps; i = i + 128)
	{
		t1 = (state[4] >> 3)  | (state[5] << 29);       
		t2 = (state[5] >> 29) | (state[6] << 3);        
		t3 = (state[6] >> 10) | (state[7] << 22);      
		t4 = (state[6] >> 31) | (state[7] << 1);     
		state[0] ^= state[3] ^ t1 ^ (~(t2 & t3)) ^ t4;

		t1 = (state[5] >> 3)  | (state[6] << 29);
		t2 = (state[6] >> 29) | (state[7] << 3);
		t3 = (state[7] >> 10) | (state[0] << 22);
		t4 = (state[7] >> 31) | (state[0] << 1);
		state[1] ^= state[4] ^ t1 ^ (~(t2 & t3)) ^ t4;

		t1 = (state[6] >> 3)  | (state[7] << 29);
		t2 = (state[7] >> 29) | (state[0] << 3);
		t3 = (state[0] >> 10) | (state[1] << 22);
		t4 = (state[0] >> 31) | (state[1] << 1);
		state[2] ^= state[5] ^ t1 ^ (~(t2 & t3)) ^ t4;

		t1 = (state[7] >> 3)  | (state[0] << 29);
		t2 = (state[0] >> 29) | (state[1] << 3);
		t3 = (state[1] >> 10) | (state[2] << 22);
		t4 = (state[1] >> 31) | (state[2] << 1);
		state[3] ^= state[6] ^ t1 ^ (~(t2 & t3)) ^ t4;

		//swap state[i] with state[i+4] for i = 0 to 3 
		temp = state[0]; state[0] = state[4]; state[4] = temp;  
		temp = state[1]; state[1] = state[5]; state[5] = temp; 
		temp = state[2]; state[2] = state[6]; state[6] = temp; 
		temp = state[3]; state[3] = state[7]; state[7] = temp; 		
	}
}

// The following code are identical for: 
// CLX-128Q, CLX-128H, CLX-192Q, CLX-192H, CLX-256Q, CLX-256H   

// The initialization  
// The input to initialization is the key and IV;  
void initialization(const unsigned char *key, const unsigned char *iv, unsigned int *state)
{
	int i;

	//initialize the state as key
	state[0] = 0;
	state[1] = 0x80000000;
	for (i = 0; i < KEYSIZE; i++) state[i + 2] = ((unsigned int*)key)[i];

	//update the state 
	state_update(state, NROUND3); 

	//introduce iv into the state  
	for (i = 0;  i < 3; i++)  
	{
		state[2+X] ^= FrameBitsIV;
		state_update(state, NROUND1); 
		state[4+X] ^= ((unsigned int*)iv)[i]; 
	}   
}

//process the associated data       
void process_ad(const unsigned char *ad, unsigned long long adlen, unsigned int *state)
{
	unsigned long long i;
	unsigned int j;

	for (i = 0; i < (adlen >> 2); i++)
	{
		state[2+X] ^= FrameBitsAD;
		state_update(state, NROUND1);
		state[4+X] ^= ((unsigned int*)ad)[i]; 
	}

	// if adlen is not a multiple of 4, we process the remaining bytes
	if ((adlen & 3) > 0)
	{
		state[2+X] ^= FrameBitsAD;
		state_update(state, NROUND1);
		for (j = 0; j < (adlen & 3); j++)  ((unsigned char*)state)[16 + (X << 2) + j] ^= ad[(i << 2) + j];
		state[2+X] ^= adlen & 3;
	}

	//update the state using Permu4
	state_update(state, NROUND4);
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
	unsigned int state[5+X]; 

	//initialization stage
	initialization(k, npub, state);

	//process the associated data   
	process_ad(ad, adlen, state); 

	//process the plaintext    
	for (i = 0; i < (mlen >> 2); i++)
	{
		state[2+X] ^= FrameBitsPC;            
		state_update(state, NROUND2);       
		state[4+X] ^= ((unsigned int*)m)[i];  
		((unsigned int*)c)[i] = state[4+X];   
	}
	// if mlen is not a multiple of 4, we process the remaining bytes
	if ((mlen & 3) > 0)
	{
		state[2+X] ^= FrameBitsPC;
		state_update(state, NROUND2);  
		for (j = 0; j < (mlen & 3); j++)
		{
			((unsigned char*)state)[16 + (X << 2) + j] ^= m[(i << 2) + j];
			c[(i << 2) + j] = ((unsigned char*)state)[16 + (X << 2) + j];
		}  
		state[2+X] ^= (mlen & 3);  
	}

	//finalization stage, we assume that the tag length is a multiple of bytes
	for (i = 0; i < 2; i++)
	{
		state[2+X] ^= FrameBitsFinalization;
		state_update(state, NROUND3);
		((unsigned int*)mac)[i] = state[4+X]; 
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
	unsigned int state[5+X]; 

	*mlen = clen - 8; 

	//initialization stage
	initialization(k, npub, state);

	//process the associated data   
	process_ad(ad, adlen, state);

	//process the ciphertext    
	for (i = 0; i < (*mlen >> 2); i++)
	{
		state[2+X] ^= FrameBitsPC;       
		state_update(state, NROUND2);
		((unsigned int*)m)[i] = state[4+X] ^ ((unsigned int*)c)[i];
		state[4+X] = ((unsigned int*)c)[i];
 	}
	// if mlen is not a multiple of 4, we process the remaining bytes
	if ((*mlen & 3) > 0) 
	{
		state[2+X] ^= FrameBitsPC;
		state_update(state, NROUND2);
		for (j = 0; j < (*mlen & 3); j++)
		{
			m[(i << 2) + j] = ((unsigned char*)state)[16 + (X << 2) + j] ^ c[(i << 2) + j];
			((unsigned char*)state)[16 + (X << 2) + j] = c[(i << 2) + j];
		}   
		state[2+X] ^= *mlen & 3;  
	}

	//finalization stage, we assume that the tag length is a multiple of bytes
	for (i = 0; i < 2; i++)
	{
		state[2+X] ^= FrameBitsFinalization;
		state_update(state, NROUND3);
		((unsigned int*)mac)[i] = state[4+X]; 
	}
    
	for (i = 0; i  < 8; i++) check |= (mac[i] ^ c[clen - 8 + i]);
	if (check == 0) return 0;
	else return -1;
}

