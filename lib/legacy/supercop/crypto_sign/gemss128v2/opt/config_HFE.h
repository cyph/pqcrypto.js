#ifndef _CONFIG_HFE_H
#define _CONFIG_HFE_H


#include "choice_crypto.h"
#include "parameters_HFE.h"
#include "tools_gf2m.h"


/* The public and secret keys are the same for encryption and signature */
#define encrypt_keypairHFE sign_keypairHFE


/***** Public-key *****/


#if (!HFEmr8)
    #define FORMAT_HYBRID_CPK8 0
    #define FORMAT_MONOMIAL_PK8 1
#else
    /* Compressed pk */
    #define FORMAT_HYBRID_CPK8 1
    #define FORMAT_MONOMIAL_PK8 1
#endif

#if FORMAT_HYBRID_CPK8
    #define EVAL_HYBRID_CPK8_UNCOMP 1
#endif

/* The verification does not need constant-time evaluation */
#if FORMAT_HYBRID_CPK8
    #define evalMQS_gf2_HFE evalMQShybrid8_nocst_gf2_m
#else
    #define evalMQS_gf2_HFE evalMQSnocst8_gf2
#endif


/***** Secret-key, seed? *****/


/* Generation of the secret-key with a seed */
#if (defined(QUARTZ)||defined(QUARTZ_V1))
    /* In Quartz, the secret-key is generated from a seed */
    #define ENABLED_SEED_SK 1
#else
    /* This option can be set to 1 or 0 */
    #define ENABLED_SEED_SK 1
#endif


/***** Secret-key (S,s,T,t) *****/


/* Choose the method to generate random invertible matrices S and T */
/* Choose !=0 for LU and 0 for classical */
#if 0
    #define GEN_INV_MATRIX_TRIAL_ERROR 1
    #define GEN_INVERTIBLE_MATRIX_LU 0
#else
    #define GEN_INV_MATRIX_TRIAL_ERROR 0
    #define GEN_INVERTIBLE_MATRIX_LU 1
#endif

#if ENABLED_SEED_SK
    /* Set to 1 is faster for the signing process. */
    #define GEN_INVERSE_IN_FIRST 1
#else
    /* The signing process is not impacted, set to 0 is faster for the keypair
       generation when GEN_INV_MATRIX_TRIAL_ERROR is set to 1. */
    #define GEN_INVERSE_IN_FIRST 0
#endif

/* To use an affine transformation (change of variable): *S+s */
#if (defined(QUARTZ)||defined(QUARTZ_V1))
    /* XXX Not implemented, equivalent to set to 0. XXX */
    #define AFFINE_TRANSFORMATION_BY_s 1
#else
    #define AFFINE_TRANSFORMATION_BY_s 0
#endif

/* To use an affine transformation (mix equations): pk = (MQS)*T +t */
#if (defined(QUARTZ)||defined(QUARTZ_V1))
    #define AFFINE_TRANSFORMATION_BY_t 1
#else
    #define AFFINE_TRANSFORMATION_BY_t 0
#endif


/***** Secret-key F *****/


#if defined(QUARTZ_V1)
    /* To take a root only if it is unique */
    #define UNIQUE_ROOT
#endif


/****************** PARAMETERS FOR THE KEYPAIR GENERATION ******************/

/* To use precomputed canonical basis and these powers.
   Requires the prec_cbasis_gf2n.h file, which depends on HFEn. */
#define PRECOMPUTED_CBASIS 0

/* XXX This part is disabled XXX */

/* To use precomputed products of elements of canonical basis and these powers
   The compilation becomes very slow!
   PRECOMPUTE2 removes the effect of PRECOMPUTED_CBASIS. */
#define PRECOMPUTE2 0




#endif

