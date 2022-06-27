/*
 * Copyright (c) 2018, Koninklijke Philips N.V.
 */

/**
 * @file
 * Implementation of the various pack and unpack functions.
 */

#include "pack.h"

#include <string.h>
#include "misc.h"

/*******************************************************************************
 * Public functions
 ******************************************************************************/

size_t pack(unsigned char *packed, const uint16_t *m, const size_t els, const uint8_t nr_bits) {
    const size_t packed_len = (size_t) (BITS_TO_BYTES(els * nr_bits));
    const uint16_t mask = (uint16_t) ((1 << nr_bits) - 1);
    size_t i;
    uint16_t val;
    size_t bits_done = 0;
    size_t idx;
    size_t bit_idx;

    memset(packed, 0, packed_len);
    if (nr_bits == 8) {
        for (i = 0; i < els; ++i) {
            packed[i] = (uint8_t) m[i];
        }
    } else {
        for (i = 0; i < els; ++i) {
            idx = bits_done >> 3;
            bit_idx = bits_done & 7;
            val = m[i] & mask;
            packed[idx] = (uint8_t) (packed[idx] | (val << bit_idx));
            if (bit_idx + nr_bits > 8) {
                /* Spill over to next packed byte */
                packed[idx + 1] = (uint8_t) (packed[idx + 1] | (val >> (8 - bit_idx)));
                if (bit_idx + nr_bits > 16) {
                    /* Spill over to next packed byte */
                    packed[idx + 2] = (uint8_t) (packed[idx + 2] | (val >> (16 - bit_idx)));
                }
            }
            bits_done += nr_bits;
        }
    }

    return packed_len;
}

size_t unpack(uint16_t *m, const unsigned char *packed, const size_t els, const uint8_t nr_bits) {
    const size_t unpacked_len = (size_t) (BITS_TO_BYTES(els * nr_bits));
    size_t i;
    uint16_t val;
    size_t bits_done = 0;
    size_t idx;
    size_t bit_idx;
    uint16_t bitmask = (uint16_t) ((1 << nr_bits) - 1);

    if (nr_bits == 8) {
        for (i = 0; i < els; ++i) {
            m[i] = packed[i];
        }
    } else {
        for (i = 0; i < els; ++i) {
            idx = bits_done >> 3;
            bit_idx = bits_done & 7;
            val = (uint16_t) (packed[idx] >> bit_idx);
            if (bit_idx + nr_bits > 8) {
                /* Get spill over from next packed byte */
                val = (uint16_t) (val | (packed[idx + 1] << (8 - bit_idx)));
                if (bit_idx + nr_bits > 16) {
                    /* Get spill over from next packed byte */
                    val = (uint16_t) (val | (packed[idx + 2] << (16 - bit_idx)));
                }
            }
            m[i] = val & bitmask;
            bits_done += nr_bits;
        }
    }

    return unpacked_len;
}

size_t pack_pk(unsigned char *packed_pk, const unsigned char *sigma, size_t sigma_len, const uint16_t *B, size_t elements, uint8_t nr_bits) {
    size_t packed_idx = 0;

    /* Pack sigma */
    memcpy(packed_pk + packed_idx, sigma, sigma_len);
    packed_idx += sigma_len;
    /* Pack B */
    packed_idx += pack((packed_pk + packed_idx), B, elements, nr_bits);

    return packed_idx;
}

size_t unpack_pk(unsigned char *sigma, uint16_t *B, const unsigned char *packed_pk, size_t sigma_len, size_t elements, uint8_t nr_bits) {
    size_t unpacked_idx = 0;

    /* Unpack sigma */
    memcpy(sigma, packed_pk + unpacked_idx, sigma_len);
    unpacked_idx += sigma_len;
    /* Unpack B */
    unpacked_idx += unpack(B, packed_pk + unpacked_idx, elements, nr_bits);

    return unpacked_idx;
}

size_t pack_ct(unsigned char *packed_ct, const uint16_t *U_T, size_t U_els, uint8_t U_bits, const uint16_t *v, size_t v_els, uint8_t v_bits) {
    size_t idx = 0;

    /* Pack U_T */
    idx += pack(packed_ct, U_T, U_els, U_bits);
    /* Pack v */
    idx += pack((packed_ct + idx), v, v_els, v_bits);

    return idx;
}

size_t unpack_ct(uint16_t *U_T, uint16_t *v, const unsigned char *packed_ct, const size_t U_els, const uint8_t U_bits, const size_t v_els, const uint8_t v_bits) {
    size_t idx = 0;

    /* Unpack U */
    idx += unpack(U_T, packed_ct, U_els, U_bits);
    /* Unpack v */
    idx += unpack(v, (packed_ct + idx), v_els, v_bits);

    return idx;
}
