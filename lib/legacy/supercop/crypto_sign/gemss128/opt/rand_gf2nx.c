#include "rand_gf2nx.h"
#include "randombytes.h"
#include "tools_gf2n.h"




/* Input: F a table of size d+1 elements of GF(2^n) */
/* Output: random polynomial F of GF(2^n)[X] of degree at most d */
void PREFIX_NAME(rand_gf2nx)(gf2nx F, unsigned int d)
{
    ++d;
    /* Generation of random excepted for the leading term */
    randombytes((unsigned char*)F,(d*NB_WORD_GFqn)<<3);

    /* Clean the last word of each element of GF(2^n) */
    #if HFEnr
        F-=1;
        unsigned int i;
        for(i=0;i<d;++i)
        {
            F+=NB_WORD_GFqn;
            *F&=HFE_MASKn;
        }
    #endif
}



/* Input: F a table of size d+1 elements of GF(2^n) */
/* Output: random monic polynomial F of GF(2^n)[X] of degree d */
void PREFIX_NAME(randMonic_gf2nx)(gf2nx F, unsigned int d)
{
    /* Generation of random excepted for the leading term */
    randombytes((unsigned char*)F,(d*NB_WORD_GFqn)<<3);
    /* The leading term is 1 */
    set1_gf2n(F+d*NB_WORD_GFqn);

    /* Clean the last word of each element of GF(2^n), excepted the leading term */
    #if HFEnr
        F-=1;
        unsigned int i;
        for(i=0;i<d;++i)
        {
            F+=NB_WORD_GFqn;
            *F&=HFE_MASKn;
        }
    #endif
}



/* Input: F of size NB_UINT_HFEPOLY words */
/* Output: random monic HFE polynomial, coefficients in GF(2^n) */
void PREFIX_NAME(randMonicHFE_gf2nx)(sparse_monic_gf2nx F)
{
    /* Generation of random excepted for the leading term */
    randombytes((unsigned char*)F,NB_UINT_HFEPOLY<<3);

    /* Clean the last word of each element of GF(2^n) */
    #if HFEnr
        F-=1;
        unsigned int i;
        for(i=0;i<NB_COEFS_HFEPOLY;++i)
        {
            F+=NB_WORD_GFqn;
            *F&=HFE_MASKn;
        }
    #endif

    /* The leading term is 1 and is not stored */
}



#if HFEv
/* Input: F of size NB_UINT_HFEVPOLY words */
/* Output: random monic HFEv polynomial, coefficients in GF(2^n) */
void PREFIX_NAME(randMonicHFEv_gf2nx)(sparse_monic_gf2nx F)
{
    /* Generation of random excepted for the leading term */
    randombytes((unsigned char*)F,NB_UINT_HFEVPOLY<<3);

    /* Clean the last word of each element of GF(2^n) */
    #if HFEnr
        F-=1;
        unsigned int i;
        for(i=0;i<NB_COEFS_HFEVPOLY;++i)
        {
            F+=NB_WORD_GFqn;
            *F&=HFE_MASKn;
        }
    #endif

    /* The leading term is 1 and is not stored */
}
#endif
