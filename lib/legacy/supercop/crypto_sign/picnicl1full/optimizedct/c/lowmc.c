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

#include "lowmc_129_129_4.h"

// clang-format off

#if !defined(NO_UINT64_FALLBACK)
/**
 * S-box for m = 43
 */
static void sbox_uint64_lowmc_129_129_4(mzd_local_t* in) {
  mzd_local_t x0m[1], x1m[1], x2m[1];
  // a
  mzd_and_uint64_192(x0m, mask_129_129_43_a, in);
  // b
  mzd_and_uint64_192(x1m, mask_129_129_43_b, in);
  // c
  mzd_and_uint64_192(x2m, mask_129_129_43_c, in);

  mzd_shift_left_uint64_192(x0m, x0m, 2);
  mzd_shift_left_uint64_192(x1m, x1m, 1);

  mzd_local_t t0[1], t1[1], t2[1];
  // b & c
  mzd_and_uint64_192(t0, x1m, x2m);
  // c & a
  mzd_and_uint64_192(t1, x0m, x2m);
  // a & b
  mzd_and_uint64_192(t2, x0m, x1m);

  // (b & c) ^ a
  mzd_xor_uint64_192(t0, t0, x0m);

  // (c & a) ^ a ^ b
  mzd_xor_uint64_192(t1, t1, x0m);
  mzd_xor_uint64_192(t1, t1, x1m);

  // (a & b) ^ a ^ b ^c
  mzd_xor_uint64_192(t2, t2, x0m);
  mzd_xor_uint64_192(t2, t2, x1m);
  mzd_xor_uint64_192(t2, t2, x2m);

  mzd_shift_right_uint64_192(t0, t0, 2);
  mzd_shift_right_uint64_192(t1, t1, 1);

  mzd_xor_uint64_192(t2, t2, t1);
  mzd_xor_uint64_192(in, t2, t0);
}


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
    /* Instances with full Sbox layer */
  case LOWMC_ID(129, 43):
    lowmc_uint64_lowmc_129_129_4(key, x, y);
    return;
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
    /* Instances with full Sbox layer */
  case LOWMC_ID(129, 43):
    lowmc_store_uint64_lowmc_129_129_4(key, x, state);
    return;
  }
#endif

  UNREACHABLE;
}

