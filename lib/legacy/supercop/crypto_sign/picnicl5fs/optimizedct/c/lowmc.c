/*
 *  This file is part of the optimized implementation of the Picnic signature scheme.
 *  See the accompanying documentation for complete details.
 *
 *  The code is provided under the MIT license, see LICENSE for
 *  more details.
 *  SPDX-License-Identifier: MIT
 */


#include "io.h"
#include "lowmc.h"
#include "mzd_additional.h"

#if !defined(_MSC_VER)
#include <stdalign.h>
#endif
#include <string.h>
#include <assert.h>

#include "lowmc_256_256_38.h"

// clang-format off
// clang-format on
/* S-box for m = 10 */
static inline uint64_t sbox_layer_10_bitsliced_uint64(uint64_t in) {
  // a, b, c
  const uint64_t x0s = (in & MASK_X0I) << 2;
  const uint64_t x1s = (in & MASK_X1I) << 1;
  const uint64_t x2m = in & MASK_X2I;

  // (b & c) ^ a
  const uint64_t t0 = (x1s & x2m) ^ x0s;
  // (c & a) ^ a ^ b
  const uint64_t t1 = (x0s & x2m) ^ x0s ^ x1s;
  // (a & b) ^ a ^ b ^c
  const uint64_t t2 = (x0s & x1s) ^ x0s ^ x1s ^ x2m;

  return (in & MASK_MASK) ^ (t0 >> 2) ^ (t1 >> 1) ^ t2;
}

/* S-box for m = 10 */
static inline void sbox_layer_10_uint64(uint64_t* d) {
  *d = sbox_layer_10_bitsliced_uint64(*d);
}

#if !defined(NO_UINT64_FALLBACK)


#endif /* NO_UINT_FALLBACK */



#if !defined(NO_UINT64_FALLBACK)
// uint64 based implementation
#define IMPL uint64

#include "lowmc_129_129_4_fns_uint64.h"
#include "lowmc.c.i"

#include "lowmc_192_192_4_fns_uint64.h"
#include "lowmc.c.i"

#include "lowmc_255_255_4_fns_uint64.h"
#include "lowmc.c.i"

#include "lowmc_128_128_20_fns_uint64.h"
#include "lowmc.c.i"

#include "lowmc_192_192_30_fns_uint64.h"
#include "lowmc.c.i"

#include "lowmc_256_256_38_fns_uint64.h"
#include "lowmc.c.i"
#endif


void lowmc_compute(const lowmc_parameters_t* lowmc, const lowmc_key_t* key, const mzd_local_t* x,
                   mzd_local_t* y) {
  const uint32_t lowmc_id = LOWMC_GET_ID(lowmc);

#if !defined(NO_UINT64_FALLBACK)
  /* uint64_t implementations */
  switch (lowmc_id) {
    /* Instances with partial Sbox layer */
  case LOWMC_ID(256, 10):
    lowmc_uint64_lowmc_256_256_38(key, x, y);
    return;
    /* Instances with full Sbox layer */
  }
#endif

  UNREACHABLE;
}


void lowmc_record_state(const lowmc_parameters_t* lowmc, const lowmc_key_t* key,
                        const mzd_local_t* x, recorded_state_t* state) {
  const uint32_t lowmc_id = LOWMC_GET_ID(lowmc);

#if !defined(NO_UINT64_FALLBACK)
  /* uint64_t implementations */
  switch (lowmc_id) {
    /* Instances with partial Sbox layer */
  case LOWMC_ID(256, 10):
    lowmc_store_uint64_lowmc_256_256_38(key, x, state);
    return;
    /* Instances with full Sbox layer */
  }
#endif

  UNREACHABLE;
}

