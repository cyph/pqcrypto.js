#ifndef _RAND_GF2N_H
#define _RAND_GF2N_H

#include "tools_gf2n.h"
#include "tools_gf2nv.h"
#include "tools_gf2m.h"

#include "randombytes.h"



#if HFEnr
    #define rand_gf2n(A) \
        randombytes((unsigned char*)(A),NB_BYTES_GFqn);\
        /* Clean the last word (included the zero padding) */\
        (A)[NB_WORD_GFqn-1]&=HFE_MASKn;
#else
    #define rand_gf2n(A) \
        randombytes((unsigned char*)(A),NB_BYTES_GFqn);
#endif


#if HFEnvr
    #define rand_gf2nv(A) \
        randombytes((unsigned char*)(A),NB_BYTES_GFqnv);\
        /* Clean the last word (included the zero padding) */\
        (A)[NB_WORD_GF2nv-1]&=HFE_MASKnv;
#else
    #define rand_gf2nv(A) \
        randombytes((unsigned char*)(A),NB_BYTES_GFqnv);
#endif


#if HFEmr
    #define rand_gf2m(A) \
        randombytes((unsigned char*)(A),NB_BYTES_GFqm);\
        /* Clean the last word (included the zero padding) */\
        (A)[NB_WORD_GF2m-1]&=HFE_MASKm;
#else
    #define rand_gf2m(A) \
        randombytes((unsigned char*)(A),NB_BYTES_GFqm);
#endif




#endif
