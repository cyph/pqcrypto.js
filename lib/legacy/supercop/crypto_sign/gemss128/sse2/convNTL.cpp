#include "convNTL.hpp"
#include <NTL/GF2X.h>
#include <NTL/GF2XFactoring.h>
#include <NTL/GF2E.h>


#define SET_COEF_I(i) \
    GF2XFromBytes(monomX,F_cp,(long)NB_BYTES_GFqn);\
    conv(monom,monomX);\
    SetCoeff(F_NTL,i,monom);\
    F_cp+=(NB_WORD_GFqn<<3);


/*  Input:
        F a HFE polynom

    Output:
        F_NTL a HFE polynom with type GF2EX
*/
void convHFEpolynomToNTLGF2EX(GF2EX& F_NTL,cst_sparse_monic_gf2nx F)
{
    const unsigned char* F_cp=(const unsigned char*)F;
    #if(HFEDeg!=1)
        unsigned int i,j,qi;
    #endif

    GF2X f=GF2X(HFEn,1),monomX;
    SetCoeff(f,0);
    #ifdef __PENTANOMHFE__
        SetCoeff(f,K1);
        SetCoeff(f,K2);
    #endif
    SetCoeff(f,K3);
    GF2E::init(f);
    f.kill();

    GF2E monom;

    /* Constant */
    SET_COEF_I(0);
    #if(HFEDeg==1)
        /* monic case */
        /* Initialise the leading term to 1 */
        set(monom);
        SetCoeff(F_NTL,1,monom);
    #else
        /* X^1 */
        SET_COEF_I(1);

        /* Quotient */
        for(i=0;i<HFEDegI;++i)
        {
            qi=(1U<<i);
            for(j=0;j<=i;++j)
            {
                /* X^(2^i + 2^j) */
                SET_COEF_I(qi+(1U<<j));
            }
        }

        /* Remainder */
        qi=(1U<<i);
        for(j=0;j<HFEDegJ;++j)
        {
            /* X^(2^i + 2^j) */
            SET_COEF_I(qi+(1U<<j));
        }

        /* j=HFEDegJ: monic case */
        /* Initialise the leading term to 1 */
        set(monom);
        SetCoeff(F_NTL,qi+(1U<<j),monom);
    #endif

    monomX.kill();
    clear(monom);
}


/*  Input:
        F a polynom

    Output:
        F_NTL a polynom with type GF2EX
*/
void convPolynomToNTLGF2EX(GF2EX& F_NTL,const UINT* F)
{
    const unsigned char* F_cp=(const unsigned char*)F;
    unsigned int i;

    GF2X f=GF2X(HFEn,1),monomX;
    SetCoeff(f,0);
    #ifdef __PENTANOMHFE__
        SetCoeff(f,K1);
        SetCoeff(f,K2);
    #endif
    SetCoeff(f,K3);
    GF2E::init(f);
    f.kill();

    GF2E monom;

    for(i=0;i<HFEDeg;++i)
    {
        SET_COEF_I(i);
    }

    monomX.kill();
    clear(monom);
}
