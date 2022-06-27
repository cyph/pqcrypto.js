#ifndef _RANDMATRIX_GF2_H
#define _RANDMATRIX_GF2_H

#include "prefix_name.h"
#include "parameters_HFE.h"
#include "matrix_gf2.h"


#if HFEnr
    void PREFIX_NAME(cleanMatrix_gf2_n)(Mn_gf2 S);
    #define cleanMatrix_gf2_n PREFIX_NAME(cleanMatrix_gf2_n)
#endif
#if HFEnvr
    void PREFIX_NAME(cleanMatrix_gf2_nv)(Mnv_gf2 S);
    #define cleanMatrix_gf2_nv PREFIX_NAME(cleanMatrix_gf2_nv)
#else
    #define cleanMatrix_gf2_nv cleanMatrix_gf2_n
#endif


void PREFIX_NAME(randMatrix_gf2_n)(Mn_gf2 S);
#define randMatrix_gf2_n PREFIX_NAME(randMatrix_gf2_n)
#if HFEv
    void PREFIX_NAME(randMatrix_gf2_nv)(Mnv_gf2 S);
    #define randMatrix_gf2_nv PREFIX_NAME(randMatrix_gf2_nv)
#else
    #define randMatrix_gf2_nv randMatrix_gf2_n
#endif

void PREFIX_NAME(cleanLowerMatrixn)(Tn_gf2 L);
void PREFIX_NAME(genLowerMatrixn)(Tn_gf2 L);
#define cleanLowerMatrixn PREFIX_NAME(cleanLowerMatrixn)
#define genLowerMatrixn PREFIX_NAME(genLowerMatrixn)
#if HFEv
    void PREFIX_NAME(cleanLowerMatrixnv)(Tnv_gf2 L);
    void PREFIX_NAME(genLowerMatrixnv)(Tnv_gf2 L);
    #define cleanLowerMatrixnv PREFIX_NAME(cleanLowerMatrixnv)
    #define genLowerMatrixnv PREFIX_NAME(genLowerMatrixnv)
#else
    #define cleanLowerMatrixnv cleanLowerMatrixn
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

void PREFIX_NAME(randInvMatrix_gf2_n)(GLn_gf2 S, Mn_gf2 S_cp);
#define randInvMatrix_gf2_n PREFIX_NAME(randInvMatrix_gf2_n)
#if HFEv
    void PREFIX_NAME(randInvMatrix_gf2_nv)(GLnv_gf2 S, Mnv_gf2 S_cp);
    #define randInvMatrix_gf2_nv PREFIX_NAME(randInvMatrix_gf2_nv)
#else
    #define randInvMatrix_gf2_nv randInvMatrix_gf2_n
#endif

void PREFIX_NAME(mulMatricesLU_gf2_n)(Mn_gf2 S, cst_Tn_gf2 L, cst_Tn_gf2 U);
#define mulMatricesLU_gf2_n PREFIX_NAME(mulMatricesLU_gf2_n)
#if HFEv
    void PREFIX_NAME(mulMatricesLU_gf2_nv)(Mnv_gf2 S, cst_Tnv_gf2 L,
                                                        cst_Tnv_gf2 U);
    #define mulMatricesLU_gf2_nv PREFIX_NAME(mulMatricesLU_gf2_nv)
#else
    #define mulMatricesLU_gf2_nv mulMatricesLU_gf2_n
#endif


#define randInvMatrixLU_gf2_n(S,L,U) \
            GEN_LU_Tn(L,U);\
            mulMatricesLU_gf2_n(S,L,U);

#define randInvMatrixLU_gf2_nv(S,L,U) \
            GEN_LU_Tnv(L,U);\
            mulMatricesLU_gf2_nv(S,L,U);


#endif

