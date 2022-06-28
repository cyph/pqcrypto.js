/*
 * Copyright (c) 2018, Koninklijke Philips N.V.
 */

/**
 * @file
 * Declaration of the DEM functions used by the Round5 CCA KEM-based encrypt algorithm.
 */

#include "r5_dem.h"
#include "r5_parameter_sets.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>

#include "r5_hash.h"
#include "rng.h"
#include "misc.h"
#include "r5_memory.h"

/*******************************************************************************
 * Public functions
 ******************************************************************************/

int round5_dem(unsigned char *c2, unsigned long long *c2_len, const unsigned char *key, const unsigned char *m, const unsigned long long m_len) {
    int result = 1;
    int len;
    int c2length;
    EVP_CIPHER_CTX *ctx = NULL;
    unsigned char final_key_iv[32 + 12];
    unsigned char tag[16];
    const unsigned char * const iv = final_key_iv + PARAMS_KAPPA_BYTES;

    /* Hash key to obtain final key and IV */
    assert(PARAMS_KAPPA_BYTES == 32 || PARAMS_KAPPA_BYTES == 24 || PARAMS_KAPPA_BYTES == 16);
    hash(final_key_iv, (size_t) (PARAMS_KAPPA_BYTES + 12), key, PARAMS_KAPPA_BYTES, PARAMS_KAPPA_BYTES);

    /* Initialise AES GCM */
    int res = 1;
    switch (PARAMS_KAPPA_BYTES) {
        case 16:
            res = !(ctx = EVP_CIPHER_CTX_new()) || (EVP_EncryptInit_ex(ctx, EVP_aes_128_gcm(), NULL, final_key_iv, iv) != 1);
            break;
        case 24:
            res = !(ctx = EVP_CIPHER_CTX_new()) || (EVP_EncryptInit_ex(ctx, EVP_aes_192_gcm(), NULL, final_key_iv, iv) != 1);
            break;
        case 32:
            res = !(ctx = EVP_CIPHER_CTX_new()) || (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, final_key_iv, iv) != 1);
            break;
    }
    if (res) {
        goto done_dem;
    }
    EVP_CIPHER_CTX_set_padding(ctx, 0); /* Disable padding */

    /* Encrypt message into c2 */
    if (EVP_EncryptUpdate(ctx, c2, &len, m, (int) m_len) != 1) {
        goto done_dem;
    }
    c2length = len;

    /* Finalise encrypt */
    if (EVP_EncryptFinal_ex(ctx, c2 + c2length, &len) != 1) {
        goto done_dem;
    }
    c2length += len;

    /* Get tag */
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag) != 1) {
        goto done_dem;
    }

    /* Append tag and IV */
    memcpy(c2 + c2length, tag, 16);
    c2length += 16;

    /* Set total length */
    *c2_len = (unsigned long long) c2length;

    /* All OK */
    result = 0;

done_dem:
    EVP_CIPHER_CTX_free(ctx);

    return result;
}

int round5_dem_inverse(unsigned char *m, unsigned long long *m_len, const unsigned char *key, const unsigned char *c2, const unsigned long long c2_len) {
    int result = 1;
    int len;
    int m_length;
    EVP_CIPHER_CTX *ctx = NULL;
    unsigned char final_key_iv[32 + 12];
    unsigned char tag[16];
    const unsigned long long c2_len_no_tag = c2_len - 16U;
    const unsigned char * const iv = final_key_iv + PARAMS_KAPPA_BYTES;

    /* Check length, must at least be as long as the tag (16 bytes).
     * Note that this is should already have been checked when calling this
     * function, so this is just an additional sanity check. */
    if (c2_len < 16) {
        *m_len = 0;
        goto done_dem_inverse;
    }

    /* Hash key to obtain final key and IV */
    assert(PARAMS_KAPPA_BYTES == 32 || PARAMS_KAPPA_BYTES == 24 || PARAMS_KAPPA_BYTES == 16);
    hash(final_key_iv, (size_t) (PARAMS_KAPPA_BYTES + 12), key, PARAMS_KAPPA_BYTES, PARAMS_KAPPA_BYTES);

    /* Get tag */
    memcpy(tag, c2 + c2_len_no_tag, 16);

    /* Initialise AES GCM */
    int res = 1;
    switch (PARAMS_KAPPA_BYTES) {
        case 16:
            res = !(ctx = EVP_CIPHER_CTX_new()) || (EVP_DecryptInit_ex(ctx, EVP_aes_128_gcm(), NULL, final_key_iv, iv) != 1);
            break;
        case 24:
            res = !(ctx = EVP_CIPHER_CTX_new()) || (EVP_DecryptInit_ex(ctx, EVP_aes_192_gcm(), NULL, final_key_iv, iv) != 1);
            break;
        case 32:
            res = !(ctx = EVP_CIPHER_CTX_new()) || (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, final_key_iv, iv) != 1);
            break;
    }
    if (res) {
        abort();
    }
    EVP_CIPHER_CTX_set_padding(ctx, 0); /* Disable padding */

    /* Decrypt */
    unsigned char * tmp_m = m;
    ptrdiff_t diff = m - c2;
    if ((diff >= 0 && diff < (ptrdiff_t) c2_len_no_tag) || (diff < 0 && diff > -((ptrdiff_t) c2_len_no_tag))) {
        /* EVP_DecryptUpdate does not handle overlapping pointers so we need
           to create a temporary buffer for the decrypted message. */
        tmp_m = checked_malloc(c2_len_no_tag);
    }
    if (EVP_DecryptUpdate(ctx, tmp_m, &len, c2, (int) c2_len_no_tag) != 1) {
        goto done_dem_inverse;
    }
    if (tmp_m != m) {
        /* Copy temporary message to result message, free temp message buffer */
        memcpy(m, tmp_m, (size_t) len);
        free(tmp_m);
    }
    m_length = len;

    /* Set expected tag value  */
    if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16, tag)) {
        goto done_dem_inverse;
    }

    /* Finalise decrypt */
    int ret = EVP_DecryptFinal_ex(ctx, m + m_length, &len);
    if (ret < 0) {
        goto done_dem_inverse;
    }

    /* Set decrypted message length */
    *m_len = (unsigned long long) m_length;

    /* OK */
    result = 0;

done_dem_inverse:
    EVP_CIPHER_CTX_free(ctx);

    return result;
}
