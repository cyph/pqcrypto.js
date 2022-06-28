#include "crypto_kem.h"
/*
 * Copyright (c) 2018, PQShield and Koninklijke Philips N.V.
 * Markku-Juhani O. Saarinen, Koninklijke Philips N.V.
 */

/**
 * @file
 * Implementation of the CPA KEM functions (NIST api).
 */

#include "cpa_kem.h"

#if CRYPTO_CIPHERTEXTBYTES != 0

extern int crypto_kem_keypair(unsigned char *pk, unsigned char *sk);
extern int crypto_kem_enc(unsigned char *ct, unsigned char *k, const unsigned char *pk);
extern int crypto_kem_dec(unsigned char *k, const unsigned char *ct, const unsigned char *sk);

#endif
