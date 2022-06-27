/*
 * Copyright (c) 2018, Koninklijke Philips N.V.
 */

/**
 * @file
 * Declaration of the CPA KEM functions.
 */

#ifndef R5_CPA_KEM_H
#define R5_CPA_KEM_H

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Generates a CPA KEM key pair. Uses the parameters as specified.
     *
     * @param[out] pk     public key
     * @param[out] sk     secret key
     * @return __0__ in case of success
     */
    int r5_cpa_kem_keygen(unsigned char *pk, unsigned char *sk);

    /**
     * CPA KEM encapsulate. Uses the parameters as specified.
     *
     * @param[out] ct     key encapsulation message
     * @param[out] k      shared secret
     * @return __0__ in case of success
     */
    int r5_cpa_kem_encapsulate(unsigned char *ct, unsigned char *k, const unsigned char *pk);

    /**
     * CPA KEM de-capsulate. Uses the parameters as specified.
     *
     * @param[out] k      shared secret
     * @param[in]  ct     key encapsulation message
     * @param[in]  sk     secret key with which the message is to be de-capsulated
     * @return __0__ in case of success
     */
    int r5_cpa_kem_decapsulate(unsigned char *k, const unsigned char *ct, const unsigned char *sk);

#ifdef __cplusplus
}
#endif

#endif /* R5_CPA_KEM_H */
