/// @file blas_comm.c
/// @brief The standard implementations for blas_comm.h
///

#include "blas_comm.h"
#include "blas.h"

#include <stdint.h>


//
//  gf256v_add() should be defined in blas.h
//


void gf256v_set_zero(uint8_t *b, unsigned _num_byte) {
    gf256v_add(b, b, _num_byte);
}

unsigned gf256v_is_zero(const uint8_t *a, unsigned _num_byte) {
    uint8_t r = 0;
    while( _num_byte-- ) { r |= a[0]; a++; }
    return (0 == r);
}

