#ifndef _EVALMQNOCST_GF2_H
#define _EVALMQNOCST_GF2_H


#include "prefix_name.h"
#include "arch.h"
#include "matrix_gf2.h"
#include "tools_gf2nv.h"


UINT PREFIX_NAME(evalMQnocst_noSIMD2_gf2)(cst_vecnv_gf2 x,const UINT* mq);
void PREFIX_NAME(evalMQSnocst_noSIMD_gf2_m)(vecm_gf2 res,cst_vecnv_gf2 x,const UINT* mq);

#define evalMQnocst_noSIMD2_gf2 PREFIX_NAME(evalMQnocst_noSIMD2_gf2)
#define evalMQSnocst_noSIMD_gf2_m PREFIX_NAME(evalMQSnocst_noSIMD_gf2_m)

#if (NB_WORD_GF2nv<10)
    UINT PREFIX_NAME(evalMQnocst_noSIMD_gf2)(const UINT* m, const UINT* mq);
    #define evalMQnocst_noSIMD_gf2 PREFIX_NAME(evalMQnocst_noSIMD_gf2)
#endif



#define evalMQSnocst_gf2_m evalMQSnocst_noSIMD_gf2_m

#if (NB_WORD_GF2nv<10)
    #define evalMQnocst_gf2 evalMQnocst_noSIMD_gf2
#else
    #define evalMQnocst_gf2 evalMQnocst_noSIMD2_gf2
#endif



#endif
