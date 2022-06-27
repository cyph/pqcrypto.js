#include "rand_gf2nx.h"
#include "randombytes.h"
#include "tools_gf2n.h"


/**
 * @brief   Generate a random monic d-degree polynomial in GF(2^n))[X].
 * @param[in]   d   The degree of F.
 * @param[out]  F   A random monic polynomial of GF(2^n)[X].
 * @remark  Requires to allocate d+1 elements of GF(2^n) for F.
 * @remark  Constant-time implementation when randombytes is constant-time and 
 * d is not secret.
 */
void PREFIX_NAME(randMonic_gf2nx)(gf2nx F, unsigned int d)
{
    /* Random generation of each coefficient excepted for the leading term */
    randombytes((unsigned char*)F,(d*NB_WORD_GFqn)<<3);
    /* The leading term is 1 */
    set1_gf2n(F+d*NB_WORD_GFqn);

    /* Clean the last word of each element of GF(2^n) */
    #if HFEnr
        unsigned int i;
        F-=1;
        for(i=0;i<d;++i)
        {
            F+=NB_WORD_GFqn;
            *F&=MASK_GF2n;
        }
    #endif
}


#if HFEnr
    void PREFIX_NAME(cleanMonicHFE_gf2nx)(sparse_monic_gf2nx F)
    {
        cst_sparse_monic_gf2nx F_end;

        /* zero padding for the last word of each element of GF(2^n) */
        F_end=F+NB_UINT_HFEPOLY;
        for(F+=NB_WORD_GFqn-1;F<F_end;F+=NB_WORD_GFqn)
        {
            *F&=MASK_GF2n;
        }
    }
#endif


/**
 * @brief   Generate a random monic D-degree HFE polynomial in GF(2^n))[X].
 * @param[out]  F   A random monic HFE polynomial of GF(2^n)[X].
 * @remark  Requires to allocate NB_UINT_HFEPOLY words for F.
 * @remark  Constant-time implementation when randombytes is constant-time.
 */
void PREFIX_NAME(randMonicHFE_gf2nx)(sparse_monic_gf2nx F)
{
    /* Random generation of each coefficient excepted for the leading term */
    randombytes((unsigned char*)F,NB_UINT_HFEPOLY<<3);
    /* The leading term is 1 and is not stored */

    /* Clean the last word of each element of GF(2^n) */
    #if HFEnr
        unsigned int i;
        F-=1;
        for(i=0;i<NB_COEFS_HFEPOLY;++i)
        {
            F+=NB_WORD_GFqn;
            *F&=MASK_GF2n;
        }
    #endif

    #if ENABLED_REMOVE_EVEN_DEGREE
        #if HFEs_even
            /* Set the last term to 0 */
            F-=NB_WORD_GFqn-1;
            set0_gf2n(F);
        #endif
        #if (HFEs_even>1)
            F-=NB_WORD_GFqn;
            set0_gf2n(F);
        #endif
    #endif
}


#if HFEv
    #if HFEnr
        void PREFIX_NAME(cleanMonicHFEv_gf2nx)(sparse_monic_gf2nx F)
        {
            cst_sparse_monic_gf2nx F_end;

            /* zero padding for the last word of each element of GF(2^n) */
            F_end=F+NB_UINT_HFEVPOLY;
            for(F+=NB_WORD_GFqn-1;F<F_end;F+=NB_WORD_GFqn)
            {
                *F&=MASK_GF2n;
            }
        }
    #endif


/**
 * @brief   Generate a random monic D-degree HFEv polynomial in GF(2^n))[X].
 * @param[out]  F   A random monic HFEv polynomial of GF(2^n)[X].
 * @remark  Requires to allocate NB_UINT_HFEVPOLY words for F.
 * @remark  Constant-time implementation when randombytes is constant-time.
 */
void PREFIX_NAME(randMonicHFEv_gf2nx)(sparse_monic_gf2nx F)
{
    /* Random generation of each coefficient excepted for the leading term */
    randombytes((unsigned char*)F,NB_UINT_HFEVPOLY<<3);
    /* The leading term is 1 and is not stored */

    /* Clean the last word of each element of GF(2^n) */
    #if HFEnr
        unsigned int i;
        --F;
        for(i=0;i<NB_COEFS_HFEVPOLY;++i)
        {
            F+=NB_WORD_GFqn;
            *F&=MASK_GF2n;
        }
    #endif
}
#endif

