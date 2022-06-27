#include "chooseRootHFE_gf2nx.h"

#ifndef UNIQUE_ROOT


#include <stdlib.h>
#include "tools_gf2n.h"
#include "add_gf2n.h"
#include "findRoots_gf2nx.h"

#if (!QUARTZ_ROOT)
    #include "sort_gf2n.h"
#endif

#if (QUARTZ_ROOT||DETERMINIST_ROOT)
    #include "hash.h"
#endif


/**
 * @brief   Find the roots of a monic HFE polynomial, then choose one.
 * @param[in]   F   A monic HFE polynomial in GF(2^n)[X] stored with a sparse
 * representation.
 * @param[in]   U   An element of GF(2^n).
 * @param[out]  root    An element of GF(2^n), one of the root of (F.poly-U).
 * @return  The number of roots of (F.poly-U), without the multiplicity.
 * @remark  Requirement: F.poly is monic.
 * @remark  Requirement: F.L must be initialized with initListDifferences_gf2nx.
 * @remark  A part of the implementation is not in constant-time.
 */
int PREFIX_NAME(chooseRootHFE_gf2nx)(gf2n root,
                                     const complete_sparse_monic_gf2nx F,
                                     cst_gf2n U)
{
    #if (HFEDeg==1)
        add_gf2n(root,F.poly,U);
        return 1;
    #else

        #if DETERMINIST_ROOT
            UINT hash[SIZE_DIGEST_UINT];
        #elif QUARTZ_ROOT
            UINT* hash;
            unsigned int j,i,ind=0;
        #endif

        vec_gf2n roots;
        int l;

        l=findRootsHFE_gf2nx(&roots,F,U);

        if(!l)
        {
            /* Zero root */
            return 0;
        } else
        {
            if(l==1)
            {
                /* One root */
                copy_gf2n(root,roots);
            } else
            {
                /* Several roots */
                #if QUARTZ_ROOT
                    hash=(UINT*)malloc(l*SIZE_DIGEST_UINT*sizeof(UINT));

                    /* We hash each root */
                    for(i=0;i<l;++i)
                    {
                        HASH((unsigned char*)(hash+i*SIZE_DIGEST_UINT),
                             (unsigned char*)(roots+i*NB_WORD_GFqn),
                             NB_BYTES_GFqn);
                    }

                    /* We search the smallest hash (seen as an integer) */
                    for(i=1;i<l;++i)
                    {
                        j=0;
                        while((j<SIZE_DIGEST_UINT)&&
                              (hash[ind*SIZE_DIGEST_UINT+j]==
                               hash[i*SIZE_DIGEST_UINT+j]))
                        {
                            ++j;
                        }
                        if((j<SIZE_DIGEST_UINT)&&
                           (hash[ind*SIZE_DIGEST_UINT+j]>
                            hash[i*SIZE_DIGEST_UINT+j]))
                        {
                            ind=i;
                        }
                    }

                    /* We choose the corresponding root */
                    copy_gf2n(root,roots+ind*NB_WORD_GFqn);

                    free(hash);
                #else

                    /* Sort the roots */
                    sort_gf2n(roots,l);

                    #if FIRST_ROOT
                        /* Choose the first root */
                        copy_gf2n(root,roots);
                    #elif DETERMINIST_ROOT
                        /* Choose a root with a determinist hash */
                        HASH((unsigned char*)hash,
                             (unsigned char*)U,NB_BYTES_GFqn);
                        copy_gf2n(root,roots+(hash[0]%l)*NB_WORD_GFqn);
                    #endif
                #endif
            }
            free(roots);
            return l;
        }
    #endif
}
#endif

