#include "crypto_encode.h"
#include "crypto_int16.h"
#include "crypto_uint16.h"
#include "crypto_uint32.h"
#define int16 crypto_int16
#define uint16 crypto_uint16
#define uint32 crypto_uint32
#include <immintrin.h>

void crypto_encode(unsigned char *out,const void *v)
{
  const int16 *R0 = v;
  uint16 R1[381],R2[192],R3[96],R4[48],R5[24],R6[12],R7[6],R8[3],R9[2],R10[1];
  long long i;
  uint16 r0,r1;
  uint32 r2;
  __m256i x,y,x2,y2;

  for (i = 24;i < 48;i += 2) {
    x = _mm256_loadu_si256((__m256i *) (R0+16*i-8));
    x2 = _mm256_loadu_si256((__m256i *) (R0+16*i+16-8));

    x = _mm256_add_epi16(x,_mm256_set1_epi16(2295));
    x2 = _mm256_add_epi16(x2,_mm256_set1_epi16(2295));

    y = x & _mm256_set1_epi32(65535);
    y2 = x2 & _mm256_set1_epi32(65535);

    x = _mm256_srli_epi32(x,16);
    x2 = _mm256_srli_epi32(x2,16);

    x = _mm256_mullo_epi32(x,_mm256_set1_epi32(4591));
    x2 = _mm256_mullo_epi32(x2,_mm256_set1_epi32(4591));

    x = _mm256_add_epi32(y,x);
    x2 = _mm256_add_epi32(y2,x2);

    x = _mm256_shuffle_epi8(x,_mm256_set_epi8(
      15,14,11,10,7,6,3,2,13,12,9,8,5,4,1,0,
      15,14,11,10,7,6,3,2,13,12,9,8,5,4,1,0
      ));
    x2 = _mm256_shuffle_epi8(x2,_mm256_set_epi8(
      15,14,11,10,7,6,3,2,13,12,9,8,5,4,1,0,
      15,14,11,10,7,6,3,2,13,12,9,8,5,4,1,0
      ));

    x = _mm256_permute4x64_epi64(x,0xd8);
    x2 = _mm256_permute4x64_epi64(x2,0xd8);

    _mm256_storeu_si256((__m256i *) (R1+8*i-4),_mm256_permute2f128_si256(x,x2,0x31));
    _mm256_storeu_si256((__m256i *) (out+16*i-8),_mm256_permute2f128_si256(x,x2,0x20));
  }

  for (i = 0;i < 24;i += 2) {
    x = _mm256_loadu_si256((__m256i *) (R0+16*i));
    x2 = _mm256_loadu_si256((__m256i *) (R0+16*i+16));

    x = _mm256_add_epi16(x,_mm256_set1_epi16(2295));
    x2 = _mm256_add_epi16(x2,_mm256_set1_epi16(2295));

    y = x & _mm256_set1_epi32(65535);
    y2 = x2 & _mm256_set1_epi32(65535);

    x = _mm256_srli_epi32(x,16);
    x2 = _mm256_srli_epi32(x2,16);

    x = _mm256_mullo_epi32(x,_mm256_set1_epi32(4591));
    x2 = _mm256_mullo_epi32(x2,_mm256_set1_epi32(4591));

    x = _mm256_add_epi32(y,x);
    x2 = _mm256_add_epi32(y2,x2);

    x = _mm256_shuffle_epi8(x,_mm256_set_epi8(
      15,14,11,10,7,6,3,2,13,12,9,8,5,4,1,0,
      15,14,11,10,7,6,3,2,13,12,9,8,5,4,1,0
      ));
    x2 = _mm256_shuffle_epi8(x2,_mm256_set_epi8(
      15,14,11,10,7,6,3,2,13,12,9,8,5,4,1,0,
      15,14,11,10,7,6,3,2,13,12,9,8,5,4,1,0
      ));

    x = _mm256_permute4x64_epi64(x,0xd8);
    x2 = _mm256_permute4x64_epi64(x2,0xd8);

    _mm256_storeu_si256((__m256i *) (R1+8*i),_mm256_permute2f128_si256(x,x2,0x31));
    _mm256_storeu_si256((__m256i *) (out+16*i),_mm256_permute2f128_si256(x,x2,0x20));
  }

  out += 380*2;
  R1[380] = R0[760]+2295;
  

  for (i = 12;i < 24;++i) {
    x = _mm256_loadu_si256((__m256i *) (R1-4+16*i));
    y = x & _mm256_set1_epi32(65535);
    x = _mm256_srli_epi32(x,16);
    x = _mm256_mullo_epi32(x,_mm256_set1_epi32(322));
    x = _mm256_add_epi32(y,x);
    x = _mm256_shuffle_epi8(x,_mm256_set_epi8(
      12,8,4,0,12,8,4,0,14,13,10,9,6,5,2,1,
      12,8,4,0,12,8,4,0,14,13,10,9,6,5,2,1
      ));
    x = _mm256_permute4x64_epi64(x,0xd8);
    _mm_storeu_si128((__m128i *) (R2-2+8*i),_mm256_extractf128_si256(x,0));
    *((uint32 *) (out-2+8*i)) = _mm256_extract_epi32(x,4);
    *((uint32 *) (out-2+8*i+4)) = _mm256_extract_epi32(x,6);
  }

  for (i = 0;i < 12;++i) {
    x = _mm256_loadu_si256((__m256i *) (R1+16*i));
    y = x & _mm256_set1_epi32(65535);
    x = _mm256_srli_epi32(x,16);
    x = _mm256_mullo_epi32(x,_mm256_set1_epi32(322));
    x = _mm256_add_epi32(y,x);
    x = _mm256_shuffle_epi8(x,_mm256_set_epi8(
      12,8,4,0,12,8,4,0,14,13,10,9,6,5,2,1,
      12,8,4,0,12,8,4,0,14,13,10,9,6,5,2,1
      ));
    x = _mm256_permute4x64_epi64(x,0xd8);
    _mm_storeu_si128((__m128i *) (R2+8*i),_mm256_extractf128_si256(x,0));
    *((uint32 *) (out+8*i)) = _mm256_extract_epi32(x,4);
    *((uint32 *) (out+8*i+4)) = _mm256_extract_epi32(x,6);
  }

  out += 190;
  R2[190] = R1[380];
  

  for (i = 0;i < 12;++i) {
    x = _mm256_loadu_si256((__m256i *) (R2+16*i));
    y = x & _mm256_set1_epi32(65535);
    x = _mm256_srli_epi32(x,16);
    x = _mm256_mullo_epi32(x,_mm256_set1_epi32(406));
    x = _mm256_add_epi32(y,x);
    x = _mm256_shuffle_epi8(x,_mm256_set_epi8(
      12,8,4,0,12,8,4,0,14,13,10,9,6,5,2,1,
      12,8,4,0,12,8,4,0,14,13,10,9,6,5,2,1
      ));
    x = _mm256_permute4x64_epi64(x,0xd8);
    _mm_storeu_si128((__m128i *) (R3+8*i),_mm256_extractf128_si256(x,0));
    *((uint32 *) (out+8*i)) = _mm256_extract_epi32(x,4);
    *((uint32 *) (out+8*i+4)) = _mm256_extract_epi32(x,6);
  }

  out += 95;
  R3[95] = R2[190];
  

  for (i = 0;i < 6;++i) {
    x = _mm256_loadu_si256((__m256i *) (R3+16*i));
    y = x & _mm256_set1_epi32(65535);
    x = _mm256_srli_epi32(x,16);
    x = _mm256_mullo_epi32(x,_mm256_set1_epi32(644));
    x = _mm256_add_epi32(y,x);
    x = _mm256_shuffle_epi8(x,_mm256_set_epi8(
      12,8,4,0,12,8,4,0,14,13,10,9,6,5,2,1,
      12,8,4,0,12,8,4,0,14,13,10,9,6,5,2,1
      ));
    x = _mm256_permute4x64_epi64(x,0xd8);
    _mm_storeu_si128((__m128i *) (R4+8*i),_mm256_extractf128_si256(x,0));
    ((uint32 *) out)[0] = _mm256_extract_epi32(x,4);
    ((uint32 *) out)[1] = _mm256_extract_epi32(x,6);
    out += 8;
  }


  for (i = 0;i < 3;++i) {
    x = _mm256_loadu_si256((__m256i *) (R4+16*i));
    y = x & _mm256_set1_epi32(65535);
    x = _mm256_srli_epi32(x,16);
    x = _mm256_mullo_epi32(x,_mm256_set1_epi32(1621));
    x = _mm256_add_epi32(y,x);
    x = _mm256_shuffle_epi8(x,_mm256_set_epi8(
      12,8,4,0,12,8,4,0,14,13,10,9,6,5,2,1,
      12,8,4,0,12,8,4,0,14,13,10,9,6,5,2,1
      ));
    x = _mm256_permute4x64_epi64(x,0xd8);
    _mm_storeu_si128((__m128i *) (R5+8*i),_mm256_extractf128_si256(x,0));
    *((uint32 *) (out+8*i)) = _mm256_extract_epi32(x,4);
    *((uint32 *) (out+8*i+4)) = _mm256_extract_epi32(x,6);
  }

  out += 23;


  r0 = R4[46];
  r1 = R4[47];
  r2 = r0+r1*(uint32)1621;
  *out++ = r2; r2 >>= 8;
  *out++ = r2; r2 >>= 8;
  R5[23] = r2;
  
  for (i = 0;i < 11;++i) {
    r0 = R5[2*i];
    r1 = R5[2*i+1];
    r2 = r0+r1*(uint32)10265;
    *out++ = r2; r2 >>= 8;
    *out++ = r2; r2 >>= 8;
    R6[i] = r2;
  }
  r0 = R5[22];
  r1 = R5[23];
  r2 = r0+r1*(uint32)10265;
  *out++ = r2; r2 >>= 8;
  R6[11] = r2;
  
  for (i = 0;i < 5;++i) {
    r0 = R6[2*i];
    r1 = R6[2*i+1];
    r2 = r0+r1*(uint32)1608;
    *out++ = r2; r2 >>= 8;
    R7[i] = r2;
  }
  r0 = R6[10];
  r1 = R6[11];
  r2 = r0+r1*(uint32)1608;
  *out++ = r2; r2 >>= 8;
  *out++ = r2; r2 >>= 8;
  R7[5] = r2;
  
  for (i = 0;i < 2;++i) {
    r0 = R7[2*i];
    r1 = R7[2*i+1];
    r2 = r0+r1*(uint32)10101;
    *out++ = r2; r2 >>= 8;
    *out++ = r2; r2 >>= 8;
    R8[i] = r2;
  }
  r0 = R7[4];
  r1 = R7[5];
  r2 = r0+r1*(uint32)10101;
  *out++ = r2; r2 >>= 8;
  R8[2] = r2;
  
  r0 = R8[0];
  r1 = R8[1];
  r2 = r0+r1*(uint32)1557;
  *out++ = r2; r2 >>= 8;
  R9[0] = r2;
  R9[1] = R8[2];
  
  r0 = R9[0];
  r1 = R9[1];
  r2 = r0+r1*(uint32)9470;
  *out++ = r2; r2 >>= 8;
  *out++ = r2; r2 >>= 8;
  R10[0] = r2;
  
  r0 = R10[0];
  *out++ = r0; r0 >>= 8;
  *out++ = r0; r0 >>= 8;
}
