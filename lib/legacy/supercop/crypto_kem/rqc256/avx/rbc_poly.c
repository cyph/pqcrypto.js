/**
 * \file rbc_poly.c
 * \brief Implementation of rbc_poly.h
 */

#include "rbc.h"
#include "rbc_vec.h"
#include "rbc_poly.h"


/**
 * \fn void rbc_poly_init(rbc_poly* p, int32_t degree)
 * \brief This function allocates the memory for a rbc_poly element.
 *
 * \param[out] p Pointer to the allocated rbc_poly
 * \param[in] degree Degree of the polynomial
 */
void rbc_poly_init(rbc_poly* p, int32_t degree) {
  *p = malloc(sizeof(rbc_poly_struct));
  if(p == NULL) exit(EXIT_FAILURE);
  rbc_vec_init(&((*p)->v), degree + 1);
  (*p)->max_degree = degree;
  (*p)->degree = degree;
}



/**
 * \fn void rbc_poly_resize(rbc_poly p, int32_t degree)
 * \brief This function changes the allocated size of a rbc_poly.
 *
 * \param p Polynomial
 * \param[in] degree New degree for <b>p</b>
 */
void rbc_poly_resize(rbc_poly p, int32_t degree) {
  rbc_vec tmp;
  rbc_vec_init(&tmp, degree + 1);
  rbc_vec_set(tmp, p->v, min(p->degree + 1, degree + 1));
  rbc_vec_clear(p->v);
  p->max_degree = degree;
  p->degree = degree;
  p->v = tmp;
}



/**
 * \fn void rbc_poly_clear(rbc_poly p)
 * \brief This function clears the memory allocated for a rbc_poly element.
 *
 * \param p Polynomial
 */
void rbc_poly_clear(rbc_poly p) {
  rbc_vec_clear(p->v);
  free(p);
  p = NULL;
}



/**
 * \fn void rbc_poly_sparse_init(rbc_poly_sparse* p, uint32_t coeff_nb, uint32_t *coeffs)
 * \brief This function allocates the memory for a rbc_poly_sparse element.
 *
 * \param p Pointer to the allocated rbc_poly_sparse
 * \param[in] coeff_nb Number of coefficients
 * \param[in] coeffs Coefficients
 */
void rbc_poly_sparse_init(rbc_poly_sparse* p, uint32_t coeff_nb, uint32_t *coeffs) {
  *p = malloc(sizeof(rbc_poly_sparse_struct));
  if(p == NULL) exit(EXIT_FAILURE);
  (*p)->coeffs_nb = coeff_nb;
  (*p)->coeffs = malloc(coeff_nb * sizeof(uint32_t));
  if((*p)->coeffs == NULL) exit(EXIT_FAILURE);
  memcpy((*p)->coeffs, coeffs, coeff_nb * sizeof(uint32_t));
}



/**
 * \fn void rbc_poly_sparse_clear(rbc_poly_sparse p)
 * \brief This function clears the memory allocated for a rbc_poly_sparse element.
 *
 * \param p Polynomial
 */
void rbc_poly_sparse_clear(rbc_poly_sparse p) {
  free(p->coeffs);
  free(p);
}



/**
 * \fn void rbc_poly_set_zero(rbc_poly o, int32_t degree)
 * \brief This functions sets a polynomial to zero.
 *
 * \param[in] p Polynomial
 * \param[in] degree Degree of the polynomial
 */
void rbc_poly_set_zero(rbc_poly o, int32_t degree) {
  if(o->max_degree < degree) {
    rbc_poly_resize(o, degree);
  }

  o->degree = degree;
  rbc_vec_set_zero(o->v, o->degree + 1);
}



/**
 * \fn void rbc_poly_set(rbc_poly o, const rbc_poly p)
 * \brief This function copies a polynomial into another one.
 *
 * \param[in] o Polynomial
 * \param[in] p Polynomial
 */
void rbc_poly_set(rbc_poly o, const rbc_poly p) {
  if(o->max_degree < p->degree) {
    rbc_poly_resize(o, p->degree);
  }

  o->degree = p->degree;
  rbc_vec_set(o->v, p->v, p->degree + 1);
}



/**
 * \fn void rbc_poly_set_random(AES_XOF_struct* ctx, rbc_poly o, int32_t degree)
 * \brief This function sets a polynomial with random values using NIST seed expander.
 *
 * \param[out] ctx NIST seed expander
 * \param[out] o Polynomial
 * \param[in] degree Degree of the polynomial
 */
void rbc_poly_set_random(AES_XOF_struct* ctx, rbc_poly o, int32_t degree) {
  if(o->max_degree < degree) {
    rbc_poly_resize(o, degree);
  }

  o->degree = degree;
  rbc_vec_set_random(ctx, o->v, o->degree + 1);
}



/**
 * \fn void rbc_poly_set_random2(rbc_poly o, int32_t degree)
 * \brief This function sets a polynomial with random values using randombytes.
 *
 * \param[out] o Polynomial
 * \param[in] degree Degree of the polynomial
 */
void rbc_poly_set_random2(rbc_poly o, int32_t degree) {
  if(o->max_degree < degree) {
    rbc_poly_resize(o, degree);
  }

  o->degree = degree;
  rbc_vec_set_random2(o->v, o->degree + 1);
}



/**
 * \fn void rbc_poly_set_random_full_rank(AES_XOF_struct* ctx, rbc_poly o, int32_t degree)
 * \brief This function sets a polynomial with random values using the NIST seed expander. The polynomial returned by this function has full rank.
 *
 * \param[out] o Polynomial
 * \param[in] degree Degree of the polynomial
 * \param[in] ctx NIST seed expander
 */
void rbc_poly_set_random_full_rank(AES_XOF_struct* ctx, rbc_poly o, int32_t degree) {
  if(o->max_degree < degree) {
    rbc_poly_resize(o, degree);
  }

  o->degree = degree;
  rbc_vec_set_random_full_rank(ctx, o->v, o->degree + 1);
}



/**
 * \fn void rbc_poly_set_random_full_rank_with_one(AES_XOF_struct* ctx, rbc_poly o, int32_t degree) {
 * \brief This function sets a polynomial with random values using the NIST seed expander. The polynomial returned by this function has full rank and contains one.
 *
 * \param[out] ctx NIST seed expander
 * \param[out] o Polynomial
 * \param[in] degree Degree of the polynomial
 */
void rbc_poly_set_random_full_rank_with_one(AES_XOF_struct* ctx, rbc_poly o, int32_t degree) {
  if(o->max_degree < degree) {
    rbc_poly_resize(o, degree);
  }

  o->degree = degree;
  rbc_vec_set_random_full_rank_with_one(ctx, o->v, o->degree + 1);
}



/**
 * \fn void rbc_poly_set_random_pair_from_support(rbc_poly o1, rbc_poly o2, int32_t degree, const rbc_vspace support, uint32_t support_size)
 * \brief This function sets a pair of polynomials with random values using NIST seedexpander. The support (o1 || o2) is the one given in input.
 *
 * \param[out] o1 Polynomial
 * \param[out] o2 Polynomial
 * \param[in] degree Degree of <b>o1</b> and <b>o2</b>
 * \param[in] support Support
 * \param[in] support_size Size of the support
 */
void rbc_poly_set_random_from_support(AES_XOF_struct* ctx, rbc_poly o, int32_t degree, const rbc_vspace support, uint32_t support_size) {
  if(o->max_degree < degree) {
    rbc_poly_resize(o, degree);
  }

  o->degree = degree;
  rbc_vec_set_random_from_support(ctx, o->v, o->degree + 1, support, support_size);
}



/**
 * \fn void rbc_poly_set_random_pair_from_support(AES_XOF_struct* ctx, rbc_poly o1, rbc_poly o2, int32_t degree, const rbc_vspace support, uint32_t support_size)
 * \brief This function sets a pair of polynomials with random values using NIST seedexpander. The support (o1 || o2) is the one given in input.
 *
 * \param[out] o1 Polynomial
 * \param[out] o2 Polynomial
 * \param[in] degree Degree of <b>o1</b> and <b>o2</b>
 * \param[in] support Support
 * \param[in] support_size Size of the support
 */
void rbc_poly_set_random_pair_from_support(AES_XOF_struct* ctx, rbc_poly o1, rbc_poly o2, int32_t degree, const rbc_vspace support, uint32_t support_size) {
  if(o1->max_degree < degree) {
    rbc_poly_resize(o1, degree);
  }

  if(o2->max_degree < degree) {
    rbc_poly_resize(o2, degree);
  }

  o1->degree = degree;
  o2->degree = degree;
  rbc_vec_set_random_pair_from_support(ctx, o1->v, o2->v, degree + 1, support, support_size);
}



/**
 * \fn void rbc_poly_set_random_pair_from_support2(rbc_poly o1, rbc_poly o2, int32_t degree, const rbc_vspace support, uint32_t support_size)
 * \brief This function sets a pair of polynomials with random values using NIST PRNG. The support (o1 || o2) is the one given in input.
 *
 * \param[out] o1 Polynomial
 * \param[out] o2 Polynomial
 * \param[in] degree Degree of <b>o1</b> and <b>o2</b>
 * \param[in] support Support
 * \param[in] support_size Size of the support
 */
void rbc_poly_set_random_pair_from_support2(rbc_poly o1, rbc_poly o2, int32_t degree, const rbc_vspace support, uint32_t support_size) {
  if(o1->max_degree < degree) {
    rbc_poly_resize(o1, degree);
  }

  if(o2->max_degree < degree) {
    rbc_poly_resize(o2, degree);
  }

  o1->degree = degree;
  o2->degree = degree;
  rbc_vec_set_random_pair_from_support2(o1->v, o2->v, degree + 1, support, support_size);
}



/**
 * \fn void rbc_poly_update_degree(rbc_poly p, int32_t max_degree) {
 * \brief This function updates the degree of a polynomial starting from a give position.
 *
 * \param[in] p Polynomial
 * \param[in] max_degree Maximum degree of the polynomial
 *
 */
void rbc_poly_update_degree(rbc_poly p, int32_t max_degree) {
  for(int32_t i = max_degree ; i >= 0 ; --i) {
    if(!rbc_elt_is_zero(p->v[i])) {
      p->degree = i;
      return;
    }
  }

  p->degree = -1;
}



/**
 * \fn uint8_t rbc_poly_is_equal_to(const rbc_poly p1, const rbc_poly p2)
 * \brief This function test if two polynomials are equal.
 *
 * \param[in] p1 Polynomial
 * \param[in] p2 Polynomial
 * \return 1 if the polynomials are equal, 0 otherwise
 */
uint8_t rbc_poly_is_equal_to(const rbc_poly p1, const rbc_poly p2) {
  if(p1->degree != p2->degree) {
    return 0;
  }

  for(int32_t i = 0 ; i < p1->degree ; ++i) {
    if(rbc_elt_is_equal_to(p1->v[i], p2->v[i]) == 0) {
      return 0;
    }
  }

  return 1;
}



/**
 * \fn void rbc_poly_add(rbc_poly o, const rbc_poly p1, const rbc_poly p2)
 * \brief This function adds two polynomials.
 *
 * \param[out] o Sum of <b>p1</b> and <b>p2</b>
 * \param[in] p1 Polynomial
 * \param[in] p2 Polynomial
 */
void rbc_poly_add(rbc_poly o, const rbc_poly p1, const rbc_poly p2) {
  int32_t max_degree = max(p1->degree, p2->degree);
  int32_t min_degree = min(p1->degree, p2->degree);

  if(o->max_degree < max_degree) {
    rbc_poly_resize(o, max_degree);
  }

  rbc_vec_add(o->v, p1->v, p2->v, min_degree + 1);

  if(p1->degree > p2->degree) {
    for(int32_t j = min_degree + 1 ; j <= max_degree ; ++j) {
      rbc_elt_set(o->v[j], p1->v[j]);
    }
  }
  else {
    for(int32_t j = min_degree + 1 ; j <= max_degree ; ++j) {
      rbc_elt_set(o->v[j], p2->v[j]);
    }
  }

  rbc_poly_update_degree(o, max_degree);
}



/**
 * \fn void rbc_poly_add2(rbc_poly o, const rbc_poly p1, const rbc_poly p2, int32_t p1_degree, int32_t p2_degree)
 * \brief This function adds two polynomials using p1_degree and p2_degree as polynomials degrees.
 *
 * \param[out] o Sum of <b>p1</b> and <b>p2</b>
 * \param[in] p1 Polynomial
 * \param[in] p2 Polynomial
 * \param[in] p1_degree Degree to consider for p1
 * \param[in] p2_degree Degree to consider for p2
 */
void rbc_poly_add2(rbc_poly o, const rbc_poly p1, const rbc_poly p2, int32_t p1_degree, int32_t p2_degree) {
  int32_t max_degree = max(p1_degree, p2_degree);
  int32_t min_degree = min(p1_degree, p2_degree);

  if(o->max_degree < max_degree) {
    rbc_poly_resize(o, max_degree);
  }

  rbc_vec_add(o->v, p1->v, p2->v, min_degree + 1);

  if(p1_degree > p2_degree) {
    for(int32_t j = min_degree + 1 ; j <= max_degree ; ++j) {
      rbc_elt_set(o->v[j], p1->v[j]);
    }
  }
  else {
    for(int32_t j = min_degree + 1 ; j <= max_degree ; ++j) {
      rbc_elt_set(o->v[j], p2->v[j]);
    }
  }

  rbc_poly_update_degree(o, max_degree);
}



/**
 * \fn void rbc_plain_mul(rbc_poly o, const rbc_poly p1, const rbc_poly p2)
 * \brief This function multiplies two polynomials.
 *
 * This function is based on NTL multiplication of GF2EX elements
 *
 * \param[out] o Polynomial
 * \param[in] a Polynomial
 * \param[in] b Polynomial
 */
void rbc_plain_mul(rbc_poly o, const rbc_poly a, const rbc_poly b) {
  int32_t d = a->degree + b->degree;
  rbc_elt acc, tmp;

  if(o->max_degree < d) {
    rbc_poly_resize(o, d);
  }

  for(int32_t i = 0 ; i <= d ; ++i) {
    int32_t jmin, jmax;
    jmin = max(0, i - b->degree);
    jmax = min(a->degree, i);

    rbc_elt_set_zero(acc);

    for(int32_t j = jmin ; j <= jmax ; ++j) {
      rbc_elt_mul(tmp, a->v[j], b->v[i - j]);
      rbc_elt_add(acc, acc, tmp);
    }

    rbc_elt_set(o->v[i], acc);
  }

  rbc_poly_update_degree(o, d);
}



/**
 * Karatsuba multiplication of a and b using notations from "A course in computational algebraic number theory" (H. Cohen), 3.1.2. Implementation inspired from the NTL library.
 *
 * \param[out] o Polynomial
 * \param[in] a Polynomial
 * \param[in] b Polynomial
 */
void rbc_kar_fold(rbc_vec res, rbc_vec src, int32_t max, int32_t half_size) {
  for(int32_t i = 0 ; i < max / 2 ; ++i) {
    rbc_elt_add(res[i], src[i], src[i + half_size]);
  }

  if(max % 2 == 1) {
    rbc_elt_set(res[half_size - 1], src[half_size - 1]);
  }
}



void rbc_kar_mul(rbc_vec o, rbc_vec a, rbc_vec b, int32_t a_size, int32_t b_size, rbc_vec stack) {
  int32_t ha_size;
  rbc_vec a2, b2;

  // In every call a_size must be >= b_size
  if(a_size == 1) {
    rbc_elt_mul(o[0], a[0], b[0]);
    return;
  }

  if(a_size == 2) {
    if(b_size == 2) {
      // Hardcoded mul
      rbc_elt ea2, eb2, ed;
      rbc_elt_mul(o[0], a[0], b[0]);
      rbc_elt_mul(o[2], a[1], b[1]);
      rbc_elt_add(ea2, a[0], a[1]);
      rbc_elt_add(eb2, b[0], b[1]);
      rbc_elt_mul(ed, ea2, eb2);
      rbc_elt_add(o[1], o[0], o[2]);
      rbc_elt_add(o[1], o[1], ed);
    } else { // b_size = 1
      rbc_elt_mul(o[0], a[0], b[0]);
      rbc_elt_mul(o[1], a[1], b[0]);
    }
    return;
  }

  ha_size = (a_size + 1) / 2;

  // Compute a2 = a0 + a1 and b2 = b0 + b1
  a2 = stack;
  b2 = stack + ha_size;

  rbc_kar_fold(a2, a, a_size, ha_size);
  rbc_kar_fold(b2, b, b_size, ha_size);

  // Computation of d = a2*b2
  
  // Reset the stack
  for(int32_t i = 2 * ha_size ; i < 4 * ha_size ; ++i) {
    rbc_elt_set_zero(stack[i]);
  }

  rbc_kar_mul(stack + 2 * ha_size, a2, b2, ha_size, ha_size, stack + 4 * ha_size);

  for(int32_t i = 0 ; i < 2 * (a_size - ha_size - 1) + 1 + 2 * ha_size ; ++i) {
    rbc_elt_set_zero(o[i]);
  }

  // Computation of c0 = a0*b0 in the low part of o
  rbc_kar_mul(o, a, b, ha_size, ha_size, stack + 4 * ha_size);

  // Computation of c2 = a1*b1 in the high part of o
  rbc_kar_mul(o + 2 * ha_size, a + ha_size, b + ha_size, a_size - ha_size, b_size - ha_size, stack + 4 * ha_size);

  // Computation of c1 = d + c2 + c0
  for(int32_t i = 0 ; i < 2 * (a_size - ha_size - 1) + 1 ; ++i) {
    rbc_elt_add(stack[i + 2 * ha_size], stack[i + 2 * ha_size], (o + 2 * ha_size)[i]);
  }

  for(int32_t i = 0 ; i < 2 * (ha_size - 1) + 1 ; ++i) {
    rbc_elt_add(stack[i + 2 * ha_size], stack[i + 2 * ha_size], o[i]);
  }

  // Add c1 to o
  for(int32_t i = 0 ; i <= 2 * (ha_size - 1) + 1 ; i++) {
    rbc_elt_add(o[i + ha_size], o[i + ha_size], stack[i + 2 * ha_size]);
  }
}



/**
 * \fn void rbc_poly_mul(rbc_poly o, const rbc_poly p1, const rbc_poly p2)
 * \brief This function multiplies two polynomials.
 *
 * \param[in] o Product of <b>p1</b> and <b>p2</b>
 * \param[in] p1 Polynomial
 * \param[in] p2 Polynomial
 */
void rbc_poly_mul(rbc_poly o, const rbc_poly a, const rbc_poly b) {
  if(a->degree == -1 || b->degree == -1) {
    o->degree = -1;
    return;
  }

  if(a->degree <= 1 || b->degree <= 1) {
    rbc_plain_mul(o, a, b);
    return;
  }

  rbc_poly_mul2(o, a, b, a->degree, b->degree);
}



/**
 * \fn void rbc_poly_mul2(rbc_poly o, const rbc_poly a, const rbc_poly b, int32_t a_degree, int32_t b_degree)
 * \brief This function multiplies two polynomials using a_degree and b_degree as polynomials degrees.
 *
 * \param[out] o Product of <b>a</b> and <b>b</b>
 * \param[in] a Polynomial
 * \param[in] b Polynomial
 * \param[in] a_degree Degree to consider for a
 * \param[in] b_degree Degree to consider for b
 */
void rbc_poly_mul2(rbc_poly o, const rbc_poly a, const rbc_poly b, int32_t a_degree, int32_t b_degree) {
  int32_t max_degree;
  int32_t stack_size = 0;
  int32_t n;

  // Prepare polynomials such that the allocated space is big enough in every polynomial
  max_degree = max(a_degree, b_degree);
  if(a->max_degree < max_degree) {
    rbc_poly_resize(a, max_degree);
  }

  if(b->max_degree < max_degree) {
    rbc_poly_resize(b, max_degree);
  }

  if(o->max_degree < 2 * (max_degree + 1)) {
    rbc_poly_resize(o, 2 * (max_degree + 1));
  }

  n = max_degree;
  do {
    stack_size += 4 * ((n + 2) >> 1);
    n >>= 1;
  } while(n > 1);

  rbc_vec stack;
  rbc_vec_init(&stack, stack_size);

  rbc_kar_mul(o->v, a->v, b->v, max_degree + 1, max_degree + 1, stack);
  rbc_poly_update_degree(o, a->degree + b->degree);

  rbc_vec_clear(stack);
}



/**
 * \fn void rbc_poly_mulmod_sparse(rbc_poly o, const rbc_poly p1, const rbc_poly p2, const rbc_poly_sparse modulus)
 * \brief This function computes the product of two polynomials modulo a sparse one.
 *
 * \param[out] o Product of <b>p1</b> and <b>p2</b> modulo <b>modulus</b>
 * \param[in] p1 Polynomial
 * \param[in] p2 Polynomial
 * \param[in] modulus Sparse polynomial
 */
void rbc_poly_mulmod_sparse(rbc_poly o, const rbc_poly p1, const rbc_poly p2, const rbc_poly_sparse modulus) {
  int32_t modulus_degree = modulus->coeffs[modulus->coeffs_nb - 1];
  rbc_poly unreduced;

  if(o->max_degree < modulus_degree - 1) {
    rbc_poly_resize(o, modulus_degree - 1);
  }

  rbc_poly_init(&unreduced, 2 * modulus_degree - 1);
  rbc_poly_set_zero(unreduced, 2 * modulus_degree - 1);

  rbc_poly_mul2(unreduced, p1, p2, modulus_degree - 1, modulus_degree - 1);

  // Modular reduction
  for(int32_t i = unreduced->degree - modulus_degree ; i >= 0 ; i--) {
    for(size_t j = 0 ; j < modulus->coeffs_nb - 1 ; j++) {
      rbc_elt_add(unreduced->v[i + modulus->coeffs[j]],
      unreduced->v[i + modulus->coeffs[j]], unreduced->v[i + modulus_degree]);
    }
    rbc_elt_set_zero(unreduced->v[i + modulus_degree]);
  }

  rbc_poly_update_degree(unreduced, modulus_degree);
  rbc_poly_set(o, unreduced);
  rbc_poly_clear(unreduced);
}



/**
 * \fn void rbc_poly_div(rbc_poly q, rbc_poly r, const rbc_poly a, const rbc_poly b)
 * \brief This function performs the euclidean division of a by b such that a = b*q + r.
 *
 * \param[out] q Quotient
 * \param[out] r Remainder
 * \param[in] a Polynomial
 * \param[in] b Polynomial
 */
void rbc_poly_div(rbc_poly q, rbc_poly r, const rbc_poly a, const rbc_poly b) {
  if(q->max_degree < a->degree) {
    rbc_poly_resize(q, a->degree);
  }

  if(r->max_degree < a->degree) {
    rbc_poly_resize(r, a->degree);
  }

  // Initialization
  rbc_poly_set(r, a);
  rbc_poly_set_zero(q, a->degree);

  rbc_elt coeff, b_inv, tmp;
  rbc_elt_inv(b_inv, b->v[b->degree]);

  while(r->degree >= b->degree) {
    rbc_elt_mul(coeff, r->v[r->degree], b_inv);

    // Update quotient
    rbc_elt_set(q->v[r->degree - b->degree], coeff);

    // Update remainder
    for(int32_t i = r->degree; i >= r->degree - b->degree; --i) {
      rbc_elt_mul(tmp, coeff, b->v[i - r->degree + b->degree]);
      rbc_elt_add(r->v[i], r->v[i], tmp);
    }

    // Update remainder degree
    rbc_poly_update_degree(r, a->degree);
  }

  // Updates degrees
  rbc_poly_update_degree(r, a->degree);
  rbc_poly_update_degree(q, a->degree);
}



/**
 * \fn void rbc_poly_inv(rbc_poly o, const rbc_poly a, const rbc_poly modulus)
 * \brief This function computes the extended euclidean algorithm to compute u and v such that a.u + mod.v = GCD(a, mod).
 *
 * \param[out] o Inverse of <b>a</b> modulo <b>modulus</b>
 * \param[in] a Polynomial
 * \param[in] modulus Polynomial
 */
 void rbc_poly_inv(rbc_poly o, const rbc_poly a, const rbc_poly modulus) {
  uint32_t max_size = max(a->degree, modulus->degree);
  rbc_poly q, tmp, u, v, u0, u1, u2, qu;

  if(a->degree == -1 || modulus->degree == -1) {
    o->degree = -1;
    return;
  }

  rbc_poly_init(&q, max_size);
  rbc_poly_init(&tmp, max_size);
  rbc_poly_init(&u, max_size);
  rbc_poly_init(&v, max_size);
  rbc_poly_init(&u0, max_size);
  rbc_poly_init(&u1, max_size);
  rbc_poly_init(&u2, max_size);
  rbc_poly_init(&qu, 2 * max_size - 1);

  rbc_poly_set_zero(q, max_size);
  rbc_poly_set_zero(tmp, max_size);
  rbc_poly_set_zero(u, max_size);
  rbc_poly_set_zero(v, max_size);
  rbc_poly_set_zero(u0, max_size);
  rbc_poly_set_zero(u1, max_size);
  rbc_poly_set_zero(u2, max_size);
  rbc_poly_set_zero(qu, 2 * max_size - 1);

  // Initialization
  rbc_poly_set(u, a);
  rbc_poly_set(v, modulus);

  rbc_elt_set_one(u1->v[0]);
  rbc_poly_update_degree(u1, 1);

  while(v->degree != -1) {
    rbc_poly_div(q, tmp, u, v);
    rbc_poly_set(u, v);
    rbc_poly_set(v, tmp);
    rbc_poly_set(u0, u2);
    rbc_poly_mul(qu, q, u2);
    rbc_poly_add(u2, u1, qu);
    rbc_poly_set(u1, u0);
  }

  rbc_poly_set(o, u1);

  // Make the GCD monic
  rbc_elt scalar;
  rbc_elt_inv(scalar, u->v[u->degree]);
  rbc_vec_scalar_mul(o->v, o->v, scalar, o->degree + 1);

  rbc_poly_clear(q);
  rbc_poly_clear(tmp);
  rbc_poly_clear(u);
  rbc_poly_clear(v);
  rbc_poly_clear(u0);
  rbc_poly_clear(u1);
  rbc_poly_clear(u2);
  rbc_poly_clear(qu);
 }



/**
 * \fn void rbc_poly_to_string(uint8_t* str, const rbc_poly p)
 * \brief This function parses a polynomial into a string.
 *
 * \param[out] str String
 * \param[in] p rbc_poly
 */
void rbc_poly_to_string(uint8_t* str, const rbc_poly p) {
  rbc_vec_to_string(str, p->v, p->degree + 1);
}



/**
 * \fn void rbc_poly_from_string(rbc_poly p, const uint8_t* str)
 * \brief This function parses a string into a polynomial.
 *
 * \param[out] p rbc_poly
 * \param[in] str String to parse
 */
void rbc_poly_from_string(rbc_poly p, const uint8_t* str) {
  rbc_vec_from_string(p->v, p->degree + 1, str);
}



/**
 * \fn void rbc_poly_print(const rbc_poly p)
 * \brief This function displays a polynomial.
 *
 * \param[in] p rbc_poly
 */
void rbc_poly_print(const rbc_poly p) {
  for(int32_t i = 0 ; i < p->degree + 1 ; ++i) {
    printf("\n%i - ", i);
    rbc_elt_print(p->v[i]);
  }
}



/**
 * \fn void rbc_poly_sparse_print(const rbc_poly_sparse p)
 * \brief This function displays a sparse polynomial.
 *
 * \param[in] p rbc_poly_sparse
 */
void rbc_poly_sparse_print(const rbc_poly_sparse p) {
  for(size_t i = 0 ; i < p->coeffs_nb - 1 ; i++) {
    printf("X^%" PRIu32 " + ", p->coeffs[i]);
  }
  printf("X^%" PRIu32 "\n", p->coeffs[p->coeffs_nb - 1]);
}

