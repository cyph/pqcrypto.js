/** 
 * \file decoder.h
 * \brief Functions for cyclic-support error decoding
 */

#ifndef DECODER_H
#define DECODER_H

#include "ffi_vec.h"

int RS_recover(ffi_vec& E, unsigned int E_expected_dim, const ffi_vec& F, unsigned int F_dim, const ffi_vec& xc, unsigned int xc_size);

#endif

