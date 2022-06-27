#ifndef _MQS_GF2_H
#define _MQS_GF2_H


#include "arch.h"
#include "parameters_HFE.h"
#include "MQ_gf2.h"


/* This type stores a multivariate quadratic system with n equations in 
GF(2)[x1,...,xn]. It is stored like one equation in GF(2^n)[x1,...,xn]. */
typedef UINT* mqsn_gf2n;
typedef const UINT* cst_mqsn_gf2n;

/* This type stores a multivariate quadratic system with n equations in 
GF(2)[x1,...,x_(n+v)]. It is stored like one equation in 
GF(2^n)[x1,...,x_(n+v)]. */
typedef UINT* mqsnv_gf2n;
typedef const UINT* cst_mqsnv_gf2n;

/* This type stores a multivariate quadratic system with n equations in 
GF(2)[x1,...,xv]. It is stored like one equation in GF(2^n)[x1,...,xv]. */
typedef UINT* mqsv_gf2n;
typedef const UINT* cst_mqsv_gf2n;

/* This type stores a multivariate quadratic system with m equations in 
GF(2)[x1,...,xn]. It is stored like one equation in GF(2^m)[x1,...,xn]. */
typedef UINT* mqsn_gf2m;
typedef const UINT* cst_mqsn_gf2m;

/* This type stores a multivariate quadratic system with m equations in 
 * GF(2)[x1,...,x_(n+v)]. It is stored like one equation in 
 * GF(2^n)[x1,...,x_(n+v)]. */
typedef UINT* mqsnv_gf2m;
typedef const UINT* cst_mqsnv_gf2m;

/* This type stores a multivariate quadratic system with m equations in 
GF(2)[x1,...,x_(n+v)]. It is stored like m equations in GF(2)[x1,...,x_(n+v)].*/
typedef UINT* m_mqnv_gf2;
typedef const UINT* cst_m_mqnv_gf2;




/* Size for m compressed equation */
#define NB_WORD_mEQUATIONS (NB_WORD_EQUATION*HFEm)
/* Size for m uncompressed equation */
#define NB_WORD_m_EQUATIONS (NB_WORD_ONE_EQUATION*HFEm+NB_WORD_GF2m)


/* Remove the last word of each coefficient in GF(2^m), \
   excepted for the constant */
#define ACCESS_last_equations ((NB_MONOM_PK-1)*HFEmq+NB_WORD_GF2m)
/* It is padded to avoid to load data outside of memory 
   during the public key evaluation */
/* XXX We remove the padding here XXX */
#define NB_WORD_HYBRID_EQUATIONS (ACCESS_last_equations+\
            NB_WORD_ONE_EQUATION*HFEmr/*+((4-(NB_WORD_GF2nv&3))&3)*/)


/* Size of a MQ polynom with n+v variables and with coefficients in GF(2^n) */
#define MQnv_GFqn_SIZE (NB_MONOM_PK*NB_WORD_GFqn)
/* Size of a MQ polynom with v variables and with coefficients in GF(2^n) */
#define MQv_GFqn_SIZE (NB_MONOM_VINEGAR*NB_WORD_GFqn)
/* Size of a multivariate linear polynomial with v variables and with 
 * coefficients in GF(2^n) */
#define MLv_GFqn_SIZE ((HFEv+1)*NB_WORD_GFqn)

#define MALLOC_MQSn (UINT*)malloc(MQnv_GFqn_SIZE*sizeof(UINT))
#define CALLOC_MQSn (UINT*)calloc(MQnv_GFqn_SIZE,sizeof(UINT))




/* Size of a MQ polynomial with coefficients in GF(2^m) */
/* It is padded to avoid to load data outside of memory 
   during the public key evaluation */
/* XXX We remove the padding here XXX */
#define MQ_GFqm_SIZE ((NB_MONOM_PK*NB_WORD_GF2m)/*+((4-(NB_WORD_GF2m&3))&3)*/)

#define MALLOC_MQSm (UINT*)malloc(MQ_GFqm_SIZE*sizeof(UINT))
#define CALLOC_MQSm (UINT*)calloc(MQ_GFqm_SIZE,sizeof(UINT))




#endif
