#ifndef _MIXEQUATIONSMQS_GF2_H
#define _MIXEQUATIONSMQS_GF2_H


#include "prefix_name.h"
#include "config_HFE.h"
#include "matrix_gf2.h"
#include "MQS_gf2.h"

#include "choice_crypto.h"


#define mixEquationsMQS8_gf2 mixEquationsMQS8_gf2_right


void PREFIX_NAME(mixEquationsMQS8_gf2_right)(mqsnv8_gf2m pk, cst_mqsnv_gf2n MQS,
                                             cst_Mn_gf2 T);
#define mixEquationsMQS8_gf2_right PREFIX_NAME(mixEquationsMQS8_gf2_right)


#endif

