/*
 * Copyright (c) 2018, Koninklijke Philips N.V.
 */

/**
 * @file
 * Implementation of the SHAKE128, SHAKE256, cSHAKE128, and cSHAKE256 hash
 * functions.
 */

#include "shake.h"

#include "misc.h"

/* Use OpenSSL's Shake unless disabled/not supported */
#undef USE_OPENSSL_SHAKE
#ifndef NO_OPENSSL_SHAKE
#include <openssl/opensslv.h>
#if OPENSSL_VERSION_NUMBER >= 0x1010100f
#define USE_OPENSSL_SHAKE
#include <openssl/evp.h>
#endif
#endif

/*******************************************************************************
 * Public functions
 ******************************************************************************/

extern void shake128_init(shake_ctx *ctx);
extern void shake128_absorb(shake_ctx *ctx, const unsigned char *input, const size_t input_len);
extern void shake128_squeezeblocks(shake_ctx *ctx, unsigned char *output, const size_t nr_blocks);

void shake128(unsigned char *output, size_t output_len, const unsigned char *input, const size_t input_len) {
#if defined(USE_OPENSSL_SHAKE)
    EVP_MD_CTX *md_ctx;
    if (!(md_ctx = EVP_MD_CTX_new())) {
        abort();
    }
    if (EVP_DigestInit_ex(md_ctx, EVP_shake128(), NULL) != 1) {
        abort();
    }
    if (EVP_DigestUpdate(md_ctx, input, input_len) != 1) {
        abort();
    }
    if (EVP_DigestFinalXOF(md_ctx, (unsigned char *) output, output_len) != 1) {
        abort();
    }
    EVP_MD_CTX_free(md_ctx);
#else
    shake_ctx ctx;
    shake128_init(&ctx);
    shake128_absorb(&ctx, input, input_len);
    if (Keccak_HashSqueeze(&ctx, output, output_len * 8) != 0) {
        abort();
    }
#endif
}

extern void shake256_init(shake_ctx *ctx);
extern void shake256_absorb(shake_ctx *ctx, const unsigned char *input, const size_t input_len);
extern void shake256_squeezeblocks(shake_ctx *ctx, unsigned char *output, const size_t nr_blocks);

void shake256(unsigned char *output, size_t output_len, const unsigned char *input, const size_t input_len) {
#if defined(USE_OPENSSL_SHAKE)
    EVP_MD_CTX *md_ctx;
    if (!(md_ctx = EVP_MD_CTX_new())) {
        abort();
    }
    if (EVP_DigestInit_ex(md_ctx, EVP_shake256(), NULL) != 1) {
        abort();
    }
    if (EVP_DigestUpdate(md_ctx, input, input_len) != 1) {
        abort();
    }
    if (EVP_DigestFinalXOF(md_ctx, (unsigned char *) output, output_len) != 1) {
        abort();
    }
    EVP_MD_CTX_free(md_ctx);
#else
    shake_ctx ctx;
    shake256_init(&ctx);
    shake256_absorb(&ctx, input, input_len);
    if (Keccak_HashSqueeze(&ctx, output, output_len * 8) != 0) {
        abort();
    }
#endif
}

extern void cshake128_init(cshake_ctx *ctx, const unsigned char *customization, const size_t customization_len);
extern void cshake128_absorb(cshake_ctx *ctx, const unsigned char *input, const size_t input_len);
extern void cshake128_squeezeblocks(cshake_ctx *ctx, unsigned char *output, const size_t nr_blocks);
extern void cshake128(unsigned char *output, size_t output_len, const unsigned char *input, const size_t input_len, const unsigned char *customization, const size_t customization_len);

extern void cshake256_init(cshake_ctx *ctx, const unsigned char *customization, const size_t customization_len);
extern void cshake256_absorb(cshake_ctx *ctx, const unsigned char *input, const size_t input_len);
extern void cshake256_squeezeblocks(cshake_ctx *ctx, unsigned char *output, const size_t nr_blocks);
extern void cshake256(unsigned char *output, size_t output_len, const unsigned char *input, const size_t input_len, const unsigned char *customization, const size_t customization_len);
