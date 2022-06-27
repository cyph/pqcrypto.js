#ifndef _CHOOSEROOTHFE_H
#define _CHOOSEROOTHFE_H

#include "choice_crypto.h"

#if 0
    /* To take a root only if it is uniq */
    #define UNIQ_ROOT
#endif



#ifdef UNIQ_ROOT
    #include "findRoots_gf2nx.h"
    #define chooseRootHFE_gf2nx findUniqRootHFE_gf2nx
#else
    /* Choose the method to choose the root */
    #define FIRST_ROOT 0
    #define DETERMINIST_ROOT 1

    #if ((FIRST_ROOT+DETERMINIST_ROOT)!=1)
        #error "It needs a uniq method to choose a root."
    #endif


    #include "prefix_name.h"
    #include "tools_gf2n.h"
    #include "gf2nx.h"

    int PREFIX_NAME(chooseRootHFE_gf2nx)(gf2n root, \
                              const complete_sparse_monic_gf2nx F, cst_gf2n U);
    #define chooseRootHFE_gf2nx PREFIX_NAME(chooseRootHFE_gf2nx)
#endif


#endif

