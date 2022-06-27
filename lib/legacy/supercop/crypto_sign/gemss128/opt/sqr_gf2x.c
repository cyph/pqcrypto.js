#include "sqr_gf2x.h"

/* Functions to compute square in GF(2)[X] */


/***********************************************************************/
/***********************************************************************/
/**************************** pclmul version ***************************/
/***********************************************************************/
/***********************************************************************/



#ifdef ENABLED_PCLMUL

/***********************************************************************/
/***********************************************************************/
/********************** store at the end version ***********************/
/***********************************************************************/
/***********************************************************************/

uint64_t PREFIX_NAME(sqr64low)(uint64_t A) 
{
    __m128i x;
    INIT64(x,&A);

    return _mm_cvtsi128_si64x(_mm_clmulepi64_si128(x,x,0));
}

void PREFIX_NAME(sqr64)(uint64_t C[2], const uint64_t A[1]) 
{
    __m128i x,z;
    INIT64(x,A);

    SQUARE64_WS(z,x,0);

    STORE128(C,z);
}

void PREFIX_NAME(sqr96)(uint64_t C[3], const uint64_t A[2]) 
{
    __m128i x,z1,z2;
    INIT128(x,A);

    SQUARE128_WS(z1,z2,x);

    STORE128(C,z1);
    STORE64(C+2,z2);
}

void PREFIX_NAME(sqr128)(uint64_t C[4], const uint64_t A[2]) 
{
    __m128i x,z1,z2;
    INIT128(x,A);

    SQUARE128_WS(z1,z2,x);

    STORE128(C,z1);
    STORE128(C+2,z2);
}

void PREFIX_NAME(sqr160)(uint64_t C[5], const uint64_t A[3]) 
{
    __m128i x1,x2,z1,z2,z3;
    INIT128(x1,A);
    INIT64(x2,A+2);

    SQUARE192_WS(z1,z2,z3,x1,x2);

    STORE128(C,z1);
    STORE128(C+2,z2);
    STORE64(C+4,z3);
}

void PREFIX_NAME(sqr192)(uint64_t C[6], const uint64_t A[3]) 
{
    __m128i x1,x2,z1,z2,z3;
    INIT128(x1,A);
    INIT64(x2,A+2);

    SQUARE192_WS(z1,z2,z3,x1,x2);

    STORE128(C,z1);
    STORE128(C+2,z2);
    STORE128(C+4,z3);
}

void PREFIX_NAME(sqr224)(uint64_t C[7], const uint64_t A[4]) 
{
    __m128i x1,x2,z1,z2,z3,z4;
    INIT128(x1,A);
    INIT128(x2,A+2);

    SQUARE256_WS(z1,z2,z3,z4,x1,x2);

    STORE128(C,z1);
    STORE128(C+2,z2);
    STORE128(C+4,z3);
    STORE64(C+6,z4);
}

void PREFIX_NAME(sqr256)(uint64_t C[8], const uint64_t A[4]) 
{
    __m128i x1,x2,z1,z2,z3,z4;
    INIT128(x1,A);
    INIT128(x2,A+2);

    SQUARE256_WS(z1,z2,z3,z4,x1,x2);

    STORE128(C,z1);
    STORE128(C+2,z2);
    STORE128(C+4,z3);
    STORE128(C+6,z4);
}

void PREFIX_NAME(sqr288)(uint64_t C[9], const uint64_t A[5]) 
{
    __m128i x1,x2,x3,z1,z2,z3,z4,z5;
    INIT128(x1,A);
    INIT128(x2,A+2);
    INIT64(x3,A+4);

    SQUARE320_WS(z1,z2,z3,z4,z5,x1,x2,x3);

    STORE128(C,z1);
    STORE128(C+2,z2);
    STORE128(C+4,z3);
    STORE128(C+6,z4);
    STORE64(C+8,z5);
}

void PREFIX_NAME(sqr320)(uint64_t C[10], const uint64_t A[5]) 
{
    __m128i x1,x2,x3,z1,z2,z3,z4,z5;
    INIT128(x1,A);
    INIT128(x2,A+2);
    INIT64(x3,A+4);

    SQUARE320_WS(z1,z2,z3,z4,z5,x1,x2,x3);

    STORE128(C,z1);
    STORE128(C+2,z2);
    STORE128(C+4,z3);
    STORE128(C+6,z4);
    STORE128(C+8,z5);
}

void PREFIX_NAME(sqr352)(uint64_t C[11], const uint64_t A[6]) 
{
    __m128i x1,x2,x3,z1,z2,z3,z4,z5,z6;
    INIT128(x1,A);
    INIT128(x2,A+2);
    INIT128(x3,A+4);

    SQUARE384_WS(z1,z2,z3,z4,z5,z6,x1,x2,x3);

    STORE128(C,z1);
    STORE128(C+2,z2);
    STORE128(C+4,z3);
    STORE128(C+6,z4);
    STORE128(C+8,z5);
    STORE64(C+10,z6);
}

void PREFIX_NAME(sqr384)(uint64_t C[12], const uint64_t A[6]) 
{
    __m128i x1,x2,x3,z1,z2,z3,z4,z5,z6;
    INIT128(x1,A);
    INIT128(x2,A+2);
    INIT128(x3,A+4);

    SQUARE384_WS(z1,z2,z3,z4,z5,z6,x1,x2,x3);

    STORE128(C,z1);
    STORE128(C+2,z2);
    STORE128(C+4,z3);
    STORE128(C+6,z4);
    STORE128(C+8,z5);
    STORE128(C+10,z6);
}

void PREFIX_NAME(sqr416)(uint64_t C[13], const uint64_t A[7]) 
{
    __m128i x1,x2,x3,x4,z1,z2,z3,z4,z5,z6,z7;
    INIT128(x1,A);
    INIT128(x2,A+2);
    INIT128(x3,A+4);
    INIT64(x4,A+6);

    SQUARE448_WS(z1,z2,z3,z4,z5,z6,z7,x1,x2,x3,x4);

    STORE128(C,z1);
    STORE128(C+2,z2);
    STORE128(C+4,z3);
    STORE128(C+6,z4);
    STORE128(C+8,z5);
    STORE128(C+10,z6);
    STORE64(C+12,z7);
}

void PREFIX_NAME(sqr448)(uint64_t C[14], const uint64_t A[7]) 
{
    __m128i x1,x2,x3,x4,z1,z2,z3,z4,z5,z6,z7;
    INIT128(x1,A);
    INIT128(x2,A+2);
    INIT128(x3,A+4);
    INIT64(x4,A+6);

    SQUARE448_WS(z1,z2,z3,z4,z5,z6,z7,x1,x2,x3,x4);

    STORE128(C,z1);
    STORE128(C+2,z2);
    STORE128(C+4,z3);
    STORE128(C+6,z4);
    STORE128(C+8,z5);
    STORE128(C+10,z6);
    STORE128(C+12,z7);
}

void PREFIX_NAME(sqr480)(uint64_t C[15], const uint64_t A[8]) 
{
    __m128i x1,x2,x3,x4,z1,z2,z3,z4,z5,z6,z7,z8;
    INIT128(x1,A);
    INIT128(x2,A+2);
    INIT128(x3,A+4);
    INIT128(x4,A+6);

    SQUARE512_WS(z1,z2,z3,z4,z5,z6,z7,z8,x1,x2,x3,x4);

    STORE128(C,z1);
    STORE128(C+2,z2);
    STORE128(C+4,z3);
    STORE128(C+6,z4);
    STORE128(C+8,z5);
    STORE128(C+10,z6);
    STORE128(C+12,z7);
    STORE64(C+14,z8);
}

void PREFIX_NAME(sqr512)(uint64_t C[16], const uint64_t A[8]) 
{
    __m128i x1,x2,x3,x4,z1,z2,z3,z4,z5,z6,z7,z8;
    INIT128(x1,A);
    INIT128(x2,A+2);
    INIT128(x3,A+4);
    INIT128(x4,A+6);

    SQUARE512_WS(z1,z2,z3,z4,z5,z6,z7,z8,x1,x2,x3,x4);

    STORE128(C,z1);
    STORE128(C+2,z2);
    STORE128(C+4,z3);
    STORE128(C+6,z4);
    STORE128(C+8,z5);
    STORE128(C+10,z6);
    STORE128(C+12,z7);
    STORE128(C+14,z8);
}

void PREFIX_NAME(sqr544)(uint64_t C[17], const uint64_t A[9]) 
{
    __m128i x1,x2,x3,x4,x5,z1,z2,z3,z4,z5,z6,z7,z8,z9;
    INIT128(x1,A);
    INIT128(x2,A+2);
    INIT128(x3,A+4);
    INIT128(x4,A+6);
    INIT64(x5,A+8);

    SQUARE576_WS(z1,z2,z3,z4,z5,z6,z7,z8,z9,x1,x2,x3,x4,x5);

    STORE128(C,z1);
    STORE128(C+2,z2);
    STORE128(C+4,z3);
    STORE128(C+6,z4);
    STORE128(C+8,z5);
    STORE128(C+10,z6);
    STORE128(C+12,z7);
    STORE128(C+14,z8);
    STORE64(C+16,z9);
}

void PREFIX_NAME(sqr576)(uint64_t C[18], const uint64_t A[9]) 
{
    __m128i x1,x2,x3,x4,x5,z1,z2,z3,z4,z5,z6,z7,z8,z9;
    INIT128(x1,A);
    INIT128(x2,A+2);
    INIT128(x3,A+4);
    INIT128(x4,A+6);
    INIT64(x5,A+8);

    SQUARE576_WS(z1,z2,z3,z4,z5,z6,z7,z8,z9,x1,x2,x3,x4,x5);

    STORE128(C,z1);
    STORE128(C+2,z2);
    STORE128(C+4,z3);
    STORE128(C+6,z4);
    STORE128(C+8,z5);
    STORE128(C+10,z6);
    STORE128(C+12,z7);
    STORE128(C+14,z8);
    STORE128(C+16,z9);
}


/***********************************************************************/
/***********************************************************************/
/********************** square and store version ***********************/
/***********************************************************************/
/***********************************************************************/


uint64_t PREFIX_NAME(old_square64low)(uint64_t A) 
{
    __m128i x;
    INIT64(x,&A);
    return SQR64LOW(x,0);
}

void PREFIX_NAME(old_square64)(uint64_t C[2], const uint64_t A[1]) 
{
    __m128i x,z;
    INIT64(x,A);
    SQR64(C,x,z,0)
}

void PREFIX_NAME(old_square96)(uint64_t C[3], const uint64_t A[2]) 
{
    __m128i x,z;
    INIT128(x,A);
    SQR96(C,x,z);
}

void PREFIX_NAME(old_square128)(uint64_t C[4], const uint64_t A[2]) 
{
    __m128i x,z;
    INIT128(x,A);
    SQR128(C,x,z);
}

void PREFIX_NAME(old_square160)(uint64_t C[5], const uint64_t A[3]) 
{
    __m128i x1,x2,z;
    INIT192(x1,x2,A);
    SQR160(C,x1,x2,z);
}

void PREFIX_NAME(old_square192)(uint64_t C[6], const uint64_t A[3]) 
{
    __m128i x1,x2,z;
    INIT192(x1,x2,A);
    SQR192(C,x1,x2,z);
}

void PREFIX_NAME(old_square224)(uint64_t C[7], const uint64_t A[4]) 
{
    __m128i x1,x2,z;
    INIT256(x1,x2,A);
    SQR224(C,x1,x2,z);
}

void PREFIX_NAME(old_square256)(uint64_t C[8], const uint64_t A[4]) 
{
    __m128i x1,x2,z;
    INIT256(x1,x2,A);
    SQR256(C,x1,x2,z);
}

void PREFIX_NAME(old_square288)(uint64_t C[9], const uint64_t A[5]) 
{
    __m128i x1,x2,x3,z;
    INIT320(x1,x2,x3,A);
    SQR288(C,x1,x2,x3,z);
}

void PREFIX_NAME(old_square320)(uint64_t C[10], const uint64_t A[5]) 
{
    __m128i x1,x2,x3,z;
    INIT320(x1,x2,x3,A);
    SQR320(C,x1,x2,x3,z);
}

void PREFIX_NAME(old_square352)(uint64_t C[11], const uint64_t A[6]) 
{
    __m128i x1,x2,x3,z;
    INIT384(x1,x2,x3,A);
    SQR352(C,x1,x2,x3,z);
}

void PREFIX_NAME(old_square384)(uint64_t C[12], const uint64_t A[6]) 
{
    __m128i x1,x2,x3,z;
    INIT384(x1,x2,x3,A);
    SQR384(C,x1,x2,x3,z);
}

void PREFIX_NAME(old_square416)(uint64_t C[13], const uint64_t A[7]) 
{
    __m128i x1,x2,x3,x4,z;
    INIT448(x1,x2,x3,x4,A);
    SQR416(C,x1,x2,x3,x4,z);
}

void PREFIX_NAME(old_square448)(uint64_t C[14], const uint64_t A[7]) 
{
    __m128i x1,x2,x3,x4,z;
    INIT448(x1,x2,x3,x4,A);
    SQR448(C,x1,x2,x3,x4,z);
}

void PREFIX_NAME(old_square480)(uint64_t C[15], const uint64_t A[8]) 
{
    __m128i x1,x2,x3,x4,z;
    INIT512(x1,x2,x3,x4,A);
    SQR480(C,x1,x2,x3,x4,z);
}

void PREFIX_NAME(old_square512)(uint64_t C[16], const uint64_t A[8]) 
{
    __m128i x1,x2,x3,x4,z;
    INIT512(x1,x2,x3,x4,A);
    SQR512(C,x1,x2,x3,x4,z);
}

void PREFIX_NAME(old_square544)(uint64_t C[17], const uint64_t A[9]) 
{
    __m128i x1,x2,x3,x4,x5,z;
    INIT576(x1,x2,x3,x4,x5,A);
    SQR544(C,x1,x2,x3,x4,x5,z);
}

void PREFIX_NAME(old_square576)(uint64_t C[18], const uint64_t A[9]) 
{
    __m128i x1,x2,x3,x4,x5,z;
    INIT576(x1,x2,x3,x4,x5,A);
    SQR576(C,x1,x2,x3,x4,x5,z);
}

#endif



/***********************************************************************/
/***********************************************************************/
/********************** version without vectorization ******************/
/***********************************************************************/
/***********************************************************************/

#ifndef ENABLED_PCLMUL
uint64_t PREFIX_NAME(square64low)(uint64_t A) 
{
    uint64_t C;
    SQUARE64LOW(C,A);
    return C;
}
#endif



