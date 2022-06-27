/** 
 * \file ffi.h
 * \brief Constants to define the finite field used
 */
#ifndef FFI_H
#define FFI_H

#include <NTL/GF2E.h>
#include <NTL/GF2EX.h>
#include "parameters.h"

using namespace NTL;

typedef GF2E  ffi_elt;
typedef GF2EX ffi_vec;

#define FIELD_Q 2 
#define FIELD_M PARAM_M
#define FIELD_ELT_BYTES GF2MBYTES

#endif

