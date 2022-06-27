#ifndef _EVALUATEMQS_HPP
#define _EVALUATEMQS_HPP

#include "config_HFE.h"

void evaluateMQS_pk(vecm_gf2 c, cst_vecnv_gf2 m, cst_mqsnv_gf2m pk);
void evaluateMQS_pk_hybrid(vecm_gf2 c, cst_vecnv_gf2 m, cst_mqsnv_gf2m pk);
void evaluateMQS(vecn_gf2 c, cst_vecv_gf2 m, cst_mqsv_gf2n pk);


#endif

