/***************************************************************************
* Additional implementation of "BIKE: Bit Flipping Key Encapsulation". 
* Copyright 2017 Amazon.com, Inc. or its affiliates. All Rights Reserved.
*
* Written by Nir Drucker and Shay Gueron
* AWS Cryptographic Algorithms Group
* (ndrucker@amazon.com, gueron@amazon.com)
*
* The license is detailed in the file LICENSE.txt, and applies to this file.
*
* The optimizations are based on the description developed in the paper: 
* N. Drucker, S. Gueron, 
* "A toolbox for software optimization of QC-MDPC code-based cryptosystems", 
* ePrint (2017).
* The decoder (in decoder/decoder.c) algorithm is the algorithm included in
* the early submission of CAKE (due to N. Sandrier and R Misoczki).
*
* ***************************************************************************/

#include "decode.h"
#include "utilities.h"
#include <string.h>

EXTERNC void compute_counter_of_unsat(OUT uint8_t upc[N_BITS],
                                      IN const uint8_t s[R_BITS],
                                      IN const compressed_idx_dv_t* inv_h0_compressed,
                                      IN const compressed_idx_dv_t* inv_h1_compressed)
{
    uint32_t i=0, j=0, pos=0;
    
    memset(upc, 0, N_BITS);

    for(i = 0; i < R_BITS; i++)
    {
        for(j = 0; j < DV; j++)
        {
            pos  = inv_h0_compressed->val[j].val;
            upc[i] += s[i+pos];
        }
    }

    for(i = 0; i < R_BITS; i++)
    {
        for(j = 0; j < DV; j++)
        {
            pos  = inv_h1_compressed->val[j].val;
            upc[i+R_BITS] += s[i+pos];
        }
    }

}

EXTERNC void recompute(OUT syndrome_t* s,
                       IN const uint32_t num_positions,
                       IN const uint32_t positions[R_BITS],
                       IN const compressed_idx_dv_t* h_compressed)
{
    for (uint32_t i = 0; i < num_positions; i++) 
    {
        uint32_t pos = positions[i];
        for (uint32_t j = 0; j < DV; j++) 
        {
            const uint32_t val = h_compressed->val[j].val;
            if (val <= pos) {
                s->dup1.raw[pos - val] ^= 1;
            } else {
                s->dup1.raw[R_BITS - val + pos] ^= 1;
            }
        }
    }
}
