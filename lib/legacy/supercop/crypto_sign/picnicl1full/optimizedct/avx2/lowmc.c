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
#include "simd.h"

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

ATTR_TARGET_S128
static inline void sbox_s128_full(mzd_local_t* in, const word128* mask_a, const word128* mask_b,
                                  const word128* mask_c) {
  word128 x0m[2] ATTR_ALIGNED(alignof(word128)), x1m[2] ATTR_ALIGNED(alignof(word128)),
      x2m[2] ATTR_ALIGNED(alignof(word128));
  mm128_and_256(x0m, CONST_BLOCK(in, 0)->w128, mask_a);
  mm128_and_256(x1m, CONST_BLOCK(in, 0)->w128, mask_b);
  mm128_and_256(x2m, CONST_BLOCK(in, 0)->w128, mask_c);

  mm128_shift_left_256(x0m, x0m, 2);
  mm128_shift_left_256(x1m, x1m, 1);

  word128 t0[2] ATTR_ALIGNED(alignof(word128)), t1[2] ATTR_ALIGNED(alignof(word128)),
      t2[2] ATTR_ALIGNED(alignof(word128));
  mm128_and_256(t0, x1m, x2m);
  mm128_and_256(t1, x0m, x2m);
  mm128_and_256(t2, x0m, x1m);

  mm128_xor_256(t0, t0, x0m);

  mm128_xor_256(x0m, x0m, x1m);
  mm128_xor_256(t1, t1, x0m);

  mm128_xor_256(t2, t2, x0m);
  mm128_xor_256(t2, t2, x2m);

  mm128_shift_right_256(t0, t0, 2);
  mm128_shift_right_256(t1, t1, 1);

  mm128_xor_256(t0, t0, t1);
  mm128_xor_256(in->w128, t0, t2);
}

ATTR_TARGET_S128
static inline void sbox_s128_lowmc_129_129_4(mzd_local_t* in) {
  sbox_s128_full(in, mask_129_129_43_a->w128, mask_129_129_43_b->w128, mask_129_129_43_c->w128);
}



ATTR_TARGET_AVX2
static inline word256 sbox_s256_lowmc_full(const word256 min, const word256 mask_a,
                                           const word256 mask_b, const word256 mask_c) {
  word256 x0m ATTR_ALIGNED(alignof(word256)) = mm256_and(min, mask_a);
  word256 x1m ATTR_ALIGNED(alignof(word256)) = mm256_and(min, mask_b);
  word256 x2m ATTR_ALIGNED(alignof(word256)) = mm256_and(min, mask_c);

  x0m = mm256_rotate_left(x0m, 2);
  x1m = mm256_rotate_left(x1m, 1);

  word256 t0 ATTR_ALIGNED(alignof(word256)) = mm256_and(x1m, x2m);
  word256 t1 ATTR_ALIGNED(alignof(word256)) = mm256_and(x0m, x2m);
  word256 t2 ATTR_ALIGNED(alignof(word256)) = mm256_and(x0m, x1m);

  t0 = mm256_xor(t0, x0m);

  x0m = mm256_xor(x0m, x1m);
  t1  = mm256_xor(t1, x0m);

  t2 = mm256_xor(t2, x0m);
  t2 = mm256_xor(t2, x2m);

  t0 = mm256_rotate_right(t0, 2);
  t1 = mm256_rotate_right(t1, 1);

  return mm256_xor(mm256_xor(t0, t1), t2);
}

ATTR_TARGET_AVX2
static inline void sbox_s256_lowmc_129_129_4(mzd_local_t* in) {
  BLOCK(in, 0)->w256 = sbox_s256_lowmc_full(
      BLOCK(in, 0)->w256, CONST_BLOCK(mask_129_129_43_a, 0)->w256,
      CONST_BLOCK(mask_129_129_43_b, 0)->w256, CONST_BLOCK(mask_129_129_43_c, 0)->w256);
}




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

#define FN_ATTR ATTR_TARGET_S128
#undef IMPL
#define IMPL s128


#include "lowmc_129_129_4_fns_s128.h"
#include "lowmc.c.i"

#include "lowmc_192_192_4_fns_s128.h"
#include "lowmc.c.i"

#include "lowmc_255_255_4_fns_s128.h"
#include "lowmc.c.i"

#include "lowmc_128_128_20_fns_s128.h"
#include "lowmc.c.i"

#include "lowmc_192_192_30_fns_s128.h"
#include "lowmc.c.i"

#include "lowmc_256_256_38_fns_s128.h"
#include "lowmc.c.i"

#undef FN_ATTR
#define FN_ATTR ATTR_TARGET_AVX2
#undef IMPL
#define IMPL s256


#include "lowmc_129_129_4_fns_s256.h"
#include "lowmc.c.i"

#include "lowmc_192_192_4_fns_s256.h"
#include "lowmc.c.i"

#include "lowmc_255_255_4_fns_s256.h"
#include "lowmc.c.i"

#include "lowmc_128_128_20_fns_s256.h"
#include "lowmc.c.i"

#include "lowmc_192_192_30_fns_s256.h"
#include "lowmc.c.i"

#include "lowmc_256_256_38_fns_s256.h"
#include "lowmc.c.i"

void lowmc_compute(const lowmc_parameters_t* lowmc, const lowmc_key_t* key, const mzd_local_t* x,
                   mzd_local_t* y) {
  const uint32_t lowmc_id = LOWMC_GET_ID(lowmc);
  /* AVX2 enabled instances */
  if (CPU_SUPPORTS_AVX2) {
    switch (lowmc_id) {
      /* Instances with partial Sbox layer */
      /* Instances with full Sbox layer */
    case LOWMC_ID(129, 43):
      lowmc_s256_lowmc_129_129_4(key, x, y);
      return;
    }
  }

  /* SSE2/NEON enabled instances */
  if (CPU_SUPPORTS_SSE2 || CPU_SUPPORTS_NEON) {
    switch (lowmc_id) {
      /* Instances with partial Sbox layer */
      /* Instances with full Sbox layer */
    case LOWMC_ID(129, 43):
      lowmc_s128_lowmc_129_129_4(key, x, y);
      return;
    }
  }

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
  /* AVX2 enabled instances */
  if (CPU_SUPPORTS_AVX2) {
    switch (lowmc_id) {
      /* Instances with partial Sbox layer */
      /* Instances with full Sbox layer */
    case LOWMC_ID(129, 43):
      lowmc_store_s256_lowmc_129_129_4(key, x, state);
      return;
    }
  }

  /* SSE2/NEON enabled instances */
  if (CPU_SUPPORTS_SSE2 || CPU_SUPPORTS_NEON) {
    switch (lowmc_id) {
      /* Instances with partial Sbox layer */
      /* Instances with full Sbox layer */
    case LOWMC_ID(129, 43):
      lowmc_store_s128_lowmc_129_129_4(key, x, state);
      return;
    }
  }

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

