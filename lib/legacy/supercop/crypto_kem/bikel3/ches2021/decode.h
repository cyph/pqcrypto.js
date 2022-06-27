/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0"
 *
 * Originally written by Nir Drucker, Shay Gueron and Dusan Kostic,
 * AWS Cryptographic Algorithms Group.
 *
 * Modified by Ming-Shing Chen and Tung Chou
 */

#pragma once

#if defined(CRYPTO_NAMESPACE)
#define decode  CRYPTO_NAMESPACE(decode)
#define rotate_right  CRYPTO_NAMESPACE(rotate_right)
#endif

#include "types.h"

ret_t decode(OUT e_t *e, IN const ct_t *ct, IN const sk_t *sk);

// Rotate right the first R_BITS of a syndrome.
// At input, the syndrome is stored as three R_BITS triplicate.
// (this makes rotation easier to implement)
// For the output: the output syndrome has only one R_BITS rotation, the remaining
// (2 * R_BITS) bits are undefined.
void rotate_right(OUT syndrome_t *out,
                  IN __m256i *in,
                  IN uint32_t bitscount);
