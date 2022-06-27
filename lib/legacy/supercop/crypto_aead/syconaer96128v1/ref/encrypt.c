

/* Reference Implementation of Sycon-128 AEAD r96
   Written by:
   Kalikinkar Mandal <kmandal@uwaterloo.ca>
*/

#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<stdint.h>

#define NUMRATEBYTES    12

#define ADDS	0x80
#define PTDS	0x40
#define TDS     0x20

#include "sycon.h"
#include "crypto_aead.h" 
#include "api.h" 

#define KAT_SUCCESS          0
#define KAT_FILE_OPEN_ERROR -1
#define KAT_DATA_ERROR      -3
#define KAT_CRYPTO_FAILURE  -4

int sycon_load( unsigned char *state, const unsigned char *npub, const unsigned char *k )
{
	unsigned char i;
        //Set IV to: 0x5980A92AFC5D9D2C
        unsigned char iv1[8] = {0x2C, 0x9D, 0x5D, 0xFC, 0x2A, 0xA9, 0x80, 0x59};

	for ( i = 0; i < STATEBYTES; i++ )
		state[i] = 0x0;
		
	if ( CRYPTO_KEYBYTES == 16 && CRYPTO_NPUBBYTES == 16 )
	{
		//Load key at state[0...15]
		for ( i = 0; i < 16; i++ )
			state[i] = k[i];

		//Assigning nonce at state[16...31]
		for ( i = 0; i < 16; i++ )
			state[16+i] = npub[i];
                //Assigning IV to state[32...39]
                for ( i = 0; i < 8; i++ )
                        state[i+32] = iv1[i];
		
		sycon_perm(state);

		for ( i = 0; i < NUMRATEBYTES; i++ )
			state[i]^=k[i];
		sycon_perm(state);
                
		for ( i = 0; i < 4; i++ )
			state[i]^=k[NUMRATEBYTES+i];
                //Padding key: 10*
                state[4]^=(0x01);

                //Domain separator
                state[STATEBYTES-1]^=ADDS;
		sycon_perm(state);
	}
	else
	{
		return KAT_CRYPTO_FAILURE;
	}
return KAT_SUCCESS;
}


int sycon_ad ( unsigned char *state, const unsigned char *ad, const u64 adlen )
{
	unsigned char i, last_block_len;
	u64 j, num_ad_block;
	num_ad_block = adlen/NUMRATEBYTES;
	last_block_len = (unsigned char)(adlen%NUMRATEBYTES);

	if ( adlen == 0 )
		return(KAT_SUCCESS);
	
	//Processing associated data
	for ( j = 0; j < num_ad_block; j++ )
	{
		for ( i = 0; i < NUMRATEBYTES; i++ )
			state[i]^=ad[NUMRATEBYTES*j+((u64)i)];
		//Domain separator
                state[STATEBYTES-1]^=ADDS;
		sycon_perm_r96(state);
	}

	//Processing the last block
	if ( last_block_len != 0 )
	{
		for ( i = 0; i < last_block_len; i++ )
			state[i]^=ad[num_ad_block*NUMRATEBYTES+(u64)i];

		state[last_block_len]^=(0x01); //Padding: 10*
		//Domain separator
		state[STATEBYTES-1]^=PTDS;
		sycon_perm_r96(state );
	}
	else
	{
		state[0]^=(0x01); //Padding: 10*
                //Domain separator
                state[STATEBYTES-1]^=PTDS;
		sycon_perm_r96(state);
	}

return (KAT_SUCCESS);
}

int sycon_tag ( unsigned char *tag, const unsigned char tlen, unsigned char *state, const unsigned char *k )
{
        unsigned char i;
        if ( CRYPTO_KEYBYTES == 16 && tlen == 16 )
        {
                for ( i = 0; i < NUMRATEBYTES; i++ )
                        state[i]^=k[i];

                //Domain separator
                state[STATEBYTES-1]^=TDS;
                sycon_perm(state);
                
                for ( i = 0; i < 4; i++ )
                        state[i]^=k[NUMRATEBYTES+i];

                //Padding: 10*
                state[0]^=(0x01);
                //Domain separator
                state[STATEBYTES-1]^=TDS;
                
                sycon_perm(state);
                
                for ( i = 0; i < 16; i++ )
                	tag[i] = state[i+16];
                
        }
        else
        {
                printf("Invalid key and tag length pair.\n");
                return KAT_CRYPTO_FAILURE;
        }
        return KAT_SUCCESS;
}

int crypto_aead_encrypt(
			unsigned char *c,unsigned long long *clen,
			const unsigned char *m,unsigned long long mlen,
			const unsigned char *ad,unsigned long long adlen,
			const unsigned char *nsec,
			const unsigned char *npub,
			const unsigned char *k
			)
{
	unsigned char *state;
	unsigned char *tag;
	unsigned char i, last_block_len;
	//int func_ret;
	u64 j, num_pt_block;

	num_pt_block = mlen/NUMRATEBYTES;
	last_block_len = (unsigned char)(mlen%NUMRATEBYTES);

	state = (unsigned char *)malloc(sizeof(unsigned char)*STATEBYTES);
	tag = (unsigned char *)malloc(sizeof(unsigned char)*CRYPTO_ABYTES);

	//Load the key, nonce, and IV into the state and initialize the state
	if ( sycon_load(state, npub, k)!= KAT_SUCCESS )
		return(KAT_CRYPTO_FAILURE);

	//Processing Associated Data	
        if ( sycon_ad( state, ad, adlen) != KAT_SUCCESS && adlen != 0 )
                        return(KAT_CRYPTO_FAILURE);

	//Processing Plaintext
        if ( mlen != 0 )
        {
                for ( j = 0; j < num_pt_block; j++ )
                {
                        for ( i = 0; i < NUMRATEBYTES; i++ )
                        {
                                c[NUMRATEBYTES*j+((u64)i)] = m[NUMRATEBYTES*j+((u64)i)]^state[i];
                                state[i] = c[NUMRATEBYTES*j+((u64)i)];
                        }
                        //Domain seperator
                        state[STATEBYTES-1]^=PTDS;
                
                        sycon_perm_r96(state);
                }

                if ( last_block_len != 0 )
                {
                        for ( i = 0; i < last_block_len; i++ )
                        {
                                c[NUMRATEBYTES*num_pt_block+((u64)i)] = m[num_pt_block*NUMRATEBYTES+(u64)i]^state[i];
                                state[i] = c[NUMRATEBYTES*num_pt_block+((u64)i)];
                        }
			state[last_block_len]^=(0x01); //Padding: 10*

                        //Domain separator
                        state[STATEBYTES-1]^=TDS;
                        sycon_perm_r96(state);
                }
		else
		{
			state[0]^=(0x01); //Padding: 10*
                        //Domain separator
                        state[STATEBYTES-1]^=TDS;
			sycon_perm_r96(state);
		}
        }
        else
	{
		state[0]^=(0x01); //Padding: 10*
                //Domain separator
                state[STATEBYTES-1]^=TDS;
		sycon_perm_r96(state);
	}
	
	if ( sycon_tag( tag, CRYPTO_ABYTES, state, k ) != KAT_SUCCESS )
		return(KAT_CRYPTO_FAILURE);
        else
        {
                for ( i = 0; i < CRYPTO_ABYTES; i++ )
                        c[mlen+(u64)i] = tag[i];
        }
	*clen = mlen+CRYPTO_ABYTES;
        
	free(state);
	free(tag);
return KAT_SUCCESS;
}

int crypto_aead_decrypt(
			unsigned char *m,unsigned long long *mlen,
			unsigned char *nsec,
			const unsigned char *c,unsigned long long clen,
			const unsigned char *ad,unsigned long long adlen,
			const unsigned char *npub,
			const unsigned char *k
			)
{
	unsigned char i, last_block_len;
	u64 j, num_ct_block1, num_ct_block;
        num_ct_block1 = clen - CRYPTO_ABYTES;
        num_ct_block = num_ct_block1/NUMRATEBYTES;
	last_block_len = (unsigned char)(num_ct_block1%NUMRATEBYTES);
	
	unsigned char *state;
	unsigned char *tag;

	state = (unsigned char *)malloc(sizeof(unsigned char)*STATEBYTES);
	tag = (unsigned char *)malloc(sizeof(unsigned char)*CRYPTO_ABYTES);

	//Initialize state with "key" and "nonce" and then absorbe "key" again
	if ( sycon_load(state, npub, k)!= KAT_SUCCESS )
		return(KAT_CRYPTO_FAILURE);

	//Absorbing "ad"
        if ( adlen != 0 )
        {
                if ( sycon_ad( state, ad, adlen) != KAT_SUCCESS)
                        return(KAT_CRYPTO_FAILURE);
        }

        if ( num_ct_block1 != 0 )
        {
                for ( j = 0; j < num_ct_block; j++ )
                {
                        for ( i = 0; i < NUMRATEBYTES; i++ )
                        {
                                m[NUMRATEBYTES*j+((u64)i)] = c[NUMRATEBYTES*j+((u64)i)]^state[i];
                                state[i] = c[NUMRATEBYTES*j+((u64)i)];
                        }
                        //Domain separator
                        state[STATEBYTES-1]^=PTDS;
                        sycon_perm_r96(state);
                }

                if ( last_block_len != 0 )
                {
                        //Decrypting last 64-bit block
                        for ( i = 0; i < last_block_len; i++ )
                        {
                                m[NUMRATEBYTES*num_ct_block +((u64)i)] = c[NUMRATEBYTES*num_ct_block +((u64)i)]^state[i];
                                state[i] = c[NUMRATEBYTES*num_ct_block +((u64)i)];
                        }
			state[last_block_len]^=(0x01); //Padding: 10*

                        //Domain separator
                        state[STATEBYTES-1]^=TDS;
                        sycon_perm_r96(state);
                }
		else
		{
			state[0]^=(0x01); //Padding: 10*
                        //Domain separator
                        state[STATEBYTES-1]^=TDS;
			sycon_perm_r96(state );
		}
        }
        else
        {
                state[0]^=(0x01); //Padding: 10*
                //Domain separator
                state[STATEBYTES-1]^=TDS;
                sycon_perm_r96(state );
        }
	
	if ( sycon_tag( tag, CRYPTO_ABYTES, state, k ) != KAT_SUCCESS )
		return(KAT_CRYPTO_FAILURE);
        else
        {
                for ( i = 0; i < CRYPTO_ABYTES; i++ )
                {
                        if ( c[num_ct_block1+(u64)i] != tag[i] )
                                return(KAT_CRYPTO_FAILURE);
                }
        }
	*mlen = clen-CRYPTO_ABYTES;

	free(state);
	free(tag);
	
return KAT_SUCCESS;
}
