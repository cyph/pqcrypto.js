#ifndef SYCON_H
#define SYCON_H
#define STATE_SIZE        10 // 320/32 = 10//

typedef unsigned long long int u64;
typedef unsigned int u32;
typedef unsigned int u8;

#define NUMINSTANCE     1

#define NUMROUNDS       14


#include<stdint.h>
#include<x86intrin.h>

#define ShuffByte1(r)      _mm_shuffle_epi8(r, _mm_set_epi8(1,2,6,4,5,3,0,7,1,2,6,4,5,3,0,7));
#define ShuffByte2(r)      _mm_shuffle_epi8(r, _mm_set_epi8(5,2,4,3,7,1,6,0,5,2,4,3,7,1,6,0));
#define ShuffByte3(r)      _mm_shuffle_epi8(r, _mm_set_epi8(3,6,0,1,5,4,2,7,3,6,0,1,5,4,2,7));
#define ShuffByte4(r)      _mm_shuffle_epi8(r, _mm_set_epi8(1,6,7,0,3,5,4,2,1,6,7,0,3,5,4,2));
#define ShuffByte5(r)      _mm_shuffle_epi8(r, _mm_set_epi8(4,2,7,5,0,1,6,3,4,2,7,5,0,1,6,3));

#define ROT64(x,u)        (_mm_slli_epi64 (x, 64-u)|_mm_srli_epi64 (x, u))
#define ROT16(y,c)        (_mm_slli_epi16 (y, 16-c)|_mm_srli_epi16 (y, c))

void sycon( u32 *state );
int crypto_hash( u32 *digest,  u32 *inp, u32 inlen);
#endif
