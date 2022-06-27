#include "sycon.h"

static const unsigned char RC[14]={0x15,0x1a,0x1d,0x0e,0x17,0x1b,0x0d,0x06,0x03,0x11,0x18,0x1c,0x1e,0x1f};

void sycon ( u32 *state )
{
        u8 i,j;
        __m128i r[5*NUMINSTANCE];
        __m128i t0, t1, t2, t3, t4;
        
        for ( j = 0; j < NUMINSTANCE; j++ )
        {
                r[5*j+0] = _mm_set_epi32(state[20*j+11], state[20*j+10], state[20*j+1], state[20*j+0]);
                r[5*j+1] = _mm_set_epi32(state[20*j+13], state[20*j+12], state[20*j+3], state[20*j+2]);
                r[5*j+2] = _mm_set_epi32(state[20*j+15], state[20*j+14], state[20*j+5], state[20*j+4]);
                r[5*j+3] = _mm_set_epi32(state[20*j+17], state[20*j+16], state[20*j+7], state[20*j+6]);
                r[5*j+4] = _mm_set_epi32(state[20*j+19], state[20*j+18], state[20*j+9], state[20*j+8]);
        }
        
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
                        //pLayer (FIST Perm)
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
        
        for ( j = 0; j < NUMINSTANCE; j++ )
        {
                t0 = _mm_unpacklo_epi64 (r[5*j+0], r[5*j+1]);
                _mm_storeu_si128((void *) (state + 0+20*j), t0);
        
                t0 = _mm_unpacklo_epi64 (r[5*j+2], r[5*j+3]);
                _mm_storeu_si128((void *) (state + 4+20*j), t0);
        
                _mm_storel_epi64 ((__m128i*)(state + 8+20*j), r[5*j+4]);
                //_mm_storeu_si128((void *) (state + 8), r5);
        
                t1 = _mm_unpackhi_epi64 (r[5*j+0], r[5*j+1]);
                _mm_storeu_si128((void *) (state + 10+20*j), t1);
                t1 = _mm_unpackhi_epi64 (r[5*j+2], r[5*j+3]);
                _mm_storeu_si128((void *) (state + 14+20*j), t1);
        
                //_mm_storel_epi64 ((__m128i*)(state + 8), r4);
                t1 = _mm_unpackhi_epi64 (r[5*j+4], _mm_setzero_si128());
                _mm_storel_epi64((void *) (state + 18+20*j), t1);
        }
        return;
}


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
                        
                        //pLayer (FIST Perm)
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

void sycon_parallel_redu( __m128i *r )
{
        u8 i, j;
        __m128i t0, t1, t2, t3, t4;
        
        for ( i = 0; i < NUMROUNDSREDU; i++ )
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


int crypto_aead_encrypt( u32 *tag, u32 tlen, u32 *c, u32 *m, u32 mlen, u32 *ad, u32 adlen, u8 *k, u8 *npub, u32 klen )
{
        u8 i;
        u64 l, mblock, adblock;
        __m128i r[5*NUMINSTANCE];
        u32 index1, index2, index3, index4;
        
        mblock = mlen/2;
        adblock = adlen/2;
        
        //Initialization: Loading Key and Nonce
        for ( i = 0; i < NUMINSTANCE; i++ )
        {
                r[5*i+0] = _mm_set_epi8(k[32*i+23], k[32*i+22], k[32*i+21], k[32*i+20], k[32*i+19], k[32*i+18], k[32*i+17], k[32*i+16], k[32*i+7], k[32*i+6], k[32*i+5], k[32*i+4], k[32*i+3], k[32*i+2], k[32*i+1], k[32*i]);
                r[5*i+1] = _mm_set_epi8(k[32*i+31], k[32*i+30], k[32*i+29], k[32*i+28], k[32*i+27], k[32*i+26], k[32*i+25], k[32*i+24], k[32*i+15], k[32*i+14], k[32*i+13], k[32*i+12], k[32*i+11], k[32*i+10], k[32*i+9], k[32*i+8]);
                
                r[5*i+2] = _mm_set_epi8(npub[32*i+23], npub[32*i+22], npub[32*i+21], npub[32*i+20], npub[32*i+19], npub[32*i+18], npub[32*i+17], npub[32*i+16], npub[32*i+7], npub[32*i+6], npub[32*i+5], npub[32*i+4], npub[32*i+3], npub[32*i+2], npub[32*i+1], npub[32*i]);
                r[5*i+3] = _mm_set_epi8(npub[32*i+31], npub[32*i+30], npub[32*i+29], npub[32*i+28], npub[32*i+27], npub[32*i+26], npub[32*i+25], npub[32*i+24], npub[32*i+15], npub[32*i+14], npub[32*i+13], npub[32*i+12], npub[32*i+11], npub[32*i+10], npub[32*i+9], npub[32*i+8]);
                
                r[5*i+4] = _mm_set_epi64x(0x0, 0x0);
        }
        sycon_parallel(r);
        for ( i = 0; i < NUMINSTANCE; i++)
        {
                r[5*i+0]^=_mm_set_epi8(k[32*i+23], k[32*i+22], k[32*i+21], k[32*i+20], k[32*i+19], k[32*i+18], k[32*i+17], k[32*i+16], k[32*i+7], k[32*i+6], k[32*i+5], k[32*i+4], k[32*i+3], k[32*i+2], k[32*i+1], k[32*i]);
        }
        sycon_parallel(r);
        for ( i = 0; i < NUMINSTANCE; i++)
        {
                r[5*i+0]^=_mm_set_epi8(k[32*i+31], k[32*i+30], k[32*i+29], k[32*i+28], k[32*i+27], k[32*i+26], k[32*i+25], k[32*i+24], k[32*i+15], k[32*i+14], k[32*i+13], k[32*i+12], k[32*i+11], k[32*i+10], k[32*i+9], k[32*i+8]);
                //Domain separator
                r[5*i+4]^=_mm_set_epi32(0x80000000, 0x00000000, 0x80000000, 0x00000000);
        }
       
        sycon_parallel(r);
        
        //Proceesing Associated Data
        for ( l = 0; l < adblock-1; l++ )
        {
                for ( i = 0; i < NUMINSTANCE; i++)
                {
                        index1 = 2*l + adlen*2*i;
                        index2 = 2*l + adlen*2*i+1;
                        index3 = 2*l + adlen*2*i+ adlen;
                        index4 = 2*l + adlen*2*i+ adlen+1;
                        r[5*i+0]^=_mm_set_epi32(ad[index4], ad[index3], ad[index2], ad[index1]);
                        
                        //Domain separator
                        r[5*i+4]^=_mm_set_epi32(0x80000000, 0x00000000, 0x80000000, 0x00000000);
                }
                sycon_parallel_redu(r);
        }
        
        //Processing last block
        l = adblock-1;
        for ( i = 0; i < NUMINSTANCE; i++)
        {
                index1 = 2*l + adlen*2*i;
                index2 = 2*l + adlen*2*i+1;
                index3 = 2*l + adlen*2*i+ adlen;
                index4 = 2*l + adlen*2*i+ adlen+1;
                r[5*i+0]^=_mm_set_epi32(ad[index4], ad[index3], ad[index2], ad[index1]);
                
                //Domain seperator
                r[5*i+4]^=_mm_set_epi32(0x40000000, 0x00000000, 0x40000000, 0x00000000);
        }
        sycon_parallel_redu(r);

        //Encrypt plaintext
        for ( l = 0; l < mblock-1; l++ )
        {
                for ( i = 0; i < NUMINSTANCE; i++)
                {
                        index1 = 2*l + mlen*2*i;
                        index2 = 2*l + mlen*2*i+1;
                        c[index1] = _mm_extract_epi32 (r[5*i+0], 0)^m[index1];
                        c[index2] = _mm_extract_epi32 (r[5*i+0], 1)^m[index2];
                        r[5*i+0]^=_mm_set_epi32(0x0, 0x0, m[index2], m[index1]);
                        
                        index1 = 2*l + mlen*2*i+ mlen;
                        index2 = 2*l + mlen*2*i+ mlen+1;
                        c[index1] = _mm_extract_epi32 (r[5*i+0], 2)^m[index1];
                        c[index2] = _mm_extract_epi32 (r[5*i+0], 3)^m[index2];
                        r[5*i+0]^=_mm_set_epi32(m[index2], m[index1], 0x0, 0x0);
                        
                        //Domain seperator
                        r[5*i+4]^=_mm_set_epi32(0x40000000, 0x00000000, 0x40000000, 0x00000000);
                }
                sycon_parallel_redu(r);
        }
        //Processing last block
        l = mblock-1;
        for ( i = 0; i < NUMINSTANCE; i++)
        {
                index1 = 2*l + mlen*2*i;
                index2 = 2*l + mlen*2*i+1;
                c[index1] = _mm_extract_epi32 (r[5*i+0], 0)^m[index1];
                c[index2] = _mm_extract_epi32 (r[5*i+0], 1)^m[index2];
                r[5*i+0]^=_mm_set_epi32(0x0, 0x0, m[index2], m[index1]);
                
                index1 = 2*l + mlen*2*i+ mlen;
                index2 = 2*l + mlen*2*i+ mlen+1;
                c[index1] = _mm_extract_epi32 (r[5*i+0], 2)^m[index1];
                c[index2] = _mm_extract_epi32 (r[5*i+0], 3)^m[index2];
                r[5*i+0]^=_mm_set_epi32(m[index2], m[index1], 0x0, 0x0);
                
                //Domain separator
                r[5*i+4]^=_mm_set_epi32(0x20000000, 0x00000000, 0x20000000, 0x00000000);
        }
        sycon_parallel_redu(r);
        
        // Tag Extraction Phase
        for ( i = 0; i < NUMINSTANCE; i++)
        {
                r[5*i+0]^=_mm_set_epi8(k[32*i+23], k[32*i+22], k[32*i+21], k[32*i+20], k[32*i+19], k[32*i+18], k[32*i+17], k[32*i+16], k[32*i+7], k[32*i+6], k[32*i+5], k[32*i+4], k[32*i+3], k[32*i+2], k[32*i+1], k[32*i]);
                //Domain separator
                r[5*i+4]^=_mm_set_epi32(0x20000000, 0x00000000, 0x20000000, 0x00000000);
        }
        sycon_parallel(r);
        for ( i = 0; i < NUMINSTANCE; i++)
        {
                r[5*i+0]^=_mm_set_epi8(k[32*i+31], k[32*i+30], k[32*i+29], k[32*i+28], k[32*i+27], k[32*i+26], k[32*i+25], k[32*i+24], k[32*i+15], k[32*i+14], k[32*i+13], k[32*i+12], k[32*i+11], k[32*i+10], k[32*i+9], k[32*i+8]);
                //Domain separator
                r[5*i+4]^=_mm_set_epi32(0x20000000, 0x00000000, 0x20000000, 0x00000000);
        }
        
        sycon_parallel(r);
        for ( i = 0; i < NUMINSTANCE; i++)
        {
                _mm_storeu_si128((void *) (tag+8*i), _mm_unpacklo_epi64(r[5*i+2],r[5*i+3]) );
                _mm_storeu_si128((void *) (tag+4+8*i), _mm_unpackhi_epi64(r[5*i+2],r[5*i+3]) );
        }
        return 0;
}

int crypto_aead_decrypt( u32 *m, u32 *c, u32 mlen, u32 *tag, u32 tlen, u32 *ad, u32 adlen, u8 *k, u8 *npub, u32 klen )
{
        u8 i;
        u64 l, mblock, adblock;
        __m128i r[5*NUMINSTANCE];
        u32 index1, index2, index3, index4;
        
        mblock = mlen/2;
        adblock = adlen/2;
        
        //Initialization: Loading Key and Nonce
        for ( i = 0; i < NUMINSTANCE; i++ )
        {
                r[5*i+0] = _mm_set_epi8(k[32*i+23], k[32*i+22], k[32*i+21], k[32*i+20], k[32*i+19], k[32*i+18], k[32*i+17], k[32*i+16], k[32*i+7], k[32*i+6], k[32*i+5], k[32*i+4], k[32*i+3], k[32*i+2], k[32*i+1], k[32*i]);
                r[5*i+1] = _mm_set_epi8(k[32*i+31], k[32*i+30], k[32*i+29], k[32*i+28], k[32*i+27], k[32*i+26], k[32*i+25], k[32*i+24], k[32*i+15], k[32*i+14], k[32*i+13], k[32*i+12], k[32*i+11], k[32*i+10], k[32*i+9], k[32*i+8]);
                
                r[5*i+2] = _mm_set_epi8(npub[32*i+23], npub[32*i+22], npub[32*i+21], npub[32*i+20], npub[32*i+19], npub[32*i+18], npub[32*i+17], npub[32*i+16], npub[32*i+7], npub[32*i+6], npub[32*i+5], npub[32*i+4], npub[32*i+3], npub[32*i+2], npub[32*i+1], npub[32*i]);
                r[5*i+3] = _mm_set_epi8(npub[32*i+31], npub[32*i+30], npub[32*i+29], npub[32*i+28], npub[32*i+27], npub[32*i+26], npub[32*i+25], npub[32*i+24], npub[32*i+15], npub[32*i+14], npub[32*i+13], npub[32*i+12], npub[32*i+11], npub[32*i+10], npub[32*i+9], npub[32*i+8]);
                
                r[5*i+4] = _mm_set_epi64x(0x0, 0x0);
        }
        sycon_parallel(r);
        for ( i = 0; i < NUMINSTANCE; i++)
        {
                r[5*i+0]^=_mm_set_epi8(k[32*i+23], k[32*i+22], k[32*i+21], k[32*i+20], k[32*i+19], k[32*i+18], k[32*i+17], k[32*i+16], k[32*i+7], k[32*i+6], k[32*i+5], k[32*i+4], k[32*i+3], k[32*i+2], k[32*i+1], k[32*i]);
        }
        sycon_parallel(r);
        for ( i = 0; i < NUMINSTANCE; i++)
        {
                r[5*i+0]^=_mm_set_epi8(k[32*i+31], k[32*i+30], k[32*i+29], k[32*i+28], k[32*i+27], k[32*i+26], k[32*i+25], k[32*i+24], k[32*i+15], k[32*i+14], k[32*i+13], k[32*i+12], k[32*i+11], k[32*i+10], k[32*i+9], k[32*i+8]);
                //Domain separator
                r[5*i+4]^=_mm_set_epi32(0x80000000, 0x00000000, 0x80000000, 0x00000000);
        }
        
        sycon_parallel(r);
        
        //Proceesing Associated Data
        for ( l = 0; l < adblock-1; l++ )
        {
                for ( i = 0; i < NUMINSTANCE; i++)
                {
                        index1 = 2*l + adlen*2*i;
                        index2 = 2*l + adlen*2*i+1;
                        index3 = 2*l + adlen*2*i+ adlen;
                        index4 = 2*l + adlen*2*i+ adlen+1;
                        r[5*i+0]^=_mm_set_epi32(ad[index4], ad[index3], ad[index2], ad[index1]);
                        
                        //Domain separator
                        r[5*i+4]^=_mm_set_epi32(0x80000000, 0x00000000, 0x80000000, 0x00000000);
                }
                sycon_parallel_redu(r);
        }
        
        //Processing last block
        l = adblock-1;
        for ( i = 0; i < NUMINSTANCE; i++)
        {
                index1 = 2*l + adlen*2*i;
                index2 = 2*l + adlen*2*i+1;
                index3 = 2*l + adlen*2*i+ adlen;
                index4 = 2*l + adlen*2*i+ adlen+1;
                r[5*i+0]^=_mm_set_epi32(ad[index4], ad[index3], ad[index2], ad[index1]);
                
                //Domain separator
                r[5*i+4]^=_mm_set_epi32(0x40000000, 0x00000000, 0x40000000, 0x00000000);
        }
        sycon_parallel_redu(r);
        
        //Encrypt plaintext
        for ( l = 0; l < mblock-1; l++ )
        {
                for ( i = 0; i < NUMINSTANCE; i++)
                {
                        index1 = 2*l + mlen*2*i;
                        index2 = 2*l + mlen*2*i+1;
                        m[index1] = _mm_extract_epi32 (r[5*i+0], 0)^c[index1];
                        m[index2] = _mm_extract_epi32 (r[5*i+0], 1)^c[index2];
                        r[5*i+0]^=_mm_set_epi32(0x0, 0x0, m[index2], m[index1]);
                        
                        index1 = 2*l + mlen*2*i+ mlen;
                        index2 = 2*l + mlen*2*i+ mlen+1;
                        m[index1] = _mm_extract_epi32 (r[5*i+0], 2)^c[index1];
                        m[index2] = _mm_extract_epi32 (r[5*i+0], 3)^c[index2];
                        r[5*i+0]^=_mm_set_epi32(m[index2], m[index1], 0x0, 0x0);
                        
                        //Domain separator
                        r[5*i+4]^=_mm_set_epi32(0x40000000, 0x00000000, 0x40000000, 0x00000000);
                }
                sycon_parallel_redu(r);
        }
        //Processing last block
        l = mblock-1;
        for ( i = 0; i < NUMINSTANCE; i++)
        {
                index1 = 2*l + mlen*2*i;
                index2 = 2*l + mlen*2*i+1;
                m[index1] = _mm_extract_epi32 (r[5*i+0], 0)^c[index1];
                m[index2] = _mm_extract_epi32 (r[5*i+0], 1)^c[index2];
                r[5*i+0]^=_mm_set_epi32(0x0, 0x0, m[index2], m[index1]);
                
                index1 = 2*l + mlen*2*i+ mlen;
                index2 = 2*l + mlen*2*i+ mlen+1;
                m[index1] = _mm_extract_epi32 (r[5*i+0], 2)^c[index1];
                m[index2] = _mm_extract_epi32 (r[5*i+0], 3)^c[index2];
                r[5*i+0]^=_mm_set_epi32(m[index2], m[index1], 0x0, 0x0);
                
                //Domain separator
                r[5*i+4]^=_mm_set_epi32(0x20000000, 0x00000000, 0x20000000, 0x00000000);
        }
        sycon_parallel_redu(r);
        //sycon_parallel(r);
        
        // Tag Extraction Phase
        for ( i = 0; i < NUMINSTANCE; i++)
        {
                r[5*i+0]^=_mm_set_epi8(k[32*i+23], k[32*i+22], k[32*i+21], k[32*i+20], k[32*i+19], k[32*i+18], k[32*i+17], k[32*i+16], k[32*i+7], k[32*i+6], k[32*i+5], k[32*i+4], k[32*i+3], k[32*i+2], k[32*i+1], k[32*i]);
                //Domain separator
                r[5*i+4]^=_mm_set_epi32(0x20000000, 0x00000000, 0x20000000, 0x00000000);
        }
        
        sycon_parallel(r);
        for ( i = 0; i < NUMINSTANCE; i++)
        {
                r[5*i+0]^=_mm_set_epi8(k[32*i+31], k[32*i+30], k[32*i+29], k[32*i+28], k[32*i+27], k[32*i+26], k[32*i+25], k[32*i+24], k[32*i+15], k[32*i+14], k[32*i+13], k[32*i+12], k[32*i+11], k[32*i+10], k[32*i+9], k[32*i+8]);
                //Domain separator
                r[5*i+4]^=_mm_set_epi32(0x20000000, 0x00000000, 0x20000000, 0x00000000);
        }
        
        sycon_parallel(r);
        for ( i = 0; i < NUMINSTANCE; i++)
        {
                _mm_storeu_si128((void *) (tag+8*i), _mm_unpacklo_epi64(r[5*i+2],r[5*i+3]) );
                _mm_storeu_si128((void *) (tag+4+8*i), _mm_unpackhi_epi64(r[5*i+2],r[5*i+3]) );
        }
        return 0;
}
