#include "rem_gf2n.h"
#include "simd.h"


/***********************************************************************/
/***********************************************************************/
/************************ Without SIMD version *************************/
/***********************************************************************/
/***********************************************************************/


/**
 * @brief   Reduction in GF(2^n) of a (2n-1)-coefficients polynomial in
 * GF(2)[x].
 * @param[in]   Pol A (2n-1)-coefficients polynomial in GF(2)[x].
 * @param[out]  P   P is Pol reduced in GF(2^n).
 * @remark  Requirement: the n-degree irreducible polynomial defining GF(2^n)
 * must be a trinomial or a pentanomial.
 * @remark  Constant-time implementation.
 * @todo    >>6 and &63 are not generic for the UINT type.
 */
void PREFIX_NAME(rem_gf2n_ref)(static_gf2n P[NB_WORD_GFqn],
                               cst_static_gf2x Pol[NB_WORD_MUL])
{
    static_gf2x res[NB_WORD_MUL];
    UINT bit_i;
    unsigned int i,ind;

    for(i=0;i<NB_WORD_MUL;++i)
    {
        res[i]=Pol[i];
    }

    for(i=(HFEn-1)<<1;i>=HFEn;--i)
    {
        /* Extraction of bit_i x^i */
        bit_i=(res[i>>6]>>(i&63))&UINT_1;
        /* x^n = 1 + ... */
        ind=i-HFEn;
        res[ind>>6]^=bit_i<<(ind&63);
        #ifdef __PENTANOMIAL_GF2N__
            /* ... + x^K1 + ... */
            ind=i-HFEn+K1;
            res[ind>>6]^=bit_i<<(ind&63);
            /* ... + x^K2 + ... */
            ind=i-HFEn+K2;
            res[ind>>6]^=bit_i<<(ind&63);
        #endif
        /* ... + x^K3 */
        ind=i-HFEn+K3;
        res[ind>>6]^=bit_i<<(ind&63);
    }

    for(i=0;i<NB_WORD_GFqn;++i)
    {
        P[i]=res[i];
    }

    #if HFEnr
        P[NB_WORD_GFqn-1]&=MASK_GF2n;
    #endif
}


#if ((K3<33) || (defined(__TRINOMIAL_GF2N__)&&\
           (((HFEn==265)&&(K3==42)) || ((HFEn==266)&&(K3==47))\
         || ((HFEn==354)&&(K3==99)) || ((HFEn==358)&&(K3==57)))))
/**
 * @brief   Reduction in GF(2^n) of a (2n-1)-coefficients polynomial in
 * GF(2)[x].
 * @param[in]   Pol A (2n-1)-coefficients polynomial in GF(2)[x].
 * @param[out]  P   P is Pol reduced in GF(2^n).
 * @remark  Requirement: the n-degree irreducible polynomial defining GF(2^n)
 * must be a trinomial or a pentanomial.
 * @remark  Requirement: K3<33, or (n,K3) in {(265,42),(266,47),(354,99),
 * (358,57)}.
 * @remark  Requirement: K1<K2<33.
 * @remark  Constant-time implementation.
 */
void PREFIX_NAME(rem_gf2n_ref2)(static_gf2n P[NB_WORD_GFqn],
                                cst_static_gf2x Pol[NB_WORD_MUL])
{
    uint64_t R;
    unsigned int i;


    #if KI
        static_gf2x Q[NB_WORD_GFqn];

        /* Q: Quotient of Pol/x^n, by word of 64-bit */
        for(i=NB_WORD_GFqn;i<NB_WORD_MMUL;++i)
        {
            Q[i-NB_WORD_GFqn]=(Pol[i-1]>>KI)^(Pol[i]<<KI64);
        }
        #if (NB_WORD_MMUL&1)
            Q[i-NB_WORD_GFqn]=Pol[i-1]>>KI;
        #endif

        #if ((HFEn==354)&&(K3==99))
            R=(Q[3]>>(K364+KI))^(Q[4]<<(K3mod64-KI));
            Q[0]^=R;
            Q[1]^=(Q[4]>>(K364+KI))^(Q[5]<<(K3mod64-KI));
        #elif ((HFEn==358)&&(K3==57))
            /* R: Quotient of Pol/x^(2n-K3), by word of 64-bit */
            R=(Q[4]>>(K364+KI))^(Q[5]<<(K3-KI));
            Q[0]^=R;
        #endif

        for(i=0;i<NB_WORD_GFqn;++i)
        {
            P[i]=Pol[i]^Q[i];
        }

        #ifdef __PENTANOMIAL_GF2N__
            P[0]^=Q[0]<<K1;
            for(i=1;i<NB_WORD_GFqn;++i)
            {
                P[i]^=(Q[i-1]>>K164)^(Q[i]<<K1);
            }

            P[0]^=Q[0]<<K2;
            for(i=1;i<NB_WORD_GFqn;++i)
            {
                P[i]^=(Q[i-1]>>K264)^(Q[i]<<K2);
            }
        #endif

        #if ((HFEn==354)&&(K3==99))
            P[1]^=Q[0]<<K3mod64;
            P[2]^=(Q[0]>>K364)^(Q[1]<<K3mod64);
            P[3]^=(Q[1]>>K364)^(Q[2]<<K3mod64);
            P[4]^=(Q[2]>>K364)^(Q[3]<<K3mod64);
            P[5]^=Q[3]>>K364;
        #else
            P[0]^=Q[0]<<K3mod64;
            for(i=1;i<NB_WORD_GFqn;++i)
            {
                P[i]^=(Q[i-1]>>K364)^(Q[i]<<K3mod64);
            }
        #endif

        #if ((K3!=1) && (!((HFEn==354)&&(K3==99)))\
                     && (!((HFEn==358)&&(K3==57))))
            /* R: Quotient of Pol/x^(2n-K3), by word of 64-bit */
            #if (KI>=K3)
                R=Q[NB_WORD_GFqn-1]>>(KI-K3mod64);
            #else
                R=(Q[NB_WORD_GFqn-2]>>(K364+KI))
                 ^(Q[NB_WORD_GFqn-1]<<(K3mod64-KI));
            #endif

            #ifdef __PENTANOMIAL_GF2N__
                #if (KI>=K2)
                    R^=Q[NB_WORD_GFqn-1]>>(KI-K2);
                #else
                    R^=(Q[NB_WORD_GFqn-2]>>(K264+KI))
                      ^(Q[NB_WORD_GFqn-1]<<(K2-KI));
                #endif

                #if(K1!=1)
                    #if (KI>=K1)
                        R^=Q[NB_WORD_GFqn-1]>>(KI-K1);
                    #else
                        R^=(Q[NB_WORD_GFqn-2]>>(K164+KI))
                          ^(Q[NB_WORD_GFqn-1]<<(K1-KI));
                    #endif
                #endif
            #endif

            P[0]^=R;
            #ifdef __PENTANOMIAL_GF2N__
                P[0]^=R<<K1;
                P[0]^=R<<K2;
            #endif
            P[0]^=R<<K3mod64;
            #if (K3>32)
                (P)[1]^=R>>K364;
            #endif
        #endif
        P[NB_WORD_GFqn-1]&=MASK_GF2n;
    #else
        for(i=0;i<NB_WORD_GFqn;++i)
        {
            P[i]=Pol[i]^Pol[i+NB_WORD_GFqn];
        }

        #ifdef __PENTANOMIAL_GF2N__
            P[0]^=Pol[NB_WORD_GFqn]<<K1;
            for(i=NB_WORD_GFqn+1;i<(NB_WORD_GFqn<<1);++i)
            {
                P[i-NB_WORD_GFqn]^=(Pol[i-1]>>K164)^(Pol[i]<<K1);
            }

            P[0]^=Pol[NB_WORD_GFqn]<<K2;
            for(i=NB_WORD_GFqn+1;i<(NB_WORD_GFqn<<1);++i)
            {
                P[i-NB_WORD_GFqn]^=(Pol[i-1]>>K264)^(Pol[i]<<K2);
            }
        #endif

        P[0]^=Pol[NB_WORD_GFqn]<<K3;
        for(i=NB_WORD_GFqn+1;i<(NB_WORD_GFqn<<1);++i)
        {
            P[i-NB_WORD_GFqn]^=(Pol[i-1]>>K364)^(Pol[i]<<K3);
        }

        R=Pol[(NB_WORD_GFqn<<1)-1]>>K364;
        #ifdef __PENTANOMIAL_GF2N__
            R^=Pol[(NB_WORD_GFqn<<1)-1]>>K264;
            #if(K1!=1)
                R^=Pol[(NB_WORD_GFqn<<1)-1]>>K164;
            #endif
        #endif

        P[0]^=R;
        #ifdef __PENTANOMIAL_GF2N__
            P[0]^=R<<K1;
            P[0]^=R<<K2;
        #endif
        P[0]^=R<<K3;
    #endif
}
#endif


/**
 * @brief   Reduction in GF(2^n) of a (2n-1)-coefficients square in GF(2)[x].
 * @details The odd degree terms are assumed to be null, and so are not
 * considered.
 * @param[in]   Pol A (2n-1)-coefficients square in GF(2)[x].
 * @param[out]  P   P is Pol reduced in GF(2^n).
 * @remark  Requirement: the odd degree terms of Pol are null.
 * @remark  Requirement: the n-degree irreducible polynomial defining GF(2^n)
 * must be a trinomial or a pentanomial.
 * @remark  Constant-time implementation.
 * @todo    >>6 and &63 are not generic for the UINT type.
 */
void PREFIX_NAME(remsqr_gf2n_ref)(static_gf2n P[NB_WORD_GFqn],
                                  cst_static_gf2x Pol[NB_WORD_MUL])
{
    static_gf2x res[NB_WORD_MUL];
    UINT bit_i;
    unsigned int i,ind;

    for(i=0;i<NB_WORD_MUL;++i)
    {
        res[i]=Pol[i];
    }

    /* Only the even degree terms are not zero */
    #if (K3==1)
    for(i=(HFEn-1)<<1;i>=HFEn;i-=2)
    #elif ((HFEn-2+K3)&1)
    for(i=(HFEn-1)<<1;i>=(HFEn-1+K3);i-=2)
    #else
    for(i=(HFEn-1)<<1;i>=(HFEn-2+K3);i-=2)
    #endif
    {
        /* Extraction of bit_i x^i */
        bit_i=(res[i>>6]>>(i&63))&UINT_1;
        /* x^n = 1 + ... */
        ind=i-HFEn;
        res[ind>>6]^=bit_i<<(ind&63);
        #ifdef __PENTANOMIAL_GF2N__
            /* ... + x^K1 + ... */
            ind=i-HFEn+K1;
            res[ind>>6]^=bit_i<<(ind&63);
            /* ... + x^K2 + ... */
            ind=i-HFEn+K2;
            res[ind>>6]^=bit_i<<(ind&63);
        #endif
        /* ... + x^K3 */
        ind=i-HFEn+K3;
        res[ind>>6]^=bit_i<<(ind&63);
    }

    #if (K3>1)
        for(++i;i>=HFEn;--i)
        {
            /* Extraction of bit_i x^i */
            bit_i=(res[i>>6]>>(i&63))&UINT_1;
            /* x^n = 1 + ... */
            ind=i-HFEn;
            res[ind>>6]^=bit_i<<(ind&63);
            #ifdef __PENTANOMIAL_GF2N__
                /* ... + x^K1 + ... */
                ind=i-HFEn+K1;
                res[ind>>6]^=bit_i<<(ind&63);
                /* ... + x^K2 + ... */
                ind=i-HFEn+K2;
                res[ind>>6]^=bit_i<<(ind&63);
            #endif
            /* ... + x^K3 */
            ind=i-HFEn+K3;
            res[ind>>6]^=bit_i<<(ind&63);
        }
    #endif

    for(i=0;i<NB_WORD_GFqn;++i)
    {
        P[i]=res[i];
    }

    #if HFEnr
        P[NB_WORD_GFqn-1]&=MASK_GF2n;
    #endif
}


