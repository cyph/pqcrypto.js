/*   
     CLX-Hash: 288-bit state, 256-bit message digest
     Reference implementation on 32-bit processor   
     The state consists of 9 32-bit registers   
     state[8] || state[7] || state[6] || state[5] || state[4] || state[3] || state[2] || state[1] || state[0]

     Implemented by Hongjun Wu 
*/   


#include "crypto_hash.h" 

#define NROUND1 1024   
#define NROUND2 2560 
#define NROUND3 256

#define X  4  // The state size is 160 + 32*x bits 

/*non-optimized state update function*/  
void state_update(unsigned int *state, unsigned int number_of_steps) 
{
	unsigned int i,j;
	unsigned int t0, t1, t2, t3, t4, feedback;

	for (i = 0; i < number_of_steps; i = i + 32)
	{
		t0 = (state[0] >> 19) | (state[1] << 13);
		t1 = (state[1 + X] >> 3)  | (state[2 + X] << 29);
		t2 = (state[2 + X] >> 29) | (state[3 + X] << 3);
		t3 = (state[3 + X] >> 10) | (state[4 + X] << 22);
		t4 = (state[3 + X] >> 31) | (state[4 + X] << 1);
		feedback = state[0] ^ t0 ^ state[X] ^ t1 ^ (~(t2 & t3)) ^ t4;
		// shift 32 bit positions
		for (j = 0; j <= X + 3; j++) state[j] = state[j + 1];
		state[X + 4] = feedback;
	}
}
  
//hash a message
int crypto_hash(
	unsigned char *out,
	const unsigned char *in,
	unsigned long long inlen
)
{
	unsigned long long i; 
	unsigned int j; 
	unsigned int state[9]; 

	//initialize the state as constants  
	for (i = 0; i <= 8; i++) state[i] = 0;  
	state[6] ^= 0x10; 
	state_update(state, NROUND1);    

	//process the message  
	for (i = 0; i < (inlen >> 2); i++) 
	{   
		state[8] ^= ((unsigned int*)in)[i];  
		state[6] ^= 0x10;
		state_update(state, NROUND2);   
	}

	// if inlen is not a multiple of 4 bytes, we process the remaining bytes
	if ((inlen & 3) > 0)
	{
		state[6] ^= (inlen & 3);
		state[6] ^= 0x10;

		for (j = 0; j < (inlen & 3); j++)       
		{
			((unsigned char*)state)[32+j] ^= in[(i << 2) + j];
		}
		state_update(state, NROUND2);
	}

	//finalization stage  
	for (i = 0; i < 8; i = i + 1)  
	{
		((unsigned int*)out)[i] = state[8];
		state[6] ^= 0x10;
		state_update(state, NROUND3);    
	}

	return 0;
}

