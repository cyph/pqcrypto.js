#ifndef _CONVMQS_GF2_H
#define _CONVMQS_GF2_H


#include "arch.h"
#include "config_HFE.h"



void convMQS_one_to_m_compressed_equations_gf2(UINT* pk2, const UINT* pk);
void convMQS_one_to_m_equations_gf2(UINT* pk2, const UINT* pk);
void convMQS_one_eq_to_hybrid_representation_gf2(UINT* pk2, const UINT* pk);



#endif
