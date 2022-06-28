/* auto-generated; do not edit */

#include "crypto_encode.h"
#include "crypto_int16.h"
#include "crypto_uint16.h"
#include "crypto_uint32.h"
#define int16 crypto_int16
#define uint16 crypto_uint16
#define uint32 crypto_uint32

void crypto_encode(unsigned char *out,const void *v)
{
  const int16 *R0 = v;
  uint16 R1[429],R2[215],R3[108],R4[54],R5[27],R6[14],R7[7],R8[4],R9[2],R10[1];
  long long i;
  uint16 r0,r1;
  uint32 r2;
  
  for (i = 0;i < 428;++i) {
    r0 = R0[2*i]+2583;
    r1 = R0[2*i+1]+2583;
    r2 = r0+r1*(uint32)5167;
    *out++ = r2; r2 >>= 8;
    *out++ = r2; r2 >>= 8;
    R1[i] = r2;
  }
  R1[428] = R0[856]+2583;
  
  for (i = 0;i < 214;++i) {
    r0 = R1[2*i];
    r1 = R1[2*i+1];
    r2 = r0+r1*(uint32)408;
    *out++ = r2; r2 >>= 8;
    R2[i] = r2;
  }
  R2[214] = R1[428];
  
  for (i = 0;i < 107;++i) {
    r0 = R2[2*i];
    r1 = R2[2*i+1];
    r2 = r0+r1*(uint32)651;
    *out++ = r2; r2 >>= 8;
    R3[i] = r2;
  }
  R3[107] = R2[214];
  
  for (i = 0;i < 53;++i) {
    r0 = R3[2*i];
    r1 = R3[2*i+1];
    r2 = r0+r1*(uint32)1656;
    *out++ = r2; r2 >>= 8;
    R4[i] = r2;
  }
  r0 = R3[106];
  r1 = R3[107];
  r2 = r0+r1*(uint32)1656;
  *out++ = r2; r2 >>= 8;
  *out++ = r2; r2 >>= 8;
  R4[53] = r2;
  
  for (i = 0;i < 26;++i) {
    r0 = R4[2*i];
    r1 = R4[2*i+1];
    r2 = r0+r1*(uint32)10713;
    *out++ = r2; r2 >>= 8;
    *out++ = r2; r2 >>= 8;
    R5[i] = r2;
  }
  r0 = R4[52];
  r1 = R4[53];
  r2 = r0+r1*(uint32)10713;
  *out++ = r2; r2 >>= 8;
  R5[26] = r2;
  
  for (i = 0;i < 13;++i) {
    r0 = R5[2*i];
    r1 = R5[2*i+1];
    r2 = r0+r1*(uint32)1752;
    *out++ = r2; r2 >>= 8;
    R6[i] = r2;
  }
  R6[13] = R5[26];
  
  for (i = 0;i < 7;++i) {
    r0 = R6[2*i];
    r1 = R6[2*i+1];
    r2 = r0+r1*(uint32)11991;
    *out++ = r2; r2 >>= 8;
    *out++ = r2; r2 >>= 8;
    R7[i] = r2;
  }
  
  for (i = 0;i < 3;++i) {
    r0 = R7[2*i];
    r1 = R7[2*i+1];
    r2 = r0+r1*(uint32)2194;
    *out++ = r2; r2 >>= 8;
    *out++ = r2; r2 >>= 8;
    R8[i] = r2;
  }
  R8[3] = R7[6];
  
  r0 = R8[0];
  r1 = R8[1];
  r2 = r0+r1*(uint32)74;
  R9[0] = r2;
  r0 = R8[2];
  r1 = R8[3];
  r2 = r0+r1*(uint32)74;
  *out++ = r2; r2 >>= 8;
  R9[1] = r2;
  
  r0 = R9[0];
  r1 = R9[1];
  r2 = r0+r1*(uint32)5476;
  *out++ = r2; r2 >>= 8;
  R10[0] = r2;
  
  r0 = R10[0];
  *out++ = r0; r0 >>= 8;
  *out++ = r0; r0 >>= 8;
}
