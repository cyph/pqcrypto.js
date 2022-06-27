#ifndef _ADD_GF2N_H
#define _ADD_GF2N_H

#include "prefix_name.h"
#include "arch.h"
#include "gf2x.h"
#include "add_gf2x.h"
#include "tools_gf2n.h"
#include "macro.h"


/* The following macros are used only in this file. */

/* Addition in GF(2^n). */
#if (NB_WORD_GFqn<7)
    #define ADD_GF2N CONCAT(CONCAT_NB_WORD_GFqn_SUP(ADD),_GF2X)
    #define ADD2_GF2N CONCAT(CONCAT_NB_WORD_GFqn_SUP(ADD),_2_GF2X)
#else
    #define ADD_GF2N(c,a,b) ADD_GF2X(c,a,b,NB_WORD_GFqn)
    #define ADD2_GF2N(c,a) ADD_2_GF2X(c,a,NB_WORD_GFqn)
#endif


/* Addition in GF(2)[x] for (2n-1)-coefficients polynomials. */
#if (NB_WORD_MMUL_TMP<19)
    #define ADD_PRODUCT_GF2N CONCAT(CONCAT_NB_WORD_MMUL(ADD),_GF2X)
    #define ADD2_PRODUCT_GF2N CONCAT(CONCAT_NB_WORD_MMUL(ADD),_2_GF2X)
#else
    #define ADD_PRODUCT_GF2N(c,a,b) ADD_GF2X(c,a,b,NB_WORD_MMUL)
    #define ADD2_PRODUCT_GF2N(c,a) ADD_2_GF2X(c,a,NB_WORD_MMUL)
#endif


void PREFIX_NAME(add_gf2n_ref)(gf2n c, cst_gf2n a, cst_gf2n b);
void PREFIX_NAME(add2_gf2n_ref)(gf2n c, cst_gf2n a);
void PREFIX_NAME(add_product_gf2n_ref)(gf2x c, cst_gf2x a, cst_gf2x b);
void PREFIX_NAME(add2_product_gf2n_ref)(gf2x c, cst_gf2x a);

#define add_gf2n_ref PREFIX_NAME(add_gf2n_ref)
#define add2_gf2n_ref PREFIX_NAME(add2_gf2n_ref)
#define add_product_gf2n_ref PREFIX_NAME(add_product_gf2n_ref)
#define add2_product_gf2n_ref PREFIX_NAME(add2_product_gf2n_ref)


#ifdef MQSOFT_REF
    #define add_gf2n add_gf2n_ref
    #define add2_gf2n add2_gf2n_ref
    #define add_product_gf2n add_product_gf2n_ref
    #define add2_product_gf2n add2_product_gf2n_ref
#else
    /* The addition function is inlined. */
    #define add_gf2n ADD_GF2N
    #define add2_gf2n ADD2_GF2N
    #define add_product_gf2n ADD_PRODUCT_GF2N
    #define add2_product_gf2n ADD2_PRODUCT_GF2N
#endif




#endif

