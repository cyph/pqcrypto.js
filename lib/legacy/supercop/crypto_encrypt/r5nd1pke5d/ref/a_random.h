/*
 * Copyright (c) 2018, Koninklijke Philips N.V.
 */

/**
 * @file
 * Declaration of the random A matrix creation function.
 */

#ifndef A_RANDOM_H
#define A_RANDOM_H

#include "parameters.h"

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Creates A random for the given seed and algorithm parameters.
     *
     * @param[out] A_random     the random A to create
     * @param[in]  seed         the seed (kappa_bytes bytes)
     * @param[in]  params       the algorithm parameters in use
     * @return __0__ on success
     */
    int create_A_random(uint16_t *A_random, const unsigned char *seed, const parameters *params);

#ifdef __cplusplus
}
#endif

#endif /* A_RANDOM_H */
