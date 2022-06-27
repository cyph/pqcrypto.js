/* Reference implementation of ACE-128, AEAD
 Written by:
 Kalikinkar Mandal <kmandal@uwaterloo.ca>
 */

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>

#include "ace.h"
#include "clock_cycle.h"

#define NUM_ITER        2000
#define NUM_TEST      	500


void print_state ( u32 *state )
{
        u8 i, j;
        
        for ( j = 0; j < 8*PARAL_INST_BY8; j++ )
        {
                for ( i = 0; i < STATEDWORD; i++ )
                        printf("%.8X", state[i+j*STATEDWORD]);
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

	u32 *plaintext, *ciphertext, *tag, *key, *nonce, *ad;
	u32 plen, tlen, klen, adlen;
	u8 *k, *pubn;

        num_parallel_inst = 8*PARAL_INST_BY8;
	adlen = 4; // Associated data length = adlen*32;
	plen = 32; // Message length = plen*32;
	tlen = 4; // 128 = 32*4 bits
	klen = 4; // 128 = 32*4 bits

	key = (u32 *)malloc(sizeof(u32)*klen*num_parallel_inst);
	nonce = (u32 *)malloc(sizeof(u32)*klen*num_parallel_inst);
	tag = (u32 *)malloc(sizeof(u32)*tlen*num_parallel_inst);
	ad = (u32 *)malloc(sizeof(u32)*adlen*num_parallel_inst);

	plaintext = (u32 *)malloc(sizeof(u32)*plen*num_parallel_inst);
	ciphertext = (u32 *)malloc(sizeof(u32)*plen*num_parallel_inst);
        
	k = (u8 *)malloc(sizeof(u8)*16*num_parallel_inst);
	pubn = (u8 *)malloc(sizeof(u8)*16*num_parallel_inst);
        
        state = (u32 *)malloc(sizeof(u32)*num_parallel_inst*STATEDWORD);
	
        //Randomly initilizes messages to states
        for ( i = 0; i < num_parallel_inst*STATEDWORD; i++ )
                *(state+i) = i%STATEDWORD;
                //*(state+i) = 0x01;
        
        print_state(state);
        ace320( state );
        //ace320( state );
        print_state(state);
        //exit(0);
       
        //Assinging 128-bit keys, nonces and messages//
	for ( i = 0; i < num_parallel_inst*klen; i++ )
	{
		*(nonce+i) = 0x40404040;
                *(key+i) = 0x10101010;
	}
	for ( i = 0; i < num_parallel_inst; i++ )
        {
                for ( j = 0; j < plen; j++ )
                        *(plaintext+plen*i+j) = j%plen;
        }
        
	for ( i = 0; i < num_parallel_inst*adlen; i++ )
                *(ad+i) = 0xffffffff;

	for ( i = 0; i < num_parallel_inst*16; i++ )
	{
		k[i] = 0x0;
                *(pubn+i) = 0;
	}
        
        for ( i = 0; i < NUM_ITER+1; i++ )
        {
                count_cc = start_rdtsc();
                for ( j = 0; j < NUM_TEST; j++ )
                        ace320( state );
                count_cc = end_rdtsc()-count_cc;
                printf("Cycles per byte = %f\n", (double)(count_cc)/(double)(num_parallel_inst*STATEBYTES*NUM_TEST));
        }
        
        //print_state(state);
        printf("\n");
		
	//===================================================================================================================
				// AEAD: Encryption and Decryption Module//
	//===================================================================================================================
	
	printf("Nonce and Key:\n");
	for ( i = 0; i < num_parallel_inst; i++ )
	{
		printf( "%08X%08X%08X%08X", nonce[4*i+0], nonce[4*i+1], nonce[4*i+2], nonce[4*i+3]);
		printf( "%08X%08X%08X%08X\n", key[4*i+0], key[4*i+1], key[4*i+2], key[4*i+3]);
	}

        for ( i = 0; i < NUM_ITER; i++ )
        {
                count_cc = start_rdtsc();
                crypto_aead_encrypt( tag, tlen, ciphertext, plaintext, plen, ad, adlen, k, pubn, 16 );
                count_cc = end_rdtsc()-count_cc;
                printf("Encryption speed = %f cpb\n", (double)(count_cc)/(double)(num_parallel_inst*plen*4));
        }
        
        
        //Checking Correctness of Autheticated encryption and Decryption
        
        crypto_aead_encrypt( tag, tlen, ciphertext, plaintext, plen, ad, adlen, k, pubn, 16 );
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
	
	crypto_aead_decrypt( plaintext, ciphertext, plen, tag, tlen, ad, adlen, k, pubn, 16 );

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
	
        free(state);
	free(plaintext);
	free(ciphertext);
	free(tag);
	free(key);
	free(nonce);
	free(ad);

return(0);
}
