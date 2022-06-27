/** 
 * \file rbc_qpoly.c
 * \brief Implementation of qpoly.h
 *
 * The rbc_qpolys are polynomials over a field of the form \f$ P(x) = \sum_{i=0}^{n} p_i \times x^{q^i} \f$ with \f$ q \f$ a rational prime.
 * See \cite ore:qpolynomials for a description of their main properties.
 *
 */

#include "rbc_vec.h"
#include "qpoly.h"

#define SUCCESS 0; /**< Return status indicating that computation ended normally */
#define INVALID_PARAMETERS 1; /**< Return status indicating that invalid inputs have been used */


/** 
 * \fn void rbc_qpoly_init(rbc_qpoly* p, int32_t max_degree)
 * \brief This function initializes a q_polynomial.
 *
 * \param[in] p q_polynomial to init
 * \param[in] max_degree The maximum degree of the q_polynomial
 */
void rbc_qpoly_init(rbc_qpoly* p, int32_t max_degree) {
  *p = malloc(sizeof(rbc_qpoly_struct));

  rbc_vec_init(&((*p)->values), max_degree + 1);
  (*p)->max_degree = max_degree;
  (*p)->degree = -1;
}



/** 
 * \fn void rbc_qpoly_clear(rbc_poly o)
 * \brief This function clears a q_polynomial.
 *
 * \param[in] p q_polynomial to clear
 */
void rbc_qpoly_clear(rbc_qpoly o) {
  rbc_vec_clear(o->values);
  free(o);
}



/** 
 * \fn void rbc_qpoly_update_degree(rbc_qpoly p, int32_t position)
 * \brief This function updates the degree of a rbc_qpoly starting from a given position.
 *
 * \param[out] p q_polynomial
 * \param[in] position Position to start from
 */
void rbc_qpoly_update_degree(rbc_qpoly p, int32_t position) {
  for(int32_t i = position ; i >= 0 ; --i) {
    if(rbc_elt_is_zero(p->values[i]) == 0) { 
      p->degree = i;
      return;
    }
  }

  p->degree = -1;
}



/** 
 * \fn int8_t rbc_qpoly_set(rbc_qpoly o, const rbc_qpoly p)
 * \brief This function copies a q_polynomial into another one.
 *
 * \param[out] o q_polynomial
 * \param[in] p q_polynomial
 * \return INVALID_PARAMETERS if <b>o.max_degree < p.degree</b>, SUCCESS otherwise
 */
int8_t rbc_qpoly_set(rbc_qpoly o, const rbc_qpoly p) {
  if(o->max_degree < p->degree) {
    return INVALID_PARAMETERS;
  }

  o->degree = p->degree;

  for(int32_t i = 0 ; i <= p->degree ; ++i) {
    rbc_elt_set(o->values[i], p->values[i]);
  }

  for(int32_t i = p->degree + 1 ; i <= o->max_degree ; ++i) {
    rbc_elt_set_zero(o->values[i]);
  }

  return SUCCESS;
}



/** 
 * \fn int8_t rbc_qpoly_set_mask(rbc_qpoly o, const rbc_qpoly p1, const rbc_qpoly p2, uint32_t mask)
 * \brief This function copies either q_polynomial p1 or p2 into q_polynomial o depending on the mask value.
 *
 * \param[out] o q_polynomial
 * \param[in] p1 q_polynomial
 * \param[in] p2 q_polynomial
 * \return INVALID_PARAMETERS if <b>o.max_degree < p1.degree || o.max_degree < p2.degree</b>, SUCCESS otherwise
 */
int8_t rbc_qpoly_set_mask(rbc_qpoly o, const rbc_qpoly p1, const rbc_qpoly p2, uint32_t mask) {
  if(o->max_degree < p1->degree || o->max_degree < p2->degree) {
    return INVALID_PARAMETERS;
  }

  o->degree = mask * p1->degree + (1 - mask) * p2->degree;

  for(int32_t i = 0 ; i <= o->degree ; ++i) {
    rbc_elt_set_mask1(o->values[i], p1->values[i], p2->values[i], mask);
  }

  for(int32_t i = o->degree + 1 ; i <= o->max_degree ; ++i) {
    rbc_elt_set_zero(o->values[i]);
  }

  return SUCCESS;
}



/** 
 * \fn int8_t rbc_qpoly_set_zero(rbc_qpoly o)
 * \brief This function sets a q_polynomial to zero.
 *
 * \param[out] o q_polynomial
 */
int8_t rbc_qpoly_set_zero(rbc_qpoly o) {
  o->degree = -1;
  rbc_vec_set_zero(o->values, o->max_degree + 1);

  return SUCCESS;
}



/** 
 * \fn int8_t rbc_qpoly_set_one(rbc_qpoly o)
 * \brief This function sets a q_polynomial to one.
 *
 * \param[out] o q_polynomial
 */
int8_t rbc_qpoly_set_one(rbc_qpoly o) {
  o->degree = 0;
  rbc_elt_set_one(o->values[0]);

  for(int32_t i = 1 ; i < o->max_degree + 1 ; ++i) {
    rbc_elt_set_zero(o->values[i]);
  }

  return SUCCESS;
}



/**
 * \fn int8_t rbc_qpoly_set_interpolate_vect_and_zero(rbc_qpoly o1, rbc_qpoly o2, const rbc_vec& p1, const rbc_vec& p2, int32_t size);
 * \brief This function computes interleaved interpolations such that \f$ o1(p1_i) = 0 \f$ and \f$ o2(p1_i) = p2_i \f$ for \f$ 0 \leq i \leq size - 1 \f$.
 *
 * \param[out] o1 rbc_qpoly such that \f$ o1(p1_i) = 0 \f$ with \f$ 0 \leq i \leq size - 1 \f$
 * \param[out] o2 rbc_qpoly such that \f$ o2(p1_i) = p2_i \f$ with \f$ 0 \leq i \leq size - 1 \f$
 * \param[in] p1 Vector over a finite field
 * \param[in] p2 Vector over a finite field
 * \param[in] size Size of the vectors
 * \return INVALID_PARAMETERS if <b>o1.max_degree < size</b> or <b>o2.max_degree < size - 1 </b>, SUCCESS otherwise
 */
int8_t rbc_qpoly_set_interpolate_vect_and_zero(rbc_qpoly o1, rbc_qpoly o2, const rbc_poly p1, const rbc_poly p2, int32_t size) {
  if(o1->max_degree < size || o2->max_degree < size - 1) {
    return INVALID_PARAMETERS;
  }

  rbc_elt tmp1, tmp2, tmp3;

  rbc_qpoly qtmp;
  rbc_qpoly_init(&qtmp, size);

  rbc_qpoly_set_one(o1);
  rbc_qpoly_set_zero(o2);
  rbc_qpoly_set_zero(qtmp);

  for(int32_t i = 0 ; i < size ; ++i)  {
    rbc_qpoly_evaluate(tmp1, o1, p1->v[i]);
    rbc_qpoly_evaluate(tmp2, o2, p1->v[i]);

    rbc_elt_inv(tmp3, tmp1);
    rbc_elt_add(tmp2, tmp2, p2->v[i]);
    rbc_elt_mul(tmp2, tmp2, tmp3);

    rbc_qpoly_scalar_mul(qtmp, o1, tmp2);
    rbc_qpoly_add(o2, qtmp, o2);

    rbc_qpoly_qexp(qtmp, o1);
    rbc_qpoly_scalar_mul(o1, o1, tmp1);
    rbc_qpoly_add(o1, o1, qtmp);
  }

  rbc_qpoly_clear(qtmp);

  return SUCCESS;
}



/** 
 * \fn int8_t rbc_qpoly_is_zero(const rbc_qpoly p)
 * \brief This function tests if a q_polynomial is equal to zero.
 *
 * \param[in] p q_polynomial
 * \return returns 1 if <b>p</b> is equal to zero, 0 otherwise
 */
int8_t rbc_qpoly_is_zero(const rbc_qpoly p) {
  if(p->degree == -1) {
    return 1;
  }

  return 0;
}



/** 
 * \fn void rbc_qpoly_evaluate(rbc_elt o, const rbc_qpoly p, const rbc_elt e)
 * \brief This function evaluates a q_polynomial on a finite field element.
 *
 * \param[out] o Finite field element equal to \f$ p(e) \f$
 * \param[in] p q_polynomial
 * \param[in] e Finite field element
 */
void rbc_qpoly_evaluate(rbc_elt o, const rbc_qpoly p, const rbc_elt e) {
  rbc_elt tmp1, tmp2;

  rbc_elt_set(tmp1, e);
  rbc_elt_mul(o, p->values[0], tmp1);

  for(int32_t i = 1 ; i <= p->degree ; ++i) {
    rbc_elt_sqr(tmp1, tmp1);
    rbc_elt_mul(tmp2, p->values[i], tmp1);
    rbc_elt_add(o, o, tmp2);
  }
}



/** 
 * \fn int8_t rbc_qpoly_scalar_mul(rbc_qpoly o, const rbc_qpoly p, const rbc_elt e)
 * \brief This function computes the scalar product between a q_polynomial and a finite field element.
 *
 * \param[out] o q_polynomial equal to \f$ e \times p \f$
 * \param[in] p q_polynomial
 * \param[in] e Finite field element
 * \return INVALID_PARAMETERS if <b>o.max_degree < p.degree</b>, SUCESS otherwise
 */
int8_t rbc_qpoly_scalar_mul(rbc_qpoly o, const rbc_qpoly p, const rbc_elt e) {
  if(o->max_degree < p->degree) {
    return INVALID_PARAMETERS;
  }

  rbc_elt tmp;
  for(int32_t i = 0 ; i <= p->degree ; ++i) {
    rbc_elt_mul(tmp, p->values[i], e);
    rbc_elt_set(o->values[i], tmp);
  }

  for(int32_t i = p->degree + 1 ; i <= o->max_degree ; ++i) {
    rbc_elt_set_zero(o->values[i]);
  }

  o->degree = p->degree;

  return SUCCESS;
}



/** 
 * \fn int8_t rbc_qpoly_qexp(rbc_qpoly o, const rbc_qpoly p)
 * \brief This function computes the qth power of a q_polynomial.
 *
 * \param[out] o q_polynomial equal to \f$ p^q \f$ where <b>q</b> is the characteristic of the field
 * \param[in] p q_polynomial
 * \return INVALID_PARAMETERS if <b>o.max_degree < p.degree + 1</b>, SUCCESS otherwise
 */
int8_t rbc_qpoly_qexp(rbc_qpoly o, const rbc_qpoly p) {
  if(o->max_degree < p->degree + 1) {
    return INVALID_PARAMETERS;
  }

  rbc_elt tmp;
  rbc_elt_set_zero(o->values[0]);
  for(int32_t i = 0 ; i <= p->degree ; ++i) {
    rbc_elt_sqr(tmp, p->values[i]);
    rbc_elt_set(o->values[(i + 1) % RBC_FIELD_M], tmp);
  }

  for(int32_t i = p->degree + 2 ; i <= o->max_degree ; ++i) {
    rbc_elt_set_zero(o->values[i]);
  }

  rbc_qpoly_update_degree(o, p->degree + 1);

  return SUCCESS;
}



/** 
 * \fn int8_t rbc_qpoly_add(rbc_qpoly o, const rbc_qpoly p1, const rbc_qpoly p2)
 * \brief This function adds two q_polynomials.
 *
 * \param[out] o Sum of q_polynomials <b>p1</b> and <b>p2</b>
 * \param[in] p1 q_polynomial
 * \param[in] p2 q_polynomial
 * \return INVALID_PARAMETERS if <b>o.max_degree < max(p1.degree, p2.degree)</b>, SUCCESS otherwise
 */
int8_t rbc_qpoly_add(rbc_qpoly o, const rbc_qpoly p1, const rbc_qpoly p2) {
  int32_t degree = p1->degree > p2->degree ? p1->degree : p2->degree; 
  if(o->max_degree < degree) {
    return INVALID_PARAMETERS;
  }

  o->degree = degree;

  for(int32_t i = 0 ; i <= o->degree ; ++i) {
    rbc_elt_add(o->values[i], p1->values[i], p2->values[i]);
  }

  for(int32_t i = o->degree + 1 ; i <= o->max_degree ; ++i) {
    rbc_elt_set_zero(o->values[i]);
  }

  rbc_qpoly_update_degree(o, o->degree);

  return SUCCESS;
}



/** 
 * \fn int8_t rbc_qpoly_mul(rbc_qpoly o, const rbc_qpoly p1, const rbc_qpoly p2)
 * \brief This function computes the multiplication of two q_polynomials.
 *
 * Multiplication of two q_polynomials is non commutative and is defined using composition namely \f$ p \times q = p \circ q \f$.
 *
 * \param[out] o q_poynomial equal to \f$ p1 \circ p2 \f$
 * \param[in] p1 q_polynomial
 * \param[in] p2 q_polynomial
 * \return INVALID_PARAMETERS if <b>o.max_degree < p1.degree + p2.degree</b>, SUCCESS otherwise
 */
int8_t rbc_qpoly_mul(rbc_qpoly o, const rbc_qpoly p1, const rbc_qpoly p2) {
  if(o->max_degree < p1->degree + p2->degree) {
    return INVALID_PARAMETERS;
  }

  rbc_qpoly_set_zero(o);
  rbc_elt tmp1, tmp2;

  for(int32_t j = 0 ; j <= p2->degree ; ++j) {
    rbc_elt_set(tmp1, p2->values[j]);
    rbc_elt_mul(tmp2, p1->values[0], tmp1);
    rbc_elt_add(o->values[j], o->values[j], tmp2);
    for(int32_t i = 1 ; i <= p1->degree ; ++i) {
      rbc_elt_sqr(tmp1, tmp1);
      rbc_elt_mul(tmp2, p1->values[i], tmp1);
      rbc_elt_add(o->values[(i + j) % RBC_FIELD_M], o->values[(i + j) % RBC_FIELD_M], tmp2); 
    }
  }

  rbc_qpoly_update_degree(o, p1->degree + p2->degree);

  return SUCCESS;
}



/** 
 * \fn int8_t rbc_qpoly_mul2(rbc_qpoly o, const rbc_qpoly p1, const rbc_qpoly p2, uint32_t p1_degree, uint32_r p2_degree)
 * \brief This function computes the multiplication of two q_polynomials.
 *
 * Multiplication of two q_polynomials is non commutative and is defined using composition namely \f$ p \times q = p \circ q \f$.
 * Execution time of this function depends on p1_degree and p2_degree rather than p1->degree and p2->degree as in rbc_qpoly_mul.
 *
 * \param[out] o q_poynomial equal to \f$ p1 \circ p2 \f$
 * \param[in] p1 q_polynomial
 * \param[in] p2 q_polynomial
 * \param[in] p1_degree
 * \param[in] p2_degree
 * \return INVALID_PARAMETERS if <b>o.max_degree < p1.degree + p2.degree</b>, SUCCESS otherwise
 */
int8_t rbc_qpoly_mul2(rbc_qpoly o, const rbc_qpoly p1, const rbc_qpoly p2, uint32_t p1_degree, uint32_t p2_degree) {
  if(o->max_degree < p1->degree + p2->degree) {
    return INVALID_PARAMETERS;
  }

  rbc_qpoly_set_zero(o);
  rbc_elt tmp1, tmp2;

  for(uint32_t j = 0 ; j <= p2_degree ; ++j) {
    rbc_elt_set(tmp1, p2->values[j]);
    rbc_elt_mul(tmp2, p1->values[0], tmp1);
    rbc_elt_add(o->values[j], o->values[j], tmp2);
    for(uint32_t i = 1 ; i <= p1_degree ; ++i) {
      rbc_elt_sqr(tmp1, tmp1);
      rbc_elt_mul(tmp2, p1->values[i], tmp1);
      rbc_elt_add(o->values[(i + j) % RBC_FIELD_M], o->values[(i + j) % RBC_FIELD_M], tmp2); 
    }
  }

  rbc_qpoly_update_degree(o, p1->degree + p2->degree);

  return SUCCESS;
}




/** 
 * \fn int8_t rbc_qpoly_left_div(rbc_qpoly q, rbc_qpoly r, const rbc_qpoly a, const rbc_qpoly b)
 * \brief This function computes the left division of two q_polynomials using euclidean algorithm: \f$ a = b \times q + r \f$.
 *
 * \param[out] q q_polynomial equal to the quotient of the division
 * \param[out] r q_polynomial equal to the remainder of the division
 * \param[in] a q_polynomial to divide
 * \param[in] b divisor q_polynomial
 * \return INVALID_PARAMETERS if <b>q.max_degree < a.degree - b.degree</b> or <b>r.max_degree < b.degree - 1</b> or <b>b = 0</b>, SUCCESS otherwise
 */
int8_t rbc_qpoly_left_div(rbc_qpoly q, rbc_qpoly r, const rbc_qpoly a, const rbc_qpoly b) {
  if(q->max_degree < a->degree - b->degree || r->max_degree < b->degree - 1 || rbc_qpoly_is_zero(b)) {
    return INVALID_PARAMETERS;
  }

  rbc_qpoly rtmp;
  rbc_qpoly s;
  rbc_qpoly t;

  rbc_qpoly_init(&rtmp, a->degree);
  rbc_qpoly_init(&s, a->degree - b->degree);
  rbc_qpoly_init(&t, a->max_degree);

  rbc_qpoly_set_zero(q);
  rbc_qpoly_set(rtmp, a);

  rbc_elt b_lead_coeff_inv;
  rbc_elt r_lead_coeff;
  rbc_elt s_lead_coeff;

  rbc_elt_set(b_lead_coeff_inv, b->values[b->degree]);
  rbc_elt_inv(b_lead_coeff_inv, b_lead_coeff_inv);

  while(rtmp->degree >= b->degree) {

    // Compute monial s
    rbc_elt_set(r_lead_coeff, rtmp->values[rtmp->degree]);
    rbc_elt_mul(s_lead_coeff, r_lead_coeff, b_lead_coeff_inv);
    rbc_elt_nth_root(s_lead_coeff, s_lead_coeff, b->degree);

    rbc_qpoly_set_zero(s);
    rbc_elt_set(s->values[rtmp->degree - b->degree], s_lead_coeff);
    s->degree = rtmp->degree - b->degree;

    // Update quotient rbc_qpoly
    rbc_qpoly_add(q, q, s);

    // Update remainder rbc_qpoly
    rbc_qpoly_mul(t, b, s);
    rbc_qpoly_add(rtmp, rtmp, t);

  }

  rbc_qpoly_set(r, rtmp);

  rbc_qpoly_clear(rtmp);
  rbc_qpoly_clear(s);
  rbc_qpoly_clear(t);


  return SUCCESS;
}



/** 
 * \fn int8_t rbc_qpoly_left_div(rbc_qpoly q, rbc_qpoly r, const rbc_qpoly a, const rbc_qpoly b, uint32_t capacity, uint32_t k)
 * \brief This function computes the left division of two q_polynomials using euclidean algorithm: \f$ a = b \times q + r \f$.
 *
 * This function is a variant of rbc_qpoly_left_div that uses rbc_poly_mul2
 *
 * \param[out] q q_polynomial equal to the quotient of the division
 * \param[out] r q_polynomial equal to the remainder of the division
 * \param[in] a q_polynomial to divide
 * \param[in] b divisor q_polynomial
 * \param[in] b_degree
 * \param[in] k
 * \return INVALID_PARAMETERS if <b>q.max_degree < a.degree - b.degree</b> or <b>r.max_degree < b.degree - 1</b> or <b>b = 0</b>, SUCCESS otherwise
 */
int8_t rbc_qpoly_left_div2(rbc_qpoly q, rbc_qpoly r, const rbc_qpoly a, const rbc_qpoly b, uint32_t capacity, uint32_t k) {
  if(q->max_degree < a->degree - b->degree || r->max_degree < b->degree - 1 || rbc_qpoly_is_zero(b)) {
    return INVALID_PARAMETERS;
  }

  rbc_qpoly rtmp;
  rbc_qpoly s;
  rbc_qpoly t;

  rbc_qpoly_init(&rtmp, a->degree);
  rbc_qpoly_init(&s, a->degree - b->degree);
  rbc_qpoly_init(&t, a->max_degree);

  rbc_qpoly_set_zero(q);
  rbc_qpoly_set(rtmp, a);

  rbc_elt b_lead_coeff_inv;
  rbc_elt r_lead_coeff;
  rbc_elt s_lead_coeff;

  rbc_elt_set(b_lead_coeff_inv, b->values[b->degree]);
  rbc_elt_inv(b_lead_coeff_inv, b_lead_coeff_inv);

  int i = k - 1;
  while(rtmp->degree >= b->degree) {

    // Compute monial s
    rbc_elt_set(r_lead_coeff, rtmp->values[rtmp->degree]);
    rbc_elt_mul(s_lead_coeff, r_lead_coeff, b_lead_coeff_inv);
    rbc_elt_nth_root(s_lead_coeff, s_lead_coeff, b->degree);

    rbc_qpoly_set_zero(s);
    rbc_elt_set(s->values[rtmp->degree - b->degree], s_lead_coeff);
    s->degree = rtmp->degree - b->degree;

    // Update quotient rbc_qpoly
    rbc_qpoly_add(q, q, s);

    // Update remainder rbc_qpoly
    rbc_qpoly_mul2(t, b, s, capacity, i);
    rbc_qpoly_add(rtmp, rtmp, t);

    i--;
  }

  rbc_qpoly_set(r, rtmp);

  rbc_qpoly_clear(rtmp);
  rbc_qpoly_clear(s);
  rbc_qpoly_clear(t);


  return SUCCESS;
}



/** 
 * \fn void rbc_qpoly_print(const rbc_qpoly p)
 * \brief This function prints a q_polynomial.
 *
 * \param p q_polynomial
 */
void rbc_qpoly_print(const rbc_qpoly p) {
  if(rbc_qpoly_is_zero(p) == 1) {
    printf("0");
  } 
  else {
    for(int32_t i = p->degree; i > 0 ; --i) {
      rbc_elt_print(p->values[i]);
      printf("x^[%i] + ", i);
    }
    rbc_elt_print(p->values[0]);
    printf(" x^[0]");
  }
  printf("\n");
}

