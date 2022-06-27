#include "sycon.h"

static const unsigned char RC[14]={0x15,0x1a,0x1d,0x0e,0x17,0x1b,0x0d,0x06,0x03,0x11,0x18,0x1c,0x1e,0x1f};

void sycon_parallel( __m128i *r )
{
        u8 i, j;
        __m128i t0, t1, t2, t3, t4;
        
        for ( i = 0; i < NUMROUNDS; i++ )
        {
                for ( j = 0; j < NUMINSTANCE; j++ )
                {
                        //Sbox Layer
                        t0 = _mm_xor_si128 (r[5*j+2], r[5*j+4]);
                        t1 = _mm_xor_si128 (t0, r[5*j+1]);
                        t2 = _mm_xor_si128 (r[5*j+1], r[5*j+3]);
                        t3 = _mm_xor_si128 (r[5*j+0], r[5*j+4]);
                        t4 = t3^_mm_and_si128(t1, r[5*j+3]);
                        r[5*j+1] = _mm_and_si128(~r[5*j+1], r[5*j+3])^t1^_mm_and_si128(~t2, r[5*j+0]);
                        t1 = _mm_and_si128(~t3, r[5*j+2])^t2;
                        r[5*j+3] = _mm_and_si128(~t0, r[5*j+3])^r[5*j+0]^(~r[5*j+2]);
                        r[5*j+4] = _mm_and_si128(~r[4], r[0])^t2;
                        r[5*j+0] = t4;
                        r[5*j+2] = t1;
                
                        //SubBlockDiffusion Layer
                        r[5*j+0] = r[5*j+0]^ROT64(r[5*j+0],11)^ROT64(r[5*j+0],22);
                        r[5*j+1] = r[5*j+1]^ROT64(r[5*j+1],13)^ROT64(r[5*j+1],26);
                        r[5*j+2] = r[5*j+2]^ROT64(r[5*j+2],31)^ROT64(r[5*j+2],62);
                        r[5*j+3] = r[5*j+3]^ROT64(r[5*j+3],56)^ROT64(r[5*j+3],60);
                        r[5*j+4] = r[5*j+4]^ROT64(r[5*j+4],6)^ROT64(r[5*j+4],12);
                
                        //AddRoundConstant
                        r[5*j+2] = r[5*j+2]^(_mm_set_epi64x( 0xaaaaaaaaaaaaaa00^(u64)RC[i], 0xaaaaaaaaaaaaaa00^(u64)RC[i]));
                        
                        //pLayer (P)
                        r[5*j+0] = ROT16(r[5*j+0],11);
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


int crypto_hash( u32 *digest, u32 *in, u32 inlen)
{
        u8 i;
        u64 l, inpblock;
        __m128i r[5*NUMINSTANCE];
        u32 index1, index2, index3, index4;
        
        inpblock = inlen/2;
        
        //Initilize the state
        for ( i = 0; i < NUMINSTANCE; i++ )
        {
                r[5*i] = _mm_set_epi64x(0x0, 0x0);
                r[5*i+1] = _mm_set_epi64x(0x0, 0x0);
                r[5*i+2] = _mm_set_epi64x(0x1C0A80D42C6E63C5, 0x1C0A80D42C6E63C5); //Set IV here
                r[5*i+3] = _mm_set_epi64x(0x0, 0x0);
                r[5*i+4] = _mm_set_epi64x(0x0, 0x0);
        }
        sycon_parallel(r);
        
        
        //Absorbing message
	// Assuming the message length multiple of 64//
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
                        	r[5*i+0]^=_mm_set_epi32(in[index4], in[index3], in[index2], in[index1]);
                	}
                	sycon_parallel(r);
        	}
	}
	else
	{
                for ( i = 0; i < NUMINSTANCE; i++)
                       	r[5*i]^=_mm_set_epi32(0x00000000, 0x00000001, 0x00000000, 0x00000001);
                sycon_parallel(r);
	}
        //Extratc the message digest
        for ( i = 0; i < NUMINSTANCE; i++)
        {
                digest[16*i+0] = _mm_extract_epi32 (r[5*i], 0);
                digest[16*i+1] = _mm_extract_epi32 (r[5*i], 1);
                
                digest[16*i+8] = _mm_extract_epi32 (r[5*i], 2);
                digest[16*i+9] = _mm_extract_epi32 (r[5*i], 3);
        }
        sycon_parallel(r);
        for ( i = 0; i < NUMINSTANCE; i++)
        {
                digest[16*i+2] = _mm_extract_epi32 (r[5*i], 0);
                digest[16*i+3] = _mm_extract_epi32 (r[5*i], 1);
                
                digest[16*i+10] = _mm_extract_epi32 (r[5*i], 2);
                digest[16*i+11] = _mm_extract_epi32 (r[5*i], 3);
        }
        sycon_parallel(r);
        for ( i = 0; i < NUMINSTANCE; i++)
        {
                digest[16*i+4] = _mm_extract_epi32 (r[5*i], 0);
                digest[16*i+5] = _mm_extract_epi32 (r[5*i], 1);
                
                digest[16*i+12] = _mm_extract_epi32 (r[5*i], 2);
                digest[16*i+13] = _mm_extract_epi32 (r[5*i], 3);
        }
        sycon_parallel(r);
        for ( i = 0; i < NUMINSTANCE; i++)
        {
                digest[16*i+6] = _mm_extract_epi32 (r[5*i], 0);
                digest[16*i+7] = _mm_extract_epi32 (r[5*i], 1);
                
                digest[16*i+14] = _mm_extract_epi32 (r[5*i], 2);
                digest[16*i+15] = _mm_extract_epi32 (r[5*i], 3);
        }
        return 0;
}
