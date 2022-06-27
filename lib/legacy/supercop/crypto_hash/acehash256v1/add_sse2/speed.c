/* Reference implementation of ACE-Hash256
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
        
        for ( j = 0; j < 4*PARAL_INST_BY4; j++ )
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

	u32 *plaintext, *digest;
	u64 plen;
	u32 hlen;

        num_parallel_inst = 4*PARAL_INST_BY4;
	plen = 32; // Message length = plen*32;
	hlen = 8; //256 = 32*8 bits

	digest = (u32 *)malloc(sizeof(u32)*hlen*num_parallel_inst);

	plaintext = (u32 *)malloc(sizeof(u32)*plen*num_parallel_inst);
        
        state = (u32 *)malloc(sizeof(u32)*num_parallel_inst*STATEDWORD);
       
        
        //Randomly generating messages//
        for ( i = 0; i < num_parallel_inst; i++ )
        {
                for ( j = 0; j < plen; j++ )
                        plaintext[i*plen+j] = j%128;
        }
	
        
	//===================================================================================================================
				// HASH Mode//
	//===================================================================================================================
        
        //Testing speed for ACE-Hash
        for ( i = 0; i < NUM_ITER; i++ )
        {
                count_cc = start_rdtsc();
                crypto_hash ( digest, plaintext, plen );
                count_cc = end_rdtsc()-count_cc;
                printf("Hash speed = %f cpb\n", (double)(count_cc)/(double)(num_parallel_inst*plen*4));
        }
        printf("Hash speed = %f cbp\n", (double)(count_cc)/(double)(num_parallel_inst*plen*4));
        //Conputing hash
        crypto_hash ( digest, plaintext, plen );
	printf("Original plaintext:\n");
	for ( i = 0; i < num_parallel_inst; i++ )
	{
		for ( j = 0; j < plen; j++ )
			printf("%08X", plaintext[i*plen+j]);
		printf("\n");
	}

	printf("Digest:\n");
	for ( i = 0; i < num_parallel_inst; i++ )
	{
		for ( j = 0; j < hlen; j++ )
			printf("%08X", digest[i*hlen+j]);
		printf("\n");
	}
	
	

        free(state);
	free(plaintext);
	free(digest);
return(0);
}
