#ifndef _DETERMINANTN_GF2_H
#define _DETERMINANTN_GF2_H

#include "prefix_name.h"
#include "gf2.h"
#include "matrix_gf2.h"
#include "arch.h"


gf2 PREFIX_NAME(determinantn_nocst_gf2)(cst_Mn_gf2 S, Mn_gf2 S_cp);
#define determinantn_nocst_gf2 PREFIX_NAME(determinantn_nocst_gf2)

gf2 PREFIX_NAME(determinantn_cst_gf2)(cst_Mn_gf2 S, Mn_gf2 S_cp);
#define determinantn_cst_gf2 PREFIX_NAME(determinantn_cst_gf2)


#if CONSTANT_TIME
    #define determinantn_gf2 determinantn_cst_gf2
#else
    #define determinantn_gf2 determinantn_nocst_gf2
#endif


#endif

