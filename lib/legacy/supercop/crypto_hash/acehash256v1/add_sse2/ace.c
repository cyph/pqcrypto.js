/* Reference implementation of the ACE-Hash256
 Written by:
 Kalikinkar Mandal <kmandal@uwaterloo.ca>
 */


#include "ace.h"

void ace320( u32 *state )
{
        u8 i, j, k;
        u32 t1, t2;
        
        __m128i x[10*PARAL_INST_BY4];
        __m128i xtmp, ytmp;
        
        //Packing state
        for ( k = 0; k < PARAL_INST_BY4; k++)
        {
        	xtmp = _mm_loadu_si128((void *) (state + 0+40*k));
        	ytmp = _mm_loadu_si128((void *) (state + 4+40*k));
        	x[10*k+0] = _mm_unpacklo_epi64(xtmp, ytmp);
        	x[10*k+4] = _mm_unpackhi_epi64(xtmp, ytmp);

        	xtmp = _mm_loadu_si128((void *) (state + 10+40*k));
        	ytmp = _mm_loadu_si128((void *) (state + 14+40*k));
        	x[10*k+1] = _mm_unpacklo_epi64(xtmp, ytmp);
        	x[10*k+5] = _mm_unpackhi_epi64(xtmp, ytmp);
        	
		xtmp = _mm_loadu_si128((void *) (state + 20+40*k));
        	ytmp = _mm_loadu_si128((void *) (state + 24+40*k));
        	x[10*k+2] = _mm_unpacklo_epi64(xtmp, ytmp);
        	x[10*k+6] = _mm_unpackhi_epi64(xtmp, ytmp);
		
		xtmp = _mm_loadu_si128((void *) (state + 30+40*k));
        	ytmp = _mm_loadu_si128((void *) (state + 34+40*k));
        	x[10*k+3] = _mm_unpacklo_epi64(xtmp, ytmp);
        	x[10*k+7] = _mm_unpackhi_epi64(xtmp, ytmp);
        	
		xtmp = _mm_loadu_si128((void *) (state + 8+40*k));
        	ytmp = _mm_loadu_si128((void *) (state + 18+40*k));
        	x[10*k+8] = _mm_unpacklo_epi64(xtmp, ytmp);

		xtmp = _mm_loadu_si128((void *) (state + 28+40*k));
        	ytmp = _mm_loadu_si128((void *) (state + 38+40*k));
        	x[10*k+9] = _mm_unpacklo_epi64(xtmp, ytmp);
        }

        for ( i = 0; i < NUMSTEPS; i++ )
        {
                
                for ( k = 0; k < PARAL_INST_BY4; k++ )
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

			xtmp = _mm_unpacklo_epi64(_mm_shuffle_epi32(x[10*k], _MM_SHUFFLE(1, 0, 3,2)), x[10*k+8]);
			x[10*k+4]^=xtmp;
			x[10*k+4] = _mm_shuffle_epi32(x[10*k+4], _MM_SHUFFLE(1, 0, 3,2));
			
			xtmp = _mm_unpackhi_epi64(x[10*k+1], x[10*k+8]);
			x[10*k+5]^=xtmp;
			x[10*k+5] = _mm_shuffle_epi32(x[10*k+5], _MM_SHUFFLE(1, 0, 3,2));
			
			xtmp = _mm_unpacklo_epi64(_mm_shuffle_epi32(x[10*k+2], _MM_SHUFFLE(1, 0, 3,2)), x[10*k+9]);
			x[10*k+6]^=xtmp;
			x[10*k+6] = _mm_shuffle_epi32(x[10*k+6], _MM_SHUFFLE(1, 0, 3,2));
			
			xtmp = _mm_unpackhi_epi64(x[10*k+3], x[10*k+9]);
			x[10*k+7]^=xtmp;
			x[10*k+7] = _mm_shuffle_epi32(x[10*k+7], _MM_SHUFFLE(1, 0, 3,2));
			
			x[10*k+8]^=(x[10*k]&masklo);
			x[10*k+8]^=(_mm_shuffle_epi32(x[10*k+1], _MM_SHUFFLE(1, 0, 3,2))&maskhi);
			x[10*k+8]^=SC((u32)SC2[i], (u32)SC2[i]);
			
			x[10*k+9]^=(x[10*k+2]&masklo);
			x[10*k+9]^=(_mm_shuffle_epi32(x[10*k+3], _MM_SHUFFLE(1, 0, 3,2))&maskhi);
			x[10*k+9]^=SC((u32)SC2[i], (u32)SC2[i]);


			xtmp = _mm_unpacklo_epi64(x[10*k+4], x[10*k]);
			ytmp = _mm_unpackhi_epi64(x[10*k], _mm_shuffle_epi32(x[10*k+8], _MM_SHUFFLE(1, 0, 3,2)));
			x[10*k] = xtmp;
			x[10*k+8] = x[10*k+8]&maskhi; 
			x[10*k+8]^=(_mm_shuffle_epi32(x[10*k+4], _MM_SHUFFLE(1, 0, 3,2))&masklo);
			x[10*k+4] = ytmp;

			xtmp = _mm_unpacklo_epi64(x[10*k+5], x[10*k+1]);
			ytmp = _mm_unpackhi_epi64(x[10*k+1], x[10*k+8]);
			x[10*k+1] = xtmp;
			x[10*k+8] = x[10*k+8]&masklo;
			x[10*k+8]^=x[10*k+5]&maskhi;
			x[10*k+5] = ytmp;

			xtmp = _mm_unpacklo_epi64(x[10*k+6], x[10*k+2]);
			ytmp = _mm_unpackhi_epi64(x[10*k+2], _mm_shuffle_epi32(x[10*k+9], _MM_SHUFFLE(1, 0, 3,2)));
			x[10*k+2] = xtmp;
			x[10*k+9] = x[10*k+9]&maskhi; 
			x[10*k+9]^=(_mm_shuffle_epi32(x[10*k+6], _MM_SHUFFLE(1, 0, 3,2))&masklo);
			x[10*k+6] = ytmp;

			xtmp = _mm_unpacklo_epi64(x[10*k+7], x[10*k+3]);
			ytmp = _mm_unpackhi_epi64(x[10*k+3], x[10*k+9]);
			x[10*k+3] = xtmp;
			x[10*k+9] = x[10*k+9]&masklo;
			x[10*k+9]^=x[10*k+7]&maskhi;
			x[10*k+7] = ytmp;
                }
        }
        //Unpack state
        for ( k = 0; k < PARAL_INST_BY4; k++)
        {
		xtmp = _mm_unpacklo_epi64(x[10*k], x[10*k+4]);
		ytmp = _mm_unpackhi_epi64(x[10*k], x[10*k+4]);
		_mm_storeu_si128((void *) (state + 0+40*k),xtmp);
		_mm_storeu_si128((void *) (state + 4 +40*k),ytmp);
		_mm_storeu_si128((void *) (state + 8 +40*k), x[10*k+8]);


		xtmp = _mm_unpacklo_epi64(x[10*k+1], x[10*k+5]);
		ytmp = _mm_unpackhi_epi64(x[10*k+1], x[10*k+5]);
		_mm_storeu_si128((void *) (state + 10+40*k),xtmp);
		_mm_storeu_si128((void *) (state + 14 +40*k),ytmp);
		_mm_storeu_si128((void *) (state + 18 +40*k), _mm_shuffle_epi32(x[10*k+8], _MM_SHUFFLE(1, 0, 3,2)));

		xtmp = _mm_unpacklo_epi64(x[10*k+2], x[10*k+6]);
		ytmp = _mm_unpackhi_epi64(x[10*k+2], x[10*k+6]);
		_mm_storeu_si128((void *) (state + 20+40*k),xtmp);
		_mm_storeu_si128((void *) (state + 24 +40*k),ytmp);
		_mm_storeu_si128((void *) (state + 28 +40*k), x[10*k+9]);
		
		xtmp = _mm_unpacklo_epi64(x[10*k+3], x[10*k+7]);
		ytmp = _mm_unpackhi_epi64(x[10*k+3], x[10*k+7]);
		_mm_storeu_si128((void *) (state + 30+40*k),xtmp);
		_mm_storeu_si128((void *) (state + 34 +40*k),ytmp);
		//_mm_storeu_si128((void *) (state + 38 +40*k), _mm_shuffle_epi32(x[10*k+9], _MM_SHUFFLE(1, 0, 3,2)));
		state[38 +40*k] = _mm_extract_epi32(x[10*k+9], 0);
		state[39 +40*k] = _mm_extract_epi32(x[10*k+9], 1);
        }
        return;
}

void ace( __m128i *x )
{
        u8 i, j, k;
        u32 t1, t2;
        __m128i xtmp, ytmp;
        
        for ( i = 0; i < NUMSTEPS; i++ )
        {
                
                for ( k = 0; k < PARAL_INST_BY4; k++ )
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

			xtmp = _mm_unpacklo_epi64(_mm_shuffle_epi32(x[10*k], _MM_SHUFFLE(1, 0, 3,2)), x[10*k+8]);
			x[10*k+4]^=xtmp;
			x[10*k+4] = _mm_shuffle_epi32(x[10*k+4], _MM_SHUFFLE(1, 0, 3,2));
			
			xtmp = _mm_unpackhi_epi64(x[10*k+1], x[10*k+8]);
			x[10*k+5]^=xtmp;
			x[10*k+5] = _mm_shuffle_epi32(x[10*k+5], _MM_SHUFFLE(1, 0, 3,2));
			
			xtmp = _mm_unpacklo_epi64(_mm_shuffle_epi32(x[10*k+2], _MM_SHUFFLE(1, 0, 3,2)), x[10*k+9]);
			x[10*k+6]^=xtmp;
			x[10*k+6] = _mm_shuffle_epi32(x[10*k+6], _MM_SHUFFLE(1, 0, 3,2));
			
			xtmp = _mm_unpackhi_epi64(x[10*k+3], x[10*k+9]);
			x[10*k+7]^=xtmp;
			x[10*k+7] = _mm_shuffle_epi32(x[10*k+7], _MM_SHUFFLE(1, 0, 3,2));
			
			x[10*k+8]^=(x[10*k]&masklo);
			x[10*k+8]^=(_mm_shuffle_epi32(x[10*k+1], _MM_SHUFFLE(1, 0, 3,2))&maskhi);
			x[10*k+8]^=SC((u32)SC2[i], (u32)SC2[i]);
			
			x[10*k+9]^=(x[10*k+2]&masklo);
			x[10*k+9]^=(_mm_shuffle_epi32(x[10*k+3], _MM_SHUFFLE(1, 0, 3,2))&maskhi);
			x[10*k+9]^=SC((u32)SC2[i], (u32)SC2[i]);


			xtmp = _mm_unpacklo_epi64(x[10*k+4], x[10*k]);
			ytmp = _mm_unpackhi_epi64(x[10*k], _mm_shuffle_epi32(x[10*k+8], _MM_SHUFFLE(1, 0, 3,2)));
			x[10*k] = xtmp;
			x[10*k+8] = x[10*k+8]&maskhi; 
			x[10*k+8]^=(_mm_shuffle_epi32(x[10*k+4], _MM_SHUFFLE(1, 0, 3,2))&masklo);
			x[10*k+4] = ytmp;

			xtmp = _mm_unpacklo_epi64(x[10*k+5], x[10*k+1]);
			ytmp = _mm_unpackhi_epi64(x[10*k+1], x[10*k+8]);
			x[10*k+1] = xtmp;
			x[10*k+8] = x[10*k+8]&masklo;
			x[10*k+8]^=x[10*k+5]&maskhi;
			x[10*k+5] = ytmp;

			xtmp = _mm_unpacklo_epi64(x[10*k+6], x[10*k+2]);
			ytmp = _mm_unpackhi_epi64(x[10*k+2], _mm_shuffle_epi32(x[10*k+9], _MM_SHUFFLE(1, 0, 3,2)));
			x[10*k+2] = xtmp;
			x[10*k+9] = x[10*k+9]&maskhi; 
			x[10*k+9]^=(_mm_shuffle_epi32(x[10*k+6], _MM_SHUFFLE(1, 0, 3,2))&masklo);
			x[10*k+6] = ytmp;

			xtmp = _mm_unpacklo_epi64(x[10*k+7], x[10*k+3]);
			ytmp = _mm_unpackhi_epi64(x[10*k+3], x[10*k+9]);
			x[10*k+3] = xtmp;
			x[10*k+9] = x[10*k+9]&masklo;
			x[10*k+9]^=x[10*k+7]&maskhi;
			x[10*k+7] = ytmp;
                }
        }
        return;
}

int crypto_hash( u32 *out, u32 *in, u64 inlen )
{
        u8 i;
        u64 l, inblock;
        __m128i x[10*PARAL_INST_BY4];
	u32 index1, index2;

	inblock = inlen/2;
       
	//Initialization: IV
	for ( i = 0; i < PARAL_INST_BY4; i++)
	{
		x[10*i+4] = _mm_set_epi64x (0x0, 0x00404080);
		x[10*i+5] = _mm_set_epi64x (0x0, 0x00404080);
		x[10*i+6] = _mm_set_epi64x (0x0, 0x00404080);
		x[10*i+7] = _mm_set_epi64x (0x0, 0x00404080);
		
		x[10*i+0] = _mm_set_epi64x (0x0, 0x0);
		x[10*i+1] = _mm_set_epi64x (0x0, 0x0);
		x[10*i+2] = _mm_set_epi64x (0x0, 0x0);
		x[10*i+3] = _mm_set_epi64x (0x0, 0x0);
                
		x[10*i+8] = _mm_set_epi64x (0x0, 0x0);
		x[10*i+9] = _mm_set_epi64x (0x0, 0x0);
	}
	ace(x);
	
        if ( inlen != 0 )
        {
                //Absorbing plaintext
                for ( l = 0; l < inblock; l++ )
                {
                        for ( i = 0; i < PARAL_INST_BY4; i++)
                        {
                                index1 = 2*l + inlen*4*i;
                                index2 = 2*l + inlen*4*i+1;
                                x[10*i+0] = x[10*i+0]^_mm_set_epi32(0x0, in[index2], 0x0, in[index1]);
			
                                index1 = 2*l + inlen*4*i+ inlen;
                                index2 = 2*l + inlen*4*i+ inlen+1;
                                x[10*i+1] = x[10*i+1]^_mm_set_epi32(0x0, in[index2], 0x0, in[index1]);
			
                                index1 = 2*l + inlen*4*i+ 2*inlen;
                                index2 = 2*l + inlen*4*i+ 2*inlen+1;
                                x[10*i+2] = x[10*i+2]^_mm_set_epi32(0x0, in[index2], 0x0, in[index1]);
			
                                index1 = 2*l + inlen*4*i+ 3*inlen;
                                index2 = 2*l + inlen*4*i+ 3*inlen+1;
                                x[10*i+3] = x[10*i+3]^_mm_set_epi32(0x0, in[index2], 0x0, in[index1]);
                        }
                        ace(x);
                }
        }
        else
        {
                for ( i = 0; i < PARAL_INST_BY4; i++)
                {
                        x[10*i+0] = x[10*i+0]^_mm_set_epi32(0x0, 0x0, 0x0, 0x00000080);
                        x[10*i+1] = x[10*i+1]^_mm_set_epi32(0x0, 0x0, 0x0, 0x00000080);
                        x[10*i+2] = x[10*i+2]^_mm_set_epi32(0x0, 0x0, 0x0, 0x00000080);
                        x[10*i+3] = x[10*i+3]^_mm_set_epi32(0x0, 0x0, 0x0, 0x00000080);
                }
                ace(x);
        }

	//Digest Extraction Phase
	for ( i = 0; i < PARAL_INST_BY4; i++)
	{
		out[32*i] = _mm_extract_epi32 (x[10*i], 0);
		out[32*i+1] = _mm_extract_epi32 (x[10*i], 2);
		
		out[32*i+8] = _mm_extract_epi32 (x[10*i+1], 0);
		out[32*i+9] = _mm_extract_epi32 (x[10*i+1], 2);
		
		out[32*i+16] = _mm_extract_epi32 (x[10*i+2], 0);
		out[32*i+17] = _mm_extract_epi32 (x[10*i+2], 2);
		
		out[32*i+24] = _mm_extract_epi32 (x[10*i+3], 0);
		out[32*i+25] = _mm_extract_epi32 (x[10*i+3], 2);
	}
	ace(x);
	for ( i = 0; i < PARAL_INST_BY4; i++)
	{
		out[32*i+2] = _mm_extract_epi32 (x[10*i], 0);
		out[32*i+3] = _mm_extract_epi32 (x[10*i], 2);
		
		out[32*i+10] = _mm_extract_epi32 (x[10*i+1], 0);
		out[32*i+11] = _mm_extract_epi32 (x[10*i+1], 2);
		
		out[32*i+18] = _mm_extract_epi32 (x[10*i+2], 0);
		out[32*i+19] = _mm_extract_epi32 (x[10*i+2], 2);
		
		out[32*i+26] = _mm_extract_epi32 (x[10*i+3], 0);
		out[32*i+27] = _mm_extract_epi32 (x[10*i+3], 2);
	}
	ace(x);
	for ( i = 0; i < PARAL_INST_BY4; i++)
	{
		out[32*i+4] = _mm_extract_epi32 (x[10*i], 0);
		out[32*i+5] = _mm_extract_epi32 (x[10*i], 2);
		
		out[32*i+12] = _mm_extract_epi32 (x[10*i+1], 0);
		out[32*i+13] = _mm_extract_epi32 (x[10*i+1], 2);
		
		out[32*i+20] = _mm_extract_epi32 (x[10*i+2], 0);
		out[32*i+21] = _mm_extract_epi32 (x[10*i+2], 2);
		
		out[32*i+28] = _mm_extract_epi32 (x[10*i+3], 0);
		out[32*i+29] = _mm_extract_epi32 (x[10*i+3], 2);
	}
	ace(x);
	for ( i = 0; i < PARAL_INST_BY4; i++)
	{
		out[32*i+6] = _mm_extract_epi32 (x[10*i], 0);
		out[32*i+7] = _mm_extract_epi32 (x[10*i], 2);
		
		out[32*i+14] = _mm_extract_epi32 (x[10*i+1], 0);
		out[32*i+15] = _mm_extract_epi32 (x[10*i+1], 2);
		
		out[32*i+22] = _mm_extract_epi32 (x[10*i+2], 0);
		out[32*i+23] = _mm_extract_epi32 (x[10*i+2], 2);
		
		out[32*i+30] = _mm_extract_epi32 (x[10*i+3], 0);
		out[32*i+31] = _mm_extract_epi32 (x[10*i+3], 2);
	}
return 0;
}
