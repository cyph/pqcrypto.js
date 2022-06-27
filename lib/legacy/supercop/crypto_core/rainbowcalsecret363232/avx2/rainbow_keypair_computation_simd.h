/// @file rainbow_keypair_computation_simd.h
///
/// The SIMD implementation of functions in rainbow_keypair_computation.h
///

#ifndef _RAINBOW_KEYPAIR_COMP_SIMD_H_
#define _RAINBOW_KEYPAIR_COMP_SIMD_H_

#include "rainbow_keypair_computation.h"


#define calculate_Q_from_F_simd CRYPTO_NAMESPACE( calculate_Q_from_F_simd )
#define calculate_F_from_Q_simd CRYPTO_NAMESPACE( calculate_F_from_Q_simd )
#define calculate_Q_from_F_cyclic_simd CRYPTO_NAMESPACE( calculate_Q_from_F_cyclic_simd )


#ifdef  __cplusplus
extern  "C" {
#endif


///
/// @brief Computing public key from secret key
///
/// @param[out] Qs       - the public key
/// @param[in]  Fs       - parts of the secret key: l1_F1, l1_F2, l2_F1, l2_F2, l2_F3, l2_F5, l2_F6
/// @param[in]  Ts       - parts of the secret key: T1, T4, T3
///
void calculate_Q_from_F_simd( ext_cpk_t * Qs, const sk_t * Fs , const sk_t * Ts );

///
/// @brief Computing parts of the sk from parts of pk and sk
///
/// @param[out] Fs       - parts of the sk: l1_F1, l1_F2, l2_F1, l2_F2, l2_F3, l2_F5, l2_F6
/// @param[in]  Qs       - parts of the pk: l1_Q1, l1_Q2, l2_Q1, l2_Q2, l2_Q3, l2_Q5, l2_Q6
/// @param[in]  Ts       - parts of the sk: T1, T4, T3
///
void calculate_F_from_Q_simd( sk_t * Fs , const sk_t * Qs , const sk_t * Ts );

///
/// @brief Computing parts of the pk from the secret key
///
/// @param[out] Qs       - parts of the pk: l1_Q3, l1_Q5, l2_Q6, l1_Q9, l2_Q9
/// @param[in]  Fs       - parts of the sk: l1_F1, l1_F2, l2_F1, l2_F2, l2_F3, l2_F5, l2_F6
/// @param[in]  Ts       - parts of the sk: T1, T4, T3
///
void calculate_Q_from_F_cyclic_simd( cpk_t * Qs, const sk_t * Fs , const sk_t * Ts );



#ifdef  __cplusplus
}
#endif

#endif  // _RAINBOW_KEYPAIR_COMP_SIMD_H_

