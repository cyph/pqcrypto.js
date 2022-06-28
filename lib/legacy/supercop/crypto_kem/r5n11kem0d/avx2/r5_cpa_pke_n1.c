/*
 * Copyright (c) 2018, Koninklijke Philips N.V.
 */

#include "r5_cpa_pke.h"
#include "r5_parameter_sets.h"

#if PARAMS_K != 1

#include "r5_hash.h"
#include "rng.h"
#include "xef.h"
#include "matmul.h"
#include "misc.h"
#include "a_random.h"

#include <stdio.h>
#include <string.h>

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

/* Wrapper around xef functions so we can seamlessly make use of the optimized xe5 */
#if PARAMS_F == 5
#if PARAMS_XE == 190
#define XEF(function, block, len, f) xe5_190_##function(block)
#elif PARAMS_XE == 218
#define XEF(function, block, len, f) xe5_218_##function(block)
#elif PARAMS_XE == 234
#define XEF(function, block, len, f) xe5_234_##function(block)
#endif
#elif PARAMS_F == 4 && PARAMS_XE == 163
#define XEF(function, block, len, f) xe4_163_##function(block)
#elif PARAMS_F == 2 && PARAMS_XE == 53
#define XEF(function, block, len, f) xe2_53_##function(block)
#else
#define XEF(function, block, len, f) xef_##function(block, len, f)
#endif
#define xef_compute(block, len, f) XEF(compute, block, len, f)
#define xef_fixerr(block, len, f) XEF(fixerr, block, len, f)

#endif

#if PARAMS_TAU != 0
#include "little_endian.h"
#include "drbg.h"

/**
 * The DRBG customization when creating the tau=1 or tau=2 permutations.
 */
static const uint8_t permutation_customization[2] = {0, 1};
#endif

#if PARAMS_TAU == 1

#include "a_fixed.h"

static int create_A_permutation(uint32_t A_permutation[PARAMS_D], const unsigned char *sigma) {
    /* Compute the permutation */
    uint16_t rnd;
    drbg_init_customization(sigma, permutation_customization, sizeof (permutation_customization));
    for (uint32_t i = 0; i < PARAMS_D; ++i) {
        do {
            drbg16_customization(rnd);
        } while (rnd >= PARAMS_RS_LIM);
        rnd = (uint16_t) (rnd / PARAMS_RS_DIV);
        A_permutation[i] = 2 * i * PARAMS_D + rnd;
    }

    return 0;
}

#elif PARAMS_TAU == 2

static int create_A_permutation(uint16_t A_permutation[PARAMS_D], const unsigned char *sigma) {
    /* Compute the permutation */
    uint16_t rnd;
    uint32_t i;
    uint8_t v[PARAMS_TAU2_LEN] = {0};

    drbg_init_customization(sigma, permutation_customization, sizeof (permutation_customization));

    for (i = 0; i < PARAMS_D; ++i) {
        do {
            drbg16_customization(rnd);
            rnd = (uint16_t) (rnd & (PARAMS_TAU2_LEN - 1));
        } while (v[rnd]);
        v[rnd] = 1;
        A_permutation[i] = rnd;
    }

    return 0;
}

#endif

// compress D*M_BAR elements of q bits into p bits and pack into a byte string

static void pack_q_p_m_bar(uint8_t *pv, const modq_t *vq, const modq_t rounding_constant) {
#if (PARAMS_P_BITS == 8)
    size_t i;

    for (i = 0; i < PARAMS_D * PARAMS_M_BAR; ++i) {
        pv[i] = ((vq[i] + rounding_constant) >> (PARAMS_Q_BITS - PARAMS_P_BITS)) & (PARAMS_P - 1);
    }
#else
    size_t i, j;
    modp_t t;

    memset(pv, 0, (size_t) BITS_TO_BYTES(PARAMS_P_BITS * PARAMS_D * PARAMS_M_BAR));
    j = 0;
    for (i = 0; i < PARAMS_D * PARAMS_M_BAR; ++i) {
        t = ((vq[i] + rounding_constant) >> (PARAMS_Q_BITS - PARAMS_P_BITS)) & (PARAMS_P - 1);
        //pack p bits
        pv[j >> 3] |= (uint8_t) (t << (j & 7));
        if ((j & 7) + PARAMS_P_BITS > 8) {
            pv[(j >> 3) + 1] |= (uint8_t) (t >> (8 - (j & 7)));
            if ((j & 7) + PARAMS_P_BITS > 16) {
                pv[(j >> 3) + 2] |= (uint8_t) (t >> (16 - (j & 7)));
            }
        }
        j += PARAMS_P_BITS;
    }
#endif
}

// compress D*N_BAR elements of q bits into p bits and pack into a byte string

static void pack_q_p_n_bar(uint8_t *pv, const modq_t *vq, const modq_t rounding_constant) {
#if (PARAMS_P_BITS == 8)
    size_t i;

    for (i = 0; i < PARAMS_D * PARAMS_N_BAR; ++i) {
        pv[i] = ((vq[i] + rounding_constant) >> (PARAMS_Q_BITS - PARAMS_P_BITS)) & (PARAMS_P - 1);
    }
#else
    size_t i, j;
    modp_t t;

    memset(pv, 0, (size_t) BITS_TO_BYTES(PARAMS_P_BITS * PARAMS_D * PARAMS_N_BAR));
    j = 0;
    for (i = 0; i < PARAMS_D * PARAMS_N_BAR; ++i) {
        t = ((vq[i] + rounding_constant) >> (PARAMS_Q_BITS - PARAMS_P_BITS)) & (PARAMS_P - 1);
        //pack p bits
        pv[j >> 3] |= (uint8_t) (t << (j & 7));
        if ((j & 7) + PARAMS_P_BITS > 8) {
            pv[(j >> 3) + 1] |= (uint8_t) (t >> (8 - (j & 7)));
            if ((j & 7) + PARAMS_P_BITS > 16) {
                pv[(j >> 3) + 2] |= (uint8_t) (t >> (16 - (j & 7)));
            }
        }
        j += PARAMS_P_BITS;
    }
#endif
}

// unpack a byte string into D*M_BAR elements of p bits

static void unpack_p_m_bar(modp_t *vp, const uint8_t *pv) {
#if (PARAMS_P_BITS == 8)
    memcpy(vp, pv, PARAMS_D * PARAMS_M_BAR);
#else
    size_t i, bits_done, idx, bit_idx;
    modp_t val;

    bits_done = 0;
    for (i = 0; i < PARAMS_D * PARAMS_M_BAR; i++) {
        idx = bits_done >> 3;
        bit_idx = bits_done & 7;
        val = (uint16_t) (pv[idx] >> bit_idx);
        if (bit_idx + PARAMS_P_BITS > 8) {
            /* Get spill over from next packed byte */
            val = (uint16_t) (val | (pv[idx + 1] << (8 - bit_idx)));
            if (bit_idx + PARAMS_P_BITS > 16) {
                /* Get spill over from next packed byte */
                val = (uint16_t) (val | (pv[idx + 2] << (16 - bit_idx)));
            }
        }
        vp[i] = val & (PARAMS_P - 1);
        bits_done += PARAMS_P_BITS;
    }
#endif
}

// unpack a byte string into D*N_BAR elements of p bits

static void unpack_p_n_bar(modp_t *vp, const uint8_t *pv) {
#if (PARAMS_P_BITS == 8)
    memcpy(vp, pv, PARAMS_D * PARAMS_N_BAR);
#else
    size_t i, bits_done, idx, bit_idx;
    modp_t val;

    bits_done = 0;
    for (i = 0; i < PARAMS_D * PARAMS_N_BAR; i++) {
        idx = bits_done >> 3;
        bit_idx = bits_done & 7;
        val = (uint16_t) (pv[idx] >> bit_idx);
        if (bit_idx + PARAMS_P_BITS > 8) {
            /* Get spill over from next packed byte */
            val = (uint16_t) (val | (pv[idx + 1] << (8 - bit_idx)));
            if (bit_idx + PARAMS_P_BITS > 16) {
                /* Get spill over from next packed byte */
                val = (uint16_t) (val | (pv[idx + 2] << (16 - bit_idx)));
            }
        }
        vp[i] = val & (PARAMS_P - 1);
        bits_done += PARAMS_P_BITS;
    }
#endif
}

// generate a keypair (sigma, B)

int r5_cpa_pke_keygen(uint8_t *pk, uint8_t *sk) {
    modq_t B[PARAMS_D][PARAMS_N_BAR];
#ifdef CM_CACHE
    int16_t S_T[PARAMS_N_BAR][PARAMS_D];
#else
    uint16_t S_T[PARAMS_N_BAR][PARAMS_H / 2][2];
#endif

    randombytes(pk, PARAMS_KAPPA_BYTES); // sigma = seed of (permutation of) A
#ifdef NIST_KAT_GENERATION
    printf("r5_cpa_pke_keygen: tau=%u\n", PARAMS_TAU);
    print_hex("r5_cpa_pke_keygen: sigma", pk, PARAMS_KAPPA_BYTES, 1);
#endif


#if PARAMS_TAU == 0
    modq_t A_random[PARAMS_D][PARAMS_D];
    // A from sigma
    create_A_random((modq_t *) A_random, pk);
#define A_matrix A_random
#define A_element(r,c) A_random[r][c]
#elif PARAMS_TAU == 1
    uint32_t A_permutation[PARAMS_D];
    // Permutation of A_fixed
    create_A_permutation(A_permutation, pk);
#define A_matrix A_fixed
#define A_element(r,c) A_fixed[A_permutation[r] + (uint32_t) c]
#elif PARAMS_TAU == 2
    modq_t A_random[PARAMS_TAU2_LEN + PARAMS_D];
    // A from sigma
    create_A_random(A_random, pk);
    memcpy(A_random + PARAMS_TAU2_LEN, A_random, PARAMS_D * sizeof (modq_t));
    uint16_t A_permutation[PARAMS_D];
    // Permutation of A_random
    create_A_permutation(A_permutation, pk);
#define A_matrix A_random
#define A_element(r,c) A_random[A_permutation[r] + (uint32_t) c]
#endif

    randombytes(sk, PARAMS_KAPPA_BYTES); // secret key -- Random S
    create_secret_matrix_s_t(S_T, sk);

#if PARAMS_TAU == 0
    matmul_as_q(B, A_matrix, S_T); // B = A * S
#else
    matmul_as_q(B, A_matrix, A_permutation, S_T); // B = A * S
#endif


    // Compress B q_bits -> p_bits, pk = sigma | B
    pack_q_p_n_bar(pk + PARAMS_KAPPA_BYTES, &B[0][0], PARAMS_H1);
    return 0;
}

int r5_cpa_pke_encrypt(uint8_t *ct, const uint8_t *pk, const uint8_t *m, const uint8_t *rho) {
    size_t i, j;
#ifdef CM_CACHE
    int16_t R_T[PARAMS_M_BAR][PARAMS_D];
#else
    uint16_t R_T[PARAMS_M_BAR][PARAMS_H / 2][2];
#endif
    modq_t U_T[PARAMS_M_BAR][PARAMS_D];
    modp_t B[PARAMS_D][PARAMS_N_BAR];
    modp_t X[PARAMS_MU];
    uint8_t m1[BITS_TO_BYTES(PARAMS_MU * PARAMS_B_BITS)];
    modp_t t, tm;

    // unpack public key
    unpack_p_n_bar(&B[0][0], pk + PARAMS_KAPPA_BYTES);

#undef A_matrix
#undef A_element
#if PARAMS_TAU == 0
    modq_t A_random[PARAMS_D][PARAMS_K];
    // A from sigma
    create_A_random((modq_t *) A_random, pk);
#define A_matrix A_random
#define A_element(r,c) A_random[r][c]
#elif PARAMS_TAU == 1
    uint32_t A_permutation[PARAMS_D];
    // Permutation of A_fixed
    create_A_permutation(A_permutation, pk);
#define A_matrix A_fixed
#define A_element(r,c) A_fixed[A_permutation[r] + (uint32_t) c]
#elif PARAMS_TAU == 2
    modq_t A_random[PARAMS_TAU2_LEN + PARAMS_D];
    // A from sigma
    create_A_random(A_random, pk);
    memcpy(A_random + PARAMS_TAU2_LEN, A_random, PARAMS_D * sizeof (modq_t));
    uint16_t A_permutation[PARAMS_D];
    // Permutation of A_random
    create_A_permutation(A_permutation, pk);
#define A_matrix A_random
#define A_element(r,c) A_random[A_permutation[r] + (uint32_t) c]
#endif

    memcpy(m1, m, PARAMS_KAPPA_BYTES);
    memset(m1 + PARAMS_KAPPA_BYTES, 0, BITS_TO_BYTES(PARAMS_MU * PARAMS_B_BITS) - PARAMS_KAPPA_BYTES);
#if (PARAMS_XE != 0)
    xef_compute(m1, PARAMS_KAPPA_BYTES, PARAMS_F);
#endif

    // Create R
    create_secret_matrix_r_t(R_T, rho);

#if PARAMS_TAU == 0
    matmul_rta_q(U_T, A_matrix, R_T); // U^T = (R^T x A)^T   (mod q)
#else
    matmul_rta_q(U_T, A_matrix, A_permutation, R_T); // U^T = (R^T x A)^T   (mod q)
#endif
    matmul_btr_p(X, B, R_T); // X = R^T x B   (mod p)

#ifdef NIST_KAT_GENERATION
    print_hex("r5_cpa_pke_encrypt: rho", rho, PARAMS_KAPPA_BYTES, 1);
    print_hex("r5_cpa_pke_encrypt: sigma", pk, PARAMS_KAPPA_BYTES, 1);
    print_hex("r5_cpa_pke_encrypt: m1", m1, BITS_TO_BYTES(PARAMS_MU * PARAMS_B_BITS), 1);
#endif

    pack_q_p_m_bar(ct, &U_T[0][0], PARAMS_H2); // ct = U^T | v

    memset(ct + PARAMS_DPU_SIZE, 0, PARAMS_MUT_SIZE);
    j = 8 * PARAMS_DPU_SIZE;

    for (i = 0; i < PARAMS_MU; i++) { // compute, pack v
        // compress p->t
        t = (modp_t) ((X[i] + PARAMS_H2) >> (PARAMS_P_BITS - PARAMS_T_BITS));
        // add message
        tm = (modp_t) (m1[(i * PARAMS_B_BITS) >> 3] >> ((i * PARAMS_B_BITS) & 7));
#if (8 % PARAMS_B_BITS != 0)
        if (((i * PARAMS_B_BITS) & 7) + PARAMS_B_BITS > 8) {
            /* Get spill over from next message byte */
            tm = (modp_t) (tm | (m1[((i * PARAMS_B_BITS) >> 3) + 1] << (8 - ((i * PARAMS_B_BITS) & 7))));
        }
#endif
        t = (modp_t) (t + ((tm & ((1 << PARAMS_B_BITS) - 1)) << (PARAMS_T_BITS - PARAMS_B_BITS))) & ((1 << PARAMS_T_BITS) - 1);

        ct[j >> 3] |= (uint8_t) (t << (j & 7)); // pack t bits
        if ((j & 7) + PARAMS_T_BITS > 8) {
            ct[(j >> 3) + 1] |= (uint8_t) (t >> (8 - (j & 7)));
            if ((j & 7) + PARAMS_T_BITS > 16) {
                ct[(j >> 3) + 2] |= (uint8_t) (t >> (16 - (j & 7)));
            }
        }
        j += PARAMS_T_BITS;
    }

    return 0;
}

int r5_cpa_pke_decrypt(uint8_t *m, const uint8_t *sk, const uint8_t *ct) {
    size_t i, j;
#ifdef CM_CACHE
    int16_t S_T[PARAMS_N_BAR][PARAMS_D];
#else
    uint16_t S_T[PARAMS_N_BAR][PARAMS_H / 2][2];
#endif
    modp_t U_T[PARAMS_M_BAR][PARAMS_D];
    modp_t v[PARAMS_MU];
    modp_t t, X_prime[PARAMS_MU];
    uint8_t m1[BITS_TO_BYTES(PARAMS_MU * PARAMS_B_BITS)];

    create_secret_matrix_s_t(S_T, sk);

    unpack_p_m_bar((modp_t *) U_T, ct); // ct = U^T | v

    j = 8 * PARAMS_DPU_SIZE;
    for (i = 0; i < PARAMS_MU; i++) {
        t = (modp_t) (ct[j >> 3] >> (j & 7)); // unpack t bits
        if ((j & 7) + PARAMS_T_BITS > 8) {
            t |= (modp_t) (ct[(j >> 3) + 1] << (8 - (j & 7)));
            if ((j & 7) + PARAMS_T_BITS > 16) {
                t |= (modp_t) ((ct[(j >> 3) + 2]) << (16 - (j & 7)));
            }
        }
        v[i] = t & ((1 << PARAMS_T_BITS) - 1);
        j += PARAMS_T_BITS;
    }


    // X' = S^T * U (mod p)
    matmul_stu_p(X_prime, U_T, S_T);


    // X' = v - X', compressed to 1 bit
    modp_t x_p;
    memset(m1, 0, sizeof (m1));
    for (i = 0; i < PARAMS_MU; i++) {
        // v - X' as mod q value (to be able to perform the rounding!)
        x_p = (modp_t) ((v[i] << (PARAMS_P_BITS - PARAMS_T_BITS)) - X_prime[i]);
        x_p = (modp_t) (((x_p + PARAMS_H3) >> (PARAMS_P_BITS - PARAMS_B_BITS)) & ((1 << PARAMS_B_BITS) - 1));
        m1[i * PARAMS_B_BITS >> 3] = (uint8_t) (m1[i * PARAMS_B_BITS >> 3] | (x_p << ((i * PARAMS_B_BITS) & 7)));
#if (8 % PARAMS_B_BITS != 0)
        if (((i * PARAMS_B_BITS) & 7) + PARAMS_B_BITS > 8) {
            /* Spill over to next message byte */
            m1[(i * PARAMS_B_BITS >> 3) + 1] = (uint8_t) (m1[(i * PARAMS_B_BITS >> 3) + 1] | (x_p >> (8 - ((i * PARAMS_B_BITS) & 7))));
        }
#endif
    }


#if (PARAMS_XE != 0)
    // Apply error correction
    xef_compute(m1, PARAMS_KAPPA_BYTES, PARAMS_F);
    xef_fixerr(m1, PARAMS_KAPPA_BYTES, PARAMS_F);
#endif
    memcpy(m, m1, PARAMS_KAPPA_BYTES);

#ifdef NIST_KAT_GENERATION
    print_hex("r5_cpa_pke_decrypt: m", m, PARAMS_KAPPA_BYTES, 1);
#endif

    return 0;
}

#endif /* PARAMS_K != 1 */
