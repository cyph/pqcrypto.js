/*
 * Copyright (c) 2018, Koninklijke Philips N.V.
 */

/**
 * @file
 * Implementation of the random A matrix creation function.
 */

#include "a_random.h"

#include "misc.h"
#include "little_endian.h"
#include "drbg.h"

int create_A_random(uint16_t *A_random, const unsigned char *seed, const parameters *params) {
    if (params->tau == 2) {
        return drbg_sampler16_2_once(A_random, params->tau2_len, seed, params->kappa_bytes, params->q);
    } else {
        return drbg_sampler16_2_once(A_random, (size_t) (params->d * params->k), seed, params->kappa_bytes, params->q);
    }
}
