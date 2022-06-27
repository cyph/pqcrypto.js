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
  lo = mullo(a1,-10434);
  a1 = mulhi(a1,-656)-mulhi(lo,1608);
  a1 += *--s; /* -804...1056 */
  a1 += (a1>>15)&1608; /* 0...1607 */
  R[0] = a1;
  
  /* reconstruct mod 1*[9470]+[11127] */
  
  ri = R[0];
  s1 = *--s;
  s0 = *--s;
  lo = mullo(ri,-1772);
  a0 = mulhi(ri,-3624)-mulhi(lo,9470); /* -5641...4735 */
  a0 += s1; /* -5641...4990 */
  lo = mullo(a0,-1772);
  a0 = mulhi(a0,-3624)-mulhi(lo,9470); /* -5011...5046 */
  a0 += s0; /* -5011...5301 */
  a0 += (a0>>15)&9470; /* 0...9469 */
  a1 = (ri<<15)+(s1<<7)+((s0-a0)>>1);
  a1 = mullo(a1,-21121);

  /* invalid inputs might need reduction mod 11127 */
  a1 -= 11127;
  a1 += (a1>>15)&11127;

  R[0] = a0;
  R[1] = a1;
  
  /* reconstruct mod 2*[1557]+[11127] */
  
  R[2] = R[1];
  for (i = 0;i >= 0;--i) {
    ri = R[i];
    s0 = *--s;
    lo = mullo(ri,-10775);
    a0 = mulhi(ri,541)-mulhi(lo,1557); /* -779...913 */
    a0 += s0; /* -779...1168 */
    a0 += (a0>>15)&1557; /* 0...1556 */
    a1 = (ri<<8)+s0-a0;
    a1 = mullo(a1,-26307);

    /* invalid inputs might need reduction mod 1557 */
    a1 -= 1557;
    a1 += (a1>>15)&1557;

    R[2*i] = a0;
    R[2*i+1] = a1;
  }
  
  /* reconstruct mod 5*[10101]+[282] */
  
  ri = R[2];
  s0 = *--s;
  lo = mullo(ri,-1661);
  a0 = mulhi(ri,-545)-mulhi(lo,10101); /* -5187...5050 */
  a0 += s0; /* -5187...5305 */
  a0 += (a0>>15)&10101; /* 0...10100 */
  a1 = (ri<<8)+s0-a0;
  a1 = mullo(a1,12509);

  /* invalid inputs might need reduction mod 282 */
  a1 -= 282;
  a1 += (a1>>15)&282;

  R[4] = a0;
  R[5] = a1;
  for (i = 1;i >= 0;--i) {
    ri = R[i];
    s1 = *--s;
    s0 = *--s;
    lo = mullo(ri,-1661);
    a0 = mulhi(ri,-545)-mulhi(lo,10101); /* -5187...5050 */
    a0 += s1; /* -5187...5305 */
    lo = mullo(a0,-1661);
    a0 = mulhi(a0,-545)-mulhi(lo,10101); /* -5095...5093 */
    a0 += s0; /* -5095...5348 */
    a0 += (a0>>15)&10101; /* 0...10100 */
    a1 = (s1<<8)+s0-a0;
    a1 = mullo(a1,12509);

    /* invalid inputs might need reduction mod 10101 */
    a1 -= 10101;
    a1 += (a1>>15)&10101;

    R[2*i] = a0;
    R[2*i+1] = a1;
  }
  
  /* reconstruct mod 11*[1608]+[11468] */
  
  ri = R[5];
  s1 = *--s;
  s0 = *--s;
  lo = mullo(ri,-10434);
  a0 = mulhi(ri,-656)-mulhi(lo,1608); /* -968...804 */
  a0 += s1; /* -968...1059 */
  lo = mullo(a0,-10434);
  a0 = mulhi(a0,-656)-mulhi(lo,1608); /* -815...813 */
  a0 += s0; /* -815...1068 */
  a0 += (a0>>15)&1608; /* 0...1607 */
  a1 = (ri<<13)+(s1<<5)+((s0-a0)>>3);
  a1 = mullo(a1,6521);

  /* invalid inputs might need reduction mod 11468 */
  a1 -= 11468;
  a1 += (a1>>15)&11468;

  R[10] = a0;
  R[11] = a1;
  for (i = 4;i >= 0;--i) {
    ri = R[i];
    s0 = *--s;
    lo = mullo(ri,-10434);
    a0 = mulhi(ri,-656)-mulhi(lo,1608); /* -968...804 */
    a0 += s0; /* -968...1059 */
    a0 += (a0>>15)&1608; /* 0...1607 */
    a1 = (ri<<5)+((s0-a0)>>3);
    a1 = mullo(a1,6521);

    /* invalid inputs might need reduction mod 1608 */
    a1 -= 1608;
    a1 += (a1>>15)&1608;

    R[2*i] = a0;
    R[2*i+1] = a1;
  }
  
  /* reconstruct mod 23*[10265]+[286] */
  
  ri = R[11];
  s0 = *--s;
  lo = mullo(ri,-1634);
  a0 = mulhi(ri,4206)-mulhi(lo,10265); /* -5133...6184 */
  a0 += s0; /* -5133...6439 */
  a0 += (a0>>15)&10265; /* 0...10264 */
  a1 = (ri<<8)+s0-a0;
  a1 = mullo(a1,-19415);

  /* invalid inputs might need reduction mod 286 */
  a1 -= 286;
  a1 += (a1>>15)&286;

  R[22] = a0;
  R[23] = a1;
  for (i = 10;i >= 0;--i) {
    ri = R[i];
    s1 = *--s;
    s0 = *--s;
    lo = mullo(ri,-1634);
    a0 = mulhi(ri,4206)-mulhi(lo,10265); /* -5133...6184 */
    a0 += s1; /* -5133...6439 */
    lo = mullo(a0,-1634);
    a0 = mulhi(a0,4206)-mulhi(lo,10265); /* -5462...5545 */
    a0 += s0; /* -5462...5800 */
    a0 += (a0>>15)&10265; /* 0...10264 */
    a1 = (s1<<8)+s0-a0;
    a1 = mullo(a1,-19415);

    /* invalid inputs might need reduction mod 10265 */
    a1 -= 10265;
    a1 += (a1>>15)&10265;

    R[2*i] = a0;
    R[2*i+1] = a1;
  }
  
  /* reconstruct mod 47*[1621]+[11550] */
  
  ri = R[23];
  s1 = *--s;
  s0 = *--s;
  lo = mullo(ri,-10350);
  a0 = mulhi(ri,-134)-mulhi(lo,1621); /* -844...810 */
  a0 += s1; /* -844...1065 */
  lo = mullo(a0,-10350);
  a0 = mulhi(a0,-134)-mulhi(lo,1621); /* -813...812 */
  a0 += s0; /* -813...1067 */
  a0 += (a0>>15)&1621; /* 0...1620 */
  a1 = (s1<<8)+s0-a0;
  a1 = mullo(a1,-14595);

  /* invalid inputs might need reduction mod 11550 */
  a1 -= 11550;
  a1 += (a1>>15)&11550;

  R[46] = a0;
  R[47] = a1;
  for (i = 22;i >= 0;--i) {
    ri = R[i];
    s0 = *--s;
    lo = mullo(ri,-10350);
    a0 = mulhi(ri,-134)-mulhi(lo,1621); /* -844...810 */
    a0 += s0; /* -844...1065 */
    a0 += (a0>>15)&1621; /* 0...1620 */
    a1 = (ri<<8)+s0-a0;
    a1 = mullo(a1,-14595);

    /* invalid inputs might need reduction mod 1621 */
    a1 -= 1621;
    a1 += (a1>>15)&1621;

    R[2*i] = a0;
    R[2*i+1] = a1;
  }
  
  /* reconstruct mod 95*[644]+[4591] */
  
  ri = R[47];
  s0 = *--s;
  lo = mullo(ri,-26052);
  a0 = mulhi(ri,-272)-mulhi(lo,644); /* -390...322 */
  a0 += s0; /* -390...577 */
  a0 += (a0>>15)&644; /* 0...643 */
  a1 = (ri<<6)+((s0-a0)>>2);
  a1 = mullo(a1,-7327);

  /* invalid inputs might need reduction mod 4591 */
  a1 -= 4591;
  a1 += (a1>>15)&4591;

  R[94] = a0;
  R[95] = a1;
  for (i = 46;i >= 0;--i) {
    ri = R[i];
    s0 = *--s;
    lo = mullo(ri,-26052);
    a0 = mulhi(ri,-272)-mulhi(lo,644); /* -390...322 */
    a0 += s0; /* -390...577 */
    a0 += (a0>>15)&644; /* 0...643 */
    a1 = (ri<<6)+((s0-a0)>>2);
    a1 = mullo(a1,-7327);

    /* invalid inputs might need reduction mod 644 */
    a1 -= 644;
    a1 += (a1>>15)&644;

    R[2*i] = a0;
    R[2*i+1] = a1;
  }
  
  /* reconstruct mod 190*[406]+[4591] */
  
  R[190] = R[95];
  for (i = 94;i >= 0;--i) {
    ri = R[i];
    s0 = *--s;
    lo = mullo(ri,24213);
    a0 = mulhi(ri,78)-mulhi(lo,406); /* -203...222 */
    a0 += s0; /* -203...477 */
    a0 -= 406; /* -609..>71 */
    a0 += (a0>>15)&406; /* -203...405 */
    a0 += (a0>>15)&406; /* 0...405 */
    a1 = (ri<<7)+((s0-a0)>>1);
    a1 = mullo(a1,25827);

    /* invalid inputs might need reduction mod 406 */
    a1 -= 406;
    a1 += (a1>>15)&406;

    R[2*i] = a0;
    R[2*i+1] = a1;
  }
  
  /* reconstruct mod 380*[322]+[4591] */
  
  R[380] = R[190];
  for (i = 189;i >= 0;--i) {
    ri = R[i];
    s0 = *--s;
    lo = mullo(ri,13433);
    a0 = mulhi(ri,50)-mulhi(lo,322); /* -161...173 */
    a0 += s0; /* -161...428 */
    a0 -= 322; /* -483..>106 */
    a0 += (a0>>15)&322; /* -161...321 */
    a0 += (a0>>15)&322; /* 0...321 */
    a1 = (ri<<7)+((s0-a0)>>1);
    a1 = mullo(a1,-7327);

    /* invalid inputs might need reduction mod 322 */
    a1 -= 322;
    a1 += (a1>>15)&322;

    R[2*i] = a0;
    R[2*i+1] = a1;
  }
  
  /* reconstruct mod 761*[4591] */
  
  R[760] = R[380]-2295;
  for (i = 379;i >= 0;--i) {
    ri = R[i];
    s1 = *--s;
    s0 = *--s;
    lo = mullo(ri,-3654);
    a0 = mulhi(ri,1702)-mulhi(lo,4591); /* -2296...2721 */
    a0 += s1; /* -2296...2976 */
    lo = mullo(a0,-3654);
    a0 = mulhi(a0,1702)-mulhi(lo,4591); /* -2356...2372 */
    a0 += s0; /* -2356...2627 */
    a0 += (a0>>15)&4591; /* 0...4590 */
    a1 = (s1<<8)+s0-a0;
    a1 = mullo(a1,15631);

    /* invalid inputs might need reduction mod 4591 */
    a1 -= 4591;
    a1 += (a1>>15)&4591;

    R[2*i] = a0-2295;
    R[2*i+1] = a1-2295;
  }
}
