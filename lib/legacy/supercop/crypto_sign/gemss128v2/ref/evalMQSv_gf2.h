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


/* Choose the best function */
#define evalMQSv_gf2 evalMQSv_classical_gf2


#endif


#endif

