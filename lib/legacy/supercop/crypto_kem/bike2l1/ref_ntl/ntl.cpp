/******************************************************************************
 * BIKE -- Bit Flipping Key Encapsulation
 *
 * Copyright (c) 2017 Nir Drucker, Shay Gueron, Rafael Misoczki, Tobias Oder, Tim Gueneysu
 * (drucker.nir@gmail.com, shay.gueron@gmail.com, rafael.misoczki@intel.com, tobias.oder@rub.de, tim.gueneysu@rub.de)
 *
 * Permission to use this code for BIKE is granted.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * * The names of the contributors may not be used to endorse or promote
 *   products derived from this software without specific prior written
 *   permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ""AS IS"" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS CORPORATION OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

extern "C" {
#include "types.h"
}

#include <NTL/GF2X.h>

using namespace NTL;

typedef unsigned char uint8_t;

void ntl_add(OUT uint8_t res_bin[R_SIZE],
        IN const uint8_t a_bin[R_SIZE],
        IN const uint8_t b_bin[R_SIZE])
{
    GF2X a, b, res;

    GF2XFromBytes(a, a_bin, R_SIZE);
    GF2XFromBytes(b, b_bin, R_SIZE);

    add(res, a, b);

    BytesFromGF2X(res_bin, res, R_SIZE);
}

void ntl_mod_inv(OUT uint8_t res_bin[R_SIZE],
        IN const uint8_t a_bin[R_SIZE])
{
    GF2X _m, a, res;

    GF2XFromBytes(a, a_bin, R_SIZE);

    //Create the modulus
    GF2XModulus m;
    SetCoeff(_m, 0, 1);
    SetCoeff(_m, R_BITS, 1);
    build(m, _m);

    InvMod(res, a, m);
    BytesFromGF2X(res_bin, res, R_SIZE);
}

void ntl_mod_mul(OUT uint8_t res_bin[R_SIZE], 
        IN const uint8_t a_bin[R_SIZE],
        IN const uint8_t b_bin[R_SIZE])
{
    GF2X _m, a, b, res;

    GF2XFromBytes(a, a_bin, R_SIZE);
    GF2XFromBytes(b, b_bin, R_SIZE);

    //Create the modulus
    GF2XModulus m;
    SetCoeff(_m, 0, 1);
    SetCoeff(_m, R_BITS, 1);
    build(m, _m);

    MulMod(res, a, b, m);

    BytesFromGF2X(res_bin, res, R_SIZE);
}

void ntl_split_polynomial(OUT uint8_t e0[R_SIZE],
        OUT uint8_t e1[R_SIZE],
        IN const uint8_t e[2*R_SIZE])
{
    GF2X e_pol, e0_pol, e1_pol;
    GF2XFromBytes(e_pol, e, N_SIZE);
    trunc(e0_pol, e_pol, R_BITS);
    RightShift(e1_pol, e_pol, R_BITS);

    BytesFromGF2X(e0, e0_pol, R_SIZE);
    BytesFromGF2X(e1, e1_pol, R_SIZE);
}

