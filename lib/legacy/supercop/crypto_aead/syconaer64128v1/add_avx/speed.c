
 /*Bit-slice Implementation of sycon-ae128-r64
   Written by:
   Kalikinkar Mandal <kmandal@uwaterloo.ca>
   */
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>

#include "sycon.h"
#include "clock_cycle.h"

#define NUM_ITER        1000
#define NUM_TEST      	500


void print_state ( u32 *state )
{
        u8 i, j;
        
        for ( j = 0; j < NUMINSTANCE*4; j++ )
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
        u32 *state;
        int i, j;
        double cpb_val, tmp;
        
        clock_t k1, k2;
        
        u64 t[NUM_ITER+1];
        u64 count_cc;
        
        num_parallel_inst = 4*NUMINSTANCE;
        state = (u32 *)malloc(sizeof(u32)*num_parallel_inst*STATE_SIZE);
        
        u32 *plaintext, *ciphertext, *tag, *ad;
        u32 plen, tlen, klen, adlen;
        u8 *k, *pubn;
        
        adlen = 4; // Associated data length = adlen*32;
        plen = 64; // Message length = plen*32;
        tlen = 4; // 128 = 32*4 bits
        klen = 4; // 128 = 32*4 bits
        
        tag = (u32 *)malloc(sizeof(u32)*tlen*num_parallel_inst);
        ad = (u32 *)malloc(sizeof(u32)*adlen*num_parallel_inst);
        
        plaintext = (u32 *)malloc(sizeof(u32)*plen*num_parallel_inst);
        ciphertext = (u32 *)malloc(sizeof(u32)*plen*num_parallel_inst);
        
        k = (u8 *)malloc(sizeof(u8)*16*num_parallel_inst);
        pubn = (u8 *)malloc(sizeof(u8)*16*num_parallel_inst);
        
        for ( i = 0; i < num_parallel_inst*STATE_SIZE; i++ )
                *(state+i) = 0x0;
        
        print_state(state);
        sycon( state );
        print_state(state);
        //exit(0);
        
        //Assinging messages
        for ( i = 0; i < num_parallel_inst; i++ )
        {
                for ( j = 0; j < plen; j++ )
                        *(plaintext+i*plen+j) = j%plen;
        }
        
        for ( i = 0; i < num_parallel_inst*adlen; i++ )
                *(ad+i) = i%adlen;
        
        for ( i = 0; i < num_parallel_inst*16; i++ )
        {
                *(k+i) = i%16;
                *(pubn+i) = i%16;
        }
        
        
         for ( i = 0; i < NUM_ITER+1; i++ )
         {
                 count_cc = start_rdtsc();
                 for ( j = 0; j < NUM_TEST; j++ )
                         sycon ( state );
                 count_cc = end_rdtsc()-count_cc;
                 printf("Cycles per byte = %f cpb\n", (double)(count_cc)/(double)(num_parallel_inst*40*NUM_TEST));
         }
         //print_state(state);
         //printf("\n");
        
        //AEAD: Encryption and Decryption Module//
        
        for ( i = 0; i < NUM_ITER; i++ )
        {
                count_cc = start_rdtsc();
                crypto_aead_encrypt( tag, tlen, ciphertext, plaintext, plen, ad, adlen, k, pubn, 16 );
                count_cc = end_rdtsc()-count_cc;
                printf("Encryption speed = %f cpb\n", (double)(count_cc)/(double)(num_parallel_inst*plen*4));
        }
        
        
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
        free(ad);
        
        return(0);
}
