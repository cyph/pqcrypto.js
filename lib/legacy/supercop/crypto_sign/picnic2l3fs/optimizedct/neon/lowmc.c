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
#include "picnic2_impl.h"
#include "simd.h"

#if !defined(_MSC_VER)
#include <stdalign.h>
#endif
#include <string.h>

static uint64_t sbox_layer_10_bitsliced_uint64(uint64_t in) {
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

/**
 * S-box for m = 10
 */
static void sbox_layer_10_uint64(uint64_t* d) {
  *d = sbox_layer_10_bitsliced_uint64(*d);
}

#include "lowmc_192_192_30.h"

#if !defined(NO_UINT64_FALLBACK)
// uint64 based implementation
#include "lowmc_fns_uint64_L1.h"
#define LOWMC lowmc_uint64_128
#include "lowmc.c.i"

#include "lowmc_fns_uint64_L3.h"
#undef LOWMC
#define LOWMC lowmc_uint64_192
#include "lowmc.c.i"

#include "lowmc_fns_uint64_L5.h"
#undef LOWMC
#define LOWMC lowmc_uint64_256
#include "lowmc.c.i"
#endif


// L1 using SSE2/NEON
#include "lowmc_fns_s128_L1.h"
#undef LOWMC
#define LOWMC lowmc_s128_128
#include "lowmc.c.i"

// L3 using SSE2/NEON
#include "lowmc_fns_s128_L3.h"
#undef LOWMC
#define LOWMC lowmc_s128_192
#include "lowmc.c.i"

// L5 using SSE2/NEON
#include "lowmc_fns_s128_L5.h"
#undef LOWMC
#define LOWMC lowmc_s128_256
#include "lowmc.c.i"

#undef FN_ATTR


lowmc_implementation_f lowmc_get_implementation(const lowmc_t* lowmc) {
  ASSUME(lowmc->m == 10);
  ASSUME(lowmc->n == 128 || lowmc->n == 192 || lowmc->n == 256);

  if (CPU_SUPPORTS_SSE2 || CPU_SUPPORTS_NEON) {
    if (lowmc->m == 10) {
      switch (lowmc->n) {
      case 192:
        return lowmc_s128_192_10;
      }
    }
  }

#if !defined(NO_UINT64_FALLBACK)
  if (lowmc->m == 10) {
    switch (lowmc->n) {
    case 192:
      return lowmc_uint64_192_10;
    }
  }

#endif

  return NULL;
}


lowmc_compute_aux_implementation_f lowmc_compute_aux_get_implementation(const lowmc_t* lowmc) {
  ASSUME(lowmc->m == 10);
  ASSUME(lowmc->n == 128 || lowmc->n == 192 || lowmc->n == 256);

  if (CPU_SUPPORTS_SSE2 || CPU_SUPPORTS_NEON) {
    if (lowmc->m == 10) {
      switch (lowmc->n) {
      case 192:
        return lowmc_s128_192_compute_aux_10;
      }
    }
  }

#if !defined(NO_UINT64_FALLBACK)
  if (lowmc->m == 10) {
    switch (lowmc->n) {
    case 192:
      return lowmc_uint64_192_compute_aux_10;
    }
  }
#endif

  return NULL;
}
