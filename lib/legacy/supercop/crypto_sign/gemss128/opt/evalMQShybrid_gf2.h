#ifndef _EVALMQSHYBRID_GF2_H
#define _EVALMQSHYBRID_GF2_H

#include "prefix_name.h"
#include "arch.h"
#include "matrix_gf2.h"


void PREFIX_NAME(evalMQShybrid_nocst_gf2_m)(vecm_gf2 res, cst_vecnv_gf2 x, const UINT* mq);
#define evalMQShybrid_nocst_gf2_m PREFIX_NAME(evalMQShybrid_nocst_gf2_m)


#endif
