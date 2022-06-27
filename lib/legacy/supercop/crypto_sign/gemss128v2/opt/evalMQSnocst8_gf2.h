#ifndef _EVALMQSNOCST8_GF2_H
#define _EVALMQSNOCST8_GF2_H

#include "prefix_name.h"
#include "arch.h"
#include "tools_gf2m.h"
#include "matrix_gf2.h"
#include "MQS_gf2.h"


void PREFIX_NAME(evalMQSnocst8_unrolled_gf2)(vecm_gf2 c, cst_vecnv_gf2 m,
                                            cst_mqsnv8_gf2m pk);
#define evalMQSnocst8_unrolled_gf2 PREFIX_NAME(evalMQSnocst8_unrolled_gf2)


/* Choose the best evaluation */
#ifdef MQSOFT_REF
    #define evalMQSnocst8_gf2 evalMQSnocst8_unrolled_gf2
#else
    #define evalMQSnocst8_gf2 evalMQSnocst8_unrolled_gf2
#endif


#endif

