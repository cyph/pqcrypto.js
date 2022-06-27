#include "sycon.h"

static const unsigned char RC[14]={0x15,0x1a,0x1d,0x0e,0x17,0x1b,0x0d,0x06,0x03,0x11,0x18,0x1c,0x1e,0x1f};

void sycon_parallel( __m256i *r )
{
        u8 i, j;
        __m256i t0, t1, t2, t3, t4;
        
        for ( i = 0; i < NUMROUNDS; i++ )
        {
                for ( j = 0; j < NUMINSTANCE; j++ )
                {
                        //Sbox Layer
                        t0 = _mm256_xor_si256 (r[5*j+2], r[5*j+4]);
                        t1 = _mm256_xor_si256 (t0, r[5*j+1]);
                        t2 = _mm256_xor_si256 (r[5*j+1], r[5*j+3]);
                        t3 = _mm256_xor_si256 (r[5*j+0], r[5*j+4]);
                        t4 = t3^_mm256_and_si256 (t1, r[5*j+3]);
                        r[5*j+1] = _mm256_and_si256(~r[5*j+1], r[5*j+3])^t1^_mm256_and_si256(~t2, r[5*j+0]);
                        t1 = _mm256_and_si256(~t3, r[5*j+2])^t2;
                        r[5*j+3] = _mm256_and_si256(~t0, r[5*j+3])^r[5*j+0]^(~r[5*j+2]);
                        r[5*j+4] = _mm256_and_si256(~r[5*j+4], r[5*j+0])^t2;
                        r[5*j+0] = t4;
                        r[5*j+2] = t1;
                        
                        //SubBlockDiffusion Layer
                        r[5*j+0] = r[5*j+0]^ROT64(r[5*j+0],11)^ROT64(r[5*j+0],22);
                        r[5*j+1] = r[5*j+1]^ROT64(r[5*j+1],13)^ROT64(r[5*j+1],26);
                        r[5*j+2] = r[5*j+2]^ROT64(r[5*j+2],31)^ROT64(r[5*j+2],62);
                        r[5*j+3] = r[5*j+3]^ROT64(r[5*j+3],56)^ROT64(r[5*j+3],60);
                        r[5*j+4] = r[5*j+4]^ROT64(r[5*j+4],6)^ROT64(r[5*j+4],12);
                        
                        //AddRoundConstants
                        r[5*j+2] = _mm256_xor_si256 (r[5*j+2], _mm256_set_epi64x(0xaaaaaaaaaaaaaa00^(u64)RC[i], 0xaaaaaaaaaaaaaa00^(u64)RC[i], 0xaaaaaaaaaaaaaa00^(u64)RC[i], 0xaaaaaaaaaaaaaa00^(u64)RC[i]));
                        
                        //PLayer (P)
                        r[5*j] = ROT16(r[5*j],11);
                        r[5*j+0] = ShuffByte1(r[5*j+0]);
                        r[5*j+1] = ROT16(r[5*j+1],4);
                        r[5*j+1] = ShuffByte2(r[5*j+1]);
                        r[5*j+2] = ROT16(r[5*j+2],10);
                        r[5*j+2] = ShuffByte3(r[5*j+2]);
                        r[5*j+3] = ROT16(r[5*j+3],7);
                        r[5*j+3] = ShuffByte4(r[5*j+3]);
                        r[5*j+4] = ROT16(r[5*j+4],5);
                        r[5*j+4] = ShuffByte5(r[5*j+4]);
                }
        }
        return;
}


int crypto_hash( u32 *digest, u32 *in, u32 inlen )
{
        u8 i, j, index;
        u64 l, inpblock, adblock;
        __m256i r[5*NUMINSTANCE];
        u32 index1, index2, index3, index4, index5, index6, index7, index8;
        
        inpblock = inlen/2;
        
        //Initilize the state
        for ( i = 0; i < NUMINSTANCE; i++ )
        {
                r[5*i] = _mm256_set_epi64x(0x0, 0x0, 0x0, 0x0);
                r[5*i+1] = _mm256_set_epi64x(0x0, 0x0, 0x0, 0x0);
                r[5*i+2] = _mm256_set_epi64x(0x1C0A80D42C6E63C5, 0x1C0A80D42C6E63C5, 0x1C0A80D42C6E63C5, 0x1C0A80D42C6E63C5); //Set IV here
                r[5*i+3] = _mm256_set_epi64x(0x0, 0x0, 0x0, 0x0);
                r[5*i+4] = _mm256_set_epi64x(0x0, 0x0, 0x0, 0x0);
        }
        sycon_parallel(r);
        
        //Absorbing message
	if ( inlen != 0 )
	{
        	for ( l = 0; l < inpblock; l++ )
        	{
                	for ( i = 0; i < NUMINSTANCE; i++)
                	{
                        	index1 = 2*l + inlen*2*i;
                        	index2 = 2*l + inlen*2*i+1;
                        	index3 = 2*l + inlen*2*i+ inlen;
                        	index4 = 2*l + inlen*2*i+ inlen+1;
                        	index5 = 2*l + inlen*4*i+2*inlen;
                        	index6 = 2*l + inlen*4*i+1+2*inlen;
                        	index7 = 2*l + inlen*4*i + 3*inlen;
                        	index8 = 2*l + inlen*4*i + 3*inlen + 1;
                        	r[5*i]^=_mm256_set_epi32(in[index8], in[index7], in[index6], in[index5], in[index4], in[index3], in[index2], in[index1]);
                	}
                	sycon_parallel(r);
        	}
	}
	else
	{
		for ( i = 0; i < NUMINSTANCE; i++ )
			r[5*i]^=_mm256_set_epi32(0x00000000, 0x00000001, 0x00000000, 0x00000001,0x00000000, 0x00000001, 0x00000000, 0x00000001);
		sycon_parallel(r);
	}
        
        //Extratc the message digest
        for ( i = 0; i < NUMINSTANCE; i++)
        {
                digest[32*i+0] = _mm256_extract_epi32 (r[5*i], 0);
                digest[32*i+1] = _mm256_extract_epi32 (r[5*i], 1);
                
                digest[32*i+8] = _mm256_extract_epi32 (r[5*i], 2);
                digest[32*i+9] = _mm256_extract_epi32 (r[5*i], 3);
                
                digest[32*i+16] = _mm256_extract_epi32 (r[5*i], 4);
                digest[32*i+17] = _mm256_extract_epi32 (r[5*i], 5);
                
                digest[32*i+24] = _mm256_extract_epi32 (r[5*i], 6);
                digest[32*i+25] = _mm256_extract_epi32 (r[5*i], 7);
        }
        sycon_parallel(r);
        for ( i = 0; i < NUMINSTANCE; i++)
        {
                digest[32*i+2] = _mm256_extract_epi32 (r[5*i], 0);
                digest[32*i+3] = _mm256_extract_epi32 (r[5*i], 1);
                
                digest[32*i+10] = _mm256_extract_epi32 (r[5*i], 2);
                digest[32*i+11] = _mm256_extract_epi32 (r[5*i], 3);
                
                digest[32*i+18] = _mm256_extract_epi32 (r[5*i], 4);
                digest[32*i+19] = _mm256_extract_epi32 (r[5*i], 5);
                
                digest[32*i+26] = _mm256_extract_epi32 (r[5*i], 6);
                digest[32*i+27] = _mm256_extract_epi32 (r[5*i], 7);
        }
        sycon_parallel(r);
        
        for ( i = 0; i < NUMINSTANCE; i++)
        {
                digest[32*i+4] = _mm256_extract_epi32 (r[5*i], 0);
                digest[32*i+5] = _mm256_extract_epi32 (r[5*i], 1);
                
                digest[32*i+12] = _mm256_extract_epi32 (r[5*i], 2);
                digest[32*i+13] = _mm256_extract_epi32 (r[5*i], 3);
                
                digest[32*i+20] = _mm256_extract_epi32 (r[5*i], 4);
                digest[32*i+21] = _mm256_extract_epi32 (r[5*i], 5);
                
                digest[32*i+28] = _mm256_extract_epi32 (r[5*i], 6);
                digest[32*i+29] = _mm256_extract_epi32 (r[5*i], 7);
        }
        sycon_parallel(r);
        for ( i = 0; i < NUMINSTANCE; i++)
        {
                digest[32*i+6] = _mm256_extract_epi32 (r[5*i], 0);
                digest[32*i+7] = _mm256_extract_epi32 (r[5*i], 1);
                
                digest[32*i+14] = _mm256_extract_epi32 (r[5*i], 2);
                digest[32*i+15] = _mm256_extract_epi32 (r[5*i], 3);
                
                digest[32*i+22] = _mm256_extract_epi32 (r[5*i], 4);
                digest[32*i+23] = _mm256_extract_epi32 (r[5*i], 5);
                
                digest[32*i+30] = _mm256_extract_epi32 (r[5*i], 6);
                digest[32*i+31] = _mm256_extract_epi32 (r[5*i], 7);
        }
        return 0;
}
