#include "gf2nx.h"
#include "div_gf2nx.h"

#include "tools_gf2n.h"
#include "arith_gf2n.h"
#include "inv_gf2n.h"



/* Input: A an element of GF(2^n)[X], of degree deg
          B an element of GF(2^n)[X], of degree d2
   Output: A = A mod B, and d the degree of A mod B
           Q the quotient
*/
/* Requirement: B is monic
                Q is allocated by calloc */
/* In place */
unsigned int PREFIX_NAME(div_qr_gf2nx)(gf2nx Q, gf2nx A, unsigned int deg, cst_gf2nx B, \
                           unsigned int d2)
{
    static_gf2n mul_coef[NB_WORD_GFqn];
    gf2n leading_coef,res;
    unsigned int i,d=deg;
    
    /* modular reduction */
    while((int)d>=(int)d2)
    {
        /* Search the current degree of A */
        while(is0_gf2n(A+d*NB_WORD_GFqn)&&((int)d>=(int)d2))
        {
            --d;
        }

        if((int)d<(int)d2)
        {
            /* The computation of remainder is finished */
            break;
        }

        leading_coef=A+d*NB_WORD_GFqn;
        /* Update the quotient */
        copy_gf2n(Q+(d-d2)*NB_WORD_GFqn,leading_coef);
        res=leading_coef-d2*NB_WORD_GFqn;
        /* i=0: Constant of B */
        mul_gf2n(mul_coef,leading_coef,B);
        add2_gf2n(res,mul_coef);

        for(i=1;i<d2;++i)
        {
            mul_gf2n(mul_coef,leading_coef,B+i*NB_WORD_GFqn);
            res+=NB_WORD_GFqn;
            add2_gf2n(res,mul_coef);
        }

        /* The leading term becomes 0 */

        /* useless because every coefficients >= d2 will be never used */
        /* set0_gf2n(leading_coef); */
        --d;
    }

    if((int)d==(-1))
    {
        ++d;
    }

    /* Here, d=d2-1 */
    while(d&&is0_gf2n(A+d*NB_WORD_GFqn))
    {
        --d;
    }
    /* Degree of the remainder */
    return d;
}


/* Input: A an element of GF(2^n)[X], of degree deg
          B an element of GF(2^n)[X], of degree d2
   Output: A = A mod B, and d the degree of A mod B
           Q the quotient
*/
/* Requirement: Q is allocated by calloc */
/* In place */
unsigned int PREFIX_NAME(div_qr_not_monic_gf2nx)(gf2nx Q, gf2nx A, unsigned int deg, 
                                    cst_gf2nx B, unsigned int d2)
{
    static_gf2n mul_coef[NB_WORD_GFqn],leading_coef[NB_WORD_GFqn],inv[NB_WORD_GFqn];
    gf2n res;
    unsigned int i,d=deg;

    /* Compute the inverse of the leading term of B */
    inv_gf2n(inv,B+d2*NB_WORD_GFqn);

    /* B=constant */
    if(!d2)
    {
        for(i=0;i<=deg;++i)
        {
            mul_gf2n(Q+i*NB_WORD_GFqn,A+i*NB_WORD_GFqn,inv);
        }

        /* R=0 */
        set0_gf2n(A);
        return 0;
    }

    /* modular reduction */
    while(d>=d2)
    {
        /* Search the current degree of A */
        while(is0_gf2n(A+d*NB_WORD_GFqn)&&(d>=d2))
        {
            --d;
        }

        if(d<d2)
        {
            /* The computation of remainder is finished */
            break;
        }

        res=A+(d-d2)*NB_WORD_GFqn;
        mul_gf2n(leading_coef,A+d*NB_WORD_GFqn,inv);

        /* Update the quotient */
        copy_gf2n(Q+(d-d2)*NB_WORD_GFqn,leading_coef);

        /* i=0: Constant of B */
        mul_gf2n(mul_coef,leading_coef,B);
        add2_gf2n(res,mul_coef);

        for(i=1;i<d2;++i)
        {
            mul_gf2n(mul_coef,leading_coef,B+i*NB_WORD_GFqn);
            res+=NB_WORD_GFqn;
            add2_gf2n(res,mul_coef);
        }

        /* The leading term becomes 0 */

        /* useless because every coefficients >= d2 will be never used */
        /* set0_gf2n(leading_coef); */
        --d;
    }

    /* Here, d=d2-1 */
    while(is0_gf2n(A+d*NB_WORD_GFqn)&&d)
    {
        --d;
    }

    /* Degree of the remainder */
    return d;
}


/* Input: A an element of GF(2^n)[X], of degree deg
          B an element of GF(2^n)[X], of degree d2
   Output: A = A mod B, and d the degree of A mod B
*/
/* Requirement: 
    B is monic
    for the vectorial version, A and B must be aligned
*/
/* In place */
unsigned int PREFIX_NAME(div_r_gf2nx)(gf2nx A, unsigned int deg, cst_gf2nx B, \
                         unsigned int d2)
{
    static_gf2n mul_coef[NB_WORD_GFqn] ALIGNED;
    gf2n leading_coef,res;
    unsigned int i,d=deg;

    /* modular reduction */
    while(d>=d2)
    {
        /* Search the current degree of A */
        while(is0_gf2n(A+d*NB_WORD_GFqn)&&(d>=d2))
        {
            --d;
        }

        if(d<d2)
        {
            /* The computation of remainder is finished */
            break;
        }

        leading_coef=A+d*NB_WORD_GFqn;
        res=leading_coef-d2*NB_WORD_GFqn;
        /* i=0: Constant of B */
        mul_gf2n(mul_coef,leading_coef,B);
        add2_gf2n(res,mul_coef);

        for(i=1;i<d2;++i)
        {
            mul_gf2n(mul_coef,leading_coef,B+i*NB_WORD_GFqn);
            res+=NB_WORD_GFqn;
            add2_gf2n(res,mul_coef);
        }

        /* The leading term becomes 0 */

        /* useless because every coefficients >= d2 will be never used */
        /* set0_gf2n(leading_coef); */
        --d;
    }

    /* Here, d=d2-1 */
    while(is0_gf2n(A+d*NB_WORD_GFqn)&&d)
    {
        --d;
    }

    /* Degree of the remainder */
    return d;
}


/* Input: A an element of GF(2^n)[X], of degree deg
          B an element of GF(2^n)[X], of degree d2
   Output: A = A mod B, and d the degree of A mod B
*/
/* Requirement: 
    for the vectorial version, A and B must be aligned
*/
/* In place */
unsigned int PREFIX_NAME(div_r_not_monic_gf2nx)(gf2nx A, unsigned int deg, cst_gf2nx B, \
                         unsigned int d2)
{
    static_gf2n mul_coef[NB_WORD_GFqn] ALIGNED, \
                leading_coef[NB_WORD_GFqn] ALIGNED,inv[NB_WORD_GFqn] ALIGNED;
    gf2n res;
    unsigned int i,d=deg;

    /* Compute the inverse of the leading term of B */
    inv_gf2n(inv,B+d2*NB_WORD_GFqn);

    /* modular reduction */
    while(d>=d2)
    {
        /* Search the current degree of A */
        while(is0_gf2n(A+d*NB_WORD_GFqn)&&(d>=d2))
        {
            --d;
        }

        if(d<d2)
        {
            /* The computation of remainder is finished */
            break;
        }

        res=A+(d-d2)*NB_WORD_GFqn;
        mul_gf2n(leading_coef,A+d*NB_WORD_GFqn,inv);

        /* i=0: Constant of B */
        mul_gf2n(mul_coef,leading_coef,B);
        add2_gf2n(res,mul_coef);

        for(i=1;i<d2;++i)
        {
            mul_gf2n(mul_coef,leading_coef,B+i*NB_WORD_GFqn);
            res+=NB_WORD_GFqn;
            add2_gf2n(res,mul_coef);
        }

        /* The leading term becomes 0 */

        /* useless because every coefficients >= d2 will be never used */
        /* set0_gf2n(leading_coef); */
        --d;
    }

    /* Here, d=d2-1 */
    while(is0_gf2n(A+d*NB_WORD_GFqn)&&d)
    {
        --d;
    }

    /* Degree of the remainder */
    return d;
}


/* Input: poly an element of GF(2^n)[X], of degree deg
          F a HFE polynomial in GF(2^n)[X] with L a list of integer, 
          L[i] is the difference between 2 monoms of F,
          for example, X^(6 + L[6]) = X^8, X^(8 + L[7]) = X^9
          cst an element in GF(2^n)
   Output: poly mod (F-cst), and d its degree
*/
/* Requirement: F is monic 
    for the vectorial version, poly, F and cst must be aligned
*/
/* In place */
unsigned int PREFIX_NAME(div_r_HFE_gf2nx)(gf2nx poly, unsigned int deg, \
                             const complete_sparse_monic_gf2nx F, cst_gf2n cst)
{
    static_gf2n mul_coef[NB_WORD_GFqn] ALIGNED;
    gf2n leading_coef,res;
    unsigned int i,d=deg;

    const unsigned int* L=F.L;

    while(d>=HFEDeg)
    {
        /* Search the current degree of poly */
        while(is0_gf2n(poly+d*NB_WORD_GFqn)&&(d>=HFEDeg))
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
        mul_gf2n(mul_coef,leading_coef,cst);
        add2_gf2n(res,mul_coef);

        for(i=1;i<NB_COEFS_HFEPOLY;++i)
        {
            mul_gf2n(mul_coef,leading_coef,F.poly+i*NB_WORD_GFqn);
            res+=L[i];
            add2_gf2n(res,mul_coef);
        }

        /* The leading term becomes 0 */

        /* useless because every coefficients >= HFEDeg will be delete by square_gf2ex, \
        and after the call to frobeniusMap, it just keeps the coefficients < HFEDeg */
        /* set0_gf2n(leading_coef); */
        --d;
    }

    /* Here, d=HFEDeg-1 */
    while(is0_gf2n(poly+d*NB_WORD_GFqn)&&d)
    {
        --d;
    }

    /* Degree of the remainder */
    return d;
}


#if ENABLED_REMOVE_ODD_DEGREE
/* Same version in constant time */
void PREFIX_NAME(div_r_HFE_gf2nx_d_cst)(gf2nx poly, unsigned int d, \
                             const complete_sparse_monic_gf2nx F, cst_gf2n cst)
{
    static_gf2n mul_coef[NB_WORD_GFqn] ALIGNED;
    gf2n leading_coef,res;
    unsigned int i;

    const unsigned int* L=F.L;

    for(;d>(HFEDeg+HFE_odd_degree);d-=2)
    {
        leading_coef=poly+d*NB_WORD_GFqn;
        res=leading_coef-HFEDeg*NB_WORD_GFqn;
        /* i=0: Constant of F-U */
        mul_gf2n(mul_coef,leading_coef,cst);
        add2_gf2n(res,mul_coef);

        for(i=1;i<NB_COEFS_HFEPOLY;++i)
        {
            mul_gf2n(mul_coef,leading_coef,F.poly+i*NB_WORD_GFqn);
            res+=L[i];
            add2_gf2n(res,mul_coef);
        }
    }

    for(;d>=HFEDeg;--d)
    {
        leading_coef=poly+d*NB_WORD_GFqn;
        res=leading_coef-HFEDeg*NB_WORD_GFqn;
        /* i=0: Constant of F-U */
        mul_gf2n(mul_coef,leading_coef,cst);
        add2_gf2n(res,mul_coef);

        for(i=1;i<NB_COEFS_HFEPOLY;++i)
        {
            mul_gf2n(mul_coef,leading_coef,F.poly+i*NB_WORD_GFqn);
            res+=L[i];
            add2_gf2n(res,mul_coef);
        }
    }
}


/* Same version in constant time: the degree is at most 2*(HFEDeg-1) */
void PREFIX_NAME(div_r_HFE_gf2nx_cst)(gf2nx poly, \
                             const complete_sparse_monic_gf2nx F, cst_gf2n cst)
{
    static_gf2n mul_coef[NB_WORD_GFqn] ALIGNED;
    gf2n leading_coef,res;
    unsigned int i,d;

    const unsigned int* L=F.L;

    for(d=(HFEDeg-1)<<1;d>(HFEDeg+HFE_odd_degree);d-=2)
    {
        leading_coef=poly+d*NB_WORD_GFqn;
        res=leading_coef-HFEDeg*NB_WORD_GFqn;
        /* i=0: Constant of F-U */
        mul_gf2n(mul_coef,leading_coef,cst);
        add2_gf2n(res,mul_coef);

        for(i=1;i<NB_COEFS_HFEPOLY;++i)
        {
            mul_gf2n(mul_coef,leading_coef,F.poly+i*NB_WORD_GFqn);
            res+=L[i];
            add2_gf2n(res,mul_coef);
        }
    }

    /* Here, d=HFEDeg+HFE_odd_degree-1 */
    for(;d>=HFEDeg;--d)
    {
        leading_coef=poly+d*NB_WORD_GFqn;
        res=leading_coef-HFEDeg*NB_WORD_GFqn;
        /* i=0: Constant of F-U */
        mul_gf2n(mul_coef,leading_coef,cst);
        add2_gf2n(res,mul_coef);

        for(i=1;i<NB_COEFS_HFEPOLY;++i)
        {
            mul_gf2n(mul_coef,leading_coef,F.poly+i*NB_WORD_GFqn);
            res+=L[i];
            add2_gf2n(res,mul_coef);
        }
    }
}
#else
/* Same version in constant time */
void PREFIX_NAME(div_r_HFE_gf2nx_d_cst)(gf2nx poly, unsigned int d, \
                             const complete_sparse_monic_gf2nx F, cst_gf2n cst)
{
    static_gf2n mul_coef[NB_WORD_GFqn] ALIGNED;
    gf2n leading_coef,res;
    unsigned int i;

    const unsigned int* L=F.L;

    for(;d>=HFEDeg;--d)
    {
        leading_coef=poly+d*NB_WORD_GFqn;
        res=leading_coef-HFEDeg*NB_WORD_GFqn;
        /* i=0: Constant of F-U */
        mul_gf2n(mul_coef,leading_coef,cst);
        add2_gf2n(res,mul_coef);

        for(i=1;i<NB_COEFS_HFEPOLY;++i)
        {
            mul_gf2n(mul_coef,leading_coef,F.poly+i*NB_WORD_GFqn);
            res+=L[i];
            add2_gf2n(res,mul_coef);
        }
    }
}


/* Same version in constant time: the degree is at most 2*(HFEDeg-1) */
void PREFIX_NAME(div_r_HFE_gf2nx_cst)(gf2nx poly, \
                             const complete_sparse_monic_gf2nx F, cst_gf2n cst)
{
    static_gf2n mul_coef[NB_WORD_GFqn] ALIGNED;
    gf2n leading_coef,res;
    unsigned int i,d;

    const unsigned int* L=F.L;

    for(d=(HFEDeg-1)<<1;d>=HFEDeg;--d)
    {
        leading_coef=poly+d*NB_WORD_GFqn;
        res=leading_coef-HFEDeg*NB_WORD_GFqn;
        /* i=0: Constant of F-U */
        mul_gf2n(mul_coef,leading_coef,cst);
        add2_gf2n(res,mul_coef);

        for(i=1;i<NB_COEFS_HFEPOLY;++i)
        {
            mul_gf2n(mul_coef,leading_coef,F.poly+i*NB_WORD_GFqn);
            res+=L[i];
            add2_gf2n(res,mul_coef);
        }
    }
}
#endif




