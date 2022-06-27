/*
 *  This file is part of the optimized implementation of the Picnic signature scheme.
 *  See the accompanying documentation for complete details.
 *
 *  The code is provided under the MIT license, see LICENSE for
 *  more details.
 *  SPDX-License-Identifier: MIT
 */


#include "mpc_lowmc.h"
#include "mzd_additional.h"

#if !defined(_MSC_VER)
#include <stdalign.h>
#endif
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#include "lowmc_192_192_30.h"

#define MPC_LOOP_CONST(function, result, first, second, sc)                                        \
  do {                                                                                             \
    for (unsigned int e = 0; e < (sc); ++e) {                                                      \
      function((result)[e], (first)[e], (second));                                                 \
    }                                                                                              \
  } while (0)

#define MPC_LOOP_SHARED(function, result, first, second, sc)                                       \
  do {                                                                                             \
    for (unsigned int o = 0; o < (sc); ++o) {                                                      \
      function((result)[o], (first)[o], (second)[o]);                                              \
    }                                                                                              \
  } while (0)

#define MPC_LOOP_SHARED_1(function, result, first, sc)                                             \
  do {                                                                                             \
    for (unsigned int o = 0; o < (sc); ++o) {                                                      \
      function((result)[o], (first)[o]);                                                           \
    }                                                                                              \
  } while (0)

#define MPC_LOOP_CONST_C_0(function, result, first, second, sc)                                    \
  function((result)[0], (first)[0], (second))

#define MPC_LOOP_CONST_C_ch(function, result, first, second, sc, c)                                \
  do {                                                                                             \
    if (!(c)) {                                                                                    \
      MPC_LOOP_CONST_C_0(function, result, first, second, sc);                                     \
    } else if ((c) == (sc)) {                                                                      \
      function((result)[(sc)-1], first[(sc)-1], (second));                                         \
    }                                                                                              \
  } while (0)

// clang-format off
// clang-format on
/* MPC Sbox implementation for partical Sbox */
static void mpc_and_uint64(uint64_t* res, uint64_t const* first, uint64_t const* second,
                           uint64_t const* r, view_t* view, unsigned viewshift) {
  for (unsigned m = 0; m < SC_PROOF; ++m) {
    const unsigned j = (m + 1) % SC_PROOF;
    uint64_t tmp1    = second[m] ^ second[j];
    uint64_t tmp2    = first[j] & second[m];
    tmp1             = tmp1 & first[m];
    tmp1             = tmp1 ^ tmp2;
    tmp2             = r[m] ^ r[j];
    res[m] = tmp1 = tmp1 ^ tmp2;
    if (viewshift) {
      tmp1       = tmp1 >> viewshift;
      view->t[m] = view->t[m] ^ tmp1;
    } else {
      // on first call (viewshift == 0), view->t[0..2] == 0
      view->t[m] = tmp1;
    }
  }
}

static void mpc_and_verify_uint64(uint64_t* res, uint64_t const* first, uint64_t const* second,
                                  uint64_t const* r, view_t* view, uint64_t const mask,
                                  unsigned viewshift) {
  for (unsigned m = 0; m < (SC_VERIFY - 1); ++m) {
    const unsigned j = (m + 1);
    uint64_t tmp1    = second[m] ^ second[j];
    uint64_t tmp2    = first[j] & second[m];
    tmp1             = tmp1 & first[m];
    tmp1             = tmp1 ^ tmp2;
    tmp2             = r[m] ^ r[j];
    res[m] = tmp1 = tmp1 ^ tmp2;
    if (viewshift || m) {
      tmp1       = tmp1 >> viewshift;
      view->t[m] = view->t[m] ^ tmp1;
    } else {
      // on first call (viewshift == 0), view->t[0] == 0
      view->t[m] = tmp1;
    }
  }

  const uint64_t rsc = view->t[SC_VERIFY - 1] << viewshift;
  res[SC_VERIFY - 1] = rsc & mask;
}

#define bitsliced_step_1_uint64_10(sc)                                                             \
  uint64_t r0m[sc];                                                                                \
  uint64_t r0s[sc];                                                                                \
  uint64_t r1m[sc];                                                                                \
  uint64_t r1s[sc];                                                                                \
  uint64_t r2m[sc];                                                                                \
  uint64_t x0s[sc];                                                                                \
  uint64_t x1s[sc];                                                                                \
  uint64_t x2m[sc];                                                                                \
  do {                                                                                             \
    for (unsigned int m = 0; m < (sc); ++m) {                                                      \
      const uint64_t inm   = in[m];                                                                \
      const uint64_t rvecm = rvec[m];                                                              \
                                                                                                   \
      x0s[m] = (inm & MASK_X0I) << 2;                                                              \
      x1s[m] = (inm & MASK_X1I) << 1;                                                              \
      x2m[m] = inm & MASK_X2I;                                                                     \
                                                                                                   \
      r0m[m] = rvecm & MASK_X0I;                                                                   \
      r1m[m] = rvecm & MASK_X1I;                                                                   \
      r2m[m] = rvecm & MASK_X2I;                                                                   \
                                                                                                   \
      r0s[m] = r0m[m] << 2;                                                                        \
      r1s[m] = r1m[m] << 1;                                                                        \
    }                                                                                              \
  } while (0)

#define bitsliced_step_2_uint64_10(sc)                                                             \
  do {                                                                                             \
    for (unsigned int m = 0; m < (sc); ++m) {                                                      \
      const uint64_t tmp1 = r2m[m] ^ x0s[m];                                                       \
      const uint64_t tmp2 = x0s[m] ^ x1s[m];                                                       \
      const uint64_t tmp3 = tmp2 ^ r1m[m];                                                         \
      const uint64_t tmp4 = tmp2 ^ r0m[m] ^ x2m[m];                                                \
                                                                                                   \
      in[m] = (in[m] & MASK_MASK) ^ (tmp4) ^ (tmp1 >> 2) ^ (tmp3 >> 1);                            \
    }                                                                                              \
  } while (0)

static void mpc_sbox_prove_uint64_10(uint64_t* in, view_t* view, uint64_t const* rvec) {
  bitsliced_step_1_uint64_10(SC_PROOF);

  mpc_and_uint64(r0m, x0s, x1s, r2m, view, 0);
  mpc_and_uint64(r2m, x1s, x2m, r1s, view, 1);
  mpc_and_uint64(r1m, x0s, x2m, r0s, view, 2);

  bitsliced_step_2_uint64_10(SC_PROOF - 1);
}

static void mpc_sbox_verify_uint64_10(uint64_t* in, view_t* view, uint64_t const* rvec) {
  bitsliced_step_1_uint64_10(SC_VERIFY);

  mpc_and_verify_uint64(r0m, x0s, x1s, r2m, view, MASK_X2I, 0);
  mpc_and_verify_uint64(r2m, x1s, x2m, r1s, view, MASK_X2I, 1);
  mpc_and_verify_uint64(r1m, x0s, x2m, r0s, view, MASK_X2I, 2);

  bitsliced_step_2_uint64_10(SC_VERIFY);
}

/* MPC Sbox implementation for full instances */
#if !defined(NO_UINT64_FALLBACK)


#define bitsliced_step_1(sc, AND, ROL, MASK_A, MASK_B, MASK_C)                                     \
  mzd_local_t x2m[sc] = {{{0}}};                                                                   \
  mzd_local_t r0m[sc] = {{{0}}}, r1m[sc] = {{{0}}}, r2m[sc] = {{{0}}};                             \
  mzd_local_t x0s[sc] = {{{0}}}, x1s[sc] = {{{0}}}, r0s[sc] = {{{0}}}, r1s[sc] = {{{0}}};          \
                                                                                                   \
  for (unsigned int m = 0; m < (sc); ++m) {                                                        \
    AND(&x0s[m], &in[m], MASK_A);                                                                  \
    AND(&x1s[m], &in[m], MASK_B);                                                                  \
    AND(&x2m[m], &in[m], MASK_C);                                                                  \
                                                                                                   \
    ROL(&x0s[m], &x0s[m], 2);                                                                      \
    ROL(&x1s[m], &x1s[m], 1);                                                                      \
                                                                                                   \
    AND(&r0m[m], &rvec->s[m], MASK_A);                                                             \
    AND(&r1m[m], &rvec->s[m], MASK_B);                                                             \
    AND(&r2m[m], &rvec->s[m], MASK_C);                                                             \
                                                                                                   \
    ROL(&r0s[m], &r0m[m], 2);                                                                      \
    ROL(&r1s[m], &r1m[m], 1);                                                                      \
  }

#define bitsliced_step_2(sc, XOR, ROR)                                                             \
  for (unsigned int m = 0; m < sc; ++m) {                                                          \
    XOR(&r2m[m], &r2m[m], &x0s[m]);                                                                \
    XOR(&x0s[m], &x0s[m], &x1s[m]);                                                                \
    XOR(&r1m[m], &r1m[m], &x0s[m]);                                                                \
    XOR(&r0m[m], &r0m[m], &x0s[m]);                                                                \
    XOR(&r0m[m], &r0m[m], &x2m[m]);                                                                \
                                                                                                   \
    ROR(&x0s[m], &r2m[m], 2);                                                                      \
    ROR(&x1s[m], &r1m[m], 1);                                                                      \
                                                                                                   \
    XOR(&x0s[m], &x0s[m], &r0m[m]);                                                                \
    XOR(&out[m], &x0s[m], &x1s[m]);                                                                \
  }



#endif /* NO_UINT_FALLBACK */


/* TODO: get rid of the copies */
#define SBOX(sbox, y, x, views, rvec, n, shares, shares2)                                          \
  {                                                                                                \
    mzd_local_t tmp[shares];                                                                       \
    for (unsigned int count = 0; count < shares; ++count) {                                        \
      memcpy(tmp[count].w64, CONST_BLOCK(x[count], 0)->w64, sizeof(mzd_local_t));                  \
    }                                                                                              \
    sbox(tmp, tmp, views, rvec);                                                                   \
    for (unsigned int count = 0; count < shares; ++count) {                                        \
      memcpy(BLOCK(y[count], 0)->w64, tmp[count].w64, sizeof(mzd_local_t));                        \
    }                                                                                              \
  }                                                                                                \
  while (0)

#define SBOX_uint64(sbox, y, x, views, rvec, n, shares, shares2)                                   \
  do {                                                                                             \
    uint64_t in[shares];                                                                           \
    for (unsigned int count = 0; count < shares; ++count) {                                        \
      in[count] = CONST_BLOCK(x[count], 0)->w64[(n) / (sizeof(word) * 8) - 1];                     \
    }                                                                                              \
    sbox(in, views, rvec->t);                                                                      \
    for (unsigned int count = 0; count < shares2; ++count) {                                       \
      memcpy(BLOCK(y[count], 0)->w64, CONST_BLOCK(x[count], 0)->w64,                               \
             ((n) / (sizeof(word) * 8) - 1) * sizeof(word));                                       \
      BLOCK(y[count], 0)->w64[(n) / (sizeof(word) * 8) - 1] = in[count];                           \
    }                                                                                              \
  } while (0)

#if !defined(NO_UINT64_FALLBACK)
#define IMPL uint64

// uint64 based implementation
#include "lowmc_128_128_20_fns_uint64.h"
#include "mpc_lowmc.c.i"

#include "lowmc_129_129_4_fns_uint64.h"
#include "mpc_lowmc.c.i"

#include "lowmc_192_192_30_fns_uint64.h"
#include "mpc_lowmc.c.i"

#include "lowmc_192_192_4_fns_uint64.h"
#include "mpc_lowmc.c.i"

#include "lowmc_256_256_38_fns_uint64.h"
#include "mpc_lowmc.c.i"

#include "lowmc_255_255_4_fns_uint64.h"
#include "mpc_lowmc.c.i"
#endif


zkbpp_lowmc_implementation_f get_zkbpp_lowmc_implementation(const lowmc_parameters_t* lowmc) {
  const uint32_t lowmc_id = LOWMC_GET_ID(lowmc);

#if !defined(NO_UINT64_FALLBACK)
  /* uint64_t implementations */
  switch (lowmc_id) {
    /* Instances with partial Sbox layer */
  case LOWMC_ID(192, 10):
    return mpc_lowmc_prove_uint64_lowmc_192_192_30;
    /* Instances with full Sbox layer */
  }
#endif

  UNREACHABLE;
  return NULL;
}

zkbpp_lowmc_verify_implementation_f
get_zkbpp_lowmc_verify_implementation(const lowmc_parameters_t* lowmc) {
  const uint32_t lowmc_id = LOWMC_GET_ID(lowmc);

#if !defined(NO_UINT64_FALLBACK)
  /* uint64_t implementations */
  switch (lowmc_id) {
    /* Instances with partial Sbox layer */
  case LOWMC_ID(192, 10):
    return mpc_lowmc_verify_uint64_lowmc_192_192_30;
    /* Instances with full Sbox layer */
  }
#endif

  UNREACHABLE;
  return NULL;
}

#if !defined(NO_UINT64_FALLBACK)
static void mzd_share_uint64_128(mzd_local_t* r, const mzd_local_t* v1, const mzd_local_t* v2,
                                 const mzd_local_t* v3) {
  mzd_xor_uint64_128(r, v1, v2);
  mzd_xor_uint64_128(r, r, v3);
}

static void mzd_share_uint64_192(mzd_local_t* r, const mzd_local_t* v1, const mzd_local_t* v2,
                                 const mzd_local_t* v3) {
  mzd_xor_uint64_192(r, v1, v2);
  mzd_xor_uint64_192(r, r, v3);
}

static void mzd_share_uint64_256(mzd_local_t* r, const mzd_local_t* v1, const mzd_local_t* v2,
                                 const mzd_local_t* v3) {
  mzd_xor_uint64_256(r, v1, v2);
  mzd_xor_uint64_256(r, r, v3);
}
#endif


zkbpp_share_implementation_f get_zkbpp_share_implentation(const lowmc_parameters_t* lowmc) {

#if !defined(NO_UINT64_FALLBACK)
  if (lowmc->n <= 128) {
    return mzd_share_uint64_128;
  } else if (lowmc->n <= 192) {
    return mzd_share_uint64_192;
  } else {
    return mzd_share_uint64_256;
  }
#endif
}
