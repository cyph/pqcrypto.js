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
  a1 = mulhi(a1,841)-mulhi(mullo(a1,-2695),6225);
  a1 += *--s; /* -3113...3370 */
  a1 += (a1>>15)&6225; /* 0...6224 */
  R10[0] = a1;
  
  /* R10 ------> R9: reconstruct mod 1*[5476]+[291] */
  
  a2 = a0 = R10[0];
  a0 = mulhi(a0,-1248)-mulhi(mullo(a0,-3064),5476); /* -3050...2738 */
  a0 += *--s; /* -3050...2993 */
  a0 += (a0>>15)&5476; /* 0...5475 */
  a1 = (a2<<6)+((s[0]-a0)>>2);
  a1 = mullo(a1,-3351);

  /* invalid inputs might need reduction mod 291 */
  a1 -= 291;
  a1 += (a1>>15)&291;

  R9[0] = a0;
  R9[1] = a1;
  
  /* R9 ------> R8: reconstruct mod 3*[74]+[1004] */
  
  a2 = a0 = R9[1];
  a0 = mulhi(a0,10)-mulhi(mullo(a0,-30111),74); /* -37...39 */
  a0 += *--s; /* -37...294 */
  a0 = mulhi(a0,-28)-mulhi(mullo(a0,-886),74); /* -38...37 */
  a0 += (a0>>15)&74; /* 0...73 */
  a1 = (a2<<7)+((s[0]-a0)>>1);
  a1 = mullo(a1,7085);

  /* invalid inputs might need reduction mod 1004 */
  a1 -= 1004;
  a1 += (a1>>15)&1004;

  R8[2] = a0;
  R8[3] = a1;
  for (i = 0;i >= 0;--i) {
    a2 = a0 = R9[i];
    a0 = mulhi(a0,-28)-mulhi(mullo(a0,-886),74); /* -44...37 */
    a0 += (a0>>15)&74; /* 0...73 */
    a1 = (a2-a0)>>1;
    a1 = mullo(a1,7085);

    /* invalid inputs might need reduction mod 74 */
    a1 -= 74;
    a1 += (a1>>15)&74;

    R8[2*i] = a0;
    R8[2*i+1] = a1;
  }
  
  /* R8 ------> R7: reconstruct mod 6*[2194]+[1004] */
  
  R7[6] = R8[3];
  for (i = 2;i >= 0;--i) {
    a2 = a0 = R8[i];
    a0 = mulhi(a0,-302)-mulhi(mullo(a0,-7647),2194); /* -1173...1097 */
    a0 += *--s; /* -1173...1352 */
    a0 = mulhi(a0,-302)-mulhi(mullo(a0,-7647),2194); /* -1104...1102 */
    a0 += *--s; /* -1104...1357 */
    a0 += (a0>>15)&2194; /* 0...2193 */
    a1 = (a2<<15)+(s[1]<<7)+((s[0]-a0)>>1);
    a1 = mullo(a1,11769);

    /* invalid inputs might need reduction mod 2194 */
    a1 -= 2194;
    a1 += (a1>>15)&2194;

    R7[2*i] = a0;
    R7[2*i+1] = a1;
  }
  
  /* R7 ------> R6: reconstruct mod 13*[11991]+[5483] */
  
  a2 = a0 = R7[6];
  a0 = mulhi(a0,1807)-mulhi(mullo(a0,-1399),11991); /* -5996...6447 */
  a0 += *--s; /* -5996...6702 */
  a0 = mulhi(a0,1807)-mulhi(mullo(a0,-1399),11991); /* -6161...6180 */
  a0 += *--s; /* -6161...6435 */
  a0 += (a0>>15)&11991; /* 0...11990 */
  a1 = (s[1]<<8)+s[0]-a0;
  a1 = mullo(a1,-23321);

  /* invalid inputs might need reduction mod 5483 */
  a1 -= 5483;
  a1 += (a1>>15)&5483;

  R6[12] = a0;
  R6[13] = a1;
  for (i = 5;i >= 0;--i) {
    a2 = a0 = R7[i];
    a0 = mulhi(a0,1807)-mulhi(mullo(a0,-1399),11991); /* -5996...6447 */
    a0 += *--s; /* -5996...6702 */
    a0 = mulhi(a0,1807)-mulhi(mullo(a0,-1399),11991); /* -6161...6180 */
    a0 += *--s; /* -6161...6435 */
    a0 += (a0>>15)&11991; /* 0...11990 */
    a1 = (s[1]<<8)+s[0]-a0;
    a1 = mullo(a1,-23321);

    /* invalid inputs might need reduction mod 11991 */
    a1 -= 11991;
    a1 += (a1>>15)&11991;

    R6[2*i] = a0;
    R6[2*i+1] = a1;
  }
  
  /* R6 ------> R5: reconstruct mod 26*[1752]+[5483] */
  
  R5[26] = R6[13];
  for (i = 12;i >= 0;--i) {
    a2 = a0 = R6[i];
    a0 = mulhi(a0,64)-mulhi(mullo(a0,-9576),1752); /* -876...892 */
    a0 += *--s; /* -876...1147 */
    a0 += (a0>>15)&1752; /* 0...1751 */
    a1 = (a2<<5)+((s[0]-a0)>>3);
    a1 = mullo(a1,-1197);

    /* invalid inputs might need reduction mod 1752 */
    a1 -= 1752;
    a1 += (a1>>15)&1752;

    R5[2*i] = a0;
    R5[2*i+1] = a1;
  }
  
  /* R5 ------> R4: reconstruct mod 53*[10713]+[131] */
  
  a2 = a0 = R5[26];
  a0 = mulhi(a0,658)-mulhi(mullo(a0,-1566),10713); /* -5357...5521 */
  a0 += *--s; /* -5357...5776 */
  a0 += (a0>>15)&10713; /* 0...10712 */
  a1 = (a2<<8)+s[0]-a0;
  a1 = mullo(a1,-14743);

  /* invalid inputs might need reduction mod 131 */
  a1 -= 131;
  a1 += (a1>>15)&131;

  R4[52] = a0;
  R4[53] = a1;
  for (i = 25;i >= 0;--i) {
    a2 = a0 = R5[i];
    a0 = mulhi(a0,658)-mulhi(mullo(a0,-1566),10713); /* -5357...5521 */
    a0 += *--s; /* -5357...5776 */
    a0 = mulhi(a0,658)-mulhi(mullo(a0,-1566),10713); /* -5411...5414 */
    a0 += *--s; /* -5411...5669 */
    a0 += (a0>>15)&10713; /* 0...10712 */
    a1 = (s[1]<<8)+s[0]-a0;
    a1 = mullo(a1,-14743);

    /* invalid inputs might need reduction mod 10713 */
    a1 -= 10713;
    a1 += (a1>>15)&10713;

    R4[2*i] = a0;
    R4[2*i+1] = a1;
  }
  
  /* R4 ------> R3: reconstruct mod 107*[1656]+[5167] */
  
  a2 = a0 = R4[53];
  a0 = mulhi(a0,280)-mulhi(mullo(a0,-10131),1656); /* -828...898 */
  a0 += *--s; /* -828...1153 */
  a0 = mulhi(a0,280)-mulhi(mullo(a0,-10131),1656); /* -832...832 */
  a0 += *--s; /* -832...1087 */
  a0 += (a0>>15)&1656; /* 0...1655 */
  a1 = (a2<<13)+(s[1]<<5)+((s[0]-a0)>>3);
  a1 = mullo(a1,1583);

  /* invalid inputs might need reduction mod 5167 */
  a1 -= 5167;
  a1 += (a1>>15)&5167;

  R3[106] = a0;
  R3[107] = a1;
  for (i = 52;i >= 0;--i) {
    a2 = a0 = R4[i];
    a0 = mulhi(a0,280)-mulhi(mullo(a0,-10131),1656); /* -828...898 */
    a0 += *--s; /* -828...1153 */
    a0 += (a0>>15)&1656; /* 0...1655 */
    a1 = (a2<<5)+((s[0]-a0)>>3);
    a1 = mullo(a1,1583);

    /* invalid inputs might need reduction mod 1656 */
    a1 -= 1656;
    a1 += (a1>>15)&1656;

    R3[2*i] = a0;
    R3[2*i+1] = a1;
  }
  
  /* R3 ------> R2: reconstruct mod 214*[651]+[5167] */
  
  R2[214] = R3[107];
  for (i = 106;i >= 0;--i) {
    a2 = a0 = R3[i];
    a0 = mulhi(a0,295)-mulhi(mullo(a0,-25771),651); /* -326...399 */
    a0 += *--s; /* -326...654 */
    a0 -= 651; /* -977..>3 */
    a0 += (a0>>15)&651; /* -326...650 */
    a0 += (a0>>15)&651; /* 0...650 */
    a1 = (a2<<8)+s[0]-a0;
    a1 = mullo(a1,-10973);

    /* invalid inputs might need reduction mod 651 */
    a1 -= 651;
    a1 += (a1>>15)&651;

    R2[2*i] = a0;
    R2[2*i+1] = a1;
  }
  
  /* R2 ------> R1: reconstruct mod 428*[408]+[5167] */
  
  R1[428] = R2[214];
  for (i = 213;i >= 0;--i) {
    a2 = a0 = R2[i];
    a0 = mulhi(a0,-152)-mulhi(mullo(a0,24415),408); /* -242...204 */
    a0 += *--s; /* -242...459 */
    a0 -= 408; /* -650..>51 */
    a0 += (a0>>15)&408; /* -242...407 */
    a0 += (a0>>15)&408; /* 0...407 */
    a1 = (a2<<5)+((s[0]-a0)>>3);
    a1 = mullo(a1,-1285);

    /* invalid inputs might need reduction mod 408 */
    a1 -= 408;
    a1 += (a1>>15)&408;

    R1[2*i] = a0;
    R1[2*i+1] = a1;
  }
  
  /* R1 ------> R0: reconstruct mod 857*[5167] */
  
  R0[856] = R1[428]-2583;
  for (i = 427;i >= 0;--i) {
    a2 = a0 = R1[i];
    a0 = mulhi(a0,-33)-mulhi(mullo(a0,-3247),5167); /* -2592...2583 */
    a0 += *--s; /* -2592...2838 */
    a0 = mulhi(a0,-33)-mulhi(mullo(a0,-3247),5167); /* -2585...2584 */
    a0 += *--s; /* -2585...2839 */
    a0 += (a0>>15)&5167; /* 0...5166 */
    a1 = (s[1]<<8)+s[0]-a0;
    a1 = mullo(a1,-19761);

    /* invalid inputs might need reduction mod 5167 */
    a1 -= 5167;
    a1 += (a1>>15)&5167;

    R0[2*i] = a0-2583;
    R0[2*i+1] = a1-2583;
  }
}
