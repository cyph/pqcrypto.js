/***************************************************************************
* Additional implementation of "BIKE: Bit Flipping Key Encapsulation". 
* Copyright 2017 Amazon.com, Inc. or its affiliates. All Rights Reserved.
*
* Written by Nir Drucker and Shay Gueron
* AWS Cryptographic Algorithms Group
* (ndrucker@amazon.com, gueron@amazon.com)
*
* The license is detailed in the file LICENSE.txt, and applies to this file.
* ***************************************************************************/

#include "ntl.h"
#include <NTL/GF2X.h>

using namespace NTL;

EXTERNC void mod_inv(OUT uint8_t res_bin[R_SIZE], 
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

EXTERNC void cyclic_product(OUT uint8_t res_bin[R_SIZE], 
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
