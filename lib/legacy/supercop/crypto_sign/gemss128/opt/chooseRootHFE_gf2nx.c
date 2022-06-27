#include "chooseRootHFE_gf2nx.h"

#ifndef UNIQ_ROOT


#include "tools_gf2n.h"
#include "findRoots_gf2nx.h"
#include "quickSort_gf2n.h"
#include "arith_gf2n.h"

#if DETERMINIST_ROOT
    #include "hash.h"
#endif


/*  Input:
        F a HFE polynomial in GF(2^n)[X]
        U a element of GF(2^n)

    Output:
        the number of roots of F-U
        root a root of F-U

    Requirement: F is monic
        for the vectorial version, F and U must be aligned
*/
int PREFIX_NAME(chooseRootHFE_gf2nx)(gf2n root, \
                               const complete_sparse_monic_gf2nx F, cst_gf2n U)
{
    #if (HFEDeg==1)
        add_gf2n(root,F.poly,U);
        return 1;
    #else

        #if DETERMINIST_ROOT
            UINT hash[SIZE_DIGEST_UINT];
        #endif

        vec_gf2n roots;
        int l;
        l=findRootsHFE_gf2nx(&roots,F,U);

        if(!l)
        {
            return 0;
        } else
        {
            if(l==1)
            {
                copy_gf2n(root,roots);
            } else
            {
                /* Sort the roots */
                quickSort_gf2n(roots,0,(l-1)*NB_WORD_GFqn);

                #if FIRST_ROOT
                    /* Choose the first root */
                    copy_gf2n(root,roots);
                #elif DETERMINIST_ROOT
                    /* Choose a root with a determinist hash */
                    HASH((unsigned char*)hash,(unsigned char*)U,NB_BYTES_GFqn);
                    copy_gf2n(root,roots+(hash[0]%l)*NB_WORD_GFqn);
                #endif
            }
            free(roots);
            return l;
        }
    #endif
}

#endif
