#ifndef _MIXEQUATIONSMQS_GF2_H
#define _MIXEQUATIONSMQS_GF2_H


#include "prefix_name.h"
#include "config_HFE.h"
#include "matrix_gf2.h"
#include "MQS_gf2.h"



#if RIGHT_MULTIPLICATION_BY_T
    #define mixEquationsMQS_gf2 mixEquationsMQS_gf2_right
#else
    #define mixEquationsMQS_gf2 mixEquationsMQS_gf2_left
#endif


void PREFIX_NAME(mixEquationsMQS_gf2_right)(mqsnv_gf2m pk, cst_mqsnv_gf2n MQS, cst_Mn_gf2 T);
void PREFIX_NAME(mixEquationsMQS_gf2_left)(mqsnv_gf2m pk, cst_mqsnv_gf2n MQS, cst_Mn_gf2 T);

#define mixEquationsMQS_gf2_right PREFIX_NAME(mixEquationsMQS_gf2_right)
#define mixEquationsMQS_gf2_left PREFIX_NAME(mixEquationsMQS_gf2_left)



#endif

