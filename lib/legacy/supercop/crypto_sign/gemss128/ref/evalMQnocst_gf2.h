#ifndef _EVALMQNOCST_GF2_H
#define _EVALMQNOCST_GF2_H


#include "arch.h"
#include "config_HFE.h"


UINT evalMQnocst_noSIMD2_gf2(cst_vecnv_gf2 x,const UINT* mq);
void evalMQSnocst_noSIMD_gf2_m(vecm_gf2 res,cst_vecnv_gf2 x,const UINT* mq);

#if (NB_WORD_GFqnv<10)
    UINT evalMQnocst_noSIMD_gf2(const UINT* m, const UINT* mq);
#endif



#define evalMQSnocst_gf2_m evalMQSnocst_noSIMD_gf2_m

#if (NB_WORD_GFqnv<10)
    #define evalMQnocst_gf2 evalMQnocst_noSIMD_gf2
#else
    #define evalMQnocst_gf2 evalMQnocst_noSIMD2_gf2
#endif



#endif
