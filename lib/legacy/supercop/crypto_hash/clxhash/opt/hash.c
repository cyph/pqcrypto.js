/*   
     CLX-Hash: 288-bit state, 256-bit message digest
     Optimized implementation on 32-bit processor   
     The state consists of 9 32-bit registers   
     state[8] || state[7] || state[6] || state[5] || state[4] || state[3] || state[2] || state[1] || state[0]

     Implemented by Hongjun Wu 
*/   


#include "crypto_hash.h" 

#define NROUND1 1024   
#define NROUND2 2560 
#define NROUND3 256

#define X  4   // The state size is 160 + 32*x bits 

/*optimized state update function*/
void state_update(unsigned int *state, unsigned int number_of_steps)
{
	unsigned int i, j, temp;
	unsigned int t0, t1, t2, t3, t4;

	for (i = 0; i < number_of_steps; i = i + 256) //every iteration 256 rounds 
	{
		t0 = (state[0] >> 19) | (state[1] << 13);
		t1 = (state[5] >> 3)  | (state[6] << 29);
		t2 = (state[6] >> 29) | (state[7] << 3);
		t3 = (state[7] >> 10) | (state[8] << 22);
		t4 = (state[7] >> 31) | (state[8] << 1);
		state[0] ^= t0 ^ state[4] ^ t1 ^ (~(t2 & t3)) ^ t4;

		t0 = (state[1] >> 19) | (state[2] << 13);
		t1 = (state[6] >> 3)  | (state[7] << 29);
		t2 = (state[7] >> 29) | (state[8] << 3);
		t3 = (state[8] >> 10) | (state[0] << 22);
		t4 = (state[8] >> 31) | (state[0] << 1);
		state[1] ^= t0 ^ state[5] ^ t1 ^ (~(t2 & t3)) ^ t4;

		t0 = (state[2] >> 19) | (state[3] << 13);
		t1 = (state[7] >> 3)  | (state[8] << 29);
		t2 = (state[8] >> 29) | (state[0] << 3);
		t3 = (state[0] >> 10) | (state[1] << 22);
		t4 = (state[0] >> 31) | (state[1] << 1);
		state[2] ^= t0 ^ state[6] ^ t1 ^ (~(t2 & t3)) ^ t4;

		t0 = (state[3] >> 19) | (state[4] << 13);
		t1 = (state[8] >> 3)  | (state[0] << 29);
		t2 = (state[0] >> 29) | (state[1] << 3);
		t3 = (state[1] >> 10) | (state[2] << 22);
		t4 = (state[1] >> 31) | (state[2] << 1);
		state[3] ^= t0 ^ state[7] ^ t1 ^ (~(t2 & t3)) ^ t4;

		t0 = (state[4] >> 19) | (state[5] << 13);
		t1 = (state[0] >> 3)  | (state[1] << 29);
		t2 = (state[1] >> 29) | (state[2] << 3);
		t3 = (state[2] >> 10) | (state[3] << 22);
		t4 = (state[2] >> 31) | (state[3] << 1);
		state[4] ^= t0 ^ state[8] ^ t1 ^ (~(t2 & t3)) ^ t4;

		t0 = (state[5] >> 19) | (state[6] << 13);
		t1 = (state[1] >> 3)  | (state[2] << 29);
		t2 = (state[2] >> 29) | (state[3] << 3);
		t3 = (state[3] >> 10) | (state[4] << 22);
		t4 = (state[3] >> 31) | (state[4] << 1);
		state[5] ^= t0 ^ state[0] ^ t1 ^ (~(t2 & t3)) ^ t4;

		t0 = (state[6] >> 19) | (state[7] << 13);
		t1 = (state[2] >> 3)  | (state[3] << 29);
		t2 = (state[3] >> 29) | (state[4] << 3);
		t3 = (state[4] >> 10) | (state[5] << 22);
		t4 = (state[4] >> 31) | (state[5] << 1);
		state[6] ^= t0 ^ state[1] ^ t1 ^ (~(t2 & t3)) ^ t4;

		t0 = (state[7] >> 19) | (state[8] << 13);
		t1 = (state[3] >> 3)  | (state[4] << 29);
		t2 = (state[4] >> 29) | (state[5] << 3);
		t3 = (state[5] >> 10) | (state[6] << 22);
		t4 = (state[5] >> 31) | (state[6] << 1);
		state[7] ^= t0 ^ state[2] ^ t1 ^ (~(t2 & t3)) ^ t4;

		temp = state[8]; 
		for (j = 8; j >= 1; j--)  state[j] = state[j - 1];
		state[0] = temp; 
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

	// initialize the state as constants   
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

