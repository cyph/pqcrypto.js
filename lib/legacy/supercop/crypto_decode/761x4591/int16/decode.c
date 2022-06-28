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
  a1 = mulhi(a1,-656)-mulhi(mullo(a1,-10434),1608);
  a1 += *--s; /* -804...1056 */
  a1 += (a1>>15)&1608; /* 0...1607 */
  R10[0] = a1;
  
  /* R10 ------> R9: reconstruct mod 1*[9470]+[11127] */
  
  a2 = a0 = R10[0];
  a0 = mulhi(a0,-3624)-mulhi(mullo(a0,-1772),9470); /* -5641...4735 */
  a0 += *--s; /* -5641...4990 */
  a0 = mulhi(a0,-3624)-mulhi(mullo(a0,-1772),9470); /* -5011...5046 */
  a0 += *--s; /* -5011...5301 */
  a0 += (a0>>15)&9470; /* 0...9469 */
  a1 = (a2<<15)+(s[1]<<7)+((s[0]-a0)>>1);
  a1 = mullo(a1,-21121);

  /* invalid inputs might need reduction mod 11127 */
  a1 -= 11127;
  a1 += (a1>>15)&11127;

  R9[0] = a0;
  R9[1] = a1;
  
  /* R9 ------> R8: reconstruct mod 2*[1557]+[11127] */
  
  R8[2] = R9[1];
  for (i = 0;i >= 0;--i) {
    a2 = a0 = R9[i];
    a0 = mulhi(a0,541)-mulhi(mullo(a0,-10775),1557); /* -779...913 */
    a0 += *--s; /* -779...1168 */
    a0 += (a0>>15)&1557; /* 0...1556 */
    a1 = (a2<<8)+s[0]-a0;
    a1 = mullo(a1,-26307);

    /* invalid inputs might need reduction mod 1557 */
    a1 -= 1557;
    a1 += (a1>>15)&1557;

    R8[2*i] = a0;
    R8[2*i+1] = a1;
  }
  
  /* R8 ------> R7: reconstruct mod 5*[10101]+[282] */
  
  a2 = a0 = R8[2];
  a0 = mulhi(a0,-545)-mulhi(mullo(a0,-1661),10101); /* -5187...5050 */
  a0 += *--s; /* -5187...5305 */
  a0 += (a0>>15)&10101; /* 0...10100 */
  a1 = (a2<<8)+s[0]-a0;
  a1 = mullo(a1,12509);

  /* invalid inputs might need reduction mod 282 */
  a1 -= 282;
  a1 += (a1>>15)&282;

  R7[4] = a0;
  R7[5] = a1;
  for (i = 1;i >= 0;--i) {
    a2 = a0 = R8[i];
    a0 = mulhi(a0,-545)-mulhi(mullo(a0,-1661),10101); /* -5187...5050 */
    a0 += *--s; /* -5187...5305 */
    a0 = mulhi(a0,-545)-mulhi(mullo(a0,-1661),10101); /* -5095...5093 */
    a0 += *--s; /* -5095...5348 */
    a0 += (a0>>15)&10101; /* 0...10100 */
    a1 = (s[1]<<8)+s[0]-a0;
    a1 = mullo(a1,12509);

    /* invalid inputs might need reduction mod 10101 */
    a1 -= 10101;
    a1 += (a1>>15)&10101;

    R7[2*i] = a0;
    R7[2*i+1] = a1;
  }
  
  /* R7 ------> R6: reconstruct mod 11*[1608]+[11468] */
  
  a2 = a0 = R7[5];
  a0 = mulhi(a0,-656)-mulhi(mullo(a0,-10434),1608); /* -968...804 */
  a0 += *--s; /* -968...1059 */
  a0 = mulhi(a0,-656)-mulhi(mullo(a0,-10434),1608); /* -815...813 */
  a0 += *--s; /* -815...1068 */
  a0 += (a0>>15)&1608; /* 0...1607 */
  a1 = (a2<<13)+(s[1]<<5)+((s[0]-a0)>>3);
  a1 = mullo(a1,6521);

  /* invalid inputs might need reduction mod 11468 */
  a1 -= 11468;
  a1 += (a1>>15)&11468;

  R6[10] = a0;
  R6[11] = a1;
  for (i = 4;i >= 0;--i) {
    a2 = a0 = R7[i];
    a0 = mulhi(a0,-656)-mulhi(mullo(a0,-10434),1608); /* -968...804 */
    a0 += *--s; /* -968...1059 */
    a0 += (a0>>15)&1608; /* 0...1607 */
    a1 = (a2<<5)+((s[0]-a0)>>3);
    a1 = mullo(a1,6521);

    /* invalid inputs might need reduction mod 1608 */
    a1 -= 1608;
    a1 += (a1>>15)&1608;

    R6[2*i] = a0;
    R6[2*i+1] = a1;
  }
  
  /* R6 ------> R5: reconstruct mod 23*[10265]+[286] */
  
  a2 = a0 = R6[11];
  a0 = mulhi(a0,4206)-mulhi(mullo(a0,-1634),10265); /* -5133...6184 */
  a0 += *--s; /* -5133...6439 */
  a0 += (a0>>15)&10265; /* 0...10264 */
  a1 = (a2<<8)+s[0]-a0;
  a1 = mullo(a1,-19415);

  /* invalid inputs might need reduction mod 286 */
  a1 -= 286;
  a1 += (a1>>15)&286;

  R5[22] = a0;
  R5[23] = a1;
  for (i = 10;i >= 0;--i) {
    a2 = a0 = R6[i];
    a0 = mulhi(a0,4206)-mulhi(mullo(a0,-1634),10265); /* -5133...6184 */
    a0 += *--s; /* -5133...6439 */
    a0 = mulhi(a0,4206)-mulhi(mullo(a0,-1634),10265); /* -5462...5545 */
    a0 += *--s; /* -5462...5800 */
    a0 += (a0>>15)&10265; /* 0...10264 */
    a1 = (s[1]<<8)+s[0]-a0;
    a1 = mullo(a1,-19415);

    /* invalid inputs might need reduction mod 10265 */
    a1 -= 10265;
    a1 += (a1>>15)&10265;

    R5[2*i] = a0;
    R5[2*i+1] = a1;
  }
  
  /* R5 ------> R4: reconstruct mod 47*[1621]+[11550] */
  
  a2 = a0 = R5[23];
  a0 = mulhi(a0,-134)-mulhi(mullo(a0,-10350),1621); /* -844...810 */
  a0 += *--s; /* -844...1065 */
  a0 = mulhi(a0,-134)-mulhi(mullo(a0,-10350),1621); /* -813...812 */
  a0 += *--s; /* -813...1067 */
  a0 += (a0>>15)&1621; /* 0...1620 */
  a1 = (s[1]<<8)+s[0]-a0;
  a1 = mullo(a1,-14595);

  /* invalid inputs might need reduction mod 11550 */
  a1 -= 11550;
  a1 += (a1>>15)&11550;

  R4[46] = a0;
  R4[47] = a1;
  for (i = 22;i >= 0;--i) {
    a2 = a0 = R5[i];
    a0 = mulhi(a0,-134)-mulhi(mullo(a0,-10350),1621); /* -844...810 */
    a0 += *--s; /* -844...1065 */
    a0 += (a0>>15)&1621; /* 0...1620 */
    a1 = (a2<<8)+s[0]-a0;
    a1 = mullo(a1,-14595);

    /* invalid inputs might need reduction mod 1621 */
    a1 -= 1621;
    a1 += (a1>>15)&1621;

    R4[2*i] = a0;
    R4[2*i+1] = a1;
  }
  
  /* R4 ------> R3: reconstruct mod 95*[644]+[4591] */
  
  a2 = a0 = R4[47];
  a0 = mulhi(a0,-272)-mulhi(mullo(a0,-26052),644); /* -390...322 */
  a0 += *--s; /* -390...577 */
  a0 += (a0>>15)&644; /* 0...643 */
  a1 = (a2<<6)+((s[0]-a0)>>2);
  a1 = mullo(a1,-7327);

  /* invalid inputs might need reduction mod 4591 */
  a1 -= 4591;
  a1 += (a1>>15)&4591;

  R3[94] = a0;
  R3[95] = a1;
  for (i = 46;i >= 0;--i) {
    a2 = a0 = R4[i];
    a0 = mulhi(a0,-272)-mulhi(mullo(a0,-26052),644); /* -390...322 */
    a0 += *--s; /* -390...577 */
    a0 += (a0>>15)&644; /* 0...643 */
    a1 = (a2<<6)+((s[0]-a0)>>2);
    a1 = mullo(a1,-7327);

    /* invalid inputs might need reduction mod 644 */
    a1 -= 644;
    a1 += (a1>>15)&644;

    R3[2*i] = a0;
    R3[2*i+1] = a1;
  }
  
  /* R3 ------> R2: reconstruct mod 190*[406]+[4591] */
  
  R2[190] = R3[95];
  for (i = 94;i >= 0;--i) {
    a2 = a0 = R3[i];
    a0 = mulhi(a0,78)-mulhi(mullo(a0,24213),406); /* -203...222 */
    a0 += *--s; /* -203...477 */
    a0 -= 406; /* -609..>71 */
    a0 += (a0>>15)&406; /* -203...405 */
    a0 += (a0>>15)&406; /* 0...405 */
    a1 = (a2<<7)+((s[0]-a0)>>1);
    a1 = mullo(a1,25827);

    /* invalid inputs might need reduction mod 406 */
    a1 -= 406;
    a1 += (a1>>15)&406;

    R2[2*i] = a0;
    R2[2*i+1] = a1;
  }
  
  /* R2 ------> R1: reconstruct mod 380*[322]+[4591] */
  
  R1[380] = R2[190];
  for (i = 189;i >= 0;--i) {
    a2 = a0 = R2[i];
    a0 = mulhi(a0,50)-mulhi(mullo(a0,13433),322); /* -161...173 */
    a0 += *--s; /* -161...428 */
    a0 -= 322; /* -483..>106 */
    a0 += (a0>>15)&322; /* -161...321 */
    a0 += (a0>>15)&322; /* 0...321 */
    a1 = (a2<<7)+((s[0]-a0)>>1);
    a1 = mullo(a1,-7327);

    /* invalid inputs might need reduction mod 322 */
    a1 -= 322;
    a1 += (a1>>15)&322;

    R1[2*i] = a0;
    R1[2*i+1] = a1;
  }
  
  /* R1 ------> R0: reconstruct mod 761*[4591] */
  
  R0[760] = R1[380]-2295;
  for (i = 379;i >= 0;--i) {
    a2 = a0 = R1[i];
    a0 = mulhi(a0,1702)-mulhi(mullo(a0,-3654),4591); /* -2296...2721 */
    a0 += *--s; /* -2296...2976 */
    a0 = mulhi(a0,1702)-mulhi(mullo(a0,-3654),4591); /* -2356...2372 */
    a0 += *--s; /* -2356...2627 */
    a0 += (a0>>15)&4591; /* 0...4590 */
    a1 = (s[1]<<8)+s[0]-a0;
    a1 = mullo(a1,15631);

    /* invalid inputs might need reduction mod 4591 */
    a1 -= 4591;
    a1 += (a1>>15)&4591;

    R0[2*i] = a0-2295;
    R0[2*i+1] = a1-2295;
  }
}
