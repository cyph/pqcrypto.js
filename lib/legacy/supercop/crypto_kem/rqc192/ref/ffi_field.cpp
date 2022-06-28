/**
 * \file ffi_field.cpp
 * \brief Implementation of ffi_field.h using the NTL library
 */

#include "ffi.h"
#include "ffi_field.h"

int init = 0;
ffi_vec ideal_modulo;


/**
 * \fn void ffi_field_init()
 * \brief This function initializes a finite field
 */
void ffi_field_init() {
	if(!init) {
		init = 1;

    // Polynomial used for field computation
		GF2X P = BuildSparseIrred_GF2X(FIELD_M);
		GF2E::init(P);
    GF2X::HexOutput = 1;

    // Polynomial used for vector multiplication
    unsigned int values[MODULO_NUMBER] = MODULO_VALUES;
    for(int i = 0 ; i < MODULO_NUMBER ; ++i) {
      SetCoeff(ideal_modulo, values[i], 1);
    }
	}
}



/**
 * \fn int ffi_field_get_degree()
 * \brief This function returns the degree m of the finite field GF(2^m)
 *
 * \return Degree of the finite field
 */
int ffi_field_get_degree() {
  return FIELD_M;
}

