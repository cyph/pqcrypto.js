#ifndef _POLYMUL_8X8_H_
#define _POLYMUL_8X8_H_ 

#include <stdint.h>
#include <stdio.h>
#include <emmintrin.h>
#include <tmmintrin.h>
#include <immintrin.h>

#include "gf4591_avx2.h"
#include "polymul_avx2.h"




//
// Convert 32bits data to 16bits data with montgomery multiplication.
//
// [...] <-- one 128-bits register.
//
//  input: c_0_2: [0][2],     c_1_3:  [1][3], c_4_6: [4][6], c_5_7: [5][7]  <-- 32-bits data
// output: c0:    [0,1][2,3], c1: [4,5][6,7]  <-- values stored in montgormery "divR" forms.
static inline void gf_mont_4x32_2x16 (__m256i *c0, __m256i *c1, __m256i c_0_2, __m256i c_1_3, __m256i c_4_6, __m256i c_5_7) {
// split high/low 16-bits data.
  c_0_2 = _mm256_shuffle_epi8( c_0_2 , _mm256_set_epi8(15,14,11,10,7,6,3,2,13,12,9,8,5,4,1,0, 15,14,11,10,7,6,3,2,13,12,9,8,5,4,1,0) );
  c_1_3 = _mm256_shuffle_epi8( c_1_3 , _mm256_set_epi8(15,14,11,10,7,6,3,2,13,12,9,8,5,4,1,0, 15,14,11,10,7,6,3,2,13,12,9,8,5,4,1,0) );
  c_4_6 = _mm256_shuffle_epi8( c_4_6 , _mm256_set_epi8(15,14,11,10,7,6,3,2,13,12,9,8,5,4,1,0, 15,14,11,10,7,6,3,2,13,12,9,8,5,4,1,0) );
  c_5_7 = _mm256_shuffle_epi8( c_5_7 , _mm256_set_epi8(15,14,11,10,7,6,3,2,13,12,9,8,5,4,1,0, 15,14,11,10,7,6,3,2,13,12,9,8,5,4,1,0) );

  __m256i c0_lo = _mm256_unpacklo_epi64( c_0_2 , c_1_3 );
  __m256i c0_hi = _mm256_unpackhi_epi64( c_0_2 , c_1_3 );
  __m256i c1_lo = _mm256_unpacklo_epi64( c_4_6 , c_5_7 );
  __m256i c1_hi = _mm256_unpackhi_epi64( c_4_6 , c_5_7 );

  __m256i c0_d = _mm256_mullo_epi16( c0_lo,v15631_16);
  __m256i c1_d = _mm256_mullo_epi16( c1_lo,v15631_16);

  __m256i c0_e = _mm256_mulhi_epi16(c0_d,v4591_16);
  __m256i c1_e = _mm256_mulhi_epi16(c1_d,v4591_16);

  * c0 = _mm256_sub_epi16(c0_hi,c0_e);
  * c1 = _mm256_sub_epi16(c1_hi,c1_e);
}






///////////////  8x8  ///////////////////////





// write a = (ah, al), b = (bh, bl) degree < 8 polynomials
// (c0h, c0l) + x^8 (c1h, c1l) = (ah bh, al bl) / 2^16
static inline void gf_polymul_8x8_divR(__m256i *c0, __m256i *c1, __m256i a, __m256i b){
  __m256i c_7531, c_xdb9, c_eca8, c_6420;

  polymul_8x8_32(&c_6420, &c_7531, &c_eca8, &c_xdb9, a, b);
  gf_mont_4x32_2x16 (c0, c1, c_6420, c_7531, c_eca8, c_xdb9);
}


static inline void gf_polymul_8x8 (__m256i *c0, __m256i *c1, __m256i a, __m256i b){

  __m256i c_hi, c_lo;

  gf_polymul_8x8_divR(&c_lo, &c_hi, a, b);
  *c1 = montproduct(c_hi, v4158_16);
  *c0 = montproduct(c_lo, v4158_16);
}


static inline void polymul8 (__m256i *c0, __m256i *c1, __m256i a, __m256i b) {
  gf_polymul_8x8(c0, c1, a, b);
}




////////////////  16x16   ////////////////////////////////



//
// 16x16 polynomial multiplication.
// Results are reduced and stored in montgomery form.
// Presuming size of inputs < 4591*2
// Size of outpus < 4591
//
#if 1
static inline void gf_polymul_16x16_avx2_divR(__m256i *r0, __m256i *r1, __m256i a, __m256i b){ 

  __m256i a0b0e, a0b0o, a1b1e, a1b1o;

  polymul_16x16_32(&a0b0e, &a0b0o, &a1b1e, &a1b1o,a,b);
  gf_mont_4x32_2x16(r0,r1,a0b0e,a0b0o,a1b1e,a1b1o);
}

#else

// stick on 16-bits data. slow.
static inline void gf_polymul_8x8_x2_divR( __m256i *rc0, __m256i *rc1, __m256i a , __m256i b )
{
	__m256i b_lo = _mm256_unpacklo_epi16(b,b);
	__m256i c0 = montproduct( a , _mm256_shuffle_epi32( b_lo , 0 ) ); // b0 ,0,0,0,0
	__m256i c1 = montproduct( a , _mm256_shuffle_epi32( b_lo , 0x55 ) ); // b1 ,01,01,01,01
	__m256i c2 = montproduct( a , _mm256_shuffle_epi32( b_lo , 0xaa ) ); // b2 ,10,10,10,10
	__m256i c3 = montproduct( a , _mm256_shuffle_epi32( b_lo , 0xff ) ); // b3 ,11,11,11,11
	__m256i b_hi = _mm256_unpackhi_epi16(b,b);
	__m256i c4 = montproduct( a , _mm256_shuffle_epi32( b_hi , 0 ) ); // b4
	__m256i c5 = montproduct( a , _mm256_shuffle_epi32( b_hi , 0x55 ) ); // b5
	__m256i c6 = montproduct( a , _mm256_shuffle_epi32( b_hi , 0xaa ) ); // b6
	__m256i c7 = montproduct( a , _mm256_shuffle_epi32( b_hi , 0xff ) ); // b7

        __m256i rr0 = c0;
        rr0 = _mm256_add_epi16( rr0 , _mm256_alignr_epi8(c1,_mm256_setzero_si256(),14) ); // <<2
        __m256i rr1 = _mm256_alignr_epi8(_mm256_setzero_si256(),c1,14);

        rr0 = _mm256_add_epi16( rr0 , _mm256_alignr_epi8(c2,_mm256_setzero_si256(),12) );
        rr1 = _mm256_add_epi16( rr1 , _mm256_alignr_epi8(_mm256_setzero_si256(),c2,12) );

        rr0 = _mm256_add_epi16( rr0 , _mm256_alignr_epi8(c3,_mm256_setzero_si256(),10) );
        rr1 = _mm256_add_epi16( rr1 , _mm256_alignr_epi8(_mm256_setzero_si256(),c3,10) );;

        rr0 = barrett_fake( rr0 );
        rr1 = barrett_fake( rr1 );

        rr0 = _mm256_add_epi16( rr0 , _mm256_alignr_epi8(c4,_mm256_setzero_si256(),8) );
        rr1 = _mm256_add_epi16( rr1 , _mm256_alignr_epi8(_mm256_setzero_si256(),c4,8) );

        rr0 = _mm256_add_epi16( rr0 , _mm256_alignr_epi8(c5,_mm256_setzero_si256(),6) );
        rr1 = _mm256_add_epi16( rr1 , _mm256_alignr_epi8(_mm256_setzero_si256(),c5,6) );

        rr0 = _mm256_add_epi16( rr0 , _mm256_alignr_epi8(c6,_mm256_setzero_si256(),4) );
        rr1 = _mm256_add_epi16( rr1 , _mm256_alignr_epi8(_mm256_setzero_si256(),c6,4) );

        rr0 = _mm256_add_epi16( rr0 , _mm256_alignr_epi8(c7,_mm256_setzero_si256(),2) );
        rr1 = _mm256_add_epi16( rr1 , _mm256_alignr_epi8(_mm256_setzero_si256(),c7,2) );

        *rc0 = barrett_fake( rr0 );
        *rc1 = barrett_fake( rr1 );
}

static inline void gf_polymul_16x16_avx2_divR(__m256i *r0, __m256i *r1, __m256i a, __m256i b){ 

  __m256i a_flip = _mm256_permute4x64_epi64(a, 0x4e); // 1,0|3,2
  __m256i t_lo;
  __m256i t_hi;
  gf_polymul_8x8_x2_divR( &t_lo, &t_hi, a_flip , b );

  __m256i c_mid0 = _mm256_permute2x128_si256(t_lo,t_hi,0x20); //  0,2|0,0
  __m256i c_mid1 = _mm256_permute2x128_si256(t_lo,t_hi,0x31); //  0,3|0,1

  __m256i c_lo;
  __m256i c_hi;
  gf_polymul_8x8_x2_divR( &c_lo, &c_hi, a , b );

  __m256i c_mid2 = _mm256_permute2x128_si256(c_hi,c_lo,0x30); //  0,3|0,0

  c_mid0 = _mm256_add_epi16( c_mid0 , c_mid1 );
  c_mid0 = _mm256_add_epi16( c_mid0 , c_mid2 );

  *r0 = _mm256_permute2x128_si256(c_lo,c_mid0,0x20);
  *r1 = _mm256_permute2x128_si256(c_mid0,c_hi,0x31);
}

#endif



//
// 16x16 polynomial multiplication.
// Results are reduced.
// Presuming size of inputs < 4591*2
// Size of outpus < 2336
//
static inline void gf_polymul_16x16_avx2(__m256i *r0, __m256i *r1, __m256i a, __m256i b){ 
  __m256i t0,t1;

  gf_polymul_16x16_avx2_divR(&t0,&t1,a,b);
  *r1 = montproduct(t1, v4158_16);
  *r0 = montproduct(t0, v4158_16);
}



////////////////////



static inline void gf_polymul_16x16_avx2_uvqr_fg (__m256i *ff, __m256i *gg, __m256i *uvqr, __m256i *f, __m256i *g) {
  __m256i t[2];

  __m256i u_mask = uv_over(uvqr);
  __m256i v_mask = uv_over(uvqr+1);
  __m256i uv_mask = _mm256_or_si256(u_mask,v_mask);
  
  gf_polymul_16x16_avx2_divR(ff,ff+1,uvqr[0],*f);
  gf_polymul_16x16_avx2_divR(t,t+1,uvqr[1],*g);
  ff[0]=_mm256_add_epi16(ff[0],t[0]);
  ff[1]=_mm256_add_epi16(ff[1],t[1]);

  ff[1]=_mm256_or_si256(_mm256_and_si256(uv_mask,ff[0]),ff[1]);
  ff[0]=_mm256_andnot_si256(uv_mask,ff[0]);

  gf_polymul_16x16_avx2_divR(gg,gg+1,uvqr[2],*f);
  gf_polymul_16x16_avx2_divR(t,t+1,uvqr[3],*g);
  gg[0]=_mm256_add_epi16(gg[0],t[0]);
  gg[1]=_mm256_add_epi16(gg[1],t[1]);

  ff[0] = montproduct(ff[0],v4158_16); // <3238
  gg[0] = montproduct(gg[0],v4158_16); // <3238
  ff[1] = montproduct(ff[1],v4158_16); // <3238
  gg[1] = montproduct(gg[1],v4158_16); // <3238
}

static inline void gf_polymul_16x16_avx2_uvqr_fg_x (__m256i *ff, __m256i *gg, __m256i *uvqr, __m256i *f, __m256i *g) {
  __m256i t[2];
  __m256i u_mask = uv_over(uvqr);
  __m256i v_mask = uv_over(uvqr+2);
  __m256i uv_mask = _mm256_or_si256(u_mask,v_mask);

  gf_polymul_16x16_avx2_divR(ff,ff+1,uvqr[0],*f);
  gf_polymul_16x16_avx2_divR(t,t+1,uvqr[1],*g);
  ff[0]=_mm256_add_epi16(ff[0],t[0]);
  ff[1]=_mm256_add_epi16(ff[1],t[1]);

  ff[1]=_mm256_or_si256(_mm256_and_si256(uv_mask,ff[0]),ff[1]);
  ff[0]=_mm256_andnot_si256(uv_mask,ff[0]);

  ff[0] = montproduct(ff[0],v4158_16); // <3238
  ff[1] = montproduct(ff[1],v4158_16); // <3238
}


static inline void gf_polymul_16x16_avx2_uvqr_vr (__m256i *ff, __m256i *gg, __m256i *uvqr, __m256i *f, __m256i *g) {
  __m256i t[2];

  __m256i u_mask = uv_over(uvqr);
  __m256i v_mask = uv_over(uvqr+1);
  __m256i uv_mask = _mm256_or_si256(u_mask,v_mask);
  __m256i f_mask = uv_over(f);
  
  gf_polymul_16x16_avx2_divR(ff,ff+1,uvqr[0],*f);
  ff[1]=_mm256_or_si256(_mm256_and_si256(f_mask,ff[0]),ff[1]);
  ff[0]=_mm256_andnot_si256(f_mask,ff[0]);
  
  gf_polymul_16x16_avx2_divR(t,t+1,uvqr[1],*g);

  ff[0]=_mm256_add_epi16(ff[0],t[0]);
  ff[1]=_mm256_add_epi16(ff[1],t[1]);

  ff[1]=_mm256_or_si256(_mm256_and_si256(uv_mask,ff[0]),
			_mm256_andnot_si256(uv_mask,ff[1]));
  ff[0]=_mm256_andnot_si256(uv_mask,ff[0]);

  gf_polymul_16x16_avx2_divR(gg,gg+1,uvqr[2],*f);
  gg[1]=_mm256_or_si256(_mm256_and_si256(f_mask,gg[0]),gg[1]);
  gg[0]=_mm256_andnot_si256(f_mask,gg[0]);

  gf_polymul_16x16_avx2_divR(t,t+1,uvqr[3],*g);

  gg[0]=_mm256_add_epi16(gg[0],t[0]);
  gg[1]=_mm256_add_epi16(gg[1],t[1]);

  ff[0] = montproduct(ff[0],v4158_16); // <3238
  gg[0] = montproduct(gg[0],v4158_16); // <3238
  ff[1] = montproduct(ff[1],v4158_16); // <3238
  gg[1] = montproduct(gg[1],v4158_16); // <3238

  ff[0] = _mm256_or_si256(_mm256_and_si256(_mm256_and_si256(u_mask,f_mask),v01),ff[0]);

}

static inline void gf_polymul_16x16_avx2_uvqr_vr_x (__m256i *ff, __m256i *gg, __m256i *uvqr, __m256i *f, __m256i *g) {
  __m256i t[2];

  __m256i u_mask = uv_over(uvqr);
  __m256i v_mask = uv_over(uvqr+1);
  __m256i uv_mask = _mm256_or_si256(u_mask,v_mask);
  __m256i f_mask = uv_over(f);
  
  gf_polymul_16x16_avx2_divR(ff,ff+1,uvqr[0],*f);
  ff[1]=_mm256_or_si256(_mm256_and_si256(f_mask,ff[0]),ff[1]);
  ff[0]=_mm256_andnot_si256(f_mask,ff[0]);
  
  gf_polymul_16x16_avx2_divR(t,t+1,uvqr[1],*g);

  ff[0]=_mm256_add_epi16(ff[0],t[0]);
  ff[1]=_mm256_add_epi16(ff[1],t[1]);

  ff[1]=_mm256_or_si256(_mm256_and_si256(uv_mask,ff[0]),
			_mm256_andnot_si256(uv_mask,ff[1]));
  ff[0]=_mm256_andnot_si256(uv_mask,ff[0]);

  ff[0] = montproduct(ff[0],v4158_16); // <3238
  ff[1] = montproduct(ff[1],v4158_16); // <3238

  ff[0] = _mm256_or_si256(_mm256_and_si256(_mm256_and_si256(u_mask,f_mask),v01),ff[0]);

}

#endif
