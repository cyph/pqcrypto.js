/**
 *  polynomial.h
 *  NTS-KEM
 *
 *  Parameter: NTS-KEM(12, 64)
 *  Platform: SSE2/SSE4.1
 *
 *  This file is part of the additional implemention of NTS-KEM
 *  submitted as part of NIST Post-Quantum Cryptography
 *  Standardization Process.
 **/

#ifndef _POLYNOMIAL_H
#define _POLYNOMIAL_H

#include <stdint.h>
#include "ff.h"

typedef struct {
    int size, degree;
    ff_unit *coeff;
} poly;

/**
 *  Initialise a new poly object
 *
 *  @param[in] size     The length of the polynomial
 *  @return The pointer to the polynomial object
 **/
poly* init_poly(int size);

/**
 *  Release a polynomial object
 *
 *  @param[in] px   The pointer to a polynomial object
 **/
void free_poly(poly* px);

/**
 *  Zero a polynomial
 *
 *  @param[in] px   The pointer to a polynomial object
 **/
void zero_poly(poly* px);

/**
 *  Clone a polynomial
 *
 *  @param[in] px   The pointer to a polynomial object
 *  @return The pointer to the cloned polynomial object
 **/
poly* clone_poly(const poly *px);

/**
 *  Polynomial equality check
 *
 *  @param[in] ax   The pointer to a polynomial object
 *  @param[in] bx   The pointer to another polynomial object
 *  @return 1 if they are equal, 0 otherwise
 **/
int is_equal_poly(const poly* ax, const poly* bx);

/**
 *  Compute the degree of a polynomial
 *
 *  @param[in] px   The pointer to a polynomial object
 **/
void update_poly_degree(poly *px);

/**
 *  Create a random polynomial of a given degree at F_2^m
 *
 *  @param[in] ff2m     The pointer to a finite-field object
 *  @param[in] degree   The degree of the polynomial to be created
 *  @return The pointer to the polynomial object created
 **/
poly* create_random_poly(const FF2m* ff2m, int degree);

/**
 *  Evaluate a polynomial at x = `a`
 *
 *  @note
 *  This method is slow. It is better to use AdditiveFFT that
 *  performs multipoint evaluation.
 *
 *  @param[in] ff2m The pointer to a finite-field object
 *  @param[in] px   The pointer to a polynomial object
 *  @param[in] a    The finite-field unit to be evaluated at
 *  @return The evaluated value
 **/
ff_unit evaluate_poly(const FF2m* ff2m, const poly* px, ff_unit a);

/**
 *  Compute the formal derivative of a polynomial
 *
 *  @param[in]  fx  The pointer to a polynomial object
 *  @param[out] dx  The derivative of f(x)
 *  @return 1 on success, 0 otherwise
 **/
int formal_derivative_poly(const poly* fx, poly *dx);

/**
 *  Modulo reduction of a polynomial over a finite-field
 *
 *  The method performs the following operation:
 *      a(x) = a(x) mod m(x)
 *
 *  @param ff2m The pointer to FF2m instance
 *  @param mx   The modulo polynomial
 *  @param ax   The input/output polynomial
 **/
void modulo_reduce_poly(const FF2m* ff2m, const poly *mx, poly *ax);

/**
 *  Obtain the GCD of two finite-field polynomials
 *
 *  The method performs the following operation:
 *      g(x) = GCD(a(x), b(x))
 *
 *  @param ff2m The pointer to FF2m instance
 *  @param ax   The left input polynomial
 *  @param bx   The right input polynomial
 *  @param gx   The output polynomial
 *  @return 1 on successful operation, 0 otherwise
 **/
int gcd_poly(const FF2m* ff2m, const poly* ax, const poly *bx, poly *gx);

/**
 *  Returns a polynomial whose roots are specified by `roots`.
 *
 *  @param[in]  ff2m       The pointer to FF2m instance
 *  @param[in]  roots      The array of roots which must be unique
 *  @param[in]  root_size  The number of roots
 *  @return A polynomial with the specified roots
 **/
poly* poly_from_roots(const FF2m* ff2m, const ff_unit* roots, size_t root_size);

#endif /* _POLYNOMIAL_H */
