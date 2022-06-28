/* auto-generated; do not edit */

#include "crypto_decode.h"
#include "crypto_int16.h"
#include "crypto_int32.h"
#define int16 crypto_int16
#define int32 crypto_int32

static int16 mullo(int16 x,int16 y)
{
  return x*y;
}

static int16 mulhi(int16 x,int16 y)
{
  return (x*(int32)y)>>16;
}

void crypto_decode(void *v,const unsigned char *s)
{
  int16 *R0 = v;
  int16 R1[327],R2[164],R3[82],R4[41],R5[21],R6[11],R7[6],R8[3],R9[2],R10[1];
  long long i;
  int16 a0,a1,a2;
  
  s += crypto_decode_STRBYTES;
  a1 = 0;
  a1 += *--s; /* 0...255 */
  a1 = mulhi(a1,-48)-mulhi(mullo(a1,-6433),2608);
  a1 += *--s; /* -1304...1558 */
  a1 += (a1>>15)&2608; /* 0...2607 */
  R10[0] = a1;
  
  /* R10 ------> R9: reconstruct mod 1*[71]+[9402] */
  
  a2 = a0 = R10[0];
  a0 = mulhi(a0,-13)-mulhi(mullo(a0,25845),71); /* -39...35 */
  a0 += *--s; /* -39...290 */
  a0 = mulhi(a0,3)-mulhi(mullo(a0,-923),71); /* -36...35 */
  a0 += (a0>>15)&71; /* 0...70 */
  a1 = (a2<<8)+s[0]-a0;
  a1 = mullo(a1,-22153);

  /* invalid inputs might need reduction mod 9402 */
  a1 -= 9402;
  a1 += (a1>>15)&9402;

  R9[0] = a0;
  R9[1] = a1;
  
  /* R9 ------> R8: reconstruct mod 2*[134]+[9402] */
  
  R8[2] = R9[1];
  for (i = 0;i >= 0;--i) {
    a2 = a0 = R9[i];
    a0 = mulhi(a0,14)-mulhi(mullo(a0,5869),134); /* -67...70 */
    a0 += *--s; /* -67...325 */
    a0 = mulhi(a0,10)-mulhi(mullo(a0,-489),134); /* -68...67 */
    a0 += (a0>>15)&134; /* 0...133 */
    a1 = (a2<<7)+((s[0]-a0)>>1);
    a1 = mullo(a1,19563);

    /* invalid inputs might need reduction mod 134 */
    a1 -= 134;
    a1 += (a1>>15)&134;

    R8[2*i] = a0;
    R8[2*i+1] = a1;
  }
  
  /* R8 ------> R7: reconstruct mod 5*[2953]+[815] */
  
  a2 = a0 = R8[2];
  a0 = mulhi(a0,1223)-mulhi(mullo(a0,-5681),2953); /* -1477...1782 */
  a0 += *--s; /* -1477...2037 */
  a0 += (a0>>15)&2953; /* 0...2952 */
  a1 = (a2<<8)+s[0]-a0;
  a1 = mullo(a1,-9543);

  /* invalid inputs might need reduction mod 815 */
  a1 -= 815;
  a1 += (a1>>15)&815;

  R7[4] = a0;
  R7[5] = a1;
  for (i = 1;i >= 0;--i) {
    a2 = a0 = R8[i];
    a0 = mulhi(a0,1223)-mulhi(mullo(a0,-5681),2953); /* -1477...1782 */
    a0 += *--s; /* -1477...2037 */
    a0 = mulhi(a0,1223)-mulhi(mullo(a0,-5681),2953); /* -1505...1514 */
    a0 += *--s; /* -1505...1769 */
    a0 += (a0>>15)&2953; /* 0...2952 */
    a1 = (s[1]<<8)+s[0]-a0;
    a1 = mullo(a1,-9543);

    /* invalid inputs might need reduction mod 2953 */
    a1 -= 2953;
    a1 += (a1>>15)&2953;

    R7[2*i] = a0;
    R7[2*i+1] = a1;
  }
  
  /* R7 ------> R6: reconstruct mod 10*[13910]+[815] */
  
  R6[10] = R7[5];
  for (i = 4;i >= 0;--i) {
    a2 = a0 = R7[i];
    a0 = mulhi(a0,1756)-mulhi(mullo(a0,-1206),13910); /* -6955...7394 */
    a0 += *--s; /* -6955...7649 */
    a0 = mulhi(a0,1756)-mulhi(mullo(a0,-1206),13910); /* -7142...7159 */
    a0 += *--s; /* -7142...7414 */
    a0 += (a0>>15)&13910; /* 0...13909 */
    a1 = (a2<<15)+(s[1]<<7)+((s[0]-a0)>>1);
    a1 = mullo(a1,-13437);

    /* invalid inputs might need reduction mod 13910 */
    a1 -= 13910;
    a1 += (a1>>15)&13910;

    R6[2*i] = a0;
    R6[2*i+1] = a1;
  }
  
  /* R6 ------> R5: reconstruct mod 20*[1887]+[815] */
  
  R5[20] = R6[10];
  for (i = 9;i >= 0;--i) {
    a2 = a0 = R6[i];
    a0 = mulhi(a0,-101)-mulhi(mullo(a0,-8891),1887); /* -969...943 */
    a0 += *--s; /* -969...1198 */
    a0 += (a0>>15)&1887; /* 0...1886 */
    a1 = (a2<<8)+s[0]-a0;
    a1 = mullo(a1,5279);

    /* invalid inputs might need reduction mod 1887 */
    a1 -= 1887;
    a1 += (a1>>15)&1887;

    R5[2*i] = a0;
    R5[2*i+1] = a1;
  }
  
  /* R5 ------> R4: reconstruct mod 40*[695]+[815] */
  
  R4[40] = R5[20];
  for (i = 19;i >= 0;--i) {
    a2 = a0 = R5[i];
    a0 = mulhi(a0,-84)-mulhi(mullo(a0,-24140),695); /* -369...347 */
    a0 += *--s; /* -369...602 */
    a0 += (a0>>15)&695; /* 0...694 */
    a1 = (a2<<8)+s[0]-a0;
    a1 = mullo(a1,31495);

    /* invalid inputs might need reduction mod 695 */
    a1 -= 695;
    a1 += (a1>>15)&695;

    R4[2*i] = a0;
    R4[2*i+1] = a1;
  }
  
  /* R4 ------> R3: reconstruct mod 81*[6745]+[7910] */
  
  a2 = a0 = R4[40];
  a0 = mulhi(a0,2401)-mulhi(mullo(a0,-2487),6745); /* -3373...3972 */
  a0 += *--s; /* -3373...4227 */
  a0 = mulhi(a0,2401)-mulhi(mullo(a0,-2487),6745); /* -3497...3527 */
  a0 += *--s; /* -3497...3782 */
  a0 += (a0>>15)&6745; /* 0...6744 */
  a1 = (s[1]<<8)+s[0]-a0;
  a1 = mullo(a1,-29207);

  /* invalid inputs might need reduction mod 7910 */
  a1 -= 7910;
  a1 += (a1>>15)&7910;

  R3[80] = a0;
  R3[81] = a1;
  for (i = 39;i >= 0;--i) {
    a2 = a0 = R4[i];
    a0 = mulhi(a0,2401)-mulhi(mullo(a0,-2487),6745); /* -3373...3972 */
    a0 += *--s; /* -3373...4227 */
    a0 = mulhi(a0,2401)-mulhi(mullo(a0,-2487),6745); /* -3497...3527 */
    a0 += *--s; /* -3497...3782 */
    a0 += (a0>>15)&6745; /* 0...6744 */
    a1 = (s[1]<<8)+s[0]-a0;
    a1 = mullo(a1,-29207);

    /* invalid inputs might need reduction mod 6745 */
    a1 -= 6745;
    a1 += (a1>>15)&6745;

    R3[2*i] = a0;
    R3[2*i+1] = a1;
  }
  
  /* R3 ------> R2: reconstruct mod 163*[1314]+[1541] */
  
  a2 = a0 = R3[81];
  a0 = mulhi(a0,64)-mulhi(mullo(a0,-12768),1314); /* -657...673 */
  a0 += *--s; /* -657...928 */
  a0 += (a0>>15)&1314; /* 0...1313 */
  a1 = (a2<<7)+((s[0]-a0)>>1);
  a1 = mullo(a1,-399);

  /* invalid inputs might need reduction mod 1541 */
  a1 -= 1541;
  a1 += (a1>>15)&1541;

  R2[162] = a0;
  R2[163] = a1;
  for (i = 80;i >= 0;--i) {
    a2 = a0 = R3[i];
    a0 = mulhi(a0,64)-mulhi(mullo(a0,-12768),1314); /* -657...673 */
    a0 += *--s; /* -657...928 */
    a0 += (a0>>15)&1314; /* 0...1313 */
    a1 = (a2<<7)+((s[0]-a0)>>1);
    a1 = mullo(a1,-399);

    /* invalid inputs might need reduction mod 1314 */
    a1 -= 1314;
    a1 += (a1>>15)&1314;

    R2[2*i] = a0;
    R2[2*i+1] = a1;
  }
  
  /* R2 ------> R1: reconstruct mod 326*[9277]+[1541] */
  
  R1[326] = R2[163];
  for (i = 162;i >= 0;--i) {
    a2 = a0 = R2[i];
    a0 = mulhi(a0,4400)-mulhi(mullo(a0,-1808),9277); /* -4639...5738 */
    a0 += *--s; /* -4639...5993 */
    a0 = mulhi(a0,4400)-mulhi(mullo(a0,-1808),9277); /* -4950...5040 */
    a0 += *--s; /* -4950...5295 */
    a0 += (a0>>15)&9277; /* 0...9276 */
    a1 = (s[1]<<8)+s[0]-a0;
    a1 = mullo(a1,-27883);

    /* invalid inputs might need reduction mod 9277 */
    a1 -= 9277;
    a1 += (a1>>15)&9277;

    R1[2*i] = a0;
    R1[2*i+1] = a1;
  }
  
  /* R1 ------> R0: reconstruct mod 653*[1541] */
  
  R0[652] = 3*R1[326]-2310;
  for (i = 325;i >= 0;--i) {
    a2 = a0 = R1[i];
    a0 = mulhi(a0,349)-mulhi(mullo(a0,-10887),1541); /* -771...857 */
    a0 += *--s; /* -771...1112 */
    a0 += (a0>>15)&1541; /* 0...1540 */
    a1 = (a2<<8)+s[0]-a0;
    a1 = mullo(a1,-10547);

    /* invalid inputs might need reduction mod 1541 */
    a1 -= 1541;
    a1 += (a1>>15)&1541;

    R0[2*i] = 3*a0-2310;
    R0[2*i+1] = 3*a1-2310;
  }
}
