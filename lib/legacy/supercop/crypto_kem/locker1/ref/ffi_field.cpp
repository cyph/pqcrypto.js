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

int done = 0;

/** 
 * \fn void ffi_field_init()
 * \brief This function initializes a finite field
 */
void ffi_field_init() {
	if(!done) {
		done = 1;
		GF2X P = BuildSparseIrred_GF2X(FIELD_M);
  		GF2E::init(P);
	}
}

