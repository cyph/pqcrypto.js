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
    /* Size of the salt in byte */
    #define SIZE_SALT 16
    #define SIZE_SALT_WORD 2
#else
    #define SIZE_SALT_BITS 0
    /* Size of the salt in byte */
    #define SIZE_SALT 0
    #define SIZE_SALT_WORD 0
#endif


/****************** THEORETICAL SIZES ******************/

/* Sizes in bits */
#define SIZE_PK_THEORETICAL_HFE (HFEm*(((HFEnv*(HFEnv+1))>>1)+1))
#define SIZE_SK_THEORETICAL_HFE ((HFEnv*HFEnv)+(HFEn*HFEn)+NB_COEFS_HFEVPOLY*HFEn)
#define SIZE_SIGN_THEORETICAL_HFE (HFEnv+(NB_ITE-1)*(HFEnv-HFEm)+SIZE_SALT_BITS)


/****************** PRATICAL SIZES ******************/

/* Public key (words) */
#if HYBRID_REPRESENTATION_PK
    #define SIZE_PK_HFE_WORD NB_WORD_HYBRID_EQUATIONS
#else
    #define SIZE_PK_HFE_WORD MQ_GFqm_SIZE
#endif
/* Public key (bytes) */
#define SIZE_PK_HFE (SIZE_PK_HFE_WORD<<3)

/* Secret key: HFE polynomial + matrix S_inv + matrix T_inv (+ vector -t) */

#define ACCESS_MATRIX_S NB_UINT_HFEVPOLY
#define ACCESS_MATRIX_T (ACCESS_MATRIX_S+MATRIXnv_SIZE)
#define ACCESS_VECTOR_t (ACCESS_MATRIX_T+MATRIXn_SIZE)

/* Secret key (words) */
#if AFFINE_TRANSFORMATION_BY_t
    #define SIZE_SK_HFE_WORD (ACCESS_VECTOR_t+NB_WORD_GF2m)
#else
    #define SIZE_SK_HFE_WORD ACCESS_VECTOR_t
#endif
/* Secret key (bytes) */
#define SIZE_SK_HFE (SIZE_SK_HFE_WORD<<3)


/* Signature: NB_WORD_GF2nv for signature NB_ITE, NB_WORD_GF2nvm for others */

/* Size of signature (words): */
#define SIZE_SIGN_UNCOMPRESSED (NB_WORD_GF2nv+(NB_ITE-1)*NB_WORD_GF2nvm+SIZE_SALT_WORD)
/* Size of signature (bytes): */
/* (x+7)/8 = Ceiling(x/8) */ 
#define SIZE_SIGN_HFE ((SIZE_SIGN_THEORETICAL_HFE+7)>>3)




#endif
