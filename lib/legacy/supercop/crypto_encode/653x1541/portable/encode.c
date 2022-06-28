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
  uint16 R1[327],R2[164],R3[82],R4[41],R5[21],R6[11],R7[6],R8[3],R9[2],R10[1];
  long long i;
  uint16 r0,r1;
  uint32 r2;
  
  for (i = 0;i < 326;++i) {
    r0 = ((R0[2*i]+2310)*10923)>>15;
    r1 = ((R0[2*i+1]+2310)*10923)>>15;
    r2 = r0+r1*(uint32)1541;
    *out++ = r2; r2 >>= 8;
    R1[i] = r2;
  }
  R1[326] = ((R0[652]+2310)*10923)>>15;
  
  for (i = 0;i < 163;++i) {
    r0 = R1[2*i];
    r1 = R1[2*i+1];
    r2 = r0+r1*(uint32)9277;
    *out++ = r2; r2 >>= 8;
    *out++ = r2; r2 >>= 8;
    R2[i] = r2;
  }
  R2[163] = R1[326];
  
  for (i = 0;i < 82;++i) {
    r0 = R2[2*i];
    r1 = R2[2*i+1];
    r2 = r0+r1*(uint32)1314;
    *out++ = r2; r2 >>= 8;
    R3[i] = r2;
  }
  
  for (i = 0;i < 41;++i) {
    r0 = R3[2*i];
    r1 = R3[2*i+1];
    r2 = r0+r1*(uint32)6745;
    *out++ = r2; r2 >>= 8;
    *out++ = r2; r2 >>= 8;
    R4[i] = r2;
  }
  
  for (i = 0;i < 20;++i) {
    r0 = R4[2*i];
    r1 = R4[2*i+1];
    r2 = r0+r1*(uint32)695;
    *out++ = r2; r2 >>= 8;
    R5[i] = r2;
  }
  R5[20] = R4[40];
  
  for (i = 0;i < 10;++i) {
    r0 = R5[2*i];
    r1 = R5[2*i+1];
    r2 = r0+r1*(uint32)1887;
    *out++ = r2; r2 >>= 8;
    R6[i] = r2;
  }
  R6[10] = R5[20];
  
  for (i = 0;i < 5;++i) {
    r0 = R6[2*i];
    r1 = R6[2*i+1];
    r2 = r0+r1*(uint32)13910;
    *out++ = r2; r2 >>= 8;
    *out++ = r2; r2 >>= 8;
    R7[i] = r2;
  }
  R7[5] = R6[10];
  
  for (i = 0;i < 2;++i) {
    r0 = R7[2*i];
    r1 = R7[2*i+1];
    r2 = r0+r1*(uint32)2953;
    *out++ = r2; r2 >>= 8;
    *out++ = r2; r2 >>= 8;
    R8[i] = r2;
  }
  r0 = R7[4];
  r1 = R7[5];
  r2 = r0+r1*(uint32)2953;
  *out++ = r2; r2 >>= 8;
  R8[2] = r2;
  
  r0 = R8[0];
  r1 = R8[1];
  r2 = r0+r1*(uint32)134;
  *out++ = r2; r2 >>= 8;
  R9[0] = r2;
  R9[1] = R8[2];
  
  r0 = R9[0];
  r1 = R9[1];
  r2 = r0+r1*(uint32)71;
  *out++ = r2; r2 >>= 8;
  R10[0] = r2;
  
  r0 = R10[0];
  *out++ = r0; r0 >>= 8;
  *out++ = r0; r0 >>= 8;
}
