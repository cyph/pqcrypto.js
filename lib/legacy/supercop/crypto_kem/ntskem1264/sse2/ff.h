/**
 *  ff.h
 *  NTS-KEM
 *
 *  Parameter: NTS-KEM(12, 64)
 *  Platform: SSE2/SSE4.1
 *
 *  This file is part of the additional implemention of NTS-KEM
 *  submitted as part of NIST Post-Quantum Cryptography
 *  Standardization Process.
 **/

#ifndef ff_h
#define ff_h

#include <stdint.h>
#if defined(__SSE2__) || defined(__AVX2__)
#include <immintrin.h>
#endif
#include "bits.h"

typedef uint16_t ff_unit;

/**
 *  Finite field F_{2^m}
 *
 *  @note
 *  The addition, multiplication, squaring and inversion operations
 *  are done without the use of any look-up tables. Only m = {12,13}
 *  cases are implemented at this stage.
 **/
typedef struct FF2m {
    /**
     *  Dimension of extension over F_2
     **/
    int m;
    
    /**
     *  Add two finite-field elements over F_{2^m}
     *
     *  @param[in] ff2m  Pointer to FF2m instance
     *  @param[in] a     Operand a
     *  @param[in] b     Operand b
     *  @return Sum of a and b in the field F_{2^m}
     **/
    ff_unit (*ff_add)(const struct FF2m* ff2m, ff_unit a, ff_unit b);
    
    /**
     *  Multiply two finite-field elements over F_{2^m}
     *
     *  @param[in] ff2m  Pointer to FF2m instance
     *  @param[in] a     Operand a
     *  @param[in] b     Operand b
     *  @return Product of a and b in the field F_{2^m}
     **/
    ff_unit (*ff_mul)(const struct FF2m* ff2m, ff_unit a, ff_unit b);
    
    /**
     *  Squaring of an element over F_{2^m}
     *
     *  @param[in] ff2m  Pointer to FF2m instance
     *  @param[in] a     Operand a
     *  @return a^2 in the field F_{2^m}
     **/
    ff_unit (*ff_sqr)(const struct FF2m* ff2m, ff_unit a);
    
    /**
     *  Inversion of an element over F_{2^m}
     *
     *  @param[in] ff2m  Pointer to FF2m instance
     *  @param[in] a     Operand a
     *  @return a^{-1} in the field F_{2^m}
     **/
    ff_unit (*ff_inv)(const struct FF2m* ff2m, ff_unit a);
    
    /**
     *  Vectorised multiplication
     *
     *  Each input or output operand is an array of 
     *  m {64,128,256}-bit blocks, depending on architecture.
     *
     *  @param[in]  ff2m  Pointer to FF2m instance
     *  @param[out] c     Pointer to an output array
     *  @param[in]  a     Pointer to an input array
     *  @param[in]  b     Pointer to another input array
     **/
    void (*vector_ff_mul)(const struct FF2m* ff2m,
                          vector* c,
                          const vector* a,
                          const vector* b);
    
    /**
     *  Vectorised squaring
     *
     *  Each input or output operand is an array of
     *  m {64,128,256}-bit blocks, depending on architecture.
     *
     *  @param[in]  ff2m  Pointer to FF2m instance
     *  @param[out] b     Pointer to an output array
     *  @param[in]  a     Pointer to an input array
     **/
    void (*vector_ff_sqr)(const struct FF2m* ff2m,
                          vector* b,
                          const vector* a);
    
    /**
     *  Vectorised inversion
     *
     *  Each input or output operand is an array of
     *  m {64,128,256}-bit blocks, depending on architecture.
     *
     *  @param[in]  ff2m  Pointer to FF2m instance
     *  @param[out] b     Pointer to an output array
     *  @param[in]  a     Pointer to an input array
     **/
    void (*vector_ff_inv)(const struct FF2m* ff2m,
                          vector* b,
                          const vector* a);

    /**
     *  Vectorised square and inversion
     *
     *  Each input or output operand is an array of
     *  m {64,128,256}-bit blocks, depending on architecture.
     *
     *  @param[in]  ff2m  Pointer to FF2m instance
     *  @param[out] b     Pointer to an output array
     *  @param[in]  a     Pointer to an input array
     **/
    void (*vector_ff_sqr_inv)(const struct FF2m* ff2m,
                              vector* b,
                              const vector* a);

    /**
     *  Obtain the XOR of the tranposed of the input array
     *
     *  The input operand is an array of m {64,128,256}-bit 
     *  blocks, depending on architecture.
     *
     *  @param[in]  ff2m  Pointer to FF2m instance
     *  @param[in]  a     Pointer to an input array
     *  @return The XOR output
     **/
    ff_unit (*vector_ff_transpose_xor)(const struct FF2m* ff2m,
                                       const vector* a);
    
    /**
     *  Basis
     **/
    ff_unit* basis;
} FF2m;

/**
 *  Create an instance of FF2m object
 *  
 *  @return FF2m instance on success, otherwise NULL
 **/
FF2m* ff_create();

/**
 *  Release an FF2m instance created by {@see ff_create}.
 *
 *  @param[in] ff2m  Instance to an FF2m object
 **/
void ff_release(FF2m* ff2m);

#endif /* ff_h */
