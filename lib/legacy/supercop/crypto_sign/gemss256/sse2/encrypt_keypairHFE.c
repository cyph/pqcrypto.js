#include "encrypt_keypairHFE.h"

#include <stdlib.h>
#include "init.h"
#include "genHFEtab.h"
#if PRECOMPUTE2
    #include "genSecretMQSprec.h"
#else
    #include "genSecretMQS.h"
#endif
#include "invMatrix.h"
#include "invMatrixnv.h"
#include "changeVariableMQS.h"
#include "changeEquationMQS.h"

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

int encrypt_keypairHFE(UINT* pk,UINT* sk)
{
    UINT *Q, *S;
    int ret;

    /* Generate HFE polynomial F */
    genHFEpolynom(sk);

    /* Generate f a secret MQ System, 
       with the representation phi^-1(f) = cst + xQxt */

    /* Here the first element (of GF(2^n)) of Q is reserved for store cst.
       The Q is stored like upper triangular matrix */

    #if(HFEDeg<3)
        /* there is not quadratic terms X^(2^i + 2^j) */
        Q=CALLOC_MQSn;
    #else
        Q=MALLOC_MQSn;
    #endif
    VERIFY_ALLOC_RET(Q);
    #if PRECOMPUTE2
        genSecretMQSprec(Q,sk);
    #else
        ret=genSecretMQS(Q,sk);
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
        ret=genRandomInvertibleMatrixnvLU(S);
        if(ret)
    #else
        genRandomInvertibleMatrixnv(S);
        if(ret<0)
    #endif
        {
            free(Q);
            return ret;
        }

    /* Compute Q'=S*Q*St (with Q upper triangular matrix) */
    changeVariableMQS(Q,S);

    /* Generate inverse of S */
    invMatrixnv(sk+NB_UINT_HFEPOLY,S);

    /* Generate T for linear transformation on equations */
    #if GEN_INVERTIBLE_MATRIX_LU
        ret=genRandomInvertibleMatrixnLU(S);
        if(ret)
        {
            free(Q);
            return ret;
        }
    #else
        genRandomInvertibleMatrixn(S);
    #endif

    /* Use T (variable S) to compute cst_pk and Q_pk */
    changeEquationMQS(pk,Q,S);
    free(Q);

    /* Generate inverse of T */
    invMatrix(sk+NB_UINT_HFEPOLY+MATRIXnv_SIZE,S);

    free(S);

    return 0;
}

