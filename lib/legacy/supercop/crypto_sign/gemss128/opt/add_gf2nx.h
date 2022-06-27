#ifndef _ADD_GF2NX_H
#define _ADD_GF2NX_H

#include "tools_gf2n.h"



/* Add two elements of degree (len-1) in GF(2^n)[X] */
#define add_gf2nx(res,A,B,len,i) \
    for(i=0;i<((len)*NB_WORD_GFqn);++i) \
    { \
        (res)[i]=(A)[i]^(B)[i]; \
    }

#define add2_gf2nx(res,A,len,i) \
    for(i=0;i<((len)*NB_WORD_GFqn);++i) \
    { \
        (res)[i]^=(A)[i]; \
    }



/* Copy one element of degree (len-1) in GF(2^n)[X] */
#define copy_gf2nx(res,A,len,i) \
    for(i=0;i<((len)*NB_WORD_GFqn);++i) \
    { \
        (res)[i]=(A)[i]; \
    }

/* memcpy((void*)res,(void*)A,len*NB_WORD_GFqn*sizeof(UINT)); */



#endif
