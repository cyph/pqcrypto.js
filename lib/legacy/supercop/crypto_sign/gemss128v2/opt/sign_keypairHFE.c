#include "sign_keypairHFE.h"

#include "arch.h"
#include "config_HFE.h"
#include "sizes_HFE.h"
#include <stdlib.h>
#include "rand_gf2nx.h"
#if PRECOMPUTE2
    #include "genSecretMQSprec.h"
#else
    #include "genSecretMQS_gf2.h"
#endif
#include "invMatrixn_gf2.h"
#include "invMatrixnv_gf2.h"
#include "changeVariablesMQS_gf2.h"
#include "mixEquationsMQS_gf2.h"

#if AFFINE_TRANSFORMATION_BY_t
    #include "randombytes.h"
#endif

#if FORMAT_HYBRID_CPK8
    #include "convMQS_gf2.h"
#endif
#include "randMatrix_gf2.h"

#if ENABLED_SEED_SK
    /* For the RBG */
    #include "randombytes.h"
    /* For the expandSeed */
    #include "hash.h"

    #if GEN_INV_MATRIX_TRIAL_ERROR
        #include "determinantn_gf2.h"
        #include "determinantnv_gf2.h"
    #endif
#endif


/* Format of the public-key:
    pk is stored as the concatenation of a constant and a triangular upper 
    quadratic matrix, both in GF(2^m): the m-th bit of a coefficient is the bit
    of m-th equation.
    The quadratic matrix is the concatenation of the rows, 
    without to take the 0 coefficient of the lower part.

    Example with n=4, v=0 and m=2:
    constant: 1+a
    quadratic matrix (n,n) in GF(2^m):
    1   a   1+a a
    0   a   0   1
    0   0   1+a a
    0   0   0   1
    (the term i,j is the monomial xi*xj, and xi^2=xi)

    pk[0]=0x3 (constant term)
    pk[1]=0x1,pk[2]=0x2,pk[3]=0x3,pk[4]=0x2 (first row)
    pk[5]=0x2,pk[6]=0x0,pk[7]=0x1 (second row)
    pk[8]=0x3,pk[9]=0x2 (third row)
    pk[10]=0x1 (fourth row) 

    The two equations are:
    1+x0+x0x2+x1x3+x2+x3 (terms with 1)
    1+x0x1+x0x2+x0x3+x1+x2+x2x3 (terms with a)
*/
/* Format of the secret-key:
    sk  is the concatenation of the HFEv polynomial F + the matrix S^(-1) + the 
    matrix T^(-1).
    - the HFEv polynomial in sk is just the concatenation of the useful
    coefficients:
    the coefficients of X^0,X^1, and X^(2^i + 2^j) is ascending order (j <= i)

    - The matrices S and T are the concatenation of the rows, and a row uses 
    several words: for example, a row of 128 elements of GF(2) is just two
    64-bit words.

    Example of T with n=4:
    1 0 0 1
    1 1 1 1
    0 0 1 0
    0 1 0 1

    T[0]=0x9
    T[1]=0xF
    T[2]=0x4
    T[3]=0xA

    In the code, it is the inverse of S and T which are stored in the
    secret-key.
*/


/**
 * @brief   Generate the secret-key then the public-key of the (HFEv-)-based
 * schemes.
 * @details When ENABLED_SEED_SK is set to 1, sk is a seed.
 * @param[out]  pk  The public-key, a MQ system with m equations in
 * GF(2)[x1,...,x_(n+v)].
 * @param[out]  sk  The secret-key, which is the concatenation of a HFEv
 * polynomial in GF(2^n)[X], the S^(-1) matrix (n+v,n+v) in GL_{n+v}(GF(2))
 * and the T^(-1) matrix (n,n) in GLn(GF(2)).
 * @return   0 if the result is correct, ERROR_ALLOC for error from
 * malloc/calloc functions.
 * @remark  Requires to allocate SIZE_PK_HFE_WORD words for the public-key.
 * @remark  The theoretical minimum size of pk is SIZE_PK_THEORETICAL_HFE bits.
 * @remark  Requires to allocate SIZE_SK_HFE_WORD words for the secret-key.
 * @remark  The theoretical minimum size of sk is SIZE_SK_THEORETICAL_HFE bits.
 * @remark  Constant-time implementation.
 */
int PREFIX_NAME(sign_keypairHFE)(uint8_t* pk, UINT* sk)
{
    #if (ENABLED_SEED_SK&&GEN_INV_MATRIX_TRIAL_ERROR)
        expandSeedCxtDeclaration;
    #endif
    mqsnv_gf2n Q;
    sparse_monic_gf2nx F;
    GLnv_gf2 S;
    GLn_gf2 T;
    #if AFFINE_TRANSFORMATION_BY_t
        vecm_gf2 t;
    #endif
    #if GEN_INVERTIBLE_MATRIX_LU
        Tnv_gf2 L, U;
    #elif GEN_INV_MATRIX_TRIAL_ERROR
        Mnv_gf2 S_buf;
    #endif
    #if ENABLED_SEED_SK
        UINT *sk_uncomp;
    #endif
    #if (GEN_INV_MATRIX_TRIAL_ERROR&&(!ENABLED_SEED_SK)&&GEN_INVERSE_IN_FIRST)
        unsigned int i;
    #endif
    int ret;


    #if ENABLED_SEED_SK
        randombytes((uint8_t*)sk,SIZE_SEED_SK);
    #endif


    #if ENABLED_SEED_SK
        #if GEN_INVERTIBLE_MATRIX_LU
            /* The seed generates L,U and L',U' such as S=LU and T=L'U' */
            sk_uncomp=(UINT*)malloc((NB_UINT_HFEVPOLY+(LTRIANGULAR_NV_SIZE<<1)
                                     +(LTRIANGULAR_N_SIZE<<1)+SIZE_VECTOR_t)
                                    *sizeof(UINT));
            VERIFY_ALLOC_RET(sk_uncomp);
            expandSeed((uint8_t*)sk_uncomp,(NB_UINT_HFEVPOLY
                                     +(LTRIANGULAR_NV_SIZE<<1)
                                     +(LTRIANGULAR_N_SIZE<<1)+SIZE_VECTOR_t)<<3,
                 (uint8_t*)sk,SIZE_SEED_SK);
        #elif GEN_INV_MATRIX_TRIAL_ERROR
            /* The seed generates S and T */
            sk_uncomp=(UINT*)malloc(SIZE_SK_HFE_UNCOMPRESSED_WORD*sizeof(UINT));
            VERIFY_ALLOC_RET(sk_uncomp);
            expandSeedIUF(&hashInstance,(uint8_t*)sk,SIZE_SEED_SK<<3);
            expandSeedSqueeze(&hashInstance,(uint8_t*)sk_uncomp,
                               SIZE_SK_HFE_UNCOMPRESSED_WORD<<6);

            S=sk_uncomp+ACCESS_MATRIX_S;
            T=S+MATRIXnv_SIZE;
        #endif

        /* zero padding for the HFEv polynomial F */
        F=sk_uncomp;
        cleanMonicHFEv_gf2nx(F);
    #else
        /* Generate the HFEv polynomial F */
        F=sk;
        randMonicHFEv_gf2nx(F);
    #endif

    /* Generate f a secret MQ system, 
       with the representation phi^-1(f) = cst + xQxt */

    /* Here, the first element (of GF(2^n)) of Q is reserved to store cst.
       The matrix Q is stored as upper triangular matrix. */

    Q=(UINT*)malloc(MQnv_GFqn_SIZE*sizeof(UINT));
    if(!Q)
    {
        #if ENABLED_SEED_SK
            free(sk_uncomp);
        #endif
        return ERROR_ALLOC;
    }
    #if PRECOMPUTE2
        genSecretMQSprec(Q,F);
    #else
        ret=genSecretMQS_gf2(Q,F);
        if(ret)
        {
            #if ENABLED_SEED_SK
                free(sk_uncomp);
            #endif
            free(Q);
            return ret;
        }
    #endif

    /* Generate S for the linear transformation on variables */
    #if ((!ENABLED_SEED_SK)||GEN_INVERTIBLE_MATRIX_LU)
        S=MALLOC_MATRIXnv;
        if(!S)
        {
            #if ENABLED_SEED_SK
                free(sk_uncomp);
            #endif
            free(Q);
            return ERROR_ALLOC;
        }
        /* The allocated memory for S will be use for T */
        T=S;
    #endif


    #if GEN_INVERTIBLE_MATRIX_LU
        #if ENABLED_SEED_SK
            /* The random bytes are already generated from a seed */
            L=sk_uncomp+NB_UINT_HFEVPOLY;
        #else
            /* Allocation of L and U */
            L=(UINT*)malloc((LTRIANGULAR_NV_SIZE<<1)*sizeof(UINT));
            if(!L)
            {
                free(Q);
                free(S);
                return ERROR_ALLOC;
            }
        #endif
        U=L+LTRIANGULAR_NV_SIZE;

        #if ENABLED_SEED_SK
            cleanLowerMatrixnv(L);
            cleanLowerMatrixnv(U);
        #elif GEN_INVERSE_IN_FIRST
            /* S^(-1) = L*U is stored in the secret-key */
            randInvMatrixLU_gf2_nv(sk+ACCESS_MATRIX_S,L,U);
        #else
            /* S = L*U */
            randInvMatrixLU_gf2_nv(S,L,U);
        #endif

        #if GEN_INVERSE_IN_FIRST
            /* Generate S the inverse of S^(-1) */
            invMatrixLUnv_gf2(S,L,U);
        #elif ENABLED_SEED_SK
            /* Generate S^(-1) = L*U */
            mulMatricesLU_gf2_nv(S,L,U);
        #else
            /* Compute S^(-1), stored in the secret-key */
            invMatrixLUnv_gf2(sk+ACCESS_MATRIX_S,L,U);
        #endif
    #elif GEN_INV_MATRIX_TRIAL_ERROR
        S_buf=MALLOC_MATRIXnv;

        if(!S_buf)
        {
            #if ENABLED_SEED_SK
                free(sk_uncomp);
            #endif
            free(Q);
            #if (!ENABLED_SEED_SK)
                free(S);
            #endif
            return ERROR_ALLOC;
        }

        #if ENABLED_SEED_SK
            /* The random bytes are already generated from a seed */
            cleanMatrix_gf2_nv(S);
            while(!determinantnv_gf2(S,S_buf))
            {
                expandSeedSqueeze(&hashInstance,(uint8_t*)S,MATRIXnv_SIZE<<6);
                cleanMatrix_gf2_nv(S);
            }

            #if GEN_INVERSE_IN_FIRST
                invMatrixnv_gf2(S_buf,S);
                S=S_buf;
/*          #else
                   Useless to generate S^(-1) here */
            #endif
        #elif GEN_INVERSE_IN_FIRST
            randInvMatrix_gf2_nv(sk+ACCESS_MATRIX_S,S_buf);

            /* Copy of S^(-1) in S_buf */
            for(i=0;i<MATRIXnv_SIZE;++i)
            {
                S_buf[i]=sk[ACCESS_MATRIX_S+i];
            }

            /* Computation of S (S_buf is modified) */
            invMatrixnv_gf2(S,S_buf);
        #else
            randInvMatrix_gf2_nv(S,S_buf);
            /* S^(-1) will be generated later in sk */
        #endif
    #endif


    /* Compute Q'=S*Q*St (with Q an upper triangular matrix) */
    changeVariablesMQS_gf2(Q,S);


    #if (GEN_INV_MATRIX_TRIAL_ERROR&&(!ENABLED_SEED_SK)\
                                   &&(!GEN_INVERSE_IN_FIRST))
        /* Compute S^(-1), stored in the secret-key */
        invMatrixnv_gf2(sk+ACCESS_MATRIX_S,S);
    #endif


    #if GEN_INVERTIBLE_MATRIX_LU
        #if ENABLED_SEED_SK
            /* The random bytes are already generated from a seed */
            L+=(LTRIANGULAR_NV_SIZE<<1);
            U=L+LTRIANGULAR_N_SIZE;
        #elif HFEv
            U=L+LTRIANGULAR_N_SIZE;
        #endif

        #if ENABLED_SEED_SK
            cleanLowerMatrixn(L);
            cleanLowerMatrixn(U);
        #elif GEN_INVERSE_IN_FIRST
            /* T^(-1) = L*U is stored in the secret-key */
            randInvMatrixLU_gf2_n(sk+ACCESS_MATRIX_T,L,U);
        #else
            /* T = L*U */
            randInvMatrixLU_gf2_n(T,L,U);
        #endif

        #if GEN_INVERSE_IN_FIRST
            /* Generate T the inverse of T^(-1) */
            invMatrixLUn_gf2(T,L,U);
        #elif ENABLED_SEED_SK
            /* Generate T^(-1) = L*U */
            mulMatricesLU_gf2_n(T,L,U);
        #else
            /* Compute T^(-1), stored in the secret-key */
            invMatrixLUn_gf2(sk+ACCESS_MATRIX_T,L,U);
        #endif

        #if (!ENABLED_SEED_SK)
            free(L);
        #endif
    #elif GEN_INV_MATRIX_TRIAL_ERROR
        #if ENABLED_SEED_SK
            /* The random bytes are already generated from a seed */
            cleanMatrix_gf2_n(T);
            while(!determinantn_gf2(T,S_buf))
            {
                expandSeedSqueeze(&hashInstance,(uint8_t*)T,MATRIXn_SIZE<<6);
                cleanMatrix_gf2_n(T);
            }

            #if GEN_INVERSE_IN_FIRST
                invMatrixn_gf2(S_buf,T);
                T=S_buf;
/*          #else
                   Useless to generate T^(-1) here */
            #endif
        #elif GEN_INVERSE_IN_FIRST
            randInvMatrix_gf2_n(sk+ACCESS_MATRIX_T,S_buf);

            /* Copy of T^(-1) in S_buf */
            for(i=0;i<MATRIXn_SIZE;++i)
            {
                S_buf[i]=sk[ACCESS_MATRIX_T+i];
            }

            /* Computation of T (S_buf is modified) */
            invMatrixn_gf2(T,S_buf);
        #else
            /* Generate T for linear transformation on equations */
            randInvMatrix_gf2_n(T,S_buf);
            /* T^(-1) will be generated later in sk */
        #endif

        #if !(ENABLED_SEED_SK&&GEN_INVERSE_IN_FIRST)
            free(S_buf);
        #endif
    #endif


    #if (ENABLED_SEED_SK&&GEN_INVERTIBLE_MATRIX_LU\
                        &&(!AFFINE_TRANSFORMATION_BY_t))
        free(sk_uncomp);
    #endif


    #if (FORMAT_HYBRID_CPK8)
        uint8_t* pk_tmp=(uint8_t*)malloc(MQ_GFqm8_SIZE*sizeof(uint8_t));
    #endif

    #if (FORMAT_HYBRID_CPK8)
        if(!pk_tmp)
        {
            #if (ENABLED_SEED_SK&&\
                 (GEN_INV_MATRIX_TRIAL_ERROR||AFFINE_TRANSFORMATION_BY_t))
                free(sk_uncomp);
            #endif
            free(Q);
            #if ((!ENABLED_SEED_SK)||GEN_INVERTIBLE_MATRIX_LU)
                /* T is stored in S. free(S) would have the same effect. */
                free(T);
            #endif
            #if (GEN_INV_MATRIX_TRIAL_ERROR&&ENABLED_SEED_SK\
                                           &&GEN_INVERSE_IN_FIRST)
                /* T is stored in S_buf */
                free(S_buf);
            #endif
            return ERROR_ALLOC;
        }
    #endif


    /* Use T (variable S) to compute cst_pk and Q_pk */
    #if (FORMAT_HYBRID_CPK8)
        mixEquationsMQS8_gf2(pk_tmp,Q,T);
    #elif FORMAT_MONOMIAL_PK8
        mixEquationsMQS8_gf2(pk,Q,T);
    #endif
    free(Q);


    #if (ENABLED_SEED_SK&&GEN_INV_MATRIX_TRIAL_ERROR\
                        &&(!AFFINE_TRANSFORMATION_BY_t))
        free(sk_uncomp);
    #endif
    #if (GEN_INV_MATRIX_TRIAL_ERROR&&ENABLED_SEED_SK&&GEN_INVERSE_IN_FIRST)
        /* T is stored in S_buf */
        free(S_buf);
    #endif


    /* Generate the inverse of T */
    #if (GEN_INV_MATRIX_TRIAL_ERROR&&(!ENABLED_SEED_SK)\
                                   &&(!GEN_INVERSE_IN_FIRST))
        invMatrixn_gf2(sk+ACCESS_MATRIX_T,T);
    #endif


    #if ((!ENABLED_SEED_SK)||GEN_INVERTIBLE_MATRIX_LU)
        /* T is stored in S */
        free(S);
    #endif


    #if AFFINE_TRANSFORMATION_BY_t
        #if ENABLED_SEED_SK
            #if GEN_INVERTIBLE_MATRIX_LU
                t=L+(LTRIANGULAR_N_SIZE<<1);
            #elif GEN_INV_MATRIX_TRIAL_ERROR
                t=sk_uncomp+ACCESS_VECTOR_t;
            #endif
        #else
            t=sk+ACCESS_VECTOR_t;
            randombytes((unsigned char*)t,NB_WORD_GF2m<<3);
        #endif
        #if HFEmr
            t[NB_WORD_GF2m-1]&=MASK_GF2m;
        #endif
        #if (FORMAT_HYBRID_CPK8)
            add2_gf2m((UINT*)pk_tmp,t);
        #else
            add2_gf2m((UINT*)pk,t);
        #endif

        #if ENABLED_SEED_SK
            free(sk_uncomp);
        #endif
    #endif


    #if FORMAT_HYBRID_CPK8
        convMQS_one_eq_to_hybrid_rep8_gf2(pk,pk_tmp);
    #endif

    #if (FORMAT_HYBRID_CPK8)
        free(pk_tmp);
    #endif

    return 0;
}


