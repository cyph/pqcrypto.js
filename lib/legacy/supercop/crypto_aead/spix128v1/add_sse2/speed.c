/* Optimized (Intel Intrinsics) implementation of SPIX-128 AEAD
   Written by:
   Kalikinkar Mandal <kmandal@uwaterloo.ca>
*/

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>

#include "spix.h"
#include "clock_cycle.h"

#define NUM_ITER        2000
#define NUM_TEST      	500


void print_state ( u32 *state )
{
        u8 i, j;
        
        for ( j = 0; j < 2*PARAL_INST_BY2; j++ )
        {
                for ( i = 0; i < STATE_SIZE; i++ )
                        printf("%.8X", state[i+j*STATE_SIZE]);
                printf("\n");
        }
        return;
}

int main()
{
        u8 num_parallel_inst;
        u32 *state;
        int i, j;
        u64 t[NUM_ITER+1], count_cc;

	u32 *plaintext, *ciphertext, *tag, *ad;
	u8 *key, *nonce;
	u32 plen, tlen, klen, adlen;


        num_parallel_inst = 2*PARAL_INST_BY2;
	adlen = 4; // Associated data length = adlen*32;
	plen = 32; // Message length = plen*32;
	tlen = 4; // 128 = 32*4 bits
	klen = 16; // 128 = 16*8 bits

	key = (u8 *)malloc(sizeof(u8)*klen*num_parallel_inst);
	nonce = (u8 *)malloc(sizeof(u8)*klen*num_parallel_inst);

	tag = (u32 *)malloc(sizeof(u32)*tlen*num_parallel_inst);
	ad = (u32 *)malloc(sizeof(u32)*adlen*num_parallel_inst);

	plaintext = (u32 *)malloc(sizeof(u32)*plen*num_parallel_inst);
	ciphertext = (u32 *)malloc(sizeof(u32)*plen*num_parallel_inst);
        
        
        state = (u32 *)malloc(sizeof(u32)*num_parallel_inst*STATE_SIZE);
       
	//Randomly initilizes messages to states
        for ( i = 0; i < num_parallel_inst*STATE_SIZE; i++ )
                *(state+i) = i%8;
        
        //print_state(state);
        sliscp_light256 ( state );
        //print_state(state);
        
        //Assinging 128-bit keys, nonces and messages//
	for ( i = 0; i < num_parallel_inst*klen; i++ )
	{
		*(nonce+i) = 0x0;
                *(key+i) = 0x0;
	}
	for ( i = 0; i < num_parallel_inst*plen; i++ )
                *(plaintext+i) = i%plen;
	
	for ( i = 0; i < num_parallel_inst*adlen; i++ )
                *(ad+i) = i%adlen;

        for ( i = 0; i < NUM_ITER+1; i++ )
        {
                count_cc = start_rdtsc();
                for ( j = 0; j < NUM_TEST; j++ )
                        sliscp_light256 ( state );
                count_cc = end_rdtsc()-count_cc;
                printf("Cycles per byte for sliscp_light_r18 = %f\n", (double)(count_cc)/(double)(num_parallel_inst*32*NUM_TEST));
        }
	//===================================================================================================================
				// AEAD: Encryption and Decryption Module//
	//===================================================================================================================

	printf("Nonce and Key:\n");
	for ( i = 0; i < num_parallel_inst; i++ )
	{
		printf( "%08X%08X%08X%08X", nonce[4*i+0], nonce[4*i+1], nonce[4*i+2], nonce[4*i+3]);
		printf( "%08X%08X%08X%08X\n", key[4*i+0], key[4*i+1], key[4*i+2], key[4*i+3]);
	}

        count_cc = start_rdtsc();
	crypto_aead_encrypt( tag, tlen, ciphertext, plaintext, plen, ad, adlen, nonce, key);
	count_cc = end_rdtsc()-count_cc;

	printf("Original plaintext:\n");
	for ( i = 0; i < num_parallel_inst; i++ )
	{
		for ( j = 0; j < plen; j++ )
			printf("%08X", plaintext[i*plen+j]);
		printf("\n");
	}
	printf("Ciphertext:\n");
	for ( i = 0; i < num_parallel_inst; i++ )
	{
		for ( j = 0; j < plen; j++ )
			printf("%08X", ciphertext[i*plen+j]);
		printf("\n");
	}
	printf("Tag:\n");
	for ( i = 0; i < num_parallel_inst; i++ )
	{
		for ( j = 0; j < tlen; j++ )
			printf("%08X", tag[i*tlen+j]);
		printf("\n");
	}

	crypto_aead_decrypt( plaintext, ciphertext, plen, tag, tlen, ad, adlen, nonce, key);

	printf("Decrypted plaintext:\n");
	for ( i = 0; i < num_parallel_inst; i++ )
	{
		for ( j = 0; j < plen; j++ )
			printf("%08X", plaintext[i*plen+j]);
		printf("\n");
	}
	
	printf("Tag:\n");
	for ( i = 0; i < num_parallel_inst; i++ )
	{
		for ( j = 0; j < tlen; j++ )
			printf("%08X", tag[i*tlen+j]);
		printf("\n");
	}
	printf("Encryption speed = %f cpb\n", (double)(count_cc)/(double)(num_parallel_inst*plen*4));
	

        free(state);
	free(plaintext);
	free(ciphertext);
	free(tag);
	free(key);
	free(nonce);
	free(ad);

return(0);
}
