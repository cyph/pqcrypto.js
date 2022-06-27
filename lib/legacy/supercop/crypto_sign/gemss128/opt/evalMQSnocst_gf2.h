#ifndef _EVALMQSNOCST_GF2_H
#define _EVALMQSNOCST_GF2_H

#include "prefix_name.h"
#include "arch.h"
#include "tools_gf2m.h"
#include "matrix_gf2.h"
#include "MQS_gf2.h"



void PREFIX_NAME(evalMQSnocst_unrolled_gf2)(vecm_gf2 c, cst_vecnv_gf2 m, cst_mqsnv_gf2m pk);
#define evalMQSnocst_unrolled_gf2 PREFIX_NAME(evalMQSnocst_unrolled_gf2)

#define evalMQSnocst_gf2 evalMQSnocst_unrolled_gf2


#endif

