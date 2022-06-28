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
  a1 -= 86; /* -86...169 */
  a1 -= 86; /* -172...83 */
  a1 += (a1>>15)&86; /* -86...85 */
  a1 += (a1>>15)&86; /* 0...85 */
  R10[0] = a1;
  
  /* R10 ------> R9: reconstruct mod 1*[835]+[6708] */
  
  a2 = a0 = R10[0];
  a0 = mulhi(a0,396)-mulhi(mullo(a0,-20092),835); /* -418...516 */
  a0 += *--s; /* -418...771 */
  a0 = mulhi(a0,396)-mulhi(mullo(a0,-20092),835); /* -421...422 */
  a0 += *--s; /* -421...677 */
  a0 += (a0>>15)&835; /* 0...834 */
  a1 = (s[1]<<8)+s[0]-a0;
  a1 = mullo(a1,8555);

  /* invalid inputs might need reduction mod 6708 */
  a1 -= 6708;
  a1 += (a1>>15)&6708;

  R9[0] = a0;
  R9[1] = a1;
  
  /* R9 ------> R8: reconstruct mod 2*[7396]+[6708] */
  
  R8[2] = R9[1];
  for (i = 0;i >= 0;--i) {
    a2 = a0 = R9[i];
    a0 = mulhi(a0,3088)-mulhi(mullo(a0,-2268),7396); /* -3698...4470 */
    a0 += *--s; /* -3698...4725 */
    a0 = mulhi(a0,3088)-mulhi(mullo(a0,-2268),7396); /* -3873...3920 */
    a0 += *--s; /* -3873...4175 */
    a0 += (a0>>15)&7396; /* 0...7395 */
    a1 = (a2<<14)+(s[1]<<6)+((s[0]-a0)>>2);
    a1 = mullo(a1,-18679);

    /* invalid inputs might need reduction mod 7396 */
    a1 -= 7396;
    a1 += (a1>>15)&7396;

    R8[2*i] = a0;
    R8[2*i+1] = a1;
  }
  
  /* R8 ------> R7: reconstruct mod 5*[86]+[78] */
  
  a2 = a0 = R8[2];
  a0 = mulhi(a0,4)-mulhi(mullo(a0,-762),86); /* -43...44 */
  a0 += (a0>>15)&86; /* 0...85 */
  a1 = (a2-a0)>>1;
  a1 = mullo(a1,-16765);

  /* invalid inputs might need reduction mod 78 */
  a1 -= 78;
  a1 += (a1>>15)&78;

  R7[4] = a0;
  R7[5] = a1;
  for (i = 1;i >= 0;--i) {
    a2 = a0 = R8[i];
    a0 = mulhi(a0,4)-mulhi(mullo(a0,-762),86); /* -43...44 */
    a0 += (a0>>15)&86; /* 0...85 */
    a1 = (a2-a0)>>1;
    a1 = mullo(a1,-16765);

    /* invalid inputs might need reduction mod 86 */
    a1 -= 86;
    a1 += (a1>>15)&86;

    R7[2*i] = a0;
    R7[2*i+1] = a1;
  }
  
  /* R7 ------> R6: reconstruct mod 10*[2370]+[78] */
  
  R6[10] = R7[5];
  for (i = 4;i >= 0;--i) {
    a2 = a0 = R7[i];
    a0 = mulhi(a0,-14)-mulhi(mullo(a0,-7079),2370); /* -1189...1185 */
    a0 += *--s; /* -1189...1440 */
    a0 = mulhi(a0,-14)-mulhi(mullo(a0,-7079),2370); /* -1186...1185 */
    a0 += *--s; /* -1186...1440 */
    a0 += (a0>>15)&2370; /* 0...2369 */
    a1 = (a2<<15)+(s[1]<<7)+((s[0]-a0)>>1);
    a1 = mullo(a1,-8351);

    /* invalid inputs might need reduction mod 2370 */
    a1 -= 2370;
    a1 += (a1>>15)&2370;

    R6[2*i] = a0;
    R6[2*i+1] = a1;
  }
  
  /* R6 ------> R5: reconstruct mod 20*[12461]+[78] */
  
  R5[20] = R6[10];
  for (i = 9;i >= 0;--i) {
    a2 = a0 = R6[i];
    a0 = mulhi(a0,4710)-mulhi(mullo(a0,-1346),12461); /* -6231...7408 */
    a0 += *--s; /* -6231...7663 */
    a0 = mulhi(a0,4710)-mulhi(mullo(a0,-1346),12461); /* -6679...6781 */
    a0 += *--s; /* -6679...7036 */
    a0 += (a0>>15)&12461; /* 0...12460 */
    a1 = (s[1]<<8)+s[0]-a0;
    a1 = mullo(a1,-19675);

    /* invalid inputs might need reduction mod 12461 */
    a1 -= 12461;
    a1 += (a1>>15)&12461;

    R5[2*i] = a0;
    R5[2*i+1] = a1;
  }
  
  /* R5 ------> R4: reconstruct mod 40*[1786]+[78] */
  
  R4[40] = R5[20];
  for (i = 19;i >= 0;--i) {
    a2 = a0 = R5[i];
    a0 = mulhi(a0,-468)-mulhi(mullo(a0,-9394),1786); /* -1010...893 */
    a0 += *--s; /* -1010...1148 */
    a0 += (a0>>15)&1786; /* 0...1785 */
    a1 = (a2<<7)+((s[0]-a0)>>1);
    a1 = mullo(a1,-12843);

    /* invalid inputs might need reduction mod 1786 */
    a1 -= 1786;
    a1 += (a1>>15)&1786;

    R4[2*i] = a0;
    R4[2*i+1] = a1;
  }
  
  /* R4 ------> R3: reconstruct mod 81*[676]+[7510] */
  
  a2 = a0 = R4[40];
  a0 = mulhi(a0,248)-mulhi(mullo(a0,-24818),676); /* -338...400 */
  a0 += *--s; /* -338...655 */
  a0 = mulhi(a0,248)-mulhi(mullo(a0,-24818),676); /* -340...340 */
  a0 += *--s; /* -340...595 */
  a0 += (a0>>15)&676; /* 0...675 */
  a1 = (a2<<14)+(s[1]<<6)+((s[0]-a0)>>2);
  a1 = mullo(a1,-23655);

  /* invalid inputs might need reduction mod 7510 */
  a1 -= 7510;
  a1 += (a1>>15)&7510;

  R3[80] = a0;
  R3[81] = a1;
  for (i = 39;i >= 0;--i) {
    a2 = a0 = R4[i];
    a0 = mulhi(a0,248)-mulhi(mullo(a0,-24818),676); /* -338...400 */
    a0 += *--s; /* -338...655 */
    a0 += (a0>>15)&676; /* 0...675 */
    a1 = (a2<<6)+((s[0]-a0)>>2);
    a1 = mullo(a1,-23655);

    /* invalid inputs might need reduction mod 676 */
    a1 -= 676;
    a1 += (a1>>15)&676;

    R3[2*i] = a0;
    R3[2*i+1] = a1;
  }
  
  /* R3 ------> R2: reconstruct mod 163*[416]+[4621] */
  
  a2 = a0 = R3[81];
  a0 = mulhi(a0,-64)-mulhi(mullo(a0,25206),416); /* -224...208 */
  a0 += *--s; /* -224...463 */
  a0 -= 416; /* -640..>47 */
  a0 += (a0>>15)&416; /* -224...415 */
  a0 += (a0>>15)&416; /* 0...415 */
  a1 = (a2<<3)+((s[0]-a0)>>5);
  a1 = mullo(a1,20165);

  /* invalid inputs might need reduction mod 4621 */
  a1 -= 4621;
  a1 += (a1>>15)&4621;

  R2[162] = a0;
  R2[163] = a1;
  for (i = 80;i >= 0;--i) {
    a2 = a0 = R3[i];
    a0 = mulhi(a0,-64)-mulhi(mullo(a0,25206),416); /* -224...208 */
    a0 += *--s; /* -224...463 */
    a0 -= 416; /* -640..>47 */
    a0 += (a0>>15)&416; /* -224...415 */
    a0 += (a0>>15)&416; /* 0...415 */
    a1 = (a2<<3)+((s[0]-a0)>>5);
    a1 = mullo(a1,20165);

    /* invalid inputs might need reduction mod 416 */
    a1 -= 416;
    a1 += (a1>>15)&416;

    R2[2*i] = a0;
    R2[2*i+1] = a1;
  }
  
  /* R2 ------> R1: reconstruct mod 326*[326]+[4621] */
  
  R1[326] = R2[163];
  for (i = 162;i >= 0;--i) {
    a2 = a0 = R2[i];
    a0 = mulhi(a0,-48)-mulhi(mullo(a0,14072),326); /* -175...163 */
    a0 += *--s; /* -175...418 */
    a0 -= 326; /* -501..>92 */
    a0 += (a0>>15)&326; /* -175...325 */
    a0 += (a0>>15)&326; /* 0...325 */
    a1 = (a2<<7)+((s[0]-a0)>>1);
    a1 = mullo(a1,-19701);

    /* invalid inputs might need reduction mod 326 */
    a1 -= 326;
    a1 += (a1>>15)&326;

    R1[2*i] = a0;
    R1[2*i+1] = a1;
  }
  
  /* R1 ------> R0: reconstruct mod 653*[4621] */
  
  R0[652] = R1[326]-2310;
  for (i = 325;i >= 0;--i) {
    a2 = a0 = R1[i];
    a0 = mulhi(a0,-1635)-mulhi(mullo(a0,-3631),4621); /* -2720...2310 */
    a0 += *--s; /* -2720...2565 */
    a0 = mulhi(a0,-1635)-mulhi(mullo(a0,-3631),4621); /* -2375...2378 */
    a0 += *--s; /* -2375...2633 */
    a0 += (a0>>15)&4621; /* 0...4620 */
    a1 = (s[1]<<8)+s[0]-a0;
    a1 = mullo(a1,-29499);

    /* invalid inputs might need reduction mod 4621 */
    a1 -= 4621;
    a1 += (a1>>15)&4621;

    R0[2*i] = a0-2310;
    R0[2*i+1] = a1-2310;
  }
}
