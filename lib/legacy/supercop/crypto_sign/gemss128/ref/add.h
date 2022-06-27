#ifndef _ADD_H
#define _ADD_H

/* The sum in GF(2^n) is the bitwise xor */

#include "arch.h"
#if (NB_BITS_UINT!=64U)
    #define ADD(a,b,c,SIZE) \
        FOR_LOOP((a)[RESERVED_VARIABLE]=(b)[RESERVED_VARIABLE]^(c)[RESERVED_VARIABLE],SIZE);

    #define ADD2(a,b,SIZE) \
        FOR_LOOP((a)[RESERVED_VARIABLE]^=(b)[RESERVED_VARIABLE],SIZE);
#else

/* Simple precision */

#define ADD64_RET(a,b) ((a)^(b))

#define ADD64(c,a,b) (c)=(a)^(b);

#define ADD64_2(c,a) (c)^=(a);

/* Multiple precision */

/* c[i]=a[i]^b[i] */
#define ADD64_TAB(c,a,b) \
    (c)[0]=(a)[0]^(b)[0];

#define ADD128(c,a,b) \
    ADD64_TAB(c,a,b);\
    (c)[1]=(a)[1]^(b)[1];

#define ADD192(c,a,b) \
    ADD128(c,a,b); \
    (c)[2]=(a)[2]^(b)[2];

#define ADD256(c,a,b) \
    ADD192(c,a,b); \
    (c)[3]=(a)[3]^(b)[3];

#define ADD320(c,a,b) \
    ADD256(c,a,b); \
    (c)[4]=(a)[4]^(b)[4];

#define ADD384(c,a,b) \
    ADD320(c,a,b); \
    (c)[5]=(a)[5]^(b)[5];

#define ADD448(c,a,b) \
    ADD384(c,a,b); \
    (c)[6]=(a)[6]^(b)[6];

#define ADD512(c,a,b) \
    ADD448(c,a,b); \
    (c)[7]=(a)[7]^(b)[7];

#define ADD576(c,a,b) \
    ADD512(c,a,b); \
    (c)[8]=(a)[8]^(b)[8];


/* c[i]^=a[i] */
#define ADD64_TAB2(c,a) \
    (c)[0]^=(a)[0];

#define ADD128_2(c,a) \
    ADD64_TAB2(c,a);\
    (c)[1]^=(a)[1]; 

#define ADD192_2(c,a) \
    ADD128_2(c,a); \
    (c)[2]^=(a)[2];

#define ADD256_2(c,a) \
    ADD192_2(c,a); \
    (c)[3]^=(a)[3];

#define ADD320_2(c,a) \
    ADD256_2(c,a); \
    (c)[4]^=(a)[4];

#define ADD384_2(c,a) \
    ADD320_2(c,a); \
    (c)[5]^=(a)[5];

#define ADD448_2(c,a) \
    ADD384_2(c,a); \
    (c)[6]^=(a)[6];

#define ADD512_2(c,a) \
    ADD448_2(c,a); \
    (c)[7]^=(a)[7];

#define ADD576_2(c,a) \
    ADD512_2(c,a); \
    (c)[8]^=(a)[8];



#endif
#endif
