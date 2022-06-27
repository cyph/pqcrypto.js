#ifndef _CONVMQS_GF2_H
#define _CONVMQS_GF2_H


#include "prefix_name.h"
#include "arch.h"
#include "tools_gf2nv.h"



void PREFIX_NAME(convMQS_one_to_m_compressed_equations_gf2)(UINT* pk2, const UINT* pk);
void PREFIX_NAME(convMQS_one_to_m_equations_gf2)(UINT* pk2, const UINT* pk);
void PREFIX_NAME(convMQS_one_eq_to_hybrid_representation_gf2)(UINT* pk2, const UINT* pk);

#define convMQS_one_to_m_compressed_equations_gf2 PREFIX_NAME(convMQS_one_to_m_compressed_equations_gf2)
#define convMQS_one_to_m_equations_gf2 PREFIX_NAME(convMQS_one_to_m_equations_gf2)
#define convMQS_one_eq_to_hybrid_representation_gf2 PREFIX_NAME(convMQS_one_eq_to_hybrid_representation_gf2)



#endif
