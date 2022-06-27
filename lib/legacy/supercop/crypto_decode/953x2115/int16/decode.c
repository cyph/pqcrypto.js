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
  a1 -= 124; /* -124...131 */
  a1 -= 124; /* -248...7 */
  a1 += (a1>>15)&124; /* -124...123 */
  a1 += (a1>>15)&124; /* 0...123 */
  R[0] = a1;
  
  /* reconstruct mod 1*[3846]+[2107] */
  
  ri = R[0];
  s1 = *--s;
  s0 = *--s;
  lo = mullo(ri,-4362);
  a0 = mulhi(ri,964)-mulhi(lo,3846); /* -1923...2164 */
  a0 += s1; /* -1923...2419 */
  lo = mullo(a0,-4362);
  a0 = mulhi(a0,964)-mulhi(lo,3846); /* -1952...1958 */
  a0 += s0; /* -1952...2213 */
  a0 += (a0>>15)&3846; /* 0...3845 */
  a1 = (ri<<15)+(s1<<7)+((s0-a0)>>1);
  a1 = mullo(a1,-16597);

  /* invalid inputs might need reduction mod 2107 */
  a1 -= 2107;
  a1 += (a1>>15)&2107;

  R[0] = a0;
  R[1] = a1;
  
  /* reconstruct mod 3*[15876]+[8694] */
  
  ri = R[1];
  s1 = *--s;
  s0 = *--s;
  lo = mullo(ri,-1057);
  a0 = mulhi(ri,-3716)-mulhi(lo,15876); /* -8867...7938 */
  a0 += s1; /* -8867...8193 */
  lo = mullo(a0,-1057);
  a0 = mulhi(a0,-3716)-mulhi(lo,15876); /* -8403...8440 */
  a0 += s0; /* -8403...8695 */
  a0 += (a0>>15)&15876; /* 0...15875 */
  a1 = (ri<<14)+(s1<<6)+((s0-a0)>>2);
  a1 = mullo(a1,12417);

  /* invalid inputs might need reduction mod 8694 */
  a1 -= 8694;
  a1 += (a1>>15)&8694;

  R[2] = a0;
  R[3] = a1;
  for (i = 0;i >= 0;--i) {
    ri = R[i];
    s1 = *--s;
    s0 = *--s;
    lo = mullo(ri,-1057);
    a0 = mulhi(ri,-3716)-mulhi(lo,15876); /* -8867...7938 */
    a0 += s1; /* -8867...8193 */
    lo = mullo(a0,-1057);
    a0 = mulhi(a0,-3716)-mulhi(lo,15876); /* -8403...8440 */
    a0 += s0; /* -8403...8695 */
    a0 += (a0>>15)&15876; /* 0...15875 */
    a1 = (ri<<14)+(s1<<6)+((s0-a0)>>2);
    a1 = mullo(a1,12417);

    /* invalid inputs might need reduction mod 15876 */
    a1 -= 15876;
    a1 += (a1>>15)&15876;

    R[2*i] = a0;
    R[2*i+1] = a1;
  }
  
  /* reconstruct mod 7*[126]+[69] */
  
  ri = R[3];
  a0 = ri;
  lo = mullo(a0,-520);
  a0 = mulhi(a0,16)-mulhi(lo,126); /* -63...67 */
  a0 += (a0>>15)&126; /* 0...125 */
  a1 = (ri-a0)>>1;
  a1 = mullo(a1,-4161);

  /* invalid inputs might need reduction mod 69 */
  a1 -= 69;
  a1 += (a1>>15)&69;

  R[6] = a0;
  R[7] = a1;
  for (i = 2;i >= 0;--i) {
    ri = R[i];
    a0 = ri;
    lo = mullo(a0,-520);
    a0 = mulhi(a0,16)-mulhi(lo,126); /* -63...67 */
    a0 += (a0>>15)&126; /* 0...125 */
    a1 = (ri-a0)>>1;
    a1 = mullo(a1,-4161);

    /* invalid inputs might need reduction mod 126 */
    a1 -= 126;
    a1 += (a1>>15)&126;

    R[2*i] = a0;
    R[2*i+1] = a1;
  }
  
  /* reconstruct mod 14*[2863]+[69] */
  
  R[14] = R[7];
  for (i = 6;i >= 0;--i) {
    ri = R[i];
    s1 = *--s;
    s0 = *--s;
    lo = mullo(ri,-5860);
    a0 = mulhi(ri,36)-mulhi(lo,2863); /* -1432...1440 */
    a0 += s1; /* -1432...1695 */
    lo = mullo(a0,-5860);
    a0 = mulhi(a0,36)-mulhi(lo,2863); /* -1433...1432 */
    a0 += s0; /* -1433...1687 */
    a0 += (a0>>15)&2863; /* 0...2862 */
    a1 = (s1<<8)+s0-a0;
    a1 = mullo(a1,7119);

    /* invalid inputs might need reduction mod 2863 */
    a1 -= 2863;
    a1 += (a1>>15)&2863;

    R[2*i] = a0;
    R[2*i+1] = a1;
  }
  
  /* reconstruct mod 29*[856]+[5227] */
  
  ri = R[14];
  s1 = *--s;
  s0 = *--s;
  lo = mullo(ri,-19600);
  a0 = mulhi(ri,-384)-mulhi(lo,856); /* -524...428 */
  a0 += s1; /* -524...683 */
  lo = mullo(a0,-19600);
  a0 = mulhi(a0,-384)-mulhi(lo,856); /* -433...431 */
  a0 += s0; /* -433...686 */
  a0 += (a0>>15)&856; /* 0...855 */
  a1 = (ri<<13)+(s1<<5)+((s0-a0)>>3);
  a1 = mullo(a1,-21437);

  /* invalid inputs might need reduction mod 5227 */
  a1 -= 5227;
  a1 += (a1>>15)&5227;

  R[28] = a0;
  R[29] = a1;
  for (i = 13;i >= 0;--i) {
    ri = R[i];
    s0 = *--s;
    lo = mullo(ri,-19600);
    a0 = mulhi(ri,-384)-mulhi(lo,856); /* -524...428 */
    a0 += s0; /* -524...683 */
    a0 += (a0>>15)&856; /* 0...855 */
    a1 = (ri<<5)+((s0-a0)>>3);
    a1 = mullo(a1,-21437);

    /* invalid inputs might need reduction mod 856 */
    a1 -= 856;
    a1 += (a1>>15)&856;

    R[2*i] = a0;
    R[2*i+1] = a1;
  }
  
  /* reconstruct mod 59*[468]+[2859] */
  
  ri = R[29];
  s0 = *--s;
  lo = mullo(ri,29687);
  a0 = mulhi(ri,-116)-mulhi(lo,468); /* -263...234 */
  a0 += s0; /* -263...489 */
  a0 -= 468; /* -731..>21 */
  a0 += (a0>>15)&468; /* -263...467 */
  a0 += (a0>>15)&468; /* 0...467 */
  a1 = (ri<<6)+((s0-a0)>>2);
  a1 = mullo(a1,-12323);

  /* invalid inputs might need reduction mod 2859 */
  a1 -= 2859;
  a1 += (a1>>15)&2859;

  R[58] = a0;
  R[59] = a1;
  for (i = 28;i >= 0;--i) {
    ri = R[i];
    s0 = *--s;
    lo = mullo(ri,29687);
    a0 = mulhi(ri,-116)-mulhi(lo,468); /* -263...234 */
    a0 += s0; /* -263...489 */
    a0 -= 468; /* -731..>21 */
    a0 += (a0>>15)&468; /* -263...467 */
    a0 += (a0>>15)&468; /* 0...467 */
    a1 = (ri<<6)+((s0-a0)>>2);
    a1 = mullo(a1,-12323);

    /* invalid inputs might need reduction mod 468 */
    a1 -= 468;
    a1 += (a1>>15)&468;

    R[2*i] = a0;
    R[2*i+1] = a1;
  }
  
  /* reconstruct mod 119*[346]+[2115] */
  
  ri = R[59];
  s0 = *--s;
  lo = mullo(ri,17047);
  a0 = mulhi(ri,22)-mulhi(lo,346); /* -173...178 */
  a0 += s0; /* -173...433 */
  a0 -= 346; /* -519..>87 */
  a0 += (a0>>15)&346; /* -173...345 */
  a0 += (a0>>15)&346; /* 0...345 */
  a1 = (ri<<7)+((s0-a0)>>1);
  a1 = mullo(a1,25381);

  /* invalid inputs might need reduction mod 2115 */
  a1 -= 2115;
  a1 += (a1>>15)&2115;

  R[118] = a0;
  R[119] = a1;
  for (i = 58;i >= 0;--i) {
    ri = R[i];
    s0 = *--s;
    lo = mullo(ri,17047);
    a0 = mulhi(ri,22)-mulhi(lo,346); /* -173...178 */
    a0 += s0; /* -173...433 */
    a0 -= 346; /* -519..>87 */
    a0 += (a0>>15)&346; /* -173...345 */
    a0 += (a0>>15)&346; /* 0...345 */
    a1 = (ri<<7)+((s0-a0)>>1);
    a1 = mullo(a1,25381);

    /* invalid inputs might need reduction mod 346 */
    a1 -= 346;
    a1 += (a1>>15)&346;

    R[2*i] = a0;
    R[2*i+1] = a1;
  }
  
  /* reconstruct mod 238*[4761]+[2115] */
  
  R[238] = R[119];
  for (i = 118;i >= 0;--i) {
    ri = R[i];
    s1 = *--s;
    s0 = *--s;
    lo = mullo(ri,-3524);
    a0 = mulhi(ri,-548)-mulhi(lo,4761); /* -2518...2380 */
    a0 += s1; /* -2518...2635 */
    lo = mullo(a0,-3524);
    a0 = mulhi(a0,-548)-mulhi(lo,4761); /* -2403...2401 */
    a0 += s0; /* -2403...2656 */
    a0 += (a0>>15)&4761; /* 0...4760 */
    a1 = (s1<<8)+s0-a0;
    a1 = mullo(a1,8617);

    /* invalid inputs might need reduction mod 4761 */
    a1 -= 4761;
    a1 += (a1>>15)&4761;

    R[2*i] = a0;
    R[2*i+1] = a1;
  }
  
  /* reconstruct mod 476*[69]+[2115] */
  
  R[476] = R[238];
  for (i = 237;i >= 0;--i) {
    ri = R[i];
    a0 = ri;
    lo = mullo(a0,-950);
    a0 = mulhi(a0,-14)-mulhi(lo,69); /* -38...34 */
    a0 += (a0>>15)&69; /* 0...68 */
    a1 = (ri-a0)>>0;
    a1 = mullo(a1,4749);

    /* invalid inputs might need reduction mod 69 */
    a1 -= 69;
    a1 += (a1>>15)&69;

    R[2*i] = a0;
    R[2*i+1] = a1;
  }
  
  /* reconstruct mod 953*[2115] */
  
  R[952] = 3*R[476]-3171;
  for (i = 475;i >= 0;--i) {
    ri = R[i];
    s1 = *--s;
    s0 = *--s;
    lo = mullo(ri,-7932);
    a0 = mulhi(ri,1036)-mulhi(lo,2115); /* -1058...1316 */
    a0 += s1; /* -1058...1571 */
    lo = mullo(a0,-7932);
    a0 = mulhi(a0,1036)-mulhi(lo,2115); /* -1075...1082 */
    a0 += s0; /* -1075...1337 */
    a0 += (a0>>15)&2115; /* 0...2114 */
    a1 = (s1<<8)+s0-a0;
    a1 = mullo(a1,-31637);

    /* invalid inputs might need reduction mod 2115 */
    a1 -= 2115;
    a1 += (a1>>15)&2115;

    R[2*i] = 3*a0-3171;
    R[2*i+1] = 3*a1-3171;
  }
}
