#include <immintrin.h>

#define _P1 7681
#define _P2 10753
#define _P3 12289
#define _QQ 4591
#define _P12 82593793LL

#define vP1_16 _mm256_set1_epi16(_P1)
#define vP2_16 _mm256_set1_epi16(_P2)
#define vP3_16 _mm256_set1_epi16(_P3)
#define vQQ_16 _mm256_set1_epi16(_QQ)
#define vP1h_16 _mm256_set1_epi16(_P1>>1)
#define vP2h_16 _mm256_set1_epi16(_P2>>1)
#define vP3h_16 _mm256_set1_epi16(_P3>>1)

#define vP1i_16 _mm256_set1_epi16(-7679)   // _P1^{-1} mod R
#define vP2i_16 _mm256_set1_epi16(-10751)  // _P2^{-1} mod R
#define vP3i_16 _mm256_set1_epi16(-12287)  // _P3^{-1} mod R
#define vQQi_16 _mm256_set1_epi16(15631)   // _QQ^{-1} mod R
					   
#define vP12R_16 _mm256_set1_epi16(598)    // _P1*_P2*R mod _QQ
#define vP13R_16 _mm256_set1_epi16(1735)   // _P1*_P3*R mod _QQ 
#define vP23R_16 _mm256_set1_epi16(-2290)  // _P2*_P3*R mod _QQ 
#define vP123_16 _mm256_set1_epi16(-2202)  // _P1*_P2*_P3 mod _QQ

#define vP12iR_16 _mm256_set1_epi16(5348) // R/(_P1*_P2) mod _P3
#define vP13iR_16 _mm256_set1_epi16(-3435)// R/(_P1*_P3) mod _P2
#define vP23iR_16 _mm256_set1_epi16(-889) // R/(_P2*_P3) mod _P1 

#define shift_p1  8 // floor(log_2 p1) - 4
#define shift_p2  9 // floor(log_2 p2) - 4
#define shift_p3  9 // floor(log_2 p3) - 4

#define vP1i2p27 _mm256_set1_epi16(17474)  // round(2^27/_P1)
#define vP2i2p28 _mm256_set1_epi16(24964)  // round(2^28/_P2)
#define vP3i2p28 _mm256_set1_epi16(21844)  // round(2^28/_P3)
  
#define vP1R_QQ_16 _mm256_set1_epi16(1821)  // _P1*R mod _QQ
#define vP1R_P3_16 _mm256_set1_epi16(-2)    // _P1*R mod _P3
#define v2QQi_R_16 _mm256_set1_epi16(7)    
#define vQQi2p27_16 _mm256_set1_epi16(29234)// floor(2^27/_QQ) 
#define v2p10_16 _mm256_set1_epi16(1024)
#define v6_16 _mm256_set1_epi16(6)

// _C2 = 1/_P1 mod _P2, _C3 = 1/(_P1*_P2) mod _P3
// #define _C2 -5373
// #define _C3 -4075

#define vC2R_16  _mm256_set1_epi16(3563) // _C2*R mod _P2
#define vC3R_16  _mm256_set1_epi16(5348) // _C3*R mod _P3
#define vC2R2_16 _mm256_set1_epi16(3373) // _C2*R^2 mod _P2
#define vC3R2_16 _mm256_set1_epi16(4248) // _C3*R^2 mod _P3
// #define vC3R3_16 _mm256_set1_epi16(1922) // _C3*R^3 mod _P3
// #define vP12_16 _mm256_set1_epi16(1703)  // _P1 * _P2 mod _QQ

// #define v7_16 _mm256_set1_epi16(7)  
// #define v4591_16 _mm256_set1_epi16(4591)  // M
// #define v15631_16 _mm256_set1_epi16(15631) // M^{-1} mod R

#define cofac1  -168615255292LL
#define cofac2   505184961768LL
#define cofac3  -336569706475LL

#define cofac1x  41270014LL
#define cofac2x -41270013LL


// static inline __m256i montproduct_qq(__m256i x,__m256i y)
// // note : montproduct with v4158_16 to multiply by R=2^16
// {
//   __m256i lo,hi,d,e;
// 
//   lo = _mm256_mullo_epi16(x,y);
//   hi = _mm256_mulhi_epi16(x,y);
//   d = _mm256_mullo_epi16(lo,v15631_16);
//   e = _mm256_mulhi_epi16(d,v4591_16);
//   return _mm256_sub_epi16(hi,e);
// }

static inline __m256i c_adjust(__m256i x, __m256i p, __m256i ph) {
  __m256i x_abs = _mm256_sign_epi16(x,x);
  __m256i p_pm = _mm256_sign_epi16(p,x);
  __m256i mask = _mm256_cmpgt_epi16(x_abs,ph);
  __m256i diff = _mm256_and_si256(p_pm,mask);
  __m256i res = _mm256_sub_epi16(x,diff);
  return(res);
}

static inline __m256i barrett_fake_qq(__m256i x) {
  __m256i d = _mm256_mulhrs_epi16 (x, v2QQi_R_16);
  __m256i e = _mm256_mullo_epi16 (d, vQQ_16);
  return _mm256_sub_epi16(x,e);
}

static inline __m256i barrett_real_qq(__m256i x) {
  __m256i d = _mm256_mulhi_epi16 (x, vQQi2p27_16);
  __m256i t = _mm256_srai_epi16(_mm256_add_epi16(d,v2p10_16),11);
  __m256i e = _mm256_mullo_epi16 (t, vQQ_16);
  return _mm256_sub_epi16(x,e);
}

static inline __m256i gen_montp(__m256i x, __m256i y, __m256i m, __m256i m1) {
  __m256i lo,hi,d,e;

  lo = _mm256_mullo_epi16(x,y);
  hi = _mm256_mulhi_epi16(x,y);
  d = _mm256_mullo_epi16(lo,m1);
  e = _mm256_mulhi_epi16(d,m);
  return _mm256_sub_epi16(hi,e);
}

static inline
void print16 (short *b) {
  int i;
  printf("(%-5d) + (%-5d) * x",b[0],b[1]);
  for (i=2; i<16; i++) {
    if ((i&3)==0) printf ("\n ");
    printf ("+(%-5d) * x^%d",b[i],i);
  }
  printf("\n");
}

static inline
short crt3modq_slow(short u1, short u2, short u3) {
  long long u, y1, y2, y3, P;
  short x;
  P = (long long) _P1 * (long long) _P2 * (long long) _P3;
  y1 = ((long long) u1 * cofac1) % P;
  y2 = ((long long) u2 * cofac2) % P;
  y3 = ((long long) u3 * cofac3) % P;
  u = (y1+y2+y3) % P;
  if (u > (P>>1)) u -= P;
  if (u < -(P>>1)) u += P;
  x = (short) (u % ((long long) _QQ));
  if (x > (_QQ>>1)) return (x-_QQ);
  else if (x < - (_QQ>>1)) return (x+_QQ);
  else return (x);
}

static inline
short crt2modq_slow(short u1, short u2) {
  long long u, y1, y2, P;
  short x;
  P = _P12;
  y1 = ((long long) u1 * cofac1x) % P;
  y2 = ((long long) u2 * cofac2x) % P;
  u = (y1+y2) % P;
  if (u > (P>>1)) u -= P;
  if (u < -(P>>1)) u += P;
  x = (short) (u % ((long long) _QQ));
  if (x > (_QQ>>1)) return (x-_QQ);
  else if (x < - (_QQ>>1)) return (x+_QQ);
  else return (x);
}


static int crt3__inside(short u1, short u2, short u3) {
  long long u, y1, y2, y3, P;
  P = (long long) _P1 * (long long) _P2 * (long long) _P3;
  y1 = ((long long) u1 * cofac1) % P;
  y2 = ((long long) u2 * cofac2) % P;
  y3 = ((long long) u3 * cofac3) % P;
  u = (y1+y2+y3) % P;
  if (u > (P>>1)) u -= P;
  if (u < -(P>>1)) u += P;
  if (u < 0) u = -u;
  return((4 * u < P));
}

static inline __m256i crt2modq(__m256i u1, __m256i u2) {
  __m256i y1 = u1;

  __m256i y2 = gen_montp(_mm256_sub_epi16(u2,u1),vC2R_16,vP2_16,vP2i_16);
  y2 = c_adjust(y2,vP2_16,vP2h_16);
  __m256i temp3 = gen_montp(y2,vP1R_QQ_16,vQQ_16,vQQi_16);
  
  return(barrett_real_qq(_mm256_add_epi16(y1,temp3)));
}

static inline __m256i crt3modq(__m256i u1, __m256i u2, __m256i u3) {
  __m256i y1 = u1;

  __m256i y2 = gen_montp(_mm256_sub_epi16(u2,u1),vC2R_16,vP2_16,vP2i_16);
  y2 = c_adjust(y2,vP2_16,vP2h_16);
  __m256i temp3 = gen_montp(y2,vP1R_QQ_16,vQQ_16,vQQi_16);
  
  __m256i temp = gen_montp(y2,vP1R_P3_16,vP3_16,vP3i_16);
  __m256i temp1 = _mm256_sub_epi16(u3,_mm256_add_epi16(u1,temp));
  __m256i y3 = gen_montp(temp1,vC3R_16,vP3_16,vP3i_16);
  y3 = c_adjust(y3,vP3_16,vP3h_16);
  __m256i temp2 = gen_montp(y3,vP12R_16,vQQ_16,vQQi_16);

  return(barrett_real_qq(_mm256_add_epi16(y1,_mm256_add_epi16(temp2,temp3))));
}



static inline void crt3modqx2(__m256i *u1, __m256i *u2, __m256i *u3, __m256i *umodq) {
  __m256i y10 = u1[0];
  __m256i y11 = u1[1];

  __m256i y20 = gen_montp(_mm256_sub_epi16(u2[0],y10),vC2R_16,vP2_16,vP2i_16);
  __m256i y21 = gen_montp(_mm256_sub_epi16(u2[1],y11),vC2R_16,vP2_16,vP2i_16);
  
  y20 = c_adjust(y20,vP2_16,vP2h_16);
  y21 = c_adjust(y21,vP2_16,vP2h_16);
  __m256i t30 = gen_montp(y20,vP1R_QQ_16,vQQ_16,vQQi_16);
  __m256i t31 = gen_montp(y21,vP1R_QQ_16,vQQ_16,vQQi_16);
  
  __m256i t00 = gen_montp(y20,vP1R_P3_16,vP3_16,vP3i_16);
  __m256i t01 = gen_montp(y21,vP1R_P3_16,vP3_16,vP3i_16);
  __m256i t10 = _mm256_sub_epi16(u3[0],_mm256_add_epi16(y10,t00));
  __m256i t11 = _mm256_sub_epi16(u3[1],_mm256_add_epi16(y11,t01));
  __m256i y30 = gen_montp(t10,vC3R_16,vP3_16,vP3i_16);
  __m256i y31 = gen_montp(t11,vC3R_16,vP3_16,vP3i_16);
  y30 = c_adjust(y30,vP3_16,vP3h_16);
  y31 = c_adjust(y31,vP3_16,vP3h_16);
  __m256i t20 = gen_montp(y30,vP12R_16,vQQ_16,vQQi_16);
  __m256i t21 = gen_montp(y31,vP12R_16,vQQ_16,vQQi_16);

  umodq[0]=barrett_real_qq(_mm256_add_epi16(y10,_mm256_add_epi16(t20,t30)));
  umodq[1]=barrett_real_qq(_mm256_add_epi16(y11,_mm256_add_epi16(t21,t31)));
}

static inline __m256i ecrt3modq(__m256i u1, __m256i u2, __m256i u3) {
  __m256i t1 = gen_montp(u1,vP23iR_16,vP1_16,vP1i_16);
  __m256i t2 = gen_montp(u2,vP13iR_16,vP2_16,vP2i_16);
  __m256i t3 = gen_montp(u3,vP12iR_16,vP3_16,vP3i_16);
  __m256i temp1 = gen_montp(t1,vP23R_16,vQQ_16,vQQi_16);
  __m256i temp2 = gen_montp(t2,vP13R_16,vQQ_16,vQQi_16);
  __m256i temp3 = gen_montp(t3,vP12R_16,vQQ_16,vQQi_16);
  __m256i temp = _mm256_add_epi16(temp1,_mm256_add_epi16(temp2,temp3));
  temp1 = _mm256_srai_epi16(_mm256_mulhi_epi16(t1,vP1i2p27),shift_p1);
  temp2 = _mm256_srai_epi16(_mm256_mulhi_epi16(t2,vP2i2p28),shift_p2);
  temp3 = _mm256_srai_epi16(_mm256_mulhi_epi16(t3,vP3i2p28),shift_p3);
  __m256i rounda = _mm256_srai_epi16(_mm256_add_epi16(_mm256_add_epi16(v6_16,temp1),_mm256_add_epi16(temp2,temp3)),3);
  return(barrett_real_qq(_mm256_sub_epi16(temp,_mm256_mullo_epi16(rounda,vP123_16))));
}
