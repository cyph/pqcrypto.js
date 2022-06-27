#ifndef _EVALMQSHYBRID_GF2_H
#define _EVALMQSHYBRID_GF2_H

#include "prefix_name.h"
#include "arch.h"
#include "matrix_gf2.h"
#include "config_HFE.h"


void PREFIX_NAME(evalMQShybrid8_uncomp_nocst_gf2_m)(vecm_gf2 res,
                                                    cst_vecnv_gf2 x,
                                                    const uint8_t* mq_quo,
                                                    const UINT* mq_rem);

#define evalMQShybrid8_uncomp_nocst_gf2_m \
            PREFIX_NAME(evalMQShybrid8_uncomp_nocst_gf2_m)


#if FORMAT_HYBRID_CPK8
    #define evalMQShybrid8_nocst_gf2_m evalMQShybrid8_uncomp_nocst_gf2_m
#endif


#endif

