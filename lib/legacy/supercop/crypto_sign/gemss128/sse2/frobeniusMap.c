#include "frobeniusMap.h"
#include "init.h"
#include "arith.h"


/* Input: poly an element of GF(2^n)[X], of degree d
   Output: the square of poly
*/
/* In place */
static void square_gf2ex(gf2nx poly, unsigned int d)
{
    /* Beginning: last coefficient */
    unsigned int i=NB_WORD_GFqn*d;
    gf2nx poly_i=poly+i;
    gf2nx poly_2i=poly_i+i;
    /* for each coefficient */
    for(i=0;i<d;++i)
    {
        hfesquaremod(poly_2i,poly_i);
        poly_i-=NB_WORD_GFqn;
        poly_2i-=NB_WORD_GFqn;
        HFESET0(poly_2i);
        poly_2i-=NB_WORD_GFqn;
    }

    /* Constant coefficient */
    hfesquaremod(poly,poly);
}

/* Input: poly an element of GF(2^n)[X], of degree deg
          F a HFE polynomial in GF(2^n)[X] with L a list of integer, 
          L[i] is the difference between 2 monoms of F,
          for example, X^(6 + L[6]) = X^8, X^(8 + L[7]) = X^9
          cst an element in GF(2^n)
   Output: poly mod (F-cst)
*/
/* Requirement: F is monic */
/* In place */
static unsigned int mod_gf2ex(gf2nx poly, unsigned int deg, const complete_sparse_monic_gf2nx F, cst_gf2n cst)
{
    static_gf2n mul_coef[NB_WORD_GFqn];
    gf2n leading_coef,res;
    unsigned int i,d=deg;

    const unsigned int* L=F.L;

    while(d>=HFEDeg)
    {
        /* Search the degree of poly */
        while(HFEISZERO(poly+d*NB_WORD_GFqn)&&(d>=HFEDeg))
        {
            --d;
        }

        if(d<HFEDeg)
        {
            /* The computation of remainder is finished */
            break;
        }

        leading_coef=poly+d*NB_WORD_GFqn;
        res=leading_coef-HFEDeg*NB_WORD_GFqn;
        /* i=0: Constant of F-U */
        hfemulmod(mul_coef,leading_coef,cst);
        HFEADD2(res,mul_coef);

        for(i=1;i<NB_COEFS_HFEPOLY_EVAL;++i)
        {
            hfemulmod(mul_coef,leading_coef,F.poly+i*NB_WORD_GFqn);
            res+=L[i];
            HFEADD2(res,mul_coef);
        }

        /* The leading term becomes 0 */

        /* useless because every coefficients >= HFEDeg will be delete by square_gf2ex, \
        and after the call to frobeniusMap, it just keeps the coefficients < HFEDeg */
        /* HFESET0(leading_coef); */
        --d;
    }

    /* Here, d=HFEDeg-1 */
    while(HFEISZERO(poly+d*NB_WORD_GFqn)&&d)
    {
        --d;
    }
    return d;
}

/* Input: F a HFE polynomial in GF(2^n)[X], with L a list of integer
          U in GF(2^n)
   Output: Xqn = X^(2^n) - X mod (F-U)
*/
/* Requirement: F is monic */
void frobeniusMap(gf2nx Xqn, const complete_sparse_monic_gf2nx F, cst_gf2n U)
{
    static_gf2n cst[NB_WORD_GFqn];
    unsigned int d,i;

    HFEADD(cst,F.poly,U);

    /* i=HFEDegI */
    /* The first term of degree >= HFEDeg is X^(2^(HFEDegI+1)) */
    /* Because 2^(HFEDegI+1) >= HFEDeg but 2^(HFEDegI) < HFEDeg */
    #if (HFEDegI==HFEDegJ)
        /* This step is the case i=HFEDegI and i=HFEDegI+1 */

        /* X^(2^HFEDegI+1) = F - X^(2^HFEDegI+1) mod F */
        /* Compute (F - X^(2^HFEDegI+1))^2 */
        cst_sparse_monic_gf2nx F_cp=F.poly;
        gf2nx Xqn_cp=Xqn;

        hfesquaremod(Xqn_cp,cst);
        for(i=1;i<NB_COEFS_HFEPOLY_EVAL;++i)
        {
            F_cp+=NB_WORD_GFqn;
            /* Multiply by 2 to have the coefficient of the square */
            Xqn_cp+=(F.L[i])<<1;
            hfesquaremod(Xqn_cp,F_cp);
        }
        #if(HFEDeg==2)
            d=2;
        #else
            d=HFEDeg+(1U<<HFEDegI);
        #endif
        /* (F - X^(2^HFEDegI+1))^2 mod F */
        d=mod_gf2ex(Xqn,d,F,cst);

    /* HFEDegI < HFEn-1 */
    for(i=HFEDegI+2;i<HFEn;++i)
    #else
        /* Compute X^(2^HFEDegI+1) mod F */
        d=2U<<HFEDegI;
        /* Xqn is initialized to 0 with calloc, so the multiprecision word is 
         * initialized to 1 with just the first word */
        Xqn[d*NB_WORD_GFqn]=1UL;
        d=mod_gf2ex(Xqn,d,F,cst);

    /* HFEDegI < HFEn */
    for(i=HFEDegI+1;i<HFEn;++i)
    #endif
    {
        square_gf2ex(Xqn,d);
        d=mod_gf2ex(Xqn,d<<1U,F,cst);
    }

    /* X^(2^n) - X */
    Xqn[NB_WORD_GFqn]^=1UL;
}


