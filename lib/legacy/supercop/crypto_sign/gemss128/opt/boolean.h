#ifndef _BOOLEAN_H
#define _BOOLEAN_H


#include "arch.h"


#define ADDLOADMASK1(res,a,b,SIZE) \
    FOR_LOOP((res)[RESERVED_VARIABLE]^=(a)[RESERVED_VARIABLE]&(b),SIZE);


/* Bitwise multiplication and xor to res, b a constant */
#define ADDLOADMASK1_64(res,a,b) \
    (res)[0]^=(a)[0]&(b);

#define ADDLOADMASK1_128(res,a,b) \
    ADDLOADMASK1_64(res,a,b);\
    (res)[1]^=(a)[1]&(b);

#define ADDLOADMASK1_192(res,a,b) \
    ADDLOADMASK1_128(res,a,b);\
    (res)[2]^=(a)[2]&(b);

#define ADDLOADMASK1_256(res,a,b) \
    ADDLOADMASK1_192(res,a,b);\
    (res)[3]^=(a)[3]&(b);

#define ADDLOADMASK1_320(res,a,b) \
    ADDLOADMASK1_256(res,a,b);\
    (res)[4]^=(a)[4]&(b);

#define ADDLOADMASK1_384(res,a,b) \
    ADDLOADMASK1_320(res,a,b);\
    (res)[5]^=(a)[5]&(b);

#define ADDLOADMASK1_448(res,a,b) \
    ADDLOADMASK1_384(res,a,b);\
    (res)[6]^=(a)[6]&(b);



#endif
