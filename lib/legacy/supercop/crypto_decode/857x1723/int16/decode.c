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
  int16 R1[429],R2[215],R3[108],R4[54],R5[27],R6[14],R7[7],R8[4],R9[2],R10[1];
  long long i;
  int16 a0,a1,a2;
  
  s += crypto_decode_STRBYTES;
  a1 = 0;
  a1 += *--s; /* 0...255 */
  a1 -= 160; /* -160...95 */
  a1 += (a1>>15)&160; /* 0...159 */
  R10[0] = a1;
  
  /* R10 ------> R9: reconstruct mod 1*[743]+[14044] */
  
  a2 = a0 = R10[0];
  a0 = mulhi(a0,276)-mulhi(mullo(a0,-22580),743); /* -372...440 */
  a0 += *--s; /* -372...695 */
  a0 = mulhi(a0,276)-mulhi(mullo(a0,-22580),743); /* -374...374 */
  a0 += *--s; /* -374...629 */
  a0 += (a0>>15)&743; /* 0...742 */
  a1 = (s[1]<<8)+s[0]-a0;
  a1 = mullo(a1,-3881);

  /* invalid inputs might need reduction mod 14044 */
  a1 -= 14044;
  a1 += (a1>>15)&14044;

  R9[0] = a0;
  R9[1] = a1;
  
  /* R9 ------> R8: reconstruct mod 3*[436]+[8246] */
  
  a2 = a0 = R9[1];
  a0 = mulhi(a0,-64)-mulhi(mullo(a0,27056),436); /* -234...218 */
  a0 += *--s; /* -234...473 */
  a0 -= 436; /* -670..>37 */
  a0 += (a0>>15)&436; /* -234...435 */
  a0 += (a0>>15)&436; /* 0...435 */
  a1 = (a2<<6)+((s[0]-a0)>>2);
  a1 = mullo(a1,2405);

  /* invalid inputs might need reduction mod 8246 */
  a1 -= 8246;
  a1 += (a1>>15)&8246;

  R8[2] = a0;
  R8[3] = a1;
  for (i = 0;i >= 0;--i) {
    a2 = a0 = R9[i];
    a0 = mulhi(a0,-64)-mulhi(mullo(a0,27056),436); /* -234...218 */
    a0 += *--s; /* -234...473 */
    a0 -= 436; /* -670..>37 */
    a0 += (a0>>15)&436; /* -234...435 */
    a0 += (a0>>15)&436; /* 0...435 */
    a1 = (a2<<6)+((s[0]-a0)>>2);
    a1 = mullo(a1,2405);

    /* invalid inputs might need reduction mod 436 */
    a1 -= 436;
    a1 += (a1>>15)&436;

    R8[2*i] = a0;
    R8[2*i+1] = a1;
  }
  
  /* R8 ------> R7: reconstruct mod 6*[334]+[8246] */
  
  R7[6] = R8[3];
  for (i = 2;i >= 0;--i) {
    a2 = a0 = R8[i];
    a0 = mulhi(a0,62)-mulhi(mullo(a0,15305),334); /* -167...182 */
    a0 += *--s; /* -167...437 */
    a0 -= 334; /* -501..>103 */
    a0 += (a0>>15)&334; /* -167...333 */
    a0 += (a0>>15)&334; /* 0...333 */
    a1 = (a2<<7)+((s[0]-a0)>>1);
    a1 = mullo(a1,-22761);

    /* invalid inputs might need reduction mod 334 */
    a1 -= 334;
    a1 += (a1>>15)&334;

    R7[2*i] = a0;
    R7[2*i+1] = a1;
  }
  
  /* R7 ------> R6: reconstruct mod 13*[292]+[7229] */
  
  a2 = a0 = R7[6];
  a0 = mulhi(a0,64)-mulhi(mullo(a0,8080),292); /* -146...162 */
  a0 += *--s; /* -146...417 */
  a0 -= 292; /* -438..>125 */
  a0 += (a0>>15)&292; /* -146...291 */
  a0 += (a0>>15)&292; /* 0...291 */
  a1 = (a2<<6)+((s[0]-a0)>>2);
  a1 = mullo(a1,-3591);

  /* invalid inputs might need reduction mod 7229 */
  a1 -= 7229;
  a1 += (a1>>15)&7229;

  R6[12] = a0;
  R6[13] = a1;
  for (i = 5;i >= 0;--i) {
    a2 = a0 = R7[i];
    a0 = mulhi(a0,64)-mulhi(mullo(a0,8080),292); /* -146...162 */
    a0 += *--s; /* -146...417 */
    a0 -= 292; /* -438..>125 */
    a0 += (a0>>15)&292; /* -146...291 */
    a0 += (a0>>15)&292; /* 0...291 */
    a1 = (a2<<6)+((s[0]-a0)>>2);
    a1 = mullo(a1,-3591);

    /* invalid inputs might need reduction mod 292 */
    a1 -= 292;
    a1 += (a1>>15)&292;

    R6[2*i] = a0;
    R6[2*i+1] = a1;
  }
  
  /* R6 ------> R5: reconstruct mod 26*[273]+[7229] */
  
  R5[26] = R6[13];
  for (i = 12;i >= 0;--i) {
    a2 = a0 = R6[i];
    a0 = mulhi(a0,1)-mulhi(mullo(a0,4081),273); /* -137...136 */
    a0 += *--s; /* -137...391 */
    a0 -= 273; /* -410..>118 */
    a0 += (a0>>15)&273; /* -137...272 */
    a0 += (a0>>15)&273; /* 0...272 */
    a1 = (a2<<8)+s[0]-a0;
    a1 = mullo(a1,4081);

    /* invalid inputs might need reduction mod 273 */
    a1 -= 273;
    a1 += (a1>>15)&273;

    R5[2*i] = a0;
    R5[2*i+1] = a1;
  }
  
  /* R5 ------> R4: reconstruct mod 53*[4225]+[438] */
  
  a2 = a0 = R5[26];
  a0 = mulhi(a0,-259)-mulhi(mullo(a0,-3971),4225); /* -2178...2112 */
  a0 += *--s; /* -2178...2367 */
  a0 += (a0>>15)&4225; /* 0...4224 */
  a1 = (a2<<8)+s[0]-a0;
  a1 = mullo(a1,12161);

  /* invalid inputs might need reduction mod 438 */
  a1 -= 438;
  a1 += (a1>>15)&438;

  R4[52] = a0;
  R4[53] = a1;
  for (i = 25;i >= 0;--i) {
    a2 = a0 = R5[i];
    a0 = mulhi(a0,-259)-mulhi(mullo(a0,-3971),4225); /* -2178...2112 */
    a0 += *--s; /* -2178...2367 */
    a0 = mulhi(a0,-259)-mulhi(mullo(a0,-3971),4225); /* -2122...2121 */
    a0 += *--s; /* -2122...2376 */
    a0 += (a0>>15)&4225; /* 0...4224 */
    a1 = (s[1]<<8)+s[0]-a0;
    a1 = mullo(a1,12161);

    /* invalid inputs might need reduction mod 4225 */
    a1 -= 4225;
    a1 += (a1>>15)&4225;

    R4[2*i] = a0;
    R4[2*i+1] = a1;
  }
  
  /* R4 ------> R3: reconstruct mod 107*[65]+[1723] */
  
  a2 = a0 = R4[53];
  a0 = mulhi(a0,1)-mulhi(mullo(a0,4033),65); /* -33...32 */
  a0 += *--s; /* -33...287 */
  a0 = mulhi(a0,16)-mulhi(mullo(a0,-1008),65); /* -33...32 */
  a0 += (a0>>15)&65; /* 0...64 */
  a1 = (a2<<8)+s[0]-a0;
  a1 = mullo(a1,4033);

  /* invalid inputs might need reduction mod 1723 */
  a1 -= 1723;
  a1 += (a1>>15)&1723;

  R3[106] = a0;
  R3[107] = a1;
  for (i = 52;i >= 0;--i) {
    a2 = a0 = R4[i];
    a0 = mulhi(a0,16)-mulhi(mullo(a0,-1008),65); /* -33...36 */
    a0 += (a0>>15)&65; /* 0...64 */
    a1 = (a2-a0)>>0;
    a1 = mullo(a1,4033);

    /* invalid inputs might need reduction mod 65 */
    a1 -= 65;
    a1 += (a1>>15)&65;

    R3[2*i] = a0;
    R3[2*i+1] = a1;
  }
  
  /* R3 ------> R2: reconstruct mod 214*[2053]+[1723] */
  
  R2[214] = R3[107];
  for (i = 106;i >= 0;--i) {
    a2 = a0 = R3[i];
    a0 = mulhi(a0,100)-mulhi(mullo(a0,-8172),2053); /* -1027...1051 */
    a0 += *--s; /* -1027...1306 */
    a0 = mulhi(a0,100)-mulhi(mullo(a0,-8172),2053); /* -1029...1028 */
    a0 += *--s; /* -1029...1283 */
    a0 += (a0>>15)&2053; /* 0...2052 */
    a1 = (s[1]<<8)+s[0]-a0;
    a1 = mullo(a1,-31539);

    /* invalid inputs might need reduction mod 2053 */
    a1 -= 2053;
    a1 += (a1>>15)&2053;

    R2[2*i] = a0;
    R2[2*i+1] = a1;
  }
  
  /* R2 ------> R1: reconstruct mod 428*[11597]+[1723] */
  
  R1[428] = R2[214];
  for (i = 213;i >= 0;--i) {
    a2 = a0 = R2[i];
    a0 = mulhi(a0,-3643)-mulhi(mullo(a0,-1447),11597); /* -6710...5798 */
    a0 += *--s; /* -6710...6053 */
    a0 = mulhi(a0,-3643)-mulhi(mullo(a0,-1447),11597); /* -6135...6171 */
    a0 += *--s; /* -6135...6426 */
    a0 += (a0>>15)&11597; /* 0...11596 */
    a1 = (s[1]<<8)+s[0]-a0;
    a1 = mullo(a1,-11387);

    /* invalid inputs might need reduction mod 11597 */
    a1 -= 11597;
    a1 += (a1>>15)&11597;

    R1[2*i] = a0;
    R1[2*i+1] = a1;
  }
  
  /* R1 ------> R0: reconstruct mod 857*[1723] */
  
  R0[856] = 3*R1[428]-2583;
  for (i = 427;i >= 0;--i) {
    a2 = a0 = R1[i];
    a0 = mulhi(a0,365)-mulhi(mullo(a0,-9737),1723); /* -862...952 */
    a0 += *--s; /* -862...1207 */
    a0 += (a0>>15)&1723; /* 0...1722 */
    a1 = (a2<<8)+s[0]-a0;
    a1 = mullo(a1,20083);

    /* invalid inputs might need reduction mod 1723 */
    a1 -= 1723;
    a1 += (a1>>15)&1723;

    R0[2*i] = 3*a0-2583;
    R0[2*i+1] = 3*a1-2583;
  }
}
