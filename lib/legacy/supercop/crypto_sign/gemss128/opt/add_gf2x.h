#ifndef _ADD_GF2X_H
#define _ADD_GF2X_H

/* The sum in GF(2)[x] is the bitwise xor */

#include "arch.h"


/* General add */
#define ADD(a,b,c,SIZE) \
    FOR_LOOP((a)[RESERVED_VARIABLE]=(b)[RESERVED_VARIABLE]^(c)[RESERVED_VARIABLE],SIZE);

#define ADD2(a,b,SIZE) \
    FOR_LOOP((a)[RESERVED_VARIABLE]^=(b)[RESERVED_VARIABLE],SIZE);


#if (NB_BITS_UINT==64U)
/* Simple precision */

#define ADD64_RET(a,b) ((a)^(b))

#define ADD64(c,a,b) (c)=(a)^(b);

#define ADD64_2(c,a) (c)^=(a);

#define ADD32 ADD64
#define ADD32_2 ADD64_2
#define ADD32_TAB ADD64_TAB
#define ADD32_TAB2 ADD64_TAB2


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

#define ADD640_2(c,a) \
    ADD576_2(c,a); \
    (c)[9]^=(a)[9];

#define ADD704_2(c,a) \
    ADD640_2(c,a); \
    (c)[10]^=(a)[10];

#define ADD768_2(c,a) \
    ADD704_2(c,a); \
    (c)[11]^=(a)[11];

#define ADD832_2(c,a) \
    ADD768_2(c,a); \
    (c)[12]^=(a)[12];

#define ADD896_2(c,a) \
    ADD832_2(c,a); \
    (c)[13]^=(a)[13];

#define ADD960_2(c,a) \
    ADD896_2(c,a); \
    (c)[14]^=(a)[14];

#define ADD1024_2(c,a) \
    ADD960_2(c,a); \
    (c)[15]^=(a)[15];

#define ADD1088_2(c,a) \
    ADD1024_2(c,a); \
    (c)[16]^=(a)[16];

#define ADD1152_2(c,a) \
    ADD1088_2(c,a); \
    (c)[17]^=(a)[17];


#endif
#endif
