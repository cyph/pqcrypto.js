#ifndef _MQ_GF2_H
#define _MQ_GF2_H


#include "arch.h"
#include "parameters_HFE.h"


/* This type stores a quadratic equation in GF(2)[x1,...,x_(n+v)]. */
typedef UINT* mqnv_gf2;
typedef const UINT* cst_mqnv_gf2;


/* Number of monomial */
#if (GFq==2)
    #define NB_MONOMIAL (((HFEn*(HFEn+1))>>1)+1)
    #define NB_MONOMIAL_VINEGAR (((HFEv*(HFEv+1))>>1)+1)
    #define NB_MONOMIAL_PK (((HFEnv*(HFEnv+1))>>1)+1)
#else
    #define NB_MONOMIAL (((HFEn*(HFEn+3))>>1)+1)
    #define NB_MONOMIAL_VINEGAR (((HFEv*(HFEv+3))>>1)+1)
    #define NB_MONOMIAL_PK (((HFEnv*(HFEnv+3))>>1)+1)
#endif


/* NB_MONOMIAL_PK = NB_BITS_UINT*quo + rem */
#define HFENq (NB_MONOMIAL_PK/NB_BITS_UINT)
#define HFENr (NB_MONOMIAL_PK%NB_BITS_UINT)

#define HFENq8 (NB_MONOMIAL_PK>>3)
#define HFENr8 (NB_MONOMIAL_PK&7)
#define NB_BYTES_EQUATION ((NB_MONOMIAL_PK+7)>>3)
#define HFENr8c ((8-HFENr8)&7)


/** Size for a compressed equation. A term requires Log_2(q) bits. */
#if (GFq==2)
    #if (HFENr)
        #define NB_WORD_EQUATION (HFENq+1)
    #else
        #define NB_WORD_EQUATION HFENq
    #endif
#endif


/* Size for an uncompressed equation */
#define NB_WORD_UNCOMP_EQ \
    ((((HFEnvq*(HFEnvq+1))>>1)*NB_BITS_UINT)+(HFEnvq+1)*HFEnvr)



#endif

