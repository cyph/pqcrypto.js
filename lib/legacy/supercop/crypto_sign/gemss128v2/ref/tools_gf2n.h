#ifndef _TOOLS_GF2N_H
#define _TOOLS_GF2N_H

#include "arch.h"
#include "parameters_HFE.h"
#include "macro.h"
#include "init.h"
#include "bit.h"
#include "predicate.h"
#include "loadMask.h"


/* This type stores an element of GF(2^n). */
typedef UINT* gf2n;
typedef const UINT* cst_gf2n;
/* to use data[length] */
typedef UINT static_gf2n;
typedef const UINT cst_static_gf2n;


/* Floor(Log(2,n)) */
#if (HFEn<2)
    #define LOG2_N 0
#elif (HFEn<4)
    #define LOG2_N 1
#elif (HFEn<8)
    #define LOG2_N 2
#elif (HFEn<16)
    #define LOG2_N 3
#elif (HFEn<32)
    #define LOG2_N 4
#elif (HFEn<64)
    #define LOG2_N 5
#elif (HFEn<128)
    #define LOG2_N 6
#elif (HFEn<256)
    #define LOG2_N 7
#elif (HFEn<512)
    #define LOG2_N 8
#elif (HFEn<1024)
    #define LOG2_N 9
#elif (HFEn<2048)
    #define LOG2_N 10
#endif

#define POW_FLOG2_N (1<<LOG2_N)


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
#define MASK_GF2n maskUINT(HFEnr)

#define HFEnr8 (HFEn&7)
#define MASK8_GF2n ((1U<<HFEnr8)-1)
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





#define PINIT_GF2N CONCAT_NB_BITS_GFqn_SUP(PINIT)
#define isEqual_gf2n(a,b) f_ISEQUAL(a,b,NB_WORD_GFqn)
#define is0_gf2n(a) f_ISZERO(a,NB_WORD_GFqn)
#define isNot0_gf2n(a) f_ISNOTZERO(a,NB_WORD_GFqn)
#define is1_gf2n(a) f_ISONE(a,NB_WORD_GFqn)
#define cmp_lt_gf2n(a,b) f_CMP_LT(a,b,NB_WORD_GFqn)
#define cmp_gt_gf2n(a,b) f_CMP_GT(a,b,NB_WORD_GFqn)

#if (NB_WORD_GFqn<7)
    #define swap_gf2n CONCAT_NB_WORD_GFqn_SUP(SWAP)
    #define copy_gf2n CONCAT_NB_WORD_GFqn_SUP(COPY)
    #define set0_gf2n CONCAT_NB_WORD_GFqn_SUP(SET0_)
    #define set1_gf2n CONCAT_NB_WORD_GFqn_SUP(SET1_)
    #define xorLoadMask1_gf2n CONCAT_NB_WORD_GFqn_SUP(XORLOADMASK1_)
#else
    #define swap_gf2n(a,b) SWAP(XOR_2,a,b,NB_WORD_GFqn)
    #define copy_gf2n(c,a) COPY(c,a,NB_WORD_GFqn)
    #define set0_gf2n(c) SET0(c,NB_WORD_GFqn)
    #define set1_gf2n(c) SET1(c,NB_WORD_GFqn)
    #define xorLoadMask1_gf2n(res,a,b) XORLOADMASK1(res,a,b,NB_WORD_GFqn)
#endif

#if (NB_WORD_GFqn<10)
    #define isEqual_nocst_gf2n CONCAT(CONCAT_NB_WORD_GFqn_SUP(ISEQUAL),_NOCST)
    #define cmp_lt_nocst_gf2n CONCAT(CONCAT_NB_WORD_GFqn_SUP(CMP_LT),_NOCST)
#else
    #define isEqual_nocst_gf2n(a,b) f_ISEQUAL_NOCST(a,b,NB_WORD_GFqn)
    #define cmp_lt_nocst_gf2n(a,b) f_CMP_LT_NOCST(a,b,NB_WORD_GFqn)
#endif

#define cmp_gt_nocst_gf2n(a,b) cmp_lt_nocst_gf2n(b,a)




/****************** MEMORY ALIGNMENT ***************************/


/* Size of the alignment in byte */
/* XXX Disabled XXX */
#if (!(NB_WORD_GFqn&3))
    #define SIZE_ALIGNED_GFqn 0/*32*/
#elif (!(NB_WORD_GFqn&1))
    #define SIZE_ALIGNED_GFqn 0/*16*/
#else
    #define SIZE_ALIGNED_GFqn 0
#endif

#if SIZE_ALIGNED_GFqn
    #define ALIGNED_GFqn_FREE ALIGNED_FREE
#else
    #define ALIGNED_GFqn_FREE NO_ALIGNED_FREE
#endif

#if (SIZE_ALIGNED_GFqn==16)
    #define ALIGNED_GFqn ALIGNED16
    #define ALIGNED_GFqn_MALLOC ALIGNED16_MALLOC
    #define ALIGNED_GFqn_CALLOC ALIGNED16_CALLOC

    #define LOAD128_GFqn PLOAD
    #define STORE128_GFqn PSTORE
    /* The both are unaligned */
    #define LOAD256_GFqn VPLOADU
    #define STORE256_GFqn VPSTOREU
#elif (SIZE_ALIGNED_GFqn==32)
    #define ALIGNED_GFqn ALIGNED32
    #define ALIGNED_GFqn_MALLOC ALIGNED32_MALLOC
    #define ALIGNED_GFqn_CALLOC ALIGNED32_CALLOC

    #define LOAD128_GFqn PLOAD
    #define STORE128_GFqn PSTORE
    #define LOAD256_GFqn VPLOAD
    #define STORE256_GFqn VPSTORE
#else
    /* No alignment */
    #define ALIGNED_GFqn
    #define ALIGNED_GFqn_MALLOC NO_ALIGNED_MALLOC
    #define ALIGNED_GFqn_CALLOC NO_ALIGNED_CALLOC

    #define LOAD128_GFqn PLOADU
    #define LOAD256_GFqn VPLOADU
    #define STORE128_GFqn PSTOREU
    #define STORE256_GFqn VPSTOREU
#endif

#define INIT128_GFqn(A128,A) A128=LOAD128_GFqn(A);
#define INIT256_GFqn(A256,A) A256=LOAD256_GFqn(A);





/* Number of words of the result of a multiplication */
#if (HFEn<33)
    #define NB_WORD_MUL 1
#elif ((ENABLED_GF2X&&(!defined(MODE_64_BITS)))||(!ENABLED_GF2X))
    /* Only the multiplication of gf2x in 64 bits does not 
       support an odd number of words for the product */
    /* The squaring supports the minimal number of words for the product */
    /* The mod reduction supports the minimal number of words for the product */

    /* Minimal number of words:
       The leading monomial is X^(2*(n-1)), so it needs sup(((2*(n-1)+1)/64)) 
       words. sup(((2*(n-1)+1)/64)) = inf(((2*(n-1)+1)/64) + 1) because 
       (2*(n-1)+1) is odd. A little trick: it is equal to 
       inf(((2*(n-1))/64) + 1) */
    #define NB_WORD_MUL ((((HFEn-1)<<1)/NB_BITS_UINT)+1)
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



/* Minimum number of words to store the product of 2 (n-1)-elements of GF2[x] */
#define NB_WORD_MMUL_TMP ((((HFEn-1)<<1)/NB_BITS_UINT)+1)


#if (NB_WORD_MMUL_TMP == 1)
    #define NB_WORD_MMUL 1
    #define NB_BITS_MMUL_SUP 32
#elif (NB_WORD_MMUL_TMP == 2)
    #define NB_WORD_MMUL 2
    #define NB_BITS_MMUL_SUP 64
#elif (NB_WORD_MMUL_TMP == 3)
    #define NB_WORD_MMUL 3
    #define NB_BITS_MMUL_SUP 96
#elif (NB_WORD_MMUL_TMP == 4)
    #define NB_WORD_MMUL 4
    #define NB_BITS_MMUL_SUP 128
#elif (NB_WORD_MMUL_TMP == 5)
    #define NB_WORD_MMUL 5
    #define NB_BITS_MMUL_SUP 160
#elif (NB_WORD_MMUL_TMP == 6)
    #define NB_WORD_MMUL 6
    #define NB_BITS_MMUL_SUP 192
#elif (NB_WORD_MMUL_TMP == 7)
    #define NB_WORD_MMUL 7
    #define NB_BITS_MMUL_SUP 224
#elif (NB_WORD_MMUL_TMP == 8)
    #define NB_WORD_MMUL 8
    #define NB_BITS_MMUL_SUP 256
#elif (NB_WORD_MMUL_TMP == 9)
    #define NB_WORD_MMUL 9
    #define NB_BITS_MMUL_SUP 288
#elif (NB_WORD_MMUL_TMP == 10)
    #define NB_WORD_MMUL 10
    #define NB_BITS_MMUL_SUP 320
#elif (NB_WORD_MMUL_TMP == 11)
    #define NB_WORD_MMUL 11
    #define NB_BITS_MMUL_SUP 352
#elif (NB_WORD_MMUL_TMP == 12)
    #define NB_WORD_MMUL 12
    #define NB_BITS_MMUL_SUP 384
#elif (NB_WORD_MMUL_TMP == 13)
    #define NB_WORD_MMUL 13
    #define NB_BITS_MMUL_SUP 416
#elif (NB_WORD_MMUL_TMP == 14)
    #define NB_WORD_MMUL 14
    #define NB_BITS_MMUL_SUP 448
#elif (NB_WORD_MMUL_TMP == 15)
    #define NB_WORD_MMUL 15
    #define NB_BITS_MMUL_SUP 480
#elif (NB_WORD_MMUL_TMP == 16)
    #define NB_WORD_MMUL 16
    #define NB_BITS_MMUL_SUP 512
#elif (NB_WORD_MMUL_TMP == 17)
    #define NB_WORD_MMUL 17
    #define NB_BITS_MMUL_SUP 544
#elif (NB_WORD_MMUL_TMP == 18)
    #define NB_WORD_MMUL 18
    #define NB_BITS_MMUL_SUP 576
#else
    #define NB_WORD_MMUL NB_WORD_MMUL_TMP
#endif


#define CONCAT_NB_WORD_MMUL(name) CONCAT(name,NB_WORD_MMUL)
#define CONCAT_NB_BITS_MMUL_SUP(name) CONCAT(name,NB_BITS_MMUL_SUP)



#if (NB_WORD_MMUL<7)
    #define set0_product_gf2n CONCAT_NB_WORD_MMUL(SET0_)
#else
    #define set0_product_gf2n(c) SET0(c,NB_WORD_MMUL)
#endif

#if (NB_WORD_MMUL==NB_WORD_GFqn)
    /* Nothing to set to 0 */
    #define set0_high_product_gf2n(c)
#else
    #define set0_high_product_gf2n(c) \
            SET0((c)+NB_WORD_GFqn,NB_WORD_MMUL-NB_WORD_GFqn)
#endif



#endif

