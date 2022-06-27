//
// Supersingular Isogeny Key Encapsulation Ref. Library
//
// InfoSec Global Inc., 2017-2020
// Basil Hess <basil.hess@infosecglobal.com>
//

/** @file
Algorithms for computing and evaluating isogenies
*/

#ifndef ISOGENY_REF_ISOGENY_H
#define ISOGENY_REF_ISOGENY_H

#include <montgomery.h>

/**
 * Evaluating a 2-isogeny at a point. Affine version.
 *
 * @param p Finite field parameters
 * @param P2 Point with exact order 2
 * @param P Point to be lifted to the 2-isogeny
 * @param isoP Point lifted to the 2-isogeny
 */
void eval_2_iso(const ff_Params *p, const mont_pt_t *P2, const mont_pt_t *P, mont_pt_t *isoP);

/**
 * Computing the 2-isogenous curve. Affine version.
 *
 * @param p Finite field parameters
 * @param P2 Point with exact order 2 on E
 * @param E Curve to be lifted to the 2-isogeny
 * @param isoE 2-isogenous curve to E
 */
void curve_2_iso(const ff_Params *p, const mont_pt_t *P2, const mont_curve_int_t *E, mont_curve_int_t *isoE);

/**
 * Evaluating a 3-isogeny at a point. Affine version.
 *
 * @param p Finite field parameters
 * @param P3 Point with exact order 3
 * @param P Point to be lifted to the 3-isogeny
 * @param isoP Point lifted to the 3-isogeny
 */
void eval_3_iso(const ff_Params *p, const mont_pt_t *P3, const mont_pt_t *P, mont_pt_t *isoP);

/**
 * Computing the 3-isogenous curve. Affine version.
 *
 * @param p Finite field parameters
 * @param P3 Point with exact order 3 on E
 * @param E Curve to be lifted to the 3-isogeny
 * @param isoE 3-isogenous curve to E
 */
void curve_3_iso(const ff_Params *p, const mont_pt_t *P3, const mont_curve_int_t *E, mont_curve_int_t *isoE);

/**
 * Evaluating a 4-isogeny at a point. Affine version.
 *
 * @param p Finite field parameters
 * @param P4 Point with exact order 4
 * @param P Point to be lifted to the 4-isogeny
 * @param isoP Point lifted to the 4-isogeny
 */
void eval_4_iso(const ff_Params *p, const mont_pt_t *P4, const mont_pt_t *P, mont_pt_t *isoP);

/**
 * Computing the 4-isogenous curve. Affine version.
 *
 * @param p Finite field parameters
 * @param P4 Point with exact order 4 on E
 * @param E Curve to be lifted to the 4-isogeny
 * @param isoE 4-isogenous curve to E
 */
void curve_4_iso(const ff_Params *p, const mont_pt_t *P4, const mont_curve_int_t *E, mont_curve_int_t *isoE);

/**
 * Computing and evaluating a 2^e-isogeny, simple version
 * @param p Finite field parameters
 * @param e e2 from public parameters
 * @param E E_{a,b}
 * @param S S with exact order 2^e2 on E. May be modified internally.
 * @param P1 P1 on E (may be NULL)
 * @param P2 P2 on E (may be NULL)
 * @param isoE E_{a',b')
 * @param isoP1 P1 lifted to isoE
 * @param isoP2 P2 lifted to isoE
 */
void iso_2_e(const ff_Params *p,
             int e,
             const mont_curve_int_t *E, mont_pt_t *S,
             const mont_pt_t *P1, const mont_pt_t *P2,
                   mont_curve_int_t *isoE,
                   mont_pt_t *isoP1,    mont_pt_t *isoP2);

/**
 * Computing and evaluating a 3^e-isogeny, simple version
 * @param p Finite field parameters
 * @param e e3 from public parameters
 * @param E E_{a,b}
 * @param S S with exact order 2^e3 on E. May be modified internally.
 * @param P1 P1 on E (may be NULL)
 * @param P2 P2 on E (may be NULL)
 * @param isoE E_{a',b')
 * @param isoP1 P1 lifted to isoE
 * @param isoP2 P2 lifted to isoE
 */
void iso_3_e(const ff_Params *p,
             int e,
             const mont_curve_int_t *E, mont_pt_t *S,
             const mont_pt_t *P1, const mont_pt_t *P2,
             mont_curve_int_t *isoE,
             mont_pt_t *isoP1,    mont_pt_t *isoP2);

#endif // ISOGENY_REF_ISOGENY_H