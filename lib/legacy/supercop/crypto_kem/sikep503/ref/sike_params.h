//
// Supersingular Isogeny Key Encapsulation Ref. Library
//
// InfoSec Global Inc., 2017
// Basil Hess <basil.hess@infosecglobal.com>
//

/** @file
SIKE parameters and initialization procedures
*/

#ifndef ISOGENY_REF_SIKE_PARAMS_H
#define ISOGENY_REF_SIKE_PARAMS_H

#include <montgomery.h>

/**
 * Raw SIDH parameters
 */
typedef struct {
  // Starting curve is
  // E0/F_P2: y^2 = x^3 + x
  // ord(E0) = (2^eA * 3^eB)^2
  const char* name;

  // Prime p
  // p = lA^eA*lB^eB - 1
  const char* p;

  const char* eA;
  const char* eB;

  const char* lA;
  const char* lB;

  // Public generators for Alice: Q_A, P_A
  // Differential coordinate R_A = P_A - Q_A
  const char* xQA0;
  const char* xQA1;

  const char* yQA0;
  const char* yQA1;

  const char* xPA0;
  const char* xPA1;

  const char* yPA0;
  const char* yPA1;

  const char* xRA0;
  const char* xRA1;

  const char* yRA0;
  const char* yRA1;

  // Public generators for Bob: Q_B, P_B
  // Differential coordinate R_B = P_B - Q_B
  const char* xQB0;
  const char* xQB1;

  const char* yQB0;
  const char* yQB1;

  const char* xPB0;
  const char* xPB1;

  const char* yPB0;
  const char* yPB1;

  const char* xRB0;
  const char* xRB1;

  const char* yRB0;
  const char* yRB1;

  size_t crypto_bytes;
  size_t msg_bytes;
} sike_params_raw_t;

/**
 * Internal, decoded SIKE parameters
 */
typedef struct {
  // starting curve with generator for Alice
  mont_curve_int_t EA;
  // starting curve with generator for Bob
  mont_curve_int_t EB;

  mp p;

  unsigned long eA;
  unsigned long lA;
  mp ordA;

  unsigned long eB;
  unsigned long lB;
  mp ordB;

  unsigned long msbA; // MSB of ordA
  unsigned long msbB; // MSB of ordB

  size_t crypto_bytes;
  size_t msg_bytes;
} sike_params_t;

/**
 * Set up the parameters from provided raw parameters.
 * @param raw Raw parameters
 * @param params Internal parameters to be setup.
 * @return
 */
void
sike_setup_params(const sike_params_raw_t *raw, sike_params_t *params);

/**
 * Tears down/deinitializes the SIDH parameters
 * @param params Parameters to be teared down
 * @return
 */
void
sike_teardown_params(sike_params_t *params);

/**
 * SIKEp503 raw parameters
 */
extern const sike_params_raw_t SIKEp503;

/**
 * SIKEp751 raw parameters
 */
extern const sike_params_raw_t SIKEp751;

/**
 * SIKEp964 raw parameters
 */
extern const sike_params_raw_t SIKEp964;

#endif //ISOGENY_REF_SIKE_PARAMS_H
