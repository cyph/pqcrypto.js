#ifndef _SQR_GF2X_H
#define _SQR_GF2X_H

#include "prefix_name.h"
#include <stdint.h>
#include "arch.h"
#include "simd_intel.h"

/* Squaring in GF(2)[X] */
/* (a+b*X^64)^2 = a^2 + (b^2)*X^128 */



/* To choose only one macro and function:
    macro: SQUARE
    function: square
*/
#define square32 square64low
#ifdef ENABLED_PCLMUL
    #define SQUARE32 SQR32
    #define SQUARE64low SQR64low
    #define SQUARE64 SQR64
    #define SQUARE96 SQR96
    #define SQUARE128 SQR128
    #define SQUARE160 SQR160
    #define SQUARE192 SQR192
    #define SQUARE224 SQR224
    #define SQUARE256 SQR256
    #define SQUARE288 SQR288
    #define SQUARE320 SQR320
    #define SQUARE352 SQR352
    #define SQUARE384 SQR384
    #define SQUARE416 SQR416
    #define SQUARE448 SQR448
    #define SQUARE480 SQR480
    #define SQUARE512 SQR512
    #define SQUARE576 SQR576

    #define square64low sqr64low
    #define square64 sqr64
    #define square96 sqr96
    #define square128 sqr128
    #define square160 sqr160
    #define square192 sqr192
    #define square224 sqr224
    #define square256 sqr256
    #define square288 sqr288
    #define square320 sqr320
    #define square352 sqr352
    #define square384 sqr384
    #define square416 sqr416
    #define square448 sqr448
    #define square480 sqr480
    #define square512 sqr512
    #define square576 sqr576
#else
    #define SQUARE32 SQUARE32_NO_VEC
    #define SQUARE64LOW SQUARE64LOW_NO_VEC
    #define SQUARE64 SQUARE64_NO_VEC
    #define SQUARE96 SQUARE96_NO_VEC
    #define SQUARE128 SQUARE128_NO_VEC
    #define SQUARE160 SQUARE160_NO_VEC
    #define SQUARE192 SQUARE192_NO_VEC
    #define SQUARE224 SQUARE224_NO_VEC
    #define SQUARE256 SQUARE256_NO_VEC
    #define SQUARE288 SQUARE288_NO_VEC
    #define SQUARE320 SQUARE320_NO_VEC
    #define SQUARE352 SQUARE352_NO_VEC
    #define SQUARE384 SQUARE384_NO_VEC
    #define SQUARE416 SQUARE416_NO_VEC
    #define SQUARE448 SQUARE448_NO_VEC
    #define SQUARE480 SQUARE480_NO_VEC
    #define SQUARE512 SQUARE512_NO_VEC
    #define SQUARE576 SQUARE576_NO_VEC

    uint64_t PREFIX_NAME(square64low)(uint64_t A);
    #define square64low PREFIX_NAME(square64low)

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
    #define square416 SQUARE416
    #define square448 SQUARE448
    #define square480 SQUARE480
    #define square512 SQUARE512
    #define square576 SQUARE576
#endif



/***********************************************************************/
/***********************************************************************/
/**************************** pclmul version ***************************/
/***********************************************************************/
/***********************************************************************/



#ifdef ENABLED_PCLMUL
uint64_t PREFIX_NAME(sqr64low)(uint64_t A);
#define sqr64_inline(C,A) STORE128(C,_mm_clmulepi64_si128(LOAD64(A),LOAD64(A),0))
void PREFIX_NAME(sqr64)(uint64_t C[2], const uint64_t A[1]);
void PREFIX_NAME(sqr96)(uint64_t C[3], const uint64_t A[2]);
void PREFIX_NAME(sqr128)(uint64_t C[4], const uint64_t A[2]);
void PREFIX_NAME(sqr160)(uint64_t C[5], const uint64_t A[3]);
void PREFIX_NAME(sqr192)(uint64_t C[6], const uint64_t A[3]);
void PREFIX_NAME(sqr224)(uint64_t C[7], const uint64_t A[4]);
void PREFIX_NAME(sqr256)(uint64_t C[8], const uint64_t A[4]);
void PREFIX_NAME(sqr288)(uint64_t C[9], const uint64_t A[5]);
void PREFIX_NAME(sqr320)(uint64_t C[10], const uint64_t A[5]);
void PREFIX_NAME(sqr352)(uint64_t C[11], const uint64_t A[6]);
void PREFIX_NAME(sqr384)(uint64_t C[12], const uint64_t A[6]);
void PREFIX_NAME(sqr416)(uint64_t C[13], const uint64_t A[7]);
void PREFIX_NAME(sqr448)(uint64_t C[14], const uint64_t A[7]);
void PREFIX_NAME(sqr480)(uint64_t C[15], const uint64_t A[8]);
void PREFIX_NAME(sqr512)(uint64_t C[16], const uint64_t A[8]);
void PREFIX_NAME(sqr576)(uint64_t C[18], const uint64_t A[9]);


#define sqr64low PREFIX_NAME(sqr64low)
#define sqr64 PREFIX_NAME(sqr64)
#define sqr128 PREFIX_NAME(sqr128)
#define sqr192 PREFIX_NAME(sqr192)
#define sqr256 PREFIX_NAME(sqr256)
#define sqr320 PREFIX_NAME(sqr320)
#define sqr384 PREFIX_NAME(sqr384)
#define sqr448 PREFIX_NAME(sqr448)
#define sqr512 PREFIX_NAME(sqr512)
#define sqr576 PREFIX_NAME(sqr576)

#define sqr96 PREFIX_NAME(sqr96)
#define sqr160 PREFIX_NAME(sqr160)
#define sqr224 PREFIX_NAME(sqr224)
#define sqr288 PREFIX_NAME(sqr288)
#define sqr352 PREFIX_NAME(sqr352)
#define sqr416 PREFIX_NAME(sqr416)
#define sqr480 PREFIX_NAME(sqr480)
#define sqr544 PREFIX_NAME(sqr544)


uint64_t PREFIX_NAME(old_square64low)(uint64_t A);
void PREFIX_NAME(old_square64)(uint64_t C[2], const uint64_t A[1]);
void PREFIX_NAME(old_square96)(uint64_t C[3], const uint64_t A[2]);
void PREFIX_NAME(old_square128)(uint64_t C[4], const uint64_t A[2]);
void PREFIX_NAME(old_square160)(uint64_t C[5], const uint64_t A[3]);
void PREFIX_NAME(old_square192)(uint64_t C[6], const uint64_t A[3]);
void PREFIX_NAME(old_square224)(uint64_t C[7], const uint64_t A[4]);
void PREFIX_NAME(old_square256)(uint64_t C[8], const uint64_t A[4]);
void PREFIX_NAME(old_square288)(uint64_t C[9], const uint64_t A[5]);
void PREFIX_NAME(old_square320)(uint64_t C[10], const uint64_t A[5]);
void PREFIX_NAME(old_square352)(uint64_t C[11], const uint64_t A[6]);
void PREFIX_NAME(old_square384)(uint64_t C[12], const uint64_t A[6]);
void PREFIX_NAME(old_square416)(uint64_t C[13], const uint64_t A[7]);
void PREFIX_NAME(old_square448)(uint64_t C[14], const uint64_t A[7]);
void PREFIX_NAME(old_square480)(uint64_t C[15], const uint64_t A[8]);
void PREFIX_NAME(old_square512)(uint64_t C[16], const uint64_t A[8]);
void PREFIX_NAME(old_square576)(uint64_t C[18], const uint64_t A[9]);


#define old_square64low PREFIX_NAME(old_square64low)
#define old_square64 PREFIX_NAME(old_square64)
#define old_square128 PREFIX_NAME(old_square128)
#define old_square192 PREFIX_NAME(old_square192)
#define old_square256 PREFIX_NAME(old_square256)
#define old_square320 PREFIX_NAME(old_square320)
#define old_square384 PREFIX_NAME(old_square384)
#define old_square448 PREFIX_NAME(old_square448)
#define old_square512 PREFIX_NAME(old_square512)
#define old_square576 PREFIX_NAME(old_square576)

#define old_square96 PREFIX_NAME(old_square96)
#define old_square160 PREFIX_NAME(old_square160)
#define old_square224 PREFIX_NAME(old_square224)
#define old_square288 PREFIX_NAME(old_square288)
#define old_square352 PREFIX_NAME(old_square352)
#define old_square416 PREFIX_NAME(old_square416)
#define old_square480 PREFIX_NAME(old_square480)
#define old_square544 PREFIX_NAME(old_square544)


/***********************************************************************/
/***********************************************************************/
/********************** store at the end version ***********************/
/***********************************************************************/
/***********************************************************************/


/* WS: without store */
/* 1 mul64 */
#define SQUARE64_WS(z,x,pos) \
    z=_mm_clmulepi64_si128(x,x,pos);

/* 2 mul64 */
#define SQUARE128_WS(z1,z2,x) \
    SQUARE64_WS(z1,x,0);\
    SQUARE64_WS(z2,x,17);

/* 3 mul64 */
#define SQUARE192_WS(z1,z2,z3,x1,x2) \
    SQUARE128_WS(z1,z2,x1);\
    SQUARE64_WS(z3,x2,0);

/* 4 mul64 */
#define SQUARE256_WS(z1,z2,z3,z4,x1,x2) \
    SQUARE128_WS(z1,z2,x1);\
    SQUARE128_WS(z3,z4,x2);

/* 5 mul64 */
#define SQUARE320_WS(z1,z2,z3,z4,z5,x1,x2,x3) \
    SQUARE256_WS(z1,z2,z3,z4,x1,x2);\
    SQUARE64_WS(z5,x3,0);

/* 6 mul64 */
#define SQUARE384_WS(z1,z2,z3,z4,z5,z6,x1,x2,x3) \
    SQUARE256_WS(z1,z2,z3,z4,x1,x2);\
    SQUARE128_WS(z5,z6,x3);

/* 7 mul64 */
#define SQUARE448_WS(z1,z2,z3,z4,z5,z6,z7,x1,x2,x3,x4) \
    SQUARE256_WS(z1,z2,z3,z4,x1,x2);\
    SQUARE192_WS(z5,z6,z7,x3,x4);

/* 8 mul64 */
#define SQUARE512_WS(z1,z2,z3,z4,z5,z6,z7,z8,x1,x2,x3,x4) \
    SQUARE256_WS(z1,z2,z3,z4,x1,x2);\
    SQUARE256_WS(z5,z6,z7,z8,x3,x4);

/* 9 mul64 */
#define SQUARE576_WS(z1,z2,z3,z4,z5,z6,z7,z8,z9,x1,x2,x3,x4,x5) \
    SQUARE512_WS(z1,z2,z3,z4,z5,z6,z7,z8,x1,x2,x3,x4);\
    SQUARE64_WS(z9,x5,0);


/***********************************************************************/
/***********************************************************************/
/********************** square and store version ***********************/
/***********************************************************************/
/***********************************************************************/


/* 1 mul64 */
#define SQR64LOW(x,pos) _mm_cvtsi128_si64x(_mm_clmulepi64_si128(x, x, pos))
#define SQR64LOW_TAB(C,x,pos) STORE64(C,_mm_clmulepi64_si128(x, x, pos))

#define SQR32(x) SQR64LOW(x,0)

#define SQR64(C,x,z,pos) \
    SQUARE64_WS(z,x,pos); \
    STORE128(C,z);

/* 2 mul64 */
#define SQR96(C,x,z) \
    SQR64(C,x,z,0);\
    SQR64LOW_TAB(C+2,x,17);

#define SQR128(C,x,z) \
    SQR64(C,x,z,0);\
    SQR64(C+2,x,z,17);

/* 3 mul64 */
#define SQR160(C,x1,x2,z) \
    SQR128(C,x1,z);\
    SQR64LOW_TAB(C+4,x2,0);

#define SQR192(C,x1,x2,z) \
    SQR128(C,x1,z);\
    SQR64(C+4,x2,z,0);

/* 4 mul64 */
#define SQR224(C,x1,x2,z) \
    SQR128(C,x1,z);\
    SQR96(C+4,x2,z);

#define SQR256(C,x1,x2,z) \
    SQR128(C,x1,z);\
    SQR128(C+4,x2,z);

/* 5 mul64 */
#define SQR288(C,x1,x2,x3,z) \
    SQR256(C,x1,x2,z);\
    SQR64LOW_TAB(C+8,x3,0);

#define SQR320(C,x1,x2,x3,z) \
    SQR256(C,x1,x2,z);\
    SQR64(C+8,x3,z,0);

/* 6 mul64 */
#define SQR352(C,x1,x2,x3,z) \
    SQR256(C,x1,x2,z);\
    SQR96(C+8,x3,z);

#define SQR384(C,x1,x2,x3,z) \
    SQR256(C,x1,x2,z);\
    SQR128(C+8,x3,z);

/* 7 mul64 */
#define SQR416(C,x1,x2,x3,x4,z) \
    SQR256(C,x1,x2,z);\
    SQR160(C+8,x3,x4,z);

#define SQR448(C,x1,x2,x3,x4,z) \
    SQR256(C,x1,x2,z);\
    SQR192(C+8,x3,x4,z);

/* 8 mul64 */
#define SQR480(C,x1,x2,x3,x4,z) \
    SQR256(C,x1,x2,z);\
    SQR224(C+8,x3,x4,z);

#define SQR512(C,x1,x2,x3,x4,z) \
    SQR256(C,x1,x2,z);\
    SQR256(C+8,x3,x4,z);

/* 9 mul64 */
#define SQR544(C,x1,x2,x3,x4,x5,z) \
    SQR512(C,x1,x2,x3,x4,z);\
    SQR64LOW_TAB(C+16,x5,0);

#define SQR576(C,x1,x2,x3,x4,x5,z) \
    SQR512(C,x1,x2,x3,x4,z);\
    SQR64(C+16,x5,z,0);


#endif



/***********************************************************************/
/***********************************************************************/
/********************** version without vectorization ******************/
/***********************************************************************/
/***********************************************************************/

/* Define this variable to have constant time squaring (but it is slower) */
#define SQR_CST_TIME


#ifdef SQR_CST_TIME
    #define SQUARE32_NO_VEC SQUARE32_CST_TIME
    #define SQUARE64LOW_NO_VEC SQUARE64LOW_CST_TIME
    #define SQUARE64_NO_VEC SQUARE64_CST_TIME
#else
    #define SQUARE32_NO_VEC SQUARE32_NO_CST_TIME
    #define SQUARE64LOW_NO_VEC SQUARE64LOW_NO_CST_TIME
    #define SQUARE64_NO_VEC SQUARE64_NO_CST_TIME
#endif



#define SQUARE64LOW_CST_TIME(C,A) \
    { unsigned int i;\
      uint64_t A0; \
    A0=A;\
    (C)=A0&1;\
    A0>>=1;\
    (C)^=(A0&1)<<2;\
    for(i=4;i<64;i+=2)\
    {\
        A0>>=1;\
        (C)^=(A0&1)<<i;\
        i+=2;\
        A0>>=1;\
        (C)^=(A0&1)<<i;\
    }\
    }

#define SQUARE32_CST_TIME SQUARE64LOW_CST_TIME

#define SQUARE64_CST_TIME(C,A) \
    { unsigned int i;\
      uint64_t A0; \
    A0=A;\
    (C)[0]=A0&1;\
    A0>>=1;\
    (C)[0]^=(A0&1)<<2;\
    for(i=4;i<64;i+=2)\
    {\
        A0>>=1;\
        (C)[0]^=(A0&1)<<i;\
        i+=2;\
        A0>>=1;\
        (C)[0]^=(A0&1)<<i;\
    }\
\
    A0>>=1;\
    (C)[1]=A0&1;\
    A0>>=1;\
    (C)[1]^=(A0&1)<<2;\
    for(i=4;i<64;i+=2)\
    {\
        A0>>=1;\
        (C)[1]^=(A0&1)<<i;\
        i+=2;\
        A0>>=1;\
        (C)[1]^=(A0&1)<<i;\
    }\
    }



/* XXX This version is not in constant time! XXX */
#ifndef SQR_CST_TIME

/* Squaring: it is just to put a zero between each bit */

/* Precomputed table: square of elements of GF2X of 256 bits (0 to (2^8)-1) */
static const uint64_t tab_square[256]={0,1,4,5,16,17,20,21,64,65,68,69,80,81,84,85,256,257,260,261,272,273,276,277,320,321,324,325,336,337,340,341,1024,1025,1028,1029,1040,1041,1044,1045,1088,1089,1092,1093,1104,1105,1108,1109,1280,1281,1284,1285,1296,1297,1300,1301,1344,1345,1348,1349,1360,1361,1364,1365,4096,4097,4100,4101,4112,4113,4116,4117,4160,4161,4164,4165,4176,4177,4180,4181,4352,4353,4356,4357,4368,4369,4372,4373,4416,4417,4420,4421,4432,4433,4436,4437,5120,5121,5124,5125,5136,5137,5140,5141,5184,5185,5188,5189,5200,5201,5204,5205,5376,5377,5380,5381,5392,5393,5396,5397,5440,5441,5444,5445,5456,5457,5460,5461,16384,16385,16388,16389,16400,16401,16404,16405,16448,16449,16452,16453,16464,16465,16468,16469,16640,16641,16644,16645,16656,16657,16660,16661,16704,16705,16708,16709,16720,16721,16724,16725,17408,17409,17412,17413,17424,17425,17428,17429,17472,17473,17476,17477,17488,17489,17492,17493,17664,17665,17668,17669,17680,17681,17684,17685,17728,17729,17732,17733,17744,17745,17748,17749,20480,20481,20484,20485,20496,20497,20500,20501,20544,20545,20548,20549,20560,20561,20564,20565,20736,20737,20740,20741,20752,20753,20756,20757,20800,20801,20804,20805,20816,20817,20820,20821,21504,21505,21508,21509,21520,21521,21524,21525,21568,21569,21572,21573,21584,21585,21588,21589,21760,21761,21764,21765,21776,21777,21780,21781,21824,21825,21828,21829,21840,21841,21844,21845};


#define SQUARE32_NO_CST_TIME(C,A) \
    (C)=tab_square[(A)&255];\
    (C)^=tab_square[((A)>>8)&255]<<16;\
    (C)^=tab_square[((A)>>16)&255]<<32;\
    (C)^=tab_square[(A)>>24]<<48;

#define SQUARE64LOW_NO_CST_TIME(C,A) \
    (C)=tab_square[(A)&255];\
    (C)^=tab_square[((A)>>8)&255]<<16;\
    (C)^=tab_square[((A)>>16)&255]<<32;\
    (C)^=tab_square[((A)>>24)&255]<<48;

#define SQUARE64_NO_CST_TIME(C,A) \
    (C)[0]=tab_square[(A)&255];\
    (C)[0]^=tab_square[((A)>>8)&255]<<16;\
    (C)[0]^=tab_square[((A)>>16)&255]<<32;\
    (C)[0]^=tab_square[((A)>>24)&255]<<48;\
\
    (C)[1]=tab_square[((A)>>32)&255];\
    (C)[1]^=tab_square[((A)>>40)&255]<<16;\
    (C)[1]^=tab_square[((A)>>48)&255]<<32;\
    (C)[1]^=tab_square[(A)>>56]<<48;

#endif



#define SQUARE96_NO_VEC(C,A) \
    SQUARE64_NO_VEC(C,(A)[0]);\
    SQUARE32_NO_VEC((C)[2],(A)[1]);

#define SQUARE128_NO_VEC(C,A) \
    SQUARE64_NO_VEC(C,(A)[0]);\
    SQUARE64_NO_VEC(C+2,(A)[1]);

#define SQUARE160_NO_VEC(C,A) \
    SQUARE128_NO_VEC(C,A);\
    SQUARE32_NO_VEC((C)[4],(A)[2]);

#define SQUARE192_NO_VEC(C,A) \
    SQUARE128_NO_VEC(C,A);\
    SQUARE64_NO_VEC(C+4,(A)[2]);

#define SQUARE224_NO_VEC(C,A) \
    SQUARE128_NO_VEC(C,A);\
    SQUARE96_NO_VEC(C+4,A+2);

#define SQUARE256_NO_VEC(C,A) \
    SQUARE128_NO_VEC(C,A);\
    SQUARE128_NO_VEC(C+4,A+2);

#define SQUARE288_NO_VEC(C,A) \
    SQUARE256_NO_VEC(C,A);\
    SQUARE32_NO_VEC((C)[8],(A)[4]);

#define SQUARE320_NO_VEC(C,A) \
    SQUARE256_NO_VEC(C,A);\
    SQUARE64_NO_VEC(C+8,(A)[4]);

#define SQUARE352_NO_VEC(C,A) \
    SQUARE256_NO_VEC(C,A);\
    SQUARE96_NO_VEC(C+8,A+4);

#define SQUARE384_NO_VEC(C,A) \
    SQUARE256_NO_VEC(C,A);\
    SQUARE128_NO_VEC(C+8,A+4);

#define SQUARE416_NO_VEC(C,A) \
    SQUARE256_NO_VEC(C,A);\
    SQUARE160_NO_VEC(C+8,A+4);

#define SQUARE448_NO_VEC(C,A) \
    SQUARE256_NO_VEC(C,A);\
    SQUARE192_NO_VEC(C+8,A+4);

#define SQUARE480_NO_VEC(C,A) \
    SQUARE256_NO_VEC(C,A);\
    SQUARE224_NO_VEC(C+8,A+4);

#define SQUARE512_NO_VEC(C,A) \
    SQUARE256_NO_VEC(C,A);\
    SQUARE256_NO_VEC(C+8,A+4);

#define SQUARE544_NO_VEC(C,A) \
    SQUARE512_NO_VEC(C,A);\
    SQUARE32_NO_VEC((C)[16],(A)[8]);

#define SQUARE576_NO_VEC(C,A) \
    SQUARE512_NO_VEC(C,A);\
    SQUARE64_NO_VEC(C+16,A+8);




#endif
