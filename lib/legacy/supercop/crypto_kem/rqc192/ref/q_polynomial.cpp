/** 
 * \file q_polynomial.c
 * \brief Implementation of q_polynomial.h
 *
 * The q_polynomials are polynomials over a field of the form \f$ P(x) = \sum_{i=0}^{n} p_i \times x^{q^i} \f$ with \f$ q \f$ a rational prime.
 * See \cite ore:qpolynomials for a description of their main properties.
 *
 */

#include "ffi_elt.h"
#include "ffi_field.h"
#include "ffi_vec.h"
#include "q_polynomial.h"

#define INVALID_PARAMETERS -1; /**< Return status indicating that invalid parameters have been used in input */
#define SUCCESS 0; /**< Return status indicating that computation ended normally */


/** 
 * \fn q_polynomial q_polynomial_init(unsigned int max_degree)
 * \brief This function initializes a q_polynomial
 *
 * \param[in] max_degree The maximum degree that the q_polynomial may have
 * \return q_polynomial
 */
q_polynomial q_polynomial_init(unsigned int max_degree) {
  q_polynomial p;

  ffi_vec_set_length(p.values, max_degree);
  p.max_degree = max_degree;
  p.degree = -1;

  return p;
}



/** 
 * \fn void q_polynomial_update_degree(q_polynomial& p, unsigned int position)
 * \brief This function updates the degree of a q_polynomial starting from a given position
 *
 * \param[out] p q_polynomial
 * \param[in] position Position to start from
 */
void q_polynomial_update_degree(q_polynomial& p, unsigned int position) {
  for(int i = position ; i >= 0 ; --i) {
    if(ffi_elt_is_zero(p.values[i]) == 0) { 
      p.degree = i;
      return;
    }
  }

  p.degree = -1;
}



/** 
 * \fn int q_polynomial_set(q_polynomial& o, const q_polynomial& p)
 * \brief This function copies a q_polynomial into another one
 *
 * \param[in] p q_polynomial
 * \param[out] o q_polynomial
 * \return INVALID_PARAMETERS if <b>o.max_degree < p.degree</b>, SUCCESS otherwise
 */
int q_polynomial_set(q_polynomial& o, const q_polynomial& p) {
  if(o.max_degree < p.degree) {
    return INVALID_PARAMETERS;
  }

  if(q_polynomial_is_zero(p) == 1) {
    q_polynomial_set_zero(o);
  } else {
    o.degree = p.degree;

    for(int i = 0 ; i <= p.degree ; ++i) {
      ffi_vec_set_coeff(o.values, p.values[i], i);
    }

    for(int i = p.degree + 1 ; i <= o.max_degree ; ++i) {
      ffi_vec_set_coeff(o.values, ffi_elt_get_zero(), i);
    }
  }

  return SUCCESS;
}




/** 
 * \fn int q_polynomial_set_zero(q_polynomial& o)
 * \brief This function sets a q_polynomial to zero
 *
 * \param[out] o q_polynomial
 */
int q_polynomial_set_zero(q_polynomial& o) {
  o.degree = -1;
  ffi_vec_set_zero(o.values, o.max_degree + 1);

  return SUCCESS;
}



/** 
 * \fn int q_polynomial_set_one(q_polynomial& o)
 * \brief This function sets a q_polynomial to one
 *
 * \param[out] o q_polynomial
 */
int q_polynomial_set_one(q_polynomial& o) {
  o.degree = 0;

  ffi_vec_set_coeff(o.values, ffi_elt_get_one(), 0);
  for(int i = 1 ; i <= o.max_degree ; ++i) {
    ffi_vec_set_coeff(o.values, ffi_elt_get_zero(), i);
  }

  return SUCCESS;
}



/**
 * \fn int q_polynomial_set_interpolate_vect_and_zero(q_polynomial& o1, q_polynomial& o2, const ffi_vec& v1, const ffi_vec& v2, int size);
 * \brief This function computes interleaved interpolations such that \f$ o1(v1_i) = 0 \f$ and \f$ o2(v1_i) = v2_i \f$ for \f$ 0 \leq i \leq size - 1 \f$
 *
 * This function assumes <b>FIELD_Q</b> = 2.
 *
 * \param[out] o1 q_polynomial such that \f$ o1(v1_i) = 0 \f$ with \f$ 0 \leq i \leq size - 1 \f$
 * \param[out] o2 q_polynomial such that \f$ o2(v1_i) = v2_i \f$ with \f$ 0 \leq i \leq size - 1 \f$
 * \param[in] v1 Vector over a finite field
 * \param[in] v2 Vector over a finite field
 * \param[in] size Size of the vectors
 * \return INVALID_PARAMETERS if <b>o1.max_degree < size</b> or <b>o2.max_degree < size - 1 </b>, SUCCESS otherwise
 */
int q_polynomial_set_interpolate_vect_and_zero(q_polynomial& o1, q_polynomial& o2, const ffi_vec& v1, const ffi_vec& v2, int size) {
  if(o1.max_degree < size || o2.max_degree < size - 1) {
    return INVALID_PARAMETERS;
  }

  ffi_elt tmp1, tmp2, tmp3;
  q_polynomial qtmp = q_polynomial_init(size);

  q_polynomial_set_one(o1);
  q_polynomial_set_zero(o2);
  q_polynomial_set_zero(qtmp);

  for(unsigned int i = 0 ; i < size ; ++i)  {
    q_polynomial_evaluate(tmp1, o1, v1[i]);
    q_polynomial_evaluate(tmp2, o2, v1[i]);

    ffi_elt_inv(tmp3, tmp1);
    ffi_elt_add(tmp2, tmp2, v2[i]);
    ffi_elt_mul(tmp2, tmp2, tmp3);

    q_polynomial_scalar_mul(qtmp, o1, tmp2);
    q_polynomial_add(o2, qtmp, o2);

    q_polynomial_qexp(qtmp, o1);
    q_polynomial_scalar_mul(o1, o1, tmp1);
    q_polynomial_add(o1, o1, qtmp);
  }

  return SUCCESS;
}



/** 
 * \fn int q_polynomial_is_zero(const q_polynomial& p)
 * \brief This function tests if a q_polynomial is equal to zero
 *
 * \param[in] p q_polynomial
 * \return returns 1 if <b>p</b> is equal to zero, 0 otherwise
 */
int q_polynomial_is_zero(const q_polynomial& p) {
  if(p.degree == -1) {
    return 1;
  }

  return 0;
}



/** 
 * \fn void q_polynomial_evaluate(ffi_elt& o, const q_polynomial& p, const ffi_elt& e)
 * \brief This function evaluates a q_polynomial on a finite field element
 *
 * \param[out] o Finite field element equal to \f$ p(e) \f$
 * \param[in] p q_polynomial
 * \param[in] e Finite field element
 */
void q_polynomial_evaluate(ffi_elt& o, const q_polynomial& p, const ffi_elt& e) {
  if(q_polynomial_is_zero(p) == 1) {
    ffi_elt_set_zero(o);
  } 
  else {
    ffi_elt tmp1, tmp2;

    ffi_elt_set(tmp1, e);
    ffi_elt_mul(o, p.values[0], tmp1);

    for(int i = 1 ; i <= p.degree ; ++i) {
      ffi_elt_sqr(tmp1, tmp1);
      ffi_elt_mul(tmp2, p.values[i], tmp1);
      ffi_elt_add(o, o, tmp2);
    }
  }
}



/** 
 * \fn int q_polynomial_scalar_mul(q_polynomial& o, const q_polynomial& p, const ffi_elt& e)
 * \brief This function computes the scalar product between a q_polynomial and a finite field element
 *
 * \param[out] o q_polynomial equal to \f$ e \times p \f$
 * \param[in] p q_polynomial
 * \param[in] e Finite field element
 * \return INVALID_PARAMETERS if <b>o.max_degree < p.degree</b>, SUCESS otherwise
 */
int q_polynomial_scalar_mul(q_polynomial& o, const q_polynomial& p, const ffi_elt& e) {
  if(o.max_degree < p.degree) {
    return INVALID_PARAMETERS;
  }

  if(q_polynomial_is_zero(p) == 1 || ffi_elt_is_zero(e) == 1) {
    q_polynomial_set_zero(o);
  } else {

    ffi_elt tmp;
    for(int i = 0 ; i <= p.degree ; ++i) {
      ffi_elt_mul(tmp, p.values[i], e);
      ffi_vec_set_coeff(o.values, tmp, i);
    }

    for(int i = p.degree + 1 ; i <= o.max_degree ; ++i) {
      ffi_vec_set_coeff(o.values, ffi_elt_get_zero(), i);
    }

    o.degree = p.degree;
  }

  return SUCCESS;
}



/** 
 * \fn int q_polynomial_qexp(q_polynomial& o, const q_polynomial& p)
 * \brief This function computes the qth power of a q_polynomial
 *
 * This function assumes <b>FIELD_Q</b> = 2.
 *
 * \param[out] o q_polynomial equal to \f$ p^q \f$ where <b>q</b> is the characteristic of the field
 * \param[in] p q_polynomial
 * \return INVALID_PARAMETERS if <b>o.max_degree < p.degree + 1</b>, SUCCESS otherwise
 */
int q_polynomial_qexp(q_polynomial& o, const q_polynomial& p) {
  if(o.max_degree < p.degree + 1) {
    return INVALID_PARAMETERS;
  }

  if(q_polynomial_is_zero(p) == 1) {
    q_polynomial_set_zero(o);
  } 
  else {
    ffi_elt tmp;
    ffi_vec_set_coeff(o.values, ffi_elt_get_zero(), 0);
    for(int i = 0 ; i <= p.degree ; ++i) {
      ffi_elt_sqr(tmp, p.values[i]);
      ffi_vec_set_coeff(o.values, tmp, (i+1) % ffi_field_get_degree());
    }

    for(int i = p.degree + 2 ; i <= o.max_degree ; ++i) {
      ffi_vec_set_coeff(o.values, ffi_elt_get_zero(), i);
    }

    q_polynomial_update_degree(o, p.degree + 1);
  }

  return SUCCESS;
}



/** 
 * \fn int q_polynomial_add(q_polynomial& o, const q_polynomial& p, const q_polynomial& q)
 * \brief This function adds two q_polynomials
 *
 * \param[out] o Sum of q_polynomials <b>p</b> and <b>q</b>
 * \param[in] p q_polynomial
 * \param[in] q q_polynomial
 * \return INVALID_PARAMETERS if <b>o.max_degree < max(p.degree, q.degree)</b>, SUCCESS otherwise
 */
int q_polynomial_add(q_polynomial& o, const q_polynomial& p, const q_polynomial& q) {
  int degree = p.degree > q.degree ? p.degree : q.degree; 
  if(o.max_degree < degree) {
    return INVALID_PARAMETERS;
  }

  if(degree == -1) {
    q_polynomial_set_zero(o);
  } 
  else {
    o.degree = degree;

    ffi_elt tmp;
    for(int i = 0 ; i <= o.degree ; ++i) {
      ffi_elt_add(tmp, ffi_vec_get_coeff(p.values, i), ffi_vec_get_coeff(q.values, i));
      ffi_vec_set_coeff(o.values, tmp, i); 
    }

    for(int i = o.degree + 1 ; i <= o.max_degree ; ++i) {
      ffi_vec_set_coeff(o.values, ffi_elt_get_zero(), i);
    }

    q_polynomial_update_degree(o, o.degree);
  }

  return SUCCESS;
}



/** 
 * \fn int q_polynomial_mul(q_polynomial& o, const q_polynomial& p, const q_polynomial& q)
 * \brief This function computes the multiplication of two q_polynomials
 *
 * Multiplication of two q_polynomials is non commutative and is defined using composition namely \f$ p \times q = p \circ q \f$.
 *
 * \param[out] o q_polynomial equal to \f$ p \circ q \f$
 * \param[in] p q_polynomial
 * \param[in] q q_polynomial
 * \return INVALID_PARAMETERS if <b>o.max_degree < p.degree + q.degree</b>, SUCCESS otherwise
 */
int q_polynomial_mul(q_polynomial& o, const q_polynomial& p, const q_polynomial& q) {
  if(o.max_degree < p.degree + q.degree) {
    return INVALID_PARAMETERS;
  }

  q_polynomial_set_zero(o);
  if(q_polynomial_is_zero(p) == 1 || q_polynomial_is_zero(q) == 1) {
    return SUCCESS;
  } else {
    ffi_elt tmp1, tmp2, tmp3;

    for(int j = 0 ; j <= q.degree ; ++j) {
      ffi_elt_set(tmp1, q.values[j]);
      ffi_elt_mul(tmp2, p.values[0], tmp1);
      ffi_elt_add(tmp3, o.values[j], tmp2);
      ffi_vec_set_coeff(o.values, tmp3, j);
      for(int i = 1 ; i <= p.degree ; ++i) {
        ffi_elt_sqr(tmp1, tmp1);
        ffi_elt_mul(tmp2, p.values[i], tmp1);
        ffi_elt_add(tmp3, ffi_vec_get_coeff(o.values, (i+j) % ffi_field_get_degree()), tmp2); 
        ffi_vec_set_coeff(o.values, tmp3, (i+j) % ffi_field_get_degree());
      }
    }

    q_polynomial_update_degree(o, p.degree + q.degree);

    return SUCCESS;
  }
}



/** 
 * \fn int q_polynomial_left_div(q_polynomial& q, q_polynomial& r, const q_polynomial& a, const q_polynomial& b)
 * \brief This function computes the left division of two q_polynomials using euclidean algorithm: \f$ a = b \times q + r \f$
 *
 * \param[out] q q_polynomial equal to the quotient of the division
 * \param[out] r q_polynomial equal to the remainder of the division
 * \param[in] a q_polynomial to divide
 * \param[in] b divisor q_polynomial
 * \return INVALID_PARAMETERS if <b>q.max_degree < a.degree - b.degree</b> or <b>r.max_degree < b.degree - 1</b> or <b>b = 0</b>, SUCCESS otherwise
 */
int q_polynomial_left_div(q_polynomial& q, q_polynomial& r, const q_polynomial& a, const q_polynomial& b) {
  if(q.max_degree < a.degree - b.degree || r.max_degree < b.degree - 1 || q_polynomial_is_zero(b)) {
    return INVALID_PARAMETERS;
  }

  if(a.degree < b.degree) {
    q_polynomial_set_zero(q);
    q_polynomial_set(r, a);
    return SUCCESS;
  }

  q_polynomial rtmp = q_polynomial_init(a.degree);
  q_polynomial s = q_polynomial_init(a.degree - b.degree);
  q_polynomial t = q_polynomial_init(a.max_degree);

  q_polynomial_set_zero(q);
  q_polynomial_set(rtmp, a);

  ffi_elt b_lead_coeff_inv;
  ffi_elt r_lead_coeff;
  ffi_elt s_lead_coeff;
  
  ffi_elt_set(b_lead_coeff_inv, b.values[b.degree]);
  ffi_elt_inv(b_lead_coeff_inv, b_lead_coeff_inv);

  while(rtmp.degree >= b.degree) {
    // Compute monial s
    ffi_elt_set(r_lead_coeff, rtmp.values[rtmp.degree]);
    ffi_elt_mul(s_lead_coeff, r_lead_coeff, b_lead_coeff_inv);
    ffi_elt_nth_root(s_lead_coeff, s_lead_coeff, b.degree);

    q_polynomial_set_zero(s);
    ffi_vec_set_coeff(s.values, s_lead_coeff, rtmp.degree - b.degree);
    s.degree = rtmp.degree - b.degree;

    // Update quotient q_polynomial
    q_polynomial_add(q, q, s);

    // Update remainder q_polynomial
    q_polynomial_mul(t, b, s);
    q_polynomial_add(rtmp, rtmp, t);
  }

  q_polynomial_set(r, rtmp);

  return SUCCESS;
}



/** 
 * \fn void q_polynomial_print(const q_polynomial& p)
 * \brief This function prints a q_polynomial
 *
 * \param p q_polynomial
 */
void q_polynomial_print(const q_polynomial& p) {
  if(q_polynomial_is_zero(p) == 1) {
    printf("0");
  } 
  else {
    for(int i = p.degree; i > 0 ; --i) {
      ffi_elt_print(p.values[i]);
      printf("x^[%i] + ", i);
    }
    ffi_elt_print(p.values[0]);
    printf("x^[0]");
  }
  printf("\n");
}

