//
// Supersingular Isogeny Key Encapsulation Ref. Library
//
// InfoSec Global Inc., 2017-2020
// Basil Hess <basil.hess@infosecglobal.com>
//

/** @file
Data structures and arithmetic for supersingular Montgomery curves
*/

#ifndef ISOGENY_REF_MONTGOMERY_H
#define ISOGENY_REF_MONTGOMERY_H

#include <fp2.h>

/////////////
// Data types
/////////////

/**
 * Represents a point on a (Montgomery) curve with `x` and `y`
 */
typedef struct {
  fp2 x;
  fp2 y;
} mont_pt_t;

/**
 * Internal representation of a Montgomery curve with.
 * - Underlying finite field parameters
 * - Generators `P` and `Q`
 * - Coefficients `a` and `b`
 */
typedef struct {
  ff_Params* ffData;
  fp2 a;
  fp2 b;
  mont_pt_t P;
  mont_pt_t Q;
} mont_curve_int_t;

/**
 * External representation of a Montgomery curve for the public keys.
 * - Underlying finite field parameters
 * - Projective x-coordinates of generators `P` and `Q`
 * - Projective x-coordinate of `R`=`P`-`Q`
 */
typedef struct {
  ff_Params* ffData;
  fp2 xP;
  fp2 xQ;
  fp2 xR;
} sike_public_key_t;

/////////////////////////////////////////////////////////////////////
// Initialization and deinitialization routines for Montgomery curves
/////////////////////////////////////////////////////////////////////

/**
 * Initialization of a curve
 *
 * @param p Finite field parameters
 * @param curve Curve to be initialized
 */
void mont_curve_init(ff_Params* p, mont_curve_int_t* curve);

/**
 * Copies a curve, curvecopy := curve
 * @param p Finite field parameters
 * @param curve Source curve
 * @param curvecopy Destination curve
 */
void mont_curve_copy(const ff_Params* p, const mont_curve_int_t* curve, mont_curve_int_t* curvecopy);

/**
 * Deinitialization of a curve
 *
 * @param p Finite field parameters
 * @param curve Curve to be deinitialized
 */
void mont_curve_clear(const ff_Params* p, mont_curve_int_t* curve);

/**
 * Initialization of a point
 *
 * @param p Finite field parameters
 * @param pt Point to be initialized
 */
void mont_pt_init(const ff_Params* p, mont_pt_t* pt);

/**
 * Deinitialization of a point
 *
 * @param p Finite field parameters
 * @param pt Point to be deinitialized
 */
void mont_pt_clear(const ff_Params* p, mont_pt_t* pt);


/**
 * Initialization of a public key
 *
 * @param p Finite field parameters
 * @param pk Public key to be initialized
 */
void public_key_init(ff_Params* p, sike_public_key_t* pk);

/**
 * Deinitialization of a public key
 *
 * @param p Finite field parameters
 * @param pk Public key to be deinitialized
 */
void public_key_clear(const ff_Params* p, sike_public_key_t* pk);

/**
 * Copies a point. dst := src
 *
 * @param p Finite field parameters
 * @param src Source point
 * @param dst Destination point
 */
void mont_pt_copy(const ff_Params* p, const mont_pt_t* src, mont_pt_t* dst);


///////////////////////////////////////////////
// Montgomery curve arithmetic - affine version
///////////////////////////////////////////////

/**
 * Scalar multiplication using the double-and-add.
 * Note: add side-channel countermeasures for production use.
 *
 * @param curve Underlying curve
 * @param k Scalar
 * @param P Point
 * @param Q Result Q=kP
 * @param msb Most significant bit of scalar `k`
 */
void mont_double_and_add(const mont_curve_int_t *curve, const mp k, const mont_pt_t *P, mont_pt_t *Q, int msb);

/**
 * Affine doubling.
 *
 * @param curve Underlying curve
 * @param P Point
 * @param R Result R=2P
 */
void xDBL(const mont_curve_int_t *curve, const mont_pt_t *P, mont_pt_t *R);

/**
 * Repeated affine doubling
 *
 * @param curve Underlying curve
 * @param P Point
 * @param e Repetitions
 * @param R Result R=2^e*P
 */
void xDBLe(const mont_curve_int_t *curve, const mont_pt_t *P, int e, mont_pt_t *R);

/**
 * Affine addition.
 *
 * @param curve Underlying curve
 * @param P First point
 * @param Q Second point
 * @param R Result R=P+Q
 */
void xADD(const mont_curve_int_t *curve, const mont_pt_t *P, const mont_pt_t *Q, mont_pt_t *R);

/**
 * Affine tripling.
 *
 * @param curve Underlying curve
 * @param P Point
 * @param R Result R=3P
 */
void xTPL(const mont_curve_int_t *curve, const mont_pt_t *P, mont_pt_t *R);

/**
 * Repeated affine tripling.
 *
 * @param curve Underlying curve
 * @param P Point
 * @param e Repetitions
 * @param R Result R=3^e*P
 */
void xTPLe(const mont_curve_int_t *curve, const mont_pt_t *P, int e, mont_pt_t *R);

/**
 * J-invariant of a montgomery curve
 *
 * jinv = 256*(a^2-3)^3/(a^2-4);
 *
 * @param p Finite field parameters
 * @param E Montgomery curve
 * @param jinv Result: j-invariant
 */
void j_inv(const ff_Params *p, const mont_curve_int_t *E, fp2 *jinv);

/**
 * Conversion of a Montgomery curve with affine parameters to the external format for public keys.
 *
 * a, b, P.x, P.y, Q.x, Q.y -> P.x, Q.x, (P-Q).x
 *
 * @param p Finite field arithmetic
 * @param curve Montgomery curve
 * @param pk Public key parameters
 */
void get_xR(const ff_Params *p,
            const mont_curve_int_t *curve,
            sike_public_key_t *pk);

/**
 * Conversion of public key parameters to the internal affine Montgomery curve parameters.
 *
 * P.x, Q.x. (P-Q).x -> P.x, P.y, Q.x, Q.y, a, b
 *
 * @param p
 * @param pk
 * @param curve
 */
void get_yP_yQ_A_B(const ff_Params *p,
                   const sike_public_key_t *pk,
                   mont_curve_int_t *curve);

#endif //ISOGENY_REF_MONTGOMERY_H
