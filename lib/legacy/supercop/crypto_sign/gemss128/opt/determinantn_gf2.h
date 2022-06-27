#ifndef _DETERMINANTN_GF2_H
#define _DETERMINANTN_GF2_H

#include "prefix_name.h"
#include "matrix_gf2.h"


int PREFIX_NAME(determinantn_nocst_gf2)(cst_Mn_gf2 S);
#define determinantn_nocst_gf2 PREFIX_NAME(determinantn_nocst_gf2)

int PREFIX_NAME(determinantn_gf2)(cst_Mn_gf2 S);
#define determinantn_gf2 PREFIX_NAME(determinantn_gf2)


#endif

