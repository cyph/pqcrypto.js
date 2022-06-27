#ifndef _SIGNHFE_H
#define _SIGNHFE_H

#include "prefix_name.h"
#include "arch.h"
#include "parameters_HFE.h"
#include "gf2nx.h"
#include "config_HFE.h"
#include "matrix_gf2.h"
#include <stddef.h>


typedef struct _secret_key_HFE
{
    complete_sparse_monic_gf2nx F_struct;
    #if ENABLED_SEED_SK
        sparse_monic_gf2nx F_HFEv;
        GLnv_gf2 S;
        GLn_gf2 T;
        #if AFFINE_TRANSFORMATION_BY_t
            vecm_gf2 t;
        #endif
    #else
        cst_sparse_monic_gf2nx F_HFEv;
        cst_GLnv_gf2 S;
        cst_GLn_gf2 T;
        #if AFFINE_TRANSFORMATION_BY_t
            cst_vecm_gf2 t;
        #endif
    #endif

    #if ENABLED_SEED_SK
        UINT *sk_uncomp;
    #endif
} secret_key_HFE;


#if ENABLED_SEED_SK
    void PREFIX_NAME(precSignHFESeed)(secret_key_HFE *sk_HFE, const UINT *sk);
    #define precSignHFESeed PREFIX_NAME(precSignHFESeed)
#endif

/* Precomputation for one secret-key */
int PREFIX_NAME(precSignHFE)(secret_key_HFE* sk_HFE, const UINT** linear_coefs,
                                                     const UINT *sk);

/* Sign a message: sm is the signed message, m is the original message,
 *                 and sk is the secret-key. */

/* Function for nb_ite>0 */
int PREFIX_NAME(signHFE_FeistelPatarin)(unsigned char* sm8,
                                        const unsigned char* m, size_t len,
                                        const UINT* sk);


#define precSignHFE PREFIX_NAME(precSignHFE)
#define signHFE_FeistelPatarin PREFIX_NAME(signHFE_FeistelPatarin)


#define signHFE signHFE_FeistelPatarin



#endif

