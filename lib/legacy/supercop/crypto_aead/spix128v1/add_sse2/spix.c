/* Optimized (Intel Intrinsics) implementation of SPIX-128 AEAD
   Written by:
   Kalikinkar Mandal <kmandal@uwaterloo.ca>
*/

#include "spix.h"

void sliscp_light256 ( u32 *state )
{
        u8 i, j, k;
        u32 t1, t2;
        
        __m128i x[4*PARAL_INST_BY2];
        __m128i xtmp, ytmp;
        
        for ( k = 0; k < PARAL_INST_BY2; k++ )
                PACK(x[4*k], x[4*k+1], x[4*k+2], x[4*k+3], state, 16*k, 16*k+4, 16*k+8, 16*k+12);
        
        for ( i = 0; i < STEP_ROUND; i++ )
        {
                
                for ( k = 0; k < PARAL_INST_BY2; k++ )
                {
                        //SSb
                        PACK_SSb(x[4*k+2],x[4*k+3]);
                        for ( j = 0; j < SIMECK_ROUND; j++ )
                        {
                                t1 = (u32)((rc1_256[i] >> j)&1);
                                t2 = (u32)((rc2_256[i] >> j)&1);
                                ROAX(x[4*k+2], x[4*k+3], t1, t2);
                        }
                        UNPACK_SSb(x[4*k+2],x[4*k+3]);
                        //ASc
                        t1 = (u32)RC1_256[i];
                        t2 = (u32)RC2_256[i];
                        xtmp = x[4*k+0]^SC(t1, t2);
                        ytmp = x[4*k+1]^SC(t1, t2);
                        //MSb
                        x[4*k+0] = x[4*k+2];
                        x[4*k+1] = x[4*k+3];
                        x[4*k+2]^=xtmp;
                        x[4*k+3]^=ytmp;
                        x[4*k+2] = SWAPBLK(x[4*k+2]);
                        x[4*k+3] = SWAPBLK(x[4*k+3]);
                }
        }
        //Unpacking state
        for ( k = 0; k < PARAL_INST_BY2; k++)
        {
                xtmp = _mm_unpacklo_epi64(x[4*k+0], x[4*k+2]);
                ytmp = _mm_unpackhi_epi64(x[4*k+0], x[4*k+2]);
                _mm_storeu_si128((void *) (state + 0+16*k),xtmp);
                _mm_storeu_si128((void *) (state + 4 +16*k),ytmp);
                xtmp = _mm_unpacklo_epi64(x[4*k+1], x[4*k+3]);
                ytmp = _mm_unpackhi_epi64(x[4*k+1], x[4*k+3]);
                _mm_storeu_si128((void *) (state + 8+16*k),xtmp);
                _mm_storeu_si128((void *) (state + 12+16*k),ytmp);
        }
        return;
}

void sliscp256( __m128i *x )
{
	__m128i xtmp, ytmp;
	u8 i, j, k;
	u32 t1, t2;

	for ( i = 0; i < STEP_ROUND; i++ )
	{
		for ( k = 0; k < PARAL_INST_BY2; k++ )
		{
			//SSb
        		PACK_SSb(x[4*k+2], x[4*k+3]);
        		for ( j = 0; j < SIMECK_ROUND; j++ )
        		{
				t1 = (u32)((rc1_256[i] >> j)&1);
                		t2 = (u32)((rc2_256[i] >> j)&1);
                		ROAX( x[4*k+2], x[4*k+3], t1, t2);
        		}
        		UNPACK_SSb(x[4*k+2], x[4*k+3]);
        		//ASc
        		t1 = (u32)RC1_256[i];
        		t2 = (u32)RC2_256[i];
        		xtmp = x[4*k+0]^SC(t1, t2);
        		ytmp = x[4*k+1]^SC(t1, t2);
        		//MSb
			x[4*k+0] = x[4*k+2];
			x[4*k+1] = x[4*k+3];
        		x[4*k+2]^=xtmp;
        		x[4*k+3]^=ytmp;
        		x[4*k+2] = SWAPBLK(x[4*k+2]);
        		x[4*k+3] = SWAPBLK(x[4*k+3]);
		}
	}
}

void sliscp256r9( __m128i *x )
{
	__m128i xtmp, ytmp;
	u8 i, j, k;
	u32 t1, t2;

	for ( i = 0; i < 9; i++ )
	{
		for ( k = 0; k < PARAL_INST_BY2; k++ )
		{
			//SSb
        		PACK_SSb(x[4*k+2], x[4*k+3]);
        		for ( j = 0; j < SIMECK_ROUND; j++ )
        		{
				t1 = (u32)((rc1_256[i] >> j)&1);
                		t2 = (u32)((rc2_256[i] >> j)&1);
                		ROAX( x[4*k+2], x[4*k+3], t1, t2);
        		}
        		UNPACK_SSb(x[4*k+2], x[4*k+3]);
        		//ASc
        		t1 = (u32)RC1_256[i];
        		t2 = (u32)RC2_256[i];
        		xtmp = x[4*k+0]^SC(t1, t2);
        		ytmp = x[4*k+1]^SC(t1, t2);
        		//MSb
			x[4*k+0] = x[4*k+2];
			x[4*k+1] = x[4*k+3];
        		x[4*k+2]^=xtmp;
        		x[4*k+3]^=ytmp;
        		x[4*k+2] = SWAPBLK(x[4*k+2]);
        		x[4*k+3] = SWAPBLK(x[4*k+3]);
		}
	}
}


void crypto_aead_encrypt( u32 *tag, u32 tlen, u32 *ciphertext, u32 *plaintext, u32 plen, u32 *ad, u32 adlen, u8 *nonce, u8 *key)
{
        u8 k, i;
        u32 l, mblock, adblock;
        __m128i x[4*PARAL_INST_BY2];
	u32 index1, index2;

	mblock = plen/2;
	adblock = adlen/2;
       
	//Initialization: Loading Key & Nonce
	for ( i = 0; i < PARAL_INST_BY2; i++ )
	{
		x[4*i] = _mm_set_epi8(nonce[32*i+15],nonce[32*i+14],nonce[32*i+13],nonce[32*i+12],nonce[32*i+11],nonce[32*i+10],nonce[32*i+9],nonce[32*i+8],nonce[32*i+7],nonce[32*i+6],nonce[32*i+5],nonce[32*i+4],nonce[32*i+3],nonce[32*i+2],nonce[32*i+1],nonce[32*i+0]);
		x[4*i+1] = _mm_set_epi8(nonce[32*i+31],nonce[32*i+30],nonce[32*i+29],nonce[32*i+28],nonce[32*i+27],nonce[32*i+26],nonce[32*i+25],nonce[32*i+24],nonce[32*i+23],nonce[32*i+22],nonce[32*i+21],nonce[32*i+20],nonce[32*i+19],nonce[32*i+18],nonce[32*i+17],nonce[32*i+16]);
		x[4*i+2] = _mm_set_epi8(key[32*i+15],key[32*i+14],key[32*i+13],key[32*i+12],key[32*i+11],key[32*i+10],key[32*i+9],key[32*i+8],key[32*i+7],key[32*i+6],key[32*i+5],key[32*i+4],key[32*i+3],key[32*i+2],key[32*i+1],key[32*i+0]);
		x[4*i+3] = _mm_set_epi8(key[32*i+31],key[32*i+30],key[32*i+29],key[32*i+28],key[32*i+27],key[32*i+26],key[32*i+25],key[32*i+24],key[32*i+23],key[32*i+22],key[32*i+21],key[32*i+20],key[32*i+19],key[32*i+18],key[32*i+17],key[32*i+16]);
	}
	sliscp256(x);
	for ( i = 0; i < PARAL_INST_BY2; i++ )
	{
		x[4*i+2] = x[4*i+2]^_mm_set_epi8(0x0,0x0,0x0,0x0,key[32*i+7],key[32*i+6],key[32*i+5],key[32*i+4],0x0,0x0,0x0,0x0,key[32*i+3],key[32*i+2],key[32*i+1],key[32*i+0]);
		x[4*i+3] = x[4*i+3]^_mm_set_epi8(0x0,0x0,0x0,0x0,key[32*i+23],key[32*i+22],key[32*i+21],key[32*i+20],0x0,0x0,0x0,0x0,key[32*i+19],key[32*i+18],key[32*i+17],key[32*i+16]);
	}
	sliscp256(x);
	for ( i = 0; i < PARAL_INST_BY2; i++ )
	{
		x[4*i+2] = x[4*i+2]^_mm_set_epi8(0x0,0x0,0x0,0x0,key[32*i+15],key[32*i+14],key[32*i+13],key[32*i+12],0x0,0x0,0x0,0x0,key[32*i+11],key[32*i+10],key[32*i+9],key[32*i+8]);
		x[4*i+3] = x[4*i+3]^_mm_set_epi8(0x0,0x0,0x0,0x0,key[32*i+31],key[32*i+30],key[32*i+29],key[32*i+28],0x0,0x0,0x0,0x0,key[32*i+27],key[32*i+26],key[32*i+25],key[32*i+24]);
	}
	sliscp256(x);
	
	//Proceesing Associated Data
	for ( l = 0; l < adblock; l++ )
	{
		for ( k = 0; k < PARAL_INST_BY2; k++)
		{
			index1 = 2*l + adlen*2*k;
			index2 = 2*l + adlen*2*k+1;
			x[4*k+2] = _mm_insert_epi32(x[4*k+2], _mm_extract_epi32 (x[4*k+2], 0)^ad[index1], 0);
			x[4*k+2] = _mm_insert_epi32(x[4*k+2], _mm_extract_epi32 (x[4*k+2], 2)^ad[ index2], 2);
			index1 = 2*l + adlen*2*k+adlen;
			index2 = 2*l + adlen*2*k+adlen+1;
			x[4*k+3] = _mm_insert_epi32(x[4*k+3], _mm_extract_epi32 (x[4*k+3], 0)^ad[index1], 0);
			x[4*k+3] = _mm_insert_epi32(x[4*k+3], _mm_extract_epi32 (x[4*k+3], 2)^ad[index2], 2);
			//Domain seperator
			x[4*k+2] = _mm_insert_epi32(x[4*k+2], _mm_extract_epi32 (x[4*k+2], 3)^0x00000001, 3 );
			x[4*k+3] = _mm_insert_epi32(x[4*k+3], _mm_extract_epi32 (x[4*k+3], 3)^0x00000001, 3 );
		}
		sliscp256r9(x);
	}
	if ( adlen%2 == 0 )
	{
		for ( k = 0; k < PARAL_INST_BY2; k++)
		{
			//Padding: 10*
			x[4*k+2] = _mm_insert_epi32(x[4*k+2], _mm_extract_epi32 (x[4*k+2], 0)^(0x80000000), 0);
			x[4*k+3] = _mm_insert_epi32(x[4*k+3], _mm_extract_epi32 (x[4*k+3], 0)^(0x80000000), 0);

			//Domain seperator
			x[4*k+2] = _mm_insert_epi32(x[4*k+2], _mm_extract_epi32 (x[4*k+2], 3)^0x00000001, 3 );
			x[4*k+3] = _mm_insert_epi32(x[4*k+3], _mm_extract_epi32 (x[4*k+3], 3)^0x00000001, 3 );
		}
		sliscp256r9(x);
	}
	//Encrypting plaintext
	for ( l = 0; l < mblock; l++ )
	{
		for ( k = 0; k < PARAL_INST_BY2; k++)
		{
			index1 = 2*l + plen*2*k;
			index2 = 2*l + plen*2*k+1;
			ciphertext[index1] = _mm_extract_epi32 (x[4*k+2], 0)^plaintext[index1];
			ciphertext[index2] = _mm_extract_epi32 (x[4*k+2], 2)^plaintext[ index2];
			x[4*k+2] = _mm_insert_epi32(x[4*k+2], ciphertext[index1], 0);
			x[4*k+2] = _mm_insert_epi32(x[4*k+2], ciphertext[index2], 2);
			index1 = 2*l + plen*2*k+plen;
			index2 = 2*l + plen*2*k+plen+1;
			ciphertext[index1] = _mm_extract_epi32 (x[4*k+3], 0)^plaintext[index1];
			ciphertext[index2] = _mm_extract_epi32 (x[4*k+3], 2)^plaintext[index2];
			x[4*k+3] = _mm_insert_epi32(x[4*k+3], ciphertext[index1], 0);
			x[4*k+3] = _mm_insert_epi32(x[4*k+3], ciphertext[index2], 2);
			//Domain seperator
			x[4*k+2] = _mm_insert_epi32(x[4*k+2], _mm_extract_epi32 (x[4*k+2], 3)^0x00000002, 3 );
			x[4*k+3] = _mm_insert_epi32(x[4*k+3], _mm_extract_epi32 (x[4*k+3], 3)^0x00000002, 3 );
		}
		sliscp256r9(x);
	}
	if ( plen%2 == 0 )
	{
		for ( k = 0; k < PARAL_INST_BY2; k++)
		{
			//Padding: 10*
			x[4*k+2] = _mm_insert_epi32(x[4*k+2], _mm_extract_epi32 (x[4*k+2], 0)^(0x80000000), 0);
			x[4*k+3] = _mm_insert_epi32(x[4*k+3], _mm_extract_epi32 (x[4*k+3], 0)^(0x80000000), 0);
			//Domain seperator
			x[4*k+2] = _mm_insert_epi32(x[4*k+2], _mm_extract_epi32 (x[4*k+2], 3)^0x00000002, 3 );
			x[4*k+3] = _mm_insert_epi32(x[4*k+3], _mm_extract_epi32 (x[4*k+3], 3)^0x00000002, 3 );
		}
		sliscp256r9(x);
	}
	// Tag Extraction Phase
	for ( i = 0; i < PARAL_INST_BY2; i++ )
	{
		x[4*i+2] = x[4*i+2]^_mm_set_epi8(0x0,0x0,0x0,0x0,key[32*i+7],key[32*i+6],key[32*i+5],key[32*i+4],0x0,0x0,0x0,0x0,key[32*i+3],key[32*i+2],key[32*i+1],key[32*i+0]);
		x[4*i+3] = x[4*i+3]^_mm_set_epi8(0x0,0x0,0x0,0x0,key[32*i+23],key[32*i+22],key[32*i+21],key[32*i+20],0x0,0x0,0x0,0x0,key[32*i+19],key[32*i+18],key[32*i+17],key[32*i+16]);
	}
	sliscp256(x);
	for ( i = 0; i < PARAL_INST_BY2; i++ )
	{
		x[4*i+2] = x[4*i+2]^_mm_set_epi8(0x0,0x0,0x0,0x0,key[32*i+15],key[32*i+14],key[32*i+13],key[32*i+12],0x0,0x0,0x0,0x0,key[32*i+11],key[32*i+10],key[32*i+9],key[32*i+8]);
		x[4*i+3] = x[4*i+3]^_mm_set_epi8(0x0,0x0,0x0,0x0,key[32*i+31],key[32*i+30],key[32*i+29],key[32*i+28],0x0,0x0,0x0,0x0,key[32*i+27],key[32*i+26],key[32*i+25],key[32*i+24]);
	}
	sliscp256(x);
	for ( k = 0; k < PARAL_INST_BY2; k++)
	{
		_mm_storeu_si128((void *) (tag + 0 + 8*k), x[4*k+2]);
		_mm_storeu_si128((void *) (tag + 4 + 8*k), x[4*k+3]);
	}
return;
}


void crypto_aead_decrypt( u32 *plaintext, u32 *ciphertext, u32 plen, u32 *tag, u32 tlen, u32 *ad, u32 adlen, u8 *nonce, u8 *key)
{
        u8 k, i;
        u32 l, mblock, adblock;
        
        __m128i x[4*PARAL_INST_BY2];
	u32 index1, index2;

	mblock = plen/2;
	adblock = adlen/2;
       
	//Initialization: Loading KEY & NONCE into Regs
	for ( i = 0; i < PARAL_INST_BY2; i++ )
	{
		x[4*i] = _mm_set_epi8(nonce[32*i+15],nonce[32*i+14],nonce[32*i+13],nonce[32*i+12],nonce[32*i+11],nonce[32*i+10],nonce[32*i+9],nonce[32*i+8],nonce[32*i+7],nonce[32*i+6],nonce[32*i+5],nonce[32*i+4],nonce[32*i+3],nonce[32*i+2],nonce[32*i+1],nonce[32*i+0]);
		x[4*i+1] = _mm_set_epi8(nonce[32*i+31],nonce[32*i+30],nonce[32*i+29],nonce[32*i+28],nonce[32*i+27],nonce[32*i+26],nonce[32*i+25],nonce[32*i+24],nonce[32*i+23],nonce[32*i+22],nonce[32*i+21],nonce[32*i+20],nonce[32*i+19],nonce[32*i+18],nonce[32*i+17],nonce[32*i+16]);
		x[4*i+2] = _mm_set_epi8(key[32*i+15],key[32*i+14],key[32*i+13],key[32*i+12],key[32*i+11],key[32*i+10],key[32*i+9],key[32*i+8],key[32*i+7],key[32*i+6],key[32*i+5],key[32*i+4],key[32*i+3],key[32*i+2],key[32*i+1],key[32*i+0]);
		x[4*i+3] = _mm_set_epi8(key[32*i+31],key[32*i+30],key[32*i+29],key[32*i+28],key[32*i+27],key[32*i+26],key[32*i+25],key[32*i+24],key[32*i+23],key[32*i+22],key[32*i+21],key[32*i+20],key[32*i+19],key[32*i+18],key[32*i+17],key[32*i+16]);
	}
	sliscp256(x);
	for ( i = 0; i < PARAL_INST_BY2; i++ )
	{
		x[4*i+2] = x[4*i+2]^_mm_set_epi8(0x0,0x0,0x0,0x0,key[32*i+7],key[32*i+6],key[32*i+5],key[32*i+4],0x0,0x0,0x0,0x0,key[32*i+3],key[32*i+2],key[32*i+1],key[32*i+0]);
		x[4*i+3] = x[4*i+3]^_mm_set_epi8(0x0,0x0,0x0,0x0,key[32*i+23],key[32*i+22],key[32*i+21],key[32*i+20],0x0,0x0,0x0,0x0,key[32*i+19],key[32*i+18],key[32*i+17],key[32*i+16]);
	}
	sliscp256(x);
	for ( i = 0; i < PARAL_INST_BY2; i++ )
	{
		x[4*i+2] = x[4*i+2]^_mm_set_epi8(0x0,0x0,0x0,0x0,key[32*i+15],key[32*i+14],key[32*i+13],key[32*i+12],0x0,0x0,0x0,0x0,key[32*i+11],key[32*i+10],key[32*i+9],key[32*i+8]);
		x[4*i+3] = x[4*i+3]^_mm_set_epi8(0x0,0x0,0x0,0x0,key[32*i+31],key[32*i+30],key[32*i+29],key[32*i+28],0x0,0x0,0x0,0x0,key[32*i+27],key[32*i+26],key[32*i+25],key[32*i+24]);
	}
	sliscp256(x);
	
	//Processing Associated Data
	for ( l = 0; l < adblock; l++ )
	{
		for ( k = 0; k < PARAL_INST_BY2; k++)
		{
			index1 = 2*l + adlen*2*k;
			index2 = 2*l + adlen*2*k+1;
			x[4*k+2] = _mm_insert_epi32(x[4*k+2], _mm_extract_epi32 (x[4*k+2], 0)^ad[index1], 0);
			x[4*k+2] = _mm_insert_epi32(x[4*k+2], _mm_extract_epi32 (x[4*k+2], 2)^ad[ index2], 2);

			index1 = 2*l + adlen*2*k+adlen;
			index2 = 2*l + adlen*2*k+adlen+1;
			x[4*k+3] = _mm_insert_epi32(x[4*k+3], _mm_extract_epi32 (x[4*k+3], 0)^ad[index1], 0);
			x[4*k+3] = _mm_insert_epi32(x[4*k+3], _mm_extract_epi32 (x[4*k+3], 2)^ad[ index2], 2);

			//Domain seperator
			x[4*k+2] = _mm_insert_epi32(x[4*k+2], _mm_extract_epi32 (x[4*k+2], 3)^0x00000001, 3 );
			x[4*k+3] = _mm_insert_epi32(x[4*k+3], _mm_extract_epi32 (x[4*k+3], 3)^0x00000001, 3 );
		}
		sliscp256r9(x);
	}
	if ( adlen%2 == 0 )
	{
		for ( k = 0; k < PARAL_INST_BY2; k++)
		{
			//Padding: 10*
			x[4*k+2] = _mm_insert_epi32(x[4*k+2], _mm_extract_epi32 (x[4*k+2], 0)^(0x80000000), 0);
			x[4*k+3] = _mm_insert_epi32(x[4*k+3], _mm_extract_epi32 (x[4*k+3], 0)^(0x80000000), 0);

			//Domain seperator
			x[4*k+2] = _mm_insert_epi32(x[4*k+2], _mm_extract_epi32 (x[4*k+2], 3)^0x00000001, 3 );
			x[4*k+3] = _mm_insert_epi32(x[4*k+3], _mm_extract_epi32 (x[4*k+3], 3)^0x00000001, 3 );
		}
		sliscp256r9(x);
	}
	//Decrypting ciphertext
	for ( l = 0; l < mblock; l++ )
	{
		for ( k = 0; k < PARAL_INST_BY2; k++)
		{
			index1 = 2*l + plen*2*k;
			index2 = 2*l + plen*2*k+1;
			plaintext[index1] = _mm_extract_epi32 (x[4*k+2], 0)^ciphertext[index1];
			plaintext[index2] = _mm_extract_epi32 (x[4*k+2], 2)^ciphertext[ index2];
			x[4*k+2] = _mm_insert_epi32(x[4*k+2], ciphertext[index1], 0);
			x[4*k+2] = _mm_insert_epi32(x[4*k+2], ciphertext[index2], 2);
			index1 = 2*l + plen*2*k+plen;
			index2 = 2*l + plen*2*k+plen+1;
			plaintext[index1] = _mm_extract_epi32 (x[4*k+3], 0)^ciphertext[index1];
			plaintext[index2] = _mm_extract_epi32 (x[4*k+3], 2)^ciphertext[ index2];
			
			x[4*k+3] = _mm_insert_epi32(x[4*k+3], ciphertext[index1], 0);
			x[4*k+3] = _mm_insert_epi32(x[4*k+3], ciphertext[index2], 2);

			//Domain seperator
			x[4*k+2] = _mm_insert_epi32(x[4*k+2], _mm_extract_epi32 (x[4*k+2], 3)^0x00000002, 3 );
			x[4*k+3] = _mm_insert_epi32(x[4*k+3], _mm_extract_epi32 (x[4*k+3], 3)^0x00000002, 3 );
		}
		sliscp256r9(x);
	}
	if ( plen%2 == 0 )
	{
		for ( k = 0; k < PARAL_INST_BY2; k++)
		{
			x[4*k+2] = _mm_insert_epi32(x[4*k+2], _mm_extract_epi32 (x[4*k+2], 0)^(0x80000000), 0);
			//x[4*k+2] = _mm_insert_epi32(x[4*k+2], _mm_extract_epi32 (x[4*k+2], 2)^(0x80000080), 2);

			x[4*k+3] = _mm_insert_epi32(x[4*k+3], _mm_extract_epi32 (x[4*k+3], 0)^(0x80000000), 0);
			//x[4*k+3] = _mm_insert_epi32(x[4*k+3], _mm_extract_epi32 (x[4*k+3], 2)^(0x00000080), 2);

			//Domain seperator
			x[4*k+2] = _mm_insert_epi32(x[4*k+2], _mm_extract_epi32 (x[4*k+2], 3)^0x00000002, 3 );
			x[4*k+3] = _mm_insert_epi32(x[4*k+3], _mm_extract_epi32 (x[4*k+3], 3)^0x00000002, 3 );
		}
		sliscp256r9(x);
	}
	//Tag Extraction Phase
	for ( i = 0; i < PARAL_INST_BY2; i++ )
	{
		x[4*i+2] = x[4*i+2]^_mm_set_epi8(0x0,0x0,0x0,0x0,key[32*i+7],key[32*i+6],key[32*i+5],key[32*i+4],0x0,0x0,0x0,0x0,key[32*i+3],key[32*i+2],key[32*i+1],key[32*i+0]);
		x[4*i+3] = x[4*i+3]^_mm_set_epi8(0x0,0x0,0x0,0x0,key[32*i+23],key[32*i+22],key[32*i+21],key[32*i+20],0x0,0x0,0x0,0x0,key[32*i+19],key[32*i+18],key[32*i+17],key[32*i+16]);
	}
	sliscp256(x);
	for ( i = 0; i < PARAL_INST_BY2; i++ )
	{
		x[4*i+2] = x[4*i+2]^_mm_set_epi8(0x0,0x0,0x0,0x0,key[32*i+15],key[32*i+14],key[32*i+13],key[32*i+12],0x0,0x0,0x0,0x0,key[32*i+11],key[32*i+10],key[32*i+9],key[32*i+8]);
		x[4*i+3] = x[4*i+3]^_mm_set_epi8(0x0,0x0,0x0,0x0,key[32*i+31],key[32*i+30],key[32*i+29],key[32*i+28],0x0,0x0,0x0,0x0,key[32*i+27],key[32*i+26],key[32*i+25],key[32*i+24]);
	}
	sliscp256(x);
	for ( k = 0; k < PARAL_INST_BY2; k++)
	{
		_mm_storeu_si128((void *) (tag + 0 + 8*k), x[4*k+2]);
		_mm_storeu_si128((void *) (tag + 4 + 8*k), x[4*k+3]);
	}
return;
}

