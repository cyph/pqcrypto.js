/**
 * \file gf2x.cpp
 * \brief Implementation of multiplication of two polynomials
 */

#include "gf2x.h"
using namespace NTL;

/**
 * \fn GF2XModulus init_modulo()
 * \brief This function build the modulus \f$ X^n - 1\f$
 * \return the modulus as an GF2XModulus object
 */
GF2XModulus init_modulo() {
  GF2XModulus P;
  GF2X tmp;
  SetCoeff(tmp, PARAM_N, 1);
  SetCoeff(tmp, 0, 1);
  build(P, tmp);
  return P;
}

/** 
 * \fn void ntl_cyclic_product(uint8_t*o, const uint8_t* v1, const uint8_t* v2)
 * \brief Multiply two vectors
 *
 * Vector multiplication is defined as polynomial multiplication performed modulo the polynomial \f$ X^n - 1\f$.
 *
 * \param[out] o Product of <b>v1</b> and <b>v2</b>
 * \param[in] v1 Pointer to the first vector
 * \param[in] v2 Pointer to the second vector
 */
void ntl_cyclic_product(uint8_t*o, const uint8_t* v1, const uint8_t* v2) {
  GF2X tmp, poly1, poly2;

  GF2XFromBytes(poly1, v1, VEC_N_SIZE_BYTES);
  GF2XFromBytes(poly2, v2, VEC_N_SIZE_BYTES);
  GF2XModulus P = init_modulo();

  MulMod(tmp, poly1, poly2, P);

  BytesFromGF2X(o, tmp, VEC_N_SIZE_BYTES);
}