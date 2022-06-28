/** 
 * \file q_polynomial.h
 * \brief Functions to manipulate q-polynomials. 
 *
 * The q_polynomials are polynomials over a field of the form \f$ P(x) = \sum_{i=0}^{n} p_i \times x^{q^i} \f$ with \f$ q \f$ a rational prime.
 * See \cite ore:qpolynomials for a description of their main properties.
 *
 */

#ifndef Q_POLYNOMIAL_H
#define Q_POLYNOMIAL_H

#include "ffi_elt.h"
#include "ffi_vec.h"

/**
  * \typedef q_polynomial
  * \brief Structure of a q-polynomial
  */
typedef struct q_polynomial {
  ffi_vec values; /**< Coefficients of the q-polynomial */
  int max_degree; /**< Maximum q-degree that the q-polynomial may have. This value never changes */
  int degree; /**< Q-degree of the q-polynomial. This value is updated whenever necessary */
} q_polynomial;


q_polynomial q_polynomial_init(unsigned int max_degree);
void q_polynomial_update_degree(q_polynomial& p, unsigned int position);

int q_polynomial_set(q_polynomial& o, const q_polynomial& p);
int q_polynomial_set_zero(q_polynomial& o);
int q_polynomial_set_one(q_polynomial& o);
int q_polynomial_set_interpolate_vect_and_zero(q_polynomial& o1, q_polynomial& o2, const ffi_vec& v1, const ffi_vec& v2, int size);

int q_polynomial_is_zero(const q_polynomial& p);
void q_polynomial_evaluate(ffi_elt& o, const q_polynomial& p, const ffi_elt& e);

int q_polynomial_scalar_mul(q_polynomial& o, const q_polynomial& p, const ffi_elt& e);
int q_polynomial_qexp(q_polynomial& o, const q_polynomial& p);

int q_polynomial_add(q_polynomial& o, const q_polynomial& p, const q_polynomial& q);  
int q_polynomial_mul(q_polynomial& o, const q_polynomial& p, const q_polynomial& q);  
int q_polynomial_left_div(q_polynomial& q, q_polynomial& r, const q_polynomial& a, const q_polynomial& b);

void q_polynomial_print(const q_polynomial& p);

#endif

