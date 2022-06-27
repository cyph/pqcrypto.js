#ifndef _EVALMQNOCST_GF2_H
#define _EVALMQNOCST_GF2_H


#include "prefix_name.h"
#include "arch.h"
#include "matrix_gf2.h"
#include "tools_gf2nv.h"


#if (NB_WORD_GF2nv<10)
    UINT PREFIX_NAME(evalMQnocst_unrolled_no_simd_gf2)(const UINT* m,
                                                       const UINT* mq);
    #define evalMQnocst_unrolled_no_simd_gf2 \
                PREFIX_NAME(evalMQnocst_unrolled_no_simd_gf2)
#endif



#ifdef MQSOFT_REF
    #if (NB_WORD_GF2nv<10)
        #define evalMQnocst_gf2 evalMQnocst_unrolled_no_simd_gf2
    #endif
#elif (NB_WORD_GF2nv<10)
    #define evalMQnocst_gf2 evalMQnocst_unrolled_no_simd_gf2
#endif



#endif

