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
  int16 R1[381],R2[191],R3[96],R4[48],R5[24],R6[12],R7[6],R8[3],R9[2],R10[1];
  long long i;
  int16 a0,a1,a2;
  
  s += crypto_decode_STRBYTES;
  a1 = 0;
  a1 += *--s; /* 0...255 */
  a1 = mulhi(a1,-84)-mulhi(mullo(a1,-4828),3475);
  a1 += *--s; /* -1738...1992 */
  a1 += (a1>>15)&3475; /* 0...3474 */
  R10[0] = a1;
  
  /* R10 ------> R9: reconstruct mod 1*[593]+[1500] */
  
  a2 = a0 = R10[0];
  a0 = mulhi(a0,60)-mulhi(mullo(a0,-28292),593); /* -297...311 */
  a0 += *--s; /* -297...566 */
  a0 += (a0>>15)&593; /* 0...592 */
  a1 = (a2<<8)+s[0]-a0;
  a1 = mullo(a1,-31055);

  /* invalid inputs might need reduction mod 1500 */
  a1 -= 1500;
  a1 += (a1>>15)&1500;

  R9[0] = a0;
  R9[1] = a1;
  
  /* R9 ------> R8: reconstruct mod 2*[6232]+[1500] */
  
  R8[2] = R9[1];
  for (i = 0;i >= 0;--i) {
    a2 = a0 = R9[i];
    a0 = mulhi(a0,672)-mulhi(mullo(a0,-2692),6232); /* -3116...3284 */
    a0 += *--s; /* -3116...3539 */
    a0 = mulhi(a0,672)-mulhi(mullo(a0,-2692),6232); /* -3148...3152 */
    a0 += *--s; /* -3148...3407 */
    a0 += (a0>>15)&6232; /* 0...6231 */
    a1 = (a2<<13)+(s[1]<<5)+((s[0]-a0)>>3);
    a1 = mullo(a1,12451);

    /* invalid inputs might need reduction mod 6232 */
    a1 -= 6232;
    a1 += (a1>>15)&6232;

    R8[2*i] = a0;
    R8[2*i+1] = a1;
  }
  
  /* R8 ------> R7: reconstruct mod 5*[1263]+[304] */
  
  a2 = a0 = R8[2];
  a0 = mulhi(a0,-476)-mulhi(mullo(a0,-13284),1263); /* -751...631 */
  a0 += *--s; /* -751...886 */
  a0 += (a0>>15)&1263; /* 0...1262 */
  a1 = (a2<<8)+s[0]-a0;
  a1 = mullo(a1,-22001);

  /* invalid inputs might need reduction mod 304 */
  a1 -= 304;
  a1 += (a1>>15)&304;

  R7[4] = a0;
  R7[5] = a1;
  for (i = 1;i >= 0;--i) {
    a2 = a0 = R8[i];
    a0 = mulhi(a0,-476)-mulhi(mullo(a0,-13284),1263); /* -751...631 */
    a0 += *--s; /* -751...886 */
    a0 += (a0>>15)&1263; /* 0...1262 */
    a1 = (a2<<8)+s[0]-a0;
    a1 = mullo(a1,-22001);

    /* invalid inputs might need reduction mod 1263 */
    a1 -= 1263;
    a1 += (a1>>15)&1263;

    R7[2*i] = a0;
    R7[2*i+1] = a1;
  }
  
  /* R7 ------> R6: reconstruct mod 11*[9097]+[2188] */
  
  a2 = a0 = R7[5];
  a0 = mulhi(a0,2348)-mulhi(mullo(a0,-1844),9097); /* -4549...5135 */
  a0 += *--s; /* -4549...5390 */
  a0 = mulhi(a0,2348)-mulhi(mullo(a0,-1844),9097); /* -4712...4741 */
  a0 += *--s; /* -4712...4996 */
  a0 += (a0>>15)&9097; /* 0...9096 */
  a1 = (s[1]<<8)+s[0]-a0;
  a1 = mullo(a1,17081);

  /* invalid inputs might need reduction mod 2188 */
  a1 -= 2188;
  a1 += (a1>>15)&2188;

  R6[10] = a0;
  R6[11] = a1;
  for (i = 4;i >= 0;--i) {
    a2 = a0 = R7[i];
    a0 = mulhi(a0,2348)-mulhi(mullo(a0,-1844),9097); /* -4549...5135 */
    a0 += *--s; /* -4549...5390 */
    a0 = mulhi(a0,2348)-mulhi(mullo(a0,-1844),9097); /* -4712...4741 */
    a0 += *--s; /* -4712...4996 */
    a0 += (a0>>15)&9097; /* 0...9096 */
    a1 = (s[1]<<8)+s[0]-a0;
    a1 = mullo(a1,17081);

    /* invalid inputs might need reduction mod 9097 */
    a1 -= 9097;
    a1 += (a1>>15)&9097;

    R6[2*i] = a0;
    R6[2*i+1] = a1;
  }
  
  /* R6 ------> R5: reconstruct mod 23*[1526]+[367] */
  
  a2 = a0 = R6[11];
  a0 = mulhi(a0,372)-mulhi(mullo(a0,-10994),1526); /* -763...856 */
  a0 += *--s; /* -763...1111 */
  a0 += (a0>>15)&1526; /* 0...1525 */
  a1 = (a2<<7)+((s[0]-a0)>>1);
  a1 = mullo(a1,-18381);

  /* invalid inputs might need reduction mod 367 */
  a1 -= 367;
  a1 += (a1>>15)&367;

  R5[22] = a0;
  R5[23] = a1;
  for (i = 10;i >= 0;--i) {
    a2 = a0 = R6[i];
    a0 = mulhi(a0,372)-mulhi(mullo(a0,-10994),1526); /* -763...856 */
    a0 += *--s; /* -763...1111 */
    a0 += (a0>>15)&1526; /* 0...1525 */
    a1 = (a2<<7)+((s[0]-a0)>>1);
    a1 = mullo(a1,-18381);

    /* invalid inputs might need reduction mod 1526 */
    a1 -= 1526;
    a1 += (a1>>15)&1526;

    R5[2*i] = a0;
    R5[2*i+1] = a1;
  }
  
  /* R5 ------> R4: reconstruct mod 47*[625]+[150] */
  
  a2 = a0 = R5[23];
  a0 = mulhi(a0,-284)-mulhi(mullo(a0,-26844),625); /* -384...312 */
  a0 += *--s; /* -384...567 */
  a0 += (a0>>15)&625; /* 0...624 */
  a1 = (a2<<8)+s[0]-a0;
  a1 = mullo(a1,32401);

  /* invalid inputs might need reduction mod 150 */
  a1 -= 150;
  a1 += (a1>>15)&150;

  R4[46] = a0;
  R4[47] = a1;
  for (i = 22;i >= 0;--i) {
    a2 = a0 = R5[i];
    a0 = mulhi(a0,-284)-mulhi(mullo(a0,-26844),625); /* -384...312 */
    a0 += *--s; /* -384...567 */
    a0 += (a0>>15)&625; /* 0...624 */
    a1 = (a2<<8)+s[0]-a0;
    a1 = mullo(a1,32401);

    /* invalid inputs might need reduction mod 625 */
    a1 -= 625;
    a1 += (a1>>15)&625;

    R4[2*i] = a0;
    R4[2*i+1] = a1;
  }
  
  /* R4 ------> R3: reconstruct mod 95*[6400]+[1531] */
  
  a2 = a0 = R4[47];
  a0 = mulhi(a0,2816)-mulhi(mullo(a0,-2621),6400); /* -3200...3904 */
  a0 += *--s; /* -3200...4159 */
  a0 = mulhi(a0,2816)-mulhi(mullo(a0,-2621),6400); /* -3338...3378 */
  a0 += *--s; /* -3338...3633 */
  a0 += (a0>>15)&6400; /* 0...6399 */
  a1 = (a2<<8)+s[1]+((s[0]-a0)>>8);
  a1 = mullo(a1,23593);

  /* invalid inputs might need reduction mod 1531 */
  a1 -= 1531;
  a1 += (a1>>15)&1531;

  R3[94] = a0;
  R3[95] = a1;
  for (i = 46;i >= 0;--i) {
    a2 = a0 = R4[i];
    a0 = mulhi(a0,2816)-mulhi(mullo(a0,-2621),6400); /* -3200...3904 */
    a0 += *--s; /* -3200...4159 */
    a0 = mulhi(a0,2816)-mulhi(mullo(a0,-2621),6400); /* -3338...3378 */
    a0 += *--s; /* -3338...3633 */
    a0 += (a0>>15)&6400; /* 0...6399 */
    a1 = (a2<<8)+s[1]+((s[0]-a0)>>8);
    a1 = mullo(a1,23593);

    /* invalid inputs might need reduction mod 6400 */
    a1 -= 6400;
    a1 += (a1>>15)&6400;

    R3[2*i] = a0;
    R3[2*i+1] = a1;
  }
  
  /* R3 ------> R2: reconstruct mod 190*[1280]+[1531] */
  
  R2[190] = R3[95];
  for (i = 94;i >= 0;--i) {
    a2 = a0 = R3[i];
    a0 = mulhi(a0,256)-mulhi(mullo(a0,-13107),1280); /* -640...704 */
    a0 += *--s; /* -640...959 */
    a0 += (a0>>15)&1280; /* 0...1279 */
    a1 = a2+((s[0]-a0)>>8);
    a1 = mullo(a1,-13107);

    /* invalid inputs might need reduction mod 1280 */
    a1 -= 1280;
    a1 += (a1>>15)&1280;

    R2[2*i] = a0;
    R2[2*i+1] = a1;
  }
  
  /* R2 ------> R1: reconstruct mod 380*[9157]+[1531] */
  
  R1[380] = R2[190];
  for (i = 189;i >= 0;--i) {
    a2 = a0 = R2[i];
    a0 = mulhi(a0,1592)-mulhi(mullo(a0,-1832),9157); /* -4579...4976 */
    a0 += *--s; /* -4579...5231 */
    a0 = mulhi(a0,1592)-mulhi(mullo(a0,-1832),9157); /* -4690...4705 */
    a0 += *--s; /* -4690...4960 */
    a0 += (a0>>15)&9157; /* 0...9156 */
    a1 = (s[1]<<8)+s[0]-a0;
    a1 = mullo(a1,25357);

    /* invalid inputs might need reduction mod 9157 */
    a1 -= 9157;
    a1 += (a1>>15)&9157;

    R1[2*i] = a0;
    R1[2*i+1] = a1;
  }
  
  /* R1 ------> R0: reconstruct mod 761*[1531] */
  
  R0[760] = 3*R1[380]-2295;
  for (i = 379;i >= 0;--i) {
    a2 = a0 = R1[i];
    a0 = mulhi(a0,518)-mulhi(mullo(a0,-10958),1531); /* -766...895 */
    a0 += *--s; /* -766...1150 */
    a0 += (a0>>15)&1531; /* 0...1530 */
    a1 = (a2<<8)+s[0]-a0;
    a1 = mullo(a1,15667);

    /* invalid inputs might need reduction mod 1531 */
    a1 -= 1531;
    a1 += (a1>>15)&1531;

    R0[2*i] = 3*a0-2295;
    R0[2*i+1] = 3*a1-2295;
  }
}
