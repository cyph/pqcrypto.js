#include "mul.h"


#ifdef ENABLED_AVX_MUL

#include "init.h"

uint64_t mul64low(uint64_t A, uint64_t B) 
{
    __m128i x,y;
    INIT64(x,A);
    INIT64(y,B);
    return MUL64LOW(x,y,0);
}

#if !ENABLE_GF2X


/* This file owns mul128_Karat and mul256_Karat */
/* But the implementation is slower than classical mutiplication */


/* C=A*B or C=A*A for each function */

void mul64(uint64_t C[2], uint64_t A, uint64_t B) 
{
    __m128i x,y,z;
    INIT64(x,A);
    INIT64(y,B);
    MUL64(C,x,y,z,0);
}

void mul96(uint64_t C[3], const uint64_t A[2], const uint64_t B[2])
{
    __m128i x,y,z1,z2,sum,res_low,res_high;
    INIT128(x,A);
    INIT128(y,B);
    MUL96(C,x,y,z1,z2,sum,res_low,res_high);
}

void mul128(uint64_t C[4], const uint64_t A[2], const uint64_t B[2]) 
{
    __m128i x,y,z1,z2,sum,res_low,res_high;
    INIT128(x,A);
    INIT128(y,B);
    MUL128(C,x,y,z1,z2,sum,res_low,res_high);
}

/* Karatsuba is slower */
/* 3 mul64, 2 add64, 4 add128 */
void mul128_Karat(uint64_t C[4], const uint64_t A[2], const uint64_t B[2]) 
{
    __m128i x,y,z1,z2,mid,mid2,res_low,res_high;
    INIT128(x,A);
    INIT128(y,B);

    /* L=Alow*Blow (x^0 ... x^127) */
    res_low=_mm_clmulepi64_si128(x,y,0);
    /* H=Ahigh*Bhigh (x^128 ... x^255) */
    res_high=_mm_clmulepi64_si128(x,y,0x11);

    mid=_mm_xor_si128(res_low,res_high);

    /* M=(Ahigh-Alow)*(Bhigh-Blow) */
    mid2=RIGHT_MOVE64(x);
    z1=_mm_xor_si128(x,mid2);
    mid2=RIGHT_MOVE64(y);
    z2=_mm_xor_si128(y,mid2);

    x = _mm_clmulepi64_si128(z1, z2, 0);

    /* mid2 (x^64 ... x^191) */
    mid2=_mm_xor_si128(x,mid);

    /* mid2_low: x^64 ... x^127 */
    y=LEFT_SHIFT64(mid2);
    /* mid2_high: x^128 ... x^191 */
    x=RIGHT_SHIFT64(mid2);
    /* mid2_low + L */
    z1=_mm_xor_si128(y,res_low);
    /* mid2_high + H */
    z2=_mm_xor_si128(x,res_high);
    EXTRACT128(C,z1);
    EXTRACT128(C+2,z2);
}

void mul160(uint64_t C[5], const uint64_t A[3], const uint64_t B[3]) 
{
    __m128i x1,x2,y1,y2,z1,z2,sum,res1,res2;
    INIT192(x1,x2,A);
    INIT192(y1,y2,B);
    MUL160(C,x1,x2,y1,y2,z1,z2,sum,res1,res2);
}

void mul192(uint64_t C[6], const uint64_t A[3], const uint64_t B[3]) 
{
    __m128i x1,x2,y1,y2,z1,z2,sum,res1,res2;
    INIT192(x1,x2,A);
    INIT192(y1,y2,B);
    MUL192(C,x1,x2,y1,y2,z1,z2,sum,res1,res2);
}


void mul224(uint64_t C[7], const uint64_t A[4], const uint64_t B[4]) 
{
    __m128i x1,x2,y1,y2,z1,z2,sum,res1,res2;
    INIT256(x1,x2,A);
    INIT256(y1,y2,B);
    MUL224(C,x1,x2,y1,y2,z1,z2,sum,res1,res2);
}

void mul256(uint64_t C[8], const uint64_t A[4], const uint64_t B[4]) 
{
    __m128i x1,x2,y1,y2,z1,z2,sum,res1,res2;
    INIT256(x1,x2,A);
    INIT256(y1,y2,B);
    MUL256(C,x1,x2,y1,y2,z1,z2,sum,res1,res2);
}


/* 12 mul64, 6 add64, 10 add128 */
void mul256_Karat(uint64_t C[8], const uint64_t A[4], const uint64_t B[4]) 
{
    __m128i x1,x2,y1,y2,mid1,mid2,z1,z2,z11,z12,sum,res1,res2;
    INIT256(x1,x2,A);
    INIT256(y1,y2,B);

    /* L=Alow*Blow */
    /* L*x^0 */
    MUL128(C,x1,y1,z1,z2,sum,res1,res2);

    /* H=Ahigh*Bhigh */
    /* H*x^256 */
    MUL128(C+4,x2,y2,z1,z2,sum,res1,res2);

    /* (L+H)*X^128 */
    INIT256(mid1,z1,C);
    INIT256(z2,mid2,C+4);
    sum=_mm_xor_si128(z1,z2);
    z11=_mm_xor_si128(sum,mid1);
    z12=_mm_xor_si128(sum,mid2);


    /* M=(Ahigh-Alow)*(Bhigh-Blow) */
    mid1=_mm_xor_si128(x1,x2);
    mid2=_mm_xor_si128(y1,y2);

    z1 = _mm_clmulepi64_si128(mid1, mid2, 1);
    z2 = _mm_clmulepi64_si128(mid1, mid2, 16);
    sum =_mm_xor_si128(z1,z2);
    EXTRACT128(C+3,sum);


    z1 = _mm_clmulepi64_si128(mid1, mid2, 0);
    z2 = _mm_clmulepi64_si128(mid1, mid2, 17);

    mid1=_mm_xor_si128(z11,z1);
    mid2=_mm_xor_si128(z12,z2);

    C[2]=EXTRACT64(mid1,0);
    C[3]^=EXTRACT64(mid1,1);
    C[4]^=EXTRACT64(mid2,0);
    C[5]=EXTRACT64(mid2,1);
}

/* slower than mul256_Karat, 12 mul64, 16 add64, 5 add128 */
void mul256_Karat_first_version(uint64_t C[8], const uint64_t A[4], const uint64_t B[4]) 
{
    __m128i x1,x2,y1,y2,mid1,mid2,z1,z2,sum,res1,res2;
    INIT256(x1,x2,A);
    INIT256(y1,y2,B);

    /* L=Alow*Blow */
    /* L*x^0 */
    MUL128(C,x1,y1,z1,z2,sum,res1,res2);

    /* H=Ahigh*Bhigh */
    /* H*x^256 */
    MUL128(C+4,x2,y2,z1,z2,sum,res1,res2);

    /* (L+H)*X^128 */
    C[4]^=C[2];
    C[5]^=C[3];
    C[2]=C[4]^C[0];
    C[3]=C[5]^C[1];
    C[4]^=C[6];
    C[5]^=C[7];

    /* M=(Ahigh-Alow)*(Bhigh-Blow) */
    mid1=_mm_xor_si128(x1,x2);
    mid2=_mm_xor_si128(y1,y2);

    z1 = _mm_clmulepi64_si128(mid1, mid2, 0);
    z2 = _mm_clmulepi64_si128(mid1, mid2, 17);
    C[2]^=EXTRACT64(z1,0);
    C[3]^=EXTRACT64(z1,1);
    C[4]^=EXTRACT64(z2,0);
    C[5]^=EXTRACT64(z2,1);

    z1 = _mm_clmulepi64_si128(mid1, mid2, 1);
    z2 = _mm_clmulepi64_si128(mid1, mid2, 16);
    sum =_mm_xor_si128(z1,z2);
    C[3]^=EXTRACT64(sum,0);
    C[4]^=EXTRACT64(sum,1);
}


void mul288(uint64_t C[9], const uint64_t A[5], const uint64_t B[5]) 
{
    __m128i x1,x2,x3,y1,y2,y3,z1,z2,sum,res1,res2;
    INIT320(x1,x2,x3,A);
    INIT320(y1,y2,y3,B);
    MUL288(C,x1,x2,x3,y1,y2,y3,z1,z2,sum,res1,res2);
}

void mul320(uint64_t C[10], const uint64_t A[5], const uint64_t B[5]) 
{
    __m128i x1,x2,x3,y1,y2,y3,z1,z2,sum,res1,res2;
    INIT320(x1,x2,x3,A);
    INIT320(y1,y2,y3,B);
    MUL320(C,x1,x2,x3,y1,y2,y3,z1,z2,sum,res1,res2);
}


void mul352(uint64_t C[11], const uint64_t A[6], const uint64_t B[6]) 
{
    __m128i x1,x2,x3,y1,y2,y3,z1,z2,sum,res1,res2;
    INIT384(x1,x2,x3,A);
    INIT384(y1,y2,y3,B);
    MUL352(C,x1,x2,x3,y1,y2,y3,z1,z2,sum,res1,res2);
}

void mul384(uint64_t C[12], const uint64_t A[6], const uint64_t B[6]) 
{
    __m128i x1,x2,x3,y1,y2,y3,z1,z2,sum,res1,res2;
    INIT384(x1,x2,x3,A);
    INIT384(y1,y2,y3,B);
    MUL384(C,x1,x2,x3,y1,y2,y3,z1,z2,sum,res1,res2);
}

#endif
/******* Square *******/

uint64_t square64low(uint64_t A) 
{
    __m128i x;
    INIT64(x,A);
    return SQUARE64LOW(x,0);
}

void square64(uint64_t C[2], uint64_t A) 
{
    __m128i x,z;
    INIT64(x,A);
    SQUARE64(C,x,z,0)
}

void square96(uint64_t C[3], const uint64_t A[2]) 
{
    __m128i x,z;
    INIT128(x,A);
    SQUARE96(C,x,z);
}

void square128(uint64_t C[4], const uint64_t A[2]) 
{
    __m128i x,z;
    INIT128(x,A);
    SQUARE128(C,x,z);
}

void square160(uint64_t C[5], const uint64_t A[3]) 
{
    __m128i x1,x2,z;
    INIT192(x1,x2,A);
    SQUARE160(C,x1,x2,z);
}

void square192(uint64_t C[6], const uint64_t A[3]) 
{
    __m128i x1,x2,z;
    INIT192(x1,x2,A);
    SQUARE192(C,x1,x2,z);
}

void square224(uint64_t C[7], const uint64_t A[4]) 
{
    __m128i x1,x2,z;
    INIT256(x1,x2,A);
    SQUARE224(C,x1,x2,z);
}

void square256(uint64_t C[8], const uint64_t A[4]) 
{
    __m128i x1,x2,z;
    INIT256(x1,x2,A);
    SQUARE256(C,x1,x2,z);
}

void square288(uint64_t C[9], const uint64_t A[5]) 
{
    __m128i x1,x2,x3,z;
    INIT320(x1,x2,x3,A);
    SQUARE288(C,x1,x2,x3,z);
}

void square320(uint64_t C[10], const uint64_t A[5]) 
{
    __m128i x1,x2,x3,z;
    INIT320(x1,x2,x3,A);
    SQUARE320(C,x1,x2,x3,z);
}

void square352(uint64_t C[11], const uint64_t A[6]) 
{
    __m128i x1,x2,x3,z;
    INIT384(x1,x2,x3,A);
    SQUARE352(C,x1,x2,x3,z);
}

void square384(uint64_t C[12], const uint64_t A[6]) 
{
    __m128i x1,x2,x3,z;
    INIT384(x1,x2,x3,A);
    SQUARE384(C,x1,x2,x3,z);
}

#else

uint64_t mul32(uint64_t A, uint64_t B) 
{
    uint64_t C;
    unsigned int i;
    MUL32(C,A,B,i);
    return C;
}

uint64_t mul64low(uint64_t A, uint64_t B) 
{
    uint64_t C;
    unsigned int i;
    MUL64LOW(C,A,B,i);
    return C;
}

#if !ENABLE_GF2X

void mul64(uint64_t C[2], uint64_t A, uint64_t B) 
{
    uint64_t tmp;
    unsigned int i;
    MUL64(C,A,B,i,tmp);
}

void mul128(uint64_t C[4], const uint64_t A[2], const uint64_t B[2]) 
{
    uint64_t tmp,AA,BB;
    unsigned int i;
    MUL128(C,A,B,i,tmp,AA,BB);
}

void mul192(uint64_t C[6], const uint64_t A[3], const uint64_t B[3]) 
{
    uint64_t tmp,AA,BB;
    unsigned int i;
    MUL192(C,A,B,i,tmp,AA,BB);
}

void mul256(uint64_t C[8], const uint64_t A[4], const uint64_t B[4]) 
{
    uint64_t tmp,AA[2],BB[2],tmp1[2],tmp2[2];
    unsigned int i;
    MUL256(C,A,B,i,tmp,AA,BB,tmp1,tmp2);
}

#endif

uint64_t square64low(uint64_t A) 
{
    uint64_t C;
    SQUARE64LOW(C,A);
    return C;
}

#endif

