// kem.c
// 2017-09-23  Markku-Juhani O. Saarinen <mjos@iki.fi>

// Slow (but compact and readable) reference implementation of HILA5 v 1.0.

// --------------------------------------------------------------------------
//  THIS SIMPLE REFERENCE IMPLEMENTATION IS NOT INTENDED FOR PRODUCTION USE.
//  USE THE OPTIMIZED IMPLEMENTATION INSTEAD - IT IS FASTER AND MORE SECURE.
// --------------------------------------------------------------------------

#include <stdint.h>
#include <string.h>

#include "hila5_sha3.h"
#include "hila5_endian.h"
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

// == Rings and Number Theoratic Transforms ==================================

static int32_t pow1945[2048];           // powers of g=1945 mod q
static int pow1945_ok = 0;              // true after initialization

// make sure that the pow1945[] table is initialized

static void init_pow1945()
{
    if (pow1945_ok)                     // nothing to do then
        return;

    int x = 1;                          // 1945^0 = 1
    for (int i = 0; i < 2048; i++) {    // 1945^1024 = -1 (mod q)
        pow1945[i] = x;
        x = (1945 * x) % HILA5_Q;       // consecutive powers
    }
    pow1945_ok = !0;                    // table now ok
}

// Scalar multiplication: v = c * v.

static void slow_smul(int32_t v[HILA5_N], int32_t c)
{
    for (int i = 0; i < HILA5_N; i++)
        v[i] = (c * v[i]) % HILA5_Q;
}

// Pointwise multiplication: d = a (*) b.

static void slow_vmul(int32_t d[HILA5_N],
    const int32_t a[HILA5_N], const int32_t b[HILA5_N])
{
    for (int i = 0; i < HILA5_N; i++)
        d[i] = (a[i] * b[i]) % HILA5_Q;
}

// Vector addition: d = a + b.

static void slow_vadd(int32_t d[HILA5_N],
    const int32_t a[HILA5_N], const int32_t b[HILA5_N])
{
    for (int i = 0; i < HILA5_N; i++)
        d[i] = (a[i] + b[i]) % HILA5_Q;
}

// reverse order of ten bits i.e. 0x200 -> 0x001 and vice versa

static int32_t bitrev10(int32_t x)
{
    int t;

    x &= 0x3FF;                         // 9876543210 original order
    x = (x << 5) | (x >> 5);            // 4321098765 5/5 bit swap
    t = (x ^ (x >> 4)) & 0x021;
    x ^= t ^ (t << 4);                  // 0321458769 outer bit swap
    t = (x ^ (x >> 2)) & 0x042;
    x ^= t ^ (t << 2);                  // 0123456789 inner bit swap

    return x & 0x3FF;
}

// Slow polynomial ring multiplication: d = a * b  (mod x^1024 + 1)

void slow_rmul(int32_t d[HILA5_N],
    const int32_t a[HILA5_N], const int32_t b[HILA5_N])
{
    int32_t x;

    for (int i = 0; i < HILA5_N; i++) {
        x = 0;
        for (int j = 0; j <= i; j++)            // positive side
            x = (x + a[j] * b[i - j]) % HILA5_Q;
        for (int j = i + 1; j < HILA5_N; j++)   // negative wraparound
            x = (x - a[j] * b[HILA5_N + i - j]) % HILA5_Q;
        // Force into positive [0, q-1] range ("constant time" masking)
        d[i] = x + (-((x >> 31) & 1) & HILA5_Q);
    }
}

// Slow number theoretic transform and scaling: d = c * NTT(v).

static void slow_ntt(int32_t d[HILA5_N], const int32_t v[HILA5_N], int32_t c)
{
    int k, r;
    int32_t x;

    for (int i = 0; i < HILA5_N; i++) {
        r = 2 * bitrev10(i) + 1;        // bit reverse index
        x = 0;
        k = 0;
        for (int j = 0; j < HILA5_N; j++) {
            x = (x + v[j] * pow1945[k]) % HILA5_Q;
            k = (k + r) & 0x7FF;        // k = (j * r) % 2048 next round
        }
        d[i] = (c * x) % HILA5_Q;       // multiply with scalar c
    }
}

// Slow inverse number theoretic transform: d = NTT^-1(v).

static void slow_intt(int32_t d[HILA5_N], const int32_t v[HILA5_N])
{
    int k, r;

    for (int i = 0; i < HILA5_N; i++)   // zeroise d[]
        d[i] = 0;
    for (int i = 0; i < HILA5_N; i++) {
        r = 2 * bitrev10(i) + 1;        // reverse index
        k = 0;
        for (int j = 0; j < HILA5_N; j++) {
            d[j] = (d[j] + v[i] * pow1945[k]) % HILA5_Q;
            k = (k - r) & 0x7FF;        // inverses are negative
        }
    }
}

// == Encoding and Decoding of Ring Polynomials ==============================

// 14-bit packing; mod q integer vector v[1024] to byte sequence d[1792]

static void hila5_pack14(uint8_t d[HILA5_PACKED14], const int32_t v[HILA5_N])
{
    uint32_t x, y;

    for (int i = 0, j = 0; i < HILA5_N;) {
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
    uint32_t x;

    for (int i = 0, j = 0; i < HILA5_N;) {
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


// == Random Samplers =======================================================

// generate n uniform samples from the seed

static void hila5_parse(int32_t v[HILA5_N],
                        const uint8_t seed[HILA5_SEED_LEN])
{
    hila5_sha3_ctx_t sha3;              // init SHA3 state for SHAKE-256
    uint8_t buf[2];                     // two byte output buffer
    uint32_t x;                          // random variable

    hila5_shake256_init(&sha3);         // initialize the context
    hila5_shake_update(&sha3, seed, HILA5_SEED_LEN);    // seed input
    hila5_shake_xof(&sha3);             // pad context to output mode

    // fill the vector with uniform samples
    for (int i = 0; i < HILA5_N; i++) {
        do {                            // rejection sampler
            hila5_shake_out(&sha3, buf, 2); // two bytes from SHAKE-256
            x = ((uint32_t) buf[0]) + (((uint32_t) buf[1]) << 8); // endianess
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
        x &= 0x0001000100010001llu;       // four parallel
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


// == Generate a keypair =====================================================

int crypto_kem_keypair( uint8_t *pk,    // HILA5_PUBKEY_LEN = 1824
                        uint8_t *sk)    // HILA5_PRIVKEY_LEN = 1824
{
    int32_t a[HILA5_N], e[HILA5_N], t[HILA5_N];

    init_pow1945();                     // make sure initialized

    // Create secret key
    hila5_psi16(t);                     // (t is a temporary variable)
    slow_ntt(a, t, 27);                 // a = 3**3 * NTT(Psi_16)

    // Public key
    hila5_psi16(t);                     // t = Psi_16
    slow_ntt(e, t, 27);                 // e = 3**3 * NTT(Psi_16) -- noise
    randombytes(pk, HILA5_SEED_LEN);    // Random seed for g
    hila5_parse(t, pk);                 // (t =) g = parse(seed)
    slow_vmul(t, a, t);
    slow_vadd(t, t, e);                 // A = NTT(g * a + e)
    hila5_pack14(pk + HILA5_SEED_LEN, t);   // pk = seed | A

    hila5_pack14(sk, a);                // pack secret key
    // SHA3 hash of pubic key is stored with secret key due to API limitation
    hila5_sha3(pk, HILA5_PUBKEY_LEN, sk + HILA5_PACKED14, 32);

    return 0;                           // SUCCESS
}


// == Encapsulation ==========================================================

// Create a bit selector, reconciliation bits, and payload;
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
                return 0;               // SUCCESS: enough bits
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
    int32_t a[HILA5_N], b[HILA5_N], e[HILA5_N], g[HILA5_N], t[HILA5_N];
    uint64_t z[8];
    uint8_t hash[32];
    hila5_sha3_ctx_t sha3;

    init_pow1945();                     // make sure initialized

    hila5_unpack14(a, pk + HILA5_SEED_LEN); // decode A = public key

    for (i = 0; i < HILA5_MAX_ITER; i++) {

        hila5_psi16(t);                 // recipients' ephemeral secret
        slow_ntt(b, t, 27);             // b = 3**3 NTT(Psi_16)
        slow_vmul(e, a, b);
        slow_intt(t, e);                // t = a * b  (approx. share "y")
        slow_smul(t, 1416);             // scale by 1416 = 1 / (3**6 * 1024)

        // Safe bits -- may fail (with about 1% probability);
        memset(z, 0, sizeof(z));        // ct = .. | sel | rec, z = payload
        if (hila5_safebits(ct + HILA5_PACKED14, //
            ct + HILA5_PACKED14 + HILA5_PACKED1, (uint8_t *) z, t) == 0)
            break;
    }
    if (i == HILA5_MAX_ITER)            // FAIL: too many repeats
        return -1;

    HILA5_ENDIAN_FLIP64(z, 8);
    xe5_cod(&z[4], z);                  // create linear error correction code
    HILA5_ENDIAN_FLIP64(z, 8);

    memcpy(ct + HILA5_PACKED14 + HILA5_PACKED1 + HILA5_PAYLOAD_LEN,
        &z[4], HILA5_ECC_LEN);          // ct = .. | encrypted error cor. code

    // Construct ciphertext
    hila5_parse(g, pk);                 // g = Parse(seed)
    hila5_psi16(t);                     // noise error
    slow_ntt(e, t, 27);                 // e = 3**3 * NTT(Psi_16)
    slow_vmul(t, g, b);                 // t = NTT(g * b)
    slow_vadd(t, t, e);                 // t = NTT(g * b + e)
    hila5_pack14(ct, t);                // public value in ct

    hila5_sha3_init(&sha3, HILA5_KEY_LEN);          // final hash
    hila5_sha3_update(&sha3, "HILA5v10", 8);        // version ident
    hila5_sha3(pk, HILA5_PUBKEY_LEN, hash, 32);     // SHA3(pk)
    hila5_sha3_update(&sha3, hash, 32);
    hila5_sha3(ct, HILA5_CIPHERTEXT_LEN, hash, 32); // SHA3(ct)
    hila5_sha3_update(&sha3, hash, 32);
    hila5_sha3_update(&sha3, z, HILA5_KEY_LEN);     // actual shared secret z
    hila5_sha3_final(ss, &sha3);                    // hash out to ss

    return 0;                           // SUCCESS
}


// == Decapsulation ==========================================================

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
                return 0;               // SUCCESS: got full payload
        }
    }

    return j;                           // FAIL: not enough bits
}

// Decapsulate

int crypto_kem_dec( uint8_t *ss,        // HILA5_KEY_LEN = 32
                    const uint8_t *ct,  // HILA5_CIPHERTEXT_LEN = 2012
                    const uint8_t *sk)  // HILA5_PRIVKEY_LEN = 1824
{
    int32_t a[HILA5_N], b[HILA5_N];
    uint64_t z[8];
    uint8_t ct_hash[32];
    hila5_sha3_ctx_t sha3;

    init_pow1945();                     // make sure initialized

    hila5_unpack14(a, sk);              // unpack secret key
    hila5_unpack14(b, ct);              // get B from ciphertext
    slow_vmul(a, a, b);                 // a * B
    slow_intt(b, a);                    // shared secret ("x") in b
    slow_smul(b, 1416);                 // scale by 1416 = (3^6 * 1024)^-1

    memset(z, 0x00, sizeof(z));
    if (hila5_select((uint8_t *) z,     // reconciliation
        ct + HILA5_PACKED14, ct + HILA5_PACKED14 + HILA5_PACKED1, b))
        return -1;                      // FAIL: not enough bits

    // error correction -- decrypt with "one time pad" in payload
    for (int i = 0; i < HILA5_ECC_LEN; i++) {
        ((uint8_t *) &z[4])[i] ^=
            ct[HILA5_PACKED14 + HILA5_PACKED1 + HILA5_PAYLOAD_LEN + i];
    }
    HILA5_ENDIAN_FLIP64(z, 8);
    xe5_cod(&z[4], z);                  // linear code
    xe5_fix(z, &z[4]);                  // fix possible errors
    HILA5_ENDIAN_FLIP64(z, 8);

    hila5_sha3_init(&sha3, HILA5_KEY_LEN);              // final hash
    hila5_sha3_update(&sha3, "HILA5v10", 8);            // version identifier
    hila5_sha3_update(&sha3, sk + HILA5_PACKED14, 32);  // SHA3(pk)
    hila5_sha3(ct, HILA5_CIPHERTEXT_LEN, ct_hash, 32);  // hash the ciphertext
    hila5_sha3_update(&sha3, ct_hash, 32);              // SHA3(ct)
    hila5_sha3_update(&sha3, z, HILA5_KEY_LEN);         // shared secret
    hila5_sha3_final(ss, &sha3);

    return 0;                           // SUCCESS
}

// == END ====================================================================

