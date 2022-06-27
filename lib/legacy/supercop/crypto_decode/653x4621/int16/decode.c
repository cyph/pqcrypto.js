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
  a1 -= 86; /* -86...169 */
  a1 -= 86; /* -172...83 */
  a1 += (a1>>15)&86; /* -86...85 */
  a1 += (a1>>15)&86; /* 0...85 */
  R[0] = a1;
  
  /* reconstruct mod 1*[835]+[6708] */
  
  ri = R[0];
  s1 = *--s;
  s0 = *--s;
  lo = mullo(ri,-20092);
  a0 = mulhi(ri,396)-mulhi(lo,835); /* -418...516 */
  a0 += s1; /* -418...771 */
  lo = mullo(a0,-20092);
  a0 = mulhi(a0,396)-mulhi(lo,835); /* -421...422 */
  a0 += s0; /* -421...677 */
  a0 += (a0>>15)&835; /* 0...834 */
  a1 = (s1<<8)+s0-a0;
  a1 = mullo(a1,8555);

  /* invalid inputs might need reduction mod 6708 */
  a1 -= 6708;
  a1 += (a1>>15)&6708;

  R[0] = a0;
  R[1] = a1;
  
  /* reconstruct mod 2*[7396]+[6708] */
  
  R[2] = R[1];
  for (i = 0;i >= 0;--i) {
    ri = R[i];
    s1 = *--s;
    s0 = *--s;
    lo = mullo(ri,-2268);
    a0 = mulhi(ri,3088)-mulhi(lo,7396); /* -3698...4470 */
    a0 += s1; /* -3698...4725 */
    lo = mullo(a0,-2268);
    a0 = mulhi(a0,3088)-mulhi(lo,7396); /* -3873...3920 */
    a0 += s0; /* -3873...4175 */
    a0 += (a0>>15)&7396; /* 0...7395 */
    a1 = (ri<<14)+(s1<<6)+((s0-a0)>>2);
    a1 = mullo(a1,-18679);

    /* invalid inputs might need reduction mod 7396 */
    a1 -= 7396;
    a1 += (a1>>15)&7396;

    R[2*i] = a0;
    R[2*i+1] = a1;
  }
  
  /* reconstruct mod 5*[86]+[78] */
  
  ri = R[2];
  a0 = ri;
  lo = mullo(a0,-762);
  a0 = mulhi(a0,4)-mulhi(lo,86); /* -43...44 */
  a0 += (a0>>15)&86; /* 0...85 */
  a1 = (ri-a0)>>1;
  a1 = mullo(a1,-16765);

  /* invalid inputs might need reduction mod 78 */
  a1 -= 78;
  a1 += (a1>>15)&78;

  R[4] = a0;
  R[5] = a1;
  for (i = 1;i >= 0;--i) {
    ri = R[i];
    a0 = ri;
    lo = mullo(a0,-762);
    a0 = mulhi(a0,4)-mulhi(lo,86); /* -43...44 */
    a0 += (a0>>15)&86; /* 0...85 */
    a1 = (ri-a0)>>1;
    a1 = mullo(a1,-16765);

    /* invalid inputs might need reduction mod 86 */
    a1 -= 86;
    a1 += (a1>>15)&86;

    R[2*i] = a0;
    R[2*i+1] = a1;
  }
  
  /* reconstruct mod 10*[2370]+[78] */
  
  R[10] = R[5];
  for (i = 4;i >= 0;--i) {
    ri = R[i];
    s1 = *--s;
    s0 = *--s;
    lo = mullo(ri,-7079);
    a0 = mulhi(ri,-14)-mulhi(lo,2370); /* -1189...1185 */
    a0 += s1; /* -1189...1440 */
    lo = mullo(a0,-7079);
    a0 = mulhi(a0,-14)-mulhi(lo,2370); /* -1186...1185 */
    a0 += s0; /* -1186...1440 */
    a0 += (a0>>15)&2370; /* 0...2369 */
    a1 = (ri<<15)+(s1<<7)+((s0-a0)>>1);
    a1 = mullo(a1,-8351);

    /* invalid inputs might need reduction mod 2370 */
    a1 -= 2370;
    a1 += (a1>>15)&2370;

    R[2*i] = a0;
    R[2*i+1] = a1;
  }
  
  /* reconstruct mod 20*[12461]+[78] */
  
  R[20] = R[10];
  for (i = 9;i >= 0;--i) {
    ri = R[i];
    s1 = *--s;
    s0 = *--s;
    lo = mullo(ri,-1346);
    a0 = mulhi(ri,4710)-mulhi(lo,12461); /* -6231...7408 */
    a0 += s1; /* -6231...7663 */
    lo = mullo(a0,-1346);
    a0 = mulhi(a0,4710)-mulhi(lo,12461); /* -6679...6781 */
    a0 += s0; /* -6679...7036 */
    a0 += (a0>>15)&12461; /* 0...12460 */
    a1 = (s1<<8)+s0-a0;
    a1 = mullo(a1,-19675);

    /* invalid inputs might need reduction mod 12461 */
    a1 -= 12461;
    a1 += (a1>>15)&12461;

    R[2*i] = a0;
    R[2*i+1] = a1;
  }
  
  /* reconstruct mod 40*[1786]+[78] */
  
  R[40] = R[20];
  for (i = 19;i >= 0;--i) {
    ri = R[i];
    s0 = *--s;
    lo = mullo(ri,-9394);
    a0 = mulhi(ri,-468)-mulhi(lo,1786); /* -1010...893 */
    a0 += s0; /* -1010...1148 */
    a0 += (a0>>15)&1786; /* 0...1785 */
    a1 = (ri<<7)+((s0-a0)>>1);
    a1 = mullo(a1,-12843);

    /* invalid inputs might need reduction mod 1786 */
    a1 -= 1786;
    a1 += (a1>>15)&1786;

    R[2*i] = a0;
    R[2*i+1] = a1;
  }
  
  /* reconstruct mod 81*[676]+[7510] */
  
  ri = R[40];
  s1 = *--s;
  s0 = *--s;
  lo = mullo(ri,-24818);
  a0 = mulhi(ri,248)-mulhi(lo,676); /* -338...400 */
  a0 += s1; /* -338...655 */
  lo = mullo(a0,-24818);
  a0 = mulhi(a0,248)-mulhi(lo,676); /* -340...340 */
  a0 += s0; /* -340...595 */
  a0 += (a0>>15)&676; /* 0...675 */
  a1 = (ri<<14)+(s1<<6)+((s0-a0)>>2);
  a1 = mullo(a1,-23655);

  /* invalid inputs might need reduction mod 7510 */
  a1 -= 7510;
  a1 += (a1>>15)&7510;

  R[80] = a0;
  R[81] = a1;
  for (i = 39;i >= 0;--i) {
    ri = R[i];
    s0 = *--s;
    lo = mullo(ri,-24818);
    a0 = mulhi(ri,248)-mulhi(lo,676); /* -338...400 */
    a0 += s0; /* -338...655 */
    a0 += (a0>>15)&676; /* 0...675 */
    a1 = (ri<<6)+((s0-a0)>>2);
    a1 = mullo(a1,-23655);

    /* invalid inputs might need reduction mod 676 */
    a1 -= 676;
    a1 += (a1>>15)&676;

    R[2*i] = a0;
    R[2*i+1] = a1;
  }
  
  /* reconstruct mod 163*[416]+[4621] */
  
  ri = R[81];
  s0 = *--s;
  lo = mullo(ri,25206);
  a0 = mulhi(ri,-64)-mulhi(lo,416); /* -224...208 */
  a0 += s0; /* -224...463 */
  a0 -= 416; /* -640..>47 */
  a0 += (a0>>15)&416; /* -224...415 */
  a0 += (a0>>15)&416; /* 0...415 */
  a1 = (ri<<3)+((s0-a0)>>5);
  a1 = mullo(a1,20165);

  /* invalid inputs might need reduction mod 4621 */
  a1 -= 4621;
  a1 += (a1>>15)&4621;

  R[162] = a0;
  R[163] = a1;
  for (i = 80;i >= 0;--i) {
    ri = R[i];
    s0 = *--s;
    lo = mullo(ri,25206);
    a0 = mulhi(ri,-64)-mulhi(lo,416); /* -224...208 */
    a0 += s0; /* -224...463 */
    a0 -= 416; /* -640..>47 */
    a0 += (a0>>15)&416; /* -224...415 */
    a0 += (a0>>15)&416; /* 0...415 */
    a1 = (ri<<3)+((s0-a0)>>5);
    a1 = mullo(a1,20165);

    /* invalid inputs might need reduction mod 416 */
    a1 -= 416;
    a1 += (a1>>15)&416;

    R[2*i] = a0;
    R[2*i+1] = a1;
  }
  
  /* reconstruct mod 326*[326]+[4621] */
  
  R[326] = R[163];
  for (i = 162;i >= 0;--i) {
    ri = R[i];
    s0 = *--s;
    lo = mullo(ri,14072);
    a0 = mulhi(ri,-48)-mulhi(lo,326); /* -175...163 */
    a0 += s0; /* -175...418 */
    a0 -= 326; /* -501..>92 */
    a0 += (a0>>15)&326; /* -175...325 */
    a0 += (a0>>15)&326; /* 0...325 */
    a1 = (ri<<7)+((s0-a0)>>1);
    a1 = mullo(a1,-19701);

    /* invalid inputs might need reduction mod 326 */
    a1 -= 326;
    a1 += (a1>>15)&326;

    R[2*i] = a0;
    R[2*i+1] = a1;
  }
  
  /* reconstruct mod 653*[4621] */
  
  R[652] = R[326]-2310;
  for (i = 325;i >= 0;--i) {
    ri = R[i];
    s1 = *--s;
    s0 = *--s;
    lo = mullo(ri,-3631);
    a0 = mulhi(ri,-1635)-mulhi(lo,4621); /* -2720...2310 */
    a0 += s1; /* -2720...2565 */
    lo = mullo(a0,-3631);
    a0 = mulhi(a0,-1635)-mulhi(lo,4621); /* -2375...2378 */
    a0 += s0; /* -2375...2633 */
    a0 += (a0>>15)&4621; /* 0...4620 */
    a1 = (s1<<8)+s0-a0;
    a1 = mullo(a1,-29499);

    /* invalid inputs might need reduction mod 4621 */
    a1 -= 4621;
    a1 += (a1>>15)&4621;

    R[2*i] = a0-2310;
    R[2*i+1] = a1-2310;
  }
}
