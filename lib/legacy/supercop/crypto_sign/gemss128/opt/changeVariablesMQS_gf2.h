#ifndef _CHANGEVARIABLESMQS_GF2_H
#define _CHANGEVARIABLESMQS_GF2_H


#include "prefix_name.h"
#include "matrix_gf2.h"
#include "MQS_gf2.h"
#include "arch.h"


int PREFIX_NAME(changeVariablesMQS64_gf2)(mqsnv_gf2n MQS, cst_GLnv_gf2 S);

#define changeVariablesMQS64_gf2 PREFIX_NAME(changeVariablesMQS64_gf2)


#define changeVariablesMQS_gf2 changeVariablesMQS64_gf2



#endif

