#include "conv_gf2nx.h"
#include "tools_gf2n.h"
#include "inv_gf2n.h"
#include "arith_gf2n.h"


/*  Input:
        F a d-degree polynomial in gf2nx.

    Output:
        a monic version of F.

    Requirement:
        The degree of F is exactly d.
*/
/* In place */
void PREFIX_NAME(convMonic_gf2nx)(gf2nx F, unsigned int d)
{
    static_gf2n inv[NB_WORD_GFqn] ALIGNED;
    unsigned int i;

    F+=d*NB_WORD_GFqn;
    /* At this step, F is the pointer on the term X^d of F */
    inv_gf2n(inv,F);
    set1_gf2n(F);

    for(i=d-1;i!=(unsigned int)(-1);--i)
    {
        F-=NB_WORD_GFqn;
        /* At this step, F is the pointer on the term X^i of F */
        mul_gf2n(F,F,inv);
    }
}


/*  Input:
        F a sparse HFE polynomial

    Output:
        F_dense a HFE polynomial

    Requirement: coefficients of F_dense are initialized to zero
*/
void PREFIX_NAME(convHFEpolynomialSparseToDense_gf2nx)(gf2nx F_dense, \
                                          const complete_sparse_monic_gf2nx F)
{
    cst_sparse_monic_gf2nx F_cp=F.poly;
    gf2nx F_dense_cp=F_dense;
    unsigned int i;

    /* i=0: constant of F */
    copy_gf2n(F_dense,F_cp);

    for(i=1;i<NB_COEFS_HFEPOLY;++i)
    {
        F_dense_cp+=F.L[i];
        copy_gf2n(F_dense_cp,F_cp+i*NB_WORD_GFqn);
    }

    /* Leading term: 1 */
    F_dense[HFEDeg*NB_WORD_GFqn]=1UL;
}
