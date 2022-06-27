/* Reference implementation of ACE-128, AEAD
 Written by:
 Kalikinkar Mandal <kmandal@uwaterloo.ca>
 */

#include "ace.h"

static inline __m256i _mm256_loadu2_m128i(__m128i *high,__m128i *low)
{
        return _mm256_inserti128_si256(_mm256_castsi128_si256(_mm_loadu_si128(high)),_mm_loadu_si128(low),1);
}
static __attribute__((always_inline)) inline __m256i _mm256_unpacklo_epi128(__m256i a, __m256i b)
{
        return _mm256_permute2x128_si256(a, b, 0x20);
}

static __attribute__((always_inline)) inline __m256i _mm256_unpackhi_epi128(__m256i a, __m256i b)
{
        return _mm256_permute2x128_si256(a, b, 0x31);
}

void ace320( u32 *state )
{
        u8 i, j, k;
        u32 t1, t2;
        
        __m256i x[10*PARAL_INST_BY8];
        __m256i xtmp, ytmp;
        
        //Packing state
        for ( k = 0; k < PARAL_INST_BY8; k++)
        {
		xtmp = _mm256_loadu2_m128i((void *) (state + 4 + 80*k),(void *) (state + 0 + 80*k));
		ytmp = _mm256_loadu2_m128i((void *) (state + 14 + 80*k),(void *) (state + 10 + 80*k));
		xtmp = _mm256_permutevar8x32_epi32(xtmp, _mm256_set_epi32(7, 6, 3, 2, 5, 4, 1, 0));
		ytmp = _mm256_permutevar8x32_epi32(ytmp, _mm256_set_epi32(7, 6, 3, 2, 5, 4, 1, 0));
		x[10*k+0] = _mm256_permute2x128_si256(xtmp, ytmp, 0x20);
		x[10*k+4] = _mm256_permute2x128_si256(ytmp, ytmp, 0x31);
		
		xtmp = _mm256_loadu2_m128i((void *) (state + 24 + 80*k),(void *) (state + 20 + 80*k));
		ytmp = _mm256_loadu2_m128i((void *) (state + 34 + 80*k),(void *) (state + 30 + 80*k));
		xtmp = _mm256_permutevar8x32_epi32(xtmp, _mm256_set_epi32(7, 6, 3, 2, 5, 4, 1, 0));
		ytmp = _mm256_permutevar8x32_epi32(ytmp, _mm256_set_epi32(7, 6, 3, 2, 5, 4, 1, 0));
		x[10*k+1] = _mm256_permute2x128_si256(xtmp, ytmp, 0x20);
		x[10*k+5] = _mm256_permute2x128_si256(ytmp, ytmp, 0x31);
		
		xtmp = _mm256_loadu2_m128i((void *) (state + 44 + 80*k),(void *) (state + 40 + 80*k));
		ytmp = _mm256_loadu2_m128i((void *) (state + 54 + 80*k),(void *) (state + 50 + 80*k));
		xtmp = _mm256_permutevar8x32_epi32(xtmp, _mm256_set_epi32(7, 6, 3, 2, 5, 4, 1, 0));
		ytmp = _mm256_permutevar8x32_epi32(ytmp, _mm256_set_epi32(7, 6, 3, 2, 5, 4, 1, 0));
		x[10*k+2] = _mm256_permute2x128_si256(xtmp, ytmp, 0x20);
		x[10*k+6] = _mm256_permute2x128_si256(ytmp, ytmp, 0x31);
		
		xtmp = _mm256_loadu2_m128i((void *) (state + 64 + 80*k),(void *) (state + 60 + 80*k));
		ytmp = _mm256_loadu2_m128i((void *) (state + 74 + 80*k),(void *) (state + 70 + 80*k));
		xtmp = _mm256_permutevar8x32_epi32(xtmp, _mm256_set_epi32(7, 6, 3, 2, 5, 4, 1, 0));
		ytmp = _mm256_permutevar8x32_epi32(ytmp, _mm256_set_epi32(7, 6, 3, 2, 5, 4, 1, 0));
		x[10*k+3] = _mm256_permute2x128_si256(xtmp, ytmp, 0x20);
		x[10*k+7] = _mm256_permute2x128_si256(ytmp, ytmp, 0x31);

		x[10*k+8] = _mm256_set_epi32(state[80*k+39], state[80*k+38], state[80*k+29], state[80*k+28], state[80*k+19], state[80*k+18], state[80*k+9], state[80*k+8]);
		x[10*k+9] = _mm256_set_epi32(state[80*k+79], state[80*k+78], state[80*k+69], state[80*k+68], state[80*k+59], state[80*k+58], state[80*k+49], state[80*k+48]);

        }

        for ( i = 0; i < NUMSTEPS; i++ )
        {
                
                for ( k = 0; k < PARAL_INST_BY8; k++ )
                {
                        //SSb
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

                        //ASc and MSb
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
        //Unpack state
        for ( k = 0; k < PARAL_INST_BY8; k++)
        {
		 xtmp = _mm256_set_m128i (_mm256_extracti128_si256(x[10*k+4],0), _mm256_extracti128_si256(x[10*k],0));
		 xtmp = _mm256_permute4x64_epi64(xtmp, _MM_SHUFFLE(3, 1, 2,0));
		 _mm_storeu_si128((void *) (state + 0),_mm256_extracti128_si256(xtmp,0));
		 _mm_storeu_si128((void *) (state + 4),_mm256_extracti128_si256(xtmp,1));
		state[8] = _mm256_extract_epi32 (x[10*k+8], 0);
		state[9] = _mm256_extract_epi32 (x[10*k+8], 1);

		 
		 xtmp = _mm256_set_m128i (_mm256_extracti128_si256(x[10*k+4],1), _mm256_extracti128_si256(x[10*k],1));
		 xtmp = _mm256_permute4x64_epi64(xtmp, _MM_SHUFFLE(3, 1, 2,0));
		 _mm_storeu_si128((void *) (state + 10),_mm256_extracti128_si256(xtmp,0));
		 _mm_storeu_si128((void *) (state + 14),_mm256_extracti128_si256(xtmp,1));
		_mm_storeu_si128((void *) (state + 18),_mm256_extracti128_si256(x[10*k+8],1));
		state[18] = _mm256_extract_epi32 (x[10*k+8], 2);
		state[19] = _mm256_extract_epi32 (x[10*k+8], 3);


		 xtmp = _mm256_set_m128i (_mm256_extracti128_si256(x[10*k+5],0), _mm256_extracti128_si256(x[10*k+1],0));
		 xtmp = _mm256_permute4x64_epi64(xtmp, _MM_SHUFFLE(3, 1, 2,0));
		 _mm_storeu_si128((void *) (state + 20),_mm256_extracti128_si256(xtmp,0));
		 _mm_storeu_si128((void *) (state + 24),_mm256_extracti128_si256(xtmp,1));
		state[28] = _mm256_extract_epi32 (x[10*k+8], 4);
		state[29] = _mm256_extract_epi32 (x[10*k+8], 5);
		 
		 xtmp = _mm256_set_m128i (_mm256_extracti128_si256(x[10*k+5],1), _mm256_extracti128_si256(x[10*k+1],1));
		 xtmp = _mm256_permute4x64_epi64(xtmp, _MM_SHUFFLE(3, 1, 2,0));
		 _mm_storeu_si128((void *) (state + 30),_mm256_extracti128_si256(xtmp,0));
		 _mm_storeu_si128((void *) (state + 34),_mm256_extracti128_si256(xtmp,1));
		state[38] = _mm256_extract_epi32 (x[10*k+8], 6);
		state[39] = _mm256_extract_epi32 (x[10*k+8], 7);

		 xtmp = _mm256_set_m128i (_mm256_extracti128_si256(x[10*k+6],0), _mm256_extracti128_si256(x[10*k+2],0));
		 xtmp = _mm256_permute4x64_epi64(xtmp, _MM_SHUFFLE(3, 1, 2,0));
		 _mm_storeu_si128((void *) (state + 40),_mm256_extracti128_si256(xtmp,0));
		 _mm_storeu_si128((void *) (state + 44),_mm256_extracti128_si256(xtmp,1));
		state[48] = _mm256_extract_epi32 (x[10*k+9], 0);
		state[49] = _mm256_extract_epi32 (x[10*k+9], 1);
		 
		 xtmp = _mm256_set_m128i (_mm256_extracti128_si256(x[10*k+6],1), _mm256_extracti128_si256(x[10*k+2],1));
		 xtmp = _mm256_permute4x64_epi64(xtmp, _MM_SHUFFLE(3, 1, 2,0));
		 _mm_storeu_si128((void *) (state + 50),_mm256_extracti128_si256(xtmp,0));
		 _mm_storeu_si128((void *) (state + 54),_mm256_extracti128_si256(xtmp,1));
		state[58] = _mm256_extract_epi32 (x[10*k+9], 2);
		state[59] = _mm256_extract_epi32 (x[10*k+9], 3);


		 xtmp = _mm256_set_m128i (_mm256_extracti128_si256(x[10*k+7],0), _mm256_extracti128_si256(x[10*k+3],0));
		 xtmp = _mm256_permute4x64_epi64(xtmp, _MM_SHUFFLE(3, 1, 2,0));
		 _mm_storeu_si128((void *) (state + 60),_mm256_extracti128_si256(xtmp,0));
		 _mm_storeu_si128((void *) (state + 64),_mm256_extracti128_si256(xtmp,1));
		state[68] = _mm256_extract_epi32 (x[10*k+9], 4);
		state[69] = _mm256_extract_epi32 (x[10*k+9], 5);
		 
		 xtmp = _mm256_set_m128i (_mm256_extracti128_si256(x[10*k+7],1), _mm256_extracti128_si256(x[10*k+3],1));
		 xtmp = _mm256_permute4x64_epi64(xtmp, _MM_SHUFFLE(3, 1, 2,0));
		 _mm_storeu_si128((void *) (state + 70),_mm256_extracti128_si256(xtmp,0));
		 _mm_storeu_si128((void *) (state + 74),_mm256_extracti128_si256(xtmp,1));
		state[78] = _mm256_extract_epi32 (x[10*k+9], 6);
		state[79] = _mm256_extract_epi32 (x[10*k+9], 7);
        }
        return;
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
                        //SSb
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

                        //ASc and MSb
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


int crypto_aead_encrypt( u32 *tag, u32 tlen, u32 *c, u32 *m, u32 mlen, u32 *ad, u32 adlen, u8 *k, u8 *npub, u32 klen )
{
        u8 i;
        u64 l, mblock, adblock;
        __m256i x[10*PARAL_INST_BY8];
	u32 index1, index2, index3, index4;

	mblock = mlen/2;
	adblock = adlen/2;
       
	//Initialization: Loading Key & Nonce
        for ( i = 0; i < PARAL_INST_BY8; i++ )
	{
		//Key and Nonce are loaded according to Packing.

		x[10*i+0] = _mm256_loadu2_m128i((void*) (k + 16 + 128*i), (void*) (k + 0 + 128*i)); 
		x[10*i+4] = _mm256_loadu2_m128i((void *) (npub + 16 + 128*i), (void *) (npub + 0 + 128*i));
		x[10*i+4] = x[10*i+4]&(_mm256_set_epi64x(0x0, 0xffffffffffffffff, 0x0, 0xffffffffffffffff));


		
		x[10*i+1] = _mm256_loadu2_m128i((void *) (k + 48 + 128*i),(void *) (k + 32 + 128*i));
		x[10*i+5] = _mm256_loadu2_m128i((void *) (npub + 48 + 128*i), (void *) (npub + 32 + 128*i));
		x[10*i+5] = x[10*i+5]&(_mm256_set_epi64x(0x0, 0xffffffffffffffff, 0x0, 0xffffffffffffffff));

		x[10*i+8] = _mm256_set_epi8(npub[128*i+63],npub[128*i+62],npub[128*i+61],npub[128*i+60],npub[128*i+59],npub[128*i+58],npub[128*i+57],npub[128*i+56], npub[128*i+47],npub[128*i+46],npub[128*i+45],npub[128*i+44],npub[128*i+43],npub[128*i+42],npub[128*i+41],npub[128*i+40], npub[128*i+31],npub[128*i+30],npub[128*i+29],npub[128*i+28],npub[128*i+27],npub[128*i+26],npub[128*i+25],npub[128*i+24],npub[128*i+15],npub[128*i+14],npub[128*i+13],npub[128*i+12],npub[128*i+11],npub[128*i+10],npub[128*i+9],npub[128*i+8]);

		x[10*i+2] = _mm256_loadu2_m128i((void *) (k + 80 + 128*i),(void *) (k + 64 + 128*i));
		x[10*i+6] = _mm256_loadu2_m128i((void *) (npub + 80 + 128*i), (void *) (npub + 64 + 128*i));
		x[10*i+6] = x[10*i+6]&(_mm256_set_epi64x(0x0, 0xffffffffffffffff, 0x0, 0xffffffffffffffff));

		
		x[10*i+3] = _mm256_loadu2_m128i((void *) (k + 112 + 128*i),(void *) (k + 96 + 128*i));
		x[10*i+7] = _mm256_loadu2_m128i((void *) (npub + 112 + 128*i), (void *) (npub + 96 + 128*i));
		x[10*i+7] = x[10*i+7]&(_mm256_set_epi64x(0x0, 0xffffffffffffffff, 0x0, 0xffffffffffffffff));

		x[10*i+9] = _mm256_set_epi8(npub[128*i+127],npub[128*i+126],npub[128*i+125],npub[128*i+124],npub[128*i+123],npub[128*i+122],npub[128*i+121],npub[128*i+120],npub[128*i+111],npub[128*i+110],npub[128*i+109],npub[128*i+108],npub[128*i+107],npub[128*i+106],npub[128*i+105],npub[128*i+104],npub[128*i+95],npub[128*i+94],npub[128*i+93],npub[128*i+92],npub[128*i+91],npub[128*i+90],npub[128*i+89],npub[128*i+88],npub[128*i+79],npub[128*i+78],npub[128*i+77],npub[128*i+76],npub[128*i+75],npub[128*i+74],npub[128*i+73],npub[128*i+72]);


	}
	ace(x);
	for ( i = 0; i < PARAL_INST_BY8; i++)
	{
		x[10*i+0] = x[10*i+0]^_mm256_set_epi8(0x0,0x0,0x0,0x0,k[128*i+23],k[128*i+22],k[128*i+21],k[128*i+20],0x0,0x0,0x0,0x0,k[128*i+19],k[128*i+18],k[128*i+17],k[128*i+16],0x0,0x0,0x0,0x0,k[128*i+7],k[128*i+6],k[128*i+5],k[128*i+4],0x0,0x0,0x0,0x0,k[128*i+3],k[128*i+2],k[128*i+1],k[128*i+0]);
		x[10*i+1] = x[10*i+1]^_mm256_set_epi8(0x0,0x0,0x0,0x0,k[128*i+55],k[128*i+54],k[128*i+53],k[128*i+52],0x0,0x0,0x0,0x0,k[128*i+51],k[128*i+50],k[128*i+49],k[128*i+48],0x0,0x0,0x0,0x0,k[128*i+39],k[128*i+38],k[128*i+37],k[128*i+36],0x0,0x0,0x0,0x0,k[128*i+35],k[128*i+34],k[128*i+33],k[128*i+32]);
		x[10*i+2] = x[10*i+2]^_mm256_set_epi8(0x0,0x0,0x0,0x0,k[128*i+87],k[128*i+86],k[128*i+85],k[128*i+84],0x0,0x0,0x0,0x0,k[128*i+83],k[128*i+82],k[128*i+81],k[128*i+80],0x0,0x0,0x0,0x0,k[128*i+71],k[128*i+70],k[128*i+69],k[128*i+68],0x0,0x0,0x0,0x0,k[128*i+67],k[128*i+66],k[128*i+65],k[128*i+64]);
		x[10*i+3] = x[10*i+3]^_mm256_set_epi8(0x0,0x0,0x0,0x0,k[128*i+119],k[128*i+118],k[128*i+117],k[128*i+116],0x0,0x0,0x0,0x0,k[128*i+115],k[128*i+114],k[128*i+113],k[128*i+112],0x0,0x0,0x0,0x0,k[128*i+103],k[128*i+102],k[128*i+101],k[128*i+100],0x0,0x0,0x0,0x0,k[128*i+99],k[128*i+98],k[128*i+97],k[128*i+96]);

	}
	ace(x);
	for ( i = 0; i < PARAL_INST_BY8; i++)
	{
		x[10*i+0] = x[10*i+0]^_mm256_set_epi8(0x0,0x0,0x0,0x0,k[128*i+31],k[128*i+30],k[128*i+29],k[128*i+28],0x0,0x0,0x0,0x0,k[128*i+27],k[128*i+26],k[128*i+25],k[128*i+24],0x0,0x0,0x0,0x0,k[128*i+15],k[128*i+14],k[128*i+13],k[128*i+12],0x0,0x0,0x0,0x0,k[128*i+11],k[128*i+10],k[128*i+9],k[128*i+8]);
		x[10*i+1] = x[10*i+1]^_mm256_set_epi8(0x0,0x0,0x0,0x0,k[128*i+63],k[128*i+62],k[128*i+61],k[128*i+60],0x0,0x0,0x0,0x0,k[128*i+59],k[128*i+58],k[128*i+57],k[128*i+56],0x0,0x0,0x0,0x0,k[128*i+47],k[128*i+46],k[128*i+45],k[128*i+44],0x0,0x0,0x0,0x0,k[128*i+43],k[128*i+42],k[128*i+41],k[128*i+40]);
		x[10*i+2] = x[10*i+2]^_mm256_set_epi8(0x0,0x0,0x0,0x0,k[128*i+95],k[128*i+94],k[128*i+93],k[128*i+92],0x0,0x0,0x0,0x0,k[128*i+91],k[128*i+90],k[128*i+89],k[128*i+88],0x0,0x0,0x0,0x0,k[128*i+79],k[128*i+78],k[128*i+77],k[128*i+76],0x0,0x0,0x0,0x0,k[128*i+75],k[128*i+74],k[128*i+73],k[128*i+72]);
		x[10*i+3] = x[10*i+3]^_mm256_set_epi8(0x0,0x0,0x0,0x0,k[128*i+127],k[128*i+126],k[128*i+125],k[128*i+124],0x0,0x0,0x0,0x0,k[128*i+123],k[128*i+122],k[128*i+121],k[128*i+120],0x0,0x0,0x0,0x0,k[128*i+111],k[128*i+110],k[128*i+109],k[128*i+108],0x0,0x0,0x0,0x0,k[128*i+107],k[128*i+106],k[128*i+105],k[128*i+104]);
	}
	ace(x);
	
	
	//Proceesing Associated Data
	for ( l = 0; l < adblock; l++ )
	{
		for ( i = 0; i < PARAL_INST_BY8; i++)
		{
			index1 = 2*l + adlen*8*i;
			index2 = 2*l + adlen*8*i+1;
			index3 = 2*l + adlen*8*i + adlen;
			index4 = 2*l + adlen*8*i + adlen + 1;
			x[10*i+0] = x[10*i+0]^_mm256_set_epi32(0x0, ad[index4], 0x0, ad[index3], 0x0, ad[index2], 0x0, ad[index1]);
			
			index1 = 2*l + adlen*8*i+2*adlen;
			index2 = 2*l + adlen*8*i+1+2*adlen;
			index3 = 2*l + adlen*8*i + 3*adlen;
			index4 = 2*l + adlen*8*i + 3*adlen + 1;
			x[10*i+1] = x[10*i+1]^_mm256_set_epi32(0x0, ad[index4], 0x0, ad[index3], 0x0, ad[index2], 0x0, ad[index1]);
			
			index1 = 2*l + adlen*8*i+4*adlen;
			index2 = 2*l + adlen*8*i+1+4*adlen;
			index3 = 2*l + adlen*8*i + 5*adlen;
			index4 = 2*l + adlen*8*i + 5*adlen + 1;
			x[10*i+2] = x[10*i+2]^_mm256_set_epi32(0x0, ad[index4], 0x0, ad[index3], 0x0, ad[index2], 0x0, ad[index1]);
			
			index1 = 2*l + adlen*8*i+6*adlen;
			index2 = 2*l + adlen*8*i+1+6*adlen;
			index3 = 2*l + adlen*8*i + 7*adlen;
			index4 = 2*l + adlen*8*i + 7*adlen + 1;
			x[10*i+3] = x[10*i+3]^_mm256_set_epi32(0x0, ad[index4], 0x0, ad[index3], 0x0, ad[index2], 0x0, ad[index1]);

			//Domain seperator
			x[10*i+8] = _mm256_insert_epi32(x[10*i+8], _mm256_extract_epi32 (x[10*i+8], 1)^0x00000080, 1);
			x[10*i+8] = _mm256_insert_epi32(x[10*i+8], _mm256_extract_epi32 (x[10*i+8], 3)^0x00000080, 3);
			x[10*i+8] = _mm256_insert_epi32(x[10*i+8], _mm256_extract_epi32 (x[10*i+8], 5)^0x00000080, 5);
			x[10*i+8] = _mm256_insert_epi32(x[10*i+8], _mm256_extract_epi32 (x[10*i+8], 7)^0x00000080, 7);
		
			x[10*i+9] = _mm256_insert_epi32(x[10*i+9], _mm256_extract_epi32 (x[10*i+9], 1)^0x00000080, 1);
			x[10*i+9] = _mm256_insert_epi32(x[10*i+9], _mm256_extract_epi32 (x[10*i+9], 3)^0x00000080, 3);
			x[10*i+9] = _mm256_insert_epi32(x[10*i+9], _mm256_extract_epi32 (x[10*i+9], 5)^0x00000080, 5);
			x[10*i+9] = _mm256_insert_epi32(x[10*i+9], _mm256_extract_epi32 (x[10*i+9], 7)^0x00000080, 7);
		}
		ace(x);
	}

	//Encrypt plaintext
	for ( l = 0; l < mblock; l++ )
	{
		for ( i = 0; i < PARAL_INST_BY8; i++)
		{
			index1 = 2*l + mlen*8*i;
			index2 = 2*l + mlen*8*i+1;
			index3 = 2*l + mlen*8*i + mlen;
			index4 = 2*l + mlen*8*i + mlen + 1;
			c[index1] = _mm256_extract_epi32 (x[10*i+0], 0)^m[index1];
			c[index2] = _mm256_extract_epi32 (x[10*i+0], 2)^m[index2];
			c[index3] = _mm256_extract_epi32 (x[10*i+0], 4)^m[index3];
			c[index4] = _mm256_extract_epi32 (x[10*i+0], 6)^m[index4];
			x[10*i+0] = x[10*i+0]^_mm256_set_epi32(0x0, m[index4], 0x0, m[index3], 0x0, m[index2], 0x0, m[index1]);
			
			index1 = 2*l + mlen*8*i+2*mlen;
			index2 = 2*l + mlen*8*i+1+2*mlen;
			index3 = 2*l + mlen*8*i + 3*mlen;
			index4 = 2*l + mlen*8*i + 3*mlen + 1;
			c[index1] = _mm256_extract_epi32 (x[10*i+1], 0)^m[index1];
			c[index2] = _mm256_extract_epi32 (x[10*i+1], 2)^m[index2];
			c[index3] = _mm256_extract_epi32 (x[10*i+1], 4)^m[index3];
			c[index4] = _mm256_extract_epi32 (x[10*i+1], 6)^m[index4];
			x[10*i+1] = x[10*i+1]^_mm256_set_epi32(0x0, m[index4], 0x0, m[index3], 0x0, m[index2], 0x0, m[index1]);
			
			index1 = 2*l + mlen*8*i+4*mlen;
			index2 = 2*l + mlen*8*i+1+4*mlen;
			index3 = 2*l + mlen*8*i + 5*mlen;
			index4 = 2*l + mlen*8*i + 5*mlen + 1;
			c[index1] = _mm256_extract_epi32 (x[10*i+2], 0)^m[index1];
			c[index2] = _mm256_extract_epi32 (x[10*i+2], 2)^m[index2];
			c[index3] = _mm256_extract_epi32 (x[10*i+2], 4)^m[index3];
			c[index4] = _mm256_extract_epi32 (x[10*i+2], 6)^m[index4];
			x[10*i+2] = x[10*i+2]^_mm256_set_epi32(0x0, m[index4], 0x0, m[index3], 0x0, m[index2], 0x0, m[index1]);
			
			index1 = 2*l + mlen*8*i+6*mlen;
			index2 = 2*l + mlen*8*i+1+6*mlen;
			index3 = 2*l + mlen*8*i + 7*mlen;
			index4 = 2*l + mlen*8*i + 7*mlen + 1;
			c[index1] = _mm256_extract_epi32 (x[10*i+3], 0)^m[index1];
			c[index2] = _mm256_extract_epi32 (x[10*i+3], 2)^m[index2];
			c[index3] = _mm256_extract_epi32 (x[10*i+3], 4)^m[index3];
			c[index4] = _mm256_extract_epi32 (x[10*i+3], 6)^m[index4];
			x[10*i+3] = x[10*i+3]^_mm256_set_epi32(0x0, m[index4], 0x0, m[index3], 0x0, m[index2], 0x0, m[index1]);

			//Domain seperator
			x[10*i+8] = _mm256_insert_epi32(x[10*i+8], _mm256_extract_epi32 (x[10*i+8], 1)^0x00000040, 1);
			x[10*i+8] = _mm256_insert_epi32(x[10*i+8], _mm256_extract_epi32 (x[10*i+8], 3)^0x00000040, 3);
			x[10*i+8] = _mm256_insert_epi32(x[10*i+8], _mm256_extract_epi32 (x[10*i+8], 5)^0x00000040, 5);
			x[10*i+8] = _mm256_insert_epi32(x[10*i+8], _mm256_extract_epi32 (x[10*i+8], 7)^0x00000040, 7);
		
			x[10*i+9] = _mm256_insert_epi32(x[10*i+9], _mm256_extract_epi32 (x[10*i+9], 1)^0x00000040, 1);
			x[10*i+9] = _mm256_insert_epi32(x[10*i+9], _mm256_extract_epi32 (x[10*i+9], 3)^0x00000040, 3);
			x[10*i+9] = _mm256_insert_epi32(x[10*i+9], _mm256_extract_epi32 (x[10*i+9], 5)^0x00000040, 5);
			x[10*i+9] = _mm256_insert_epi32(x[10*i+9], _mm256_extract_epi32 (x[10*i+9], 7)^0x00000040, 7);
		}
		ace(x);
	}
	
	// Tag Extraction Phase
	for ( i = 0; i < PARAL_INST_BY8; i++)
	{
		x[10*i+0] = x[10*i+0]^_mm256_set_epi8(0x0,0x0,0x0,0x0,k[128*i+23],k[128*i+22],k[128*i+21],k[128*i+20],0x0,0x0,0x0,0x0,k[128*i+19],k[128*i+18],k[128*i+17],k[128*i+16],0x0,0x0,0x0,0x0,k[128*i+7],k[128*i+6],k[128*i+5],k[128*i+4],0x0,0x0,0x0,0x0,k[128*i+3],k[128*i+2],k[128*i+1],k[128*i+0]);
		x[10*i+1] = x[10*i+1]^_mm256_set_epi8(0x0,0x0,0x0,0x0,k[128*i+55],k[128*i+54],k[128*i+53],k[128*i+52],0x0,0x0,0x0,0x0,k[128*i+51],k[128*i+50],k[128*i+49],k[128*i+48],0x0,0x0,0x0,0x0,k[128*i+39],k[128*i+38],k[128*i+37],k[128*i+36],0x0,0x0,0x0,0x0,k[128*i+35],k[128*i+34],k[128*i+33],k[128*i+32]);
		x[10*i+2] = x[10*i+2]^_mm256_set_epi8(0x0,0x0,0x0,0x0,k[128*i+87],k[128*i+86],k[128*i+85],k[128*i+84],0x0,0x0,0x0,0x0,k[128*i+83],k[128*i+82],k[128*i+81],k[128*i+80],0x0,0x0,0x0,0x0,k[128*i+71],k[128*i+70],k[128*i+69],k[128*i+68],0x0,0x0,0x0,0x0,k[128*i+67],k[128*i+66],k[128*i+65],k[128*i+64]);
		x[10*i+3] = x[10*i+3]^_mm256_set_epi8(0x0,0x0,0x0,0x0,k[128*i+119],k[128*i+118],k[128*i+117],k[128*i+116],0x0,0x0,0x0,0x0,k[128*i+115],k[128*i+114],k[128*i+113],k[128*i+112],0x0,0x0,0x0,0x0,k[128*i+103],k[128*i+102],k[128*i+101],k[128*i+100],0x0,0x0,0x0,0x0,k[128*i+99],k[128*i+98],k[128*i+97],k[128*i+96]);

	}
	ace(x);
	for ( i = 0; i < PARAL_INST_BY8; i++)
	{
		x[10*i+0] = x[10*i+0]^_mm256_set_epi8(0x0,0x0,0x0,0x0,k[128*i+31],k[128*i+30],k[128*i+29],k[128*i+28],0x0,0x0,0x0,0x0,k[128*i+27],k[128*i+26],k[128*i+25],k[128*i+24],0x0,0x0,0x0,0x0,k[128*i+15],k[128*i+14],k[128*i+13],k[128*i+12],0x0,0x0,0x0,0x0,k[128*i+11],k[128*i+10],k[128*i+9],k[128*i+8]);
		x[10*i+1] = x[10*i+1]^_mm256_set_epi8(0x0,0x0,0x0,0x0,k[128*i+63],k[128*i+62],k[128*i+61],k[128*i+60],0x0,0x0,0x0,0x0,k[128*i+59],k[128*i+58],k[128*i+57],k[128*i+56],0x0,0x0,0x0,0x0,k[128*i+47],k[128*i+46],k[128*i+45],k[128*i+44],0x0,0x0,0x0,0x0,k[128*i+43],k[128*i+42],k[128*i+41],k[128*i+40]);
		x[10*i+2] = x[10*i+2]^_mm256_set_epi8(0x0,0x0,0x0,0x0,k[128*i+95],k[128*i+94],k[128*i+93],k[128*i+92],0x0,0x0,0x0,0x0,k[128*i+91],k[128*i+90],k[128*i+89],k[128*i+88],0x0,0x0,0x0,0x0,k[128*i+79],k[128*i+78],k[128*i+77],k[128*i+76],0x0,0x0,0x0,0x0,k[128*i+75],k[128*i+74],k[128*i+73],k[128*i+72]);
		x[10*i+3] = x[10*i+3]^_mm256_set_epi8(0x0,0x0,0x0,0x0,k[128*i+127],k[128*i+126],k[128*i+125],k[128*i+124],0x0,0x0,0x0,0x0,k[128*i+123],k[128*i+122],k[128*i+121],k[128*i+120],0x0,0x0,0x0,0x0,k[128*i+111],k[128*i+110],k[128*i+109],k[128*i+108],0x0,0x0,0x0,0x0,k[128*i+107],k[128*i+106],k[128*i+105],k[128*i+104]);

	}
	ace(x);
	for ( i = 0; i < PARAL_INST_BY8; i++)
	{
		 _mm_storeu_si128((void *)(tag+32*i),_mm256_extracti128_si256(x[10*i],0));
		 _mm_storeu_si128((void *)(tag+32*i+4),_mm256_extracti128_si256(x[10*i],1));
		 
		 _mm_storeu_si128((void *)(tag+32*i+8),_mm256_extracti128_si256(x[10*i+1],0));
		 _mm_storeu_si128((void *)(tag+32*i+12),_mm256_extracti128_si256(x[10*i+1],1));
		 
		 _mm_storeu_si128((void *)(tag+32*i+16),_mm256_extracti128_si256(x[10*i+2],0));
		 _mm_storeu_si128((void *)(tag+32*i+20),_mm256_extracti128_si256(x[10*i+2],1));
		 
		 _mm_storeu_si128((void *)(tag+32*i+24),_mm256_extracti128_si256(x[10*i+3],0));
		 _mm_storeu_si128((void *)(tag+32*i+28),_mm256_extracti128_si256(x[10*i+3],1));
	}
return 0;
}

int crypto_aead_decrypt( u32 *m, u32 *c, u32 mlen, u32 *tag, u32 tlen, u32 *ad, u32 adlen, u8 *k, u8 *npub, u32 klen )
{
        u8 i;
        u64 l, mblock, adblock;
        __m256i x[10*PARAL_INST_BY8];
        //__m256i xtmp, ytmp;
	u32 index1, index2, index3, index4;

	mblock = mlen/2;
	adblock = adlen/2;
       
	//Initialization: Loading Key & Nonce
        for ( i = 0; i < PARAL_INST_BY8; i++ )
	{
		//Key and Nonce are loaded according to Packing.

		x[10*i+0] = _mm256_loadu2_m128i((void *) (k + 16 + 128*i), (void *) (k + 0 + 128*i));
		
		x[10*i+4] = _mm256_loadu2_m128i((void *) (npub + 16 + 128*i), (void *) (npub + 0 + 128*i));
		x[10*i+4] = x[10*i+4]&(_mm256_set_epi64x(0x0, 0xffffffffffffffff, 0x0, 0xffffffffffffffff));


		
		x[10*i+1] = _mm256_loadu2_m128i((void *) (k + 48 + 128*i),(void *) (k + 32 + 128*i));
		x[10*i+5] = _mm256_loadu2_m128i((void *) (npub + 48 + 128*i), (void *) (npub + 32 + 128*i));
		x[10*i+5] = x[10*i+5]&(_mm256_set_epi64x(0x0, 0xffffffffffffffff, 0x0, 0xffffffffffffffff));

		x[10*i+8] = _mm256_set_epi8(npub[128*i+63],npub[128*i+62],npub[128*i+61],npub[128*i+60],npub[128*i+59],npub[128*i+58],npub[128*i+57],npub[128*i+56], npub[128*i+47],npub[128*i+46],npub[128*i+45],npub[128*i+44],npub[128*i+43],npub[128*i+42],npub[128*i+41],npub[128*i+40], npub[128*i+31],npub[128*i+30],npub[128*i+29],npub[128*i+28],npub[128*i+27],npub[128*i+26],npub[128*i+25],npub[128*i+24],npub[128*i+15],npub[128*i+14],npub[128*i+13],npub[128*i+12],npub[128*i+11],npub[128*i+10],npub[128*i+9],npub[128*i+8]);

		x[10*i+2] = _mm256_loadu2_m128i((void *) (k + 80 + 128*i),(void *) (k + 64 + 128*i));

		x[10*i+6] = _mm256_loadu2_m128i((void *) (npub + 80 + 128*i), (void *) (npub + 64 + 128*i));
		x[10*i+6] = x[10*i+6]&(_mm256_set_epi64x(0x0, 0xffffffffffffffff, 0x0, 0xffffffffffffffff));

		
		x[10*i+3] = _mm256_loadu2_m128i((void *) (k + 112 + 128*i),(void *) (k + 96 + 128*i));
		x[10*i+7] = _mm256_loadu2_m128i((void *) (npub + 112 + 128*i), (void *) (npub + 96 + 128*i));
		x[10*i+7] = x[10*i+7]&(_mm256_set_epi64x(0x0, 0xffffffffffffffff, 0x0, 0xffffffffffffffff));

		x[10*i+9] = _mm256_set_epi8(npub[128*i+127],npub[128*i+126],npub[128*i+125],npub[128*i+124],npub[128*i+123],npub[128*i+122],npub[128*i+121],npub[128*i+120],npub[128*i+111],npub[128*i+110],npub[128*i+109],npub[128*i+108],npub[128*i+107],npub[128*i+106],npub[128*i+105],npub[128*i+104],npub[128*i+95],npub[128*i+94],npub[128*i+93],npub[128*i+92],npub[128*i+91],npub[128*i+90],npub[128*i+89],npub[128*i+88],npub[128*i+79],npub[128*i+78],npub[128*i+77],npub[128*i+76],npub[128*i+75],npub[128*i+74],npub[128*i+73],npub[128*i+72]);


	}
	ace(x);
	for ( i = 0; i < PARAL_INST_BY8; i++)
	{
		x[10*i+0] = x[10*i+0]^_mm256_set_epi8(0x0,0x0,0x0,0x0,k[128*i+23],k[128*i+22],k[128*i+21],k[128*i+20],0x0,0x0,0x0,0x0,k[128*i+19],k[128*i+18],k[128*i+17],k[128*i+16],0x0,0x0,0x0,0x0,k[128*i+7],k[128*i+6],k[128*i+5],k[128*i+4],0x0,0x0,0x0,0x0,k[128*i+3],k[128*i+2],k[128*i+1],k[128*i+0]);
		x[10*i+1] = x[10*i+1]^_mm256_set_epi8(0x0,0x0,0x0,0x0,k[128*i+55],k[128*i+54],k[128*i+53],k[128*i+52],0x0,0x0,0x0,0x0,k[128*i+51],k[128*i+50],k[128*i+49],k[128*i+48],0x0,0x0,0x0,0x0,k[128*i+39],k[128*i+38],k[128*i+37],k[128*i+36],0x0,0x0,0x0,0x0,k[128*i+35],k[128*i+34],k[128*i+33],k[128*i+32]);
		x[10*i+2] = x[10*i+2]^_mm256_set_epi8(0x0,0x0,0x0,0x0,k[128*i+87],k[128*i+86],k[128*i+85],k[128*i+84],0x0,0x0,0x0,0x0,k[128*i+83],k[128*i+82],k[128*i+81],k[128*i+80],0x0,0x0,0x0,0x0,k[128*i+71],k[128*i+70],k[128*i+69],k[128*i+68],0x0,0x0,0x0,0x0,k[128*i+67],k[128*i+66],k[128*i+65],k[128*i+64]);
		x[10*i+3] = x[10*i+3]^_mm256_set_epi8(0x0,0x0,0x0,0x0,k[128*i+119],k[128*i+118],k[128*i+117],k[128*i+116],0x0,0x0,0x0,0x0,k[128*i+115],k[128*i+114],k[128*i+113],k[128*i+112],0x0,0x0,0x0,0x0,k[128*i+103],k[128*i+102],k[128*i+101],k[128*i+100],0x0,0x0,0x0,0x0,k[128*i+99],k[128*i+98],k[128*i+97],k[128*i+96]);

	}
	ace(x);
	for ( i = 0; i < PARAL_INST_BY8; i++)
	{
		x[10*i+0] = x[10*i+0]^_mm256_set_epi8(0x0,0x0,0x0,0x0,k[128*i+31],k[128*i+30],k[128*i+29],k[128*i+28],0x0,0x0,0x0,0x0,k[128*i+27],k[128*i+26],k[128*i+25],k[128*i+24],0x0,0x0,0x0,0x0,k[128*i+15],k[128*i+14],k[128*i+13],k[128*i+12],0x0,0x0,0x0,0x0,k[128*i+11],k[128*i+10],k[128*i+9],k[128*i+8]);
		x[10*i+1] = x[10*i+1]^_mm256_set_epi8(0x0,0x0,0x0,0x0,k[128*i+63],k[128*i+62],k[128*i+61],k[128*i+60],0x0,0x0,0x0,0x0,k[128*i+59],k[128*i+58],k[128*i+57],k[128*i+56],0x0,0x0,0x0,0x0,k[128*i+47],k[128*i+46],k[128*i+45],k[128*i+44],0x0,0x0,0x0,0x0,k[128*i+43],k[128*i+42],k[128*i+41],k[128*i+40]);
		x[10*i+2] = x[10*i+2]^_mm256_set_epi8(0x0,0x0,0x0,0x0,k[128*i+95],k[128*i+94],k[128*i+93],k[128*i+92],0x0,0x0,0x0,0x0,k[128*i+91],k[128*i+90],k[128*i+89],k[128*i+88],0x0,0x0,0x0,0x0,k[128*i+79],k[128*i+78],k[128*i+77],k[128*i+76],0x0,0x0,0x0,0x0,k[128*i+75],k[128*i+74],k[128*i+73],k[128*i+72]);
		x[10*i+3] = x[10*i+3]^_mm256_set_epi8(0x0,0x0,0x0,0x0,k[128*i+127],k[128*i+126],k[128*i+125],k[128*i+124],0x0,0x0,0x0,0x0,k[128*i+123],k[128*i+122],k[128*i+121],k[128*i+120],0x0,0x0,0x0,0x0,k[128*i+111],k[128*i+110],k[128*i+109],k[128*i+108],0x0,0x0,0x0,0x0,k[128*i+107],k[128*i+106],k[128*i+105],k[128*i+104]);
	}
	ace(x);
	
	
	//Proceesing Associated Data
	for ( l = 0; l < adblock; l++ )
	{
		for ( i = 0; i < PARAL_INST_BY8; i++)
		{
			index1 = 2*l + adlen*8*i;
			index2 = 2*l + adlen*8*i+1;
			index3 = 2*l + adlen*8*i + adlen;
			index4 = 2*l + adlen*8*i + adlen + 1;
			x[10*i+0] = x[10*i+0]^_mm256_set_epi32(0x0, ad[index4], 0x0, ad[index3], 0x0, ad[index2], 0x0, ad[index1]);
			
			index1 = 2*l + adlen*8*i+2*adlen;
			index2 = 2*l + adlen*8*i+1+2*adlen;
			index3 = 2*l + adlen*8*i + 3*adlen;
			index4 = 2*l + adlen*8*i + 3*adlen + 1;
			x[10*i+1] = x[10*i+1]^_mm256_set_epi32(0x0, ad[index4], 0x0, ad[index3], 0x0, ad[index2], 0x0, ad[index1]);
			
			index1 = 2*l + adlen*8*i+4*adlen;
			index2 = 2*l + adlen*8*i+1+4*adlen;
			index3 = 2*l + adlen*8*i + 5*adlen;
			index4 = 2*l + adlen*8*i + 5*adlen + 1;
			x[10*i+2] = x[10*i+2]^_mm256_set_epi32(0x0, ad[index4], 0x0, ad[index3], 0x0, ad[index2], 0x0, ad[index1]);
			
			index1 = 2*l + adlen*8*i+6*adlen;
			index2 = 2*l + adlen*8*i+1+6*adlen;
			index3 = 2*l + adlen*8*i + 7*adlen;
			index4 = 2*l + adlen*8*i + 7*adlen + 1;
			x[10*i+3] = x[10*i+3]^_mm256_set_epi32(0x0, ad[index4], 0x0, ad[index3], 0x0, ad[index2], 0x0, ad[index1]);

			//Domain seperator
			x[10*i+8] = _mm256_insert_epi32(x[10*i+8], _mm256_extract_epi32 (x[10*i+8], 1)^0x00000080, 1);
			x[10*i+8] = _mm256_insert_epi32(x[10*i+8], _mm256_extract_epi32 (x[10*i+8], 3)^0x00000080, 3);
			x[10*i+8] = _mm256_insert_epi32(x[10*i+8], _mm256_extract_epi32 (x[10*i+8], 5)^0x00000080, 5);
			x[10*i+8] = _mm256_insert_epi32(x[10*i+8], _mm256_extract_epi32 (x[10*i+8], 7)^0x00000080, 7);
		
			x[10*i+9] = _mm256_insert_epi32(x[10*i+9], _mm256_extract_epi32 (x[10*i+9], 1)^0x00000080, 1);
			x[10*i+9] = _mm256_insert_epi32(x[10*i+9], _mm256_extract_epi32 (x[10*i+9], 3)^0x00000080, 3);
			x[10*i+9] = _mm256_insert_epi32(x[10*i+9], _mm256_extract_epi32 (x[10*i+9], 5)^0x00000080, 5);
			x[10*i+9] = _mm256_insert_epi32(x[10*i+9], _mm256_extract_epi32 (x[10*i+9], 7)^0x00000080, 7);
		}
		ace(x);
	}
        
	//Encrypt plaintext
	for ( l = 0; l < mblock; l++ )
	{
		for ( i = 0; i < PARAL_INST_BY8; i++)
		{
			index1 = 2*l + mlen*8*i;
			index2 = 2*l + mlen*8*i+1;
			index3 = 2*l + mlen*8*i + mlen;
			index4 = 2*l + mlen*8*i + mlen + 1;
			m[index1] = _mm256_extract_epi32 (x[10*i+0], 0)^c[index1];
			m[index2] = _mm256_extract_epi32 (x[10*i+0], 2)^c[index2];
			m[index3] = _mm256_extract_epi32 (x[10*i+0], 4)^c[index3];
			m[index4] = _mm256_extract_epi32 (x[10*i+0], 6)^c[index4];
			x[10*i+0] = x[10*i+0]^_mm256_set_epi32(0x0, m[index4], 0x0, m[index3], 0x0, m[index2], 0x0, m[index1]);
			
			index1 = 2*l + mlen*8*i+2*mlen;
			index2 = 2*l + mlen*8*i+1+2*mlen;
			index3 = 2*l + mlen*8*i + 3*mlen;
			index4 = 2*l + mlen*8*i + 3*mlen + 1;
			m[index1] = _mm256_extract_epi32 (x[10*i+1], 0)^c[index1];
			m[index2] = _mm256_extract_epi32 (x[10*i+1], 2)^c[index2];
			m[index3] = _mm256_extract_epi32 (x[10*i+1], 4)^c[index3];
			m[index4] = _mm256_extract_epi32 (x[10*i+1], 6)^c[index4];
			x[10*i+1] = x[10*i+1]^_mm256_set_epi32(0x0, m[index4], 0x0, m[index3], 0x0, m[index2], 0x0, m[index1]);
			
			index1 = 2*l + mlen*8*i+4*mlen;
			index2 = 2*l + mlen*8*i+1+4*mlen;
			index3 = 2*l + mlen*8*i + 5*mlen;
			index4 = 2*l + mlen*8*i + 5*mlen + 1;
			m[index1] = _mm256_extract_epi32 (x[10*i+2], 0)^c[index1];
			m[index2] = _mm256_extract_epi32 (x[10*i+2], 2)^c[index2];
			m[index3] = _mm256_extract_epi32 (x[10*i+2], 4)^c[index3];
			m[index4] = _mm256_extract_epi32 (x[10*i+2], 6)^c[index4];
			x[10*i+2] = x[10*i+2]^_mm256_set_epi32(0x0, m[index4], 0x0, m[index3], 0x0, m[index2], 0x0, m[index1]);
			
			index1 = 2*l + mlen*8*i+6*mlen;
			index2 = 2*l + mlen*8*i+1+6*mlen;
			index3 = 2*l + mlen*8*i + 7*mlen;
			index4 = 2*l + mlen*8*i + 7*mlen + 1;
			m[index1] = _mm256_extract_epi32 (x[10*i+3], 0)^c[index1];
			m[index2] = _mm256_extract_epi32 (x[10*i+3], 2)^c[index2];
			m[index3] = _mm256_extract_epi32 (x[10*i+3], 4)^c[index3];
			m[index4] = _mm256_extract_epi32 (x[10*i+3], 6)^c[index4];
			x[10*i+3] = x[10*i+3]^_mm256_set_epi32(0x0, m[index4], 0x0, m[index3], 0x0, m[index2], 0x0, m[index1]);

			//Domain seperator
			x[10*i+8] = _mm256_insert_epi32(x[10*i+8], _mm256_extract_epi32 (x[10*i+8], 1)^0x00000040, 1);
			x[10*i+8] = _mm256_insert_epi32(x[10*i+8], _mm256_extract_epi32 (x[10*i+8], 3)^0x00000040, 3);
			x[10*i+8] = _mm256_insert_epi32(x[10*i+8], _mm256_extract_epi32 (x[10*i+8], 5)^0x00000040, 5);
			x[10*i+8] = _mm256_insert_epi32(x[10*i+8], _mm256_extract_epi32 (x[10*i+8], 7)^0x00000040, 7);
		
			x[10*i+9] = _mm256_insert_epi32(x[10*i+9], _mm256_extract_epi32 (x[10*i+9], 1)^0x00000040, 1);
			x[10*i+9] = _mm256_insert_epi32(x[10*i+9], _mm256_extract_epi32 (x[10*i+9], 3)^0x00000040, 3);
			x[10*i+9] = _mm256_insert_epi32(x[10*i+9], _mm256_extract_epi32 (x[10*i+9], 5)^0x00000040, 5);
			x[10*i+9] = _mm256_insert_epi32(x[10*i+9], _mm256_extract_epi32 (x[10*i+9], 7)^0x00000040, 7);
		}
		ace(x);
	}

	// Tag Extraction Phase
	for ( i = 0; i < PARAL_INST_BY8; i++)
	{
		x[10*i+0] = x[10*i+0]^_mm256_set_epi8(0x0,0x0,0x0,0x0,k[128*i+23],k[128*i+22],k[128*i+21],k[128*i+20],0x0,0x0,0x0,0x0,k[128*i+19],k[128*i+18],k[128*i+17],k[128*i+16],0x0,0x0,0x0,0x0,k[128*i+7],k[128*i+6],k[128*i+5],k[128*i+4],0x0,0x0,0x0,0x0,k[128*i+3],k[128*i+2],k[128*i+1],k[128*i+0]);
		x[10*i+1] = x[10*i+1]^_mm256_set_epi8(0x0,0x0,0x0,0x0,k[128*i+55],k[128*i+54],k[128*i+53],k[128*i+52],0x0,0x0,0x0,0x0,k[128*i+51],k[128*i+50],k[128*i+49],k[128*i+48],0x0,0x0,0x0,0x0,k[128*i+39],k[128*i+38],k[128*i+37],k[128*i+36],0x0,0x0,0x0,0x0,k[128*i+35],k[128*i+34],k[128*i+33],k[128*i+32]);
		x[10*i+2] = x[10*i+2]^_mm256_set_epi8(0x0,0x0,0x0,0x0,k[128*i+87],k[128*i+86],k[128*i+85],k[128*i+84],0x0,0x0,0x0,0x0,k[128*i+83],k[128*i+82],k[128*i+81],k[128*i+80],0x0,0x0,0x0,0x0,k[128*i+71],k[128*i+70],k[128*i+69],k[128*i+68],0x0,0x0,0x0,0x0,k[128*i+67],k[128*i+66],k[128*i+65],k[128*i+64]);
		x[10*i+3] = x[10*i+3]^_mm256_set_epi8(0x0,0x0,0x0,0x0,k[128*i+119],k[128*i+118],k[128*i+117],k[128*i+116],0x0,0x0,0x0,0x0,k[128*i+115],k[128*i+114],k[128*i+113],k[128*i+112],0x0,0x0,0x0,0x0,k[128*i+103],k[128*i+102],k[128*i+101],k[128*i+100],0x0,0x0,0x0,0x0,k[128*i+99],k[128*i+98],k[128*i+97],k[128*i+96]);

	}
	ace(x);
	for ( i = 0; i < PARAL_INST_BY8; i++)
	{
		x[10*i+0] = x[10*i+0]^_mm256_set_epi8(0x0,0x0,0x0,0x0,k[128*i+31],k[128*i+30],k[128*i+29],k[128*i+28],0x0,0x0,0x0,0x0,k[128*i+27],k[128*i+26],k[128*i+25],k[128*i+24],0x0,0x0,0x0,0x0,k[128*i+15],k[128*i+14],k[128*i+13],k[128*i+12],0x0,0x0,0x0,0x0,k[128*i+11],k[128*i+10],k[128*i+9],k[128*i+8]);
		x[10*i+1] = x[10*i+1]^_mm256_set_epi8(0x0,0x0,0x0,0x0,k[128*i+63],k[128*i+62],k[128*i+61],k[128*i+60],0x0,0x0,0x0,0x0,k[128*i+59],k[128*i+58],k[128*i+57],k[128*i+56],0x0,0x0,0x0,0x0,k[128*i+47],k[128*i+46],k[128*i+45],k[128*i+44],0x0,0x0,0x0,0x0,k[128*i+43],k[128*i+42],k[128*i+41],k[128*i+40]);
		x[10*i+2] = x[10*i+2]^_mm256_set_epi8(0x0,0x0,0x0,0x0,k[128*i+95],k[128*i+94],k[128*i+93],k[128*i+92],0x0,0x0,0x0,0x0,k[128*i+91],k[128*i+90],k[128*i+89],k[128*i+88],0x0,0x0,0x0,0x0,k[128*i+79],k[128*i+78],k[128*i+77],k[128*i+76],0x0,0x0,0x0,0x0,k[128*i+75],k[128*i+74],k[128*i+73],k[128*i+72]);
		x[10*i+3] = x[10*i+3]^_mm256_set_epi8(0x0,0x0,0x0,0x0,k[128*i+127],k[128*i+126],k[128*i+125],k[128*i+124],0x0,0x0,0x0,0x0,k[128*i+123],k[128*i+122],k[128*i+121],k[128*i+120],0x0,0x0,0x0,0x0,k[128*i+111],k[128*i+110],k[128*i+109],k[128*i+108],0x0,0x0,0x0,0x0,k[128*i+107],k[128*i+106],k[128*i+105],k[128*i+104]);
	}
	ace(x);
	for ( i = 0; i < PARAL_INST_BY8; i++)
	{
		 _mm_storeu_si128((void *)(tag+32*i),_mm256_extracti128_si256(x[10*i],0));
		 _mm_storeu_si128((void *)(tag+32*i+4),_mm256_extracti128_si256(x[10*i],1));
		 
		 _mm_storeu_si128((void *)(tag+32*i+8),_mm256_extracti128_si256(x[10*i+1],0));
		 _mm_storeu_si128((void *)(tag+32*i+12),_mm256_extracti128_si256(x[10*i+1],1));
		 
		 _mm_storeu_si128((void *)(tag+32*i+16),_mm256_extracti128_si256(x[10*i+2],0));
		 _mm_storeu_si128((void *)(tag+32*i+20),_mm256_extracti128_si256(x[10*i+2],1));
		 
		 _mm_storeu_si128((void *)(tag+32*i+24),_mm256_extracti128_si256(x[10*i+3],0));
		 _mm_storeu_si128((void *)(tag+32*i+28),_mm256_extracti128_si256(x[10*i+3],1));
	}
return 0;
}
