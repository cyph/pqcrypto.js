#ifndef _TOOLS_GF2M_H
#define _TOOLS_GF2M_H

#include "arch.h"
#include "parameters_HFE.h"
#include "macro.h"
#include "add_gf2x.h"
#include "init.h"
#include "bit.h"
#include "predicate.h"
#include "boolean.h"
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
#define HFE_MASKm mask64(HFEmr)

#define HFEmq8 (HFEm>>3)
#define HFEmr8 (HFEm&7U)

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





#define isEqual_gf2m CONCAT_NB_BITS_GFqm_SUP(ISEQUAL)

#if (NB_WORD_GF2m<7)
    #define copy_gf2m CONCAT_NB_BITS_GFqm_SUP(COPY)
    #define set0_gf2m CONCAT_NB_BITS_GFqm_SUP(SET0_)
    #define addLoadMask1_gf2m CONCAT_NB_BITS_GFqm_SUP(ADDLOADMASK1_)
    #define dotProduct_gf2_m CONCAT_NB_BITS_GFqm_SUP(DOTPRODUCT)
#else
    #define copy_gf2m(c,a) COPY(c,a,NB_WORD_GF2m)
    #define set0_gf2m(c) SET0(c,NB_WORD_GF2m)
    #define addLoadMask1_gf2m(res,a,b) ADDLOADMASK1(res,a,b,NB_WORD_GF2m)
    #define dotProduct_gf2_m(res,a,b) DOTPRODUCT(res,a,b,NB_WORD_GF2m)
#endif

#if (NB_WORD_GF2m==1)
    #define add_gf2m ADD64_TAB
    #define add2_gf2m ADD64_TAB2
#elif (NB_WORD_GF2m<7)
    #define add_gf2m CONCAT_NB_BITS_GFqm_SUP(ADD)
    #define add2_gf2m CONCAT(CONCAT_NB_BITS_GFqm_SUP(ADD),_2)
#else
    #define add_gf2m(a,b,c) ADD(a,b,c,NB_WORD_GF2m); 
    #define add2_gf2m(a,b) ADD2(a,b,NB_WORD_GF2m); 
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
    #define STORE128_TMP_BUF(res,A) _mm_store_si128((__m128i*)(res),A);
    #define STORE256_TMP_BUF(res,A) _mm256_store_si256((__m256i*)(res),A);
    #define ALIGNED_GFqm_FREE ALIGNED_FREE
#else
    #define ALIGNED_TMP_BUF16
    #define ALIGNED_TMP_BUF32
    #define STORE128_TMP_BUF STOREU128
    #define STORE256_TMP_BUF STOREU256
    #define ALIGNED_GFqm_FREE NO_ALIGNED_FREE
#endif

#if (SIZE_ALIGNED_GFqm==16)
    #define ALIGNED_GFqm ALIGNED16
    #define ALIGNED_GFqm_MALLOC ALIGNED16_MALLOC
    #define ALIGNED_GFqm_CALLOC ALIGNED16_CALLOC

    #define LOAD128_GFqm(A) _mm_load_si128((__m128i*)(A))
    #define STORE128_GFqm(res,A) _mm_store_si128((__m128i*)(res),A);
    /* The both are unaligned */
    #define LOAD256_GFqm LOADU256
    #define STORE256_GFqm STOREU256
#elif (SIZE_ALIGNED_GFqm==32)
    #define ALIGNED_GFqm ALIGNED32
    #define ALIGNED_GFqm_MALLOC ALIGNED32_MALLOC
    #define ALIGNED_GFqm_CALLOC ALIGNED32_CALLOC

    #define LOAD128_GFqm(A) _mm_load_si128((__m128i*)(A))
    #define STORE128_GFqm(res,A) _mm_store_si128((__m128i*)(res),A);
    #define LOAD256_GFqm(A) _mm256_load_si256((__m256i*)(A))
    #define STORE256_GFqm(res,A) _mm256_store_si256((__m256i*)(res),A);
#else
    /* No alignment */
    #define ALIGNED_GFqm
    #define ALIGNED_GFqm_MALLOC NO_ALIGNED_MALLOC
    #define ALIGNED_GFqm_CALLOC NO_ALIGNED_CALLOC

    #define LOAD128_GFqm LOADU128
    #define LOAD256_GFqm LOADU256
    #define STORE128_GFqm STOREU128
    #define STORE256_GFqm STOREU256
#endif

#define INIT128_GFqm(A128,A) A128=LOAD128_GFqm(A);
#define INIT256_GFqm(A256,A) A256=LOAD256_GFqm(A);





#endif
