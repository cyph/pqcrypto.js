/* Reference implementation of ACE-128, AEAD
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

                        //Diffussion
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


int crypto_aead_encrypt( u32 *tag, u32 tlen, u32 *c, u32 *m, u32 mlen, u32 *ad, u32 adlen, u8 *k, u8 *npub, u32 klen )
{
        u8 i;
        u64 l, mblock, adblock;
        __m128i x[10*PARAL_INST_BY4];
	u32 index1, index2;

	mblock = mlen/2;
	adblock = adlen/2;
       
	//Initialization: Loading Key & Nonce
        for ( i = 0; i < PARAL_INST_BY4; i++ )
	{
		//Key and Nonce are loaded according to Packing.
		x[10*i+0] = _mm_loadu_si128((void *) (k+0+64*i));
		x[10*i+4] = _mm_loadl_epi64 ((const __m128i *)(npub+0+64*i));
		
		x[10*i+1] = _mm_loadu_si128((void *) (k+16+64*i));
		x[10*i+5] = _mm_loadl_epi64 ((const __m128i*)(npub + 16+64*i));

		x[10*i+8] = _mm_set_epi8( *(npub+31+64*i), *(npub+30+64*i), *(npub+29+64*i), *(npub+28+64*i), *(npub+27+64*i), *(npub+26+64*i), *(npub+25+64*i), *(npub+24+64*i), *(npub+15+64*i), *(npub+14+64*i), *(npub+13+64*i), *(npub+12+64*i), *(npub+11+64*i), *(npub+10+64*i), *(npub+9+64*i), *(npub+8+64*i));


		x[10*i+2] = _mm_loadu_si128((void *) (k+32+64*i));
		x[10*i+6] = _mm_loadl_epi64 ((const __m128i*)(npub+32+64*i));
		
		x[10*i+3] = _mm_loadu_si128((void *) (k+48+64*i));
		x[10*i+7] = _mm_loadl_epi64 ((const __m128i*)(npub+48+64*i));

		x[10*i+9] = _mm_set_epi8( *(npub+63+64*i), *(npub+62+64*i), *(npub+61+64*i), *(npub+60+64*i), *(npub+59+64*i), *(npub+58+64*i), *(npub+57+64*i), *(npub+56+64*i), *(npub+47+64*i), *(npub+46+64*i), *(npub+45+64*i), *(npub+44+64*i), *(npub+43+64*i), *(npub+42+64*i), *(npub+41+64*i), *(npub+40+64*i));

	}
	ace(x);
	for ( i = 0; i < PARAL_INST_BY4; i++)
	{
		x[10*i+0] = x[10*i+0]^_mm_set_epi8(0x0, 0x0, 0x0, 0x0, *(k+7+64*i), *(k+6+64*i), *(k+5+64*i), *(k+4+64*i), 0x0, 0x0, 0x0, 0x0, *(k+3+64*i), *(k+2+64*i), *(k+1+64*i), *(k+64*i));
		x[10*i+1] = x[10*i+1]^_mm_set_epi8(0x0, 0x0, 0x0, 0x0, *(k+23+64*i), *(k+22+64*i), *(k+21+64*i), *(k+20+64*i), 0x0, 0x0, 0x0, 0x0, *(k+19+64*i), *(k+18+64*i), *(k+17+64*i), *(k+16+64*i));
		x[10*i+2] = x[10*i+2]^_mm_set_epi8(0x0, 0x0, 0x0, 0x0, *(k+39+64*i), *(k+38+64*i), *(k+37+64*i), *(k+36+64*i), 0x0, 0x0, 0x0, 0x0, *(k+35+64*i), *(k+34+64*i), *(k+33+64*i), *(k+32+64*i));
		x[10*i+3] = x[10*i+3]^_mm_set_epi8(0x0, 0x0, 0x0, 0x0, *(k+55+64*i), *(k+54+64*i), *(k+53+64*i), *(k+52+64*i), 0x0, 0x0, 0x0, 0x0, *(k+51+64*i), *(k+50+64*i), *(k+49+64*i), *(k+48+64*i));
	}
	ace(x);
	for ( i = 0; i < PARAL_INST_BY4; i++)
	{
		x[10*i+0] = x[10*i+0]^_mm_set_epi8(0x0, 0x0, 0x0, 0x0, *(k+15+64*i), *(k+14+64*i), *(k+13+64*i), *(k+12+64*i), 0x0, 0x0, 0x0, 0x0, *(k+11+64*i), *(k+10+64*i), *(k+9+64*i), *(k+8+64*i));
		x[10*i+1] = x[10*i+1]^_mm_set_epi8(0x0, 0x0, 0x0, 0x0, *(k+31+64*i), *(k+30+64*i), *(k+29+64*i), *(k+28+64*i), 0x0, 0x0, 0x0, 0x0, *(k+27+64*i), *(k+26+64*i), *(k+25+64*i), *(k+24+64*i));
		x[10*i+2] = x[10*i+2]^_mm_set_epi8(0x0, 0x0, 0x0, 0x0, *(k+47+64*i), *(k+46+64*i), *(k+45+64*i), *(k+44+64*i), 0x0, 0x0, 0x0, 0x0, *(k+43+64*i), *(k+42+64*i), *(k+41+64*i), *(k+40+64*i));
		x[10*i+3] = x[10*i+3]^_mm_set_epi8(0x0, 0x0, 0x0, 0x0, *(k+63+64*i), *(k+62+64*i), *(k+61+64*i), *(k+60+64*i), 0x0, 0x0, 0x0, 0x0, *(k+59+64*i), *(k+58+64*i), *(k+57+64*i), *(k+56+64*i));
	}
	ace(x);

	//Proceesing Associated Data
	for ( l = 0; l < adblock; l++ )
	{
		for ( i = 0; i < PARAL_INST_BY4; i++)
		{
			index1 = 2*l + adlen*4*i;
			index2 = 2*l + adlen*4*i+1;
			x[10*i+0] = x[10*i+0]^_mm_set_epi32(0x0, ad[index2], 0x0, ad[index1]);
			
			index1 = 2*l + adlen*4*i+ adlen;
			index2 = 2*l + adlen*4*i+ adlen+1;
			x[10*i+1] = x[10*i+1]^_mm_set_epi32(0x0, ad[index2], 0x0, ad[index1]);
			
			index1 = 2*l + adlen*4*i+ 2*adlen;
			index2 = 2*l + adlen*4*i+ 2*adlen+1;
			x[10*i+2] = x[10*i+2]^_mm_set_epi32(0x0, ad[index2], 0x0, ad[index1]);
			
			index1 = 2*l + adlen*4*i+ 3*adlen;
			index2 = 2*l + adlen*4*i+ 3*adlen+1;
			x[10*i+3] = x[10*i+3]^_mm_set_epi32(0x0, ad[index2], 0x0, ad[index1]);

			//Domain seperator
			x[10*i+8] = _mm_insert_epi32(x[10*i+8], _mm_extract_epi32 (x[10*i+8], 1)^0x00000080, 1 );
			x[10*i+8] = _mm_insert_epi32(x[10*i+8], _mm_extract_epi32 (x[10*i+8], 3)^0x00000080, 3 );
			x[10*i+9] = _mm_insert_epi32(x[10*i+9], _mm_extract_epi32 (x[10*i+9], 1)^0x00000080, 1 );
			x[10*i+9] = _mm_insert_epi32(x[10*i+9], _mm_extract_epi32 (x[10*i+9], 3)^0x00000080, 3 );
		}
		ace(x);
	}
	//Encrypt plaintext
	for ( l = 0; l < mblock; l++ )
	{
		for ( i = 0; i < PARAL_INST_BY4; i++)
		{
			index1 = 2*l + mlen*4*i;
			index2 = 2*l + mlen*4*i+1;
			c[index1] = _mm_extract_epi32 (x[10*i+0], 0)^m[index1];
			c[index2] = _mm_extract_epi32 (x[10*i+0], 2)^m[index2];
			x[10*i+0] = x[10*i+0]^_mm_set_epi32(0x0, m[index2], 0x0, m[index1]);
			
			index1 = 2*l + mlen*4*i+ mlen;
			index2 = 2*l + mlen*4*i+ mlen+1;
			c[index1] = _mm_extract_epi32 (x[10*i+1], 0)^m[index1];
			c[index2] = _mm_extract_epi32 (x[10*i+1], 2)^m[index2];
			x[10*i+1] = x[10*i+1]^_mm_set_epi32(0x0, m[index2], 0x0, m[index1]);
			
			index1 = 2*l + mlen*4*i+ 2*mlen;
			index2 = 2*l + mlen*4*i+ 2*mlen+1;
			c[index1] = _mm_extract_epi32 (x[10*i+2], 0)^m[index1];
			c[index2] = _mm_extract_epi32 (x[10*i+2], 2)^m[index2];
			x[10*i+2] = x[10*i+2]^_mm_set_epi32(0x0, m[index2], 0x0, m[index1]);
			
			index1 = 2*l + mlen*4*i+ 3*mlen;
			index2 = 2*l + mlen*4*i+ 3*mlen+1;
			c[index1] = _mm_extract_epi32 (x[10*i+3], 0)^m[index1];
			c[index2] = _mm_extract_epi32 (x[10*i+3], 2)^m[index2];
			x[10*i+3] = x[10*i+3]^_mm_set_epi32(0x0, m[index2], 0x0, m[index1]);

			//Domain seperator
			x[10*i+8] = _mm_insert_epi32(x[10*i+8], _mm_extract_epi32 (x[10*i+8], 1)^0x00000040, 1 );
			x[10*i+8] = _mm_insert_epi32(x[10*i+8], _mm_extract_epi32 (x[10*i+8], 3)^0x00000040, 3 );
			x[10*i+9] = _mm_insert_epi32(x[10*i+9], _mm_extract_epi32 (x[10*i+9], 1)^0x00000040, 1 );
			x[10*i+9] = _mm_insert_epi32(x[10*i+9], _mm_extract_epi32 (x[10*i+9], 3)^0x00000040, 3 );
		}
		ace(x);
	}

	// Tag Extraction Phase
	for ( i = 0; i < PARAL_INST_BY4; i++)
	{
		x[10*i+0] = x[10*i+0]^_mm_set_epi8(0x0, 0x0, 0x0, 0x0, *(k+7+64*i), *(k+6+64*i), *(k+5+64*i), *(k+4+64*i), 0x0, 0x0, 0x0, 0x0, *(k+3+64*i), *(k+2+64*i), *(k+1+64*i), *(k+64*i));
		x[10*i+1] = x[10*i+1]^_mm_set_epi8(0x0, 0x0, 0x0, 0x0, *(k+23+64*i), *(k+22+64*i), *(k+21+64*i), *(k+20+64*i), 0x0, 0x0, 0x0, 0x0, *(k+19+64*i), *(k+18+64*i), *(k+17+64*i), *(k+16+64*i));
		x[10*i+2] = x[10*i+2]^_mm_set_epi8(0x0, 0x0, 0x0, 0x0, *(k+39+64*i), *(k+38+64*i), *(k+37+64*i), *(k+36+64*i), 0x0, 0x0, 0x0, 0x0, *(k+35+64*i), *(k+34+64*i), *(k+33+64*i), *(k+32+64*i));
		x[10*i+3] = x[10*i+3]^_mm_set_epi8(0x0, 0x0, 0x0, 0x0, *(k+55+64*i), *(k+54+64*i), *(k+53+64*i), *(k+52+64*i), 0x0, 0x0, 0x0, 0x0, *(k+51+64*i), *(k+50+64*i), *(k+49+64*i), *(k+48+64*i));
	}
	ace(x);
	for ( i = 0; i < PARAL_INST_BY4; i++)
	{
		x[10*i+0] = x[10*i+0]^_mm_set_epi8(0x0, 0x0, 0x0, 0x0, *(k+15+64*i), *(k+14+64*i), *(k+13+64*i), *(k+12+64*i), 0x0, 0x0, 0x0, 0x0, *(k+11+64*i), *(k+10+64*i), *(k+9+64*i), *(k+8+64*i));
		x[10*i+1] = x[10*i+1]^_mm_set_epi8(0x0, 0x0, 0x0, 0x0, *(k+31+64*i), *(k+30+64*i), *(k+29+64*i), *(k+28+64*i), 0x0, 0x0, 0x0, 0x0, *(k+27+64*i), *(k+26+64*i), *(k+25+64*i), *(k+24+64*i));
		x[10*i+2] = x[10*i+2]^_mm_set_epi8(0x0, 0x0, 0x0, 0x0, *(k+47+64*i), *(k+46+64*i), *(k+45+64*i), *(k+44+64*i), 0x0, 0x0, 0x0, 0x0, *(k+43+64*i), *(k+42+64*i), *(k+41+64*i), *(k+40+64*i));
		x[10*i+3] = x[10*i+3]^_mm_set_epi8(0x0, 0x0, 0x0, 0x0, *(k+63+64*i), *(k+62+64*i), *(k+61+64*i), *(k+60+64*i), 0x0, 0x0, 0x0, 0x0, *(k+59+64*i), *(k+58+64*i), *(k+57+64*i), *(k+56+64*i));
	}
	ace(x);

	for ( i = 0; i < PARAL_INST_BY4; i++)
	{
		_mm_storeu_si128((void *) (tag+16*i), x[10*i]);
		_mm_storeu_si128((void *) (tag+4+16*i), x[10*i+1]);
		_mm_storeu_si128((void *) (tag+8+16*i), x[10*i+2]);
		_mm_storeu_si128((void *) (tag+12+16*i), x[10*i+3]);
	}
return 0;
}

int crypto_aead_decrypt( u32 *m, u32 *c, u32 mlen, u32 *tag, u32 tlen, u32 *ad, u32 adlen, u8 *k, u8 *npub, u32 klen )
{
        u8 i;
        u64 l, mblock, adblock;
        __m128i x[10*PARAL_INST_BY4];
	u32 index1, index2;

	mblock = mlen/2;
	adblock = adlen/2;
       
	//Initialization: Loading Key & Nonce
        for ( i = 0; i < PARAL_INST_BY4; i++ )
	{
		//Key and Nonce are loaded according to Packing.
		x[10*i+0] = _mm_loadu_si128((void *) (k+0+64*i));
		x[10*i+4] = _mm_loadl_epi64 ((const __m128i *)(npub+0+64*i));
		
		x[10*i+1] = _mm_loadu_si128((void *) (k+16+64*i));
		x[10*i+5] = _mm_loadl_epi64 ((const __m128i*)(npub + 16+64*i));

		x[10*i+8] = _mm_set_epi8( *(npub+31+64*i), *(npub+30+64*i), *(npub+29+64*i), *(npub+28+64*i), *(npub+27+64*i), *(npub+26+64*i), *(npub+25+64*i), *(npub+24+64*i), *(npub+15+64*i), *(npub+14+64*i), *(npub+13+64*i), *(npub+12+64*i), *(npub+11+64*i), *(npub+10+64*i), *(npub+9+64*i), *(npub+8+64*i));


		x[10*i+2] = _mm_loadu_si128((void *) (k+32+64*i));
		x[10*i+6] = _mm_loadl_epi64 ((const __m128i*)(npub+32+64*i));
		
		x[10*i+3] = _mm_loadu_si128((void *) (k+48+64*i));
		x[10*i+7] = _mm_loadl_epi64 ((const __m128i*)(npub+48+64*i));

		x[10*i+9] = _mm_set_epi8( *(npub+63+64*i), *(npub+62+64*i), *(npub+61+64*i), *(npub+60+64*i), *(npub+59+64*i), *(npub+58+64*i), *(npub+57+64*i), *(npub+56+64*i), *(npub+47+64*i), *(npub+46+64*i), *(npub+45+64*i), *(npub+44+64*i), *(npub+43+64*i), *(npub+42+64*i), *(npub+41+64*i), *(npub+40+64*i));

	}
	ace(x);
	for ( i = 0; i < PARAL_INST_BY4; i++)
	{
		x[10*i+0] = x[10*i+0]^_mm_set_epi8(0x0, 0x0, 0x0, 0x0, *(k+7+64*i), *(k+6+64*i), *(k+5+64*i), *(k+4+64*i), 0x0, 0x0, 0x0, 0x0, *(k+3+64*i), *(k+2+64*i), *(k+1+64*i), *(k+64*i));
		x[10*i+1] = x[10*i+1]^_mm_set_epi8(0x0, 0x0, 0x0, 0x0, *(k+23+64*i), *(k+22+64*i), *(k+21+64*i), *(k+20+64*i), 0x0, 0x0, 0x0, 0x0, *(k+19+64*i), *(k+18+64*i), *(k+17+64*i), *(k+16+64*i));
		x[10*i+2] = x[10*i+2]^_mm_set_epi8(0x0, 0x0, 0x0, 0x0, *(k+39+64*i), *(k+38+64*i), *(k+37+64*i), *(k+36+64*i), 0x0, 0x0, 0x0, 0x0, *(k+35+64*i), *(k+34+64*i), *(k+33+64*i), *(k+32+64*i));
		x[10*i+3] = x[10*i+3]^_mm_set_epi8(0x0, 0x0, 0x0, 0x0, *(k+55+64*i), *(k+54+64*i), *(k+53+64*i), *(k+52+64*i), 0x0, 0x0, 0x0, 0x0, *(k+51+64*i), *(k+50+64*i), *(k+49+64*i), *(k+48+64*i));
	}
	ace(x);
	for ( i = 0; i < PARAL_INST_BY4; i++)
	{
		x[10*i+0] = x[10*i+0]^_mm_set_epi8(0x0, 0x0, 0x0, 0x0, *(k+15+64*i), *(k+14+64*i), *(k+13+64*i), *(k+12+64*i), 0x0, 0x0, 0x0, 0x0, *(k+11+64*i), *(k+10+64*i), *(k+9+64*i), *(k+8+64*i));
		x[10*i+1] = x[10*i+1]^_mm_set_epi8(0x0, 0x0, 0x0, 0x0, *(k+31+64*i), *(k+30+64*i), *(k+29+64*i), *(k+28+64*i), 0x0, 0x0, 0x0, 0x0, *(k+27+64*i), *(k+26+64*i), *(k+25+64*i), *(k+24+64*i));
		x[10*i+2] = x[10*i+2]^_mm_set_epi8(0x0, 0x0, 0x0, 0x0, *(k+47+64*i), *(k+46+64*i), *(k+45+64*i), *(k+44+64*i), 0x0, 0x0, 0x0, 0x0, *(k+43+64*i), *(k+42+64*i), *(k+41+64*i), *(k+40+64*i));
		x[10*i+3] = x[10*i+3]^_mm_set_epi8(0x0, 0x0, 0x0, 0x0, *(k+63+64*i), *(k+62+64*i), *(k+61+64*i), *(k+60+64*i), 0x0, 0x0, 0x0, 0x0, *(k+59+64*i), *(k+58+64*i), *(k+57+64*i), *(k+56+64*i));
	}
	ace(x);

	//Proceesing Associated Data
	for ( l = 0; l < adblock; l++ )
	{
		for ( i = 0; i < PARAL_INST_BY4; i++)
		{
			index1 = 2*l + adlen*4*i;
			index2 = 2*l + adlen*4*i+1;
			x[10*i+0] = x[10*i+0]^_mm_set_epi32(0x0, ad[index2], 0x0, ad[index1]);
			
			index1 = 2*l + adlen*4*i+ adlen;
			index2 = 2*l + adlen*4*i+ adlen+1;
			x[10*i+1] = x[10*i+1]^_mm_set_epi32(0x0, ad[index2], 0x0, ad[index1]);
			
			index1 = 2*l + adlen*4*i+ 2*adlen;
			index2 = 2*l + adlen*4*i+ 2*adlen+1;
			x[10*i+2] = x[10*i+2]^_mm_set_epi32(0x0, ad[index2], 0x0, ad[index1]);
			
			index1 = 2*l + adlen*4*i+ 3*adlen;
			index2 = 2*l + adlen*4*i+ 3*adlen+1;
			x[10*i+3] = x[10*i+3]^_mm_set_epi32(0x0, ad[index2], 0x0, ad[index1]);

			//Domain seperator
			x[10*i+8] = _mm_insert_epi32(x[10*i+8], _mm_extract_epi32 (x[10*i+8], 1)^0x00000080, 1 );
			x[10*i+8] = _mm_insert_epi32(x[10*i+8], _mm_extract_epi32 (x[10*i+8], 3)^0x00000080, 3 );
			x[10*i+9] = _mm_insert_epi32(x[10*i+9], _mm_extract_epi32 (x[10*i+9], 1)^0x00000080, 1 );
			x[10*i+9] = _mm_insert_epi32(x[10*i+9], _mm_extract_epi32 (x[10*i+9], 3)^0x00000080, 3 );
		}
		ace(x);
	}
	
	//Decrypt ciphertext
	for ( l = 0; l < mblock; l++ )
	{
		for ( i = 0; i < PARAL_INST_BY4; i++)
		{
			index1 = 2*l + mlen*4*i;
			index2 = 2*l + mlen*4*i+1;
			m[index1] = _mm_extract_epi32 (x[10*i+0], 0)^c[index1];
			m[index2] = _mm_extract_epi32 (x[10*i+0], 2)^c[index2];
			x[10*i+0] = x[10*i+0]^_mm_set_epi32(0x0, m[index2], 0x0, m[index1]);
			
			index1 = 2*l + mlen*4*i+ mlen;
			index2 = 2*l + mlen*4*i+ mlen+1;
			m[index1] = _mm_extract_epi32 (x[10*i+1], 0)^c[index1];
			m[index2] = _mm_extract_epi32 (x[10*i+1], 2)^c[index2];
			x[10*i+1] = x[10*i+1]^_mm_set_epi32(0x0, m[index2], 0x0, m[index1]);
			
			index1 = 2*l + mlen*4*i+ 2*mlen;
			index2 = 2*l + mlen*4*i+ 2*mlen+1;
			m[index1] = _mm_extract_epi32 (x[10*i+2], 0)^c[index1];
			m[index2] = _mm_extract_epi32 (x[10*i+2], 2)^c[index2];
			x[10*i+2] = x[10*i+2]^_mm_set_epi32(0x0, m[index2], 0x0, m[index1]);
			
			index1 = 2*l + mlen*4*i+ 3*mlen;
			index2 = 2*l + mlen*4*i+ 3*mlen+1;
			m[index1] = _mm_extract_epi32 (x[10*i+3], 0)^c[index1];
			m[index2] = _mm_extract_epi32 (x[10*i+3], 2)^c[index2];
			x[10*i+3] = x[10*i+3]^_mm_set_epi32(0x0, m[index2], 0x0, m[index1]);

			//Domain seperator
			x[10*i+8] = _mm_insert_epi32(x[10*i+8], _mm_extract_epi32 (x[10*i+8], 1)^0x00000040, 1 );
			x[10*i+8] = _mm_insert_epi32(x[10*i+8], _mm_extract_epi32 (x[10*i+8], 3)^0x00000040, 3 );
			x[10*i+9] = _mm_insert_epi32(x[10*i+9], _mm_extract_epi32 (x[10*i+9], 1)^0x00000040, 1 );
			x[10*i+9] = _mm_insert_epi32(x[10*i+9], _mm_extract_epi32 (x[10*i+9], 3)^0x00000040, 3 );
		}
		ace(x);
	}

	// Tag Extraction Phase
	for ( i = 0; i < PARAL_INST_BY4; i++)
	{
		x[10*i+0] = x[10*i+0]^_mm_set_epi8(0x0, 0x0, 0x0, 0x0, *(k+7+64*i), *(k+6+64*i), *(k+5+64*i), *(k+4+64*i), 0x0, 0x0, 0x0, 0x0, *(k+3+64*i), *(k+2+64*i), *(k+1+64*i), *(k+64*i));
		x[10*i+1] = x[10*i+1]^_mm_set_epi8(0x0, 0x0, 0x0, 0x0, *(k+23+64*i), *(k+22+64*i), *(k+21+64*i), *(k+20+64*i), 0x0, 0x0, 0x0, 0x0, *(k+19+64*i), *(k+18+64*i), *(k+17+64*i), *(k+16+64*i));
		x[10*i+2] = x[10*i+2]^_mm_set_epi8(0x0, 0x0, 0x0, 0x0, *(k+39+64*i), *(k+38+64*i), *(k+37+64*i), *(k+36+64*i), 0x0, 0x0, 0x0, 0x0, *(k+35+64*i), *(k+34+64*i), *(k+33+64*i), *(k+32+64*i));
		x[10*i+3] = x[10*i+3]^_mm_set_epi8(0x0, 0x0, 0x0, 0x0, *(k+55+64*i), *(k+54+64*i), *(k+53+64*i), *(k+52+64*i), 0x0, 0x0, 0x0, 0x0, *(k+51+64*i), *(k+50+64*i), *(k+49+64*i), *(k+48+64*i));
	}
	ace(x);
	for ( i = 0; i < PARAL_INST_BY4; i++)
        {
		x[10*i+0] = x[10*i+0]^_mm_set_epi8(0x0, 0x0, 0x0, 0x0, *(k+15+64*i), *(k+14+64*i), *(k+13+64*i), *(k+12+64*i), 0x0, 0x0, 0x0, 0x0, *(k+11+64*i), *(k+10+64*i), *(k+9+64*i), *(k+8+64*i));
		x[10*i+1] = x[10*i+1]^_mm_set_epi8(0x0, 0x0, 0x0, 0x0, *(k+31+64*i), *(k+30+64*i), *(k+29+64*i), *(k+28+64*i), 0x0, 0x0, 0x0, 0x0, *(k+27+64*i), *(k+26+64*i), *(k+25+64*i), *(k+24+64*i));
		x[10*i+2] = x[10*i+2]^_mm_set_epi8(0x0, 0x0, 0x0, 0x0, *(k+47+64*i), *(k+46+64*i), *(k+45+64*i), *(k+44+64*i), 0x0, 0x0, 0x0, 0x0, *(k+43+64*i), *(k+42+64*i), *(k+41+64*i), *(k+40+64*i));
		x[10*i+3] = x[10*i+3]^_mm_set_epi8(0x0, 0x0, 0x0, 0x0, *(k+63+64*i), *(k+62+64*i), *(k+61+64*i), *(k+60+64*i), 0x0, 0x0, 0x0, 0x0, *(k+59+64*i), *(k+58+64*i), *(k+57+64*i), *(k+56+64*i));
	}
	ace(x);

	for ( i = 0; i < PARAL_INST_BY4; i++)
	{
		_mm_storeu_si128((void *) (tag+16*i), x[10*i]);
		_mm_storeu_si128((void *) (tag+4+16*i), x[10*i+1]);
		_mm_storeu_si128((void *) (tag+8+16*i), x[10*i+2]);
		_mm_storeu_si128((void *) (tag+12+16*i), x[10*i+3]);
	}
return 0;
}
