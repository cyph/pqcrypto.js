/*
 * Copyright (c) 2018, Koninklijke Philips N.V.
 */

/**
 * @file
 * Implementation of the fixed A matrix generation function.
 */

#include "a_fixed.h"
#include "a_random.h"
#include "r5_memory.h"

#include <stdlib.h>

size_t A_fixed_len = 0;
uint16_t *A_fixed = NULL;

int create_A_fixed(const unsigned char *seed, const parameters *params) {
    A_fixed_len = (size_t) (params->d * params->k);

    /* (Re)allocate space for A_fixed */
    A_fixed = checked_realloc(A_fixed, A_fixed_len * sizeof (*A_fixed));

    /* Create A_fixed randomly */
    if (create_A_random(A_fixed, seed, params)) {
        return 1;
    }

    /* Make all elements mod q */
    for (size_t i = 0; i < A_fixed_len; ++i) {
        A_fixed[i] = (uint16_t) (A_fixed[i] & (params->q - 1));
    }

    return 0;
}
