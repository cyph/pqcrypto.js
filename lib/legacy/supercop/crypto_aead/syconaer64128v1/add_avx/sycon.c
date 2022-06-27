#include "sycon.h"

static const unsigned char RC[14]={0x15,0x1a,0x1d,0x0e,0x17,0x1b,0x0d,0x06,0x03,0x11,0x18,0x1c,0x1e,0x1f};

void sycon( u32 *state )
{
        u8 i, j;
        __m256i r[5*NUMINSTANCE];
        __m256i tmp, t0, t1, t2, t3, t4;
        
        for ( j = 0; j < NUMINSTANCE; j++ )
        {
                r[5*j+0] = _mm256_set_epi32(state[40*j+31], state[40*j+30], state[40*j+21], state[40*j+20], state[40*j+11], state[40*j+10], state[40*j+1], state[40*j+0]);
                r[5*j+1] = _mm256_set_epi32(state[40*j+33], state[40*j+32], state[40*j+23], state[40*j+22], state[40*j+13], state[40*j+12], state[40*j+3], state[40*j+2]);
                r[5*j+2] = _mm256_set_epi32(state[40*j+35], state[40*j+34], state[40*j+25], state[40*j+24], state[40*j+15], state[40*j+14], state[40*j+5], state[40*j+4]);
                r[5*j+3] = _mm256_set_epi32(state[40*j+37], state[40*j+36], state[40*j+27], state[40*j+26], state[40*j+17], state[40*j+16], state[40*j+7], state[40*j+6]);
                r[5*j+4] = _mm256_set_epi32(state[40*j+39], state[40*j+38], state[40*j+29], state[40*j+28], state[40*j+19], state[40*j+18], state[40*j+9], state[40*j+8]);
        }
        
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
                
                        //PLayer (FIST Perm)
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
                t0 = _mm256_unpacklo_epi64(r[5*j+0],r[5*j+1]);
                t1 = _mm256_unpacklo_epi64(r[5*j+2],r[5*j+3]);
                tmp = _mm256_permute2x128_si256(t0, t1, 0x20);
                _mm256_storeu_si256((void *)(state+0+40*j), tmp);
                tmp = _mm256_permute2x128_si256(t0, t1, 0x31);
                _mm256_storeu_si256((void *)(state+20+40*j), tmp);
        
                _mm256_storeu_si256((void *)(state+8+40*j), r[5*j+4]);
        
                t0 = _mm256_unpackhi_epi64(r[5*j+0],r[5*j+1]);
                t1 = _mm256_unpackhi_epi64(r[5*j+2],r[5*j+3]);
                tmp = _mm256_permute2x128_si256(t0, t1, 0x20);
                _mm256_storeu_si256((__m256i *)(state+10+40*j), tmp);
                tmp = _mm256_permute2x128_si256(t0, t1, 0x31);
                _mm256_storeu_si256((__m256i *)(state+30+40*j), tmp);
                
                state[18+40*j] = _mm256_extract_epi32(r[5*j+4],2);
                state[19+40*j] = _mm256_extract_epi32(r[5*j+4],3);
                state[28+40*j] = _mm256_extract_epi32(r[5*j+4],4);
                state[29+40*j] = _mm256_extract_epi32(r[5*j+4],5);
                state[38+40*j] = _mm256_extract_epi32(r[5*j+4],6);
                state[39+40*j] = _mm256_extract_epi32(r[5*j+4],7);
        }
        return;
}

void sycon_parallel_redu( __m256i *r )
{
        u8 i, j;
        __m256i t0, t1, t2, t3, t4;
        
        for ( i = 0; i < NUMROUNDSREDU; i++ )
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
                        
                        //PLayer (FIST Perm)
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
        __m256i r[5*NUMINSTANCE];
        u32 index1, index2, index3, index4, index5, index6, index7, index8;
        
        mblock = mlen/2;
        adblock = adlen/2;
        
        for ( i = 0; i < NUMINSTANCE; i++ )
        {
                r[5*i+0] = _mm256_set_epi8(k[64*i+55], k[64*i+54], k[64*i+53], k[64*i+52], k[64*i+51], k[64*i+50], k[64*i+49], k[64*i+48], k[64*i+39], k[64*i+38], k[64*i+37], k[64*i+36], k[64*i+35], k[64*i+34], k[64*i+33], k[64*i+32], k[64*i+23], k[64*i+22], k[64*i+21], k[64*i+20], k[64*i+19], k[64*i+18], k[64*i+17], k[64*i+16], k[64*i+7], k[64*i+6], k[64*i+5], k[64*i+4], k[64*i+3], k[64*i+2], k[64*i+1], k[64*i+0]);
                r[5*i+1] = _mm256_set_epi8(k[64*i+63], k[64*i+62], k[64*i+61], k[64*i+60], k[64*i+59], k[64*i+58], k[64*i+57], k[64*i+56], k[64*i+47], k[64*i+46], k[64*i+45], k[64*i+44], k[64*i+43], k[64*i+42], k[64*i+41], k[64*i+40], k[64*i+31], k[64*i+30], k[64*i+29], k[64*i+28], k[64*i+27], k[64*i+26], k[64*i+25], k[64*i+24], k[64*i+15], k[64*i+14], k[64*i+13], k[64*i+12], k[64*i+11], k[64*i+10], k[64*i+9], k[64*i+8]);
                
                r[5*i+2] = _mm256_set_epi8(npub[64*i+55], npub[64*i+54], npub[64*i+53], npub[64*i+52], npub[64*i+51], npub[64*i+50], npub[64*i+49], npub[64*i+48], npub[64*i+39], npub[64*i+38], npub[64*i+37], npub[64*i+36], npub[64*i+35], npub[64*i+34], npub[64*i+33], npub[64*i+32], npub[64*i+23], npub[64*i+22], npub[64*i+21], npub[64*i+20], npub[64*i+19], npub[64*i+18], npub[64*i+17], npub[64*i+16], npub[64*i+7], npub[64*i+6], npub[64*i+5], npub[64*i+4], npub[64*i+3], npub[64*i+2], npub[64*i+1], npub[64*i+0]);
                r[5*i+3] = _mm256_set_epi8(npub[64*i+63], npub[64*i+62], npub[64*i+61], npub[64*i+60], npub[64*i+59], npub[64*i+58], npub[64*i+57], npub[64*i+56], npub[64*i+47], npub[64*i+46], npub[64*i+45], npub[64*i+44], npub[64*i+43], npub[64*i+42], npub[64*i+41], npub[64*i+40], npub[64*i+31], npub[64*i+30], npub[64*i+29], npub[64*i+28], npub[64*i+27], npub[64*i+26], npub[64*i+25], npub[64*i+24], npub[64*i+15], npub[64*i+14], npub[64*i+13], npub[64*i+12], npub[64*i+11], npub[64*i+10], npub[64*i+9], npub[64*i+8]);
                
                r[5*i+4] = _mm256_set_epi64x(0x0, 0x0, 0x0, 0x0);
        }
        sycon_parallel(r);
        for ( i = 0; i < NUMINSTANCE; i++)
        {
                r[5*i]^= _mm256_set_epi8(k[64*i+55], k[64*i+54], k[64*i+53], k[64*i+52], k[64*i+51], k[64*i+50], k[64*i+49], k[64*i+48], k[64*i+39], k[64*i+38], k[64*i+37], k[64*i+36], k[64*i+35], k[64*i+34], k[64*i+33], k[64*i+32], k[64*i+23], k[64*i+22], k[64*i+21], k[64*i+20], k[64*i+19], k[64*i+18], k[64*i+17], k[64*i+16], k[64*i+7], k[64*i+6], k[64*i+5], k[64*i+4], k[64*i+3], k[64*i+2], k[64*i+1], k[64*i+0]);
        }
        sycon_parallel(r);
        for ( i = 0; i < NUMINSTANCE; i++)
        {
                r[5*i]^= _mm256_set_epi8(k[64*i+63], k[64*i+62], k[64*i+61], k[64*i+60], k[64*i+59], k[64*i+58], k[64*i+57], k[64*i+56], k[64*i+47], k[64*i+46], k[64*i+45], k[64*i+44], k[64*i+43], k[64*i+42], k[64*i+41], k[64*i+40], k[64*i+31], k[64*i+30], k[64*i+29], k[64*i+28], k[64*i+27], k[64*i+26], k[64*i+25], k[64*i+24], k[64*i+15], k[64*i+14], k[64*i+13], k[64*i+12], k[64*i+11], k[64*i+10], k[64*i+9], k[64*i+8]);
                //Domain separator
                r[5*i+4]^=_mm256_set_epi32(0x80000000, 0x00000000, 0x80000000, 0x00000000, 0x80000000, 0x00000000, 0x80000000, 0x00000000);
        }
        sycon_parallel(r);
        
        
        
        //Proceesing Associated Data
        for ( l = 0; l < adblock-1; l++ )
        {
                for ( i = 0; i < NUMINSTANCE; i++)
                {
                        index1 = 2*l + adlen*4*i;
                        index2 = 2*l + adlen*4*i+1;
                        index3 = 2*l + adlen*4*i + adlen;
                        index4 = 2*l + adlen*4*i + adlen + 1;
                        index5 = 2*l + adlen*4*i+2*adlen;
                        index6 = 2*l + adlen*4*i+1+2*adlen;
                        index7 = 2*l + adlen*4*i + 3*adlen;
                        index8 = 2*l + adlen*4*i + 3*adlen + 1;
                        r[5*i+0]^=_mm256_set_epi32(ad[index8], ad[index7], ad[index6], ad[index5], ad[index4], ad[index3], ad[index2], ad[index1]);
                        
                        //Domain separator
                        r[5*i+4]^=_mm256_set_epi32(0x80000000, 0x00000000, 0x80000000, 0x00000000, 0x80000000, 0x00000000, 0x80000000, 0x00000000);
                }
                sycon_parallel_redu(r);
        }
        
        //Processing last block
        l = adblock-1;
        for ( i = 0; i < NUMINSTANCE; i++)
        {
                index1 = 2*l + adlen*4*i;
                index2 = 2*l + adlen*4*i+1;
                index3 = 2*l + adlen*4*i + adlen;
                index4 = 2*l + adlen*4*i + adlen + 1;
                index5 = 2*l + adlen*4*i+2*adlen;
                index6 = 2*l + adlen*4*i+1+2*adlen;
                index7 = 2*l + adlen*4*i + 3*adlen;
                index8 = 2*l + adlen*4*i + 3*adlen + 1;
                r[5*i+0]^=_mm256_set_epi32(ad[index8], ad[index7], ad[index6], ad[index5], ad[index4], ad[index3], ad[index2], ad[index1]);
                
                //Domain separator
                r[5*i+4]^=_mm256_set_epi32(0x40000000, 0x00000000, 0x40000000, 0x00000000, 0x40000000, 0x00000000, 0x40000000, 0x00000000);
        }
        sycon_parallel_redu(r);
        
        //Encrypt plaintext
        for ( l = 0; l < mblock-1; l++ )
        {
                for ( i = 0; i < NUMINSTANCE; i++)
                {
                        index1 = 2*l + mlen*4*i;
                        index2 = 2*l + mlen*4*i+1;
                        index3 = 2*l + mlen*4*i + mlen;
                        index4 = 2*l + mlen*4*i + mlen + 1;
                        index5 = 2*l + mlen*4*i+2*mlen;
                        index6 = 2*l + mlen*4*i+1+2*mlen;
                        index7 = 2*l + mlen*4*i + 3*mlen;
                        index8 = 2*l + mlen*4*i + 3*mlen + 1;
                        c[index1] = _mm256_extract_epi32 (r[5*i], 0)^m[index1];
                        c[index2] = _mm256_extract_epi32 (r[5*i], 1)^m[index2];
                        c[index3] = _mm256_extract_epi32 (r[5*i], 2)^m[index3];
                        c[index4] = _mm256_extract_epi32 (r[5*i], 3)^m[index4];
                        c[index5] = _mm256_extract_epi32 (r[5*i], 4)^m[index5];
                        c[index6] = _mm256_extract_epi32 (r[5*i], 5)^m[index6];
                        c[index7] = _mm256_extract_epi32 (r[5*i], 6)^m[index7];
                        c[index8] = _mm256_extract_epi32 (r[5*i], 7)^m[index8];
                        
                        r[5*i+0]^=_mm256_set_epi32(m[index8], m[index7], m[index6], m[index5], m[index4], m[index3], m[index2], m[index1]);
                        
                        //Domain seperator
                        r[5*i+4]^=_mm256_set_epi32(0x40000000, 0x00000000, 0x40000000, 0x00000000, 0x40000000, 0x00000000, 0x40000000, 0x00000000);
                }
                sycon_parallel_redu(r);
        }
        //Processing last block
        l = mblock-1;
        for ( i = 0; i < NUMINSTANCE; i++)
        {
                index1 = 2*l + mlen*4*i;
                index2 = 2*l + mlen*4*i+1;
                index3 = 2*l + mlen*4*i + mlen;
                index4 = 2*l + mlen*4*i + mlen + 1;
                index5 = 2*l + mlen*4*i+2*mlen;
                index6 = 2*l + mlen*4*i+1+2*mlen;
                index7 = 2*l + mlen*4*i + 3*mlen;
                index8 = 2*l + mlen*4*i + 3*mlen + 1;
                c[index1] = _mm256_extract_epi32 (r[5*i], 0)^m[index1];
                c[index2] = _mm256_extract_epi32 (r[5*i], 1)^m[index2];
                c[index3] = _mm256_extract_epi32 (r[5*i], 2)^m[index3];
                c[index4] = _mm256_extract_epi32 (r[5*i], 3)^m[index4];
                c[index5] = _mm256_extract_epi32 (r[5*i], 4)^m[index5];
                c[index6] = _mm256_extract_epi32 (r[5*i], 5)^m[index6];
                c[index7] = _mm256_extract_epi32 (r[5*i], 6)^m[index7];
                c[index8] = _mm256_extract_epi32 (r[5*i], 7)^m[index8];
                r[5*i+0]^=_mm256_set_epi32(m[index8], m[index7], m[index6], m[index5], m[index4], m[index3], m[index2], m[index1]);
                
                //Domain separator
                r[5*i+4]^=_mm256_set_epi32(0x20000000, 0x00000000, 0x20000000, 0x00000000, 0x20000000, 0x00000000, 0x20000000, 0x00000000);
        }
        sycon_parallel_redu(r);
        
        // Tag Extraction Phase
        for ( i = 0; i < NUMINSTANCE; i++)
        {
                r[5*i]^= _mm256_set_epi8(k[64*i+55], k[64*i+54], k[64*i+53], k[64*i+52], k[64*i+51], k[64*i+50], k[64*i+49], k[64*i+48], k[64*i+39], k[64*i+38], k[64*i+37], k[64*i+36], k[64*i+35], k[64*i+34], k[64*i+33], k[64*i+32], k[64*i+23], k[64*i+22], k[64*i+21], k[64*i+20], k[64*i+19], k[64*i+18], k[64*i+17], k[64*i+16], k[64*i+7], k[64*i+6], k[64*i+5], k[64*i+4], k[64*i+3], k[64*i+2], k[64*i+1], k[64*i+0]);
                //Domain separator
                r[5*i+4]^=_mm256_set_epi32(0x20000000, 0x00000000, 0x20000000, 0x00000000, 0x20000000, 0x00000000, 0x20000000, 0x00000000);
        }
        sycon_parallel(r);
        for ( i = 0; i < NUMINSTANCE; i++)
        {
                r[5*i]^= _mm256_set_epi8(k[64*i+63], k[64*i+62], k[64*i+61], k[64*i+60], k[64*i+59], k[64*i+58], k[64*i+57], k[64*i+56], k[64*i+47], k[64*i+46], k[64*i+45], k[64*i+44], k[64*i+43], k[64*i+42], k[64*i+41], k[64*i+40], k[64*i+31], k[64*i+30], k[64*i+29], k[64*i+28], k[64*i+27], k[64*i+26], k[64*i+25], k[64*i+24], k[64*i+15], k[64*i+14], k[64*i+13], k[64*i+12], k[64*i+11], k[64*i+10], k[64*i+9], k[64*i+8]);
                //Domain separator
                r[5*i+4]^=_mm256_set_epi32(0x20000000, 0x00000000, 0x20000000, 0x00000000, 0x20000000, 0x00000000, 0x20000000, 0x00000000);
        }
        sycon_parallel(r);
        
        for ( i = 0; i < NUMINSTANCE; i++)
        {
                _mm_storeu_si128((void *) (tag+16*i), _mm_unpacklo_epi64( _mm256_extracti128_si256(r[5*i+2],0), _mm256_extracti128_si256(r[5*i+3],0) ) );
                _mm_storeu_si128((void *) (tag+16*i+4), _mm_unpackhi_epi64( _mm256_extracti128_si256(r[5*i+2],0), _mm256_extracti128_si256(r[5*i+3],0) ) );
                
                _mm_storeu_si128((void *) (tag+16*i+8), _mm_unpacklo_epi64( _mm256_extracti128_si256(r[5*i+2],1), _mm256_extracti128_si256(r[5*i+3],1) ) );
                _mm_storeu_si128((void *) (tag+16*i+12), _mm_unpackhi_epi64( _mm256_extracti128_si256(r[5*i+2],1), _mm256_extracti128_si256(r[5*i+3],1) ) );
        }
        return 0;
}

int crypto_aead_decrypt( u32 *m, u32 *c, u32 mlen, u32 *tag, u32 tlen, u32 *ad, u32 adlen, u8 *k, u8 *npub, u32 klen )
{
        u8 i;
        u64 l, mblock, adblock;
        __m256i r[5*NUMINSTANCE];
        u32 index1, index2, index3, index4, index5, index6, index7, index8;
        
        mblock = mlen/2;
        adblock = adlen/2;
        
        for ( i = 0; i < NUMINSTANCE; i++ )
        {
                r[5*i+0] = _mm256_set_epi8(k[64*i+55], k[64*i+54], k[64*i+53], k[64*i+52], k[64*i+51], k[64*i+50], k[64*i+49], k[64*i+48], k[64*i+39], k[64*i+38], k[64*i+37], k[64*i+36], k[64*i+35], k[64*i+34], k[64*i+33], k[64*i+32], k[64*i+23], k[64*i+22], k[64*i+21], k[64*i+20], k[64*i+19], k[64*i+18], k[64*i+17], k[64*i+16], k[64*i+7], k[64*i+6], k[64*i+5], k[64*i+4], k[64*i+3], k[64*i+2], k[64*i+1], k[64*i+0]);
                r[5*i+1] = _mm256_set_epi8(k[64*i+63], k[64*i+62], k[64*i+61], k[64*i+60], k[64*i+59], k[64*i+58], k[64*i+57], k[64*i+56], k[64*i+47], k[64*i+46], k[64*i+45], k[64*i+44], k[64*i+43], k[64*i+42], k[64*i+41], k[64*i+40], k[64*i+31], k[64*i+30], k[64*i+29], k[64*i+28], k[64*i+27], k[64*i+26], k[64*i+25], k[64*i+24], k[64*i+15], k[64*i+14], k[64*i+13], k[64*i+12], k[64*i+11], k[64*i+10], k[64*i+9], k[64*i+8]);
                
                r[5*i+2] = _mm256_set_epi8(npub[64*i+55], npub[64*i+54], npub[64*i+53], npub[64*i+52], npub[64*i+51], npub[64*i+50], npub[64*i+49], npub[64*i+48], npub[64*i+39], npub[64*i+38], npub[64*i+37], npub[64*i+36], npub[64*i+35], npub[64*i+34], npub[64*i+33], npub[64*i+32], npub[64*i+23], npub[64*i+22], npub[64*i+21], npub[64*i+20], npub[64*i+19], npub[64*i+18], npub[64*i+17], npub[64*i+16], npub[64*i+7], npub[64*i+6], npub[64*i+5], npub[64*i+4], npub[64*i+3], npub[64*i+2], npub[64*i+1], npub[64*i+0]);
                r[5*i+3] = _mm256_set_epi8(npub[64*i+63], npub[64*i+62], npub[64*i+61], npub[64*i+60], npub[64*i+59], npub[64*i+58], npub[64*i+57], npub[64*i+56], npub[64*i+47], npub[64*i+46], npub[64*i+45], npub[64*i+44], npub[64*i+43], npub[64*i+42], npub[64*i+41], npub[64*i+40], npub[64*i+31], npub[64*i+30], npub[64*i+29], npub[64*i+28], npub[64*i+27], npub[64*i+26], npub[64*i+25], npub[64*i+24], npub[64*i+15], npub[64*i+14], npub[64*i+13], npub[64*i+12], npub[64*i+11], npub[64*i+10], npub[64*i+9], npub[64*i+8]);
                
                r[5*i+4] = _mm256_set_epi64x(0x0, 0x0, 0x0, 0x0);
        }
        sycon_parallel(r);
        for ( i = 0; i < NUMINSTANCE; i++)
        {
                r[5*i]^= _mm256_set_epi8(k[64*i+55], k[64*i+54], k[64*i+53], k[64*i+52], k[64*i+51], k[64*i+50], k[64*i+49], k[64*i+48], k[64*i+39], k[64*i+38], k[64*i+37], k[64*i+36], k[64*i+35], k[64*i+34], k[64*i+33], k[64*i+32], k[64*i+23], k[64*i+22], k[64*i+21], k[64*i+20], k[64*i+19], k[64*i+18], k[64*i+17], k[64*i+16], k[64*i+7], k[64*i+6], k[64*i+5], k[64*i+4], k[64*i+3], k[64*i+2], k[64*i+1], k[64*i+0]);
        }
        sycon_parallel(r);
        for ( i = 0; i < NUMINSTANCE; i++)
        {
                r[5*i]^= _mm256_set_epi8(k[64*i+63], k[64*i+62], k[64*i+61], k[64*i+60], k[64*i+59], k[64*i+58], k[64*i+57], k[64*i+56], k[64*i+47], k[64*i+46], k[64*i+45], k[64*i+44], k[64*i+43], k[64*i+42], k[64*i+41], k[64*i+40], k[64*i+31], k[64*i+30], k[64*i+29], k[64*i+28], k[64*i+27], k[64*i+26], k[64*i+25], k[64*i+24], k[64*i+15], k[64*i+14], k[64*i+13], k[64*i+12], k[64*i+11], k[64*i+10], k[64*i+9], k[64*i+8]);
                //Domain separator
                r[5*i+4]^=_mm256_set_epi32(0x80000000, 0x00000000, 0x80000000, 0x00000000, 0x80000000, 0x00000000, 0x80000000, 0x00000000);
        }
        sycon_parallel(r);
        
        //Proceesing Associated Data
        for ( l = 0; l < adblock-1; l++ )
        {
                for ( i = 0; i < NUMINSTANCE; i++)
                {
                        index1 = 2*l + adlen*4*i;
                        index2 = 2*l + adlen*4*i+1;
                        index3 = 2*l + adlen*4*i + adlen;
                        index4 = 2*l + adlen*4*i + adlen + 1;
                        index5 = 2*l + adlen*4*i+2*adlen;
                        index6 = 2*l + adlen*4*i+1+2*adlen;
                        index7 = 2*l + adlen*4*i + 3*adlen;
                        index8 = 2*l + adlen*4*i + 3*adlen + 1;
                        r[5*i+0]^=_mm256_set_epi32(ad[index8], ad[index7], ad[index6], ad[index5], ad[index4], ad[index3], ad[index2], ad[index1]);
                        
                        //Domain separator
                        r[5*i+4]^=_mm256_set_epi32(0x80000000, 0x00000000, 0x80000000, 0x00000000, 0x80000000, 0x00000000, 0x80000000, 0x00000000);
                }
                sycon_parallel_redu(r);
        }
        
        //Processing last block
        l = adblock-1;
        for ( i = 0; i < NUMINSTANCE; i++)
        {
                index1 = 2*l + adlen*4*i;
                index2 = 2*l + adlen*4*i+1;
                index3 = 2*l + adlen*4*i + adlen;
                index4 = 2*l + adlen*4*i + adlen + 1;
                index5 = 2*l + adlen*4*i+2*adlen;
                index6 = 2*l + adlen*4*i+1+2*adlen;
                index7 = 2*l + adlen*4*i + 3*adlen;
                index8 = 2*l + adlen*4*i + 3*adlen + 1;
                r[5*i+0]^=_mm256_set_epi32(ad[index8], ad[index7], ad[index6], ad[index5], ad[index4], ad[index3], ad[index2], ad[index1]);
                
                //Domain seperator
                r[5*i+4]^=_mm256_set_epi32(0x40000000, 0x00000000, 0x40000000, 0x00000000, 0x40000000, 0x00000000, 0x40000000, 0x00000000);
        }
        sycon_parallel_redu(r);
        
        //Encrypt plaintext
        for ( l = 0; l < mblock-1; l++ )
        {
                for ( i = 0; i < NUMINSTANCE; i++)
                {
                        index1 = 2*l + mlen*4*i;
                        index2 = 2*l + mlen*4*i+1;
                        index3 = 2*l + mlen*4*i + mlen;
                        index4 = 2*l + mlen*4*i + mlen + 1;
                        index5 = 2*l + mlen*4*i+2*mlen;
                        index6 = 2*l + mlen*4*i+1+2*mlen;
                        index7 = 2*l + mlen*4*i + 3*mlen;
                        index8 = 2*l + mlen*4*i + 3*mlen + 1;
                        m[index1] = _mm256_extract_epi32 (r[5*i], 0)^c[index1] ;
                        m[index2] = _mm256_extract_epi32 (r[5*i], 1)^c[index2] ;
                        m[index3] = _mm256_extract_epi32 (r[5*i], 2)^c[index3] ;
                        m[index4] = _mm256_extract_epi32 (r[5*i], 3)^c[index4] ;
                        m[index5] = _mm256_extract_epi32 (r[5*i], 4)^c[index5] ;
                        m[index6] = _mm256_extract_epi32 (r[5*i], 5)^c[index6] ;
                        m[index7] = _mm256_extract_epi32 (r[5*i], 6)^c[index7] ;
                        m[index8] = _mm256_extract_epi32 (r[5*i], 7)^c[index8] ;
                        r[5*i]^=_mm256_set_epi32(m[index8], m[index7], m[index6], m[index5], m[index4], m[index3], m[index2], m[index1]);
                        
                        //Domain seperator
                        r[5*i+4]^=_mm256_set_epi32(0x40000000, 0x00000000, 0x40000000, 0x00000000, 0x40000000, 0x00000000, 0x40000000, 0x00000000);
                }
                sycon_parallel_redu(r);
        }
        //Processing last block
        l = mblock-1;
        for ( i = 0; i < NUMINSTANCE; i++)
        {
                index1 = 2*l + mlen*4*i;
                index2 = 2*l + mlen*4*i+1;
                index3 = 2*l + mlen*4*i + mlen;
                index4 = 2*l + mlen*4*i + mlen + 1;
                index5 = 2*l + mlen*4*i+2*mlen;
                index6 = 2*l + mlen*4*i+1+2*mlen;
                index7 = 2*l + mlen*4*i + 3*mlen;
                index8 = 2*l + mlen*4*i + 3*mlen + 1;
                m[index1] = _mm256_extract_epi32 (r[5*i], 0)^c[index1] ;
                m[index2] = _mm256_extract_epi32 (r[5*i], 1)^c[index2] ;
                m[index3] = _mm256_extract_epi32 (r[5*i], 2)^c[index3] ;
                m[index4] = _mm256_extract_epi32 (r[5*i], 3)^c[index4] ;
                m[index5] = _mm256_extract_epi32 (r[5*i], 4)^c[index5] ;
                m[index6] = _mm256_extract_epi32 (r[5*i], 5)^c[index6] ;
                m[index7] = _mm256_extract_epi32 (r[5*i], 6)^c[index7] ;
                m[index8] = _mm256_extract_epi32 (r[5*i], 7)^c[index8] ;
                r[5*i]^=_mm256_set_epi32(m[index8], m[index7], m[index6], m[index5], m[index4], m[index3], m[index2], m[index1]);
                
                //Domain seperator
                r[5*i+4]^=_mm256_set_epi32(0x20000000, 0x00000000, 0x20000000, 0x00000000, 0x20000000, 0x00000000, 0x20000000, 0x00000000);
        }
        sycon_parallel_redu(r);
        
        // Tag Extraction Phase
        for ( i = 0; i < NUMINSTANCE; i++)
        {
                r[5*i]^= _mm256_set_epi8(k[64*i+55], k[64*i+54], k[64*i+53], k[64*i+52], k[64*i+51], k[64*i+50], k[64*i+49], k[64*i+48], k[64*i+39], k[64*i+38], k[64*i+37], k[64*i+36], k[64*i+35], k[64*i+34], k[64*i+33], k[64*i+32], k[64*i+23], k[64*i+22], k[64*i+21], k[64*i+20], k[64*i+19], k[64*i+18], k[64*i+17], k[64*i+16], k[64*i+7], k[64*i+6], k[64*i+5], k[64*i+4], k[64*i+3], k[64*i+2], k[64*i+1], k[64*i+0]);
                //Domain separator
                r[5*i+4]^=_mm256_set_epi32(0x20000000, 0x00000000, 0x20000000, 0x00000000, 0x20000000, 0x00000000, 0x20000000, 0x00000000);
        }
        sycon_parallel(r);
        for ( i = 0; i < NUMINSTANCE; i++)
        {
                r[5*i]^= _mm256_set_epi8(k[64*i+63], k[64*i+62], k[64*i+61], k[64*i+60], k[64*i+59], k[64*i+58], k[64*i+57], k[64*i+56], k[64*i+47], k[64*i+46], k[64*i+45], k[64*i+44], k[64*i+43], k[64*i+42], k[64*i+41], k[64*i+40], k[64*i+31], k[64*i+30], k[64*i+29], k[64*i+28], k[64*i+27], k[64*i+26], k[64*i+25], k[64*i+24], k[64*i+15], k[64*i+14], k[64*i+13], k[64*i+12], k[64*i+11], k[64*i+10], k[64*i+9], k[64*i+8]);
                //Domain separator
                r[5*i+4]^=_mm256_set_epi32(0x20000000, 0x00000000, 0x20000000, 0x00000000, 0x20000000, 0x00000000, 0x20000000, 0x00000000);
        }
        sycon_parallel(r);
        
        for ( i = 0; i < NUMINSTANCE; i++)
        {
                _mm_storeu_si128((void *) (tag+16*i), _mm_unpacklo_epi64( _mm256_extracti128_si256(r[5*i+2],0), _mm256_extracti128_si256(r[5*i+3],0) ) );
                _mm_storeu_si128((void *) (tag+16*i+4), _mm_unpackhi_epi64( _mm256_extracti128_si256(r[5*i+2],0), _mm256_extracti128_si256(r[5*i+3],0) ) );
                
                _mm_storeu_si128((void *) (tag+16*i+8), _mm_unpacklo_epi64( _mm256_extracti128_si256(r[5*i+2],1), _mm256_extracti128_si256(r[5*i+3],1) ) );
                _mm_storeu_si128((void *) (tag+16*i+12), _mm_unpackhi_epi64( _mm256_extracti128_si256(r[5*i+2],1), _mm256_extracti128_si256(r[5*i+3],1) ) );
        }
        return 0;
}
