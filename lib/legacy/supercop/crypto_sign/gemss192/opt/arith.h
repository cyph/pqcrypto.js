#ifndef _ARITH_H
#define _ARITH_H


#include "config_HFE.h"


HFEMULMOD;
HFESQUAREMOD;

#if (NB_WORD_GFqn==1)
    #if (HFEn<33)
        uint64_t hferem(const uint64_t Pol);
    #else
        uint64_t hferem(const uint64_t Pol[NB_WORD_MUL]);
    #endif
#else
    void hferem(uint64_t P[NB_WORD_GFqn], const uint64_t Pol[NB_WORD_MUL]);
#endif


#endif
