/** 
 * \file ffi.h
 * \brief Constants defining the finite field used
 */

#ifndef FFI_H
#define FFI_H

#include <NTL/GF2E.h>
#include <NTL/GF2EX.h>
#include <NTL/GF2X.h>
#include <NTL/GF2XFactoring.h>
#include <NTL/mat_GF2.h>
#include <NTL/vec_vec_GF2.h>
#include <gmp.h>

using namespace NTL;

typedef GF2E  ffi_elt;
typedef GF2EX ffi_vec;

#define FIELD_Q 2 
#define FIELD_M 97
#define FIELD_ELT_BYTES 13

extern ffi_vec ideal_modulo;
#define MODULO_NUMBER 5
#define MODULO_VALUES {0, 1, 2, 5, 67}

#endif

