#ifndef _RANDMATRIX_GF2_H
#define _RANDMATRIX_GF2_H

#include "prefix_name.h"
#include "parameters_HFE.h"
#include "matrix_gf2.h"


void PREFIX_NAME(randMatrix_gf2_n)(Mn_gf2 S);
#define randMatrix_gf2_n PREFIX_NAME(randMatrix_gf2_n)
#if HFEv
    void PREFIX_NAME(randMatrix_gf2_nv)(Mnv_gf2 S);
    #define randMatrix_gf2_nv PREFIX_NAME(randMatrix_gf2_nv)
#else
    #define randMatrix_gf2_nv randMatrix_gf2_n
#endif

void PREFIX_NAME(genLowerMatrixn)(Tn_gf2 L);
#define genLowerMatrixn PREFIX_NAME(genLowerMatrixn)
#if HFEv
    void PREFIX_NAME(genLowerMatrixnv)(Tnv_gf2 L);
    #define genLowerMatrixnv PREFIX_NAME(genLowerMatrixnv)
#else
    #define genLowerMatrixnv genLowerMatrixn
#endif

#define GEN_LU_Tn(L,U) \
    genLowerMatrixn(L);\
    /* Generate the transpose of U */\
    genLowerMatrixn(U);

#define GEN_LU_Tnv(L,U) \
    genLowerMatrixnv(L);\
    /* Generate the transpose of U */\
    genLowerMatrixnv(U);

int PREFIX_NAME(randInvMatrix_gf2_n)(GLn_gf2 S);
#define randInvMatrix_gf2_n PREFIX_NAME(randInvMatrix_gf2_n)
#if HFEv
    int PREFIX_NAME(randInvMatrix_gf2_nv)(GLnv_gf2 S);
    #define randInvMatrix_gf2_nv PREFIX_NAME(randInvMatrix_gf2_nv)
#else
    #define randInvMatrix_gf2_nv randInvMatrix_gf2_n
#endif

void PREFIX_NAME(randInvMatrixLU_gf2_n)(GLn_gf2 S, cst_Tn_gf2 L, cst_Tn_gf2 U);
#define randInvMatrixLU_gf2_n PREFIX_NAME(randInvMatrixLU_gf2_n)
#if HFEv
    void PREFIX_NAME(randInvMatrixLU_gf2_nv)(GLn_gf2 S, cst_Tn_gf2 L, cst_Tn_gf2 U);
    #define randInvMatrixLU_gf2_nv PREFIX_NAME(randInvMatrixLU_gf2_nv)
#else
    #define randInvMatrixLU_gf2_nv randInvMatrixLU_gf2_n
#endif


#endif

