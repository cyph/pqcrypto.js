#ifndef _MUL_H
#define _MUL_H

/* To use gf2x functions */
#define ENABLE_GF2X 0

#include <stdint.h>
#include "arch.h"
#include "extract.h"

/* Multiplication in GF(2)[X] */
/* The macros use classical multiplication */
/* Squaring in GF(2)[X] */
/* The macros use (a+b*X^64)^2 = a^2 + (b^2)*X^128 */

uint64_t mul64low(uint64_t A, uint64_t B);
#if ENABLE_GF2X
    #include <gf2x/gf2x_mul1.h>
    #include <gf2x/gf2x_mul2.h>
    #include <gf2x/gf2x_mul3.h>
    #include <gf2x/gf2x_mul4.h>
    #include <gf2x/gf2x_mul5.h>
    #include <gf2x/gf2x_mul6.h>
    #define mul64(C,A,B) gf2x_mul1(C,A,B)
    #define mul128(C,A,B) gf2x_mul2(C,A,B)
    #define mul192(C,A,B) gf2x_mul3(C,A,B)
    #define mul256(C,A,B) gf2x_mul4(C,A,B)
    #define mul320(C,A,B) gf2x_mul5(C,A,B)
    #define mul384(C,A,B) gf2x_mul6(C,A,B)

    #define mul96 mul128
    #define mul160 mul192
    #define mul224 mul256
    #define mul288 mul320
    #define mul352 mul384
#else
    void mul64(uint64_t C[2], uint64_t A, uint64_t B);
    void mul128(uint64_t C[4], const uint64_t A[2], const uint64_t B[2]);
    void mul192(uint64_t C[6], const uint64_t A[3], const uint64_t B[3]);
    void mul256(uint64_t C[8], const uint64_t A[4], const uint64_t B[4]);
    void mul320(uint64_t C[10], const uint64_t A[5], const uint64_t B[5]);
    void mul384(uint64_t C[12], const uint64_t A[6], const uint64_t B[6]);

    #ifdef ENABLED_AVX_MUL
        void mul96(uint64_t C[3], const uint64_t A[2], const uint64_t B[2]);
        void mul160(uint64_t C[5], const uint64_t A[3], const uint64_t B[3]);
        void mul224(uint64_t C[7], const uint64_t A[4], const uint64_t B[4]);
        void mul288(uint64_t C[9], const uint64_t A[5], const uint64_t B[5]);
        void mul352(uint64_t C[11], const uint64_t A[6], const uint64_t B[6]);
    #else
        #define mul96 mul128
        #define mul160 mul192
        #define mul224 mul256
    #endif
#endif


uint64_t square64low(uint64_t A);
#define square32 square64low
#ifdef ENABLED_AVX_MUL
    void square64(uint64_t C[2], uint64_t A);
    void square96(uint64_t C[3], const uint64_t A[2]);
    void square128(uint64_t C[4], const uint64_t A[2]);
    void square160(uint64_t C[5], const uint64_t A[3]);
    void square192(uint64_t C[6], const uint64_t A[3]);
    void square224(uint64_t C[7], const uint64_t A[4]);
    void square256(uint64_t C[8], const uint64_t A[4]);
    void square288(uint64_t C[9], const uint64_t A[5]);
    void square320(uint64_t C[10], const uint64_t A[5]);
    void square352(uint64_t C[11], const uint64_t A[6]);
    void square384(uint64_t C[12], const uint64_t A[6]);
#else
    #define square64 SQUARE64
    #define square96 SQUARE96
    #define square128 SQUARE128
    #define square160 SQUARE160
    #define square192 SQUARE192
    #define square224 SQUARE224
    #define square256 SQUARE256
    #define square288 SQUARE288
    #define square320 SQUARE320
    #define square352 SQUARE352
    #define square384 SQUARE384
#endif


#ifdef ENABLED_AVX_MUL
#include <immintrin.h>

#define mul32 mul64low
void mul128_Karat(uint64_t C[4], const uint64_t A[2], const uint64_t B[2]);
void mul256_Karat(uint64_t C[8], const uint64_t A[4], const uint64_t B[4]);
void mul256_Karat_first_version(uint64_t C[8], const uint64_t A[4], const uint64_t B[4]);


/* C is the result, a tabular of word of 64 bits */
/* pos is the 3th argument in _mm_clmulepi64_si128 intrinsics */
/* The other variables are __m128i, with x and y initialized (to do x*y) */

/* To understand better how to use, look the file mul.c */


/* 1 mul64 */
#define MUL64LOW(x,y,pos) EXTRACT64(_mm_clmulepi64_si128(x, y, pos),0)

#define MUL32(x,y) MUL64LOW(x,y,0)

/* 1 mul64 */
#define MUL64(C,x,y,z,pos) \
    z = _mm_clmulepi64_si128(x, y, pos);\
    EXTRACT128(C,z);


/* 4 mul64, 3 add128 */
#define MUL128_FINALMUL(FINAL_EXTRACT,C,x,y,z1,z2,sum,res_low,res_high) \
    /* X^0 */\
    res_low =_mm_clmulepi64_si128(x,y,0);\
\
    /* X^64 */\
    z1 = _mm_clmulepi64_si128(x, y, 1);\
    z2 = _mm_clmulepi64_si128(x, y, 0x10);\
    res_high =_mm_xor_si128(z1,z2);\
\
    /* mid2_low: x^64 ... x^127 */\
    sum=LEFT_SHIFT64(res_high);\
    /* mid2_low + L */\
    z1=_mm_xor_si128(res_low,sum);\
    EXTRACT128(C,z1);\
\
    /* X^128 */\
    res_low=_mm_clmulepi64_si128(x,y,0x11);\
\
    /* mid2_high: x^128 ... x^191 */\
    sum=RIGHT_SHIFT64(res_high);\
    /* mid2_high + H */\
    z2=_mm_xor_si128(res_low,sum);\
    FINAL_EXTRACT;

#define MUL96(C,x,y,z1,z2,sum,res_low,res_high) MUL128_FINALMUL((C)[2]=EXTRACT64(z2,0),C,x,y,z1,z2,sum,res_low,res_high)
#define MUL128(C,x,y,z1,z2,sum,res_low,res_high) MUL128_FINALMUL(EXTRACT128(C+2,z2),C,x,y,z1,z2,sum,res_low,res_high)


/* 9 mul64, 7 add128 */
#define MUL192_FINALMUL(FINAL_EXTRACT,C,x1,x2,y1,y2,z1,z2,sum,res1,res2) \
    /* X^0 */\
    res1 =_mm_clmulepi64_si128(x1,y1,0);\
\
    /* X^64 */\
    z1 = _mm_clmulepi64_si128(x1, y1, 1);\
    z2 = _mm_clmulepi64_si128(x1, y1, 0x10);\
    res2 =_mm_xor_si128(z1,z2);\
\
    sum=LEFT_SHIFT64(res2);\
    z1=_mm_xor_si128(res1,sum);\
    EXTRACT128(C,z1);\
\
    /* X^128 */\
    z1 = _mm_clmulepi64_si128(x1, y1, 0x11);\
    z2 = _mm_clmulepi64_si128(x2, y1, 0);\
    sum =_mm_xor_si128(z1,z2);\
    z1 = _mm_clmulepi64_si128(x1, y2, 0);\
    res1 =_mm_xor_si128(sum,z1);\
\
    /* X^192 */\
    z1 = _mm_clmulepi64_si128(y1, x2, 1);\
    z2 = _mm_clmulepi64_si128(x1, y2, 1);\
    sum =_mm_xor_si128(z1,z2);\
\
    z1=RIGHT_MOVE64(res2);\
    z2=_mm_unpacklo_epi64(z1,sum);\
    z1=_mm_xor_si128(res1,z2);\
    EXTRACT128(C+2,z1);\
\
    /* X^256 */\
    res1=_mm_clmulepi64_si128(x2,y2,0);\
    z1=RIGHT_SHIFT64(sum);\
    z2=_mm_xor_si128(res1,z1);\
    FINAL_EXTRACT;


#define MUL160(C,x1,x2,y1,y2,z1,z2,sum,res1,res2) MUL192_FINALMUL((C)[4]=EXTRACT64(z2,0),C,x1,x2,y1,y2,z1,z2,sum,res1,res2)
#define MUL192(C,x1,x2,y1,y2,z1,z2,sum,res1,res2) MUL192_FINALMUL(EXTRACT128(C+4,z2),C,x1,x2,y1,y2,z1,z2,sum,res1,res2)


/* 16 mul64, 13 add128 */
#define MUL256_FINALMUL(FINAL_EXTRACT,C,x1,x2,y1,y2,z1,z2,sum,res1,res2) \
    /* X^0 */\
    res1 =_mm_clmulepi64_si128(x1,y1,0);\
\
    /* X^64 */\
    z1 = _mm_clmulepi64_si128(x1, y1, 1);\
    z2 = _mm_clmulepi64_si128(x1, y1, 0x10);\
    res2 =_mm_xor_si128(z1,z2);\
\
    sum=LEFT_SHIFT64(res2);\
    z1=_mm_xor_si128(res1,sum);\
    EXTRACT128(C,z1);\
\
    /* X^128 */\
    z1 = _mm_clmulepi64_si128(x1, y1, 0x11);\
    z2 = _mm_clmulepi64_si128(x2, y1, 0);\
    sum =_mm_xor_si128(z1,z2);\
    z1 = _mm_clmulepi64_si128(x1, y2, 0);\
    res1 =_mm_xor_si128(sum,z1);\
\
    /* X^192 */\
    z1 = _mm_clmulepi64_si128(x1, y2, 1);\
    z2 = _mm_clmulepi64_si128(x1, y2, 0x10);\
    sum =_mm_xor_si128(z1,z2);\
    z1 = _mm_clmulepi64_si128(y1, x2, 1);\
    z2 =_mm_xor_si128(sum,z1);\
    z1 = _mm_clmulepi64_si128(y1, x2, 0x10);\
    sum =_mm_xor_si128(z1,z2);\
\
    z1=RIGHT_MOVE64(res2);\
    z2=_mm_unpacklo_epi64(z1,sum);\
    z1=_mm_xor_si128(res1,z2);\
    EXTRACT128(C+2,z1);\
\
    /* X^256 */\
    z1 = _mm_clmulepi64_si128(y1, x2, 0x11);\
    z2 = _mm_clmulepi64_si128(y2, x2, 0);\
    res2 =_mm_xor_si128(z1,z2);\
    z1 = _mm_clmulepi64_si128(x1, y2, 0x11);\
    res1 =_mm_xor_si128(res2,z1);\
\
    /* X^320 */\
    z1 = _mm_clmulepi64_si128(x2, y2, 1);\
    z2 = _mm_clmulepi64_si128(x2, y2, 0x10);\
    res2 =_mm_xor_si128(z1,z2);\
\
    z1=RIGHT_MOVE64(sum);\
    z2=_mm_unpacklo_epi64(z1,res2);\
    z1=_mm_xor_si128(res1,z2);\
    EXTRACT128(C+4,z1);\
\
    /* X^384 */\
    res1 =_mm_clmulepi64_si128(x2,y2,0x11);\
    z1=RIGHT_SHIFT64(res2);\
    z2=_mm_xor_si128(res1,z1);\
    FINAL_EXTRACT;


#define MUL224(C,x1,x2,y1,y2,z1,z2,sum,res1,res2) MUL256_FINALMUL((C)[6]=EXTRACT64(z2,0),C,x1,x2,y1,y2,z1,z2,sum,res1,res2)
#define MUL256(C,x1,x2,y1,y2,z1,z2,sum,res1,res2) MUL256_FINALMUL(EXTRACT128(C+6,z2),C,x1,x2,y1,y2,z1,z2,sum,res1,res2)


/* 25 mul64, 21 add128 */
#define MUL320_FINALMUL(FINAL_EXTRACT,C,x1,x2,x3,y1,y2,y3,z1,z2,sum,res1,res2) \
    /* X^0 */\
    res1 =_mm_clmulepi64_si128(x1,y1,0);\
\
    /* X^64 */\
    z1 = _mm_clmulepi64_si128(x1, y1, 1);\
    z2 = _mm_clmulepi64_si128(x1, y1, 0x10);\
    res2 =_mm_xor_si128(z1,z2);\
\
    sum=LEFT_SHIFT64(res2);\
    z1=_mm_xor_si128(res1,sum);\
    EXTRACT128(C,z1);\
\
    /* X^128 */\
    z1 = _mm_clmulepi64_si128(x1, y1, 0x11);\
    z2 = _mm_clmulepi64_si128(x2, y1, 0);\
    sum =_mm_xor_si128(z1,z2);\
    z1 = _mm_clmulepi64_si128(x1, y2, 0);\
    res1 =_mm_xor_si128(sum,z1);\
\
    /* X^192 */\
    z1 = _mm_clmulepi64_si128(x1, y2, 1);\
    z2 = _mm_clmulepi64_si128(x1, y2, 0x10);\
    sum =_mm_xor_si128(z1,z2);\
    z1 = _mm_clmulepi64_si128(y1, x2, 1);\
    z2 =_mm_xor_si128(sum,z1);\
    z1 = _mm_clmulepi64_si128(y1, x2, 0x10);\
    sum =_mm_xor_si128(z1,z2);\
\
    z1=RIGHT_MOVE64(res2);\
    z2=_mm_unpacklo_epi64(z1,sum);\
    z1=_mm_xor_si128(res1,z2);\
    EXTRACT128(C+2,z1);\
\
    /* X^256 */\
    z1 = _mm_clmulepi64_si128(x1, y3, 0);\
    z2 = _mm_clmulepi64_si128(x1, y2, 0x11);\
    res2 =_mm_xor_si128(z1,z2);\
    z1 = _mm_clmulepi64_si128(x2, y2, 0);\
    res1 =_mm_xor_si128(res2,z1);\
    z1 = _mm_clmulepi64_si128(x2, y1, 0x11);\
    res2 =_mm_xor_si128(z1,res1);\
    z2 = _mm_clmulepi64_si128(x3, y1, 0);\
    res1 =_mm_xor_si128(res2,z2);\
\
    /* X^320 */\
    z1 = _mm_clmulepi64_si128(x1, y3, 1);\
    z2 = _mm_clmulepi64_si128(x2, y2, 0x10);\
    res2 =_mm_xor_si128(z1,z2);\
    z1 = _mm_clmulepi64_si128(x2, y2, 1);\
    z2 =_mm_xor_si128(res2,z1);\
    z1 = _mm_clmulepi64_si128(x3, y1, 0x10);\
    res2 =_mm_xor_si128(z1,z2);\
\
    z1=RIGHT_MOVE64(sum);\
    z2=_mm_unpacklo_epi64(z1,res2);\
    z1=_mm_xor_si128(res1,z2);\
    EXTRACT128(C+4,z1);\
\
    /* X^384 */\
    z1 = _mm_clmulepi64_si128(x2, y3, 0);\
    z2 = _mm_clmulepi64_si128(x2, y2, 0x11);\
    sum =_mm_xor_si128(z1,z2);\
    z1 = _mm_clmulepi64_si128(x3, y2, 0);\
    res1 =_mm_xor_si128(z1,sum);\
\
    /* X^448 */\
    z1 = _mm_clmulepi64_si128(x2, y3, 1);\
    z2 = _mm_clmulepi64_si128(x3, y2, 0x10);\
    sum =_mm_xor_si128(z1,z2);\
\
    z1=RIGHT_MOVE64(res2);\
    z2=_mm_unpacklo_epi64(z1,sum);\
    z1=_mm_xor_si128(res1,z2);\
    EXTRACT128(C+6,z1);\
\
    /* X^512 */\
    res1 =_mm_clmulepi64_si128(x3,y3,0);\
    z1=RIGHT_SHIFT64(sum);\
    z2=_mm_xor_si128(res1,z1);\
    FINAL_EXTRACT;


#define MUL288(C,x1,x2,x3,y1,y2,y3,z1,z2,sum,res1,res2) MUL320_FINALMUL((C)[8]=EXTRACT64(z2,0),C,x1,x2,x3,y1,y2,y3,z1,z2,sum,res1,res2)
#define MUL320(C,x1,x2,x3,y1,y2,y3,z1,z2,sum,res1,res2) MUL320_FINALMUL(EXTRACT128(C+8,z2),C,x1,x2,x3,y1,y2,y3,z1,z2,sum,res1,res2)


/* ??????????????????????????? 25 mul64, 21 add128 */
#define MUL384_FINALMUL(FINAL_EXTRACT,C,x1,x2,x3,y1,y2,y3,z1,z2,sum,res1,res2) \
    /* X^0 */\
    res1 =_mm_clmulepi64_si128(x1,y1,0);\
\
    /* X^64 */\
    z1 = _mm_clmulepi64_si128(x1, y1, 1);\
    z2 = _mm_clmulepi64_si128(x1, y1, 0x10);\
    res2 =_mm_xor_si128(z1,z2);\
\
    sum=LEFT_SHIFT64(res2);\
    z1=_mm_xor_si128(res1,sum);\
    EXTRACT128(C,z1);\
\
    /* X^128 */\
    z1 = _mm_clmulepi64_si128(x1, y1, 0x11);\
    z2 = _mm_clmulepi64_si128(x2, y1, 0);\
    sum =_mm_xor_si128(z1,z2);\
    z1 = _mm_clmulepi64_si128(x1, y2, 0);\
    res1 =_mm_xor_si128(sum,z1);\
\
    /* X^192 */\
    z1 = _mm_clmulepi64_si128(x1, y2, 1);\
    z2 = _mm_clmulepi64_si128(x1, y2, 0x10);\
    sum =_mm_xor_si128(z1,z2);\
    z1 = _mm_clmulepi64_si128(y1, x2, 1);\
    z2 =_mm_xor_si128(sum,z1);\
    z1 = _mm_clmulepi64_si128(y1, x2, 0x10);\
    sum =_mm_xor_si128(z1,z2);\
\
    z1=RIGHT_MOVE64(res2);\
    z2=_mm_unpacklo_epi64(z1,sum);\
    z1=_mm_xor_si128(res1,z2);\
    EXTRACT128(C+2,z1);\
\
    /* X^256 */\
    z1 = _mm_clmulepi64_si128(x1, y3, 0);\
    z2 = _mm_clmulepi64_si128(x1, y2, 0x11);\
    res2 =_mm_xor_si128(z1,z2);\
    z1 = _mm_clmulepi64_si128(x2, y2, 0);\
    res1 =_mm_xor_si128(res2,z1);\
    z1 = _mm_clmulepi64_si128(x2, y1, 0x11);\
    res2 =_mm_xor_si128(z1,res1);\
    z2 = _mm_clmulepi64_si128(x3, y1, 0);\
    res1 =_mm_xor_si128(res2,z2);\
\
    /* X^320 */\
    z1 = _mm_clmulepi64_si128(x1, y3, 0x10);\
    z2 = _mm_clmulepi64_si128(x1, y3, 1);\
    res2 =_mm_xor_si128(z1,z2);\
    z1 = _mm_clmulepi64_si128(x2, y2, 0x10);\
    z2 =_mm_xor_si128(res2,z1);\
    z1 = _mm_clmulepi64_si128(x2, y2, 1);\
    res2 =_mm_xor_si128(z1,z2);\
    z1 = _mm_clmulepi64_si128(x3, y1, 0x10);\
    z2 =_mm_xor_si128(res2,z1);\
    z1 = _mm_clmulepi64_si128(x3, y1, 1);\
    res2 =_mm_xor_si128(z1,z2);\
\
    z1=RIGHT_MOVE64(sum);\
    z2=_mm_unpacklo_epi64(z1,res2);\
    z1=_mm_xor_si128(res1,z2);\
    EXTRACT128(C+4,z1);\
\
    /* X^384 */\
    z1 = _mm_clmulepi64_si128(x1, y3, 0x11);\
    z2 = _mm_clmulepi64_si128(x2, y3, 0);\
    sum =_mm_xor_si128(z1,z2);\
    z1 = _mm_clmulepi64_si128(x2, y2, 0x11);\
    res1 =_mm_xor_si128(z1,sum);\
    z2 = _mm_clmulepi64_si128(x3, y2, 0);\
    sum =_mm_xor_si128(res1,z2);\
    z1 = _mm_clmulepi64_si128(x3, y1, 0x11);\
    res1 =_mm_xor_si128(z1,sum);\
\
    /* X^448 */\
    z1 = _mm_clmulepi64_si128(x2, y3, 0x10);\
    z2 = _mm_clmulepi64_si128(x2, y3, 1);\
    sum =_mm_xor_si128(z1,z2);\
    z2 = _mm_clmulepi64_si128(x3, y2, 0x10);\
    z1 =_mm_xor_si128(sum,z2);\
    z2 = _mm_clmulepi64_si128(x3, y2, 1);\
    sum =_mm_xor_si128(z1,z2);\
\
    z1=RIGHT_MOVE64(res2);\
    z2=_mm_unpacklo_epi64(z1,sum);\
    z1=_mm_xor_si128(res1,z2);\
    EXTRACT128(C+6,z1);\
\
    /* X^512 */\
    z1 = _mm_clmulepi64_si128(x2, y3, 0x11);\
    z2 = _mm_clmulepi64_si128(x3, y3, 0);\
    res2 =_mm_xor_si128(z1,z2);\
    z1 = _mm_clmulepi64_si128(x3, y2, 0x11);\
    res1 =_mm_xor_si128(res2,z1);\
\
    /* X^576 */\
    z1 = _mm_clmulepi64_si128(x3, y3, 0x10);\
    z2 = _mm_clmulepi64_si128(x3, y3, 1);\
    res2 =_mm_xor_si128(z1,z2);\
\
    /* X^640 */\
    z1=RIGHT_MOVE64(sum);\
    z2=_mm_unpacklo_epi64(z1,res2);\
    z1=_mm_xor_si128(res1,z2);\
    EXTRACT128(C+8,z1);\
\
    /* X^704 */\
    res1 =_mm_clmulepi64_si128(x3,y3,0x11);\
    z1=RIGHT_SHIFT64(res2);\
    z2=_mm_xor_si128(res1,z1);\
    FINAL_EXTRACT;


#define MUL352(C,x1,x2,x3,y1,y2,y3,z1,z2,sum,res1,res2) MUL384_FINALMUL((C)[10]=EXTRACT64(z2,0),C,x1,x2,x3,y1,y2,y3,z1,z2,sum,res1,res2)
#define MUL384(C,x1,x2,x3,y1,y2,y3,z1,z2,sum,res1,res2) MUL384_FINALMUL(EXTRACT128(C+10,z2),C,x1,x2,x3,y1,y2,y3,z1,z2,sum,res1,res2)


/* 1 mul64 */
#define SQUARE64LOW(x,pos) EXTRACT64(_mm_clmulepi64_si128(x, x, pos),0)

#define SQUARE32(x) SQUARE64LOW(x,0)

/* 1 mul64 */
#define SQUARE64(C,x,z,pos) \
    z = _mm_clmulepi64_si128(x, x, pos);\
    EXTRACT128(C,z);

/* 2 mul64 */
#define SQUARE96(C,x,z) \
    SQUARE64(C,x,z,0);\
    (C)[2]=SQUARE64LOW(x,17);

/* 2 mul64 */
#define SQUARE128(C,x,z) \
    SQUARE64(C,x,z,0);\
    SQUARE64(C+2,x,z,17);

/* 3 mul64 */
#define SQUARE160(C,x1,x2,z) \
    SQUARE128(C,x1,z);\
    (C)[4]=SQUARE64LOW(x2,0);

/* 3 mul64 */
#define SQUARE192(C,x1,x2,z) \
    SQUARE128(C,x1,z);\
    SQUARE64(C+4,x2,z,0);

/* 4 mul64 */
#define SQUARE224(C,x1,x2,z) \
    SQUARE128(C,x1,z);\
    SQUARE96(C+4,x2,z);

/* 4 mul64 */
#define SQUARE256(C,x1,x2,z) \
    SQUARE128(C,x1,z);\
    SQUARE128(C+4,x2,z);

/* 5 mul64 */
#define SQUARE288(C,x1,x2,x3,z) \
    SQUARE256(C,x1,x2,z);\
    (C)[8]=SQUARE64LOW(x3,0);

/* 5 mul64 */
#define SQUARE320(C,x1,x2,x3,z) \
    SQUARE256(C,x1,x2,z);\
    SQUARE64(C+8,x3,z,0);

/* 6 mul64 */
#define SQUARE352(C,x1,x2,x3,z) \
    SQUARE256(C,x1,x2,z);\
    SQUARE96(C+8,x3,z);

/* 6 mul64 */
#define SQUARE384(C,x1,x2,x3,z) \
    SQUARE256(C,x1,x2,z);\
    SQUARE128(C+8,x3,z);

#else

uint64_t mul32(uint64_t A, uint64_t B);

/* initialize C */
#define MUL32(C,A,B,i) \
    (C)=(-((B)&1))&A; \
    for(i=1;i<32;++i)\
    {\
        (C)^=((-(((B)>>i)&1UL))&A)<<i; \
    }

/* initialize C */
#define MUL64LOW(C,A,B,i) \
    (C)=(-((B)&1))&A; \
    for(i=1;i<64;++i)\
    {\
        (C)^=((-(((B)>>i)&1UL))&A)<<i; \
    }

/* initialize C */
#define MUL64(C,A,B,i,tmp) \
    (C)[0]=(-((B)&1UL))&A; \
    tmp=((-(((B)>>63)&1UL))&A);\
    (C)[0]^=tmp<<63; \
    (C)[1]=tmp>>1; \
    for(i=1;i<63;++i)\
    {\
        tmp=((-(((B)>>i)&1UL))&A);\
        (C)[0]^=tmp<<i; \
        (C)[1]^=tmp>>(64-i); \
    }

/* initialize C[0] only */
#define MUL64_INIT1(C,A,B,i,tmp) \
    (C)[0]=(-((B)&1UL))&A; \
    for(i=1;i<64;++i)\
    {\
        tmp=((-(((B)>>i)&1UL))&A);\
        (C)[0]^=tmp<<i; \
        (C)[1]^=tmp>>(64-i); \
    }

/* No initialize C */
#define MUL64_NOINIT(C,A,B,i,tmp) \
    (C)[0]^=(-((B)&1UL))&A; \
    for(i=1;i<64;++i)\
    {\
        tmp=((-(((B)>>i)&1UL))&A);\
        (C)[0]^=tmp<<i; \
        (C)[1]^=tmp>>(64-i); \
    }

/* C0 is the low part of result and C1 is the high part */
/* No initialize C0 and C1 */
#define MUL64_NOINIT_SPLIT(C0,C1,A,B,i,tmp) \
    (C0)^=(-((B)&1UL))&A; \
    for(i=1;i<64;++i)\
    {\
        tmp=((-(((B)>>i)&1UL))&A);\
        (C0)^=tmp<<i; \
        (C1)^=tmp>>(64-i); \
    }


/* Karatsuba */
/* initialize C */
#define MUL128(C,A,B,i,tmp,AA,BB) \
    MUL64(C+2,(A)[1],(B)[1],i,tmp);\
    MUL64_INIT1(C+1,(A)[0],(B)[0],i,tmp);\
    /*  C[0] = unitialised
        C[1] = C0
        C[2] = C1^C2
        C[3] = C3 */\
    (C)[0]=(C)[1];\
    (C)[1]^=(C)[2];\
    (C)[2]^=(C)[3];\
    /*  C[0] = C0
        C[1] = C0^C1^C2
        C[2] = C1^C2^C3
        C[3] = C3 */\
    AA=(A)[0]^(A)[1];\
    BB=(B)[0]^(B)[1];\
    MUL64_NOINIT(C+1,AA,BB,i,tmp);

/* No initialize C */
#define MUL128_NOINIT(C,A,B,i,tmp,AA,BB) \
    MUL64(AA,(A)[0],(B)[0],i,tmp);\
    MUL64(BB,(A)[1],(B)[1],i,tmp);\
    /* C1^C2 */\
    (AA)[1]^=(BB)[0];\
    /* ^=C0 */\
    (C)[0]^=(AA)[0];\
    /* ^=C0^(C1^C2) */\
    (C)[1]^=(AA)[0]^(AA)[1];\
    /* ^=C3^(C1^C2) */\
    (C)[2]^=(BB)[1]^(AA)[1];\
    /* ^=C3 */\
    (C)[3]^=(BB)[1];\
    (AA)[0]^=(A)[1];\
    (BB)[0]^=(B)[1];\
    MUL64_NOINIT(C+1,(AA)[0],(BB)[0],i,tmp);

/* Karatsuba with 6 multiplications */
/* initialize C */
#define MUL192(C,A,B,i,tmp,AA,BB) \
    /* A0*B0 */\
    MUL64(C,(A)[0],(B)[0],i,tmp);\
    /* A2*B2 */\
    MUL64(C+4,(A)[2],(B)[2],i,tmp);\
    /* A1*B1 */\
    MUL64_NOINIT_SPLIT(C[1],C[4],(A)[1],(B)[1],i,tmp)\
    (C)[2]=(C)[4];\
    (C)[3]=(C)[1];\
    /*  C[0] = C0
        C[1] = C1^C2
        C[2] = C4^C3
        C[3] = C1^C2
        C[4] = C4^C3
        C[5] = C5 */\
    (C)[1]^=(C)[0];\
    (C)[4]^=(C)[5];\
    (C)[2]^=(C)[1];\
    (C)[3]^=(C)[4];\
    /*  C[0] = C0
        C[1] = C0^(C1^C2)
        C[2] = (C0^C1^C2)^(C3^C4)
        C[3] = (C1^C2)^(C3^C4^C5)
        C[4] = (C3^C4)^C5
        C[5] = C5 */\
    AA=(A)[0]^(A)[1];\
    BB=(B)[0]^(B)[1];\
    /* (A0+A1)*(B0+B1) */\
    MUL64_NOINIT(C+1,AA,BB,i,tmp);\
    AA=(A)[1]^(A)[2];\
    BB=(B)[1]^(B)[2];\
    /* (A1+A2)*(B1+B2) */\
    MUL64_NOINIT(C+3,AA,BB,i,tmp);\
    AA=(A)[0]^(A)[2];\
    BB=(B)[0]^(B)[2];\
    /* (A0+A2)*(B0+B2) */\
    MUL64_NOINIT(C+2,AA,BB,i,tmp);

/* Karatsuba */
/* initialize C */
#define MUL256(C,A,B,i,tmp,AA,BB,tmp1,tmp2) \
    MUL128(C,A,B,i,tmp,BB[0],BB[1]);\
    MUL128(C+4,A+2,B+2,i,tmp,AA[0],AA[1]);\
    C[2]^=C[4];\
    C[3]^=C[5];\
    C[4]=C[2];\
    C[5]=C[3];\
    C[2]^=C[0];\
    C[3]^=C[1];\
    C[4]^=C[6];\
    C[5]^=C[7];\
    AA[0]=A[0]^A[2];\
    AA[1]=A[1]^A[3];\
    BB[0]=B[0]^B[2];\
    BB[1]=B[1]^B[3];\
    MUL128_NOINIT(C+2,AA,BB,i,tmp,tmp1,tmp2);

/* Here it misses a code for MUL320, so use gf2x for this size */


/* Squaring: it is just to put a zero between each bit */

/* Precomputed table: square of elements of GF2X of 256 bits (0 to (2^8)-1) */
static const uint64_t tab_square[256]={0,1,4,5,16,17,20,21,64,65,68,69,80,81,84,85,256,257,260,261,272,273,276,277,320,321,324,325,336,337,340,341,1024,1025,1028,1029,1040,1041,1044,1045,1088,1089,1092,1093,1104,1105,1108,1109,1280,1281,1284,1285,1296,1297,1300,1301,1344,1345,1348,1349,1360,1361,1364,1365,4096,4097,4100,4101,4112,4113,4116,4117,4160,4161,4164,4165,4176,4177,4180,4181,4352,4353,4356,4357,4368,4369,4372,4373,4416,4417,4420,4421,4432,4433,4436,4437,5120,5121,5124,5125,5136,5137,5140,5141,5184,5185,5188,5189,5200,5201,5204,5205,5376,5377,5380,5381,5392,5393,5396,5397,5440,5441,5444,5445,5456,5457,5460,5461,16384,16385,16388,16389,16400,16401,16404,16405,16448,16449,16452,16453,16464,16465,16468,16469,16640,16641,16644,16645,16656,16657,16660,16661,16704,16705,16708,16709,16720,16721,16724,16725,17408,17409,17412,17413,17424,17425,17428,17429,17472,17473,17476,17477,17488,17489,17492,17493,17664,17665,17668,17669,17680,17681,17684,17685,17728,17729,17732,17733,17744,17745,17748,17749,20480,20481,20484,20485,20496,20497,20500,20501,20544,20545,20548,20549,20560,20561,20564,20565,20736,20737,20740,20741,20752,20753,20756,20757,20800,20801,20804,20805,20816,20817,20820,20821,21504,21505,21508,21509,21520,21521,21524,21525,21568,21569,21572,21573,21584,21585,21588,21589,21760,21761,21764,21765,21776,21777,21780,21781,21824,21825,21828,21829,21840,21841,21844,21845};


#define SQUARE32(C,A) \
    (C)=tab_square[(A)&255];\
    (C)^=tab_square[((A)>>8)&255]<<16;\
    (C)^=tab_square[((A)>>16)&255]<<32;\
    (C)^=tab_square[(A)>>24]<<48;

#define SQUARE64LOW_OLD(C,A,i) \
    (C)=(A)&1;\
    for(i=1;i<32;++i)\
    {\
        (C)^=((((A)>>i)&1)<<(i<<1));\
    }

#define SQUARE64LOW(C,A) \
    (C)=tab_square[(A)&255];\
    (C)^=tab_square[((A)>>8)&255]<<16;\
    (C)^=tab_square[((A)>>16)&255]<<32;\
    (C)^=tab_square[((A)>>24)&255]<<48;

#define SQUARE64_OLD(C,A,i) \
    (C)[0]=(A)&1;\
    for(i=1;i<32;++i)\
    {\
        (C)[0]^=((((A)>>i)&1)<<(i<<1));\
    }\
\
    (C)[1]=((A)>>32)&1;\
    for(i=33;i<64;++i)\
    {\
        (C)[1]^=((((A)>>i)&1)<<((i&31)<<1));\
    }

#define SQUARE64(C,A) \
    (C)[0]=tab_square[(A)&255];\
    (C)[0]^=tab_square[((A)>>8)&255]<<16;\
    (C)[0]^=tab_square[((A)>>16)&255]<<32;\
    (C)[0]^=tab_square[((A)>>24)&255]<<48;\
\
    (C)[1]=tab_square[((A)>>32)&255];\
    (C)[1]^=tab_square[((A)>>40)&255]<<16;\
    (C)[1]^=tab_square[((A)>>48)&255]<<32;\
    (C)[1]^=tab_square[(A)>>56]<<48;

#define SQUARE96(C,A) \
    SQUARE64(C,(A)[0]);\
    SQUARE32((C)[2],(A)[1]);

#define SQUARE128(C,A) \
    SQUARE64(C,(A)[0]);\
    SQUARE64(C+2,(A)[1]);

#define SQUARE160(C,A) \
    SQUARE128(C,A);\
    SQUARE32((C)[4],(A)[2]);

#define SQUARE192(C,A) \
    SQUARE128(C,A);\
    SQUARE64(C+4,(A)[2]);

#define SQUARE224(C,A) \
    SQUARE128(C,A);\
    SQUARE96(C+4,A+2);

#define SQUARE256(C,A) \
    SQUARE128(C,A);\
    SQUARE128(C+4,A+2);

#define SQUARE288(C,A) \
    SQUARE256(C,A);\
    SQUARE32((C)[8],(A)[4]);

#define SQUARE320(C,A) \
    SQUARE256(C,A);\
    SQUARE64(C+8,(A)[4]);

#define SQUARE352(C,A) \
    SQUARE256(C,A);\
    SQUARE96(C+8,A+4);

#define SQUARE384(C,A) \
    SQUARE256(C,A);\
    SQUARE128(C+8,A+4);


#endif


#endif
