/* Reference implementation of ACE-Hash256
 Written by:
 Kalikinkar Mandal <kmandal@uwaterloo.ca>
 */

#include "ace.h"

/*static inline __m256i _mm256_loadu2_m128i(__m128i *low,__m128i *high)
{
	return _mm256_inserti128_si256(_mm256_castsi128_si256(_mm_loadu_si128(high)),_mm_loadu_si128(low),1);
}*/
static __attribute__((always_inline)) inline __m256i
    _mm256_unpacklo_epi128(__m256i a, __m256i b) {
	      return _mm256_permute2x128_si256(a, b, 0x20);
    }

static __attribute__((always_inline)) inline __m256i
    _mm256_unpackhi_epi128(__m256i a, __m256i b) {
	      return _mm256_permute2x128_si256(a, b, 0x31);
    }

void ace( __m256i *x )
{
        u8 i, j, k;
        u32 t1, t2;
        __m256i xtmp, ytmp;
        
        for ( i = 0; i < NUMSTEPS; i++ )
        {
                
                for ( k = 0; k < PARAL_INST_BY8; k++ )
                {
                        //Chi
                        PACK_SSb(x[10*k],x[10*k+1]);
                        for ( j = 0; j < SIMECKROUND; j++ )
                        {
                                t1 = (u32)((RC0[i] >> j)&1);
                                t2 = (u32)((RC1[i] >> j)&1);
                                ROAX(x[10*k], x[10*k+1], t1, t2);
                        }
                        UNPACK_SSb(x[10*k],x[10*k+1]);
                        
			PACK_SSb(x[10*k+2],x[10*k+3]);
                        for ( j = 0; j < SIMECKROUND; j++ )
                        {
                                t1 = (u32)((RC0[i] >> j)&1);
                                t2 = (u32)((RC1[i] >> j)&1);
                                ROAX(x[10*k+2], x[10*k+3], t1, t2);
                        }
                        UNPACK_SSb(x[10*k+2],x[10*k+3]);
			
			PACK_SSb(x[10*k+8],x[10*k+9]);
                        for ( j = 0; j < SIMECKROUND; j++ )
                        {
                                t1 = (u32)((RC2[i] >> j)&1);
                                t2 = (u32)((RC2[i] >> j)&1);
                                ROAX(x[10*k+8], x[10*k+9], t1, t2);
                        }
                        UNPACK_SSb(x[10*k+8],x[10*k+9]);

                        //Diffusion
                        t1 = (u32)SC0[i];
                        t2 = (u32)SC1[i];
                        x[10*k+4] = x[10*k+4]^SC(t1, t2);
                        x[10*k+5] = x[10*k+5]^SC(t1, t2);
                        x[10*k+6] = x[10*k+6]^SC(t1, t2);
                        x[10*k+7] = x[10*k+7]^SC(t1, t2);

			//R0, R4
			x[10*k] = _mm256_permute4x64_epi64(x[10*k], _MM_SHUFFLE(2, 0, 3,1));
			x[10*k+4] = _mm256_permute4x64_epi64(x[10*k+4], _MM_SHUFFLE(3, 1, 2,0));
			xtmp = _mm256_unpacklo_epi128(x[10*k], x[10*k+8]);
			x[10*k+4]^=xtmp;
			xtmp = _mm256_unpackhi_epi128(x[10*k+4],x[10*k]);
			xtmp = _mm256_permute4x64_epi64(xtmp, _MM_SHUFFLE(3, 1, 2,0));

			ytmp = x[10*k+8]^_mm256_permute4x64_epi64(x[10*k], _MM_SHUFFLE(1,0,3,2));
			ytmp^=SC((u32)SC2[i], (u32)SC2[i]);
			ytmp = _mm256_unpacklo_epi128(x[10*k], ytmp);
			ytmp = _mm256_permute4x64_epi64(ytmp, _MM_SHUFFLE(3, 1, 2,0));
			x[10*k] = xtmp;
			x[10*k+8] = x[10*k+8]&maskhi;
			x[10*k+8]^=(x[10*k+4]&masklo);
			x[10*k+4] = ytmp;

			//R1, R5
			x[10*k+1] = _mm256_permute4x64_epi64(x[10*k+1], _MM_SHUFFLE(2, 0, 3,1));
			x[10*k+5] = _mm256_permute4x64_epi64(x[10*k+5], _MM_SHUFFLE(3, 1, 2,0));
			xtmp = _mm256_unpackhi_epi128(_mm256_permute4x64_epi64(x[10*k+1], _MM_SHUFFLE(1,0,3,2)), x[10*k+8]);
			x[10*k+5]^=xtmp;
			xtmp = _mm256_unpackhi_epi128(x[10*k+5],x[10*k+1]);
			xtmp = _mm256_permute4x64_epi64(xtmp, _MM_SHUFFLE(3, 1, 2,0));

			ytmp = x[10*k+8]^x[10*k+1];
			ytmp^=SC((u32)SC2[i], (u32)SC2[i]);
			ytmp = _mm256_unpackhi_epi128(_mm256_permute4x64_epi64(x[10*k+1],_MM_SHUFFLE(1,0,3,2)), ytmp);
			ytmp = _mm256_permute4x64_epi64(ytmp, _MM_SHUFFLE(3, 1, 2,0));
			x[10*k+1] = xtmp;
			x[10*k+8] = x[10*k+8]&masklo;
			x[10*k+8]^=(_mm256_permute4x64_epi64(x[10*k+5], _MM_SHUFFLE(1,0,3,2))&maskhi);
			x[10*k+5] = ytmp;

			//R2, R6
			x[10*k+2] = _mm256_permute4x64_epi64(x[10*k+2], _MM_SHUFFLE(2, 0, 3,1));
			x[10*k+6] = _mm256_permute4x64_epi64(x[10*k+6], _MM_SHUFFLE(3, 1, 2,0));
			xtmp = _mm256_unpacklo_epi128(x[10*k+2], x[10*k+9]);
			x[10*k+6]^=xtmp;
			xtmp = _mm256_unpackhi_epi128(x[10*k+6],x[10*k+2]);
			xtmp = _mm256_permute4x64_epi64(xtmp, _MM_SHUFFLE(3, 1, 2,0));

			ytmp = x[10*k+9]^_mm256_permute4x64_epi64(x[10*k+2], _MM_SHUFFLE(1,0,3,2));
			ytmp^=SC((u32)SC2[i], (u32)SC2[i]);
			ytmp = _mm256_unpacklo_epi128(x[10*k+2], ytmp);
			ytmp = _mm256_permute4x64_epi64(ytmp, _MM_SHUFFLE(3, 1, 2,0));
			x[10*k+2] = xtmp;
			x[10*k+9] = x[10*k+9]&maskhi;
			x[10*k+9]^=(x[10*k+6]&masklo);
			x[10*k+6] = ytmp;

			//R3, R7
			x[10*k+3] = _mm256_permute4x64_epi64(x[10*k+3], _MM_SHUFFLE(2, 0, 3,1));
			x[10*k+7] = _mm256_permute4x64_epi64(x[10*k+7], _MM_SHUFFLE(3, 1, 2,0));
			xtmp = _mm256_unpackhi_epi128(_mm256_permute4x64_epi64(x[10*k+3], _MM_SHUFFLE(1,0,3,2)), x[10*k+9]);
			x[10*k+7]^=xtmp;
			xtmp = _mm256_unpackhi_epi128(x[10*k+7],x[10*k+3]);
			xtmp = _mm256_permute4x64_epi64(xtmp, _MM_SHUFFLE(3, 1, 2,0));

			ytmp = x[10*k+9]^x[10*k+3];
			ytmp^=SC((u32)SC2[i], (u32)SC2[i]);
			ytmp = _mm256_unpackhi_epi128(_mm256_permute4x64_epi64(x[10*k+3],_MM_SHUFFLE(1,0,3,2)), ytmp);
			ytmp = _mm256_permute4x64_epi64(ytmp, _MM_SHUFFLE(3, 1, 2,0));
			x[10*k+3] = xtmp;
			x[10*k+9] = x[10*k+9]&masklo;
			x[10*k+9]^=(_mm256_permute4x64_epi64(x[10*k+7], _MM_SHUFFLE(1,0,3,2))&maskhi);
			x[10*k+7] = ytmp;
                }
        }
        return;
}



int crypto_hash( u32 *out, u32 *in, u64 inlen )
{
        u8 i;
        u64 l, inblock;
        __m256i x[10*PARAL_INST_BY8];
	u32 index1, index2, index3, index4;

	inblock = inlen/2;
       
	//Initialization: IV
	for ( i = 0; i < PARAL_INST_BY8; i++)
	{
		//_mm256_zeroall();
		x[10*i+4] = _mm256_set_epi64x(0x0, 0x00404080, 0x0, 0x00404080);
		x[10*i+5] = _mm256_set_epi64x(0x0, 0x00404080, 0x0, 0x00404080);
		x[10*i+6] = _mm256_set_epi64x(0x0, 0x00404080, 0x0, 0x00404080);
		x[10*i+7] = _mm256_set_epi64x(0x0, 0x00404080, 0x0, 0x00404080);
		
		x[10*i+0] = _mm256_set_epi64x(0x0, 0x0, 0x0, 0x0);
		x[10*i+1] = _mm256_set_epi64x(0x0, 0x0, 0x0, 0x0);
		x[10*i+2] = _mm256_set_epi64x(0x0, 0x0, 0x0, 0x0);
		x[10*i+3] = _mm256_set_epi64x(0x0, 0x0, 0x0, 0x0);
                
		x[10*i+8] = _mm256_set_epi64x(0x0, 0x0, 0x0, 0x0);
		x[10*i+9] = _mm256_set_epi64x(0x0, 0x0, 0x0, 0x0);
		

	}
	ace(x);

	//Absorbing message
        if ( inlen != 0 )
        {
                for ( l = 0; l < inblock; l++ )
                {
                        for ( i = 0; i < PARAL_INST_BY8; i++)
                        {
                                index1 = 2*l + inlen*8*i;
                                index2 = 2*l + inlen*8*i+1;
                                index3 = 2*l + inlen*8*i + inlen;
                                index4 = 2*l + inlen*8*i + inlen + 1;
                                x[10*i+0] = x[10*i+0]^_mm256_set_epi32(0x0, in[index4], 0x0, in[index3], 0x0, in[index2], 0x0, in[index1]);
			
                                index1 = 2*l + inlen*8*i+2*inlen;
                                index2 = 2*l + inlen*8*i+1+2*inlen;
                                index3 = 2*l + inlen*8*i + 3*inlen;
                                index4 = 2*l + inlen*8*i + 3*inlen + 1;
                                x[10*i+1] = x[10*i+1]^_mm256_set_epi32(0x0, in[index4], 0x0, in[index3], 0x0, in[index2], 0x0, in[index1]);
			
                                index1 = 2*l + inlen*8*i+4*inlen;
                                index2 = 2*l + inlen*8*i+1+4*inlen;
                                index3 = 2*l + inlen*8*i + 5*inlen;
                                index4 = 2*l + inlen*8*i + 5*inlen + 1;
                                x[10*i+2] = x[10*i+2]^_mm256_set_epi32(0x0, in[index4], 0x0, in[index3], 0x0, in[index2], 0x0, in[index1]);
			
                                index1 = 2*l + inlen*8*i+6*inlen;
                                index2 = 2*l + inlen*8*i+1+6*inlen;
                                index3 = 2*l + inlen*8*i + 7*inlen;
                                index4 = 2*l + inlen*8*i + 7*inlen + 1;
                                x[10*i+3] = x[10*i+3]^_mm256_set_epi32(0x0, in[index4], 0x0, in[index3], 0x0, in[index2], 0x0, in[index1]);
                        }
                        ace(x);
                }
        }
        else
        {
                for ( i = 0; i < PARAL_INST_BY8; i++)
                {
                        
                        x[10*i+0] = x[10*i+0]^_mm256_set_epi32(0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x00000080);
                        x[10*i+1] = x[10*i+1]^_mm256_set_epi32(0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x00000080);
                        x[10*i+2] = x[10*i+2]^_mm256_set_epi32(0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x00000080);
                        x[10*i+3] = x[10*i+3]^_mm256_set_epi32(0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x00000080);
                }
                ace(x);
        }

	for ( i = 0; i < PARAL_INST_BY8; i++)
	{
		out[64*i] = _mm256_extract_epi32 (x[10*i], 0);
		out[64*i+1] = _mm256_extract_epi32 (x[10*i], 2);

		out[64*i+8] = _mm256_extract_epi32 (x[10*i], 4);
		out[64*i+9] = _mm256_extract_epi32 (x[10*i], 6);

		out[64*i+16] = _mm256_extract_epi32 (x[10*i+1], 0);
		out[64*i+17] = _mm256_extract_epi32 (x[10*i+1], 2);
		
		out[64*i+24] = _mm256_extract_epi32 (x[10*i+1], 4);
		out[64*i+25] = _mm256_extract_epi32 (x[10*i+1], 6);
		
		out[64*i+32] = _mm256_extract_epi32 (x[10*i+2], 0);
		out[64*i+33] = _mm256_extract_epi32 (x[10*i+2], 2);
		
		out[64*i+40] = _mm256_extract_epi32 (x[10*i+2], 4);
		out[64*i+41] = _mm256_extract_epi32 (x[10*i+2], 6);
		
		out[64*i+48] = _mm256_extract_epi32 (x[10*i+3], 0);
		out[64*i+49] = _mm256_extract_epi32 (x[10*i+3], 2);
		
		out[64*i+56] = _mm256_extract_epi32 (x[10*i+3], 4);
		out[64*i+57] = _mm256_extract_epi32 (x[10*i+3], 6);
	}
	ace(x);
	for ( i = 0; i < PARAL_INST_BY8; i++)
	{
		out[64*i+2] = _mm256_extract_epi32 (x[10*i], 0);
		out[64*i+3] = _mm256_extract_epi32 (x[10*i], 2);

		out[64*i+10] = _mm256_extract_epi32 (x[10*i], 4);
		out[64*i+11] = _mm256_extract_epi32 (x[10*i], 6);

		out[64*i+18] = _mm256_extract_epi32 (x[10*i+1], 0);
		out[64*i+19] = _mm256_extract_epi32 (x[10*i+1], 2);
		
		out[64*i+26] = _mm256_extract_epi32 (x[10*i+1], 4);
		out[64*i+27] = _mm256_extract_epi32 (x[10*i+1], 6);
		
		out[64*i+34] = _mm256_extract_epi32 (x[10*i+2], 0);
		out[64*i+35] = _mm256_extract_epi32 (x[10*i+2], 2);
		
		out[64*i+42] = _mm256_extract_epi32 (x[10*i+2], 4);
		out[64*i+43] = _mm256_extract_epi32 (x[10*i+2], 6);
		
		out[64*i+50] = _mm256_extract_epi32 (x[10*i+3], 0);
		out[64*i+51] = _mm256_extract_epi32 (x[10*i+3], 2);
		
		out[64*i+58] = _mm256_extract_epi32 (x[10*i+3], 4);
		out[64*i+59] = _mm256_extract_epi32 (x[10*i+3], 6);
	}
	ace(x);
	for ( i = 0; i < PARAL_INST_BY8; i++)
	{
		out[64*i+4] = _mm256_extract_epi32 (x[10*i], 0);
		out[64*i+5] = _mm256_extract_epi32 (x[10*i], 2);

		out[64*i+12] = _mm256_extract_epi32 (x[10*i], 4);
		out[64*i+13] = _mm256_extract_epi32 (x[10*i], 6);

		out[64*i+20] = _mm256_extract_epi32 (x[10*i+1], 0);
		out[64*i+21] = _mm256_extract_epi32 (x[10*i+1], 2);
		
		out[64*i+28] = _mm256_extract_epi32 (x[10*i+1], 4);
		out[64*i+29] = _mm256_extract_epi32 (x[10*i+1], 6);
		
		out[64*i+36] = _mm256_extract_epi32 (x[10*i+2], 0);
		out[64*i+37] = _mm256_extract_epi32 (x[10*i+2], 2);
		
		out[64*i+44] = _mm256_extract_epi32 (x[10*i+2], 4);
		out[64*i+45] = _mm256_extract_epi32 (x[10*i+2], 6);
		
		out[64*i+52] = _mm256_extract_epi32 (x[10*i+3], 0);
		out[64*i+53] = _mm256_extract_epi32 (x[10*i+3], 2);
		
		out[64*i+60] = _mm256_extract_epi32 (x[10*i+3], 4);
		out[64*i+61] = _mm256_extract_epi32 (x[10*i+3], 6);
	}
	ace(x);
	for ( i = 0; i < PARAL_INST_BY8; i++)
	{
		out[64*i+6] = _mm256_extract_epi32 (x[10*i], 0);
		out[64*i+7] = _mm256_extract_epi32 (x[10*i], 2);

		out[64*i+14] = _mm256_extract_epi32 (x[10*i], 4);
		out[64*i+15] = _mm256_extract_epi32 (x[10*i], 6);

		out[64*i+22] = _mm256_extract_epi32 (x[10*i+1], 0);
		out[64*i+23] = _mm256_extract_epi32 (x[10*i+1], 2);
		
		out[64*i+30] = _mm256_extract_epi32 (x[10*i+1], 4);
		out[64*i+31] = _mm256_extract_epi32 (x[10*i+1], 6);
		
		out[64*i+38] = _mm256_extract_epi32 (x[10*i+2], 0);
		out[64*i+39] = _mm256_extract_epi32 (x[10*i+2], 2);
		
		out[64*i+46] = _mm256_extract_epi32 (x[10*i+2], 4);
		out[64*i+47] = _mm256_extract_epi32 (x[10*i+2], 6);
		
		out[64*i+54] = _mm256_extract_epi32 (x[10*i+3], 0);
		out[64*i+55] = _mm256_extract_epi32 (x[10*i+3], 2);
		
		out[64*i+62] = _mm256_extract_epi32 (x[10*i+3], 4);
		out[64*i+63] = _mm256_extract_epi32 (x[10*i+3], 6);
	}
return 0;
}
