
#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<stdint.h>

#include "sycon.h"
#include "crypto_hash.h" 
#include "api.h" 

#define KAT_SUCCESS          0
#define KAT_FILE_OPEN_ERROR -1
#define KAT_DATA_ERROR      -3
#define KAT_CRYPTO_FAILURE  -4

#define NUMRATEBYTES    8

int sycon_load_iv( unsigned char *state )
{
	unsigned char i;

	//Set state to all-ZERO
	for ( i = 0; i < STATEBYTES; i++ )
		state[i] = 0x0;
	if ( CRYPTO_BYTES == 32 )
	{
                //Initialize state with IV 1c0a80d42c6e63c5
		state[16] = 0xC5;
		state[17] = 0x63;
		state[18] = 0x6E;
                state[19] = 0x2C;
                state[20] = 0xD4;
                state[21] = 0x80;
                state[22] = 0x0A;
                state[23] = 0x1C;
		sycon_perm(state);
	}
	else
	{
		return KAT_CRYPTO_FAILURE;
	}
return KAT_SUCCESS;
}

int crypto_hash( unsigned char *out, const unsigned char *in, unsigned long long inlen )
{
	unsigned char *state;
	unsigned char i, len_last_block;
	u64 j, num_inp_block;

	num_inp_block = inlen/NUMRATEBYTES;
	len_last_block = (unsigned char)(inlen%NUMRATEBYTES);

	state = (unsigned char *)malloc(sizeof(unsigned char)*STATEBYTES);

	//Initialize state with predefined IV.
	if ( sycon_load_iv(state)!= KAT_SUCCESS )
		return(KAT_CRYPTO_FAILURE);

	if ( inlen != 0 )
	{

		for ( j = 0; j < num_inp_block; j++ )
		{
			for ( i = 0; i < NUMRATEBYTES; i++ )
				state[i]^=in[NUMRATEBYTES*j+((u64)i)];
			sycon_perm(state);
		}

		if ( len_last_block != 0 )
		{
			for ( i = 0; i < len_last_block; i++ )
				state[i]^= in[num_inp_block*NUMRATEBYTES+(u64)i];
			
			state[len_last_block]^=(0x01); //Padding: 10*
			sycon_perm(state);
		}
		else
		{
			state[0]^=(0x01); //Padding: 10*
			sycon_perm(state);
		}
	}
	else
	{
		state[0]^=(0x01); //Padding: 10*
		sycon_perm(state);
	}
	//Extracting message digest
	if ( CRYPTO_BYTES == 32 )
	{
		for ( i = 0; i < NUMRATEBYTES; i++ )
			out[i] = state[i];
		sycon_perm(state);
		for ( i = 0; i < NUMRATEBYTES; i++ )
			out[i+NUMRATEBYTES] = state[i];
		sycon_perm(state);
		for ( i = 0; i < NUMRATEBYTES; i++ )
			out[i+2*NUMRATEBYTES] = state[i];
		sycon_perm(state);
		for ( i = 0; i < NUMRATEBYTES; i++ )
			out[i+3*NUMRATEBYTES] = state[i];
	}
	else
		out=NULL;
free(state);

return KAT_SUCCESS;
}
