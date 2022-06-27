/**
 * \file gabidulin.c
 * \brief Implementation of gabidulin.h
 *
 * The decoding algorithm provided is based on rbc_qpolys reconstruction, see \cite gabidulin:welch and \cite gabidulin:generalized for details.
 *
 */

#include "rbc_elt.h"
#include "rbc_vec.h"
#include "gabidulin.h"
#include "qpoly.h"

/** 
 * \fn void rbc_gabidulin_init(rbc_gabidulin* code, const rbc_poly g, uint32_t k, uint32_t n)
 * \brief This function initializes a gabidulin code.
 *
 * \param[in] code Gabidulin code
 * \param[in] g Generator vector defining the code
 * \param[in] k Size of vectors representing messages
 * \param[in] n Size of vectors representing codewords
 */
void rbc_gabidulin_init(rbc_gabidulin* code, const rbc_poly g, uint32_t k, uint32_t n) {
  code->g = g;
  code->k = k;
  code->n = n;
}



/** 
 * \fn void rbc_gabidulin_encode(rbc_qre c, const rbc_gabidulin gc, const rbc_vec m)
 * \brief This function encodes a message into a codeword.
 *
 * \param[out] c Vector of size <b>n</b> representing a codeword
 * \param[in] gc Gabidulin code
 * \param[in] m Vector of size <b>k</b> representing a message
 */
void rbc_gabidulin_encode(rbc_qre c, const rbc_gabidulin gc, const rbc_vec m) {
  // Compute generator matrix
  rbc_elt matrix[gc.k][gc.n];
  for(size_t j = 0 ; j < gc.n ; ++j) {
    rbc_elt_set(matrix[0][j], gc.g->v[j]);
    for(size_t i = 1 ; i < gc.k ; ++i) {
      rbc_elt_sqr(matrix[i][j], matrix[i-1][j]);
    }
  }

  // Encode message
  rbc_elt tmp;
  rbc_elt_set_zero(tmp);
  rbc_vec_set_zero(c->v, gc.n);
  for(size_t i = 0 ; i < gc.k ; ++i) {
    for(size_t j = 0 ; j < gc.n ; ++j) {
      rbc_elt_mul(tmp, m[i], matrix[i][j]);
      rbc_elt_add(c->v[j], c->v[j], tmp);
    }
  }

  #ifdef VERBOSE
    printf("\n\n\n# Gabidulin Encoding - Begin #");
    printf("\n\ng: "); rbc_poly_print(gc.g);

    printf("\n\nmatrix:[ ");
    for(size_t i = 0 ; i < gc.k ; ++i) {
      printf("[ ");
      for(size_t j = 0 ; j < gc.n ; ++j) {
        rbc_elt_print(matrix[i][j]);
      }
      printf("] ");
    }
    printf("]\n");

    printf("\ncodeword: "); rbc_poly_print(c);
    printf("\n\n# Gabidulin Encoding - End #\n");
  #endif
}



/** 
 * \fn void rbc_gabidulin_decode(rbc_vec m, const rbc_gabidulin gc, const rbc_qre y)
 * \brief This function decodes a word.
 *
 * The provided decoding algorithm works as follows (see \cite gabidulin:welch and \cite gabidulin:generalized for details):
 *   1. Find a solution (<b>V</b>, <b>N</b>) of the q-polynomial Reconstruction2(<b>y</b>, <b>gc.g</b>, <b>gc.k</b>, (<b>gc.n</b> - <b>gc.k</b>)/2) problem using \cite gabidulin:generalized (section 4, algorithm 5) ;
 *   2. Find <b>f</b> by computing <b>V \ (N.A) + I</b> (see "Polynomials with lower degree" improvement from \cite gabidulin:generalized, section 4.4.2) ;
 *   3. Retrieve the message <b>m</b> as the k first coordinates of <b>f</b>.
 *
 * \param[out] m Vector of size <b>k</b> representing a message
 * \param[in] gc Gabidulin code
 * \param[in] y Vector of size <b>n</b> representing a word to decode
 */
void rbc_gabidulin_decode(rbc_vec m, const rbc_gabidulin gc, const rbc_qre y) {

  /*  
   *  Step 1: Solving the q-polynomial reconstruction2 problem 
   */

  uint32_t t = (gc.n - gc.k) / 2;
  int32_t max_degree_N = (gc.n - gc.k) % 2 == 0 ? gc.k + t - 1 : gc.k + t;

  rbc_qpoly A, I, N0, N1, V0, V1, N1_result, V1_result;
  rbc_qpoly_init(&A, gc.k);
  rbc_qpoly_init(&I, gc.k - 1);
  rbc_qpoly_init(&N0, max_degree_N);
  rbc_qpoly_init(&N1, max_degree_N);
  rbc_qpoly_init(&V0, t);
  rbc_qpoly_init(&V1, t);
  rbc_qpoly_init(&N1_result, max_degree_N);
  rbc_qpoly_init(&V1_result, t);

  rbc_qpoly qtmp1, qtmp2, qtmp3, qtmp4, qtmp5;
  rbc_qpoly_init(&qtmp1, max_degree_N);
  rbc_qpoly_init(&qtmp2, max_degree_N);
  rbc_qpoly_init(&qtmp3, max_degree_N);
  rbc_qpoly_init(&qtmp4, t);
  rbc_qpoly_init(&qtmp5, t);

  rbc_vec u0, u1, rand;
  rbc_vec_init(&u0, gc.n);
  rbc_vec_init(&u1, gc.n);
  rbc_vec_init(&rand, gc.n);
  rbc_vec_set_random2(rand, gc.n);

  rbc_elt e1, e2, tmp1, tmp2;
  uint32_t status = 0;

  // Initialization step
  
  // A(g[i]) = 0 for 0 <= i <= k - 1
  // I(g[i]) = y[i] for 0 <= i <= k - 1
  
  rbc_qpoly_set_interpolate_vect_and_zero(A, I, gc.g, y, gc.k);

  rbc_qpoly_set_one(N0);
  rbc_qpoly_set_zero(N1);
  rbc_qpoly_set_zero(V0);
  rbc_qpoly_set_one(V1);

  rbc_qpoly_set_zero(N1_result);
  rbc_qpoly_set_one(V1_result);

  rbc_qpoly_set_zero(qtmp1);
  rbc_qpoly_set_zero(qtmp2);
  rbc_qpoly_set_zero(qtmp3);
  rbc_qpoly_set_zero(qtmp4);
  rbc_qpoly_set_zero(qtmp5);

  // u0[i] = A(g[i]) - V0(y[i])
  // u1[i] = I(g[i]) - V1(y[i])

  for(size_t i = 0 ; i < gc.n ; ++i) {
    rbc_qpoly_evaluate(tmp1, A, gc.g->v[i]);
    rbc_qpoly_evaluate(tmp2, V0, y->v[i]);
    rbc_elt_add(tmp1, tmp1, tmp2);
    rbc_elt_set(u0[i], tmp1);

    rbc_qpoly_evaluate(tmp1, I, gc.g->v[i]);
    rbc_qpoly_evaluate(tmp2, V1, y->v[i]);
    rbc_elt_add(tmp1, tmp1, tmp2);
    rbc_elt_set(u1[i], tmp1);

    status = status | !rbc_elt_is_zero(u1[i]);
  }

  #ifdef VERBOSE
    printf("\n\n# Gabidulin Decoding - Begin #");
    printf("\n\ng: "); rbc_poly_print(gc.g);
    printf("\n\nA: "); rbc_qpoly_print(A);
    printf("\nI: "); rbc_qpoly_print(I);
    printf("\nN0 (init): "); rbc_qpoly_print(N0);
    printf("\nN1 (init): "); rbc_qpoly_print(N1);
    printf("\nV0 (init): "); rbc_qpoly_print(V0);
    printf("\nV1 (init): "); rbc_qpoly_print(V1);
  #endif

  // Interpolation step 
  for(size_t i = gc.k ; i < gc.n ; ++i) {

    uint32_t next = i;
    uint32_t r = 1;

    for(size_t k = i ; k < gc.n ; ++k) {
      r &= rbc_elt_is_zero(u1[k]);
      next = r * (next + 1) + (1 - r) * next;
    }

    status = status & (next != gc.n);
    next = next == gc.n ? next - 1 : next;

    // Permutation of the coordinates of positions i and next
    rbc_elt_set(tmp1, u0[i]);
    rbc_elt_set_mask1(u0[i], u0[next], rand[i], status);
    rbc_elt_set_mask2(u0[next], rand[i], tmp1, status);

    rbc_elt_set(tmp2, u1[i]);
    rbc_elt_set_mask1(u1[i], u1[next], rand[i], status);
    rbc_elt_set_mask2(u1[next], rand[i], tmp2, status);

    // Update q_polynomials according to discrepancies

    // e1 = - u1[i]^q / u1[i] 
    // e2 = - u0[i] / u1[i]
    // N0' = N1^q - e1.N1
    // V0' = V1^q - e1.V1
    // N1' = N0 - e2.N1 
    // V1' = V0 - e2.V1
    
    rbc_elt_inv(tmp1, u1[i]);
    rbc_elt_sqr(e1, u1[i]);
    rbc_elt_mul(e1, e1, tmp1);
    rbc_elt_mul(e2, u0[i], tmp1);
      
    rbc_qpoly_scalar_mul(qtmp1, N1, e1);
    rbc_qpoly_qexp(qtmp2, N1);
    rbc_qpoly_scalar_mul(qtmp4, V1, e1);
    rbc_qpoly_qexp(qtmp5, V1);

    rbc_qpoly_scalar_mul(N1, N1, e2);
    rbc_qpoly_add(N1, N0, N1);

    rbc_qpoly_scalar_mul(V1, V1, e2);
    rbc_qpoly_add(V1, V0, V1);
      
    rbc_qpoly_add(N0, qtmp1, qtmp2);
    rbc_qpoly_add(V0, qtmp4, qtmp5);

    rbc_qpoly_set_mask(N1_result, N1, N1_result, status);
    rbc_qpoly_set_mask(V1_result, V1, V1_result, status);

    // Update discrepancies
    for(size_t k = i + 1 ; k < gc.n ; ++k) {

      // u0[k]' = u1[k]^q - e1.u1[k]
      // u1[k]' = u0[k] - e2.u1[k] 
      
      rbc_elt_mul(tmp1, e1, u1[k]);
      rbc_elt_sqr(tmp2, u1[k]);
      rbc_elt_add(tmp1, tmp1, tmp2);

      rbc_elt_mul(tmp2, e2, u1[k]);
      rbc_elt_add(tmp2, tmp2, u0[k]);
      rbc_elt_set(u1[k], tmp2);

      rbc_elt_set(u0[k], tmp1);
    }

    #ifdef VERBOSE
      printf("\n");
      printf("\nN0 (%zu): ", i); rbc_qpoly_print(N0);
      printf("\nN1 (%zu): ", i); rbc_qpoly_print(N1);
      printf("\nV0 (%zu): ", i); rbc_qpoly_print(V0);
      printf("\nV1 (%zu): ", i); rbc_qpoly_print(V1);
    #endif
  }



  /*  
   *  Step 2: Computing f (qtmp1 variable) using Loidreau's improvement for lower degree polynomials
   */

  rbc_qpoly_mul2(qtmp1, N1_result, A, t - 1, gc.k);

  status = 1 - (N1_result->degree == -1 && V1_result->degree == 0);
  rbc_qpoly_set_mask(qtmp1, qtmp1, N1, status);
  rbc_qpoly_set_mask(V1_result, V1_result, V1, status);

  rbc_qpoly_left_div2(qtmp3, qtmp2, qtmp1, V1_result, t, gc.k);

  rbc_qpoly_add(qtmp1, qtmp3, I);
  rbc_qpoly_set_mask(qtmp1, qtmp1, I, status);



  /*  
   *  Step 3: Decoding the message as the value of the k first coordinates of f (qtmp1 variable)
   */
  
  rbc_vec_set(m, qtmp1->values, gc.k);

  #ifdef VERBOSE
    printf("\nquotient: "); rbc_qpoly_print(qtmp1);
    printf("\nremainder: "); rbc_qpoly_print(qtmp2);
    printf("\nmu: "); rbc_vec_print(m, gc.k);
    printf("\n# Gabidulin Decoding - End #\n");
  #endif

  rbc_qpoly_clear(A);
  rbc_qpoly_clear(I);

  rbc_qpoly_clear(N0);
  rbc_qpoly_clear(N1);
  rbc_qpoly_clear(V0);
  rbc_qpoly_clear(V1);

  rbc_qpoly_clear(N1_result);
  rbc_qpoly_clear(V1_result);

  rbc_qpoly_clear(qtmp1);
  rbc_qpoly_clear(qtmp2);
  rbc_qpoly_clear(qtmp3);
  rbc_qpoly_clear(qtmp4);
  rbc_qpoly_clear(qtmp5);

  rbc_vec_clear(u0);
  rbc_vec_clear(u1);
  rbc_vec_clear(rand);
}

