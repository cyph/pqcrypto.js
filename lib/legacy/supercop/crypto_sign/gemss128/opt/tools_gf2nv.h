#ifndef _TOOLS_GF2NV_H
#define _TOOLS_GF2NV_H

#include "arch.h"
#include "parameters_HFE.h"
#include "macro.h"
#include "add_gf2x.h"
#include "init.h"
#include "bit.h"
#include "boolean.h"
#include "tools_gf2m.h"



/* This type stores an element of GF(2^(n+v)). */
typedef UINT* gf2nv;
typedef const UINT* cst_gf2nv;
/* to use data[length] */
typedef UINT static_gf2nv;



/* n+v = NB_BITS_UINT*quo + rem */
#define HFEnvq (HFEnv/NB_BITS_UINT)
#define HFEnvr (HFEnv%NB_BITS_UINT)
/* The number of word that an element of GF(2^(n+v)) needs */
#if (HFEnvr)
    #define NB_WORD_GF2nv_TMP (HFEnvq+1)
#else
    #define NB_WORD_GF2nv_TMP HFEnvq
#endif

/* Mask for arithmetic in GF(2^(n+v)) */
#define HFE_MASKnv mask64(HFEnvr)

#define HFEnvq8 (HFEnv>>3)
#define HFEnvr8 (HFEnv&7)
#define HFE_MASKnv8 ((1U<<HFEnvr8)-1)
/* Number of bytes that an element of GF(2^(n+v)) needs */
#define NB_BYTES_GFqnv (HFEnvq8+((HFEnvr8)?1:0))




/* To choose macro for NB_WORD_GF2nv*64 bits */
#if (NB_WORD_GF2nv_TMP == 1)
    #define NB_BITS_GFqnv_SUP 64
    #define NB_WORD_GF2nv 1
#elif (NB_WORD_GF2nv_TMP == 2)
    #define NB_BITS_GFqnv_SUP 128
    #define NB_WORD_GF2nv 2
#elif (NB_WORD_GF2nv_TMP == 3)
    #define NB_BITS_GFqnv_SUP 192
    #define NB_WORD_GF2nv 3
#elif (NB_WORD_GF2nv_TMP == 4)
    #define NB_BITS_GFqnv_SUP 256
    #define NB_WORD_GF2nv 4
#elif (NB_WORD_GF2nv_TMP == 5)
    #define NB_BITS_GFqnv_SUP 320
    #define NB_WORD_GF2nv 5
#elif (NB_WORD_GF2nv_TMP == 6)
    #define NB_BITS_GFqnv_SUP 384
    #define NB_WORD_GF2nv 6
#elif (NB_WORD_GF2nv_TMP == 7)
    #define NB_BITS_GFqnv_SUP 448
    #define NB_WORD_GF2nv 7
#elif (NB_WORD_GF2nv_TMP == 8)
    #define NB_BITS_GFqnv_SUP 512
    #define NB_WORD_GF2nv 8
#elif (NB_WORD_GF2nv_TMP == 9)
    #define NB_BITS_GFqnv_SUP 576
    #define NB_WORD_GF2nv 9
#elif (NB_WORD_GF2nv_TMP == 10)
    #define NB_BITS_GFqnv_SUP 640
    #define NB_WORD_GF2nv 10
#endif

#define CONCAT_NB_BITS_GFqnv_SUP(name) CONCAT(name,NB_BITS_GFqnv_SUP)
#define CONCAT_NB_WORD_GF2nv_SUP(name) CONCAT(name,NB_WORD_GF2nv)





#if (NB_WORD_GF2nv<7)
    #define set0_gf2nv CONCAT_NB_BITS_GFqnv_SUP(SET0_)
    #define addLoadMask1_gf2nv CONCAT_NB_BITS_GFqnv_SUP(ADDLOADMASK1_)
#else
    #define set0_gf2nv(c) SET0(c,NB_WORD_GF2nv)
    #define addLoadMask1_gf2nv(res,a,b) ADDLOADMASK1(res,a,b,NB_WORD_GF2nv)
#endif


#if (NB_WORD_GF2nv==1)
    #define add_gf2nv ADD64_TAB
    #define add2_gf2nv ADD64_TAB2
    #define swap_gf2nv PERMUTATION64_TAB
#elif (NB_WORD_GF2nv<7)
    #define add_gf2nv CONCAT_NB_BITS_GFqnv_SUP(ADD)
    #define add2_gf2nv CONCAT(CONCAT_NB_BITS_GFqnv_SUP(ADD),_2)
    #define swap_gf2nv CONCAT_NB_BITS_GFqnv_SUP(PERMUTATION)
#else
    #define add_gf2nv(a,b,c) ADD(a,b,c,NB_WORD_GF2nv)
    #define add2_gf2nv(a,b) ADD2(a,b,NB_WORD_GF2nv)
    #define swap_gf2nv(a,b) PERMUTATION(ADD2,a,b,NB_WORD_GF2nv)
#endif






#if ((HFEn==HFEm)&&(!HFEv))
    /* This case is never used */
    #define NB_WORD_GF2nvm 0U
#else
    #define NB_WORD_GF2nvm (NB_WORD_GF2nv-NB_WORD_GF2m+(HFEmr?1:0))
#endif

#define HFE_MASKnvm (~HFE_MASKm)


/* To choose macro for NB_WORD_GF2nvm*64 bits */
#if (NB_WORD_GF2nvm == 1)
    #define copy_gf2nvm COPY64
#elif (NB_WORD_GF2nvm == 2)
    #define copy_gf2nvm COPY128
    #define copy_gf2nvm1 COPY64
#elif (NB_WORD_GF2nvm == 3)
    #define copy_gf2nvm COPY192
    #define copy_gf2nvm1 COPY128
#elif (NB_WORD_GF2nvm == 4)
    #define copy_gf2nvm COPY256
    #define copy_gf2nvm1 COPY192
#elif (NB_WORD_GF2nvm == 5)
    #define copy_gf2nvm COPY320
    #define copy_gf2nvm1 COPY256
#elif (NB_WORD_GF2nvm == 6)
    #define copy_gf2nvm COPY384
    #define copy_gf2nvm1 COPY320
#elif (NB_WORD_GF2nvm == 7)
    #define copy_gf2nvm COPY448
    #define copy_gf2nvm1 COPY384
#elif (NB_WORD_GF2nvm == 8)
    #define copy_gf2nvm COPY512
    #define copy_gf2nvm1 COPY448
#endif





#endif
