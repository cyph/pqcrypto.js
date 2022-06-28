/*
 * Copyright (c) 2018, Koninklijke Philips N.V.
 */

/**
 * @file
 * Declaration of the random A matrix creation function.
 */

#ifndef A_RANDOM_H
#define A_RANDOM_H

#include "r5_parameter_sets.h"

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Creates A random for the given seed and algorithm parameters.
     *
     * @param[out] A_random     the random A to create
     * @param[in]  seed         the seed (PARAMS_KAPPA_BYTES bytes)
     */
    void create_A_random(modq_t *A_random, const unsigned char *seed);

#ifdef __cplusplus
}
#endif

#endif /* A_RANDOM_H */
