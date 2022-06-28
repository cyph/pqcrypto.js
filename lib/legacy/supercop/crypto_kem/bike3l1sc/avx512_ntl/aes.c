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

#include "aes.h"
#include "utilities.h"
#include <string.h>

void aes256_enc(OUT uint8_t *ct,
                IN const uint8_t *pt,
                IN const aes256_ks_t *ks) {
    uint32_t i = 0;
    __m128i block = _mm_setr_epi8(pt[0],  pt[1],  pt[2],  pt[3],
                                  pt[4],  pt[5],  pt[6],  pt[7],
                                  pt[8],  pt[9],  pt[10], pt[11],
                                  pt[12], pt[13], pt[14], pt[15]);

    block = _mm_xor_si128(block, ks->keys[0]);
    for (i = 1; i < AES256_ROUNDS; i++) {
        block = _mm_aesenc_si128(block, ks->keys[i]);
    }
    block = _mm_aesenclast_si128(block, ks->keys[AES256_ROUNDS]);

    //We use memcpy to avoid align casting.
    memcpy(ct, (const void*)&block, sizeof(block));

    // Clear the secret data when done
    secure_clean((uint8_t*)&block, sizeof(block));
}
