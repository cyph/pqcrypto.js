/**
 * \file gabidulin.h
 * \brief Functions to encode and decode messages using Gabidulin codes
 *
 * The decoding algorithm provided is based on q_polynomials reconstruction, see \cite gabidulin:welch and \cite gabidulin:generalized for details.
 *
 */

#ifndef RBC_GABIDULIN_H
#define RBC_GABIDULIN_H

#include "rbc_vec.h"
#include "rbc_qre.h"


/**
  * \typedef rbc_gabidulin
  * \brief Structure of a gabidulin code
  */
typedef struct rbc_gabidulin {
  rbc_poly g; /**< Generator vector defining the code */
  uint32_t k; /**< Size of vectors representing messages */
  uint32_t n; /**< Size of vectors representing codewords */
} rbc_gabidulin;


void rbc_gabidulin_init(rbc_gabidulin* code, const rbc_poly g, uint32_t k, uint32_t n);

void rbc_gabidulin_encode(rbc_qre c, const rbc_gabidulin gc, const rbc_vec m);
void rbc_gabidulin_decode(rbc_vec m, const rbc_gabidulin gc, const rbc_qre y);

#endif

