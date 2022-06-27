//
// Supersingular Isogeny Key Encapsulation Ref. Library
//
// InfoSec Global Inc., 2017-2020
// Basil Hess <basil.hess@infosecglobal.com>
//

/** @file
SIDH key agreement high-level functions:
 Private key generation
 Public key generation
 Shared secret generation
*/

#ifndef ISOGENY_REF_SIDH_H
#define ISOGENY_REF_SIDH_H

#include <fp2.h>
#include <sike_params.h>
#include <sike.h>

/**
 * SIDH private-key generation
 *
 * Private keys are integers in the range:
 * {0,...,ordA - 1} for ALICE
 * {0,...,ordB - 1} for BOB
 *
 * @param params SIKE parameters
 * @param party ALICE or BOB
 * @param sk Private key to be generated
 */
void sidh_sk_keygen(const sike_params_t* params,
                    party_t party,
                    sike_private_key sk);

/**
 * Isogen
 * (SIDH public-key generation)
 *
 * For A:
 * Given a private key m_A, a base curve E_0, generators P_A, Q_A and P_B, Q_B:
 * - generates kernel defining an isogeny: K = P_A + m_A*Q_A
 * - gets isogenous curve E_A
 * - evaluates P_B and Q_B under the isogeny: phi_A(P_B), phi_A(Q_B)
 * - Returns public key as E_A with generators phi_A(P_B), phi_A(Q_B)
 * For B:
 * Given a private key m_B, a base curve E_0, generators P_B, Q_B and P_A, Q_A:
 * - generates kernel defining an isogeny: K = P_B + m_B*Q_B
 * - gets isogenous curve E_B
 * - evaluates P_A and Q_A under the isogeny: phi_B(P_A), phi_B(Q_A)
 * - Returns public key as E_B with generators phi_B(P_A), phi_B(Q_A)
 *
 * @param params SIDH parameters
 * @param pk Public key to be generated
 * @param sk Private key, externally provided
 * @param party `ALICE` or `BOB`
 */
void sidh_isogen(const sike_params_t *params,
                 sike_public_key_t *pk,
                 const sike_private_key sk,
                 party_t party);

/**
 * Isoex
 * (SIDH shared secret generation)
 *
 * For A:
 * Given a private key m_A, and B's public key: curve E_B, generators phi_B(P_A), phi_B(Q_A)
 * - generates kernel defining an isogeny: K = phi_B(P_A) + m_A*phi_B(Q_A)
 * - gets isogenous curve E_AB
 * - Shared secret is the j-invariant of E_AB
 * For B:
 * Given a private key m_B, and A's public key: curve E_A, generators phi_A(P_B), phi_A(Q_B)
 * - generates kernel defining an isogeny: K = phi_A(P_B) + m_B*phi_A(Q_B)
 * - gets isogenous curve E_BA
 * - Shared secret is the j-invariant of E_BA
 *
 * @param params SIDH parameters
 * @param pkO Public key of the other party
 * @param skI Own private key
 * @param party `ALICE` or `BOB`
 * @param secret Shared secret to be generated
 */
void sidh_isoex(const sike_params_t *params,
                const sike_public_key_t *pkO,
                const sike_private_key skI,
                party_t party,
                fp2 *secret);

#endif // ISOGENY_REF_SIDH_H
