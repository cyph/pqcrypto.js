/*
 * Copyright (c) 2018, PQShield and Koninklijke Philips N.V.
 * Markku-Juhani O. Saarinen, Koninklijke Philips N.V.
 */

#ifndef _RINGMUL_H_
#define _RINGMUL_H_

#include "r5_parameter_sets.h"

#if PARAMS_K == 1

// create a sparse ternary vector from a seed
void create_secret_vector(uint16_t idx[PARAMS_H / 2][2], const uint8_t *seed);

// multiplication mod q, result length n
void ringmul_q(modq_t d[PARAMS_ND], modq_t a[PARAMS_ND], uint16_t idx[PARAMS_H / 2][2]);

// multiplication mod p, result length mu
void ringmul_p(modp_t d[PARAMS_MU], modp_t a[PARAMS_ND], uint16_t idx[PARAMS_H / 2][2]);

#endif

#endif /* _RINGMUL_H_ */
