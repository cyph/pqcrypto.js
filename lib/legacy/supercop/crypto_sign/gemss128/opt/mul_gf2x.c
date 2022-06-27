#include "mul_gf2x.h"


#ifdef ENABLED_PCLMUL

#include "init.h"

uint64_t PREFIX_NAME(mul64low)(uint64_t A, uint64_t B) 
{
    __m128i x,y;
    INIT64(x,&A);
    INIT64(y,&B);
    return MUL64LOW(x,y,0);
}

#if !ENABLED_GF2X


/* This file owns mul128_Karat and mul256_Karat */
/* But the implementation is slower than classical mutiplication */


/* C=A*B or C=A*A for each function */

void PREFIX_NAME(mul64)(uint64_t C[2], const uint64_t A[1], const uint64_t B[1]) 
{
    __m128i x,y,z;
    INIT64(x,A);
    INIT64(y,B);
    MUL64(C,x,y,z,0);
}

void PREFIX_NAME(mul96)(uint64_t C[3], const uint64_t A[2], const uint64_t B[2])
{
    __m128i x,y,z1,z2,sum,res_low,res_high;
    INIT128(x,A);
    INIT128(y,B);
    MUL96(C,x,y,z1,z2,sum,res_low,res_high);
}

void PREFIX_NAME(mul128)(uint64_t C[4], const uint64_t A[2], const uint64_t B[2]) 
{
    __m128i x,y,z1,z2,sum,res_low,res_high;
    INIT128(x,A);
    INIT128(y,B);
    MUL128(C,x,y,z1,z2,sum,res_low,res_high);
}

void PREFIX_NAME(mul160)(uint64_t C[5], const uint64_t A[3], const uint64_t B[3]) 
{
    __m128i x1,x2,y1,y2,z1,z2,sum,res1,res2;
    INIT192(x1,x2,A);
    INIT192(y1,y2,B);
    MUL160(C,x1,x2,y1,y2,z1,z2,sum,res1,res2);
}

void PREFIX_NAME(mul192)(uint64_t C[6], const uint64_t A[3], const uint64_t B[3]) 
{
    __m128i x1,x2,y1,y2,z1,z2,sum,res1,res2;
    INIT192(x1,x2,A);
    INIT192(y1,y2,B);
    MUL192(C,x1,x2,y1,y2,z1,z2,sum,res1,res2);
}

void PREFIX_NAME(mul224)(uint64_t C[7], const uint64_t A[4], const uint64_t B[4]) 
{
    __m128i x1,x2,y1,y2,z1,z2,sum,res1,res2;
    INIT256(x1,x2,A);
    INIT256(y1,y2,B);
    MUL224(C,x1,x2,y1,y2,z1,z2,sum,res1,res2);
}

void PREFIX_NAME(mul256)(uint64_t C[8], const uint64_t A[4], const uint64_t B[4]) 
{
    __m128i x1,x2,y1,y2,z1,z2,sum,res1,res2;
    INIT256(x1,x2,A);
    INIT256(y1,y2,B);
    MUL256(C,x1,x2,y1,y2,z1,z2,sum,res1,res2);
}

void PREFIX_NAME(mul288)(uint64_t C[9], const uint64_t A[5], const uint64_t B[5]) 
{
    __m128i x1,x2,x3,y1,y2,y3,z1,z2,sum,res1,res2;
    INIT320(x1,x2,x3,A);
    INIT320(y1,y2,y3,B);
    MUL288(C,x1,x2,x3,y1,y2,y3,z1,z2,sum,res1,res2);
}

void PREFIX_NAME(mul320)(uint64_t C[10], const uint64_t A[5], const uint64_t B[5]) 
{
    __m128i x1,x2,x3,y1,y2,y3,z1,z2,sum,res1,res2;
    INIT320(x1,x2,x3,A);
    INIT320(y1,y2,y3,B);
    MUL320(C,x1,x2,x3,y1,y2,y3,z1,z2,sum,res1,res2);
}

void PREFIX_NAME(mul352)(uint64_t C[11], const uint64_t A[6], const uint64_t B[6]) 
{
    __m128i x1,x2,x3,y1,y2,y3,z1,z2,sum,res1,res2;
    INIT384(x1,x2,x3,A);
    INIT384(y1,y2,y3,B);
    MUL352(C,x1,x2,x3,y1,y2,y3,z1,z2,sum,res1,res2);
}

void PREFIX_NAME(mul384)(uint64_t C[12], const uint64_t A[6], const uint64_t B[6]) 
{
    __m128i x1,x2,x3,y1,y2,y3,z1,z2,sum,res1,res2;
    INIT384(x1,x2,x3,A);
    INIT384(y1,y2,y3,B);
    MUL384(C,x1,x2,x3,y1,y2,y3,z1,z2,sum,res1,res2);
}

void PREFIX_NAME(mul416)(uint64_t C[13], const uint64_t A[7], const uint64_t B[7]) 
{
    __m128i x1,x2,x3,x4,y1,y2,y3,y4,z1,z2,sum,res1,res2;
    INIT448(x1,x2,x3,x4,A);
    INIT448(y1,y2,y3,y4,B);
    MUL416(C,x1,x2,x3,x4,y1,y2,y3,y4,z1,z2,sum,res1,res2);
}

void PREFIX_NAME(mul448)(uint64_t C[14], const uint64_t A[7], const uint64_t B[7]) 
{
    __m128i x1,x2,x3,x4,y1,y2,y3,y4,z1,z2,sum,res1,res2;
    INIT448(x1,x2,x3,x4,A);
    INIT448(y1,y2,y3,y4,B);
    MUL448(C,x1,x2,x3,x4,y1,y2,y3,y4,z1,z2,sum,res1,res2);
}

void PREFIX_NAME(mul480)(uint64_t C[15], const uint64_t A[8], const uint64_t B[8]) 
{
    __m128i x1,x2,x3,x4,y1,y2,y3,y4,z1,z2,sum,res1,res2;
    INIT512(x1,x2,x3,x4,A);
    INIT512(y1,y2,y3,y4,B);
    MUL480(C,x1,x2,x3,x4,y1,y2,y3,y4,z1,z2,sum,res1,res2);
}

void PREFIX_NAME(mul512)(uint64_t C[16], const uint64_t A[8], const uint64_t B[8]) 
{
    __m128i x1,x2,x3,x4,y1,y2,y3,y4,z1,z2,sum,res1,res2;
    INIT512(x1,x2,x3,x4,A);
    INIT512(y1,y2,y3,y4,B);
    MUL512(C,x1,x2,x3,x4,y1,y2,y3,y4,z1,z2,sum,res1,res2);
}

void PREFIX_NAME(mul544)(uint64_t C[17], const uint64_t A[9], const uint64_t B[9]) 
{
    __m128i x1,x2,x3,x4,x5,y1,y2,y3,y4,y5,z1,z2,sum,res1,res2;
    INIT576(x1,x2,x3,x4,x5,A);
    INIT576(y1,y2,y3,y4,y5,B);
    MUL544(C,x1,x2,x3,x4,x5,y1,y2,y3,y4,y5,z1,z2,sum,res1,res2);
}

void PREFIX_NAME(mul576)(uint64_t C[18], const uint64_t A[9], const uint64_t B[9]) 
{
    __m128i x1,x2,x3,x4,x5,y1,y2,y3,y4,y5,z1,z2,sum,res1,res2;
    INIT576(x1,x2,x3,x4,x5,A);
    INIT576(y1,y2,y3,y4,y5,B);
    MUL576(C,x1,x2,x3,x4,x5,y1,y2,y3,y4,y5,z1,z2,sum,res1,res2);
}

#endif

#else

uint64_t PREFIX_NAME(mul32)(uint64_t A, uint64_t B) 
{
    uint64_t C;
    unsigned int i;
    MUL32(C,A,B,i);
    return C;
}

uint64_t PREFIX_NAME(mul64low)(uint64_t A, uint64_t B) 
{
    uint64_t C;
    unsigned int i;
    MUL64LOW(C,A,B,i);
    return C;
}

#if !ENABLED_GF2X

void PREFIX_NAME(mul64)(uint64_t C[2], const uint64_t A[1], const uint64_t B[1]) 
{
    uint64_t tmp;
    unsigned int i;
    MUL64(C,*A,*B,i,tmp);
}

void PREFIX_NAME(mul128)(uint64_t C[4], const uint64_t A[2], const uint64_t B[2]) 
{
    uint64_t tmp,AA,BB;
    unsigned int i;
    MUL128(C,A,B,i,tmp,AA,BB);
}

void PREFIX_NAME(mul192)(uint64_t C[6], const uint64_t A[3], const uint64_t B[3]) 
{
    uint64_t tmp,AA,BB;
    unsigned int i;
    MUL192(C,A,B,i,tmp,AA,BB);
}

void PREFIX_NAME(mul256)(uint64_t C[8], const uint64_t A[4], const uint64_t B[4]) 
{
    uint64_t tmp,AA[2],BB[2],tmp1[2],tmp2[2];
    unsigned int i;
    MUL256(C,A,B,i,tmp,AA,BB,tmp1,tmp2);
}

#endif

#endif

