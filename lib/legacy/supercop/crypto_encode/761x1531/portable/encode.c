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
  uint16 R1[381],R2[191],R3[96],R4[48],R5[24],R6[12],R7[6],R8[3],R9[2],R10[1];
  long long i;
  uint16 r0,r1;
  uint32 r2;
  
  for (i = 0;i < 380;++i) {
    r0 = ((R0[2*i]+2295)*10923)>>15;
    r1 = ((R0[2*i+1]+2295)*10923)>>15;
    r2 = r0+r1*(uint32)1531;
    *out++ = r2; r2 >>= 8;
    R1[i] = r2;
  }
  R1[380] = ((R0[760]+2295)*10923)>>15;
  
  for (i = 0;i < 190;++i) {
    r0 = R1[2*i];
    r1 = R1[2*i+1];
    r2 = r0+r1*(uint32)9157;
    *out++ = r2; r2 >>= 8;
    *out++ = r2; r2 >>= 8;
    R2[i] = r2;
  }
  R2[190] = R1[380];
  
  for (i = 0;i < 95;++i) {
    r0 = R2[2*i];
    r1 = R2[2*i+1];
    r2 = r0+r1*(uint32)1280;
    *out++ = r2; r2 >>= 8;
    R3[i] = r2;
  }
  R3[95] = R2[190];
  
  for (i = 0;i < 48;++i) {
    r0 = R3[2*i];
    r1 = R3[2*i+1];
    r2 = r0+r1*(uint32)6400;
    *out++ = r2; r2 >>= 8;
    *out++ = r2; r2 >>= 8;
    R4[i] = r2;
  }
  
  for (i = 0;i < 24;++i) {
    r0 = R4[2*i];
    r1 = R4[2*i+1];
    r2 = r0+r1*(uint32)625;
    *out++ = r2; r2 >>= 8;
    R5[i] = r2;
  }
  
  for (i = 0;i < 12;++i) {
    r0 = R5[2*i];
    r1 = R5[2*i+1];
    r2 = r0+r1*(uint32)1526;
    *out++ = r2; r2 >>= 8;
    R6[i] = r2;
  }
  
  for (i = 0;i < 6;++i) {
    r0 = R6[2*i];
    r1 = R6[2*i+1];
    r2 = r0+r1*(uint32)9097;
    *out++ = r2; r2 >>= 8;
    *out++ = r2; r2 >>= 8;
    R7[i] = r2;
  }
  
  for (i = 0;i < 3;++i) {
    r0 = R7[2*i];
    r1 = R7[2*i+1];
    r2 = r0+r1*(uint32)1263;
    *out++ = r2; r2 >>= 8;
    R8[i] = r2;
  }
  
  r0 = R8[0];
  r1 = R8[1];
  r2 = r0+r1*(uint32)6232;
  *out++ = r2; r2 >>= 8;
  *out++ = r2; r2 >>= 8;
  R9[0] = r2;
  R9[1] = R8[2];
  
  r0 = R9[0];
  r1 = R9[1];
  r2 = r0+r1*(uint32)593;
  *out++ = r2; r2 >>= 8;
  R10[0] = r2;
  
  r0 = R10[0];
  *out++ = r0; r0 >>= 8;
  *out++ = r0; r0 >>= 8;
}
