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
#include "bitstream.h"
#include "picnic3_impl.h"
#include "picnic3_types.h"

#if !defined(_MSC_VER)
#include <stdalign.h>
#endif
#include <string.h>
#include <assert.h>

#include "lowmc_192_192_4.h"

// clang-format off

#if !defined(NO_UINT64_FALLBACK)

/**
 * S-box for m = 64
 */
static void sbox_uint64_lowmc_192_192_4(mzd_local_t* in) {
  mzd_local_t x0m[1], x1m[1], x2m[1];
  // a
  mzd_and_uint64_192(x0m, mask_192_192_64_a, in);
  // b
  mzd_and_uint64_192(x1m, mask_192_192_64_b, in);
  // c
  mzd_and_uint64_192(x2m, mask_192_192_64_c, in);

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
#define picnic3_aux_sbox_bitsliced(LOWMC_N, XOR, AND, SHL, SHR, bitmask_a, bitmask_b, bitmask_c)   \
  do {                                                                                             \
    mzd_local_t a[1], b[1], c[1];                                                                  \
    /* a */                                                                                        \
    AND(a, bitmask_a, statein);                                                                    \
    /* b */                                                                                        \
    AND(b, bitmask_b, statein);                                                                    \
    /* c */                                                                                        \
    AND(c, bitmask_c, statein);                                                                    \
                                                                                                   \
    SHL(a, a, 2);                                                                                  \
    SHL(b, b, 1);                                                                                  \
    mzd_local_t d[1], e[1], f[1];                                                                  \
    /* a */                                                                                        \
    AND(d, bitmask_a, stateout);                                                                   \
    /* b */                                                                                        \
    AND(e, bitmask_b, stateout);                                                                   \
    /* c */                                                                                        \
    AND(f, bitmask_c, stateout);                                                                   \
                                                                                                   \
    SHL(d, d, 2);                                                                                  \
    SHL(e, e, 1);                                                                                  \
                                                                                                   \
    mzd_local_t fresh_output_ab[1], fresh_output_bc[1], fresh_output_ca[1];                        \
    XOR(fresh_output_ab, a, b);                                                                    \
    XOR(fresh_output_ca, e, fresh_output_ab);                                                      \
    XOR(fresh_output_bc, d, a);                                                                    \
    XOR(fresh_output_ab, fresh_output_ab, c);                                                      \
    XOR(fresh_output_ab, fresh_output_ab, f);                                                      \
                                                                                                   \
    mzd_local_t t0[1], t1[1], t2[1], aux[1];                                                       \
    SHR(t2, fresh_output_ca, 2);                                                                   \
    SHR(t1, fresh_output_bc, 1);                                                                   \
    XOR(t2, t2, t1);                                                                               \
    XOR(aux, t2, fresh_output_ab);                                                                 \
    /* a & b */                                                                                    \
    AND(t0, a, b);                                                                                 \
    /* b & c */                                                                                    \
    AND(t1, b, c);                                                                                 \
    /* c & a */                                                                                    \
    AND(t2, c, a);                                                                                 \
    SHR(t2, t2, 2);                                                                                \
    SHR(t1, t1, 1);                                                                                \
    XOR(t2, t2, t1);                                                                               \
    XOR(t2, t2, t0);                                                                               \
    XOR(aux, aux, t2);                                                                             \
                                                                                                   \
    bitstream_t parity_tape     = {{tapes->parity_tapes}, tapes->pos};                             \
    bitstream_t last_party_tape = {{tapes->tape[15]}, tapes->pos};                                 \
                                                                                                   \
    /* calculate aux_bits to fix and_helper */                                                     \
    mzd_from_bitstream(&parity_tape, t0, (LOWMC_N + 63) / (sizeof(uint64_t) * 8), LOWMC_N);        \
    XOR(aux, aux, t0);                                                                             \
    mzd_from_bitstream(&last_party_tape, t1, (LOWMC_N + 63) / (sizeof(uint64_t) * 8), LOWMC_N);    \
    XOR(aux, aux, t1);                                                                             \
                                                                                                   \
    last_party_tape.position = tapes->pos;                                                         \
    mzd_to_bitstream(&last_party_tape, aux, (LOWMC_N + 63) / (sizeof(uint64_t) * 8), LOWMC_N);     \
    bitstream_t aux_tape = {{tapes->aux_bits}, tapes->aux_pos};                                    \
    mzd_to_bitstream(&aux_tape, aux, (LOWMC_N + 63) / (sizeof(uint64_t) * 8), LOWMC_N);            \
                                                                                                   \
    tapes->aux_pos += LOWMC_N;                                                                     \
  } while (0)

static void sbox_aux_uint64_lowmc_192_192_4(mzd_local_t* statein, mzd_local_t* stateout,
                                            randomTape_t* tapes) {
  picnic3_aux_sbox_bitsliced(LOWMC_192_192_4_N, mzd_xor_uint64_192, mzd_and_uint64_192,
                             mzd_shift_left_uint64_192, mzd_shift_right_uint64_192,
                             mask_192_192_64_a, mask_192_192_64_b, mask_192_192_64_c);
}
#endif /* !NO_UINT64_FALLBACK */

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
    /* Instances with full Sbox layer */
  case LOWMC_ID(192, 64):
    lowmc_uint64_lowmc_192_192_4(key, x, y);
    return;
  }
#endif

  UNREACHABLE;
}



void lowmc_compute_aux(const lowmc_parameters_t* lowmc, lowmc_key_t* key, randomTape_t* tapes) {
  const uint32_t lowmc_id = LOWMC_GET_ID(lowmc);

#if !defined(NO_UINT64_FALLBACK)
  /* uint64_t implementations */
  switch (lowmc_id) {
    /* Instances with full Sbox layer */
  case LOWMC_ID(192, 64):
    lowmc_aux_uint64_lowmc_192_192_4(key, tapes);
    return;
  }
#endif

  UNREACHABLE;
}
