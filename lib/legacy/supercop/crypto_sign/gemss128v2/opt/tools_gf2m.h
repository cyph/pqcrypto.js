#ifndef _TOOLS_GF2M_H
#define _TOOLS_GF2M_H

#include "arch.h"
#include "parameters_HFE.h"
#include "macro.h"
#include "add_gf2x.h"
#include "init.h"
#include "bit.h"
#include "predicate.h"
#include "loadMask.h"
#include "dotProduct_gf2.h"


/* This type stores an element of GF(2^m). */
typedef UINT* gf2m;
typedef const UINT* cst_gf2m;
/* to use data[length] */
typedef UINT static_gf2m;


/* m = NB_BITS_UINT*quo + rem */
#define HFEmq (HFEm/NB_BITS_UINT)
#define HFEmr (HFEm%NB_BITS_UINT)
/* The number of word that an element of GF(2^m) needs */
#if (HFEmr)
    #define NB_WORD_GF2m_TMP (HFEmq+1)
#else
    #define NB_WORD_GF2m_TMP HFEmq
#endif

/* Mask to truncate the last word */
#define MASK_GF2m maskUINT(HFEmr)

#define HFEmq8 (HFEm>>3)
#define HFEmr8 (HFEm&7U)
#define HFEmr8c ((8-HFEmr8)&7)

/* Number of bytes that an element of GF(2^m) needs */
#define NB_BYTES_GFqm (HFEmq8+((HFEmr8)?1:0))

/* To choose macro for NB_WORD_GF2m*64 bits */
#if (NB_WORD_GF2m_TMP == 1)
    #define NB_BITS_GFqm_SUP 64
    #define NB_WORD_GF2m 1
#elif (NB_WORD_GF2m_TMP == 2)
    #define NB_BITS_GFqm_SUP 128
    #define NB_WORD_GF2m 2
#elif (NB_WORD_GF2m_TMP == 3)
    #define NB_BITS_GFqm_SUP 192
    #define NB_WORD_GF2m 3
#elif (NB_WORD_GF2m_TMP == 4)
    #define NB_BITS_GFqm_SUP 256
    #define NB_WORD_GF2m 4
#elif (NB_WORD_GF2m_TMP == 5)
    #define NB_BITS_GFqm_SUP 320
    #define NB_WORD_GF2m 5
#elif (NB_WORD_GF2m_TMP == 6)
    #define NB_BITS_GFqm_SUP 384
    #define NB_WORD_GF2m 6
#elif (NB_WORD_GF2m_TMP == 7)
    #define NB_BITS_GFqm_SUP 448
    #define NB_WORD_GF2m 7
#elif (NB_WORD_GF2m_TMP == 8)
    #define NB_BITS_GFqm_SUP 512
    #define NB_WORD_GF2m 8
#endif

#define CONCAT_NB_BITS_GFqm_SUP(name) CONCAT(name,NB_BITS_GFqm_SUP)
#define CONCAT_NB_WORD_GF2m_SUP(name) CONCAT(name,NB_WORD_GF2m)





#define isEqual_gf2m(a,b) f_ISEQUAL(a,b,NB_WORD_GF2m)

#if (NB_WORD_GF2m<7)
    #define add_gf2m CONCAT(CONCAT_NB_WORD_GF2m_SUP(ADD),_GF2X)
    #define add2_gf2m CONCAT(CONCAT_NB_WORD_GF2m_SUP(ADD),_2_GF2X)
    #define copy_gf2m CONCAT_NB_WORD_GF2m_SUP(COPY)
    #define set0_gf2m CONCAT_NB_WORD_GF2m_SUP(SET0_)
    #define xorLoadMask1_gf2m CONCAT_NB_WORD_GF2m_SUP(XORLOADMASK1_)
    #define dotProduct_gf2_m CONCAT_NB_WORD_GF2m_SUP(DOTPRODUCT)
#else
    #define add_gf2m(a,b,c) ADD_GF2X(a,b,c,NB_WORD_GF2m); 
    #define add2_gf2m(a,b) ADD_2_GF2X(a,b,NB_WORD_GF2m); 
    #define copy_gf2m(c,a) COPY(c,a,NB_WORD_GF2m)
    #define set0_gf2m(c) SET0(c,NB_WORD_GF2m)
    #define xorLoadMask1_gf2m(res,a,b) XORLOADMASK1(res,a,b,NB_WORD_GF2m)
    #define dotProduct_gf2_m(res,a,b) DOTPRODUCT(res,a,b,NB_WORD_GF2m)
#endif

#if (NB_WORD_GF2m<10)
    #define isEqual_nocst_gf2m CONCAT(CONCAT_NB_WORD_GF2m_SUP(ISEQUAL),_NOCST)
#else
    #define isEqual_nocst_gf2m(a,b) f_ISEQUAL_NOCST(a,b,NB_WORD_GF2m)
#endif




/* To choose macro for HFEmq*64 bits */
#if (HFEmq == 1)
    #define NB_BITS_mq_SUP 64
#elif (HFEmq == 2)
    #define NB_BITS_mq_SUP 128
#elif (HFEmq == 3)
    #define NB_BITS_mq_SUP 192
#elif (HFEmq == 4)
    #define NB_BITS_mq_SUP 256
#elif (HFEmq == 5)
    #define NB_BITS_mq_SUP 320
#elif (HFEmq == 6)
    #define NB_BITS_mq_SUP 384
#elif (HFEmq == 7)
    #define NB_BITS_mq_SUP 448
#elif (HFEmq == 8)
    #define NB_BITS_mq_SUP 512
#endif

#define CONCAT_NB_BITS_mq_SUP(name) CONCAT(name,NB_BITS_mq_SUP)





/****************** MEMORY ALIGNMENT ***************************/


/* Size of the alignment in byte */
/* XXX Disabled XXX */
#if (!(NB_WORD_GF2m&3))
    #define SIZE_ALIGNED_GFqm 0/*32*/
#elif (!(NB_WORD_GF2m&1))
    #define SIZE_ALIGNED_GFqm 0/*16*/
#else
    #define SIZE_ALIGNED_GFqm 0
#endif

#if SIZE_ALIGNED_GFqm
    #define ALIGNED_TMP_BUF16 ALIGNED16
    #define ALIGNED_TMP_BUF32 ALIGNED32
    #define STORE128_TMP_BUF PSTORE
    #define STORE256_TMP_BUF VPSTORE
    #define ALIGNED_GFqm_FREE ALIGNED_FREE
#else
    #define ALIGNED_TMP_BUF16
    #define ALIGNED_TMP_BUF32
    #define STORE128_TMP_BUF PSTOREU
    #define STORE256_TMP_BUF VPSTOREU
    #define ALIGNED_GFqm_FREE NO_ALIGNED_FREE
#endif

#if (SIZE_ALIGNED_GFqm==16)
    #define ALIGNED_GFqm ALIGNED16
    #define ALIGNED_GFqm_MALLOC ALIGNED16_MALLOC
    #define ALIGNED_GFqm_CALLOC ALIGNED16_CALLOC

    #define LOAD128_GFqm PLOAD
    #define STORE128_GFqm PSTORE
    /* The both are unaligned */
    #define LOAD256_GFqm VPLOADU
    #define STORE256_GFqm VPSTOREU
#elif (SIZE_ALIGNED_GFqm==32)
    #define ALIGNED_GFqm ALIGNED32
    #define ALIGNED_GFqm_MALLOC ALIGNED32_MALLOC
    #define ALIGNED_GFqm_CALLOC ALIGNED32_CALLOC

    #define LOAD128_GFqm PLOAD
    #define STORE128_GFqm PSTORE
    #define LOAD256_GFqm VPLOAD
    #define STORE256_GFqm VPSTORE
#else
    /* No alignment */
    #define ALIGNED_GFqm
    #define ALIGNED_GFqm_MALLOC NO_ALIGNED_MALLOC
    #define ALIGNED_GFqm_CALLOC NO_ALIGNED_CALLOC

    #define LOAD128_GFqm PLOADU
    #define LOAD256_GFqm VPLOADU
    #define STORE128_GFqm PSTOREU
    #define STORE256_GFqm VPSTOREU
#endif

#define INIT128_GFqm(A128,A) A128=LOAD128_GFqm(A);
#define INIT256_GFqm(A256,A) A256=LOAD256_GFqm(A);





#endif

