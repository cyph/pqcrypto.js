/** 
 * \file rsr_algorithm.h
 * \brief Functions for quasi-cyclic rank support recovery problem
 */

#ifndef RSR_ALGORITHM_H
#define RSR_ALGORITHM_H

#include "ffi_vec.h"

void rank_support_recover(ffi_vec& E, unsigned int& E_dim, unsigned int E_expected_dim, const ffi_vec& F, unsigned int F_dim, const ffi_vec& ec, unsigned int ec_size);

#endif

