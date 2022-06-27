/*
Implementation of the Lilliput-AE tweakable block cipher.

Authors, hereby denoted as "the implementer":
    Kévin Le Gouguec,
    2019.

For more information, feedback or questions, refer to our website:
https://paclido.fr/lilliput-ae

To the extent possible under law, the implementer has waived all copyright
and related or neighboring rights to the source code in this file.
http://creativecommons.org/publicdomain/zero/1.0/

---

This file provides the implementation of Lilliput-TBC's tweakey schedule.
*/

#include <stdint.h>
#include <string.h>

#include "constants.h"
#include "multiplications.h"
#include "tweakey.h"


#define LANES_NB (TWEAKEY_BYTES/LANE_BYTES)


void tweakey_state_init(
    uint8_t TK[TWEAKEY_BYTES],
    const uint8_t key[KEY_BYTES],
    const uint8_t tweak[TWEAK_BYTES]
)
{
    memcpy(TK,             tweak, TWEAK_BYTES);
    memcpy(TK+TWEAK_BYTES, key,   KEY_BYTES);
}


void tweakey_state_extract(
    const uint8_t TK[TWEAKEY_BYTES],
    uint8_t round_constant,
    uint8_t round_tweakey[ROUND_TWEAKEY_BYTES]
)
{
    memset(round_tweakey, 0, ROUND_TWEAKEY_BYTES);

    for (size_t j=0; j<LANES_NB; j++)
    {
        const uint8_t *TKj = TK + j*LANE_BYTES;

        for (size_t k=0; k<LANE_BYTES; k++)
        {
            round_tweakey[k] ^= TKj[k];
        }
    }

    round_tweakey[0] ^= round_constant;
}


typedef void (*matrix_multiplication)(const uint8_t x[LANE_BYTES], uint8_t y[LANE_BYTES]);

static const matrix_multiplication ALPHAS[7] = {
    _multiply_M,
    _multiply_M2,
    _multiply_M3,
    _multiply_M4,
    _multiply_MR,
    _multiply_MR2,
    _multiply_MR3
};


void tweakey_state_update(uint8_t TK[TWEAKEY_BYTES])
{
    for (size_t j=0; j<LANES_NB; j++)
    {
        uint8_t *TKj = TK + j*LANE_BYTES;

        uint8_t TKj_old[LANE_BYTES];
        memcpy(TKj_old, TKj, LANE_BYTES);

        ALPHAS[j](TKj_old, TKj);
    }
}
