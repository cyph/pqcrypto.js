/*
 * Copyright (c) 2018, Koninklijke Philips N.V.
 */

/**
 * @file
 * Declaration of the various pack and unpack functions.
 */

#ifndef PACK_H
#define PACK_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Packs the given vector using the specified number of bits per element.
     *
     * @param[out] packed  the buffer for the packed vector
     * @param[in]  m       the vector to pack
     * @param[in]  els     the number of elements
     * @param[in]  nr_bits the number of significant bits value
     * @return the length of the packed vector in bytes
     */
    size_t pack(unsigned char *packed, const uint16_t *m, const size_t els, const uint8_t nr_bits);

    /**
     * Unpacks the given vector using the specified number of bits per element.
     *
     * @param[in]  m       unpacked vector
     * @param[in]  packed  the packed vector
     * @param[in]  els     number of elements
     * @param[in]  nr_bits number of significant bits per element
     * @return total number of packed bytes processed
     */
    size_t unpack(uint16_t *m, const unsigned char *packed, const size_t els, const uint8_t nr_bits);

    /**
     * Packs a public key from its sigma and B components.
     *
     * @param[out] packed_pk the packed key
     * @param[in]  sigma     sigma
     * @param[in]  sigma_len length of sigma
     * @param[in]  B         B
     * @param[in]  elements  number of elements of B
     * @param[in]  nr_bits   number of significant bits per element
     * @return the length of packed pk in bytes
     */
    size_t pack_pk(unsigned char *packed_pk, const unsigned char *sigma, size_t sigma_len, const uint16_t *B, size_t elements, uint8_t nr_bits);

    /**
     * Unpacks a packed public key into its sigma and B components.
     *
     * @param[out] sigma     sigma
     * @param[out] B         B
     * @param[in]  packed_pk packed public key
     * @param[in]  sigma_len length of sigma
     * @param[in]  elements  the number of elements of B
     * @param[in]  nr_bits   the number of significant bits per element
     * @return total unpacked bytes
     */
    size_t unpack_pk(unsigned char *sigma, uint16_t *B, const unsigned char *packed_pk, size_t sigma_len, size_t elements, uint8_t nr_bits);

    /**
     * Packs the given ciphertext
     *
     * @param[out] packed_ct buffer for the packed ciphertext
     * @param[in]  U_T       matrix U^T
     * @param[in]  U_els     elements in U_T
     * @param[in]  U_bits    significant bits per element
     * @param[in]  v         vector v
     * @param[in]  v_els     elements in v
     * @param[in]  v_bits    significant bits per element
     * @return the length of packed ct in bytes
     */
    size_t pack_ct(unsigned char *packed_ct, const uint16_t *U_T, size_t U_els, uint8_t U_bits, const uint16_t *v, size_t v_els, uint8_t v_bits);

    /**
     * Unpacks the given ciphertext into its U and v components.
     *
     * @param[out] U_T       matrix U^T
     * @param[out] v         vector v
     * @param[in]  packed_ct packed ciphertext
     * @param[in]  U_els     elements in U
     * @param[in]  U_bits    significant bits per element
     * @param[in]  v_els     elements in v
     * @param[in]  v_bits    significant bits per element
     * @return total unpacked bytes
     */
    size_t unpack_ct(uint16_t *U_T, uint16_t *v, const unsigned char *packed_ct, const size_t U_els, const uint8_t U_bits, const size_t v_els, const uint8_t v_bits);

#ifdef __cplusplus
}
#endif

#endif /* PACK_H */
