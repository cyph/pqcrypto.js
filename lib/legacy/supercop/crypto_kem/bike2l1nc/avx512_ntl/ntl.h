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

#ifndef _NTL_H
#define _NTL_H

#include "types.h"

EXTERNC void mod_inv(OUT uint8_t res_bin[R_SIZE], 
                     IN const uint8_t a_bin[R_SIZE]);

EXTERNC void cyclic_product(OUT uint8_t res_bin[R_SIZE], 
                            IN const uint8_t a_bin[R_SIZE],
                            IN const uint8_t b_bin[R_SIZE]);

#endif
