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

This file implements the alpha-multiplications used in Lilliput-TBC's
tweakey schedule, where matrices M and M_R to the power n>1 are
implemented by applying functions for M and M_R n times.
*/

#ifndef MULTIPLICATIONS_H
#define MULTIPLICATIONS_H

#include <stdint.h>

#include "constants.h"


static void _multiply_M(const uint8_t x[LANE_BYTES], uint8_t y[LANE_BYTES])
{
    y[7] = x[6];
    y[6] = x[5];
    y[5] = x[5]<<3 ^ x[4];
    y[4] = x[4]>>3 ^ x[3];
    y[3] = x[2];
    y[2] = x[6]<<2 ^ x[1];
    y[1] = x[0];
    y[0] = x[7];
}

static void _multiply_M2(const uint8_t x[LANE_BYTES], uint8_t y[LANE_BYTES])
{
    uint8_t M_x[LANE_BYTES];
    _multiply_M(x, M_x);
    _multiply_M(M_x, y);
}

static void _multiply_M3(const uint8_t x[LANE_BYTES], uint8_t y[LANE_BYTES])
{
    uint8_t M_x[LANE_BYTES];
    uint8_t M2_x[LANE_BYTES];
    _multiply_M(x, M_x);
    _multiply_M(M_x, M2_x);
    _multiply_M(M2_x, y);
}

static void _multiply_M4(const uint8_t x[LANE_BYTES], uint8_t y[LANE_BYTES])
{
    uint8_t M_x[LANE_BYTES];
    uint8_t M2_x[LANE_BYTES];
    uint8_t M3_x[LANE_BYTES];
    _multiply_M(x, M_x);
    _multiply_M(M_x, M2_x);
    _multiply_M(M2_x, M3_x);
    _multiply_M(M3_x, y);
}

static void _multiply_MR(const uint8_t x[LANE_BYTES], uint8_t y[LANE_BYTES])
{
    y[0] = x[1];
    y[1] = x[2];
    y[2] = x[3]    ^ x[4]>>3;
    y[3] = x[4];
    y[4] = x[5]    ^ x[6]<<3;
    y[5] = x[3]<<2 ^ x[6];
    y[6] = x[7];
    y[7] = x[0];
}

static void _multiply_MR2(const uint8_t x[LANE_BYTES], uint8_t y[LANE_BYTES])
{
    uint8_t MR_x[LANE_BYTES];
    _multiply_MR(x, MR_x);
    _multiply_MR(MR_x, y);
}

static void _multiply_MR3(const uint8_t x[LANE_BYTES], uint8_t y[LANE_BYTES])
{
    uint8_t MR_x[LANE_BYTES];
    uint8_t MR2_x[LANE_BYTES];
    _multiply_MR(x, MR_x);
    _multiply_MR(MR_x, MR2_x);
    _multiply_MR(MR2_x, y);
}


#endif /* MULTIPLICATIONS_H */
