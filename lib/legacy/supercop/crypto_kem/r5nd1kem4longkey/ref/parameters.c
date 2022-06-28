/*
 * Copyright (c) 2018, Koninklijke Philips N.V.
 */

/**
 * @file
 * Implementation of the parameters, structure, and functions.
 */

#include "parameters.h"
#include "misc.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*******************************************************************************
 * Private variables & functions
 ******************************************************************************/

/**
 * The algorithm parameters in case the NIST API functions are used.
 */
static parameters api_params;

/**
 * The parameter set number (see file `r5_parameter_sets.h`)
 * that corresponds to the settings as specified by the NIST API (i.e.
 * `CRYPTO_SECRETKEYBYTES`, `CRYPTO_PUBLICKEYBYTES`, `CRYPTO_BYTES`,
 * and `CRYPTO_CIPHERTEXTBYTES`).
 * -1 when the `api_params` have not yet been initialized.
 */
static int api_params_set_number = -1;

/*******************************************************************************
 * Public functions
 ******************************************************************************/

uint32_t get_crypto_secret_key_bytes(const parameters *params, const int is_cca_or_encrypt) {
    return is_cca_or_encrypt ? ((uint32_t) params->kappa_bytes + (uint32_t) params->kappa_bytes + params->pk_size) : params->kappa_bytes;
}

uint32_t get_crypto_public_key_bytes(const parameters *params) {
    return params->pk_size;
}

uint16_t get_crypto_bytes(const parameters *params, const int is_encrypt) {
    return is_encrypt ? (uint16_t) (params->ct_size + params->kappa_bytes + 16) : params->kappa_bytes;
}

uint16_t get_crypto_cipher_text_bytes(const parameters *params, const int is_cca, const int is_encrypt) {
    return is_encrypt ? 0U : (uint16_t) (params->ct_size + (is_cca ? params->kappa_bytes : 0U));
}

uint16_t get_crypto_seed_bytes(const parameters *params) {
    return params->kappa_bytes;
}

parameters *set_parameters_from_api() {
    if (api_params_set_number >= 0) {
        return &api_params;
    }

    api_params_set_number = ROUND5_API_SET;

    /* Set up parameters from set */
    int err;

    /* Algorithm parameters */
    uint8_t kappa_bytes;
    uint16_t d;
    uint16_t n;
    uint16_t h;
    uint8_t q_bits;
    uint8_t p_bits;
    uint8_t t_bits;
    uint16_t n_bar;
    uint16_t m_bar;
    uint8_t b_bits;
    uint8_t f;
    uint8_t xe;

    kappa_bytes = (uint8_t) r5_parameter_sets[api_params_set_number][POS_KAPPA_BYTES];
    d = (uint16_t) r5_parameter_sets[api_params_set_number][POS_D];
    n = (uint16_t) r5_parameter_sets[api_params_set_number][POS_N];
    h = (uint16_t) r5_parameter_sets[api_params_set_number][POS_H];
    q_bits = (uint8_t) r5_parameter_sets[api_params_set_number][POS_Q_BITS];
    p_bits = (uint8_t) r5_parameter_sets[api_params_set_number][POS_P_BITS];
    t_bits = (uint8_t) r5_parameter_sets[api_params_set_number][POS_T_BITS];
    n_bar = (uint16_t) r5_parameter_sets[api_params_set_number][POS_N_BAR];
    m_bar = (uint16_t) r5_parameter_sets[api_params_set_number][POS_M_BAR];
    b_bits = (uint8_t) r5_parameter_sets[api_params_set_number][POS_B_BITS];
    f = (uint8_t) r5_parameter_sets[api_params_set_number][POS_F];
    xe = (uint8_t) r5_parameter_sets[api_params_set_number][POS_XE];

    err = set_parameters(&api_params, ROUND5_API_TAU, ROUND5_API_TAU2_LEN, kappa_bytes, d, n, h, q_bits, p_bits, t_bits, b_bits, n_bar, m_bar, f, xe);


    if (err) {
        api_params_set_number = -1;
        return NULL;
    } else {
        return &api_params;
    }

}

int set_parameters(parameters *params, const uint8_t tau, const uint32_t tau2_len, const uint8_t kappa_bytes, const uint16_t d, const uint16_t n, const uint16_t h, const uint8_t q_bits, const uint8_t p_bits, const uint8_t t_bits, const uint8_t b_bits, const uint16_t n_bar, const uint16_t m_bar, const uint8_t f, const uint8_t xe) {
    params->kappa_bytes = kappa_bytes;
    params->d = d;
    params->n = n;
    params->h = h;
    params->q_bits = q_bits;
    params->p_bits = p_bits;
    params->t_bits = t_bits;
    params->b_bits = b_bits;
    params->n_bar = n_bar;
    params->m_bar = m_bar;
    params->f = f;
    params->xe = xe;

    /* Derived parameters */
    params->kappa = (uint16_t) (8 * kappa_bytes);
    params->k = (uint16_t) (n ? d / n : 0); /* Avoid arithmetic exception if n = 0 */
    params->mu = (uint16_t) (b_bits ? CEIL_DIV((params->kappa + params->xe), b_bits) : 0); /* Avoid arithmetic exception if B = 0 */
    params->q = (uint32_t) (1U << q_bits);
    params->p = (uint16_t) (1U << p_bits);

    /* Message sizes */
    params->pk_size = (uint32_t) (kappa_bytes + BITS_TO_BYTES(d * n_bar * p_bits));
    params->ct_size = (uint16_t) (BITS_TO_BYTES(d * m_bar * p_bits) + BITS_TO_BYTES(params->mu * t_bits));

    /* Rounding constants */
    params->z_bits = (uint16_t) (params->q_bits - params->p_bits + params->t_bits);
    if (params->z_bits < params->p_bits) {
        params->z_bits = params->p_bits;
    }
    params->h1 = (uint16_t) ((uint16_t) 1 << (params->q_bits - params->p_bits - 1));
    params->h2 = (uint16_t) (1 << (params->q_bits - params->z_bits - 1));
    params->h3 = (uint16_t) ((uint16_t) (1 << (params->p_bits - params->t_bits - 1)) + (uint16_t) (1 << (params->p_bits - params->b_bits - 1)) - (uint16_t) (1 << (params->q_bits - params->z_bits - 1)));

    /* n must be either d or 1 and both must be > 0 */
    assert(params->n != 0 && params->d != 0 && (params->n == params->d || params->n == 1));
    /* Hamming weight must be even, > 0, and < d */
    assert(params->h != 0 && params->h <= params->d && !(params->h & 1));
    /* p, q, and t must be > 0 and power of 2 */
    /* p must be < q */
    /* t must be < p */
    assert(params->q_bits > 0 && params->p_bits > 0 && params->t_bits > 0 && params->p_bits < params->q_bits && params->t_bits < params->p_bits);
    /* Dimensions must be > 0 */
    assert(params->n_bar > 0 && params->m_bar > 0);
    /* b must be > 0, < p */
    assert(params->b_bits > 0 && params->b_bits < params->p_bits);
    /* Seed size must be > 0 */
    assert(params->kappa_bytes > 0);

    /* tau */
    set_parameter_tau(params, tau);

    /* tau2 length */
    set_parameter_tau2_len(params, tau2_len);

    return 0;
}

void set_parameter_tau(parameters *params, const uint8_t tau) {
    params->tau = params->k == 1 ? 0 : tau;

    /* tau must be 0, 1, or 2 for non-ring, 0 for ring (but this is actually already enforced) */
    assert(params->tau <= 2 && (params->k != 1 || params->tau == 0));
}

void set_parameter_tau2_len(parameters *params, const uint32_t tau2_len) {
    if (tau2_len == 0) {
        params->tau2_len = 1 << 11;
    } else {
        params->tau2_len = tau2_len;
    }

    /* For non-ring, tau2_len must be a power of two and larger than or equal to d */
    assert(params->k == 1 || (params->tau2_len >= params->d && (params->tau2_len & (params->tau2_len - 1)) == 0));
}
