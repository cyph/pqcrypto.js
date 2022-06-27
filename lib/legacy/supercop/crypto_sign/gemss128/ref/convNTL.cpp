#include "convNTL.hpp"
#include <NTL/GF2X.h>
#include <NTL/GF2XFactoring.h>
#include <NTL/GF2E.h>

#include "config_gf2n.h"


#define SET_COEF_I(i) \
    GF2XFromBytes(monomX,F_cp,(long)NB_BYTES_GFqn);\
    conv(monom,monomX);\
    SetCoeff(F_NTL,i,monom);


/*  Input:
        F a HFE polynomial

    Output:
        F_NTL a HFE polynomial with type GF2EX
*/
void convHFEpolynomToNTLGF2EX(GF2EX& F_NTL,cst_sparse_monic_gf2nx F)
{
    const unsigned char* F_cp=(const unsigned char*)F;
    #if(HFEDeg!=1)
        unsigned int i,j,qi;
    #endif

    /* Creation of GF2E == GF(2^n) */
    GF2X f=GF2X(HFEn,1);
    SetCoeff(f,0);
    #ifdef __PENTANOMHFE__
        SetCoeff(f,K1);
        SetCoeff(f,K2);
    #endif
    SetCoeff(f,K3);
    GF2E::init(f);
    f.kill();

    GF2X monomX;
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
        F_cp+=(NB_WORD_GFqn<<3);
        SET_COEF_I(1);

        /* Quotient */
        for(i=0;i<HFEDegI;++i)
        {
            qi=(1U<<i);
            #if ENABLED_REMOVE_ODD_DEGREE
            for(j=((qi+1)<=HFE_odd_degree)?0:1;j<=i;++j)
            #else
            for(j=0;j<=i;++j)
            #endif
            {
                /* X^(2^i + 2^j) */
                F_cp+=(NB_WORD_GFqn<<3);
                SET_COEF_I(qi+(1U<<j));
            }
        }

        /* Remainder */
        #if HFEDegJ
            qi=(1U<<i);
            #if ENABLED_REMOVE_ODD_DEGREE
            for(j=((qi+1)<=HFE_odd_degree)?0:1;j<HFEDegJ;++j)
            #else
            for(j=0;j<HFEDegJ;++j)
            #endif
            {
                /* X^(2^i + 2^j) */
                F_cp+=(NB_WORD_GFqn<<3);
                SET_COEF_I(qi+(1U<<j));
            }
        #endif

        /* j=HFEDegJ: monic case */
        /* Initialise the leading term to 1 */
        set(monom);
        SetCoeff(F_NTL,HFEDeg,monom);
    #endif

    monomX.kill();
    clear(monom);
}


/*  Input:
        F a HFE polynomial with vinegar variables

    Output:
        F_NTL a HFE polynomial with type GF2EX, without to take vinegar variables
*/
void convHFEpolynomVToNTLGF2EX(GF2EX& F_NTL,cst_sparse_monic_gf2nx F)
{
    const unsigned char* F_cp=(const unsigned char*)F;
    #if(HFEDeg!=1)
        unsigned int i,j,qi;
    #endif

    /* Creation of GF2E == GF(2^n) */
    GF2X f=GF2X(HFEn,1);
    SetCoeff(f,0);
    #ifdef __PENTANOMHFE__
        SetCoeff(f,K1);
        SetCoeff(f,K2);
    #endif
    SetCoeff(f,K3);
    GF2E::init(f);
    f.kill();

    GF2X monomX;
    GF2E monom;

    /* Constant */
    SET_COEF_I(0);
    #if(HFEDeg==1)
        /* monic case */
        /* Initialise the leading term to 1 */
        set(monom);
        SetCoeff(F_NTL,1,monom);
    #else
        /* To avoid the quadratic element of GF(2^n)[y1,...,yv] */
        F_cp+=MQv_GFqn_SIZE<<3;
        /* X^1 */
        SET_COEF_I(1);
        /* To avoid a linear element of GF(2^n)[y1,...,yv] */
        F_cp+=(MLv_GFqn_SIZE<<3);

        /* Quotient */
        for(i=0;i<HFEDegI;++i)
        {
            qi=(1U<<i);
            #if ENABLED_REMOVE_ODD_DEGREE
            for(j=((qi+1)<=HFE_odd_degree)?0:1;j<i;++j)
            #else
            for(j=0;j<i;++j)
            #endif
            {
                /* X^(2^i + 2^j) */
                SET_COEF_I(qi+(1U<<j));
                F_cp+=(NB_WORD_GFqn<<3);
            }
            /* i=j: X^(2^i + 2^i) */
            SET_COEF_I(qi<<1);
            /* To avoid a linear element of GF(2^n)[y1,...,yv] */
            F_cp+=(MLv_GFqn_SIZE<<3);
        }

        /* Remainder */
        #if HFEDegJ
            qi=(1U<<i);
            #if ENABLED_REMOVE_ODD_DEGREE
            for(j=((qi+1)<=HFE_odd_degree)?0:1;j<HFEDegJ;++j)
            #else
            for(j=0;j<HFEDegJ;++j)
            #endif
            {
                /* X^(2^i + 2^j) */
                SET_COEF_I(qi+(1U<<j));
                F_cp+=(NB_WORD_GFqn<<3);
            }
        #endif

        /* j=HFEDegJ: monic case */
        /* Initialise the leading term to 1 */
        set(monom);
        SetCoeff(F_NTL,HFEDeg,monom);
    #endif

    monomX.kill();
    clear(monom);
}


#define CONV_VEC_TO_UINT(NAME,NB_WORD_n) \
void NAME(vecn_gf2 res,const vec_GF2& S) \
{\
    const UINT* S_conv=(const UINT*)(S.rep.elts());\
    unsigned int j;\
\
    /* for each word of the ith row */\
    for(j=0;j<NB_WORD_n;++j)\
    {\
        res[j]=S_conv[j];\
    }\
}

CONV_VEC_TO_UINT(convNTLvecn_GF2ToUINT,NB_WORD_GFqn);
CONV_VEC_TO_UINT(convNTLvecnv_GF2ToUINT,NB_WORD_GFqnv);


#define CONV_UINT_TO_VEC(NAME,n,NB_WORD_n) \
void NAME(vec_GF2& res,cst_vecn_gf2 S) \
{\
    UINT* S_conv;\
    unsigned int j;\
\
    res.SetLength(n);\
    S_conv = (UINT*)(res.rep.elts());\
\
    /* for each word of the ith row */\
    for(j=0;j<NB_WORD_n;++j)\
    {\
        S_conv[j]=S[j];\
    }\
}

CONV_UINT_TO_VEC(convUINTToNTLvecv_GF2,HFEv,NB_WORD_GFqv);
CONV_UINT_TO_VEC(convUINTToNTLvecn_GF2,HFEn,NB_WORD_GFqn);
CONV_UINT_TO_VEC(convUINTToNTLvecnv_GF2,HFEnv,NB_WORD_GFqnv);



#define CONV_MAT_TO_UINT(NAME,n,NB_WORD_n) \
void NAME(Mn_gf2 res,const mat_GF2& S) \
{\
    const UINT* S_conv;\
    unsigned int i,j;\
\
    /* for each row */\
    for(i=0;i<n;++i)\
    {\
        S_conv = (UINT*)(S[i].rep.elts());\
        /* for each word of the ith row */\
        for(j=0;j<NB_WORD_n;++j)\
        {\
            res[i*NB_WORD_n+j]=S_conv[j];\
        }\
    }\
}

/* For T matrix n*n */
CONV_MAT_TO_UINT(convNTLmatT_GF2ToUINT,HFEn,NB_WORD_GFqn);
/* For S matrix (n+v)*(n+v) */
CONV_MAT_TO_UINT(convNTLmatS_GF2ToUINT,HFEnv,NB_WORD_GFqnv);


#define CONV_UINT_TO_MAT(NAME,nb_row,n,NB_WORD_n) \
void NAME(mat_GF2& res,cst_Mn_gf2 S) \
{\
    UINT* S_conv;\
    unsigned int i,j;\
\
    res.SetDims(nb_row,n);\
\
    /* for each row */\
    for(i=0;i<nb_row;++i)\
    {\
        S_conv = (UINT*)(res[i].rep.elts());\
        /* for each word of the ith row */\
        for(j=0;j<NB_WORD_n;++j)\
        {\
            S_conv[j]=S[i*NB_WORD_n+j];\
        }\
    }\
}

/* For T matrix n*n */
CONV_UINT_TO_MAT(convUINTToNTLmatT_GF2,HFEn,HFEn,NB_WORD_GFqn);
/* For S matrix (n+v)*(n+v) */
CONV_UINT_TO_MAT(convUINTToNTLmatS_GF2,HFEnv,HFEnv,NB_WORD_GFqnv);
/* For V matrix v*n */
CONV_UINT_TO_MAT(convUINTToNTLmatV_GF2,HFEv,HFEn,NB_WORD_GFqn);


/* S is a lower triangular matrix without store the zeros of upper part */
#define CONV_UINT_LOWER_TO_MAT(NAME,n,NB_WORD_n) \
void NAME(mat_GF2& res,cst_Tn_gf2 S) \
{\
    const UINT* S_cp=S;\
    UINT* S_conv;\
    unsigned int i,j;\
\
    res.SetDims(n,n);\
\
    /* for each row */\
    for(i=0;i<n;++i)\
    {\
        S_conv = (UINT*)(res[i].rep.elts());\
        /* for each word of the ith row */\
        for(j=0;j<=(i/NB_BITS_UINT);++j)\
        {\
            S_conv[j]=*S_cp;\
            ++S_cp;\
        }\
        /* Because the matrix is lower triangular */\
        for(;j<NB_WORD_n;++j)\
        {\
            S_conv[j]=0UL;\
        }\
    }\
}

CONV_UINT_LOWER_TO_MAT(convUINTlowerToNTLmatn_GF2,HFEn,NB_WORD_GFqn);
CONV_UINT_LOWER_TO_MAT(convUINTlowerToNTLmatnv_GF2,HFEnv,NB_WORD_GFqnv);

/* n variables, m equations */
/* MQS is a upper triangular matrix without store the zeros of lower part */
#define CONV_UINT_TO_MQS(NAME,n,NB_WORD_m,NB_BYTES_m) \
void NAME(mat_GF2E& res,cst_mqsn_gf2m MQS) \
{\
    const UINT *MQS_cp=MQS;\
    unsigned int i,j;\
\
    res.SetDims(n,n);\
\
    /* for each row */\
    for(i=0;i<n;++i)\
    {\
        /* for each column */\
        for(j=i;j<n;++j)\
        {\
            conv(res[i][j],GF2XFromBytes((const unsigned char*)(MQS_cp),NB_BYTES_m));\
            MQS_cp+=NB_WORD_m;\
        }\
    }\
}

CONV_UINT_TO_MQS(convUINT_pk_ToNTLmat_GF2E,HFEnv,NB_WORD_GFqm,NB_BYTES_GFqm);
#if HFEmq
    CONV_UINT_TO_MQS(convUINT_pk_hybrid_ToNTLmat_GF2E,HFEnv,HFEmq,(HFEmq<<3));
#endif
CONV_UINT_TO_MQS(convUINTToNTLmat_GF2E,HFEv,NB_WORD_GFqn,NB_BYTES_GFqn);




