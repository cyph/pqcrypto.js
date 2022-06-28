/*
 * Copyright (c) 2018, Koninklijke Philips N.V.
 */

/**
 * @file
 * Implementation of the deterministic random bits (bytes) functions.
 *
 * Uses cSHAKE128 (seed size <= 16 bytes) or cSHAKE256 (seed size > 16 bytes) to
 * generate the random bytes. Unless USE_AES_DRBG is defined, in which case AES
 * in CTR mode on a zero input block with the seed as key is used to generate
 * the random data.
 *
 * Note: in case there is no customization, we use SHAKE directly instead of
 * cSHAKE since this saves some overhead.
 */

#include "drbg.h"

/*******************************************************************************
 * Public functions
 ******************************************************************************/

int drbg_sampler16_2_once(uint16_t *x, const size_t xlen, const void *seed) {
    /* Since without customization, SHAKE == CSHAKE, we use SHAKE here directly. */

#ifdef USE_AES_DRBG
    uint8_t key[32];

    EVP_CIPHER_CTX * aes_ctx;
    if (!(aes_ctx = EVP_CIPHER_CTX_new())) {
        abort();
    }

    int res;
    assert(PARAMS_KAPPA_BYTES == 16 || PARAMS_KAPPA_BYTES == 24 || PARAMS_KAPPA_BYTES == 32);
    hash(key, PARAMS_KAPPA_BYTES, seed, PARAMS_KAPPA_BYTES, (uint8_t) PARAMS_KAPPA_BYTES);
    switch (PARAMS_KAPPA_BYTES) {
        case 16:
            res = EVP_EncryptInit_ex(aes_ctx, EVP_aes_128_ctr(), NULL, key, NULL);
            break;
        case 24:
            res = EVP_EncryptInit_ex(aes_ctx, EVP_aes_192_ctr(), NULL, key, NULL);
            break;
        case 32:
            res = EVP_EncryptInit_ex(aes_ctx, EVP_aes_256_ctr(), NULL, key, NULL);
            break;
    }
    if (res != 1) {
        abort();
    }

    size_t nr_full_blocks = (xlen * sizeof (uint16_t)) >> 4;
    int len;
    if (nr_full_blocks) {
        uint8_t *input = checked_calloc(nr_full_blocks, 16);
        if (EVP_EncryptUpdate(aes_ctx, (uint8_t *) x, &len, input, (int) (nr_full_blocks << 4)) != 1) {
            abort();
        }
        free(input);
    }
    if ((xlen * sizeof (uint16_t)) & 15) {
        uint8_t final_block_output[16];
        uint8_t final_input[16] = {0};
        u64_to_le(final_input, (uint64_t) (nr_full_blocks + 1));
        if (EVP_EncryptUpdate(aes_ctx, final_block_output, &len, final_input, 16) != 1) {
            abort();
        }
        memcpy(((uint8_t *) x) + (nr_full_blocks << 4), final_block_output, (xlen * sizeof (uint16_t)) & 15);
    }

    EVP_CIPHER_CTX_free(aes_ctx);

#else

    if (PARAMS_KAPPA_BYTES > 16) {
        shake256((uint8_t *) x, xlen * sizeof (uint16_t), (const uint8_t *) seed, PARAMS_KAPPA_BYTES);
    } else {
        shake128((uint8_t *) x, xlen * sizeof (uint16_t), (const uint8_t *) seed, PARAMS_KAPPA_BYTES);
    }

#endif

#if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
    /* Flip byte order if necessary */
    for (size_t i = 0; i < xlen; ++i) {
        x[i] = (uint16_t) LITTLE_ENDIAN16(x[i]);
    }
#endif

    return 0;
}

int drbg_sampler16_2_once_customization(uint16_t *x, const size_t xlen, const void *seed, const void *customization, const size_t customization_len) {

#ifdef USE_AES_DRBG
    uint8_t key[32];

    EVP_CIPHER_CTX * aes_ctx;
    if (!(aes_ctx = EVP_CIPHER_CTX_new())) {
        abort();
    }

    int res;
    assert(PARAMS_KAPPA_BYTES == 16 || PARAMS_KAPPA_BYTES == 24 || PARAMS_KAPPA_BYTES == 32);
    hash_customization(key, PARAMS_KAPPA_BYTES, seed, PARAMS_KAPPA_BYTES, customization, customization_len, (uint8_t) PARAMS_KAPPA_BYTES);
    switch (PARAMS_KAPPA_BYTES) {
        case 16:
            res = EVP_EncryptInit_ex(aes_ctx, EVP_aes_128_ctr(), NULL, key, NULL);
            break;
        case 24:
            res = EVP_EncryptInit_ex(aes_ctx, EVP_aes_192_ctr(), NULL, key, NULL);
            break;
        case 32:
            res = EVP_EncryptInit_ex(aes_ctx, EVP_aes_256_ctr(), NULL, key, NULL);
            break;
    }
    if (res != 1) {
        abort();
    }

    size_t nr_full_blocks = (xlen * sizeof (uint16_t)) >> 4;
    int len;
    if (nr_full_blocks) {
        uint8_t *input = checked_calloc(nr_full_blocks, 16);
        if (EVP_EncryptUpdate(aes_ctx, (uint8_t *) x, &len, input, (int) (nr_full_blocks << 4)) != 1) {
            abort();
        }
        free(input);
    }
    if ((xlen * sizeof (uint16_t)) & 15) {
        uint8_t final_block_output[16];
        uint8_t final_input[16] = {0};
        u64_to_le(final_input, (uint64_t) (nr_full_blocks + 1));
        if (EVP_EncryptUpdate(aes_ctx, final_block_output, &len, final_input, 16) != 1) {
            abort();
        }
        memcpy(((uint8_t *) x) + (nr_full_blocks << 4), final_block_output, (xlen * sizeof (uint16_t)) & 15);
    }

    EVP_CIPHER_CTX_free(aes_ctx);

#else

    if (PARAMS_KAPPA_BYTES > 16) {
        cshake256((uint8_t *) x, xlen * sizeof (uint16_t), (const uint8_t *) seed, PARAMS_KAPPA_BYTES, (const uint8_t *) customization, customization_len);
    } else {
        cshake128((uint8_t *) x, xlen * sizeof (uint16_t), (const uint8_t *) seed, PARAMS_KAPPA_BYTES, (const uint8_t *) customization, customization_len);
    }

#endif

#if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
    /* Flip byte order if necessary */
    for (size_t i = 0; i < xlen; ++i) {
        x[i] = (uint16_t) LITTLE_ENDIAN16(x[i]);
    }
#endif

    return 0;
}
