#ifndef _ADD_GF2N_H
#define _ADD_GF2N_H

#include "prefix_name.h"
#include "arch.h"
#include "gf2x.h"
#include "tools_gf2n.h"


void PREFIX_NAME(add_gf2n_ref)(gf2n c, cst_gf2n a, cst_gf2n b);
void PREFIX_NAME(add2_gf2n_ref)(gf2n c, cst_gf2n a);
void PREFIX_NAME(add_product_gf2n_ref)(gf2x c, cst_gf2x a, cst_gf2x b);
void PREFIX_NAME(add2_product_gf2n_ref)(gf2x c, cst_gf2x a);

#define add_gf2n_ref PREFIX_NAME(add_gf2n_ref)
#define add2_gf2n_ref PREFIX_NAME(add2_gf2n_ref)
#define add_product_gf2n_ref PREFIX_NAME(add_product_gf2n_ref)
#define add2_product_gf2n_ref PREFIX_NAME(add2_product_gf2n_ref)


#define add_gf2n add_gf2n_ref
#define add2_gf2n add2_gf2n_ref
#define add_product_gf2n add_product_gf2n_ref
#define add2_product_gf2n add2_product_gf2n_ref


#endif

