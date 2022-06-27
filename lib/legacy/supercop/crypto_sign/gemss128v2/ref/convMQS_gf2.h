#ifndef _CONVMQS_GF2_H
#define _CONVMQS_GF2_H


#include "prefix_name.h"
#include "arch.h"
#include "tools_gf2nv.h"
#include "tools_gf2m.h"
#include "MQS_gf2.h"
#include "config_HFE.h"


#if HFEmr8
    void PREFIX_NAME(convMQS_one_eq_to_hybrid_rep8_comp_gf2)(uint8_t* pk2,
                                                             const uint8_t* pk);
    #define convMQS_one_eq_to_hybrid_rep8_comp_gf2 \
        PREFIX_NAME(convMQS_one_eq_to_hybrid_rep8_comp_gf2)

    #if (EVAL_HYBRID_CPK8_UNCOMP&&HFENr8&&(HFEmr8>1))
        void PREFIX_NAME(convMQS_one_eq_to_hybrid_rep8_uncomp_gf2)(uint8_t* pk2,
                                                             const uint8_t* pk);
        #define convMQS_one_eq_to_hybrid_rep8_uncomp_gf2 \
            PREFIX_NAME(convMQS_one_eq_to_hybrid_rep8_uncomp_gf2)
    #else
        #define convMQS_one_eq_to_hybrid_rep8_uncomp_gf2 \
                convMQS_one_eq_to_hybrid_rep8_comp_gf2
    #endif

    #define convMQS_one_eq_to_hybrid_rep8_gf2 \
            convMQS_one_eq_to_hybrid_rep8_uncomp_gf2
#endif


#endif

