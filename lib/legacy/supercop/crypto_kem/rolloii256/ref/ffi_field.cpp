/**
 * \file ffi_field.cpp
 * \brief Implementation of ffi_field.h
 */

#include <NTL/GF2E.h>
#include <NTL/GF2X.h>
#include <NTL/GF2XFactoring.h>
#include "ffi.h"
#include "ffi_field.h"

using namespace NTL;

int init = 0;
ffi_vec ideal_modulo;

/** 
 * \fn void ffi_field_init()
 * \brief This function initializes a finite field
 */
void ffi_field_init() {
	if(!init) {
		init = 1;
		GF2X P = BuildSparseIrred_GF2X(FIELD_M);
  		GF2E::init(P);
		GF2X::HexOutput = 1;

	    int coeffs[NMODCOEFFS] = MODCOEFFS;
	    int i;
	    for(i=0 ; i<NMODCOEFFS ; i++) {
	      SetCoeff(ideal_modulo, coeffs[i], 1);
	    }
	}
}

