#include "sign_keypairHFE.h"

#include "config_HFE.h"
#include "sizes_HFE.h"
#include <stdlib.h>
#include "init.h"
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

#if HYBRID_REPRESENTATION_PK
    #include "convMQS_gf2.h"
#endif


/* Choose the method to generate random invertible matrix */
/* Choose !=0 for LU and 0 for classical */
#include "randMatrix_gf2.h"
#ifdef GUI
    #define GEN_INVERTIBLE_MATRIX_LU 0
#else
    #define GEN_INVERTIBLE_MATRIX_LU 1
#endif


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
/*    Requirement:
        for the vectorial version, sk must be aligned
*/

int PREFIX_NAME(sign_keypairHFE)(UINT* pk,UINT* sk)
{
    UINT *Q, *S;
    #if GEN_INVERTIBLE_MATRIX_LU
        Tnv_gf2 L, U;
    #endif
    int ret;

    /* Generate HFEv polynomial F */
    randMonicHFEv_gf2nx(sk);

    /* Generate f a secret MQ System, 
       with the representation phi^-1(f) = cst + xQxt */

    /* Here the first element (of GF(2^n)) of Q is reserved for store cst.
       The Q is stored like upper triangular matrix */

    #if(HFEDeg<3)
        /* there is not quadratic terms X^(2^i + 2^j) */
        ALIGNED_CALLOC(Q,UINT*,MQnv_GFqn_SIZE,sizeof(UINT));
    #else
        ALIGNED_MALLOC(Q,UINT*,MQnv_GFqn_SIZE,sizeof(UINT));
    #endif
    VERIFY_ALLOC_RET(Q);
    #if PRECOMPUTE2
        genSecretMQSprec(Q,sk);
    #else
        ret=genSecretMQS_gf2(Q,sk);
        if(ret)
        {
            free(Q);
            return ret;
        }
    #endif

    /* Generate S for linear transformation on variables */
    S=MALLOC_MATRIXnv;
    if(!S)
    {
        free(Q);
        return ERROR_ALLOC;
    }

    #if GEN_INVERTIBLE_MATRIX_LU
        /* Generation of L and U */
        L=(UINT*)malloc(LTRIANGULAR_NV_SIZE*sizeof(UINT));
        if(!L)
        {
            free(Q);
            free(S);
            return ERROR_ALLOC;
        }
        U=(UINT*)malloc(LTRIANGULAR_NV_SIZE*sizeof(UINT));
        if(!U)
        {
            free(Q);
            free(S);
            free(L);
            return ERROR_ALLOC;
        }

        GEN_LU_Tnv(L,U);
        randInvMatrixLU_gf2_nv(S,L,U);
    #else
        randInvMatrix_gf2_nv(S);
        if(ret<0)
        {
            free(Q);
            free(S);
            return ret;
        }
    #endif

    /* Compute Q'=S*Q*St (with Q upper triangular matrix) */
    changeVariablesMQS_gf2(Q,S);

    #if GEN_INVERTIBLE_MATRIX_LU
        /* Generate inverse of S */
        invMatrixLUnv_gf2(sk+ACCESS_MATRIX_S,L,U);

        /* Generate T for linear transformation on equations */
        GEN_LU_Tn(L,U);
        randInvMatrixLU_gf2_n(S,L,U);
    #else
        /* Generate inverse of S */
        invMatrixnv_gf2(sk+ACCESS_MATRIX_S,S);

        /* Generate T for linear transformation on equations */
        randInvMatrix_gf2_n(S);
        if(ret<0)
        {
            free(Q);
            free(S);
            return ret;
        }
    #endif

    /* Use T (variable S) to compute cst_pk and Q_pk */
    #if HYBRID_REPRESENTATION_PK
        UINT* pk_tmp=(UINT*)malloc(MQ_GFqm_SIZE*sizeof(UINT));
        if(!pk_tmp)
        {
            free(Q);
            free(S);
            #if GEN_INVERTIBLE_MATRIX_LU
                free(L);
                free(U);
            #endif
            return ERROR_ALLOC;
        }

        mixEquationsMQS_gf2(pk_tmp,Q,S);
    #else
        mixEquationsMQS_gf2(pk,Q,S);
    #endif
    free(Q);

    /* Generate inverse of T */
    #if GEN_INVERTIBLE_MATRIX_LU
        invMatrixLUn_gf2(sk+ACCESS_MATRIX_T,L,U);

        free(L);
        free(U);
    #else
        invMatrixn_gf2(sk+ACCESS_MATRIX_T,S);
    #endif
    free(S);

    #if AFFINE_TRANSFORMATION_BY_t
        randombytes((unsigned char*)(sk+ACCESS_VECTOR_t),NB_WORD_GF2m<<3);
        #if HFEmr
            sk[ACCESS_VECTOR_t+NB_WORD_GF2m-1]&=HFE_MASKm;
        #endif
        #if HYBRID_REPRESENTATION_PK
            add2_gf2m(pk_tmp,sk+ACCESS_VECTOR_t);
        #else
            add2_gf2m(pk,sk+ACCESS_VECTOR_t);
        #endif
    #endif

    #if HYBRID_REPRESENTATION_PK
        convMQS_one_eq_to_hybrid_representation_gf2(pk,pk_tmp);
        free(pk_tmp);
    #endif

    return 0;
}

