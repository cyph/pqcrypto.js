/*
 * Copyright (c) 2018, Koninklijke Philips N.V.
 */

/**
 * @file
 * Declaration of the random bytes functions.
 */

#ifndef RNG_H
#define RNG_H

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Initializes the random number generator used for generating the random
     * bytes.
     *
     * @param[in] entropy_input the bytes to use as input entropy (48 bytes)
     * @param[in] personalization_string an optional personalization string (48 bytes)
     * @param[in] security_strength parameter to specify the security strength of the random bytes
     */
    void randombytes_init(unsigned char *entropy_input, unsigned char *personalization_string, int security_strength);

    /**
     * Generates a sequence of random bytes.
     *
     * @param[out] x destination of the random bytes
     * @param[in] xlen the number of random bytes
     * @return _0_ in case of success, non-zero otherwise
     */
    int randombytes(unsigned char *x, unsigned long long xlen);

#ifdef __cplusplus
}
#endif

#endif /* RNG_H */
