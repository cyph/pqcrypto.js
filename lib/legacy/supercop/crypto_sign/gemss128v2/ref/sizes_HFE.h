#ifndef _SIZES_HFE_H
#define _SIZES_HFE_H

#include "choice_crypto.h"
#include "arch.h"
#include "macro.h"
#include "config_HFE.h"
#include "bit.h"
#include "hash.h"
#include "KAT_int.h"
#include "parameters_HFE.h"
#include "tools_gf2n.h"
#include "tools_gf2nv.h"
#include "tools_gf2v.h"
#include "gf2nx.h"
#include "matrix_gf2.h"
#include "MQS_gf2.h"


#if EUF_CMA_PROPERTY
    #define SIZE_SALT_BITS 128
    /* Size of the salt in bytes */
    #define SIZE_SALT 16
    #define SIZE_SALT_WORD 2
#else
    #define SIZE_SALT_BITS 0
    /* Size of the salt in bytes */
    #define SIZE_SALT 0
    #define SIZE_SALT_WORD 0
#endif


#if ENABLED_SEED_SK
    /* Size of the secret seed in bytes */
    #if (defined(QUARTZ)||defined(QUARTZ_V1))
        #define SIZE_SEED_SK 16
    #else
        /* K bits */
        #define SIZE_SEED_SK (K>>3)
    #endif
#endif


/****************** THEORETICAL SIZES ******************/

/* Sizes in bits */
#define SIZE_PK_THEORETICAL_HFE (HFEm*(((HFEnv*(HFEnv+1))>>1)+1))

#if ENABLED_SEED_SK
    #define SIZE_SK_THEORETICAL_HFE (SIZE_SEED_SK<<3)
#else
    #define SIZE_SK_THEORETICAL_HFE \
                ((HFEnv*HFEnv)+(HFEn*HFEn)+NB_COEFS_HFEVPOLY*HFEn)
#endif

#define SIZE_SIGN_THEORETICAL_HFE (HFEnv+(NB_ITE-1)*(HFEnv-HFEm)+SIZE_SALT_BITS)


/****************** PRACTICAL SIZES ******************/

/* Public-key (words) */
#if (FORMAT_HYBRID_CPK8&&EVAL_HYBRID_CPK8_UNCOMP)
    #define SIZE_PK_HFE_WORD ((((NB_MONOMIAL_PK*HFEm+7)>>3)+7)>>3)
#elif FORMAT_MONOMIAL_PK8
    #define SIZE_PK_HFE_WORD ((MQ_GFqm8_SIZE>>3)+((MQ_GFqm8_SIZE&7)?1:0))
#endif

/* Public-key (bytes) */
#if (FORMAT_HYBRID_CPK8&&EVAL_HYBRID_CPK8_UNCOMP)
    /* Minimal size */
    #define SIZE_PK_HFE ((NB_MONOMIAL_PK*HFEm+7)>>3)
#elif FORMAT_MONOMIAL_PK8
    /* Minimal when m mod 8 == 0 */
    #define SIZE_PK_HFE MQ_GFqm8_SIZE
#endif

/* Secret-key: HFE polynomial + matrix S_inv + matrix T_inv (+ vector -t) */

#if AFFINE_TRANSFORMATION_BY_t
    #define SIZE_VECTOR_t NB_WORD_GF2m
#else
    #define SIZE_VECTOR_t 0
#endif

#define ACCESS_MATRIX_S NB_UINT_HFEVPOLY
#define ACCESS_MATRIX_T (ACCESS_MATRIX_S+MATRIXnv_SIZE)
#define ACCESS_VECTOR_t (ACCESS_MATRIX_T+MATRIXn_SIZE)

/* Secret-key (words) */
#define SIZE_SK_HFE_UNCOMPRESSED_WORD (ACCESS_VECTOR_t+SIZE_VECTOR_t)
/* Secret-key (bytes) */
#define SIZE_SK_HFE_UNCOMPRESSED (SIZE_SK_HFE_UNCOMPRESSED_WORD<<3)

#if ENABLED_SEED_SK
    /* Secret-key (bytes) */
    #define SIZE_SK_HFE SIZE_SEED_SK
    /* Secret-key (words) */
    #define SIZE_SK_HFE_WORD ((SIZE_SK_HFE+7)>>3)
#else
    /* Secret-key (words) */
    #define SIZE_SK_HFE_WORD SIZE_SK_HFE_UNCOMPRESSED_WORD
    /* Secret-key (bytes) */
    #define SIZE_SK_HFE SIZE_SK_HFE_UNCOMPRESSED
#endif


/* Signature: NB_WORD_GF2nv for signature NB_ITE, NB_WORD_GF2nvm for others */

/* Size of signature (words): */
#define SIZE_SIGN_UNCOMPRESSED \
            (NB_WORD_GF2nv+(NB_ITE-1)*NB_WORD_GF2nvm+SIZE_SALT_WORD)
/* Size of signature (bytes): */
/* (x+7)/8 = Ceiling(x/8) */ 
#define SIZE_SIGN_HFE ((SIZE_SIGN_THEORETICAL_HFE+7)>>3)


#endif

