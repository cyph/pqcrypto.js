#ifndef _RAND_GF2NX_H
#define _RAND_GF2NX_H

#include "prefix_name.h"
#include "parameters_HFE.h"
#include "gf2nx.h"


void PREFIX_NAME(randMonic_gf2nx)(gf2nx F, unsigned int d);
void PREFIX_NAME(randMonicHFE_gf2nx)(sparse_monic_gf2nx F);

#define randMonic_gf2nx PREFIX_NAME(randMonic_gf2nx)
#define randMonicHFE_gf2nx PREFIX_NAME(randMonicHFE_gf2nx)


#if HFEv
    void PREFIX_NAME(randMonicHFEv_gf2nx)(sparse_monic_gf2nx F);
    #define randMonicHFEv_gf2nx PREFIX_NAME(randMonicHFEv_gf2nx)
#else
    #define randMonicHFEv_gf2nx randMonicHFE_gf2nx
#endif


#if HFEnr
    void PREFIX_NAME(cleanMonicHFE_gf2nx)(sparse_monic_gf2nx F);
    #define cleanMonicHFE_gf2nx PREFIX_NAME(cleanMonicHFE_gf2nx)
#else
    #define cleanMonicHFE_gf2nx
#endif


#if HFEv
    #if HFEnr
        void PREFIX_NAME(cleanMonicHFEv_gf2nx)(sparse_monic_gf2nx F);
        #define cleanMonicHFEv_gf2nx PREFIX_NAME(cleanMonicHFEv_gf2nx)
    #else
        #define cleanMonicHFEv_gf2nx
    #endif
#else
    #define cleanMonicHFEv_gf2nx cleanMonicHFE_gf2nx
#endif


#endif

