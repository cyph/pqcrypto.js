/* Reference Implementation of sycon-320 permutation
   Written by:
   Kalikinkar Mandal <kmandal@uwaterloo.ca>
*/

#include<stdio.h>
#include<stdlib.h>
#include "sycon.h"


static const unsigned char RC[14]={0x15,0x1a,0x1d,0x0e,0x17,0x1b,0x0d,0x06,0x03,0x11,0x18,0x1c,0x1e,0x1f};

unsigned char rotl8 ( const unsigned char x, const unsigned char y, const unsigned char shift )
{
        return ((x>>shift)|(y<<(8-shift)));
}

/***********************************************************
  ******* Sycon permutation implementation *****************
  *********************************************************/

void print_sycon_state( const unsigned char *state )
{
	unsigned char i;
	for ( i = 0; i < STATEBYTES; i++ )
        {
                printf("%02X", state[i]);
        }
	printf("\n");
}

void left_cyclic_shift64 ( unsigned char *tmp, unsigned char rot_const )
{
        unsigned char i, u, v, t[8];
        u = rot_const/8;
        v = rot_const%8;
        
        for ( i = 0; i < 8; i++ )
                t[i] = tmp[(i+u)%8];
        if ( v != 0 )
        {
                for ( i = 0; i < 8; i++ )
                        tmp[i] = rotl8 ( t[i], t[(i+1)%8], v );
        }
        else
        {
                for ( i = 0; i < 8; i++ )
                        tmp[i] = t[i];
        }
        
}

void left_cyclic_shift16 ( unsigned char *a, unsigned char rotc )
{
        unsigned char i, u, v, t, tmp[8];
        u = rotc/8;
        v = rotc%8;
        
        if ( rotc == 0 )
                return;
        
        for ( i = 0; i < 4; i++ )
        {
                if ( v != 0 && u >= 1 )
                {
                        tmp[2*i] = rotl8 ( a[2*i+1], a[2*i], v );
                        tmp[2*i+1] = rotl8 ( a[2*i], a[2*i+1], v );
                }
                else if ( v != 0 && u < 1 )
                {
                        tmp[2*i] = rotl8 ( a[2*i], a[2*i+1], v );
                        tmp[2*i+1] = rotl8 ( a[2*i+1], a[2*i], v );
                }
                else
                {
                        t = tmp[2*i];
                        tmp[2*i] = tmp[2*i+1];
                        tmp[2*i+1] = t;
                }
        }
        for ( i = 0; i < 8; i++ )
                a[i] = tmp[i];
        return;
}

void shuffle_bytes1( unsigned char *a )
{
        unsigned char i, tmp[8];
        unsigned char shuffle[8] = {7, 0, 3, 5, 4, 6, 2, 1};
        for ( i = 0; i < 8; i++ )
                tmp[i] = a[i];
        
        for ( i = 0; i < 8; i++ )
                a[i] = tmp[shuffle[i]];
        return;
}
void shuffle_bytes2( unsigned char *a )
{
        unsigned char i, tmp[8];
        unsigned char shuffle[8] = {0, 6, 1, 7, 3, 4, 2, 5};
        for ( i = 0; i < 8; i++ )
                tmp[i] = a[i];
        for ( i = 0; i < 8; i++ )
                a[i] = tmp[shuffle[i]];
        return;
}
void shuffle_bytes3( unsigned char *a )
{
        unsigned char i, tmp[8];
        unsigned char shuffle[8] = {7, 2, 4, 5, 1, 0, 6, 3};
        for ( i = 0; i < 8; i++ )
                tmp[i] = a[i];
        for ( i = 0; i < 8; i++ )
                a[i] = tmp[shuffle[i]];
        return;
}
void shuffle_bytes4( unsigned char *a )
{
        unsigned char i, tmp[8];
        unsigned char shuffle[8] = {2, 4, 5, 3, 0, 7, 6, 1};
        for ( i = 0; i < 8; i++ )
                tmp[i] = a[i];
        for ( i = 0; i < 8; i++ )
                a[i] = tmp[shuffle[i]];
        return;
}
void shuffle_bytes5( unsigned char *a )
{
        unsigned char i, tmp[8];
        unsigned char shuffle[8] = {3, 6, 1, 0, 5, 7, 2, 4};
        for ( i = 0; i < 8; i++ )
                tmp[i] = a[i];
        for ( i = 0; i < 8; i++ )
                a[i] = tmp[shuffle[i]];
        return;
}


void sycon_perm( unsigned char *input )
{
	unsigned char i, j;
        unsigned char t[5][8];
	
        for ( i = 0; i < NUMROUNDS; i++ )
        {
                //Sbox layer
                for ( j = 0; j < 8; j++ )
                        t[0][j] = input[8*2+j]^input[8*4+j];
                for ( j = 0; j < 8; j++ )
                        t[1][j] = t[0][j]^input[8*1+j];
                for ( j = 0; j < 8; j++ )
                        t[2][j] = input[8*1+j]^input[8*3+j];
                for ( j = 0; j < 8; j++ )
                        t[3][j] = input[j]^input[8*4+j];
                for ( j = 0; j < 8; j++ )
                        t[4][j] = t[3][j]^(t[1][j]&input[8*3+j]);
                for ( j = 0; j < 8; j++ )
                        input[8*1+j] = ((~input[8*1+j])&input[8*3+j])^t[1][j]^((~t[2][j])&input[j]);
                for ( j = 0; j < 8; j++ )
                        t[1][j] = ((~t[3][j])&input[8*2+j])^t[2][j];
                for ( j = 0; j < 8; j++ )
                        input[8*3+j] = ((~t[0][j])&input[8*3+j])^input[j]^(~input[8*2+j]);
                for ( j = 0; j < 8; j++ )
                        input[8*4+j] = ((~input[8*4+j])&input[j])^t[2][j];
                for ( j = 0; j < 8; j++ )
                        input[j] = t[4][j];
                for ( j = 0; j < 8; j++ )
                        input[8*2+j] = t[1][j];
                
                //Diffusion Layer
                for ( j = 0; j < 8; j++ )
                        t[0][j] = input[j];
                for ( j = 0; j < 8; j++ )
                        t[1][j] = input[j];
                left_cyclic_shift64 (t[0], 11);
                left_cyclic_shift64 (t[1], 22);
                for ( j = 0; j < 8; j++ )
                        input[j] = input[j]^t[0][j]^t[1][j];
                
                for ( j = 0; j < 8; j++ )
                        t[0][j] = input[8*1+j];
                for ( j = 0; j < 8; j++ )
                        t[1][j] = input[8*1+j];
                left_cyclic_shift64 (t[0], 13);
                left_cyclic_shift64 (t[1], 26);
                for ( j = 0; j < 8; j++ )
                        input[8*1+j] = input[8*1+j]^t[0][j]^t[1][j];
                
                for ( j = 0; j < 8; j++ )
                        t[0][j] = input[8*2+j];
                for ( j = 0; j < 8; j++ )
                        t[1][j] = input[8*2+j];
                left_cyclic_shift64 (t[0], 31);
                left_cyclic_shift64 (t[1], 62);
                for ( j = 0; j < 8; j++ )
                        input[8*2+j] = input[8*2+j]^t[0][j]^t[1][j];
                
                for ( j = 0; j < 8; j++ )
                        t[0][j] = input[8*3+j];
                for ( j = 0; j < 8; j++ )
                        t[1][j] = input[8*3+j];
                left_cyclic_shift64 (t[0], 56);
                left_cyclic_shift64 (t[1], 60);
                for ( j = 0; j < 8; j++ )
                        input[8*3+j] = input[8*3+j]^t[0][j]^t[1][j];
                
                for ( j = 0; j < 8; j++ )
                        t[0][j] = input[8*4+j];
                for ( j = 0; j < 8; j++ )
                        t[1][j] = input[8*4+j];
                left_cyclic_shift64 (t[0], 6);
                left_cyclic_shift64 (t[1], 12);
                for ( j = 0; j < 8; j++ )
                        input[8*4+j] = input[8*4+j]^t[0][j]^t[1][j];
                
                //AddRoundConst
                input[8*2] = input[8*2]^RC[i];
                for ( j = 1; j < 8; j++ )
                        input[8*2+j] = input[8*2+j]^(0xaa);
                
                //FIST permutation
                for ( j = 0; j < 8; j++ )
                        t[0][j] = input[j];
                
                left_cyclic_shift16 ( t[0], 11);
                shuffle_bytes1(t[0]);
                for ( j = 0; j < 8; j++ )
                        input[j] = t[0][j];
                
                for ( j = 0; j < 8; j++ )
                        t[0][j] = input[8*1+j];
                left_cyclic_shift16 ( t[0], 4);
                shuffle_bytes2(t[0]);
                for ( j = 0; j < 8; j++ )
                        input[8*1+j] = t[0][j];
                
                for ( j = 0; j < 8; j++ )
                        t[0][j] = input[8*2+j];
                left_cyclic_shift16 ( t[0], 10);
                shuffle_bytes3(t[0]);
                for ( j = 0; j < 8; j++ )
                        input[8*2+j] = t[0][j];
                
                for ( j = 0; j < 8; j++ )
                        t[0][j] = input[8*3+j];
                left_cyclic_shift16 ( t[0], 7 );
                shuffle_bytes4(t[0]);
                for ( j = 0; j < 8; j++ )
                        input[8*3+j] = t[0][j];
                
                for ( j = 0; j < 8; j++ )
                        t[0][j] = input[8*4+j];
                left_cyclic_shift16 ( t[0], 5);
                shuffle_bytes5(t[0]);
                for ( j = 0; j < 8; j++ )
                        input[8*4+j] = t[0][j];
                //print_sycon_state(input);
        }
return;
}

void sycon_perm_ALLZERO ( unsigned char *state )
{
	unsigned char i;
        
	for ( i = 0; i < STATEBYTES; i++ )
		state[i] = 0x0;
        print_sycon_state(state);
	sycon_perm(state);
        print_sycon_state(state);
return;
}

void sycon_perm_ALLONE ( unsigned char *state )
{
	unsigned char i;
	
	for ( i = 0; i < STATEBYTES; i++ )
		state[i] = 0xff;
	sycon_perm(state);
return;
}

