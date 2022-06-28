/*
 * Copyright (c) 2018, Koninklijke Philips N.V.
 */

/**
 * @file
 * Declaration of the DEM functions used by the Round5 CCA KEM-based encrypt algorithm.
 */

#ifndef PST_DEM_H
#define PST_DEM_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Applies a DEM to the given message using the specified key.
     *
     * @param[out] c2     the encapsulated message
     * @param[out] c2_len the length of the encapsulated message (`m_len` + 16 bytes)
     * @param[in]  key    the key to use for the encapsulation
     * @param[in]  key_len the length of the key
     * @param[in]  m      the message to encapsulate
     * @param[in]  m_len  the length of the message
     * @return __0__ in case of success
     */
    int round5_dem(unsigned char *c2, unsigned long long *c2_len, const unsigned char *key, const uint8_t key_len, const unsigned char *m, const unsigned long long m_len);

    /**
     * Inverses the application of a DEM to a message.
     *
     * @param[out] m       the original message
     * @param[out] m_len   the length of the decapsulated message (`c2_len` - 16)
     * @param[in]  key     the key to use for the encapsulation
     * @param[in]  key_len the length of the key
     * @param[in]  c2      the encapsulated message
     * @param[in]  c2_len  the length of the encapsulated message
     * @return __0__ in case of success
     */
    int round5_dem_inverse(unsigned char *m, unsigned long long *m_len, const unsigned char *key, const uint8_t key_len, const unsigned char *c2, const unsigned long long c2_len);

#ifdef __cplusplus
}
#endif

#endif /* PST_DEM_H */
