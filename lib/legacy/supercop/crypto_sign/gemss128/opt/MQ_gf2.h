#ifndef _MQ_GF2_H
#define _MQ_GF2_H


#include "arch.h"
#include "parameters_HFE.h"


/* Number of monomial */
#if (GFq==2)
    #define NB_MONOM (((HFEn*(HFEn+1))>>1)+1)
    #define NB_MONOM_VINEGAR (((HFEv*(HFEv+1))>>1)+1)
    #define NB_MONOM_PK (((HFEnv*(HFEnv+1))>>1)+1)
#else
    #define NB_MONOM (((HFEn*(HFEn+3))>>1)+1)
    #define NB_MONOM_VINEGAR (((HFEv*(HFEv+3))>>1)+1)
    #define NB_MONOM_PK (((HFEnv*(HFEnv+3))>>1)+1)
#endif


/* NB_MONOM_PK = NB_BITS_UINT*quo + rem */
#define HFENq (NB_MONOM_PK/NB_BITS_UINT)
#define HFENr (NB_MONOM_PK%NB_BITS_UINT)


/** Size for a compressed equation. A term requires Log_2(q) bits. */
#if (GFq==2)
    #if (HFENr)
        #define NB_WORD_EQUATION (HFENq+1)
    #else
        #define NB_WORD_EQUATION HFENq
    #endif
#endif


/* Size for an uncompressed equation */
#define NB_WORD_ONE_EQUATION \
    ((((HFEnvq*(HFEnvq+1))>>1)*NB_BITS_UINT)+(HFEnvq+1)*HFEnvr)



#endif
