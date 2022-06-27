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
  int16 *R = v;
  long long i;
  int16 a0,a1,ri,lo,hi,s0,s1;
  
  s += crypto_decode_STRBYTES;
  a1 = 0;
  a1 += *--s; /* 0...255 */
  lo = mullo(a1,-6433);
  a1 = mulhi(a1,-48)-mulhi(lo,2608);
  a1 += *--s; /* -1304...1558 */
  a1 += (a1>>15)&2608; /* 0...2607 */
  R[0] = a1;
  
  /* reconstruct mod 1*[71]+[9402] */
  
  ri = R[0];
  s0 = *--s;
  lo = mullo(ri,25845);
  a0 = mulhi(ri,-13)-mulhi(lo,71); /* -39...35 */
  a0 += s0; /* -39...290 */
  lo = mullo(a0,-923);
  a0 = mulhi(a0,3)-mulhi(lo,71); /* -36...35 */
  a0 += (a0>>15)&71; /* 0...70 */
  a1 = (ri<<8)+s0-a0;
  a1 = mullo(a1,-22153);

  /* invalid inputs might need reduction mod 9402 */
  a1 -= 9402;
  a1 += (a1>>15)&9402;

  R[0] = a0;
  R[1] = a1;
  
  /* reconstruct mod 2*[134]+[9402] */
  
  R[2] = R[1];
  for (i = 0;i >= 0;--i) {
    ri = R[i];
    s0 = *--s;
    lo = mullo(ri,5869);
    a0 = mulhi(ri,14)-mulhi(lo,134); /* -67...70 */
    a0 += s0; /* -67...325 */
    lo = mullo(a0,-489);
    a0 = mulhi(a0,10)-mulhi(lo,134); /* -68...67 */
    a0 += (a0>>15)&134; /* 0...133 */
    a1 = (ri<<7)+((s0-a0)>>1);
    a1 = mullo(a1,19563);

    /* invalid inputs might need reduction mod 134 */
    a1 -= 134;
    a1 += (a1>>15)&134;

    R[2*i] = a0;
    R[2*i+1] = a1;
  }
  
  /* reconstruct mod 5*[2953]+[815] */
  
  ri = R[2];
  s0 = *--s;
  lo = mullo(ri,-5681);
  a0 = mulhi(ri,1223)-mulhi(lo,2953); /* -1477...1782 */
  a0 += s0; /* -1477...2037 */
  a0 += (a0>>15)&2953; /* 0...2952 */
  a1 = (ri<<8)+s0-a0;
  a1 = mullo(a1,-9543);

  /* invalid inputs might need reduction mod 815 */
  a1 -= 815;
  a1 += (a1>>15)&815;

  R[4] = a0;
  R[5] = a1;
  for (i = 1;i >= 0;--i) {
    ri = R[i];
    s1 = *--s;
    s0 = *--s;
    lo = mullo(ri,-5681);
    a0 = mulhi(ri,1223)-mulhi(lo,2953); /* -1477...1782 */
    a0 += s1; /* -1477...2037 */
    lo = mullo(a0,-5681);
    a0 = mulhi(a0,1223)-mulhi(lo,2953); /* -1505...1514 */
    a0 += s0; /* -1505...1769 */
    a0 += (a0>>15)&2953; /* 0...2952 */
    a1 = (s1<<8)+s0-a0;
    a1 = mullo(a1,-9543);

    /* invalid inputs might need reduction mod 2953 */
    a1 -= 2953;
    a1 += (a1>>15)&2953;

    R[2*i] = a0;
    R[2*i+1] = a1;
  }
  
  /* reconstruct mod 10*[13910]+[815] */
  
  R[10] = R[5];
  for (i = 4;i >= 0;--i) {
    ri = R[i];
    s1 = *--s;
    s0 = *--s;
    lo = mullo(ri,-1206);
    a0 = mulhi(ri,1756)-mulhi(lo,13910); /* -6955...7394 */
    a0 += s1; /* -6955...7649 */
    lo = mullo(a0,-1206);
    a0 = mulhi(a0,1756)-mulhi(lo,13910); /* -7142...7159 */
    a0 += s0; /* -7142...7414 */
    a0 += (a0>>15)&13910; /* 0...13909 */
    a1 = (ri<<15)+(s1<<7)+((s0-a0)>>1);
    a1 = mullo(a1,-13437);

    /* invalid inputs might need reduction mod 13910 */
    a1 -= 13910;
    a1 += (a1>>15)&13910;

    R[2*i] = a0;
    R[2*i+1] = a1;
  }
  
  /* reconstruct mod 20*[1887]+[815] */
  
  R[20] = R[10];
  for (i = 9;i >= 0;--i) {
    ri = R[i];
    s0 = *--s;
    lo = mullo(ri,-8891);
    a0 = mulhi(ri,-101)-mulhi(lo,1887); /* -969...943 */
    a0 += s0; /* -969...1198 */
    a0 += (a0>>15)&1887; /* 0...1886 */
    a1 = (ri<<8)+s0-a0;
    a1 = mullo(a1,5279);

    /* invalid inputs might need reduction mod 1887 */
    a1 -= 1887;
    a1 += (a1>>15)&1887;

    R[2*i] = a0;
    R[2*i+1] = a1;
  }
  
  /* reconstruct mod 40*[695]+[815] */
  
  R[40] = R[20];
  for (i = 19;i >= 0;--i) {
    ri = R[i];
    s0 = *--s;
    lo = mullo(ri,-24140);
    a0 = mulhi(ri,-84)-mulhi(lo,695); /* -369...347 */
    a0 += s0; /* -369...602 */
    a0 += (a0>>15)&695; /* 0...694 */
    a1 = (ri<<8)+s0-a0;
    a1 = mullo(a1,31495);

    /* invalid inputs might need reduction mod 695 */
    a1 -= 695;
    a1 += (a1>>15)&695;

    R[2*i] = a0;
    R[2*i+1] = a1;
  }
  
  /* reconstruct mod 81*[6745]+[7910] */
  
  ri = R[40];
  s1 = *--s;
  s0 = *--s;
  lo = mullo(ri,-2487);
  a0 = mulhi(ri,2401)-mulhi(lo,6745); /* -3373...3972 */
  a0 += s1; /* -3373...4227 */
  lo = mullo(a0,-2487);
  a0 = mulhi(a0,2401)-mulhi(lo,6745); /* -3497...3527 */
  a0 += s0; /* -3497...3782 */
  a0 += (a0>>15)&6745; /* 0...6744 */
  a1 = (s1<<8)+s0-a0;
  a1 = mullo(a1,-29207);

  /* invalid inputs might need reduction mod 7910 */
  a1 -= 7910;
  a1 += (a1>>15)&7910;

  R[80] = a0;
  R[81] = a1;
  for (i = 39;i >= 0;--i) {
    ri = R[i];
    s1 = *--s;
    s0 = *--s;
    lo = mullo(ri,-2487);
    a0 = mulhi(ri,2401)-mulhi(lo,6745); /* -3373...3972 */
    a0 += s1; /* -3373...4227 */
    lo = mullo(a0,-2487);
    a0 = mulhi(a0,2401)-mulhi(lo,6745); /* -3497...3527 */
    a0 += s0; /* -3497...3782 */
    a0 += (a0>>15)&6745; /* 0...6744 */
    a1 = (s1<<8)+s0-a0;
    a1 = mullo(a1,-29207);

    /* invalid inputs might need reduction mod 6745 */
    a1 -= 6745;
    a1 += (a1>>15)&6745;

    R[2*i] = a0;
    R[2*i+1] = a1;
  }
  
  /* reconstruct mod 163*[1314]+[1541] */
  
  ri = R[81];
  s0 = *--s;
  lo = mullo(ri,-12768);
  a0 = mulhi(ri,64)-mulhi(lo,1314); /* -657...673 */
  a0 += s0; /* -657...928 */
  a0 += (a0>>15)&1314; /* 0...1313 */
  a1 = (ri<<7)+((s0-a0)>>1);
  a1 = mullo(a1,-399);

  /* invalid inputs might need reduction mod 1541 */
  a1 -= 1541;
  a1 += (a1>>15)&1541;

  R[162] = a0;
  R[163] = a1;
  for (i = 80;i >= 0;--i) {
    ri = R[i];
    s0 = *--s;
    lo = mullo(ri,-12768);
    a0 = mulhi(ri,64)-mulhi(lo,1314); /* -657...673 */
    a0 += s0; /* -657...928 */
    a0 += (a0>>15)&1314; /* 0...1313 */
    a1 = (ri<<7)+((s0-a0)>>1);
    a1 = mullo(a1,-399);

    /* invalid inputs might need reduction mod 1314 */
    a1 -= 1314;
    a1 += (a1>>15)&1314;

    R[2*i] = a0;
    R[2*i+1] = a1;
  }
  
  /* reconstruct mod 326*[9277]+[1541] */
  
  R[326] = R[163];
  for (i = 162;i >= 0;--i) {
    ri = R[i];
    s1 = *--s;
    s0 = *--s;
    lo = mullo(ri,-1808);
    a0 = mulhi(ri,4400)-mulhi(lo,9277); /* -4639...5738 */
    a0 += s1; /* -4639...5993 */
    lo = mullo(a0,-1808);
    a0 = mulhi(a0,4400)-mulhi(lo,9277); /* -4950...5040 */
    a0 += s0; /* -4950...5295 */
    a0 += (a0>>15)&9277; /* 0...9276 */
    a1 = (s1<<8)+s0-a0;
    a1 = mullo(a1,-27883);

    /* invalid inputs might need reduction mod 9277 */
    a1 -= 9277;
    a1 += (a1>>15)&9277;

    R[2*i] = a0;
    R[2*i+1] = a1;
  }
  
  /* reconstruct mod 653*[1541] */
  
  R[652] = 3*R[326]-2310;
  for (i = 325;i >= 0;--i) {
    ri = R[i];
    s0 = *--s;
    lo = mullo(ri,-10887);
    a0 = mulhi(ri,349)-mulhi(lo,1541); /* -771...857 */
    a0 += s0; /* -771...1112 */
    a0 += (a0>>15)&1541; /* 0...1540 */
    a1 = (ri<<8)+s0-a0;
    a1 = mullo(a1,-10547);

    /* invalid inputs might need reduction mod 1541 */
    a1 -= 1541;
    a1 += (a1>>15)&1541;

    R[2*i] = 3*a0-2310;
    R[2*i+1] = 3*a1-2310;
  }
}
