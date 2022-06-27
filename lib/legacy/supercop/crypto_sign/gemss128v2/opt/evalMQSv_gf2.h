#ifndef _EVALMQSV_GF2_H
#define _EVALMQSV_GF2_H

#include "prefix_name.h"
#include "arch.h"
#include "parameters_HFE.h"
#include "matrix_gf2.h"
#include "MQS_gf2.h"


/* n equations, v variables */
#if HFEv

void PREFIX_NAME(evalMQSv_classical_gf2)(vecm_gf2 c, cst_vecnv_gf2 m,
                                         cst_mqsnv_gf2m pk);
#define evalMQSv_classical_gf2 PREFIX_NAME(evalMQSv_classical_gf2)

#if (HFEv>2)
    void PREFIX_NAME(evalMQSv_unrolled_gf2)(vecm_gf2 c, cst_vecnv_gf2 m,
                                            cst_mqsnv_gf2m pk);
    #define evalMQSv_unrolled_gf2 PREFIX_NAME(evalMQSv_unrolled_gf2)
#endif


/* Choose the best function */
#ifdef MQSOFT_REF
    #define evalMQSv_gf2 evalMQSv_classical_gf2
#elif (HFEv>2)
    #define evalMQSv_gf2 evalMQSv_unrolled_gf2
#else
    #define evalMQSv_gf2 evalMQSv_classical_gf2
#endif


#endif


#endif

