#ifndef _TOOLS_GF2V_H
#define _TOOLS_GF2V_H

#include "arch.h"
#include "parameters_HFE.h"
#include "bit.h"



/* v = NB_BITS_UINT*quo + rem */
#define HFEvq (HFEv/NB_BITS_UINT)
#define HFEvr (HFEv%NB_BITS_UINT)
/* The number of word that an element of GF(2^v) needs */
#if (HFEvr)
    #define NB_WORD_GFqv_TMP (HFEvq+1)
#else
    #define NB_WORD_GFqv_TMP HFEvq
#endif

#if (NB_WORD_GFqv_TMP == 1)
    #define NB_WORD_GFqv 1
#elif (NB_WORD_GFqv_TMP == 2)
    #define NB_WORD_GFqv 2
#else
    #define NB_WORD_GFqv NB_WORD_GFqv_TMP
#endif


/* Mask for arithmetic in GF(2^v) */
#define HFE_MASKv mask64(HFEvr)

#define HFEvr8 (HFEv&7)
/* Number of bytes that an element of GF(2^(n+v)) needs */
#define NB_BYTES_GFqv ((HFEv>>3)+((HFEvr8)?1:0))




#endif
