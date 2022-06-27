// kem.c
// 2017-09-09  Markku-Juhani O. Saarinen <mjos@iki.fi>

// Optimized C99 implementation of HILA5 KEM.

#include <stdint.h>
#include <string.h>

#include "hila5_sha3.h"
#include "hila5_endian.h"
#include "ms_priv.h"
#include "crypto_kem.h"

// Parameters

#define HILA5_N                 1024
#define HILA5_Q                 12289
#define HILA5_B                 799
#define HILA5_MAX_ITER          100
#define HILA5_SEED_LEN          32
#define HILA5_KEY_LEN           32
#define HILA5_ECC_LEN           30
#define HILA5_PACKED1           (HILA5_N / 8)
#define HILA5_PACKED14          (14 * HILA5_N / 8)
#define HILA5_PAYLOAD_LEN       (HILA5_KEY_LEN + HILA5_ECC_LEN)
#define HILA5_PUBKEY_LEN        (HILA5_SEED_LEN + HILA5_PACKED14)
#define HILA5_PRIVKEY_LEN       (HILA5_PACKED14 + 32)
#define HILA5_CIPHERTEXT_LEN    (HILA5_PACKED14 + HILA5_PACKED1 + \
                                HILA5_PAYLOAD_LEN + HILA5_ECC_LEN)

// Prototype here so that we wouldn't have to find rng.h
int randombytes(unsigned char *x, unsigned long long xlen);


// == Encoding and Decoding of Ring Polynomials ==============================

// 14-bit packing; mod q integer vector v[1024] to byte sequence d[1792]

static void hila5_pack14(uint8_t d[HILA5_PACKED14],
    const int32_t v[HILA5_N])
{
    int i, j, x, y;

    for (i = 0, j = 0; i < HILA5_N;) {
        x = v[i++];
        d[j++] = x;
        y = v[i++];
        d[j++] = (x >> 8) | (y << 6);
        d[j++] = y >> 2;
        x = v[i++];
        d[j++] = (y >> 10) | (x << 4);
        d[j++] = x >> 4;
        y = v[i++];
        d[j++] = (x >> 12) | (y << 2);
        d[j++] = y >> 6;
    }
}

// 14-bit unpacking; bytes in d[1792] to integer vector v[1024]

static void hila5_unpack14(int32_t v[HILA5_N],
    const uint8_t d[HILA5_PACKED14])
{
    int i, j;
    uint32_t x;

    for (i = 0, j = 0; i < HILA5_N;) {
        x = d[j++];
        x |= (((uint32_t) d[j++]) << 8);
        v[i++] = x & 0x3FFF;
        x >>= 14;
        x |= (((uint32_t) d[j++]) << 2);
        x |= (((uint32_t) d[j++]) << 10);
        v[i++] = x & 0x3FFF;
        x >>= 14;
        x |= (((uint32_t) d[j++]) << 4);
        x |= (((uint32_t) d[j++]) << 12);
        v[i++] = x & 0x3FFF;
        x >>= 14;
        x |= (((uint32_t) d[j++]) << 6);
        v[i++] = x;
    }
}

// == Samplers ===============================================================

// generate n uniform samples from the seed

static void hila5_parse(int32_t v[HILA5_N],
                        const uint8_t seed[HILA5_SEED_LEN])
{
    hila5_sha3_ctx_t sha3;              // init SHA3 state for SHAKE-256
    uint8_t buf[2];                     // two byte output buffer
    int32_t x;                          // random variable

    hila5_shake256_init(&sha3);         // initialize the context
    hila5_shake_update(&sha3, seed, HILA5_SEED_LEN);    // seed input
    hila5_shake_xof(&sha3);             // pad context to output mode

    // fill the vector with uniform samples
    for (int i = 0; i < HILA5_N; i++) {
        do {                            // rejection sampler
            hila5_shake_out(&sha3, buf, 2); // two bytes from SHAKE-256
            x = ((int32_t) buf[0]) + (((int32_t) buf[1]) << 8); // endianess
        } while (x >= 5 * HILA5_Q);     // reject
        v[i] = x;                       // reduction (mod q) unnecessary
    }
}

// sample a vector of values from the psi16 distribution

static void hila5_psi16(int32_t v[HILA5_N])
{
    uint32_t x = 0;                     // 32-bit variable

    for (int i = 0; i < HILA5_N; i++) {

        randombytes((unsigned char *) &x, sizeof(x));   // get 4 random bytes

        x -= (x >> 1) & 0x55555555;     // Hamming weight
        x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
        x = (x + (x >> 4)) & 0x0F0F0F0F;
        x += x >> 8;
        x = (x + (x >> 16)) & 0x3F;

        x -= 16;                        // Make signed in range [0, q-1]
        v[i] = x + (-((x >> 31) & 1) & HILA5_Q);    // "constant time"
    }
}

// == Error Correction Code XE5 ==============================================

//  Field       subcodeword:     r0  r1  r2  r3  r4  r5  r6  r7  r8  r9 (end)
//  lengths.    bit offset:      0   16  32  49  80  99  128 151 176 203 240
static const int xe5_len[10] = { 16, 16, 17, 31, 19, 29, 23, 25, 27, 37 };

// Compute redundancy r[] (XOR over original) from data d[]

static void xe5_cod(uint64_t r[4], const uint64_t d[4])
{
    int i, j, l;
    uint64_t x, t, ri[10];

    for (i = 0; i < 10; i++)            // initialize
        ri[i] = 0;

    for (i = 3; i >= 0; i--) {          // four words
        x = d[i];                       // payload
        for (j = 1; j < 10; j++) {
            l = xe5_len[j];             // length
            t = (ri[j] << (64 % l));    // rotate
            t ^= x;                     // payload
            if (l < 32)                 // extra fold
                t ^= t >> (2 * l);
            t ^= t >> l;                // fold
            ri[j] = t & ((1llu << l) - 1);   // mask
        }
        x ^= x >> 8;                    // parity of 16
        x ^= x >> 4;
        x ^= x >> 2;
        x ^= x >> 1;
        x &= 0x0001000100010001;        // four parallel
        x ^= (x >> (16 - 1)) ^ (x >> (32 - 2)) ^ (x >> (48 - 3));
        ri[0] |= (x & 0xF) << (4 * i);
    }
    // pack coefficients into 240 bits (note output the XOR)
    r[0] ^= ri[0] ^ (ri[1] << 16) ^ (ri[2] << 32) ^ (ri[3] << 49);
    r[1] ^= (ri[3] >> 15) ^ (ri[4] << 16) ^ (ri[5] << 35);
    r[2] ^= ri[6] ^ (ri[7] << 23) ^ (ri[8] << 48);
    r[3] ^= (ri[8] >> 16) ^ (ri[9] << 11);
}

// Fix errors in data d[] using redundancy in r[]

static void xe5_fix(uint64_t d[4], const uint64_t r[4])
{
    int i, j, k, l;
    uint64_t x, t, ri[10];

    ri[0] = r[0];                       // unpack
    ri[1] = r[0] >> 16;
    ri[2] = r[0] >> 32;
    ri[3] = (r[0] >> 49) ^ (r[1] << 15);
    ri[4] = r[1] >> 16;
    ri[5] = r[1] >> 35;
    ri[6] = r[2];
    ri[7] = r[2] >> 23;
    ri[8] = (r[2] >> 48) ^ (r[3] << 16);
    ri[9] = r[3] >> 11;

    for (i = 0; i < 4; i++) {           // four words
        for (j = 1; j < 10; j++) {
            l = xe5_len[j];             // length
            x = ri[j] & ((1llu << l) - 1);   // mask
            x |= x << l;                // expand
            if (l < 32)                 // extra unfold
                x |= (x << (2 * l));
            ri[j] = x;                  // store it
        }
        x = (ri[0] >> (4 * i)) & 0xF;   // parity mask for ri[0]
        x ^= (x << (16 - 1)) ^ (x << (32 - 2)) ^ (x << (48 - 3));
        x  = 0x0100010001000100 - (x & 0x0001000100010001);
        x &= 0x00FF00FF00FF00FF;
        x |= x << 8;

        for (j = 0; j < 4; j++) {       // threshold sum
            t = (x >> j) & 0x1111111111111111;
            for (k = 1; k < 10; k++)
                t += (ri[k] >> j) & 0x1111111111111111;
            // threshold 6 -- add 2 to weight and take bit number 3
            t = ((t + 0x2222222222222222) >> 3) & 0x1111111111111111;
            d[i] ^= t << j;             // fix bits
        }
        if (i < 3) {                    // rotate if not last
            for (j = 1; j < 10; j++)
                ri[j] >>= 64 % xe5_len[j];
        }
    }
}

// === Main functionality ====================================================

// key generation

int crypto_kem_keypair( uint8_t *pk,    // HILA5_PUBKEY_LEN = 1824
                        uint8_t *sk)    // HILA5_PRIVKEY_LEN = 1824
{
    int32_t a[HILA5_N], e[HILA5_N], t[HILA5_N];

    // Secret key
    hila5_psi16(a);                         // a = NTT(Psi_16)
    mslc_ntt(a, mslc_psi_rev_ntt1024, HILA5_N);

    // Public key
    hila5_psi16(e);                         // e = NTT(Psi_16)
    mslc_ntt(e, mslc_psi_rev_ntt1024, HILA5_N);
    randombytes(pk, HILA5_SEED_LEN);        // Random seed for g (=t)
    hila5_parse(t, pk);                     // g = Parse(seed);
    mslc_pmuladd(t, a, e, t, HILA5_N);      // t = NTT(g * a + e)
    mslc_correction(t, HILA5_Q, HILA5_N);
    hila5_pack14(pk + HILA5_SEED_LEN, t);   // pk = seed | A

    // Pack private key: sk = a | SHA3(pk)
    // Hash of pubic key is stored with secret key due to API limitation.
    mslc_two_reduce12289(a, HILA5_N);
    mslc_correction(a, HILA5_Q, HILA5_N);
    hila5_pack14(sk, a);
    hila5_sha3(pk, HILA5_PUBKEY_LEN, sk + HILA5_PACKED14, 32);

    // Try to clear out sensitive data
    memset(t, 0x00, sizeof(t));
    memset(e, 0x00, sizeof(e));
    memset(a, 0x00, sizeof(a));

    return 0;
}


// create a bit selector, reconciliation bits, and payload;
// return nonzero on failure

static int hila5_safebits(uint8_t sel[HILA5_PACKED1],
    uint8_t rec[HILA5_PAYLOAD_LEN],
    uint8_t pld[HILA5_PAYLOAD_LEN],
    const int32_t v[HILA5_N])
{
    int i, j, x;

    memset(sel, 0, HILA5_PACKED1);      // selector array
    memset(rec, 0, HILA5_PAYLOAD_LEN);  // reconciliation bits for payload
    memset(pld, 0, HILA5_PAYLOAD_LEN);  // the actual payload XOR mask

    j = 0;                              // reset the bit counter
    for (i = 0; i < HILA5_N; i++) {     // scan for "safe bits"
        // x in { [737, 2335] U [3809, 5407] U [6881, 8479] U [9953, 11551] }
        x = v[i] % (HILA5_Q / 4);
        if (x >= ((HILA5_Q / 8) - HILA5_B) &&
            x <= ((HILA5_Q / 8) + HILA5_B)) {
                                        // set selector bit
            sel[i >> 3] |= 1 << (i & 7);
            x = (4 * v[i]) / HILA5_Q;   // reconciliation bits
            rec[j >> 3] ^= (x & 1) << (j & 7);
            x >>= 1;                    // payload bits
            pld[j >> 3] ^= (x & 1) << (j & 7);
            j++;                        // payload bit count
            if (j >= 8 * HILA5_PAYLOAD_LEN)
                return 0;               // success: enough bits
        }
    }
    return j;                           // FAIL: not enough bits
}

// Encapsulate

int crypto_kem_enc( uint8_t *ct,        // HILA5_CIPHERTEXT_LEN = 2012
                    uint8_t *ss,        // HILA5_KEY_LEN = 32
                    const uint8_t *pk)  // HILA5_PUBKEY_LEN = 1824
{
    int i;
    int32_t a[HILA5_N], b[HILA5_N], t[HILA5_N], e[HILA5_N];
    uint64_t z[8];
    uint8_t hash[32];
    hila5_sha3_ctx_t sha3;

    // Get A
    hila5_unpack14(a, pk + HILA5_SEED_LEN);

    for (i = 0; i < HILA5_MAX_ITER; i++) {

        // Ephemeral secret
        hila5_psi16(t);                     // t = NTT(Psi_16)
        mslc_ntt(t, mslc_psi_rev_ntt1024, HILA5_N);
        mslc_pmul(a, t, b, HILA5_N);        // b = a * t
        // 8281 = sqrt(-1) * 2^-10 * 3^-10, 7755 = 2^-10 * 3^-10
        mslc_intt(b, mslc_inv_rev_ntt1024, 8281, 7755, HILA5_N);
        mslc_two_reduce12289(b, HILA5_N);
        mslc_correction(b, HILA5_Q, HILA5_N);

        memset(z, 0, sizeof(z));

        // safe bits -- may fail (with about 1% probability);
        if (hila5_safebits(ct + HILA5_PACKED14,
            ct + HILA5_PACKED14 + HILA5_PACKED1, (uint8_t *) z, b) == 0)
            break;
    }
    if (i == HILA5_MAX_ITER)
        return -1;

    HILA5_ENDIAN_FLIP64(z, 8);
    xe5_cod(&z[4], z);                      // error correction
    HILA5_ENDIAN_FLIP64(z, 8);

    memcpy(ct + HILA5_PACKED14 + HILA5_PACKED1 + HILA5_PAYLOAD_LEN,
        &z[4], HILA5_ECC_LEN);

    hila5_parse(a, pk);                     // Construct ciphertext
    hila5_psi16(e);
    mslc_ntt(e, mslc_psi_rev_ntt1024, HILA5_N);
    mslc_pmuladd(a, t, e, a, HILA5_N);      // a = NTT(g * b + e)
    mslc_correction(a, HILA5_Q, HILA5_N);

    hila5_pack14(ct, a);                    // public value in ct

    hila5_sha3_init(&sha3, HILA5_KEY_LEN);          // final hash
    hila5_sha3_update(&sha3, "HILA5v10", 8);        // version ident
    hila5_sha3(pk, HILA5_PUBKEY_LEN, hash, 32);     // SHA3(pk)
    hila5_sha3_update(&sha3, hash, 32);
    hila5_sha3(ct, HILA5_CIPHERTEXT_LEN, hash, 32); // SHA3(ct)
    hila5_sha3_update(&sha3, hash, 32);
    hila5_sha3_update(&sha3, z, HILA5_KEY_LEN);     // shared secret z

    hila5_sha3_final(ss, &sha3);

    // clear sensitive data
    hila5_sha3_init(&sha3, 0);
    memset(hash, 0x00, sizeof(hash));
    memset(a, 0x00, sizeof(a));
    memset(b, 0x00, sizeof(b));
    memset(t, 0x00, sizeof(t));
    memset(e, 0x00, sizeof(e));
    memset(z, 0x00, sizeof(z));

    return 0;
}

// Decode selected key bits. Return nonzero on failure.

static int hila5_select(uint8_t pld[HILA5_PAYLOAD_LEN],
    const uint8_t sel[HILA5_PACKED1],
    const uint8_t rec[HILA5_PAYLOAD_LEN],
    const int32_t v[HILA5_N])
{
    int i, j, x;

    memset(pld, 0, HILA5_PAYLOAD_LEN);  // set payload to all zeros

    j = 0;
    for (i = 0; i < HILA5_N; i++) {     // scan for selected bits
        if ((sel[i >> 3] >> (i & 7)) & 1) {
            x = v[i] + HILA5_Q / 8;     // reconciliation:
            x -= -((rec[j >> 3] >> (j & 7)) & 1) &
                    (HILA5_Q / 4);      // "90 degrees" if rec bit set
            x = ((2 * ((x + HILA5_Q) % HILA5_Q)) / HILA5_Q);
            pld[j >> 3] ^= (x & 1) << (j & 7);
            j++;
            if (j >= 8 * HILA5_PAYLOAD_LEN)
                return 0;               // got full payload
        }
    }

    return j;                           // FAIL: not enough bits
}

// Decapsulate

int crypto_kem_dec( uint8_t *ss,        // HILA5_KEY_LEN = 32
                    const uint8_t *ct,  // HILA5_CIPHERTEXT_LEN = 2012
                    const uint8_t *sk)  // HILA5_PRIVKEY_LEN = 1824
{
    int i;
    int32_t c[HILA5_N], s[HILA5_N];
    uint64_t z[8];
    uint8_t hct[32];
    hila5_sha3_ctx_t sha3;

    hila5_unpack14(s, sk);                  // unpack secret key
    hila5_unpack14(c, ct);                  // get c
    mslc_pmul(c, s, c, HILA5_N);

    // scaling factors
    // 3651 = sqrt(-1) * 2^-10 * 3^-12
    // 4958 = 2^-10 * 3^-12
    mslc_intt(c, mslc_inv_rev_ntt1024, 3651, 4958, HILA5_N);
    mslc_two_reduce12289(c, HILA5_N);
    mslc_correction(c, HILA5_Q, HILA5_N);

    memset(z, 0x00, sizeof(z));
    if (hila5_select((uint8_t *) z,         // reconciliation
        ct + HILA5_PACKED14,
        ct + HILA5_PACKED14 + HILA5_PACKED1, c))
        return -2;
    for (i = 0; i < HILA5_ECC_LEN; i++) {   // error correction
        ((uint8_t *) &z[4])[i] ^=
            ct[HILA5_PACKED14 + HILA5_PACKED1 + HILA5_PAYLOAD_LEN + i];
    }
    HILA5_ENDIAN_FLIP64(z, 8);
    xe5_cod(&z[4], z);
    xe5_fix(z, &z[4]);
    HILA5_ENDIAN_FLIP64(z, 8);

    // hash the ciphertext
    hila5_sha3(ct, HILA5_CIPHERTEXT_LEN, hct, 32);

    hila5_sha3_init(&sha3, HILA5_KEY_LEN);              // final hash
    hila5_sha3_update(&sha3, "HILA5v10", 8);            // version identifier
    hila5_sha3_update(&sha3, sk + HILA5_PACKED14, 32);  // SHA3(pk)
    hila5_sha3_update(&sha3, hct, 32);                  // SHA3(ct)
    hila5_sha3_update(&sha3, z, HILA5_KEY_LEN);         // shared secret
    hila5_sha3_final(ss, &sha3);

    // clear sensitive data
    hila5_sha3_init(&sha3, 0);
    memset(hct, 0x00, sizeof(hct));
    memset(c, 0x00, sizeof(c));
    memset(s, 0x00, sizeof(z));
    memset(z, 0x00, sizeof(z));

    return 0;
}

