/**
 * \file rbc_qre.c
 * \brief Implementation of rbc_qre.h
 */

#include "rbc_elt.h"
#include "rbc_qre.h"
#include "rbc_poly.h"

static uint32_t RBC_QRE_DEG;
static int rbc_init_modulus = 0;
static rbc_poly_sparse rbc_qre_modulus;
static rbc_poly rbc_qre_modulus_inv;


/**
 * \fn void rbc_qre_init_modulus(uint32_t degree) {
 * \brief This function init the quotient ring modulus corresponding to PARAM_N.
 *
 * \param[in] degree Degree of the modulus
 */
void rbc_qre_init_modulus(uint32_t degree) {
  if(rbc_init_modulus == 0) {
    RBC_QRE_DEG = degree - 1;
    rbc_qre_modulus = rbc_qre_get_modulus(degree);
    rbc_poly_init(&rbc_qre_modulus_inv, RBC_QRE_DEG + 1);
    rbc_poly_set_zero(rbc_qre_modulus_inv, RBC_QRE_DEG + 1);
    for(uint32_t i = 0 ; i < rbc_qre_modulus->coeffs_nb ; i++) {
      rbc_elt_set_one(rbc_qre_modulus_inv->v[rbc_qre_modulus->coeffs[i]]);
    }
  }

  rbc_init_modulus++;
}



/**
 * \fn void rbc_qre_clear_modulus() {
 * \brief This function clears the quotient ring modulus in use.
 */
void rbc_qre_clear_modulus() {
  rbc_init_modulus--;

  if(rbc_init_modulus == 0) {
    rbc_poly_sparse_clear(rbc_qre_modulus);
    rbc_poly_clear(rbc_qre_modulus_inv);
  }
}



/**
 * \fn rbc_poly_sparse rbc_qre_get_modulus(uint32_t degree) {
 * \brief This function return the sparse polynomial used as the quotient ring modulus for PARAM_N.
 *
 * \param[in] degree Degree of the modulus
 */
rbc_poly_sparse rbc_qre_get_modulus(uint32_t degree) {
  rbc_poly_sparse modulus;
  uint32_t* values;

  if(degree == 83) {
    values = (uint32_t*) malloc(5 * sizeof(uint32_t));
    values[0] = 0;
    values[1] = 2;
    values[2] = 4;
    values[3] = 7;
    values[4] = 83;
    rbc_poly_sparse_init(&modulus, 5, values);
    free(values);
  } else if(degree == 97) {
    values = (uint32_t*) malloc(3 * sizeof(uint32_t));
    values[0] = 0;
    values[1] = 6;
    values[2] = 97;
    rbc_poly_sparse_init(&modulus, 3, values);
    free(values);
  } else if(degree == 113) {
    values = (uint32_t*) malloc(3 * sizeof(uint32_t));
    if(values == NULL) exit(EXIT_FAILURE);
    values[0] = 0;
    values[1] = 9;
    values[2] = 113;
    rbc_poly_sparse_init(&modulus, 3, values);
    free(values);
  } else if(degree == 149) {
    values = (uint32_t*) malloc(5 * sizeof(uint32_t));
    if(values == NULL) exit(EXIT_FAILURE);
    values[0] = 0;
    values[1] = 7;
    values[2] = 9;
    values[3] = 10;
    values[4] = 149;
    rbc_poly_sparse_init(&modulus, 5, values);
    free(values);
  } else if(degree == 179) {
    values = (uint32_t*) malloc(5 * sizeof(uint32_t));
    if(values == NULL) exit(EXIT_FAILURE);
    values[0] = 0;
    values[1] = 1;
    values[2] = 2;
    values[3] = 4;
    values[4] = 179;
    rbc_poly_sparse_init(&modulus, 5, values);
    free(values);
  } else if(degree == 189) {
    values = (uint32_t*) malloc(5 * sizeof(uint32_t));
    values[0] = 0;
    values[1] = 2;
    values[2] = 5;
    values[3] = 6;
    values[4] = 189;
    rbc_poly_sparse_init(&modulus, 5, values);
    free(values);
  } else if(degree == 193) {
    values = (uint32_t*) malloc(3 * sizeof(uint32_t));
    values[0] = 0;
    values[1] = 15;
    values[2] = 193;
    rbc_poly_sparse_init(&modulus, 3, values);
    free(values);
  } else if(degree == 211) {
    values = (uint32_t*) malloc(5 * sizeof(uint32_t));
    values[0] = 0;
    values[1] = 8;
    values[2] = 10;
    values[3] = 11;
    values[4] = 211;
    rbc_poly_sparse_init(&modulus, 5, values);
    free(values);
  }

  return modulus;
}



/**
 * \fn void rbc_qre_init(rbc_qre* p)
 * \brief This function allocates the memory for a rbc_qre element.
 *
 * \param[out] p Pointer to the allocated rbc_qre
 */
void rbc_qre_init(rbc_qre* p) {
  rbc_poly_init(p, RBC_QRE_DEG);
}



/**
 * \fn void rbc_qre_clear(rbc_qre p)
 * \brief This function clears the memory allocated for a rbc_qre element.
 *
 * \param[in] p rbc_qre
 */
void rbc_qre_clear(rbc_qre p) {
  rbc_poly_clear(p);
}



/**
 * \fn void rbc_qre_set_zero(rbc_qre o)
 * \brief This functions sets a rbc_qre to zero.
 *
 * \param[in] o rbc_qre
 */
void rbc_qre_set_zero(rbc_qre o) {
  rbc_poly_set_zero(o, RBC_QRE_DEG);
}



/**
 * \fn void rbc_qre_set_random(AES_XOF_struct* ctx, rbc_qre o)
 * \brief This function sets a rbc_qre with random values using NIST seed expander.
 *
 * \param[out] ctx NIST seed expander
 * \param[out] o rbc_qre
 */
void rbc_qre_set_random(AES_XOF_struct* ctx, rbc_qre o) {
  rbc_poly_set_random(ctx, o, RBC_QRE_DEG);
}



/**
 * \fn void rbc_qre_set_random2(rbc_qre o)
 * \brief This function sets a rbc_qre with random values using randombytes.
 *
 * \param[out] o rbc_qre
 */
void rbc_qre_set_random2(rbc_qre o) {
  rbc_poly_set_random2(o, RBC_QRE_DEG);
}



/**
 * \fn void rbc_qre_set_random_full_rank(AES_XOF_struct* ctx, rbc_qre o)
 * \brief This function sets a rbc_qre with random values using the NIST seed expander. The returned rbc_qre has full rank.
 *
 * \param[out] ctx NIST seed expander
 * \param[out] o rbc_qre
 */
void rbc_qre_set_random_full_rank(AES_XOF_struct* ctx, rbc_qre o) {
  rbc_poly_set_random_full_rank(ctx, o, RBC_QRE_DEG);
}



/**
 * \fn void rbc_qre_set_random_full_rank_with_one(AES_XOF_struct* ctx, rbc_qre o)
 * \brief This function sets a rbc_qre with random values using the NIST seed expander. The returned rbc_qre has full rank and contains one.
 *
 * \param[out] ctx NIST seed expander
 * \param[out] o rbc_qre
 */
void rbc_qre_set_random_full_rank_with_one(AES_XOF_struct* ctx, rbc_qre o) {
  rbc_poly_set_random_full_rank_with_one(ctx, o, RBC_QRE_DEG);
}



/**
 * \fn void rbc_qre_set_random_from_support(AES_XOF_struct* ctx, rbc_qre o, const rbc_vspace support, uint32_t support_size)
 * \brief This function sets a rbc_qre with random values using the NIST seed expander. The support of the rbc_qre returned by this function is the one given in input.
 *
 * \param[out] ctx NIST seed expander
 * \param[out] o rbc_qre
 * \param[in] support Support of <b>o</b>
 * \param[in] support_size Size of the support
 */
void rbc_qre_set_random_from_support(AES_XOF_struct* ctx, rbc_qre o, const rbc_vspace support, uint32_t support_size) {
  rbc_poly_set_random_from_support(ctx, o, RBC_QRE_DEG, support, support_size);
}



/**
 * \fn void rbc_qre_set_random_pair_from_support(AES_XOF_struct* ctx, rbc_qre o1, rbc_qre o2, const rbc_vspace support, uint32_t support_size)
 * \brief This function sets a pair of rbc_qre with random values using the NIST seed expander. The support of (o1 || o2) is the one given in input.
 *
 * \param[out] ctx NIST seed expander
 * \param[out] o1 rbc_qre
 * \param[out] o2 rbc_qre
 * \param[in] support Support
 * \param[in] support_size Size of the support
 */
void rbc_qre_set_random_pair_from_support(AES_XOF_struct* ctx, rbc_qre o1, rbc_qre o2, const rbc_vspace support, uint32_t support_size) {
  rbc_poly_set_random_pair_from_support(ctx, o1, o2, RBC_QRE_DEG, support, support_size);
}



/**
 * \fn void rbc_qre_set_random_pair_from_support(AES_XOF_struct* ctx, rbc_qre o1, rbc_qre o2, const rbc_vspace support, uint32_t support_size)
 * \brief This function sets a pair of rbc_qre with random values using the NIST PRNG. The support of (o1 || o2) is the one given in input.
 *
 * \param[out] ctx NIST seed expander
 * \param[out] o1 rbc_qre
 * \param[out] o2 rbc_qre
 * \param[in] support Support
 * \param[in] support_size Size of the support
 */
void rbc_qre_set_random_pair_from_support2(rbc_qre o1, rbc_qre o2, const rbc_vspace support, uint32_t support_size) {
  rbc_poly_set_random_pair_from_support2(o1, o2, RBC_QRE_DEG, support, support_size);
}



/**
 * \fn uint8_t rbc_qre_is_equal_to(const rbc_qre p1, const rbc_qre p2)
 * \brief This function test if two rbc_qre are equal.
 *
 * \param[in] p1 rbc_qre
 * \param[in] p2 rbc_qre
 * \return 1 if the rbc_qre are equal, 0 otherwise
 */
uint8_t rbc_qre_is_equal_to(const rbc_qre p1, const rbc_qre p2) {
  return rbc_poly_is_equal_to(p1, p2);
}



/**
 * \fn void rbc_qre_add(rbc_qre o, const rbc_qre p1, const rbc_qre p2)
 * \brief This function adds two rbc_qre.
 *
 * \param[out] o Sum of <b>p1</b> and <b>p2</b>
 * \param[in] p1 rbc_qre
 * \param[in] p2 rbc_qre
 */
void rbc_qre_add(rbc_qre o, const rbc_qre p1, const rbc_qre p2) {
  rbc_poly_add2(o, p1, p2, RBC_QRE_DEG, RBC_QRE_DEG);
}



/**
 * \fn void rbc_qre_mul(rbc_qre o, const rbc_qre p1, const rbc_qre p2)
 * \brief This function multiplies two rbc_qre.
 *
 * \param[out] o Product of <b>p1</b> and <b>p2</b>
 * \param[in] p1 rbc_qre
 * \param[in] p2 rbc_qre
 */
void rbc_qre_mul(rbc_qre o, const rbc_qre p1, const rbc_qre p2) {
  rbc_poly_mulmod_sparse(o, p1, p2, rbc_qre_modulus);
}



/**
 * \fn void rbc_qre_div(rbc_qre q, rbc_qre r, const rbc_qre a, const rbc_qre b)
 * \brief This function performs the euclidean division of a by b such that a = b*q + r
 *
 * \param[out] q Quotient
 * \param[out] r Remainder
 * \param[in] a rbc_qre
 * \param[in] b rbc_qre
 */
void rbc_qre_div(rbc_qre q, rbc_qre r, const rbc_qre a, const rbc_qre b) {
  rbc_poly_div(q, r, a, b);
}



/**
 * \fn void rbc_qre_inv(rbc_qre o, const rbc_qre p)
 * \brief This function performs the extended euclidean algorithm to compute the inverse of p.
 *
 * \param[out] o Inverse of <b>e</b> modulo <b>modulus</b>
 * \param[in] p rbc_qre
 * \param[in] modulus Polynomial
 */
void rbc_qre_inv(rbc_qre o, const rbc_qre p) {
  rbc_poly_inv(o, p, rbc_qre_modulus_inv);
}



/**
 * \fn void rbc_qre_to_string(uint8_t* str, const rbc_qre p)
 * \brief This function parses a rbc_qre into a string.
 *
 * \param[out] str String
 * \param[in] e rbc_qre
 */
void rbc_qre_to_string(uint8_t* str, const rbc_qre p) {
  rbc_poly_to_string(str, p);
}



/**
 * \fn void rbc_qre_from_string(rbc_qre o, const uint8_t* str)
 * \brief This function parses a string into a rbc_qre.
 *
 * \param[out] o rbc_qre
 * \param[in] str String to parse
 */
void rbc_qre_from_string(rbc_qre o, const uint8_t* str) {
  rbc_poly_from_string(o, str);
}



/**
 * \fn void rbc_qre_print(const rbc_qre p)
 * \brief This function displays a rbc_qre.
 *
 * \param[in] p rbc_qre
 */
void rbc_qre_print(const rbc_qre p) {
  rbc_poly_print(p);
}

