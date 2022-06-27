#ifndef _CONVMQ_GF2_H
#define _CONVMQ_GF2_H


#include "prefix_name.h"
#include "arch.h"
#include "tools_gf2nv.h"
#include "tools_gf2m.h"
#include "MQ_gf2.h"


void PREFIX_NAME(convMQ_UL_gf2)(uint8_t* pk2, const uint8_t* pk);
UINT PREFIX_NAME(convMQ_uncompressL_gf2)(uint64_t* pk2, const uint8_t* pk);
UINT PREFIX_NAME(convMQ_last_uncompressL_gf2)(uint64_t* pk2, const uint8_t* pk);

#define convMQ_UL_gf2 PREFIX_NAME(convMQ_UL_gf2)
#define convMQ_uncompressL_gf2 PREFIX_NAME(convMQ_uncompressL_gf2)
#define convMQ_last_uncompressL_gf2 PREFIX_NAME(convMQ_last_uncompressL_gf2)

#if (HFENr8&&(HFEmr8>1))
    uint64_t PREFIX_NAME(convMQ_last_UL_gf2)(uint8_t* pk2, const uint8_t* pk);
    #define convMQ_last_UL_gf2 PREFIX_NAME(convMQ_last_UL_gf2)
#endif



#endif

