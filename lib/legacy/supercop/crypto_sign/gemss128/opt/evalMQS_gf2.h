#ifndef _EVALMQS_GF2_H
#define _EVALMQS_GF2_H

#include "prefix_name.h"
#include "arch.h"
#include "parameters_HFE.h"
#include "matrix_gf2.h"
#include "MQS_gf2.h"



void PREFIX_NAME(evalMQS_classical_gf2)(vecm_gf2 c, cst_vecnv_gf2 m, cst_mqsnv_gf2m pk);
#define evalMQS_classical_gf2 PREFIX_NAME(evalMQS_classical_gf2)

#if (HFEnv>2)
    void PREFIX_NAME(evalMQS_unrolled_gf2)(vecm_gf2 c, cst_vecnv_gf2 m, cst_mqsnv_gf2m pk);
    #define evalMQS_unrolled_gf2 PREFIX_NAME(evalMQS_unrolled_gf2)
#endif



/* Choose the best function */
#if (HFEnv>2)
    #define evalMQS_gf2 evalMQS_unrolled_gf2
#else
    #define evalMQS_gf2 evalMQS_classical_gf2
#endif



#endif

