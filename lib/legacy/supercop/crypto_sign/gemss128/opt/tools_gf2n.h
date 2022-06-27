#ifndef _TOOLS_GF2N_H
#define _TOOLS_GF2N_H

#include "arch.h"
#include "parameters_HFE.h"
#include "macro.h"
#include "init.h"
#include "bit.h"
#include "predicate.h"
#include "boolean.h"



/* This type stores an element of GF(2^n). */
typedef UINT* gf2n;
typedef const UINT* cst_gf2n;
/* to use data[length] */
typedef UINT static_gf2n;



/* n = NB_BITS_UINT*quo + rem */
#define HFEnq (HFEn/NB_BITS_UINT)
#define HFEnr (HFEn%NB_BITS_UINT)
/* The number of word that an element of GF(2^n) needs */
#if (HFEnr)
    #define NB_WORD_GFqn_TMP (HFEnq+1)
#else
    #define NB_WORD_GFqn_TMP HFEnq
#endif

/* Mask for arithmetic in GF(2^n) */
#define HFE_MASKn mask64(HFEnr)

#define HFEnr8 (HFEn&7)
#define HFE_MASKn8 ((1U<<HFEnr8)-1)
/* Number of bytes that an element of GF(2^n) needs */
#define NB_BYTES_GFqn ((HFEn>>3)+((HFEnr8)?1:0))




/* To choose macro for NB_WORD_GFqn*64 bits */
#if (NB_WORD_GFqn_TMP == 1)
    #define NB_BITS_GFqn_SUP 64
    #define NB_WORD_GFqn 1
#elif (NB_WORD_GFqn_TMP == 2)
    #define NB_BITS_GFqn_SUP 128
    #define NB_WORD_GFqn 2
#elif (NB_WORD_GFqn_TMP == 3)
    #define NB_BITS_GFqn_SUP 192
    #define NB_WORD_GFqn 3
#elif (NB_WORD_GFqn_TMP == 4)
    #define NB_BITS_GFqn_SUP 256
    #define NB_WORD_GFqn 4
#elif (NB_WORD_GFqn_TMP == 5)
    #define NB_BITS_GFqn_SUP 320
    #define NB_WORD_GFqn 5
#elif (NB_WORD_GFqn_TMP == 6)
    #define NB_BITS_GFqn_SUP 384
    #define NB_WORD_GFqn 6
#elif (NB_WORD_GFqn_TMP == 7)
    #define NB_BITS_GFqn_SUP 448
    #define NB_WORD_GFqn 7
#elif (NB_WORD_GFqn_TMP == 8)
    #define NB_BITS_GFqn_SUP 512
    #define NB_WORD_GFqn 8
#elif (NB_WORD_GFqn_TMP == 9)
    #define NB_BITS_GFqn_SUP 576
    #define NB_WORD_GFqn 9
#endif

#define CONCAT_NB_BITS_GFqn_SUP(name) CONCAT(name,NB_BITS_GFqn_SUP)
#define CONCAT_NB_WORD_GFqn_SUP(name) CONCAT(name,NB_WORD_GFqn)





#define HFEINIT CONCAT_NB_BITS_GFqn_SUP(INIT)
#define isEqual_gf2n CONCAT_NB_BITS_GFqn_SUP(ISEQUAL)

#if (NB_WORD_GFqn<7)
    #define copy_gf2n CONCAT_NB_BITS_GFqn_SUP(COPY)
    #define set0_gf2n CONCAT_NB_BITS_GFqn_SUP(SET0_)
    #define set1_gf2n CONCAT_NB_BITS_GFqn_SUP(SET1_)
    #define is0_gf2n CONCAT_NB_BITS_GFqn_SUP(ISZERO)
    #define is1_gf2n CONCAT_NB_BITS_GFqn_SUP(ISONE)
    #define cmp_lt_gf2n CONCAT_NB_BITS_GFqn_SUP(CMP_LT)
    #define cmp_gt_gf2n CONCAT_NB_BITS_GFqn_SUP(CMP_GT)
    #define addLoadMask1_gf2n CONCAT_NB_BITS_GFqn_SUP(ADDLOADMASK1_)
#else
    #define copy_gf2n(c,a) COPY(c,a,NB_WORD_GFqn)
    #define set0_gf2n(c) SET0(c,NB_WORD_GFqn)
    #define set1_gf2n(c) SET1(c,NB_WORD_GFqn)
    #define is0_gf2n(a) f_ISZERO(a,NB_WORD_GFqn)
    #define is1_gf2n(a) f_ISONE(a,NB_WORD_GFqn)
    #define cmp_lt_gf2n(a,b) f_CMP_LT(a,b,NB_WORD_GFqn)
    #define cmp_gt_gf2n(a,b) f_CMP_GT(a,b,NB_WORD_GFqn)
    #define addLoadMask1_gf2n(res,a,b) ADDLOADMASK1(res,a,b,NB_WORD_GFqn)
#endif



#if (NB_WORD_GFqn==1)
    #define swap_gf2n PERMUTATION64_TAB
#elif (NB_WORD_GFqn<7)
    #define swap_gf2n CONCAT_NB_BITS_GFqn_SUP(PERMUTATION)
#else
    #define swap_gf2n(a,b) PERMUTATION(ADD2,a,b,NB_WORD_GFqn)
#endif




/****************** Memory alignment ******************/
/* Choose the size {0,16,32} in bytes */

/* Size of the alignment in byte */
#if (!(NB_WORD_GFqn&3))
    #define SIZE_ALIGNED_GFqn 0/*32*/
#elif (!(NB_WORD_GFqn&1))
    #define SIZE_ALIGNED_GFqn 0/*16*/
#else
    #define SIZE_ALIGNED_GFqn 0
#endif

#if ((SIZE_ALIGNED_GFqn)&&(SIZE_ALIGNED_GFqn!=16)&&(SIZE_ALIGNED_GFqn!=32))
    #error "SIZE_ALIGNED_GFqn is 0, 16 or 32 bytes."
#endif

#if SIZE_ALIGNED_GFqn
    #define ALIGNED_GFqn_FREE ALIGNED_FREE
#else
    #define ALIGNED_GFqn_FREE NO_ALIGNED_FREE
#endif

#if (SIZE_ALIGNED_GFqn==32)
    #define ALIGNED ALIGNED32
    #define ALIGNED_MALLOC ALIGNED32_MALLOC
    #define ALIGNED_CALLOC ALIGNED32_CALLOC
#elif (SIZE_ALIGNED_GFqn==16)
    #define ALIGNED ALIGNED16
    #define ALIGNED_MALLOC ALIGNED16_MALLOC
    #define ALIGNED_CALLOC ALIGNED16_CALLOC
#else 
    #define ALIGNED
    #define ALIGNED_MALLOC(p,type,nmemb,size) p=(type)malloc(nmemb*size);
    #define ALIGNED_CALLOC(p,type,nmemb,size) p=(type)calloc(nmemb,size);
#endif






/* Number of word of the res of multiplication */
/* The leading monom is X^(2*(n-1)), so it needs sup(((2*(n-1)+1)/64)) words */
/* sup(((2*(n-1)+1)/64)) = inf(((2*(n-1)+1)/64) + 1) 
 * because (2*(n-1)+1) is odd */
/* A little trick: it is equal at inf(((2*(n-1))/64) + 1) */
#include "mul_gf2x.h"
/* ENABLED_PCLMUL is not defined in optimized implementation */

#if (defined(ENABLED_PCLMUL)&&(!ENABLED_GF2X))
    /* Cela ne marche pas si j'utilise pas *_gf2n */
    /* TODO: decrease the number when it is possible */
/*    #define NB_WORD_MUL ((((HFEn-1)<<1)/NB_BITS_UINT)+1) */

    /* This value is correct */
    #if (HFEn<33)
        #define NB_WORD_MUL 1
    #else
        #define NB_WORD_MUL (NB_WORD_GFqn<<1)
    #endif
#else
    #define NB_WORD_MUL (NB_WORD_GFqn<<1)
#endif



/* To choose macro for NB_WORD_MUL*32 bits */
#if (NB_WORD_MUL == 1)
    #define NB_BITS_MUL_SUP 32
#elif (NB_WORD_MUL == 2)
    #define NB_BITS_MUL_SUP 64
#elif (NB_WORD_MUL == 3)
    #define NB_BITS_MUL_SUP 96
#elif (NB_WORD_MUL == 4)
    #define NB_BITS_MUL_SUP 128
#elif (NB_WORD_MUL == 5)
    #define NB_BITS_MUL_SUP 160
#elif (NB_WORD_MUL == 6)
    #define NB_BITS_MUL_SUP 192
#elif (NB_WORD_MUL == 7)
    #define NB_BITS_MUL_SUP 224
#elif (NB_WORD_MUL == 8)
    #define NB_BITS_MUL_SUP 256
#elif (NB_WORD_MUL == 9)
    #define NB_BITS_MUL_SUP 288
#elif (NB_WORD_MUL == 10)
    #define NB_BITS_MUL_SUP 320
#elif (NB_WORD_MUL == 11)
    #define NB_BITS_MUL_SUP 352
#elif (NB_WORD_MUL == 12)
    #define NB_BITS_MUL_SUP 384
#elif (NB_WORD_MUL == 13)
    #define NB_BITS_MUL_SUP 416
#elif (NB_WORD_MUL == 14)
    #define NB_BITS_MUL_SUP 448
#elif (NB_WORD_MUL == 15)
    #define NB_BITS_MUL_SUP 480
#elif (NB_WORD_MUL == 16)
    #define NB_BITS_MUL_SUP 512
#elif (NB_WORD_MUL == 17)
    #define NB_BITS_MUL_SUP 544
#elif (NB_WORD_MUL == 18)
    #define NB_BITS_MUL_SUP 576
#endif

#define CONCAT_NB_BITS_MUL_SUP(name) CONCAT(name,NB_BITS_MUL_SUP)




#endif
