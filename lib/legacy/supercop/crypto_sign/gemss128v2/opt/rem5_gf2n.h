#ifndef _REM5_GF2N_H
#define _REM5_GF2N_H

/* Tools for pentanomials in rem_gf2n.h */


#include "arch.h"
#include "parameters_HFE.h"
#include "tools_gf2n.h"


#include "config_gf2n.h"


#ifdef __PENTANOMIAL_GF2N__
    #define K164 (64U-K1)
    #define K264 (64U-K2)


    #if (K1>63)
        #define K1mod64 (K1&63U)
    #else
        #define K1mod64 K1
    #endif


    #if (K2>63)
        #define K2mod64 (K2&63U)
    #else
        #define K2mod64 K2
    #endif
#endif




#endif

