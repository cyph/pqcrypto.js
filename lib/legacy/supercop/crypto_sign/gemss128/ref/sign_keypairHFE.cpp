#include "sign_keypairHFE.hpp"
#include "init.h"
#include "config_HFE.h"

#include "convNTL.hpp"
#include "genHFEtab.hpp"
#include "genSecretMQS.hpp"

#include "config_gf2n.h"

#if HYBRID_REPRESENTATION_PK
    #include "convMQS_gf2.h"
#endif

#include <NTL/mat_GF2.h>
#include <NTL/mat_GF2E.h>
#include <NTL/GF2EX.h>
#include <NTL/GF2XFactoring.h>


/* Choose the method to generate random invertible matrix */
/* Choose !=0 for LU and 0 for classical */
#define GEN_INVERTIBLE_MATRIX_LU 1


/* Generates a keypair - pk is the public key and sk is the secret key. */

/*  Output :
    0 if the result is correct, ERROR_ALLOC for error from malloc/calloc functions
    pk a MQ system with m equations in GF(2)[x1,...,x_(n+v)]
    sk the concatenation of a HFE polynom (in GF(2^n)[X]) with the S^(-1) matrix (n+v)*(n+v), T^(-1) matrix n*n 
    (each with coefficients in GF(2))
*/
/* Format of public key:
    pk is stored like the concatenation of a constant and a triangular upper quadratic matrix,
    both in GF(2^m): the m-th bit of a coefficient is the bit of m-th equation.
    The quadratic matrix is the concatenation of the rows, 
    without take the 0 coefficient of the lower part.

    Example with n=4, v=0 and m=2:
    constant: 1+a
    quadratic matrix (n*n) in GF(2^m):
    1   a   1+a a
    0   a   0   1
    0   0   1+a a
    0   0   0   1
    (the term i,j is the monom xi*xj, and xi^2=xi)

    pk[0]=0x3 (constant term)
    pk[1]=0x1,pk[2]=0x2,pk[3]=0x3,pk[4]=0x2 (first row)
    pk[5]=0x2,pk[6]=0x0,pk[7]=0x1 (second row)
    pk[8]=0x3,pk[9]=0x2 (third row)
    pk[10]=0x1 (fourth row) 

    The two equations are:
    1+x0+x0x2+x1x3+x2+x3 (terms with 1)
    1+x0x1+x0x2+x0x3+x1+x2+x2x3 (terms with a)
*/
/* Format of secret key:
    the HFE polynom in sk is just the concatenation of the useful coefficients:
    the coefficients of X^0,X^1, and X^(2^i + 2^j) is ascending order (j <= i)

    The matrices S and T are the concatenation of the rows, 
    and a row uses several words: for example, a row of 128 elements of GF(2) is just two words.

    Example of T with n=4:
    1 0 0 1
    1 1 1 1
    0 0 1 0
    0 1 0 1

    T[0]=0x9
    T[1]=0xF
    T[2]=0x4
    T[3]=0xA

    In the code, it is the inverse of S and T which is stored in the secret key.
*/

int sign_keypairHFE(UINT* pk,UINT* sk)
{
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

    GF2E tmp_coef;
    GF2EX F_NTL;

    mat_GF2 S,S_inv,T,T_inv;
    mat_GF2E Q,SQ,S_E;
    vec_GF2 F_coef_vec,res_vec;
    UINT *cast_vec, *pk_cp;

    unsigned int i,j;
    #if GEN_INVERTIBLE_MATRIX_LU
        int ret;
    #endif

    /* Generate HFE polynomial F */
    genHFEpolynom(sk);
    /* Cast in NTL */
    #if(HFEv)
        convHFEpolynomVToNTLGF2EX(F_NTL,sk);
    #else
        convHFEpolynomToNTLGF2EX(F_NTL,sk);
    #endif

    /* Generate f a secret MQ System, 
       with the representation phi^-1(f) = cst + xQxt */
    genSecretMQS(Q,F_NTL,sk);
    /* At this step, cst = coeff(F_NTL,0) */

    /* Generate S for linear transformation on variables */
    #if GEN_INVERTIBLE_MATRIX_LU
        ret=genRandomInvertibleMatrixLUnv(S);
        if(ret)
        {
            F_NTL.kill();
            Q.kill();
            S.kill();
            return ret;
        }
        /* Compute S^-(1) */
        inv(S_inv,S);
    #else
        genRandomInvertibleMatrixnv(S_inv,S);
    #endif
    S_inv.SetDims(HFEnv,HFEnv);
    /* The inverse of S is in the secret key because it is useful for decryption/signature.
       After the generation of public key, S becomes useless. */
    convNTLmatS_GF2ToUINT(sk+NB_UINT_HFEPOLY,S_inv);
    S_inv.kill();

    /* Linear transformation on variables: compute Q'=S*Q*transpose(S) */
    S_E.SetDims(HFEnv,HFEnv);
    SQ.SetDims(HFEnv,HFEnv);
    /* Because S*Q*tranpose(S) is impossible because it computes product of a
       matrix of GF(2) by a matrix of GF(2^(n+v)), it casts element of GF(2) in GF(2^(n+v)) */
    conv(S_E,S);
    S.kill();
    /* S*Q*transpose(S) */
    mul(SQ,S_E,Q);
    mul(Q,SQ,transpose(S_E));
    S_E.kill();
    SQ.kill();

    /* Generate T for linear transformation on equations */
    #if GEN_INVERTIBLE_MATRIX_LU
        ret=genRandomInvertibleMatrixLUn(T);
        if(ret)
        {
            F_NTL.kill();
            Q.kill();
            T.kill();
            return ret;
        }
        /* Compute T^-(1) */
        inv(T_inv,T);
    #else
        genRandomInvertibleMatrixn(T_inv,T);
    #endif
    T_inv.SetDims(HFEn,HFEn);
    /* The inverse of T is in the secret key because it is useful for decryption/signature.
       After the generation of public key, T becomes useless. */
    convNTLmatT_GF2ToUINT(sk+NB_UINT_HFEPOLY+MATRIXnv_SIZE,T_inv);
    T_inv.kill();

    /* Linear transformation on equations: */

    /* The output is a system with m equations, so it uses only the m first rows of T. */
    #if (HFEn!=HFEm)
        #if !RIGHT_MULTIPLICATION_BY_T
            /* The output is a system with m equations, so it uses only the m first rows of T. */
            T.SetDims(HFEm,HFEn);
        #endif
    #endif
    F_coef_vec.SetLength(HFEn);


    #if HYBRID_REPRESENTATION_PK
        UINT* pk_tmp=(UINT*)malloc(MQ_GFqm_SIZE*sizeof(UINT));
        if(!pk_tmp)
        {
            F_NTL.kill();
            Q.kill();
            T.kill();
            return ERROR_ALLOC;
        }
        pk_cp=pk_tmp;
    #else
        pk_cp=pk;
    #endif


    /* Constant case: compute cst_pk */
    /* Remember that the constant cst is exactly the constant coefficient of HFE polynomial F. */

    /* Cast an element of GF(2^n) like a vector of n bits */
    conv(F_coef_vec,conv<GF2X>(coeff(F_NTL,0)));
    F_NTL.kill();
    F_coef_vec.SetLength(HFEn);
    #if RIGHT_MULTIPLICATION_BY_T
        /* Compute this vector * T = a vector of m bits */
        mul(res_vec,F_coef_vec,T);
        res_vec.SetLength(HFEm);
    #else
        /* Compute T * this vector = a vector of m bits */
        mul(res_vec,T,F_coef_vec);
    #endif
    /* Store the last result like words */
    cast_vec=(UINT*)(res_vec.rep.elts());
    HFECOPY_M(pk_cp,cast_vec);
    pk_cp+=NB_WORD_GFqm;

    /* General case: compute Q_pk */
    for(i=0;i<HFEnv;++i)
    {
        /* i=j */
        /* Cast an element of GF(2^n) like a vector of n bits */
        conv(F_coef_vec,conv<GF2X>(Q[i][i]));
        F_coef_vec.SetLength(HFEn);
        #if RIGHT_MULTIPLICATION_BY_T
            /* Compute this vector * T = a vector of m bits */
            mul(res_vec,F_coef_vec,T);
            res_vec.SetLength(HFEm);
        #else
            /* Compute T * this vector = a vector of m bits */
            mul(res_vec,T,F_coef_vec);
        #endif
        /* Store the last result like words */
        cast_vec=(UINT*)(res_vec.rep.elts());
        HFECOPY_M(pk_cp,cast_vec);
        pk_cp+=NB_WORD_GFqm;
        for(j=i+1;j<HFEnv;++j)
        {
            /* xi xj = xj xi ; it avoids to compute/store two times for the same monom */
            add(tmp_coef,Q[i][j],Q[j][i]);
            /* Cast an element of GF(2^n) like a vector of n bits */
            conv(F_coef_vec,conv<GF2X>(tmp_coef));
            F_coef_vec.SetLength(HFEn);
            #if RIGHT_MULTIPLICATION_BY_T
                /* Compute this vector * T = a vector of m bits */
                mul(res_vec,F_coef_vec,T);
                res_vec.SetLength(HFEm);
            #else
                /* Compute T * this vector = a vector of m bits */
                mul(res_vec,T,F_coef_vec);
            #endif
            /* Store the last result like words */
            cast_vec=(UINT*)(res_vec.rep.elts());
            HFECOPY_M(pk_cp,cast_vec);
            pk_cp+=NB_WORD_GFqm;
        }
    }

    res_vec.kill();
    F_coef_vec.kill();
    clear(tmp_coef);
    T.kill();
    Q.kill();

    #if HYBRID_REPRESENTATION_PK
        convMQS_one_eq_to_hybrid_representation_gf2(pk,pk_tmp);
        free(pk_tmp);
    #endif

    return 0;
}




