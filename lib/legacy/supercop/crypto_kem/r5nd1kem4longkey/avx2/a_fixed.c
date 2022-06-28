/*
 * Copyright (c) 2018, Koninklijke Philips N.V.
 */

/**
 * @file
 * Implementation of the fixed A matrix generation function.
 */

#include "a_fixed.h"
#include "a_random.h"

#include <stdio.h>
#include <string.h>

#if PARAMS_TAU == 1
modq_t A_fixed[PARAMS_D * 2 * PARAMS_K];
#endif

int create_A_fixed(const unsigned char *seed) {
#if PARAMS_TAU == 1
    /* Create A_fixed randomly */
    create_A_random(A_fixed, seed);

    /* Duplicate rows */
    for (int i = PARAMS_K - 1; i >= 0; --i) {
        memcpy(A_fixed + (2 * i + 1) * PARAMS_D, A_fixed + i*PARAMS_D, PARAMS_D * sizeof (modq_t));
        if (i != 0) {
            memcpy(A_fixed + (2 * i) * PARAMS_D, A_fixed + i*PARAMS_D, PARAMS_D * sizeof (modq_t));
        }
    }

    return 0;
#else
    (void) seed;
    abort();
#endif
}
