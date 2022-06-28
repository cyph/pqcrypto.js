/*
 * Copyright (c) 2018, Koninklijke Philips N.V.
 */

/**
 * @file
 * Implementation of the hash function as used within Round5.
 */

#include "r5_hash.h"

extern void hash(uint8_t *output, const size_t output_len, const uint8_t *input, const size_t input_len, const uint8_t kappa_bytes);

extern void hash_customization(uint8_t *output, const size_t output_len, const uint8_t *input, const size_t input_len, const uint8_t *customization, const size_t customization_len, const uint8_t kappa_bytes);
