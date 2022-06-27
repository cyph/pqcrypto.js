/* Optimized (Intel Intrinsics) implementation of SPIX-128 AEAD
   Written by:
   Kalikinkar Mandal <kmandal@uwaterloo.ca>
*/
#include "spix.h"
static inline __m256i _mm256_loadu2_m128i(__m128i *low,__m128i *high)
{
	return _mm256_inserti128_si256(_mm256_castsi128_si256(_mm_loadu_si128(high)),_mm_loadu_si128(low),1);
}
void sliscp_light256( u32 *state )
{
        u8 i, j, k;
        u32 t1, t2;
        
        __m256i x2tmp, x3tmp;
        __m256i x[4*PARAL_INST_BY4];
        
        //Packing state
        for ( k = 0; k < PARAL_INST_BY4; k++ )
        {
                x[4*k+0] = _mm256_loadu2_m128i((void *) (state + 4 + 32*k),(void *) (state + 0 + 32*k));
                x[4*k+2] = _mm256_loadu2_m128i((void *) (state + 12 + 32*k),(void *) (state + 8 + 32*k));
                x[4*k+1] = _mm256_loadu2_m128i((void *) (state + 20 + 32*k),(void *) (state + 16 + 32*k));
                x[4*k+3] = _mm256_loadu2_m128i((void *) (state + 28 + 32*k),(void *) (state + 24 + 32*k));
                PACK(x[4*k+0], x[4*k+1], x[4*k+2], x[4*k+3]);
        }
        
        for ( i = 0; i < STEP_ROUND; i++ )
        {
                for ( k = 0; k < PARAL_INST_BY4; k++ )
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
                        x2tmp = x[4*k+0]^SC(t1,t2);
                        x3tmp = x[4*k+1]^SC(t1,t2);
                        //MSb
                        x[4*k+0] = x[4*k+2];
                        x[4*k+1] = x[4*k+3];
                        x2tmp^=x[4*k+2];
                        x3tmp^=x[4*k+3];
                        x[4*k+2] = SWAPBLK(x2tmp);
                        x[4*k+3] = SWAPBLK(x3tmp);
                }
        }
        
        // Unpacking state
        for ( k = 0; k < PARAL_INST_BY4; k++ )
        {
                UNPACK( x[4*k+0], x[4*k+1], x[4*k+2], x[4*k+3] );
                _mm_storeu_si128((void *) (state + 0 + 32*k),_mm256_extracti128_si256(x[4*k+0],0));
                _mm_storeu_si128((void *) (state + 4 + 32*k),_mm256_extracti128_si256(x[4*k+0],1));
                _mm_storeu_si128((void *) (state + 8 + 32*k),_mm256_extracti128_si256(x[4*k+2],0));
                _mm_storeu_si128((void *) (state + 12 + 32*k),_mm256_extracti128_si256(x[4*k+2],1));
                _mm_storeu_si128((void *) (state + 16 + 32*k),_mm256_extracti128_si256(x[4*k+1],0));
                _mm_storeu_si128((void *) (state + 20 + 32*k),_mm256_extracti128_si256(x[4*k+1],1));
                _mm_storeu_si128((void *) (state + 24 + 32*k),_mm256_extracti128_si256(x[4*k+3],0));
                _mm_storeu_si128((void *) (state + 28 + 32*k),_mm256_extracti128_si256(x[4*k+3],1));
        }
        return;
}

void sliscp256( __m256i *x )
{
	u8 i, j, k;
	u32 t1, t2;
        __m256i x2tmp, x3tmp;
        for ( i = 0; i < STEP_ROUND; i++ )
        {
                for ( k = 0; k < PARAL_INST_BY4; k++ )
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
                        x2tmp = x[4*k+0]^SC(t1,t2);
                        x3tmp = x[4*k+1]^SC(t1,t2);
                        //MSb
                        x[4*k+0] = x[4*k+2];
                        x[4*k+1] = x[4*k+3];
                        x2tmp^=x[4*k+2];
                        x3tmp^=x[4*k+3];
                        x[4*k+2] = SWAPBLK(x2tmp);
                        x[4*k+3] = SWAPBLK(x3tmp);
                }
        }
}

void sliscp256r9( __m256i *x )
{
	u8 i, j, k;
	u32 t1, t2;
        __m256i x2tmp, x3tmp;
        for ( i = 0; i < 9; i++ )
        {
                for ( k = 0; k < PARAL_INST_BY4; k++ )
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
                        x2tmp = x[4*k+0]^SC(t1,t2);
                        x3tmp = x[4*k+1]^SC(t1,t2);
                        //MSb
                        x[4*k+0] = x[4*k+2];
                        x[4*k+1] = x[4*k+3];
                        x2tmp^=x[4*k+2];
                        x3tmp^=x[4*k+3];
                        x[4*k+2] = SWAPBLK(x2tmp);
                        x[4*k+3] = SWAPBLK(x3tmp);
                }
        }
}


void crypto_aead_encrypt( u32 *tag, u32 tlen, u32 *ciphertext, u32 *plaintext, u32 plen, u32 *ad, u32 adlen, u8 *nonce, u8 *key)
{
        u8 k, i;        
	u32 l, mblock, adblock;
        __m256i x[4*PARAL_INST_BY4];
	u32 index1, index2, index3, index4;

	mblock = plen/2;
	adblock = adlen/2;
       
	//Initialization
	for ( i = 0; i < PARAL_INST_BY4; i++)
	{
		x[4*i] = _mm256_set_epi8(nonce[64*i+31],nonce[64*i+30],nonce[64*i+29],nonce[64*i+28],nonce[64*i+27],nonce[64*i+26],nonce[64*i+25],nonce[64*i+24],nonce[64*i+23],nonce[64*i+22],nonce[64*i+21],nonce[64*i+20],nonce[64*i+19],nonce[64*i+18],nonce[64*i+17],nonce[64*i+16],nonce[64*i+15],nonce[64*i+14],nonce[64*i+13],nonce[64*i+12],nonce[64*i+11],nonce[64*i+10],nonce[64*i+9],nonce[64*i+8],nonce[64*i+7],nonce[64*i+6],nonce[64*i+5],nonce[64*i+4],nonce[64*i+3],nonce[64*i+2],nonce[64*i+1],nonce[64*i+0]);

		x[4*i+1] = _mm256_set_epi8(nonce[64*i+63],nonce[64*i+62],nonce[64*i+61],nonce[64*i+60],nonce[64*i+59],nonce[64*i+58],nonce[64*i+57],nonce[64*i+56],nonce[64*i+55],nonce[64*i+54],nonce[64*i+53],nonce[64*i+52],nonce[64*i+51],nonce[64*i+50],nonce[64*i+49],nonce[64*i+48],nonce[64*i+47],nonce[64*i+46],nonce[64*i+45],nonce[64*i+44],nonce[64*i+43],nonce[64*i+42],nonce[64*i+41],nonce[64*i+40],nonce[64*i+39],nonce[64*i+38],nonce[64*i+37],nonce[64*i+36],nonce[64*i+35],nonce[64*i+34],nonce[64*i+33],nonce[64*i+32]);

		x[4*i+2] = _mm256_set_epi8(key[64*i+31],key[64*i+30],key[64*i+29],key[64*i+28],key[64*i+27],key[64*i+26],key[64*i+25],key[64*i+24],key[64*i+23],key[64*i+22],key[64*i+21],key[64*i+20],key[64*i+19],key[64*i+18],key[64*i+17],key[64*i+16],key[64*i+15],key[64*i+14],key[64*i+13],key[64*i+12],key[64*i+11],key[64*i+10],key[64*i+9],key[64*i+8],key[64*i+7],key[64*i+6],key[64*i+5],key[64*i+4],key[64*i+3],key[64*i+2],key[64*i+1],key[64*i+0]);

		x[4*i+3] = _mm256_set_epi8(key[64*i+63],key[64*i+62],key[64*i+61],key[64*i+60],key[64*i+59],key[64*i+58],key[64*i+57],key[64*i+56],key[64*i+55],key[64*i+54],key[64*i+53],key[64*i+52],key[64*i+51],key[64*i+50],key[64*i+49],key[64*i+48],key[64*i+47],key[64*i+46],key[64*i+45],key[64*i+44],key[64*i+43],key[64*i+42],key[64*i+41],key[64*i+40],key[64*i+39],key[64*i+38],key[64*i+37],key[64*i+36],key[64*i+35],key[64*i+34],key[64*i+33],key[64*i+32]);

	}
	sliscp256(x);
	
	for ( i = 0; i < PARAL_INST_BY4; i++)
	{
		x[4*i+2] = x[4*i+2]^_mm256_set_epi8(0x0,0x0,0x0,0x0,key[64*i+23],key[64*i+22],key[64*i+21],key[64*i+20],0x0,0x0,0x0,0x0,key[64*i+19],key[64*i+18],key[64*i+17],key[64*i+16],0x0,0x0,0x0,0x0,key[64*i+7],key[64*i+6],key[64*i+5],key[64*i+4],0x0,0x0,0x0,0x0,key[64*i+3],key[64*i+2],key[64*i+1],key[64*i+0]);

		x[4*i+3] = x[4*i+3]^_mm256_set_epi8(0x0,0x0,0x0,0x0,key[64*i+55],key[64*i+54],key[64*i+53],key[64*i+52],0x0,0x0,0x0,0x0,key[64*i+51],key[64*i+50],key[64*i+49],key[64*i+48],0x0,0x0,0x0,0x0,key[64*i+39],key[64*i+38],key[64*i+37],key[64*i+36],0x0,0x0,0x0,0x0,key[64*i+35],key[64*i+34],key[64*i+33],key[64*i+32]);
	}
	sliscp256(x);
	for ( i = 0; i < PARAL_INST_BY4; i++)
	{
		x[4*i+2] = x[4*i+2]^_mm256_set_epi8(0x0,0x0,0x0,0x0,key[64*i+31],key[64*i+30],key[64*i+29],key[64*i+28],0x0,0x0,0x0,0x0, key[64*i+27],key[64*i+26],key[64*i+25],key[64*i+24],0x0,0x0,0x0,0x0,key[64*i+15],key[64*i+14],key[64*i+13],key[64*i+12],0x0,0x0,0x0,0x0,key[64*i+11],key[64*i+10],key[64*i+9],key[64*i+8]);

		x[4*i+3] = x[4*i+3]^_mm256_set_epi8(0x0,0x0,0x0,0x0,key[64*i+63],key[64*i+62],key[64*i+61],key[64*i+60],0x0,0x0,0x0,0x0,key[64*i+59],key[64*i+58],key[64*i+57],key[64*i+56],0x0,0x0,0x0,0x0,key[64*i+47],key[64*i+46],key[64*i+45],key[64*i+44],0x0,0x0,0x0,0x0,key[64*i+43],key[64*i+42],key[64*i+41],key[64*i+40]);
	}
	sliscp256(x);

	//Processing Associated Data
	for ( l = 0; l < adblock; l++ )
	{
		for ( k = 0; k < PARAL_INST_BY4; k++)
		{
			index1 = 2*l + adlen*4*k;
			index2 = 2*l + adlen*4*k+1;
			index3 = 2*l + adlen*4*k + adlen;
			index4 = 2*l + adlen*4*k + adlen + 1;
			x[4*k+2] = _mm256_insert_epi32(x[4*k+2], _mm256_extract_epi32 (x[4*k+2], 0)^ad[index1], 0);
			x[4*k+2] = _mm256_insert_epi32(x[4*k+2], _mm256_extract_epi32 (x[4*k+2], 2)^ad[index2], 2);
			x[4*k+2] = _mm256_insert_epi32(x[4*k+2], _mm256_extract_epi32 (x[4*k+2], 4)^ad[index3], 4);
			x[4*k+2] = _mm256_insert_epi32(x[4*k+2], _mm256_extract_epi32 (x[4*k+2], 6)^ad[index4], 6);

                        index1 = 2*l + adlen*4*k+2*adlen;
                        index2 = 2*l + adlen*4*k+2*adlen+1;
                        index3 = 2*l + adlen*4*k +3*adlen;
                        index4 = 2*l + adlen*4*k +3*adlen + 1;
			
			x[4*k+3] = _mm256_insert_epi32(x[4*k+3], _mm256_extract_epi32 (x[4*k+3], 0)^ad[index1], 0);
			x[4*k+3] = _mm256_insert_epi32(x[4*k+3], _mm256_extract_epi32 (x[4*k+3], 2)^ad[index2], 2);
			x[4*k+3] = _mm256_insert_epi32(x[4*k+3], _mm256_extract_epi32 (x[4*k+3], 4)^ad[index3], 4);
			x[4*k+3] = _mm256_insert_epi32(x[4*k+3], _mm256_extract_epi32 (x[4*k+3], 6)^ad[index4], 6);

			//Domain seperator
			x[4*k+2] = _mm256_insert_epi32(x[4*k+2], _mm256_extract_epi32 (x[4*k+2], 3)^0x00000001, 3 );
			x[4*k+2] = _mm256_insert_epi32(x[4*k+2], _mm256_extract_epi32 (x[4*k+2], 7)^0x00000001, 7 );
			x[4*k+3] = _mm256_insert_epi32(x[4*k+3], _mm256_extract_epi32 (x[4*k+3], 3)^0x00000001, 3 );
			x[4*k+3] = _mm256_insert_epi32(x[4*k+3], _mm256_extract_epi32 (x[4*k+3], 7)^0x00000001, 7 );
		}
		sliscp256r9(x);
	}
	if ( adlen%2 == 0 )
	{
		for ( k = 0; k < PARAL_INST_BY4; k++)
		{
			x[4*k+2] = _mm256_insert_epi32(x[4*k+2], _mm256_extract_epi32 (x[4*k+2], 0)^(0x80000000), 0);
			x[4*k+2] = _mm256_insert_epi32(x[4*k+2], _mm256_extract_epi32 (x[4*k+2], 4)^(0x80000000), 4);

			x[4*k+3] = _mm256_insert_epi32(x[4*k+3], _mm256_extract_epi32 (x[4*k+3], 0)^(0x80000000), 0);
			x[4*k+3] = _mm256_insert_epi32(x[4*k+3], _mm256_extract_epi32 (x[4*k+3], 4)^(0x80000000), 4);

			//Domain seperator
			x[4*k+2] = _mm256_insert_epi32(x[4*k+2], _mm256_extract_epi32 (x[4*k+2], 3)^0x00000001, 3 );
			x[4*k+2] = _mm256_insert_epi32(x[4*k+2], _mm256_extract_epi32 (x[4*k+2], 7)^0x00000001, 7 );
			x[4*k+3] = _mm256_insert_epi32(x[4*k+3], _mm256_extract_epi32 (x[4*k+3], 3)^0x00000001, 3 );
			x[4*k+3] = _mm256_insert_epi32(x[4*k+3], _mm256_extract_epi32 (x[4*k+3], 7)^0x00000001, 7 );
		}
		sliscp256r9(x);
	}
	//Encrypt plaintext
	for ( l = 0; l < mblock; l++ )
	{
		for ( k = 0; k < PARAL_INST_BY4; k++)
		{
			index1 = 2*l + plen*4*k;
			index2 = 2*l + plen*4*k+1;
			index3 = 2*l + plen*4*k + plen;
			index4 = 2*l + plen*4*k + plen + 1;
			
			ciphertext[index1] = _mm256_extract_epi32 (x[4*k+2], 0)^plaintext[index1];
			ciphertext[index2] = _mm256_extract_epi32 (x[4*k+2], 2)^plaintext[index2];
			ciphertext[index3] = _mm256_extract_epi32 (x[4*k+2], 4)^plaintext[index3];
			ciphertext[index4] = _mm256_extract_epi32 (x[4*k+2], 6)^plaintext[index4];

			x[4*k+2] = _mm256_insert_epi32(x[4*k+2], ciphertext[index1], 0);
			x[4*k+2] = _mm256_insert_epi32(x[4*k+2], ciphertext[index2], 2);
			x[4*k+2] = _mm256_insert_epi32(x[4*k+2], ciphertext[index1], 4);
			x[4*k+2] = _mm256_insert_epi32(x[4*k+2], ciphertext[index2], 6);

                        index1 = 2*l + plen*4*k+2*plen;
                        index2 = 2*l + plen*4*k+2*plen+1;
                        index3 = 2*l + plen*4*k +3*plen;
                        index4 = 2*l + plen*4*k +3*plen + 1;

			ciphertext[index1] = _mm256_extract_epi32 (x[4*k+3], 0)^plaintext[index1];
			ciphertext[index2] = _mm256_extract_epi32 (x[4*k+3], 2)^plaintext[index2];
			ciphertext[index3] = _mm256_extract_epi32 (x[4*k+3], 4)^plaintext[index3];
			ciphertext[index4] = _mm256_extract_epi32 (x[4*k+3], 6)^plaintext[index4];
			
			x[4*k+3] = _mm256_insert_epi32(x[4*k+3], ciphertext[index1], 0);
			x[4*k+3] = _mm256_insert_epi32(x[4*k+3], ciphertext[index2], 2);
			x[4*k+3] = _mm256_insert_epi32(x[4*k+3], ciphertext[index1], 4);
			x[4*k+3] = _mm256_insert_epi32(x[4*k+3], ciphertext[index2], 6);

			//Domain seperator
			x[4*k+2] = _mm256_insert_epi32(x[4*k+2], _mm256_extract_epi32 (x[4*k+2], 3)^0x00000002, 3 );
			x[4*k+2] = _mm256_insert_epi32(x[4*k+2], _mm256_extract_epi32 (x[4*k+2], 7)^0x00000002, 7 );
			x[4*k+3] = _mm256_insert_epi32(x[4*k+3], _mm256_extract_epi32 (x[4*k+3], 3)^0x00000002, 3 );
			x[4*k+3] = _mm256_insert_epi32(x[4*k+3], _mm256_extract_epi32 (x[4*k+3], 7)^0x00000002, 7 );
		}
		sliscp256r9(x);
	}
	if ( plen%2 == 0 )
	{
		for ( k = 0; k < PARAL_INST_BY4; k++)
		{
			x[4*k+2] = _mm256_insert_epi32(x[4*k+2], _mm256_extract_epi32 (x[4*k+2], 0)^(0x80000000), 0);
			x[4*k+2] = _mm256_insert_epi32(x[4*k+2], _mm256_extract_epi32 (x[4*k+2], 4)^(0x80000000), 4);
			x[4*k+3] = _mm256_insert_epi32(x[4*k+3], _mm256_extract_epi32 (x[4*k+3], 0)^(0x80000000), 0);
			x[4*k+3] = _mm256_insert_epi32(x[4*k+3], _mm256_extract_epi32 (x[4*k+3], 4)^(0x80000000), 4);

			//Domain seperator
			x[4*k+2] = _mm256_insert_epi32(x[4*k+2], _mm256_extract_epi32 (x[4*k+2], 3)^0x00000002, 3 );
			x[4*k+2] = _mm256_insert_epi32(x[4*k+2], _mm256_extract_epi32 (x[4*k+2], 7)^0x00000002, 7 );
			x[4*k+3] = _mm256_insert_epi32(x[4*k+3], _mm256_extract_epi32 (x[4*k+3], 3)^0x00000002, 3 );
			x[4*k+3] = _mm256_insert_epi32(x[4*k+3], _mm256_extract_epi32 (x[4*k+3], 7)^0x00000002, 7 );
		}
		sliscp256r9(x);
	}
	//Tag Extraction Phase
	for ( i = 0; i < PARAL_INST_BY4; i++)
	{
		x[4*i+2] = x[4*i+2]^_mm256_set_epi8(0x0,0x0,0x0,0x0,key[64*i+23],key[64*i+22],key[64*i+21],key[64*i+20],0x0,0x0,0x0,0x0,key[64*i+19],key[64*i+18],key[64*i+17],key[64*i+16],0x0,0x0,0x0,0x0,key[64*i+7],key[64*i+6],key[64*i+5],key[64*i+4],0x0,0x0,0x0,0x0,key[64*i+3],key[64*i+2],key[64*i+1],key[64*i+0]);
		x[4*i+3] = x[4*i+3]^_mm256_set_epi8(0x0,0x0,0x0,0x0,key[64*i+55],key[64*i+54],key[64*i+53],key[64*i+52],0x0,0x0,0x0,0x0,key[64*i+51],key[64*i+50],key[64*i+49],key[64*i+48],0x0,0x0,0x0,0x0,key[64*i+39],key[64*i+38],key[64*i+37],key[64*i+36],0x0,0x0,0x0,0x0,key[64*i+35],key[64*i+34],key[64*i+33],key[64*i+32]);
	}
	sliscp256(x);
	for ( i = 0; i < PARAL_INST_BY4; i++)
	{
		x[4*i+2] = x[4*i+2]^_mm256_set_epi8(0x0,0x0,0x0,0x0,key[64*i+31],key[64*i+30],key[64*i+29],key[64*i+28],0x0,0x0,0x0,0x0, key[64*i+27],key[64*i+26],key[64*i+25],key[64*i+24],0x0,0x0,0x0,0x0,key[64*i+15],key[64*i+14],key[64*i+13],key[64*i+12],0x0,0x0,0x0,0x0,key[64*i+11],key[64*i+10],key[64*i+9],key[64*i+8]);

		x[4*i+3] = x[4*i+3]^_mm256_set_epi8(0x0,0x0,0x0,0x0,key[64*i+63],key[64*i+62],key[64*i+61],key[64*i+60],0x0,0x0,0x0,0x0,key[64*i+59],key[64*i+58],key[64*i+57],key[64*i+56],0x0,0x0,0x0,0x0,key[64*i+47],key[64*i+46],key[64*i+45],key[64*i+44],0x0,0x0,0x0,0x0,key[64*i+43],key[64*i+42],key[64*i+41],key[64*i+40]);
	}
	sliscp256(x);

	for ( k = 0; k < PARAL_INST_BY4; k++)
	{
                _mm256_storeu2_m128i ((void *) (tag + 4 + 16*k), (void *) (tag + 0 + 16*k), x[4*k+2]);
                _mm256_storeu2_m128i ((void *) (tag + 12 + 16*k), (void *) (tag + 8 + 16*k), x[4*k+3]);
	}
return;
}

void crypto_aead_decrypt( u32 *plaintext,  u32 *ciphertext, u32 plen, u32 *tag, u32 tlen, u32 *ad, u32 adlen, u8 *nonce, u8 *key)
{
        u8 k, i;
        u32 l, mblock, adblock;
        __m256i x[4*PARAL_INST_BY4];
        u32 index1, index2, index3, index4;
        
        mblock = plen/2;
        adblock = adlen/2;
        
        //Initialization
	for ( i = 0; i < PARAL_INST_BY4; i++)
	{
		x[4*i+2] = _mm256_set_epi8(key[64*i+31],key[64*i+30],key[64*i+29],key[64*i+28],key[64*i+27],key[64*i+26],key[64*i+25],key[64*i+24],key[64*i+23],key[64*i+22],key[64*i+21],key[64*i+20],key[64*i+19],key[64*i+18],key[64*i+17],key[64*i+16],key[64*i+15],key[64*i+14],key[64*i+13],key[64*i+12],key[64*i+11],key[64*i+10],key[64*i+9],key[64*i+8],key[64*i+7],key[64*i+6],key[64*i+5],key[64*i+4],key[64*i+3],key[64*i+2],key[64*i+1],key[64*i+0]);
		x[4*i+3] = _mm256_set_epi8(key[64*i+63],key[64*i+62],key[64*i+61],key[64*i+60],key[64*i+59],key[64*i+58],key[64*i+57],key[64*i+56],key[64*i+55],key[64*i+54],key[64*i+53],key[64*i+52],key[64*i+51],key[64*i+50],key[64*i+49],key[64*i+48],key[64*i+47],key[64*i+46],key[64*i+45],key[64*i+44],key[64*i+43],key[64*i+42],key[64*i+41],key[64*i+40],key[64*i+39],key[64*i+38],key[64*i+37],key[64*i+36],key[64*i+35],key[64*i+34],key[64*i+33],key[64*i+32]);

		x[4*i] = _mm256_set_epi8(nonce[64*i+31],nonce[64*i+30],nonce[64*i+29],nonce[64*i+28],nonce[64*i+27],nonce[64*i+26],nonce[64*i+25],nonce[64*i+24],nonce[64*i+23],nonce[64*i+22],nonce[64*i+21],nonce[64*i+20],nonce[64*i+19],nonce[64*i+18],nonce[64*i+17],nonce[64*i+16],nonce[64*i+15],nonce[64*i+14],nonce[64*i+13],nonce[64*i+12],nonce[64*i+11],nonce[64*i+10],nonce[64*i+9],nonce[64*i+8],nonce[64*i+7],nonce[64*i+6],nonce[64*i+5],nonce[64*i+4],nonce[64*i+3],nonce[64*i+2],nonce[64*i+1],nonce[64*i+0]);
		x[4*i+1] = _mm256_set_epi8(nonce[64*i+63],nonce[64*i+62],nonce[64*i+61],nonce[64*i+60],nonce[64*i+59],nonce[64*i+58],nonce[64*i+57],nonce[64*i+56],nonce[64*i+55],nonce[64*i+54],nonce[64*i+53],nonce[64*i+52],nonce[64*i+51],nonce[64*i+50],nonce[64*i+49],nonce[64*i+48],nonce[64*i+47],nonce[64*i+46],nonce[64*i+45],nonce[64*i+44],nonce[64*i+43],nonce[64*i+42],nonce[64*i+41],nonce[64*i+40],nonce[64*i+39],nonce[64*i+38],nonce[64*i+37],nonce[64*i+36],nonce[64*i+35],nonce[64*i+34],nonce[64*i+33],nonce[64*i+32]);
	}
	sliscp256(x);
	
	for ( i = 0; i < PARAL_INST_BY4; i++)
	{
		x[4*i+2] = x[4*i+2]^_mm256_set_epi8(0x0,0x0,0x0,0x0,key[64*i+23],key[64*i+22],key[64*i+21],key[64*i+20],0x0,0x0,0x0,0x0,key[64*i+19],key[64*i+18],key[64*i+17],key[64*i+16],0x0,0x0,0x0,0x0,key[64*i+7],key[64*i+6],key[64*i+5],key[64*i+4],0x0,0x0,0x0,0x0,key[64*i+3],key[64*i+2],key[64*i+1],key[64*i+0]);
		x[4*i+3] = x[4*i+3]^_mm256_set_epi8(0x0,0x0,0x0,0x0,key[64*i+55],key[64*i+54],key[64*i+53],key[64*i+52],0x0,0x0,0x0,0x0,key[64*i+51],key[64*i+50],key[64*i+49],key[64*i+48],0x0,0x0,0x0,0x0,key[64*i+39],key[64*i+38],key[64*i+37],key[64*i+36],0x0,0x0,0x0,0x0,key[64*i+35],key[64*i+34],key[64*i+33],key[64*i+32]);
	}
	sliscp256(x);
	for ( i = 0; i < PARAL_INST_BY4; i++)
	{
		x[4*i+2] = x[4*i+2]^_mm256_set_epi8(0x0,0x0,0x0,0x0,key[64*i+31],key[64*i+30],key[64*i+29],key[64*i+28],0x0,0x0,0x0,0x0, key[64*i+27],key[64*i+26],key[64*i+25],key[64*i+24],0x0,0x0,0x0,0x0,key[64*i+15],key[64*i+14],key[64*i+13],key[64*i+12],0x0,0x0,0x0,0x0,key[64*i+11],key[64*i+10],key[64*i+9],key[64*i+8]);

		x[4*i+3] = x[4*i+3]^_mm256_set_epi8(0x0,0x0,0x0,0x0,key[64*i+63],key[64*i+62],key[64*i+61],key[64*i+60],0x0,0x0,0x0,0x0,key[64*i+59],key[64*i+58],key[64*i+57],key[64*i+56],0x0,0x0,0x0,0x0,key[64*i+47],key[64*i+46],key[64*i+45],key[64*i+44],0x0,0x0,0x0,0x0,key[64*i+43],key[64*i+42],key[64*i+41],key[64*i+40]);
	}
	sliscp256(x);
        
        //Processing Associated Data
	for ( l = 0; l < adblock; l++ )
	{
		for ( k = 0; k < PARAL_INST_BY4; k++)
		{
			index1 = 2*l + adlen*4*k;
			index2 = 2*l + adlen*4*k+1;
			index3 = 2*l + adlen*4*k + adlen;
			index4 = 2*l + adlen*4*k + adlen + 1;
			x[4*k+2] = _mm256_insert_epi32(x[4*k+2], _mm256_extract_epi32 (x[4*k+2], 0)^ad[index1], 0);
			x[4*k+2] = _mm256_insert_epi32(x[4*k+2], _mm256_extract_epi32 (x[4*k+2], 2)^ad[index2], 2);
			x[4*k+2] = _mm256_insert_epi32(x[4*k+2], _mm256_extract_epi32 (x[4*k+2], 4)^ad[index3], 4);
			x[4*k+2] = _mm256_insert_epi32(x[4*k+2], _mm256_extract_epi32 (x[4*k+2], 6)^ad[index4], 6);

                        index1 = 2*l + adlen*4*k+2*adlen;
                        index2 = 2*l + adlen*4*k+2*adlen+1;
                        index3 = 2*l + adlen*4*k +3*adlen;
                        index4 = 2*l + adlen*4*k +3*adlen + 1;
			
			x[4*k+3] = _mm256_insert_epi32(x[4*k+3], _mm256_extract_epi32 (x[4*k+3], 0)^ad[index1], 0);
			x[4*k+3] = _mm256_insert_epi32(x[4*k+3], _mm256_extract_epi32 (x[4*k+3], 2)^ad[index2], 2);
			x[4*k+3] = _mm256_insert_epi32(x[4*k+3], _mm256_extract_epi32 (x[4*k+3], 4)^ad[index3], 4);
			x[4*k+3] = _mm256_insert_epi32(x[4*k+3], _mm256_extract_epi32 (x[4*k+3], 6)^ad[index4], 6);

			//Domain seperator
			x[4*k+2] = _mm256_insert_epi32(x[4*k+2], _mm256_extract_epi32 (x[4*k+2], 3)^0x00000001, 3 );
			x[4*k+2] = _mm256_insert_epi32(x[4*k+2], _mm256_extract_epi32 (x[4*k+2], 7)^0x00000001, 7 );
			x[4*k+3] = _mm256_insert_epi32(x[4*k+3], _mm256_extract_epi32 (x[4*k+3], 3)^0x00000001, 3 );
			x[4*k+3] = _mm256_insert_epi32(x[4*k+3], _mm256_extract_epi32 (x[4*k+3], 7)^0x00000001, 7 );
		}
		sliscp256r9(x);
	}
	if ( adlen%2 == 0 )
	{
		for ( k = 0; k < PARAL_INST_BY4; k++)
		{
			x[4*k+2] = _mm256_insert_epi32(x[4*k+2], _mm256_extract_epi32 (x[4*k+2], 0)^(0x80000000), 0);
			x[4*k+2] = _mm256_insert_epi32(x[4*k+2], _mm256_extract_epi32 (x[4*k+2], 4)^(0x80000000), 4);
			x[4*k+3] = _mm256_insert_epi32(x[4*k+3], _mm256_extract_epi32 (x[4*k+3], 0)^(0x80000000), 0);
			x[4*k+3] = _mm256_insert_epi32(x[4*k+3], _mm256_extract_epi32 (x[4*k+3], 4)^(0x80000000), 4);

			//Domain seperator
			x[4*k+2] = _mm256_insert_epi32(x[4*k+2], _mm256_extract_epi32 (x[4*k+2], 3)^0x00000001, 3 );
			x[4*k+2] = _mm256_insert_epi32(x[4*k+2], _mm256_extract_epi32 (x[4*k+2], 7)^0x00000001, 7 );
			x[4*k+3] = _mm256_insert_epi32(x[4*k+3], _mm256_extract_epi32 (x[4*k+3], 3)^0x00000001, 3 );
			x[4*k+3] = _mm256_insert_epi32(x[4*k+3], _mm256_extract_epi32 (x[4*k+3], 7)^0x00000001, 7 );
		}
		sliscp256r9(x);
	}
        //Encrypting plaintext
        for ( l = 0; l < mblock; l++ )
        {
                for ( k = 0; k < PARAL_INST_BY4; k++)
                {
                        index1 = 2*l + plen*4*k;
                        index2 = 2*l + plen*4*k+1;
                        index3 = 2*l + plen*4*k + plen;
                        index4 = 2*l + plen*4*k + plen + 1;
                        
                        plaintext[index1] = _mm256_extract_epi32 (x[4*k+2], 0)^ciphertext[index1];
                        plaintext[index2] = _mm256_extract_epi32 (x[4*k+2], 2)^ciphertext[index2];
                        plaintext[index3] = _mm256_extract_epi32 (x[4*k+2], 4)^ciphertext[index3];
                        plaintext[index4] = _mm256_extract_epi32 (x[4*k+2], 6)^ciphertext[index4];
                        
                        x[4*k+2] = _mm256_insert_epi32(x[4*k+2], ciphertext[index1], 0);
                        x[4*k+2] = _mm256_insert_epi32(x[4*k+2], ciphertext[index2], 2);
                        x[4*k+2] = _mm256_insert_epi32(x[4*k+2], ciphertext[index1], 4);
                        x[4*k+2] = _mm256_insert_epi32(x[4*k+2], ciphertext[index2], 6);
                        
                        index1 = 2*l + plen*4*k+2*plen;
                        index2 = 2*l + plen*4*k+2*plen+1;
                        index3 = 2*l + plen*4*k +3*plen;
                        index4 = 2*l + plen*4*k +3*plen + 1;
                        
                        plaintext[index1] = _mm256_extract_epi32 (x[4*k+3], 0)^ciphertext[index1];
                        plaintext[index2] = _mm256_extract_epi32 (x[4*k+3], 2)^ciphertext[index2];
                        plaintext[index3] = _mm256_extract_epi32 (x[4*k+3], 4)^ciphertext[index3];
                        plaintext[index4] = _mm256_extract_epi32 (x[4*k+3], 6)^ciphertext[index4];
                        
                        x[4*k+3] = _mm256_insert_epi32(x[4*k+3], ciphertext[index1], 0);
                        x[4*k+3] = _mm256_insert_epi32(x[4*k+3], ciphertext[index2], 2);
                        x[4*k+3] = _mm256_insert_epi32(x[4*k+3], ciphertext[index1], 4);
                        x[4*k+3] = _mm256_insert_epi32(x[4*k+3], ciphertext[index2], 6);
                        
                        //Domain seperator
                        x[4*k+2] = _mm256_insert_epi32(x[4*k+2], _mm256_extract_epi32 (x[4*k+2], 3)^0x00000002, 3 );
                        x[4*k+2] = _mm256_insert_epi32(x[4*k+2], _mm256_extract_epi32 (x[4*k+2], 7)^0x00000002, 7 );
                        x[4*k+3] = _mm256_insert_epi32(x[4*k+3], _mm256_extract_epi32 (x[4*k+3], 3)^0x00000002, 3 );
                        x[4*k+3] = _mm256_insert_epi32(x[4*k+3], _mm256_extract_epi32 (x[4*k+3], 7)^0x00000002, 7 );
                }
                sliscp256r9(x);
        }
	if ( plen%2 == 0 )
	{
		for ( k = 0; k < PARAL_INST_BY4; k++)
		{
			x[4*k+2] = _mm256_insert_epi32(x[4*k+2], _mm256_extract_epi32 (x[4*k+2], 0)^(0x80000000), 0);
			x[4*k+2] = _mm256_insert_epi32(x[4*k+2], _mm256_extract_epi32 (x[4*k+2], 4)^(0x80000000), 4);
			x[4*k+3] = _mm256_insert_epi32(x[4*k+3], _mm256_extract_epi32 (x[4*k+3], 0)^(0x80000000), 0);
			x[4*k+3] = _mm256_insert_epi32(x[4*k+3], _mm256_extract_epi32 (x[4*k+3], 4)^(0x80000000), 4);

			//Domain seperator
			x[4*k+2] = _mm256_insert_epi32(x[4*k+2], _mm256_extract_epi32 (x[4*k+2], 3)^0x00000002, 3 );
			x[4*k+2] = _mm256_insert_epi32(x[4*k+2], _mm256_extract_epi32 (x[4*k+2], 7)^0x00000002, 7 );
			x[4*k+3] = _mm256_insert_epi32(x[4*k+3], _mm256_extract_epi32 (x[4*k+3], 3)^0x00000002, 3 );
			x[4*k+3] = _mm256_insert_epi32(x[4*k+3], _mm256_extract_epi32 (x[4*k+3], 7)^0x00000002, 7 );
		}
		sliscp256r9(x);
	}

        // Tag Extraction Phase
	for ( i = 0; i < PARAL_INST_BY4; i++)
	{
		x[4*i+2] = x[4*i+2]^_mm256_set_epi8(0x0,0x0,0x0,0x0,key[64*i+23],key[64*i+22],key[64*i+21],key[64*i+20],0x0,0x0,0x0,0x0,key[64*i+19],key[64*i+18],key[64*i+17],key[64*i+16],0x0,0x0,0x0,0x0,key[64*i+7],key[64*i+6],key[64*i+5],key[64*i+4],0x0,0x0,0x0,0x0,key[64*i+3],key[64*i+2],key[64*i+1],key[64*i+0]);
		x[4*i+3] = x[4*i+3]^_mm256_set_epi8(0x0,0x0,0x0,0x0,key[64*i+55],key[64*i+54],key[64*i+53],key[64*i+52],0x0,0x0,0x0,0x0,key[64*i+51],key[64*i+50],key[64*i+49],key[64*i+48],0x0,0x0,0x0,0x0,key[64*i+39],key[64*i+38],key[64*i+37],key[64*i+36],0x0,0x0,0x0,0x0,key[64*i+35],key[64*i+34],key[64*i+33],key[64*i+32]);
	}
	sliscp256(x);
	for ( i = 0; i < PARAL_INST_BY4; i++)
	{
		x[4*i+2] = x[4*i+2]^_mm256_set_epi8(0x0,0x0,0x0,0x0,key[64*i+31],key[64*i+30],key[64*i+29],key[64*i+28],0x0,0x0,0x0,0x0, key[64*i+27],key[64*i+26],key[64*i+25],key[64*i+24],0x0,0x0,0x0,0x0,key[64*i+15],key[64*i+14],key[64*i+13],key[64*i+12],0x0,0x0,0x0,0x0,key[64*i+11],key[64*i+10],key[64*i+9],key[64*i+8]);

		x[4*i+3] = x[4*i+3]^_mm256_set_epi8(0x0,0x0,0x0,0x0,key[64*i+63],key[64*i+62],key[64*i+61],key[64*i+60],0x0,0x0,0x0,0x0,key[64*i+59],key[64*i+58],key[64*i+57],key[64*i+56],0x0,0x0,0x0,0x0,key[64*i+47],key[64*i+46],key[64*i+45],key[64*i+44],0x0,0x0,0x0,0x0,key[64*i+43],key[64*i+42],key[64*i+41],key[64*i+40]);
	}
	sliscp256(x);

        for ( k = 0; k < PARAL_INST_BY4; k++)
        {
                _mm256_storeu2_m128i ((void *) (tag + 4 + 16*k), (void *) (tag + 0 + 16*k), x[4*k+2]);
                _mm256_storeu2_m128i ((void *) (tag + 12 + 16*k), (void *) (tag + 8 + 16*k), x[4*k+3]);
        }
        return;
}
