#include "findRootsSplit_gf2nx.h"
#include "rand_gf2n.h"
#include "gcd_gf2nx.h"
#include "inv_gf2n.h"
#include "tools_gf2n.h"
#include "arith_gf2n.h"
#include "div_gf2nx.h"
#include <string.h>



/* To return the degree of poly_frob in traceMap_gf2nx */
#define DEGREE_FROB 0


#include "add_gf2nx.h"
#include "sqr_gf2nx.h"
/* Input: poly_trace = rX with r in GF(2^n)
          poly_frob a tmp buffer used to store the Frobenius map of poly_trace
          f a "deg" degree polynomial

   Output: poly_trace = sum from i=0 to n-1 of ((rX)^i mod f)
           poly_frob = (rX)^(n-1) mod f

   Requirement: f is monic, with its degree deg>1 
                poly_frob is initialized to zero
*/
#if DEGREE_FROB
static unsigned int traceMap_gf2nx(gf2nx poly_trace, gf2nx poly_frob, \
                                   cst_gf2nx f, unsigned int deg)
#else
static void traceMap_gf2nx(gf2nx poly_trace, gf2nx poly_frob, \
                           cst_gf2nx f, unsigned int deg)
#endif
{
    /* d is the degree of poly_trace */
    unsigned int j,i=1,d;

    /* 2^i < deg does not require modular reduction by f */
    #if(HFEn<33)
    unsigned int min=(deg<(1U<<HFEn))?deg:HFEn;
    while((1U<<i)<min)
    #else
    while((1U<<i)<deg)
    #endif
    {
        sqr_gf2n(poly_trace+(NB_WORD_GFqn<<i),poly_trace+(NB_WORD_GFqn<<(i-1)));
        ++i;
    }

    /* (rX)^(2^(i+1)) is the first time where we need modular reduction */
    #if DEGREE_FROB
        d=1U<<(i-1);
    #endif
    if(i<HFEn)
    {
        /* Initialization of poly_frob */
        sqr_gf2n(poly_frob+(NB_WORD_GFqn<<i),poly_trace+(NB_WORD_GFqn<<(i-1)));
        d=div_r_gf2nx(poly_frob,1U<<i,f,deg);
        add2_gf2nx(poly_trace,poly_frob,d+1,j);
        ++i;

        for(;i<HFEn;++i)
        {
            sqr_gf2nx(poly_frob,d);
            d=div_r_gf2nx(poly_frob,d<<1U,f,deg);
            add2_gf2nx(poly_trace,poly_frob,d+1,j);
        }
    }

    #if DEGREE_FROB
        return d;
    #endif
}



/* Input: f a deg degree polynomial in GF(2^n)[X]
   Output: roots the vector of roots in GF(2^n) of f

   Requirement: f is split and monic, with its degree deg>0 */
void PREFIX_NAME(findRootsSplit_gf2nx)(vec_gf2n roots, gf2nx f, unsigned int deg)
{
    if(deg==1)
    {
        /* Extract the uniq root which is the constant of f */
        copy_gf2n(roots,f);
        return;
    }

    gf2nx poly_frob,poly_trace,f_cp,tmp_p;
    static_gf2n inv[NB_WORD_GFqn] ALIGNED;
    unsigned int b,i,l,d;

    /* The double of memory is allocated for the squaring */
    ALIGNED_MALLOC(poly_frob,UINT*,((deg<<1)-1)*NB_WORD_GFqn,sizeof(UINT));
    /* poly_trace is modulo f, this degree is strictly less than deg */
    ALIGNED_MALLOC(poly_trace,UINT*,deg*NB_WORD_GFqn,sizeof(UINT));
    /* f_cp a copy of f */
    ALIGNED_MALLOC(f_cp,UINT*,(deg+1)*NB_WORD_GFqn,sizeof(UINT));
    do
    {
        /* Set poly_frob to zero */
        memset((void*)poly_frob,0,((deg<<1)-1)*NB_WORD_GFqn*sizeof(UINT));
        /* Set poly_trace to zero */
        memset((void*)poly_trace,0,deg*NB_WORD_GFqn*sizeof(UINT));

        /* Initialization to rX */
        /* Probability 2^(-n) to find 0 with a correct RNG */
        do
        {
            rand_gf2n(poly_trace+NB_WORD_GFqn);
        } while(is0_gf2n(poly_trace+NB_WORD_GFqn));

        /* copy of f because the gcd modifies f */
        copy_gf2nx(f_cp,f,deg+1,l);

        /* d: degree of poly_trace */
        traceMap_gf2nx(poly_trace,poly_frob,f_cp,deg);
        /* Degree of poly_trace */
        d=deg-1;
        while(is0_gf2n(poly_trace+d*NB_WORD_GFqn)&&d)
        {
            --d;
        }
        l=gcd_gf2nx(&b,f_cp,deg,poly_trace,d);

    } while((!l)||(l==deg));
    free(poly_frob);

    if(b)
    {
        tmp_p=poly_trace;
        poly_trace=f_cp;
        f_cp=tmp_p;
    }
    /* Here, f_cp is a non-trivial divisor of degree l */

    /* f_cp is the gcd */
    /* Here, it becomes monic */
    inv_gf2n(inv,f_cp+l*NB_WORD_GFqn);
    set1_gf2n(f_cp+l*NB_WORD_GFqn);
    for(i=l-1;i!=(unsigned int)(-1);--i)
    {
        mul_gf2n(f_cp+i*NB_WORD_GFqn,f_cp+i*NB_WORD_GFqn,inv);
    }


    /* Set poly_trace to zero */
    memset((void*)poly_trace,0,(deg-l+1)*NB_WORD_GFqn*sizeof(UINT));

    /* f = f_cp * poly_trace */
    /* This function destroyes f */
    div_qr_gf2nx(poly_trace,f,deg,f_cp,l);
    /* Necessarily, the polynomial f is null here */

    /* f_cp is monic */
    /* We can apply findRootsSplit_gf2nx recursively */
    findRootsSplit_gf2nx(roots,f_cp,l);
    free(f_cp);

    /* f is monic and f_cp is monic so poly_trace is monic */
    /* We can apply findRootsSplit_gf2nx recursively */
    findRootsSplit_gf2nx(roots+l*NB_WORD_GFqn,poly_trace,deg-l);
    free(poly_trace);
}


