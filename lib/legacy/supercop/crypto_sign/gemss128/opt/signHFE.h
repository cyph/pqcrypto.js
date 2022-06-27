#ifndef _SIGNHFE_H
#define _SIGNHFE_H

#include "prefix_name.h"
#include "arch.h"
#include "parameters_HFE.h"
#include "gf2nx.h"
#include <stddef.h>


/* Precomputation for one secret key */
int PREFIX_NAME(precSignHFE)(complete_sparse_monic_gf2nx* F_struct, \
                const UINT** linear_coefs, const UINT* sk);

/* This function can be used with precSignHFE */
int PREFIX_NAME(signHFE1withPrec)(unsigned char* sm8, const UINT* sk, \
                     complete_sparse_monic_gf2nx F_struct, \
                     const UINT** linear_coefs, const UINT* Hi);


/* Sign a message: sm is the signed message, m is the original message,
 *                 and sk is the secret key. */

/* Special function for nb_ite==1 */
int PREFIX_NAME(signHFE1)(unsigned char* sm8, \
                          const unsigned char* m, size_t len, const UINT* sk);

/* Function for nb_ite>0 */
int PREFIX_NAME(signHFE_FeistelPatarin)(unsigned char* sm8, \
                                        const unsigned char* m, size_t len, \
                                        const UINT* sk);


#define precSignHFE PREFIX_NAME(precSignHFE)
#define signHFE1withPrec PREFIX_NAME(signHFE1withPrec)
#define signHFE1 PREFIX_NAME(signHFE1)
#define signHFE_FeistelPatarin PREFIX_NAME(signHFE_FeistelPatarin)


#if ((NB_ITE==1)&&(!EUF_CMA_PROPERTY))
    #define signHFE signHFE1
#else
    #define signHFE signHFE_FeistelPatarin
#endif



#endif
