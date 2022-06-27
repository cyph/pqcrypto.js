
/*Bit-slice Implementation of sycon-hash-256
   Written by:
   Kalikinkar Mandal <kmandal@uwaterloo.ca>
*/

#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#include "sycon.h"
#include "clock_cycle.h"

#define NUM_ITER        1000
#define NUM_TEST      	500
#define NUMINSTANCE     1

void print_state ( u32 *state )
{
        u8 i, j;
        
        for ( j = 0; j < NUMINSTANCE*2; j++ )
        {
                for ( i = 0; i < STATE_SIZE; i++ )
                        printf("%.8X ", state[j*STATE_SIZE+i]);
                printf("\n");
        }
        printf("\n");
        return;
}

int main()
{
        u8 num_parallel_inst;
        int i, j;
        u64 t[NUM_ITER+1], count_cc;
        
        u32 *plaintext, *digest;
        u64 plen;
        u32 hlen;
        
        num_parallel_inst = 2*NUMINSTANCE;
        plen = 64; // Message length = plen*32;
        hlen = 8; //256 = 32*8 bits
        
        digest = (u32 *)malloc(sizeof(u32)*hlen*num_parallel_inst);
        
        plaintext = (u32 *)malloc(sizeof(u32)*plen*num_parallel_inst);
        
        
        for ( i = 0; i < num_parallel_inst; i++ )
        {
                for ( j = 0; j < plen; j++ )
                        plaintext[i*plen+j] = j%128;
        }
        
        
        //===================================================================================================================
        // 						Sycon-HASH-256
        //===================================================================================================================
        
        for ( i = 0; i < NUM_ITER; i++ ){
                count_cc = start_rdtsc();
                crypto_hash ( digest, plaintext, plen );
                count_cc = end_rdtsc()-count_cc;
		if ( plen != 0 )
                	printf("Hash speed = %f cpb\n", (double)(count_cc)/(double)(num_parallel_inst*plen*4));
		else
                	printf("Hash speed = %f cpb\n", (double)(count_cc)/(double)(num_parallel_inst*4));
        }
        
        printf("Original plaintext:\n");
        for ( i = 0; i < num_parallel_inst; i++ )
        {
                for ( j = 0; j < plen; j++ )
                        printf("%08X", plaintext[i*plen+j]);
                printf("\n");
        }
        crypto_hash ( digest, plaintext, plen );
        
        printf("Digest:\n");
        for ( i = 0; i < num_parallel_inst; i++ )
        {
                for ( j = 0; j < hlen; j++ )
                        printf("%08X", digest[i*hlen+j]);
                printf("\n");
        }
        
        printf("Hash speed = %f cbp\n", (double)(count_cc)/(double)(num_parallel_inst*plen*4));
        
        
        free(plaintext);
        free(digest);
        return(0);
}
