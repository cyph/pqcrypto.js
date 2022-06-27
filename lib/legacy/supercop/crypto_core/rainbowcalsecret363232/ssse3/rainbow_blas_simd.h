#ifndef _RAINBOW_BLAS_SIMD_H_
#define _RAINBOW_BLAS_SIMD_H_

#include "blas_config.h"

#include "rainbow_config.h"

#ifndef _BLAS_SSE_
error here.
#endif


#include "gf16_sse.h"
#include "blas_sse.h"
#include "blas_matrix_sse.h"

#include "parallel_matrix_op_sse.h"


#ifdef _USE_GF16

#define gfv_generate_multab            gf16v_generate_multab_sse
#define gfmat_prod_multab              gf16mat_prod_multab_sse
#define batch_quad_trimat_eval_multab  batch_quad_trimat_eval_multab_gf16_sse

#define batch_trimat_madd_multab    batch_trimat_madd_multab_gf16_sse
#define batch_trimatTr_madd_multab  batch_trimatTr_madd_multab_gf16_sse
#define batch_2trimat_madd_multab   batch_2trimat_madd_multab_gf16_sse
#define batch_matTr_madd_multab     batch_matTr_madd_multab_gf16_sse
#define batch_bmatTr_madd_multab    batch_bmatTr_madd_multab_gf16_sse
#define batch_mat_madd_multab       batch_mat_madd_multab_gf16_sse

#else

#define gfv_generate_multab            gf256v_generate_multab_sse
#define gfmat_prod_multab              gf256mat_prod_multab_sse
#define batch_quad_trimat_eval_multab  batch_quad_trimat_eval_multab_gf256_sse

#define batch_trimat_madd_multab    batch_trimat_madd_multab_gf256_sse
#define batch_trimatTr_madd_multab  batch_trimatTr_madd_multab_gf256_sse
#define batch_2trimat_madd_multab   batch_2trimat_madd_multab_gf256_sse
#define batch_matTr_madd_multab     batch_matTr_madd_multab_gf256_sse
#define batch_bmatTr_madd_multab    batch_bmatTr_madd_multab_gf256_sse
#define batch_mat_madd_multab       batch_mat_madd_multab_gf256_sse

#endif


#endif
