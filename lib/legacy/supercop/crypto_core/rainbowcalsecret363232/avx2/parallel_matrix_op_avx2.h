///  @file  parallel_matrix_op_avx2.h
///  @brief Librarys for operations of batched matrixes,  specialized for AVX2 instruction set.
///
///
#ifndef _P_MATRIX_OP_AVX2_H_
#define _P_MATRIX_OP_AVX2_H_


#define batch_trimat_madd_multab_gf16_avx2 CRYPTO_NAMESPACE( batch_trimat_madd_multab_gf16_avx2 )
#define batch_trimat_madd_multab_gf256_avx2 CRYPTO_NAMESPACE( batch_trimat_madd_multab_gf256_avx2 )
#define batch_trimatTr_madd_multab_gf16_avx2 CRYPTO_NAMESPACE( batch_trimatTr_madd_multab_gf16_avx2 )
#define batch_trimatTr_madd_multab_gf256_avx2 CRYPTO_NAMESPACE( batch_trimatTr_madd_multab_gf256_avx2 )
#define batch_2trimat_madd_multab_gf16_avx2 CRYPTO_NAMESPACE( batch_2trimat_madd_multab_gf16_avx2 )
#define batch_2trimat_madd_multab_gf256_avx2 CRYPTO_NAMESPACE( batch_2trimat_madd_multab_gf256_avx2 )
#define batch_matTr_madd_multab_gf16_avx2 CRYPTO_NAMESPACE( batch_matTr_madd_multab_gf16_avx2 )
#define batch_matTr_madd_multab_gf256_avx2 CRYPTO_NAMESPACE( batch_matTr_madd_multab_gf256_avx2 )
#define batch_bmatTr_madd_multab_gf16_avx2 CRYPTO_NAMESPACE( batch_bmatTr_madd_multab_gf16_avx2 )
#define batch_bmatTr_madd_multab_gf256_avx2 CRYPTO_NAMESPACE( batch_bmatTr_madd_multab_gf256_avx2 )
#define batch_mat_madd_multab_gf16_avx2 CRYPTO_NAMESPACE( batch_mat_madd_multab_gf16_avx2 )
#define batch_mat_madd_multab_gf256_avx2 CRYPTO_NAMESPACE( batch_mat_madd_multab_gf256_avx2 )
#define batch_quad_trimat_eval_multab_gf16_avx2 CRYPTO_NAMESPACE( batch_quad_trimat_eval_multab_gf16_avx2 )
#define batch_quad_trimat_eval_multab_gf256_avx2 CRYPTO_NAMESPACE( batch_quad_trimat_eval_multab_gf256_avx2 )


#ifdef  __cplusplus
extern  "C" {
#endif



////////////////////////////////////////////////////////////////////////
///   Librarys for batched matrix operations.
///   A batched matrix is a matrix which each element of the matrix
///   contains size_batch GF elements.
////////////////////////////////////////////////////////////////////////




////////////////////  Section:  matrix multiplications  ///////////////////////////////



///
/// @brief  bC += btriA * B  , in GF(16)
///
/// @param[out]  bC         - the batched matrix C.
/// @param[in]   btriA      - a batched UT matrix A.
/// @param[in]   B          - a column-major matrix B, in multiplication tables.
/// @param[in]   Bheight          - the height of B.
/// @param[in]   size_Bcolvec     - the size of the column vector in B.
/// @param[in]   Bwidth           - the width of B.
/// @param[in]   size_batch - number of the batched elements in the corresponding position of the matrix.
///
void batch_trimat_madd_multab_gf16_avx2( unsigned char * bC , const unsigned char* btriA ,
        const unsigned char* B , unsigned Bheight, unsigned size_Bcolvec , unsigned Bwidth, unsigned size_batch );

///
/// @brief  bC += btriA * B  , in GF(256)
///
/// @param[out]  bC         - the batched matrix C.
/// @param[in]   btriA      - a batched UT matrix A.
/// @param[in]   B          - a column-major matrix B, in multiplication tables.
/// @param[in]   Bheight          - the height of B.
/// @param[in]   size_Bcolvec     - the size of the column vector in B.
/// @param[in]   Bwidth           - the width of B.
/// @param[in]   size_batch - number of the batched elements in the corresponding position of the matrix.
///
void batch_trimat_madd_multab_gf256_avx2( unsigned char * bC , const unsigned char* btriA ,
        const unsigned char* B , unsigned Bheight, unsigned size_Bcolvec , unsigned Bwidth, unsigned size_batch );


///
/// @brief  bC += btriA^Tr * B  , in GF(16)
///
/// @param[out]  bC         - the batched matrix C.
/// @param[in]   btriA      - a batched UT matrix A. A will be transposed while multiplying.
/// @param[in]   B          - a column-major matrix B, in multiplication tables.
/// @param[in]   Bheight          - the height of B.
/// @param[in]   size_Bcolvec     - the size of the column vector in B.
/// @param[in]   Bwidth           - the width of B.
/// @param[in]   size_batch - number of the batched elements in the corresponding position of the matrix.
///
void batch_trimatTr_madd_multab_gf16_avx2( unsigned char * bC , const unsigned char* btriA ,
        const unsigned char* B , unsigned Bheight, unsigned size_Bcolvec , unsigned Bwidth, unsigned size_batch );

///
/// @brief  bC += btriA^Tr * B  , in GF(256)
///
/// @param[out]  bC         - the batched matrix C.
/// @param[in]   btriA      - a batched UT matrix A. A will be transposed while multiplying.
/// @param[in]   B          - a column-major matrix B, in multiplication tables.
/// @param[in]   Bheight          - the height of B.
/// @param[in]   size_Bcolvec     - the size of the column vector in B.
/// @param[in]   Bwidth           - the width of B.
/// @param[in]   size_batch - number of the batched elements in the corresponding position of the matrix.
///
void batch_trimatTr_madd_multab_gf256_avx2( unsigned char * bC , const unsigned char* btriA ,
        const unsigned char* B , unsigned Bheight, unsigned size_Bcolvec , unsigned Bwidth, unsigned size_batch );



///
/// @brief  bC +=  (btriA + btriA^Tr) *B  , in GF(16)
///
/// @param[out]  bC         - the batched matrix C.
/// @param[in]   btriA      - a batched UT matrix A. The operand for multiplication is (btriA + btriA^Tr).
/// @param[in]   B          - a column-major matrix B, in multiplication tables.
/// @param[in]   Bheight          - the height of B.
/// @param[in]   size_Bcolvec     - the size of the column vector in B.
/// @param[in]   Bwidth           - the width of B.
/// @param[in]   size_batch - number of the batched elements in the corresponding position of the matrix.
///
void batch_2trimat_madd_multab_gf16_avx2( unsigned char * bC , const unsigned char* btriA ,
        const unsigned char* B , unsigned Bheight, unsigned size_Bcolvec , unsigned Bwidth, unsigned size_batch );

///
/// @brief  bC +=  (btriA + btriA^Tr) *B  , in GF(256)
///
/// @param[out]  bC         - the batched matrix C.
/// @param[in]   btriA      - a batched UT matrix A. The operand for multiplication is (btriA + btriA^Tr).
/// @param[in]   B          - a column-major matrix B, in multiplication tables.
/// @param[in]   Bheight          - the height of B.
/// @param[in]   size_Bcolvec     - the size of the column vector in B.
/// @param[in]   Bwidth           - the width of B.
/// @param[in]   size_batch - number of the batched elements in the corresponding position of the matrix.
///
void batch_2trimat_madd_multab_gf256_avx2( unsigned char * bC , const unsigned char* btriA ,
        const unsigned char* B , unsigned Bheight, unsigned size_Bcolvec , unsigned Bwidth, unsigned size_batch );



///
/// @brief  bC += A^Tr * bB  , in GF(16)
///
/// @param[out]  bC           - the batched matrix C.
/// @param[in]   A_to_tr      - the multiplication tables of a column-major matrix A. The operand for multiplication is A^Tr.
/// @param[in]   Aheight      - the height of A.
/// @param[in]   size_Acolvec    - the size of a column vector in A.
/// @param[in]   Awidth           - the width of A.
/// @param[in]   bB          - a batched matrix B.
/// @param[in]   Bwidth           - the width of B.
/// @param[in]   size_batch - number of the batched elements in the corresponding position of the matrix.
///
void batch_matTr_madd_multab_gf16_avx2( unsigned char * bC ,
        const unsigned char* A_to_tr , unsigned Aheight, unsigned size_Acolvec, unsigned Awidth,
        const unsigned char* bB, unsigned Bwidth, unsigned size_batch );

///
/// @brief  bC += A^Tr * bB  , in GF(256)
///
/// @param[out]  bC           - the batched matrix C.
/// @param[in]   A_to_tr      - the multiplication tables of a column-major matrix A. The operand for multiplication is A^Tr.
/// @param[in]   Aheight      - the height of A.
/// @param[in]   size_Acolvec    - the size of a column vector in A.
/// @param[in]   Awidth           - the width of A.
/// @param[in]   bB          - a batched matrix B.
/// @param[in]   Bwidth           - the width of B.
/// @param[in]   size_batch - number of the batched elements in the corresponding position of the matrix.
///
void batch_matTr_madd_multab_gf256_avx2( unsigned char * bC ,
        const unsigned char* A_to_tr , unsigned Aheight, unsigned size_Acolvec, unsigned Awidth,
        const unsigned char* bB, unsigned Bwidth, unsigned size_batch );


/// @brief  bC += bA^Tr * B  , in GF(16)
///
/// @param[out]  bC         - the batched matrix C.
/// @param[in]   bA_to_tr   - a batched matrix A. The operand for multiplication is (bA^Tr).
/// @param[in]   Awidth_befor_tr     - the width of A.
/// @param[in]   B          - a column-major matrix B, in multiplication tables.
/// @param[in]   Bheight          - the height of B.
/// @param[in]   size_Bcolvec     - the size of the column vector in B.
/// @param[in]   Bwidth           - the width of B.
/// @param[in]   size_batch - number of the batched elements in the corresponding position of the matrix.
///
void batch_bmatTr_madd_multab_gf16_avx2( unsigned char *bC , const unsigned char *bA_to_tr, unsigned Awidth_before_tr,
        const unsigned char *B, unsigned Bheight, unsigned size_Bcolvec, unsigned Bwidth, unsigned size_batch );

/// @brief  bC += bA^Tr * B  , in GF(256)
///
/// @param[out]  bC         - the batched matrix C.
/// @param[in]   bA_to_tr   - a batched matrix A. The operand for multiplication is (bA^Tr).
/// @param[in]   Awidth_befor_tr     - the width of A.
/// @param[in]   B          - a column-major matrix B, in multiplication tables.
/// @param[in]   Bheight          - the height of B.
/// @param[in]   size_Bcolvec     - the size of the column vector in B.
/// @param[in]   Bwidth           - the width of B.
/// @param[in]   size_batch - number of the batched elements in the corresponding position of the matrix.
///
void batch_bmatTr_madd_multab_gf256_avx2( unsigned char *bC , const unsigned char *bA_to_tr, unsigned Awidth_before_tr,
        const unsigned char *B, unsigned Bheight, unsigned size_Bcolvec, unsigned Bwidth, unsigned size_batch );



///
/// @brief  bC += bA * B  , in GF(16)
///
/// @param[out]  bC         - the batched matrix C.
/// @param[in]   bA         - a batched matrix A.
/// @param[in]   Aheigh     - the height of A.
/// @param[in]   B          - a column-major matrix B, in multiplicaiton tables.
/// @param[in]   Bheight          - the height of B.
/// @param[in]   size_Bcolvec     - the size of the column vector in B.
/// @param[in]   Bwidth           - the width of B.
/// @param[in]   size_batch - number of the batched elements in the corresponding position of the matrix.
///
void batch_mat_madd_multab_gf16_avx2( unsigned char * bC , const unsigned char* bA , unsigned Aheight,
        const unsigned char* B , unsigned Bheight, unsigned size_Bcolvec , unsigned Bwidth, unsigned size_batch );


///
/// @brief  bC += bA * B  , in GF(256)
///
/// @param[out]  bC         - the batched matrix C.
/// @param[in]   bA         - a batched matrix A.
/// @param[in]   Aheigh     - the height of A.
/// @param[in]   B          - a column-major matrix B, in multiplicaiton tables.
/// @param[in]   Bheight          - the height of B.
/// @param[in]   size_Bcolvec     - the size of the column vector in B.
/// @param[in]   Bwidth           - the width of B.
/// @param[in]   size_batch - number of the batched elements in the corresponding position of the matrix.
///
void batch_mat_madd_multab_gf256_avx2( unsigned char * bC , const unsigned char* bA , unsigned Aheight,
        const unsigned char* B , unsigned Bheight, unsigned size_Bcolvec , unsigned Bwidth, unsigned size_batch );





////////////////////  "quadratric" matrix evaluation  ///////////////////////////////


///
/// @brief  y =  x^Tr * trimat * x  , in GF(16)
///
/// @param[out]  y          - the returned batched element y.
/// @param[in]   trimat     - a batched matrix.
/// @param[in]   multab_x          - an input vector x, in multiplication tables.
/// @param[in]   dim        - the dimension of matrix trimat (and x).
/// @param[in]   size_batch - number of the batched elements in the corresponding position of the matrix.
///
void batch_quad_trimat_eval_multab_gf16_avx2( unsigned char * y, const unsigned char * trimat, const unsigned char * multab_x, unsigned dim , unsigned size_batch );

///
/// @brief  y =  x^Tr * trimat * x  , in GF(256)
///
/// @param[out]  y          - the returned batched element y.
/// @param[in]   trimat     - a batched matrix.
/// @param[in]   multab_x          - an input vector x, in multiplication tables.
/// @param[in]   dim        - the dimension of matrix trimat (and x).
/// @param[in]   size_batch - number of the batched elements in the corresponding position of the matrix.
///
void batch_quad_trimat_eval_multab_gf256_avx2( unsigned char * y, const unsigned char * trimat, const unsigned char * multab_x, unsigned dim , unsigned size_batch );







#ifdef  __cplusplus
}
#endif


#endif // _P_MATRIX_OP_AVX2_H_

