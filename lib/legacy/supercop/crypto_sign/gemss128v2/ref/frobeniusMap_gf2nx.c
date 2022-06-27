#include "frobeniusMap_gf2nx.h"
#include "arch.h"
#include "gf2x.h"
#include <stdlib.h>
#include "add_gf2nx.h"
#include "sqr_gf2nx.h"
#include "div_gf2nx.h"

#include "tools_gf2n.h"
#include "add_gf2n.h"
#include "sqr_gf2n.h"
#include "mul_gf2n.h"
#include "rem_gf2n.h"


/**
 * @brief   Computation of (X^(2^n) - X) mod (F-U).
 * @param[out]  Xqn Xqn = (X^(2^n) - X) mod (F.poly-U) in GF(2^n)[X].
 * @param[in]   F   A HFE polynomial in GF(2^n)[X] stored with a sparse rep.
 * @param[in]   U   An element of GF(2^n).
 * @return  The degree of Xqn.
 * @remark  Requires to allocate (2*HFEDeg-1)*NB_WORD_GFqn words for Xqn,
 * initialized to zero.
 * @remark  Requirement: F is monic.
 * @remark  Requirement: D>1 (if D==1, X^(2^n)-X == 0 mod (F-U)).
 * @remark  Requirement: F.L must be initialized with initListDifferences_gf2nx.
 * @remark  Constant-time implementation when CONSTANT_TIME!=0.
 */
unsigned int PREFIX_NAME(frobeniusMap_HFE_gf2nx)(gf2nx Xqn, const
                                    complete_sparse_monic_gf2nx F, cst_gf2n U)
{
    static_gf2n cst[NB_WORD_GFqn];
    #if (HFEDegI==HFEDegJ)
        cst_sparse_monic_gf2nx F_cp;
        gf2nx Xqn_cp;
    #endif
    #if CONSTANT_TIME
        UINT b,mask;
    #endif
    unsigned int d,i;

    /* Constant term of F-U */
    add_gf2n(cst,F.poly,U);

    /* For i=HFEDegI, we have X^(2^i) mod (F-U) = X^(2^i).
       The first term of degree >= HFEDeg is X^(2^(HFEDegI+1)):
       2^(HFEDegI+1) >= HFEDeg but 2^HFEDegI < HFEDeg.
       So, we begin at the step i=HFEDegI+1 */
    #if (HFEDegI==HFEDegJ)
        /* Compute X^(2^(HFEDegI+2)) mod (F-U) */

        /* We have X^D = X^(2^HFEDegI + 2^HFEDegJ) = X^(2^(HFEDegI+1)).
           So, X^(2^(HFEDegI+1)) mod (F-U) = F-U - X^D.
           Then, X^(2^(HFEDegI+2)) = (F-U - X^D)^2 mod (F-U) */

        /* Step 1: compute (F-U - X^D)^2 */
        F_cp=F.poly;
        Xqn_cp=Xqn;

        sqr_gf2n(Xqn_cp,cst);
        for(i=1;i<NB_COEFS_HFEPOLY;++i)
        {
            F_cp+=NB_WORD_GFqn;
            /* Multiplication by 2 to have the coefficient of the square */
            Xqn_cp+=(F.L[i])<<1;
            sqr_gf2n(Xqn_cp,F_cp);
        }
        /* Degree of (F-U - X^D)^2 */
        #if(HFEDeg==2)
            d=2;
        #else
            d=HFEDeg+(1U<<HFEDegI);
        #endif

        /* Step 2: reduction of (F-U - X^D)^2 modulo (F-U) */
        #if CONSTANT_TIME
            divsqr_r_HFE_cstdeg_gf2nx(Xqn,d,F,cst);
        #else
            d=div_r_HFE_gf2nx(Xqn,d,F,cst);
        #endif

    for(i=HFEDegI+2;i<HFEn;++i)
    #else
        /* Compute X^(2^(HFEDegI+1)) mod (F-U) */

        /* Step 1: compute X^(2^(HFEDegI+1)) */
        d=2U<<HFEDegI;
        /* Xqn is initialized to 0 with calloc, so the multiprecision word is 
           initialized to 1 just by setting the first word */
        Xqn[d*NB_WORD_GFqn]=1;

        /* Step 2: reduction of X^(2^(HFEDegI+1)) modulo (F-U) */
        #if CONSTANT_TIME
            divsqr_r_HFE_cstdeg_gf2nx(Xqn,d,F,cst);
        #else
            d=div_r_HFE_gf2nx(Xqn,d,F,cst);
        #endif

    for(i=HFEDegI+1;i<HFEn;++i)
    #endif
    {
        #if CONSTANT_TIME
            /* Step 1: (X^(2^i) mod (F-U))^2 = X^(2^(i+1)) */
            sqr_HFE_gf2nx(Xqn);
            /* Step 2: X^(2^(i+1)) mod (F-U) */
            divsqr_r_HFE_cst_gf2nx(Xqn,F,cst);
        #else
            /* Step 1: (X^(2^i) mod (F-U))^2 = X^(2^(i+1)) */
            sqr_gf2nx(Xqn,d);
            /* Step 2: X^(2^(i+1)) mod (F-U) */
            d=div_r_HFE_gf2nx(Xqn,d<<1U,F,cst);
        #endif
    }

    /* (X^(2^n) mod (F-U)) - X */
    Xqn[NB_WORD_GFqn]^=1;

    /* Search the degree of X^(2^n) - X mod (F-U) */
    #if CONSTANT_TIME
        d=0;
        mask=0;

        for(i=HFEDeg-1;i>0;--i)
        {
            b=isNot0_gf2n(Xqn+i*NB_WORD_GFqn);
            mask|=b;
            /* We add 1 to d as soon as we exceed all left zero coefficients */
            d+=mask;
        }
    #else
        if(d==1)
        {
            if(is0_gf2n(Xqn+NB_WORD_GFqn))
            {
                d=0;
            }
        }
    #endif

    return d;
}


