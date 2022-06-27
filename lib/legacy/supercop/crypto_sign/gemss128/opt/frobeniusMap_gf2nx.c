#include "frobeniusMap_gf2nx.h"
#include "add_gf2nx.h"
#include "sqr_gf2nx.h"
#include "div_gf2nx.h"

#include "tools_gf2n.h"
#include "arith_gf2n.h"



/* Requirement: F is monic 
                Xqn is allocated by calloc */
unsigned int PREFIX_NAME(frobeniusMap_gf2nx)(gf2nx Xqn, cst_gf2nx F, unsigned int df)
{
    unsigned int i,j,d;

    #if (HFEn<32)
        d=(1U<<HFEn);
        if(d<df)
        {
            /* X^(2^n) mod F = X^(2^n) */
            Xqn[d*NB_WORD_GFqn]=1UL;
            /* X^(2^n) - X */
            Xqn[NB_WORD_GFqn]=1UL;
            return d;
        }
    #endif

    for(i=0;(1U<<i)<df;++i);
    d=(1U<<i);

    /* Compute X(2^i) mod F */

    /* X^(2^i) mod F = (X^((2^i)-df) * (F-X^df)) mod F */
    /* Copy all coefficients of F excepted the LT */
    copy_gf2nx(Xqn+NB_WORD_GFqn*(d-df),F,df,j);
    --d;
    if(d>=df)
    {
        d=div_r_gf2nx(Xqn,d,F,df);
    }

    for(;i<HFEn;++i)
    {
        /* Compute X^(2^(i+1)) mod F */
        sqr_gf2nx(Xqn,d);
        d=div_r_gf2nx(Xqn,d<<1U,F,df);
    }

    /* X^(2^n) - X */
    Xqn[NB_WORD_GFqn]^=1UL;

    if(d==1)
    {
        if(is0_gf2n(Xqn+NB_WORD_GFqn))
        {
            d=0;
        }
    }

    return d;
}



/* Input: F a HFE polynomial in GF(2^n)[X], with L a list of integer
          U an element of GF(2^n)
   Output: Xqn = X^(2^n) - X mod (F-U)
           d the degree of Xqn
*/
/* Requirement: 
    F is monic 
    for the vectorial version, Xqn, F and U must be aligned
*/
unsigned int PREFIX_NAME(frobeniusMap_HFE_gf2nx)(gf2nx Xqn, const \
                                    complete_sparse_monic_gf2nx F, cst_gf2n U)
{
    static_gf2n cst[NB_WORD_GFqn] ALIGNED;
    unsigned int d,i;

    add_gf2n(cst,F.poly,U);

    /* i=HFEDegI */
    /* The first term of degree >= HFEDeg is X^(2^(HFEDegI+1)) */
    /* Because 2^(HFEDegI+1) >= HFEDeg but 2^(HFEDegI) < HFEDeg */
    #if (HFEDegI==HFEDegJ)
        /* This step is the case i=HFEDegI and i=HFEDegI+1 */

        /* X^(2^HFEDegI+1) = F - X^(2^HFEDegI+1) mod F */
        /* Compute (F - X^(2^HFEDegI+1))^2 */
        cst_sparse_monic_gf2nx F_cp=F.poly;
        gf2nx Xqn_cp=Xqn;

        sqr_gf2n(Xqn_cp,cst);
        for(i=1;i<NB_COEFS_HFEPOLY;++i)
        {
            F_cp+=NB_WORD_GFqn;
            /* Multiply by 2 to have the coefficient of the square */
            Xqn_cp+=(F.L[i])<<1;
            sqr_gf2n(Xqn_cp,F_cp);
        }
        #if(HFEDeg==2)
            d=2;
        #else
            d=HFEDeg+(1U<<HFEDegI);
        #endif
        /* (F - X^(2^HFEDegI+1))^2 mod F */
        #if CONSTANT_TIME
            div_r_HFE_gf2nx_d_cst(Xqn,d,F,cst);
        #else
            d=div_r_HFE_gf2nx(Xqn,d,F,cst);
        #endif

    /* HFEDegI < HFEn-1 */
    for(i=HFEDegI+2;i<HFEn;++i)
    #else
        /* Compute X^(2^HFEDegI+1) mod F */
        d=2U<<HFEDegI;
        /* Xqn is initialized to 0 with calloc, so the multiprecision word is 
         * initialized to 1 with just the first word */
        Xqn[d*NB_WORD_GFqn]=1UL;
        #if CONSTANT_TIME
            div_r_HFE_gf2nx_d_cst(Xqn,d,F,cst);
        #else
            d=div_r_HFE_gf2nx(Xqn,d,F,cst);
        #endif

    /* HFEDegI < HFEn */
    for(i=HFEDegI+1;i<HFEn;++i)
    #endif
    {
        #if CONSTANT_TIME
            sqr_HFE_gf2nx(Xqn);
            div_r_HFE_gf2nx_cst(Xqn,F,cst);
        #else
            sqr_gf2nx(Xqn,d);
            d=div_r_HFE_gf2nx(Xqn,d<<1U,F,cst);
        #endif
    }

    /* X^(2^n) - X */
    Xqn[NB_WORD_GFqn]^=1UL;

    #if CONSTANT_TIME
        d=HFEDeg-1;
        while(is0_gf2n(Xqn+d*NB_WORD_GFqn)&&d)
        {
            --d;
        }
    #endif
    if(d==1)
    {
        if(is0_gf2n(Xqn+NB_WORD_GFqn))
        {
            d=0;
        }
    }

    return d;
}



/* Requirement: 
                the first polynomial is initialized to 0,
                and also the POW_II first coefficients of the second polynomial
                the table requires (KX*HFEDeg+POW_II)*NB_WORD_GFqn UINT
                cst is the constant of F
*/
static void precompute_table(gf2nx table, const complete_sparse_monic_gf2nx F, cst_gf2n cst)
{
    static_gf2n mul_coef[NB_WORD_GFqn] ALIGNED;
    gf2nx table_cp;
    gf2n leading_coef;
    const unsigned int* L=F.L;
    unsigned int k,j,i;

    /* Case k=KP: compute X^(k*(2^II)) mod F */

    /* j=POW_II*KP-HFEDeg */
    j=POW_II*KP-HFEDeg;
    /* i=0: constant of F */
    table_cp=table+NB_WORD_GFqn*j;
    copy_gf2n(table_cp,cst);

    for(i=1;i<NB_COEFS_HFEPOLY;++i)
    {
        table_cp+=L[i];
        copy_gf2n(table_cp,F.poly+i*NB_WORD_GFqn);
    }

    --j;
    /* Reduce leading_coef*X^(HFEDeg+j) mod F */
    /* X^(D+j) = X^j * X^D = X^j * (F-X^D) */
    for(;j!=(unsigned int)(-1);--j)
    {
        leading_coef=table+(j+HFEDeg)*NB_WORD_GFqn;

        /* i=0: constant of F */
        table_cp=table+NB_WORD_GFqn*j;
        mul_gf2n(mul_coef,leading_coef,cst);
        add2_gf2n(table_cp,mul_coef);

        for(i=1;i<NB_COEFS_HFEPOLY;++i)
        {
            table_cp+=L[i];
            mul_gf2n(mul_coef,leading_coef,F.poly+i*NB_WORD_GFqn);
            add2_gf2n(table_cp,mul_coef);
        }
    }

    /* Compute X^(k*(2^i)) mod (F-U) */
    for(k=KP+1;k<HFEDeg;++k)
    {
        /* Update the current polynomial */
        table_cp=table+HFEDeg*NB_WORD_GFqn;

        for(j=0;j<POW_II;++j)
        {
            set0_gf2n(table_cp+j*NB_WORD_GFqn);
        }

        table_cp+=POW_II*NB_WORD_GFqn;
        for(j=0;j<HFEDeg;++j)
        {
            copy_gf2n(table_cp+j*NB_WORD_GFqn,table+j*NB_WORD_GFqn);
        }
        table_cp-=POW_II*NB_WORD_GFqn;


        /* Update the current polynomial */
        table=table_cp;

        /* Reduce leading_coef*X^(HFEDeg+j) mod F */
        /* X^(D+j) = X^j * X^D = X^j * (F-X^D) */
        for(j=POW_II-1;j!=(unsigned int)(-1);--j)
        {
            leading_coef=table+(j+HFEDeg)*NB_WORD_GFqn;

            /* i=0: constant of F */
            table_cp=table+NB_WORD_GFqn*j;
            mul_gf2n(mul_coef,leading_coef,cst);
            add2_gf2n(table_cp,mul_coef);

            for(i=1;i<NB_COEFS_HFEPOLY;++i)
            {
                table_cp+=L[i];
                mul_gf2n(mul_coef,leading_coef,F.poly+i*NB_WORD_GFqn);
                add2_gf2n(table_cp,mul_coef);
            }
        }
    }
}


/* Input: F a HFE polynomial in GF(2^n)[X], with L a list of integer
          U an element of GF(2^n)
   Output: Xqn = X^(2^n) - X mod (F-U)
           d the degree of Xqn
*/
/* Requirement: 
    F is monic 
    for the vectorial version, Xqn, F and U must be aligned
*/
unsigned int PREFIX_NAME(frobeniusMap_multisqr_HFE_gf2nx)(gf2nx Xqn, const \
                                    complete_sparse_monic_gf2nx F, cst_gf2n U)
{
    static_gf2n cst[NB_WORD_GFqn] ALIGNED, mul_coef[NB_WORD_GFqn] ALIGNED;
    gf2nx table,table_cp,Xqn_sqr,Xqn_cp;
    gf2n current_coef;
    unsigned int d,i,j,k;

    /* Constant of F-U */
    add_gf2n(cst,F.poly,U);

    table=(UINT*)malloc((KX*HFEDeg+POW_II)*NB_WORD_GFqn*sizeof(UINT));
    /* The first polynomial is initialized to 0 */
    for(i=0;i<(HFEDeg+POW_II);++i)
    {
        set0_gf2n(table+i*NB_WORD_GFqn);
    }
    precompute_table(table,F,cst);

    /* X^(2^(HFEDegI+II)) = X^( (2^HFEDegI) * (2^II)) */
    /* We take the polynomial from the table */
    for(i=0;i<HFEDeg;++i)
    {
        copy_gf2n(Xqn+i*NB_WORD_GFqn,table+(((1UL<<HFEDegI)-KP)*HFEDeg+i)*NB_WORD_GFqn);
    }

    Xqn_sqr=(UINT*)calloc(HFEDeg*NB_WORD_GFqn,sizeof(UINT));
    for(i=0;i<((HFEn-HFEDegI-II)/II);++i)
    {
        /* II squarings */

        /* j=0 */
        for(k=0;k<HFEDeg;++k)
        {
            sqr_gf2n(Xqn_sqr+k*NB_WORD_GFqn,Xqn+k*NB_WORD_GFqn);
        }

        for(j=1;j<II;++j)
        {
            for(k=0;k<HFEDeg;++k)
            {
                sqr_gf2n(Xqn_sqr+k*NB_WORD_GFqn,Xqn_sqr+k*NB_WORD_GFqn);
            }
        }

        /* Use the table */

        /* j=KP */
        current_coef=Xqn_sqr+KP*NB_WORD_GFqn;
        table_cp=table;
        Xqn_cp=Xqn;
        for(k=0;k<HFEDeg;++k)
        {
            mul_gf2n(Xqn_cp,table_cp,current_coef);
            table_cp+=NB_WORD_GFqn;
            Xqn_cp+=NB_WORD_GFqn;
        }

        for(j=KP+1;j<HFEDeg;++j)
        {
            current_coef+=NB_WORD_GFqn;
            Xqn_cp=Xqn;
            for(k=0;k<HFEDeg;++k)
            {
                mul_gf2n(mul_coef,table_cp,current_coef);
                add2_gf2n(Xqn_cp,mul_coef);
                table_cp+=NB_WORD_GFqn;
                Xqn_cp+=NB_WORD_GFqn;
            }
        }

        /* Before to use the table */
        for(j=0;j<KP;++j)
        {
            /* (X^j)^(POW_II) */
            add2_gf2n(Xqn+j*POW_II*NB_WORD_GFqn,Xqn_sqr+j*NB_WORD_GFqn);
        }
    }

    free(table);
    free(Xqn_sqr);

    #if ((HFEn-HFEDegI)%II)

    for(i=0;i<((HFEn-HFEDegI)%II);++i)
    {
        sqr_HFE_gf2nx(Xqn);
        div_r_HFE_gf2nx_cst(Xqn,F,cst);
    }
    #endif

    /* X^(2^n) - X */
    Xqn[NB_WORD_GFqn]^=1UL;

    d=HFEDeg-1;
    while(is0_gf2n(Xqn+d*NB_WORD_GFqn)&&d)
    {
        --d;
    }

    if(d==1)
    {
        if(is0_gf2n(Xqn+NB_WORD_GFqn))
        {
            d=0;
        }
    }

    return d;
}










