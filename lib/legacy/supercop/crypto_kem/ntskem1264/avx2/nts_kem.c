/**
 *  nts_kem.c
 *  NTS-KEM
 *
 *  Parameter: NTS-KEM(12, 64)
 *  Platform: AVX2
 *
 *  This file is part of the additional implemention of NTS-KEM
 *  submitted as part of NIST Post-Quantum Cryptography
 *  Standardization Process.
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "nts_kem.h"
#include "ff.h"
#include "bits.h"
#include "matrix_ff2.h"
#include "polynomial.h"
#include "random.h"
#include "keccak.h"
#include "nts_kem_params.h"
#include "nts_kem_errors.h"
#include "vector_utils.h"
#include "bitslice_bma_128.h"
#include "bitslice_fft_256.h"
#if defined(BENCHMARK)
#include "utils.h"
#endif

typedef struct {
    uint32_t m;
    FF2m *ff2m;
    ff_unit a[ NTS_KEM_PARAM_BC ];
    ff_unit h[ NTS_KEM_PARAM_BC ];
    ff_unit p[ NTS_KEM_PARAM_N ];
} NTSKEM_private;

static const int kNTSKEMKeysize = NTS_KEM_KEY_SIZE;

#define NTS_KEM_PARAM_A_REM     (((NTS_KEM_PARAM_A - (NTS_KEM_KEY_SIZE << 3)) & MOD) >> 3)
#define UINT64_SIZE             4
#define BLOCK_SIZE              256
#define NTS_KEM_PARAM_BC_VEC    NTS_KEM_PARAM_BC_DIV_256
#define NTS_KEM_PARAM_R_VEC     NTS_KEM_PARAM_R_DIV_256
#define NTS_KEM_PARAM_N_VEC     NTS_KEM_PARAM_N_DIV_256

#define bitslice_fft    bitslice_fft12_256
#define vector_ff_or    vector_ff_or_256

/* Function definitions */
poly* create_random_goppa_polynomial(const FF2m* ff2m, int degree);
matrix_ff2* create_matrix_G(const NTSKEM* nts_kem,
                            const poly* Gz,
                            ff_unit *a,
                            ff_unit *h);
void fisher_yates_shuffle(ff_unit *buffer);
void random_vector(uint32_t tau, uint32_t n, uint8_t *e);
int compute_syndrome(const NTSKEM* nts_kem,
                     const vector *c_ast,
                     ff_unit* s);
void correct_error_and_recover_ke(const uint8_t* e_prime,
                                  const ff_unit* p,
                                  uint8_t *e,
                                  uint8_t *k_e);
void pack_buffer(const uint8_t *src, int src_len, uint8_t *dst);
void unpack_buffer(const uint8_t *src, ff_unit *dst, int dst_len);
int serialise_public_key(NTSKEM* nts_kem, const matrix_ff2* SGP);
int serialise_private_key(NTSKEM *nts_kem);
int deserialise_private_key(NTSKEM* nts_kem, const uint8_t *buf);
void load_input_ciphertext(vector *out, const uint8_t *in);

/**
 *  Initialise an NTS-KEM object with a given parameter
 *
 *  @param[out] nts_kem A pointer of NTSKEM object created
 *  @return NTS_KEM_SUCCESS on success, otherwise a negative error code
 *          {@see nts_kem_errors.h}
 **/
int nts_kem_create(NTSKEM** nts_kem)
{
    int32_t status = NTS_KEM_BAD_MEMORY_ALLOCATION;
    int32_t i;
    ff_unit h[NTS_KEM_PARAM_N];
    ff_unit a[NTS_KEM_PARAM_N];
    poly* Gz = NULL;
    NTSKEM_private *priv = NULL;
    NTSKEM *nts_kem_ptr = NULL;
    matrix_ff2 *Q = NULL;
#if defined(BENCHMARK)
    uint64_t start_clock, end_clock;
#endif
    
    *nts_kem = (NTSKEM *)malloc(sizeof(NTSKEM));
    if (!(*nts_kem))
        goto nts_kem_create_fail;
    
    nts_kem_ptr = *nts_kem;
    nts_kem_ptr->public_key = nts_kem_ptr->private_key = NULL;
    nts_kem_ptr->public_key_size = nts_kem_ptr->private_key_size = 0;
    priv = (NTSKEM_private *)malloc(sizeof(NTSKEM_private));
    if (!priv)
        goto nts_kem_create_fail;
    nts_kem_ptr->priv = priv;
    nts_kem_ptr->length = NTS_KEM_PARAM_N;
    nts_kem_ptr->t = NTS_KEM_PARAM_T;
    priv->m = NTS_KEM_PARAM_M;
    
    /* Initialise finite-field */
    priv->ff2m = ff_create(priv->m);
    if (!priv->ff2m)
        goto nts_kem_create_fail;
    
    /**
     * NTS-KEM Key Generation procudure
     *
     * Step 1. Randomly generate a monic Goppa polynomial G(z) of degree τ
     **/
#if defined(BENCHMARK)
    start_clock = cpucycles();
#endif
    Gz = create_random_goppa_polynomial(priv->ff2m, nts_kem_ptr->t);
    if (!Gz)
        goto nts_kem_create_fail;
#if defined(BENCHMARK)
    end_clock = cpucycles();
    fprintf(stdout, "# KeyGen : (1) random_goppa_polynomial consumes %" PRId64 " cycles\n", end_clock-start_clock);
#endif

    /**
     * Step 2. Randomly generate a permutation vector p of length n,
     *         representing a permutation π_p on the set of n elements
     *
     * Let p = (p_0, p_1, ..., p_{n-1}) and let a sequence
     * a = (a_0, a_1, ..., a_{n-1}), by applying permutation
     * defined by p to vector a, we have the sequence a in
     * the following order: 
     *     a = (a_{p_0}, a_{p_1}, ..., a_{p_{n-1}})
     *
     * Obviously we can also define the permutation vector p
     * as a permutation matrix P. But the vector p is 
     * preferred for storage efficiency.
     *
     * Note that the permutation defined by p may need
     * to be altered, and this is captured by permutation
     * ρ, see create_matrix_G method.
     **/
#if defined(BENCHMARK)
    start_clock = cpucycles();
#endif
    for (i=0; i<NTS_KEM_PARAM_N; i++) {
        priv->p[i] = i;
    }
    fisher_yates_shuffle(priv->p);
#if defined(BENCHMARK)
    end_clock = cpucycles();
    fprintf(stdout, "# KeyGen : (2) random_permutation consumes %" PRId64 " cycles\n", end_clock-start_clock);
#endif
    
    /**
     * Step 3. Construct a generator matrix in the reduced row echelon
     *         form G = [ I_k | Q ] of a permuted code
     **/

#if defined(BENCHMARK)
    start_clock = cpucycles();
#endif
    Q = create_matrix_G(nts_kem_ptr, Gz, a, h);
    if (Q == NULL)
        goto nts_kem_create_fail;
#if defined(BENCHMARK)
    end_clock = cpucycles();
    fprintf(stdout, "# KeyGen : (3) create_matrix_G consumes %" PRId64 " cycles\n", end_clock-start_clock);
#endif
    
    /**
     * Step 4. Partion vectors a = (a_a | a_b | a_c) and h = (h_a | h_b | h_c)
     *         and let a* = (a_b | a_c) and  h* = (h_b | h_c)
     **/
    memcpy(priv->a, &a[NTS_KEM_PARAM_A], NTS_KEM_PARAM_BC*sizeof(ff_unit));
    memcpy(priv->h, &h[NTS_KEM_PARAM_A], NTS_KEM_PARAM_BC*sizeof(ff_unit));

    /**
     * The NTS-KEM public key is (Q, τ, l), where l = kNTSKEMKeysize
     * and NTS-KEM private key is (a*, h*, p)
     *
     * Serialise the public and private key pair
     **/
    if ((NTS_KEM_SUCCESS != serialise_public_key(nts_kem_ptr, Q)) ||
        (NTS_KEM_SUCCESS != serialise_private_key(nts_kem_ptr)))
        goto nts_kem_create_fail;
    
    status = NTS_KEM_SUCCESS;
nts_kem_create_fail:
    if (Gz) {
        zero_poly(Gz);
        free_poly(Gz);
        Gz = NULL;
    }
    free_matrix_ff2(Q);
    if (status != NTS_KEM_SUCCESS) {
        if (nts_kem_ptr) {
            nts_kem_release(nts_kem_ptr);
            nts_kem_ptr = NULL;
        }
    }
    
    return status;
}

/**
 *  Initialise an NTS-KEM object from a buffer containing the private key
 *
 *  @param[out] nts_kem     A pointer of NTSKEM object created
 *  @param[in]  buffer      The buffer containing the private key
 *  @param[in]  buffer_size The size of the private key buffer in bytes
 *  @return NTS_KEM_SUCCESS on success, otherwise a negative error code
 *          {@see nts_kem_errors.h}
 **/
int nts_kem_init_from_private_key(NTSKEM** nts_kem,
                                  const uint8_t *buffer,
                                  size_t buffer_size)
{
    const uint8_t *ptr = buffer;
    int status = NTS_KEM_BAD_MEMORY_ALLOCATION;
    NTSKEM_private *priv = NULL;
    NTSKEM *nts_kem_ptr = NULL;
    
    if (buffer_size != NTS_KEM_PRIVATE_KEY_SIZE) {
        status = NTS_KEM_BAD_PARAMETERS;
        goto nts_kem_init_fail;
    }
    
    *nts_kem = (NTSKEM *)malloc(sizeof(NTSKEM));
    if (!(*nts_kem))
        goto nts_kem_init_fail;
    nts_kem_ptr = *nts_kem;
    nts_kem_ptr->public_key = nts_kem_ptr->private_key = NULL;
    nts_kem_ptr->public_key_size = 0;
    nts_kem_ptr->private_key_size = NTS_KEM_PRIVATE_KEY_SIZE;
    priv = (NTSKEM_private *)malloc(sizeof(NTSKEM_private));
    if (!priv)
        goto nts_kem_init_fail;
    nts_kem_ptr->priv = priv;
    
    /* Read the private-key buffer */
    
    nts_kem_ptr->length = NTS_KEM_PARAM_N;
    nts_kem_ptr->t = NTS_KEM_PARAM_T;
    priv->m = NTS_KEM_PARAM_M;
    
    /* Initialise finite-field */
    priv->ff2m = ff_create(priv->m);
    if (!priv->ff2m)
        goto nts_kem_init_fail;
    
    /* Deserialise the private key blob */
    if (deserialise_private_key(nts_kem_ptr, ptr) != NTS_KEM_SUCCESS) {
        goto nts_kem_init_fail;
    }
    
    status = NTS_KEM_SUCCESS;
nts_kem_init_fail:
    if (status != NTS_KEM_SUCCESS) {
        if (nts_kem_ptr) {
            nts_kem_release(nts_kem_ptr);
            nts_kem_ptr = NULL;
        }
    }
    
    return status;
}

/**
 *  Release the NTS-KEM object
 *
 *  @param[in] nts_kem  A pointer to an NTS-KEM object
 **/
void nts_kem_release(NTSKEM *nts_kem)
{
    NTSKEM_private *priv = NULL;
    
    if (nts_kem) {
        priv = nts_kem->priv;
        if (priv) {
            memset(priv->a, 0, sizeof(priv->a));
            memset(priv->h, 0, sizeof(priv->h));
            memset(priv->p, 0, sizeof(priv->p));
            ff_release(priv->ff2m);
            priv->ff2m = NULL;
            priv->m = 0;
            free(priv);
        }
        nts_kem->priv = NULL;
        
        if (nts_kem->public_key)
            free(nts_kem->public_key);
        if (nts_kem->private_key) {
            memset(nts_kem->private_key, 0, NTS_KEM_PRIVATE_KEY_SIZE);
            free(nts_kem->private_key);
        }
        nts_kem->public_key_size = 0;
        nts_kem->private_key_size = 0;
        nts_kem->length = 0;
        nts_kem->t = 0;
        free(nts_kem);
    }
}

/**
 *  Return the key size in bytes of NTS-KEM
 
 *  @return NTS-KEM key size in bytes
 **/
int nts_kem_key_size()
{
    return kNTSKEMKeysize;
}

/**
 *  Return the ciphertext size in bytes of NTS-KEM
 
 *  @param[in] nts_kem  The pointer to an NTS-KEM object
 *  @return NTS-KEM ciphertext size in bytes
 **/
int nts_kem_ciphertext_size(const NTSKEM *nts_kem)
{
    return nts_kem_key_size() + NTS_KEM_PARAM_CEIL_R_BYTE;
}

/**
 *  NTS-KEM encapsulation
 *
 *  @param[in]  pk      The pointer to NTS-KEM public key
 *  @param[in]  pk_size The size of the public key in bytes
 *  @param[out] c_ast   The pointer to the NTS-KEM ciphertext
 *  @param[out] k_r     The pointer to the encapsulated key
 *  @return NTS_KEM_SUCCESS on success, otherwise a negative error code
 *          {@see nts_kem_errors.h}
 **/
int nts_kem_encapsulate(const uint8_t *pk,
                        size_t pk_size,
                        uint8_t *c_ast,
                        uint8_t *k_r)
{
    int status = NTS_KEM_BAD_MEMORY_ALLOCATION;
    int32_t i, j, l;
    packed_t v;
    const uint8_t *pk_ptr = pk;
    vector c_c[NTS_KEM_PARAM_R_VEC];
    vector Q[NTS_KEM_PARAM_K][NTS_KEM_PARAM_R_VEC];
    uint8_t kr_in_buf[kNTSKEMKeysize + NTS_KEM_PARAM_CEIL_N_BYTE];
    uint8_t k_e[kNTSKEMKeysize], e[NTS_KEM_PARAM_CEIL_N_BYTE];
#if defined(BENCHMARK)
    uint64_t start_clock, end_clock;
#endif
   
    /* Extract information from public-key */
    
    if (kNTSKEMKeysize > (NTS_KEM_PARAM_K >> 3)) {
        return NTS_KEM_BAD_KEY_LENGTH;
    }
    
#if defined(BENCHMARK)
    start_clock = cpucycles();
#endif
    /**
     * Populate the generator matrix, but only the parity section
     **/
    for (i=0; i<NTS_KEM_PARAM_K; i++) {
        Q[i][NTS_KEM_PARAM_R_VEC-1] = _mm256_setzero_si256();
        memcpy(Q[i], pk_ptr, NTS_KEM_PARAM_CEIL_R_BYTE);
        pk_ptr += NTS_KEM_PARAM_CEIL_R_BYTE;
    }

    /**
     * NTS-KEM Encapsulation procudure
     *
     * Step 1. Generate a uniformly distributed random vector e
     *         of length n and Hamming weight τ
     * Step 2. Partition e into sections, e = ( e_a | e_b | e_c )
     **/
    random_vector(NTS_KEM_PARAM_T, NTS_KEM_PARAM_N, e);

    /**
     * Step 3. Compute SHAKE256(e) to produce k_e
     **/
    shake_256(e, NTS_KEM_PARAM_CEIL_N_BYTE, k_e, kNTSKEMKeysize);

    /**
     * Step 4. Construct a length k message vector m = (e_a | k_e)
     * Step 5. Perform systematic encoding with matrix Q,
     *         i.e. c = ( m | mQ ) + e
     *                = ( c_a | c_b | c_c )
     *                = ( 0_a | c_b | c_c )
     *          c_ast = ( c_b | c_c )
     *
     * The c_c (or parity block) is basically section c of the
     * ciphertext. It's basically c_c = (e_a | k_e)*Q where Q
     * is the last n-k bits of the generator matrix in reduced
     * echelon form G = [ I | Q ].
     *
     * Instead of doing matrix multiplication, we use vectorised
     * XOR operations.
     **/
    for (i=0; i<NTS_KEM_PARAM_R_VEC; i++)
        c_c[i] = _mm256_setzero_si256();
    for (i=0; i<NTS_KEM_PARAM_A >> LOG2; i++) {
        memcpy(&v, &e[i*sizeof(v)], sizeof(v));
        while (v) {
            l = (int32_t)lowest_bit_idx(v);
            v ^= (ONE << l);
            l += (BITSIZE*i);
            for (j=0; j<NTS_KEM_PARAM_R_VEC; j++) {
                c_c[j] ^= Q[l][j];
            }
        }
    }
    v = 0x0ULL;
    memcpy(&v,  &e[i*sizeof(v)], NTS_KEM_PARAM_A_REM);
    while (v) {
        l = (int32_t)lowest_bit_idx(v);
        v ^= (ONE << l);
        l += (BITSIZE*i);
        for (j=0; j<NTS_KEM_PARAM_R_VEC; j++) {
            c_c[j] ^= Q[l][j];
        }
    }
    for (i=0; i<NTS_KEM_PARAM_B >> LOG2; i++) {
        memcpy(&v, &k_e[i*sizeof(v)], sizeof(v));
        while (v) {
            l = (int32_t)lowest_bit_idx(v);
            v ^= (ONE << l);
            l += ((BITSIZE*i) + NTS_KEM_PARAM_A);
            for (j=0; j<NTS_KEM_PARAM_R_VEC; j++) {
                c_c[j] ^= Q[l][j];
            }
        }
    }

    /**
     * The output is ciphertext containing the following section:
     *
     *     c = ( c_a | c_b | c_c )
     *
     * By construction, c_b = k_e and its length is kNTSKEMKeysize bytes,
     * the length of c_a is (k/8 - kNTSKEMKeysize) bytes and the length of
     * c_c is (n-k)/8 bytes.
     *
     * The error pattern e = ( e_a | e_b | e_c ), therefore, after
     * adding e to c, c_a = 0, and we have
     *
     *     c_ast = ( k_e + e_b | c_c + e_c )
     **/
    memcpy(c_ast, k_e, kNTSKEMKeysize);                             /* k_e */
    memcpy(&c_ast[kNTSKEMKeysize], c_c, NTS_KEM_PARAM_CEIL_R_BYTE); /* c_c */
    
    /**
     * Perturb the NTS ciphertext with error pattern in section b and c.
     *
     * There is no need to perturb section a as we know it will result
     * to 0 and we are going to drop this section anyway.
     */
    for (i=0; i<NTS_KEM_CIPHERTEXT_SIZE; i++) {
        c_ast[i] ^= e[(NTS_KEM_PARAM_A>>3) + i];    /* c_b = k_e + e_b, c_c = c_c + e_c */
    }
    
    /**
     * Step 6. Output the pair (k_r, c_ast) where k_r = SHAKE256(k_e | e)
     *
     * Construct (k_e | e) and obtain k_r = SHAKE256(k_e | e)
     **/
    memcpy(kr_in_buf, k_e, kNTSKEMKeysize);
    memcpy(&kr_in_buf[kNTSKEMKeysize], e, NTS_KEM_PARAM_CEIL_N_BYTE);
    shake_256(kr_in_buf, kNTSKEMKeysize + NTS_KEM_PARAM_CEIL_N_BYTE, k_r, kNTSKEMKeysize);
    
#if defined(BENCHMARK)
    end_clock = cpucycles();
    fprintf(stdout, "# Encap : (1-6) encapsulation consumes %" PRId64 " cycles\n", end_clock-start_clock);
#endif

    status = NTS_KEM_SUCCESS;
    memset(e, 0, NTS_KEM_PARAM_CEIL_N_BYTE);
    memset(kr_in_buf, 0, kNTSKEMKeysize + NTS_KEM_PARAM_CEIL_N_BYTE);
    
    return status;
}

/**
 *  NTS-KEM decapsulation
 *
 *  @param[in]  sk      The pointer to NTS-KEM private key
 *  @param[in]  sk_size The size of the private key in bytes
 *  @param[in]  c_ast   The pointer to the NTS-KEM ciphertext
 *  @param[out] k_r     The pointer to the encapsulated key
 *  @return NTS_KEM_SUCCESS on success, otherwise a negative error code
 *          {@see nts_kem_errors.h}
 **/
int nts_kem_decapsulate(const uint8_t *sk,
                        size_t sk_size,
                        const uint8_t *c_ast,
                        uint8_t *k_r)
{
    int32_t i, status = NTS_KEM_BAD_MEMORY_ALLOCATION;
    int32_t extended_error = 0;
    uint32_t checksum = 0, error_weight = 0;
    NTSKEM *nts_kem = NULL;
    NTSKEM_private *priv = NULL;
    uint8_t cwork[NTS_KEM_CIPHERTEXT_SIZE];
    vector in_cipher[NTS_KEM_PARAM_BC_VEC];
    __m128i vec_syndromes[1][NTS_KEM_PARAM_M] = {{{0}}};
    uint64_t sigma[2][NTS_KEM_PARAM_M];
    vector evals[NTS_KEM_PARAM_N_VEC][NTS_KEM_PARAM_M];
    vector error[NTS_KEM_PARAM_N_VEC];
    vector allones = _mm256_set1_epi64x(-1);
    uint8_t *e_prime = NULL;
    ff_unit syndromes[2*NTS_KEM_PARAM_T];
    uint8_t e[NTS_KEM_PARAM_CEIL_N_BYTE];
    uint8_t kr_in_buf[kNTSKEMKeysize + NTS_KEM_PARAM_CEIL_N_BYTE];
#if defined(BENCHMARK)
    uint64_t start_clock, end_clock;
#endif
    
    if (!k_r || !c_ast) {
        status = NTS_KEM_BAD_PARAMETERS;
        goto decapsulation_failure;
    }
    memcpy(cwork,c_ast,sizeof cwork);
    
    /**
     * Construct an NTS object from private key
     **/
    if (NTS_KEM_SUCCESS != nts_kem_init_from_private_key(&nts_kem, sk, sk_size))
        goto decapsulation_failure;
    
    priv = nts_kem->priv;

    /**
     * Load the input ciphertext c* to a vectorised array
     **/
    load_input_ciphertext(in_cipher, cwork);

    /**
     * Step 1a. Rewrite the vector a = (a_b | a_c) and h = (h_b | h_c)
     * Step 1b. Construct a truncated parity-check matrix H*_m from a and h
     * Step 1c. Compute all 2*τ syndromes of c* as s = (c_b | c_c).(H*_m)^T,
     *         see Algorithm 2 in the supporting document
     */
#if defined(BENCHMARK)
    start_clock = cpucycles();
#endif
    status = compute_syndrome(nts_kem, in_cipher, syndromes);
    if (status != NTS_KEM_SUCCESS)
        goto decapsulation_failure;
#if defined(BENCHMARK)
    end_clock = cpucycles();
    fprintf(stdout, "# Decap : (1c) compute_syndrome consumes %" PRId64 " cycles\n", end_clock-start_clock);
#endif
    status = NTS_KEM_BAD_MEMORY_ALLOCATION; /* Reset the status value */
   
    /**
     * Step 1d. Compute the error-locator polynomial σ(x)
     *
     * Given the syndromes computed earlier, use Berlekamp-Massey algorithm
     * to obtain the error-locator polynomial σ(x)
     **/
    vector_load_2d_128(vec_syndromes, syndromes, 2*NTS_KEM_PARAM_T);
#if defined(BENCHMARK)
    start_clock = cpucycles();
#endif
    bitslice_bma(sigma, vec_syndromes, &extended_error);
#if defined(BENCHMARK)
    end_clock = cpucycles();
    fprintf(stdout, "# Decap : (1d) berlekamp_massey consumes %" PRId64 " cycles\n", end_clock-start_clock);
#endif
    
    /**
     * Step 1e. Compute the roots of the error-locator polynomial σ(x)
     *
     * The roots of the error-locating polynomial σ(x) tells us where
     * the locations of the error are. They can be efficiently computed
     * using additive FFT over Finite Field
     **/
#if defined(BENCHMARK)
    start_clock = cpucycles();
#endif
    /**
     * Convert the coefficients of the locator polynomial in bit-slice format
     **/
    bitslice_fft(evals, sigma, -(1ULL-extended_error));
#if defined(BENCHMARK)
    end_clock = cpucycles();
    fprintf(stdout, "# Decap : (1e) roots_finding consumes %" PRId64 " cycles\n", end_clock-start_clock);
#endif

    /**
     * Step 1f. Given Λ, obtain the error vector e_prime
     *
     * We know the locations of the error, it's time to correct them,
     * in other words, we want to obtain e_prime.
     **/
#if defined(BENCHMARK)
    start_clock = cpucycles();
#endif
    for (i=0; i<NTS_KEM_PARAM_N_VEC; i++) {
        error[i] = vector_ff_or(evals[i]);
        error[i] = _mm256_xor_si256(error[i], allones);
        error_weight += vector_popcount(error[i]);
    }
    e_prime = (uint8_t *)error;
    /* Correct the error in the zero-th coordinate if necessary */
    e_prime[0] |= ((uint8_t)extended_error);
    error_weight += extended_error;
#if defined(BENCHMARK)
    end_clock = cpucycles();
    fprintf(stdout, "# Decap : (1f) get_e_prime consumes %" PRId64 " cycles\n", end_clock-start_clock);
#endif

    /**
     * Step 7. Permute e_prime with permutation p to obtain e
     * Step 8. Consider e = (e_a | e_b | e_c), recover k_e = c_b - e_b
     *
     * A countermeasure is added to prevent potential cache timing attack
     **/
#if defined(BENCHMARK)
    start_clock = cpucycles();
#endif
    correct_error_and_recover_ke(e_prime, priv->p, e, cwork);
#if defined(BENCHMARK)
    end_clock = cpucycles();
    fprintf(stdout, "# Decap : (2-3) permute_e_and_recover_ke consumes %" PRId64 " cycles\n", end_clock-start_clock);
#endif
    
    /**
     * Step 9. Check if k_e == SHAKE256(e), if not return an error indicating
     *         an invalid ciphertext, otherwise return k_r = SHAKE256(k_e | e)
     *
     * Obtain k_e from the error pattern, k_e = SHAKE256(e)
     **/
#if defined(BENCHMARK)
    start_clock = cpucycles();
#endif
    shake_256((const uint8_t *)e, NTS_KEM_PARAM_CEIL_N_BYTE, kr_in_buf, kNTSKEMKeysize);
    /**
     * Construct (k_e | e)
     **/
    memcpy(&kr_in_buf[kNTSKEMKeysize], e, NTS_KEM_PARAM_CEIL_N_BYTE);
    /**
     * Obtain k_r, i.e. k_r = SHAKE256(k_e | e)
     **/
    shake_256(kr_in_buf, kNTSKEMKeysize + NTS_KEM_PARAM_CEIL_N_BYTE, k_r, kNTSKEMKeysize);
    /**
     * Verify the equality of k_e and SHAKE256(e)
     **/
    for (checksum=0,i=0; i<kNTSKEMKeysize; i++) {
        checksum += (cwork[i] ^ kr_in_buf[i]);
    }
    status = CT_mux(CT_is_equal_zero(checksum) && CT_is_equal(error_weight, NTS_KEM_PARAM_T),
                    NTS_KEM_SUCCESS, NTS_KEM_INVALID_CIPHERTEXT);
#if defined(BENCHMARK)
    end_clock = cpucycles();
    fprintf(stdout, "# Decap : (4) kr_and_status consumes %" PRId64 " cycles\n", end_clock-start_clock);
#endif
decapsulation_failure:
    memset(kr_in_buf, 0, kNTSKEMKeysize + NTS_KEM_PARAM_CEIL_N_BYTE);
    memset(e, 0, NTS_KEM_PARAM_CEIL_N_BYTE);
    memset(e_prime, 0, NTS_KEM_PARAM_CEIL_N_BYTE);
    memset(syndromes, 0, sizeof(syndromes));
    memset(evals, 0, sizeof(evals));
    if (nts_kem)
        nts_kem_release(nts_kem);
    
    return status;
}

/** -------------------- Private helper methods -------------------- **/

/**
 *  Check whether or not a Goppa polynomial is valid
 *
 *  @note
 *  Given a Goppa polynomial G(z) over GF(2^m), it's considered
 *  a valid polynomial if it has no roots in the field GF(2^m)
 *  and that it has no repeated factors.
 *
 *  If GCD(G(z), 1+z^{2^m-1}) = 1, this means that G(z) has no
 *  roots in GF(2^m). However, this can be more efficiently done
 *  by means of additive FFT.
 *
 *  If GCD(G(z), G'(z)) = 1, where G'(z) is the formal derivate
 *  of G(z) wrt z, this means that G(z) has no repeated factors.
 *
 *  If the above two conditions are met, G(z) is a valid Goppa
 *  polynomial
 *
 *  @param[in] ff2m   The finite field F_{2^m}
 *  @param[in] Gz     The Goppa polynomial G(z)
 *  @return 1 if G(z) is a valid Goppa polynomial, 0 otherwise
 **/
int is_valid_goppa_polynomial(const FF2m *ff2m, const poly *Gz)
{
    int i, status = 1;
    uint64_t g[2][NTS_KEM_PARAM_M] = {{0}};
    vector v[NTS_KEM_PARAM_N_VEC];
    vector evals[NTS_KEM_PARAM_N_VEC][NTS_KEM_PARAM_M];
    poly* Fz = init_poly((1 << ff2m->m));
    poly* Dz = init_poly((1 << ff2m->m));
    if (!Fz || !Dz)
        return 0;

    vector_load_2d_64(g, Gz->coeff, (NTS_KEM_PARAM_T+1));
    bitslice_fft(evals, g, -1);
    for (i=0; i<NTS_KEM_PARAM_N_VEC && status; i++) {
        v[i] = vector_ff_or(evals[i]);
        v[i] = ~v[i];
        status = ((~(v[i][3] | -v[i][3]) >> 63) &
                  (~(v[i][2] | -v[i][2]) >> 63) &
                  (~(v[i][1] | -v[i][1]) >> 63) &
                  (~(v[i][0] | -v[i][0]) >> 63));   /* status is 1 if v[i] is 0 */
    }
    if (status) {
        /* Does it have repeated roots? */
        /* F(z) = GCD(G(z), d/dz G(z))  */
        if (formal_derivative_poly(Gz, Dz)) {
            if (gcd_poly(ff2m, Gz, Dz, Fz)) {
                status = (Fz->degree < 1);
            }
        }
    }
    
    zero_poly(Dz); free_poly(Dz);
    zero_poly(Fz); free_poly(Fz);
    memset(g, 0, sizeof(g));
    memset(v, 0, sizeof(v));
    memset(evals, 0, sizeof(evals));
    
    return status;
}

/**
 *  Create a random Goppa polynomial
 *
 *  @note
 *  This function implements Step 1 of NTS-KEM
 *  Key Generation procedure as described in the
 *  submitted NIST document.
 *
 *  @note
 *  Use the method {@see is_valid_goppa_polynomial} to check 
 *  whether or not a Goppa polynomial is a valid one
 *
 *  @param[in] ff2m   The finite field F_{2^m}
 *  @param[in] degree The degree of the Goppa polynomial
 *  @return a valid Goppa polynomial on success, NULL otherwise
 **/
poly* create_random_goppa_polynomial(const FF2m *ff2m, int degree)
{
#if defined(BENCHMARK)
    int32_t count = 0;
#endif
    uint8_t buffer[NTS_KEM_PARAM_CEIL_R_BYTE];
    poly *Gz = init_poly(1 << ff2m->m);
    if (!Gz)
        return NULL;
    
    Gz->degree = degree;
    do {
        /**
         * (a) Sample uniformly at random mτ bits (or (n-k)/8 bytes) of random data
         *     and sequentially assign m bits for g_i in g = (g_0,g_1,...,g_{τ-1})
         **/
        randombytes(buffer, NTS_KEM_PARAM_CEIL_R_BYTE);
        unpack_buffer(buffer, Gz->coeff, NTS_KEM_PARAM_T);
        
        /**
         * (b) Set g_τ = 1 and let G(z) = \sum_{i=0}^τ g_iz^i
         **/
        Gz->coeff[ Gz->degree ] = 1;
        
        /**
         * (c) Restart to step (a) if the first coefficient is 0 or G(z) has roots
         *     in F_{2^m} or G(z) has repeated roots in any extension field.
         **/
#if defined(BENCHMARK)
        ++count;
#endif
    } while (!Gz->coeff[0] || !is_valid_goppa_polynomial(ff2m, Gz));
    memset(buffer, 0, sizeof(buffer));
#if defined(BENCHMARK)
    fprintf(stdout, "# num_Gz_trials %d\n", count);
#endif

    return Gz;
}

/**
 *  Given a Goppa polynomial, construct the reduced row echelon
 *  generator matrix G = [I | Q] of the Goppa code
 *
 *  @note
 *  This function implements Step 3 of NTS-KEM Key Generation
 *  procedure as described in the submitted NIST document.
 *
 *  @param[in]  nts_kem  The pointer to an NTS-KEM object
 *  @param[in]  Gz       The Goppa polynomial G(z)
 *  @param[out] a        The vector containing all elements of
 *                       F_2^m, permuted by vector p
 *  @param[out] h        The evaluation of G(z) based on the
 *                       elements in vector a
 *  @return A pointer to matrix Q over F_2
 **/
matrix_ff2* create_matrix_G(const NTSKEM* nts_kem,
                            const poly* Gz,
                            ff_unit *a,
                            ff_unit *h)
{
    NTSKEM_private* priv = (NTSKEM_private *)nts_kem->priv;
    int32_t i, j, l, rank;
    matrix_ff2 *H = NULL, *Q = NULL;
    packed_t *v_ptr = NULL;
    ff_unit f;
    vector av0[NTS_KEM_PARAM_N_VEC][NTS_KEM_PARAM_M];
    vector hv0[NTS_KEM_PARAM_N_VEC][NTS_KEM_PARAM_M];
    vector hv1[NTS_KEM_PARAM_N_VEC][NTS_KEM_PARAM_M];
    uint16_t a_prime[NTS_KEM_PARAM_N];
    uint64_t g[2][NTS_KEM_PARAM_M] = {{0}};
	
    vector vh[NTS_KEM_PARAM_N_VEC][NTS_KEM_PARAM_M];
    uint64_t v[NTS_KEM_PARAM_N_DIV_64];

    /**
     * Let a = π_p(a′) = (a_{p_0},a_{p_1},...,a_{p_{n−1}}) ∈ F^n_{2^m}
     * be the sequence obtained by ordering the elements of
     * a′ = (B[0], B[1], ..., B[n-1]) according to π_p.
     **/
    a_prime[0] = 0x00;
    for (i=1; i<nts_kem->length; i++) {
        a_prime[i] = 0;
        for (j=0; j<priv->ff2m->m; j++) {
            a_prime[i] ^= (((i & (1 << j)) >> j) * priv->ff2m->basis[j]);
        }
    }
    
    /**
     * Perform additive FFT on Gz to obtain an evaluation of G(z)
     * at points B[0], B[1], B[2], ..., B[n-1]
     **/
    vector_load_2d_64(g, Gz->coeff, (NTS_KEM_PARAM_T+1));
    bitslice_fft(vh, g, -1);
    
    /**
     * Permute the bit-slice output of the FFT with permutation p
     **/
    for (j=0; j<NTS_KEM_PARAM_M; j++) {
        memset(v, 0, sizeof(v));
        for (i=0; i<NTS_KEM_PARAM_N; i++) {
            l = priv->p[i];
			switch ((l & 255) >> 6) {
				case 3:
					v[i >> 6] |= (((_mm256_extract_epi64(vh[l >> 8][j], 3) & (1ULL << (l & 63))) >> (l & 63)) << (i & 63));
					break;
				case 2:
					v[i >> 6] |= (((_mm256_extract_epi64(vh[l >> 8][j], 2) & (1ULL << (l & 63))) >> (l & 63)) << (i & 63));
					break;
				case 1:
					v[i >> 6] |= (((_mm256_extract_epi64(vh[l >> 8][j], 1) & (1ULL << (l & 63))) >> (l & 63)) << (i & 63));
					break;
				default:
					v[i >> 6] |= (((_mm256_extract_epi64(vh[l >> 8][j], 0) & (1ULL << (l & 63))) >> (l & 63)) << (i & 63));
			}
        }
        for (i=0; i<NTS_KEM_PARAM_N_VEC; i++) {
            hv0[i][j] = _mm256_set_epi64x(v[4*i + 3], v[4*i + 2], v[4*i + 1], v[4*i]);
        }
    }

    /**
     * Permute the sequences a' with permutation p
     **/
    for (i=0; i<NTS_KEM_PARAM_N; i++) {
        a[i] = a_prime[priv->p[i]];
    }
    
    /**
     * Calculate G(a_i)^{-2} using vectorised implementation
     **/
    for (i=0,j=0; i<NTS_KEM_PARAM_N_VEC; i++) {
        priv->ff2m->vector_ff_sqr_inv(priv->ff2m, hv1[i], hv0[i]);
    }
    vector_store_2d_256(h, (const __m256i (*)[])hv1, NTS_KEM_PARAM_N);
    
    /* Prepare vector a for vectorised multiplication */
    vector_load_2d_256(av0, a, NTS_KEM_PARAM_N);
    
    /* Obtain the parity-check matrix H from h */
    if (!(H = calloc_matrix_ff2(Gz->degree * priv->ff2m->m, (1 << priv->m))))
        return NULL;
    /* The first batch of NTS_KEM_PARAM_M rows of parity-check matrix */
    for (i=0; i<NTS_KEM_PARAM_N_VEC; i++) {
        for (j=0; j<NTS_KEM_PARAM_M; j++) {
            memcpy(row_ptr_matrix_ff2(H, j) + (i*BLOCK_SIZE >> 3),
                   &hv1[i][j],
                   UINT64_SIZE*sizeof(uint64_t));
        }
    }
    /* h_1 = h_0 * a */
    for (i=0; i<NTS_KEM_PARAM_N_VEC; i++) {
        priv->ff2m->vector_ff_mul(priv->ff2m, hv0[i], hv1[i], av0[i]);
    }
    /* The second batch of NTS_KEM_PARAM_M rows of parity-check matrix */
    for (i=0; i<NTS_KEM_PARAM_N_VEC; i++) {
        for (j=0; j<NTS_KEM_PARAM_M; j++) {
            memcpy(row_ptr_matrix_ff2(H, j+NTS_KEM_PARAM_M) + (i*BLOCK_SIZE >> 3),
                   &hv0[i][j],
                   UINT64_SIZE*sizeof(uint64_t));
        }
    }
    for (l=2; l<Gz->degree; l+=2) {
        /* h_l = h_{l-1} * a */
        for (i=0; i<NTS_KEM_PARAM_N_VEC; i++) {
            priv->ff2m->vector_ff_mul(priv->ff2m, hv1[i], hv0[i], av0[i]);
        }
        /* The l-th batch of NTS_KEM_PARAM_M rows of parity-check matrix */
        for (i=0; i<NTS_KEM_PARAM_N_VEC; i++) {
            for (j=0; j<NTS_KEM_PARAM_M; j++) {
                memcpy(row_ptr_matrix_ff2(H, j+(l*NTS_KEM_PARAM_M)) + (i*BLOCK_SIZE >> 3),
                       &hv1[i][j],
                       UINT64_SIZE*sizeof(uint64_t));
            }
        }
        /* h_l = h_{l-1} * a */
        for (i=0; i<NTS_KEM_PARAM_N_VEC; i++) {
            priv->ff2m->vector_ff_mul(priv->ff2m, hv0[i], hv1[i], av0[i]);
        }
        /* The (l+1)-th batch of NTS_KEM_PARAM_M rows of parity-check matrix */
        for (i=0; i<NTS_KEM_PARAM_N_VEC; i++) {
            for (j=0; j<NTS_KEM_PARAM_M; j++) {
                memcpy(row_ptr_matrix_ff2(H, j+((l+1)*NTS_KEM_PARAM_M)) + (i*BLOCK_SIZE >> 3),
                       &hv0[i][j],
                       UINT64_SIZE*sizeof(uint64_t));
            }
        }
    }
    
    /**
     * Perform M4RI for reduced row echelon transformation
     **/
    rank = reduce_row_echelon_matrix_ff2(H);
    if (NTS_KEM_PARAM_K != nts_kem->length - rank) {
        fprintf(stderr, "FATAL ERROR: The Goppa code is invalid, ");
        fprintf(stderr, "this indicates that there is bugs in the code\n\n");
        return NULL;
    }

    /**
     * Perform permutation on the columns of H such that the last (n-k)
     * columns of H form a (n-k)x(n-k) identity matrix. Let ρ be the
     * permutation that makes H in the form that we need. Update the
     * permutation matrix generated in Step 2 with ρ. The vectors
     * a and h have to be permuted too.
     **/
    for (j=0,i=H->nrows-1; i>=0; i--) {
        v_ptr = (packed_t *)row_ptr_matrix_ff2(H, i);
        while (!is_bit_set(v_ptr, H->ncols-j-1)) ++j;
        
        column_swap_matrix_ff2(H, NTS_KEM_PARAM_K + i, H->ncols-j-1);

        /* Update the order of permutation p */
        f = priv->p[ NTS_KEM_PARAM_K + i ];
        priv->p[ NTS_KEM_PARAM_K + i ] = priv->p[ H->ncols-j-1 ];
        priv->p[ H->ncols-j-1 ] = f;
        
        /* Update the order of vector a */
        f = a[ NTS_KEM_PARAM_K + i ];
        a[ NTS_KEM_PARAM_K + i ] = a[ H->ncols-j-1 ];
        a[ H->ncols-j-1 ] = f;
        
        /* Update the order of vector a */
        f = h[ NTS_KEM_PARAM_K + i ];
        h[ NTS_KEM_PARAM_K + i ] = h[ H->ncols-j-1 ];
        h[ H->ncols-j-1 ] = f;
    }
    
    if (!(Q = calloc_matrix_ff2(NTS_KEM_PARAM_K, NTS_KEM_PARAM_C)))
        return NULL;
    for (i=0; i<NTS_KEM_PARAM_C; i++) {
        v_ptr = (packed_t *)row_ptr_matrix_ff2(H, i);
        for (j=0; j<NTS_KEM_PARAM_K; j++) {
            bit_set_value((packed_t *)row_ptr_matrix_ff2(Q, j),
                          i,
                          bit_value(v_ptr, j));
        }
    }

    zero_matrix_ff2(H);
    free_matrix_ff2(H);
    
    return Q;
}

/**
 *  Pack buffer of 2*src_len bytes into a buffer of
 *  size src_len*3/2.
 *
 *  @param[in]  src     Pointer to the input buffer
 *  @param[in]  src_len Length of src in 2-byte (word) unit
 *  @param[out] dst     Pointer to the output buffer
 **/
void pack_buffer(const uint8_t *src, int src_len, uint8_t *dst)
{
    int32_t i;
    const uint8_t* src_ptr = src;
    uint8_t *dst_ptr = dst;
    /* Every two units consume three bytes */
    for (i=0; i<src_len; i+=2) {
        *dst_ptr  = *src_ptr++; dst_ptr++;
        *dst_ptr  = *src_ptr++;
        *dst_ptr |= ((*src_ptr   & 0x0F) << 4); dst_ptr++;
        *dst_ptr  = ((*src_ptr++ & 0xF0) >> 4);
        *dst_ptr |= ((*src_ptr++ & 0x0F) << 4); dst_ptr++;
    }
}

/**
 *  Unpack a buffer to a word buffer of length dst_len
 *
 *  @param[in]  src     Pointer to the input buffer
 *  @param[out] dst     Pointer to the output buffer (in words)
 *  @param[in]  dst_len Length of dst in 2-byte (word) unit
 **/
void unpack_buffer(const uint8_t *src, ff_unit *dst, int dst_len)
{
    int32_t i, value;
    const uint8_t *src_ptr = src;
    /* Read three bytes in ago */
    for (i=0; i<dst_len; i+=2,src_ptr+=3) {
        value = (*(src_ptr+2) << 16) | (*(src_ptr+1) << 8) | *src_ptr;
        dst[i]   = (value & 0x00000FFF);
        dst[i+1] = (value & 0x00FFF000) >> 12;
    }
}

/**
 *  Serialise NTS public key
 *
 *  @param[in] nts_kem    The pointer to NTS-KEM object
 *  @param[in] Q          The pointer to matrix Q
 *  @return NTS_KEM_SUCCESS on success, a negative integer otherwise
 **/
int serialise_public_key(NTSKEM* nts_kem,
                         const matrix_ff2* Q)
{
    NTSKEM_private* priv = (NTSKEM_private *)nts_kem->priv;
    int i, j, s;
    const int32_t b = NTS_KEM_PARAM_CEIL_R_BYTE;
    uint8_t *key_ptr = NULL;
    packed_t bit_value, *v_ptr = NULL;
    
    nts_kem->public_key_size = NTS_KEM_PUBLIC_KEY_SIZE;
    nts_kem->public_key = (uint8_t *)calloc(nts_kem->public_key_size, sizeof(uint8_t));
    if (!nts_kem->public_key)
        return NTS_KEM_BAD_MEMORY_ALLOCATION;
    priv->m = NTS_KEM_PARAM_M;
    key_ptr = nts_kem->public_key;
    for (i=0; i<NTS_KEM_PARAM_K; i++) {
        v_ptr = (packed_t *)row_ptr_matrix_ff2(Q, i);
        for (j=0; j<NTS_KEM_PARAM_C; j++) {
            bit_value = bit_value(v_ptr, j);
            s = j >> 3;
            key_ptr[(i * b) + s] |= ((uint8_t)(bit_value << (j - (s << 3))));
        }
    }
    
    return NTS_KEM_SUCCESS;
}

/**
 *  Serialise NTS private key
 *
 *  @param[in] nts_kem    The pointer to NTS-KEM object
 *  @return NTS_KEM_SUCCESS on success, a negative integer otherwise
 **/
int serialise_private_key(NTSKEM *nts_kem)
{
    uint8_t *key_ptr = NULL;
    NTSKEM_private* priv = (NTSKEM_private *)nts_kem->priv;
    
    nts_kem->private_key_size = NTS_KEM_PRIVATE_KEY_SIZE;
    nts_kem->private_key = (uint8_t *)calloc(nts_kem->private_key_size, sizeof(uint8_t));
    if (!nts_kem->private_key)
        return NTS_KEM_BAD_MEMORY_ALLOCATION;
    key_ptr = nts_kem->private_key;
    
    pack_buffer((const uint8_t *)priv->a, NTS_KEM_PARAM_BC, key_ptr);
    key_ptr += (NTS_KEM_PARAM_BC * 3/2);
    
    pack_buffer((const uint8_t *)priv->h, NTS_KEM_PARAM_BC, key_ptr);
    key_ptr += (NTS_KEM_PARAM_BC * 3/2);
    
    pack_buffer((const uint8_t *)priv->p, NTS_KEM_PARAM_N, key_ptr);
    
    return NTS_KEM_SUCCESS;
}

/**
 *  Deserialise NTS-KEM private key
 *
 *  @param[out] nts_kem    The pointer to NTS-KEM object
 *  @param[in]  buf        Pointer to buffer containing private key blob
 *  @return NTS_KEM_SUCCESS on success, a negative integer otherwise
 **/
int deserialise_private_key(NTSKEM* nts_kem, const uint8_t *buf)
{
    NTSKEM_private* priv = (NTSKEM_private *)nts_kem->priv;
    
    /* Deserialise vector a */
    unpack_buffer(buf, priv->a, NTS_KEM_PARAM_BC);
    buf += (NTS_KEM_PARAM_BC * 3/2);
    
    /* Deserialise vector h */
    unpack_buffer(buf, priv->h, NTS_KEM_PARAM_BC);
    buf += (NTS_KEM_PARAM_BC * 3/2);
    
    unpack_buffer(buf, priv->p, NTS_KEM_PARAM_N);
    
    return NTS_KEM_SUCCESS;
}

/**
 *  Fisher-Yates-Knuth-Yao shuffle on a sequence (in-place)
 *
 *  @note
 *  Fisher-Yates shuffle requires the generation of uniform
 *  random number between a certain range. Knuth-Yao method
 *  may be used to generate such numbers uniformly.
 *
 *  @param[in,out] buffer      The input/output sequence
 **/
void fisher_yates_shuffle(ff_unit *buffer)
{
    ff_unit index, swap;
    int i = NTS_KEM_PARAM_N - 1;
    while (i > 0) {
        index = random_uint16_bounded(i+1);
        swap = buffer[index];
        buffer[index] = buffer[i];
        buffer[i] = swap;
        --i;
    }
}

/**
 *  Compute the syndrome vectors
 *
 *  @note
 *  This function implements Step 3 of NTS-KEM Decapsulation 
 *  procedure as described in the submitted NIST document.
 *
 *  @note
 *  Given the data and parity-check vectors, both of which have 
 *  been corrupted with errors, compute the syndrome vector.
 *
 *  @param[in]  nts_kem   The pointer to NTS-KEM object
 *  @param[in]  c_ptr     The pointer to the inpute ciphertext
 *  @param[out] s         The computed 2*t syndromes
 *  @return NTS_KEM_SUCCESS on success, otherwise a negative status
 *  {@see nts_kem_errors.h}
 **/
int compute_syndrome(const NTSKEM* nts_kem,
                     const vector *c_ptr,
                     ff_unit* s)
{
    int32_t i, j;
    FF2m *ff2m = NULL;
    NTSKEM_private *priv = NULL;
    vector a[NTS_KEM_PARAM_BC_VEC][NTS_KEM_PARAM_M];
    vector g[NTS_KEM_PARAM_BC_VEC][NTS_KEM_PARAM_M];
    vector h[NTS_KEM_PARAM_BC_VEC][NTS_KEM_PARAM_M];
    
    if (!nts_kem || !nts_kem->priv)
        return NTS_KEM_BAD_PARAMETERS;
    
    priv = (NTSKEM_private *)nts_kem->priv;
    ff2m = priv->ff2m;
    
    vector_load_2d_256(a, priv->a, NTS_KEM_PARAM_BC);
    vector_load_2d_256(g, priv->h, NTS_KEM_PARAM_BC);
    
    memset(s, 0, 2*NTS_KEM_PARAM_T*sizeof(ff_unit));
    memset(h, 0, NTS_KEM_PARAM_BC_VEC*NTS_KEM_PARAM_M*sizeof(vector));
    for (i=0; i<NTS_KEM_PARAM_BC_VEC; i++) {
        for (j=0; j<NTS_KEM_PARAM_M; j++)
            g[i][j] &= *c_ptr;
        c_ptr++;
        s[0] ^= ff2m->vector_ff_transpose_xor(ff2m, g[i]);
        for (j=1; j<=2*NTS_KEM_PARAM_T-2; j+=2) {
            ff2m->vector_ff_mul(ff2m, h[i], a[i], g[i]);
            s[j]   ^= ff2m->vector_ff_transpose_xor(ff2m, h[i]);
            ff2m->vector_ff_mul(ff2m, g[i], a[i], h[i]);
            s[j+1] ^= ff2m->vector_ff_transpose_xor(ff2m, g[i]);
        }
        ff2m->vector_ff_mul(ff2m, h[i], a[i], g[i]);
        s[j] ^= ff2m->vector_ff_transpose_xor(ff2m, h[i]);
    }
    
    memset(a, 0, sizeof(a));
    memset(g, 0, sizeof(g));
    memset(h, 0, sizeof(h));

    return NTS_KEM_SUCCESS;
}

/**
 *  Permute the error and recover k_e
 *
 *  @note
 *  This method performs Steps 9 and 10 of NTS-KEM decapsulation
 *
 *  @param[in]      e_prime Error vector in inverse permuted order
 *  @param[in]      p       Permutation vector p
 *  @param[out]     e       Error vector
 *  @param[in,out]  k_e     Recovered vector k_e
 **/
void correct_error_and_recover_ke(const uint8_t* e_prime,
                                  const ff_unit* p,
                                  uint8_t *e,
                                  uint8_t *k_e)
{
    int32_t i;
    ff_unit a;
    packed_t bit_value, *e_prime_ptr, *e_ptr;
    
    e_prime_ptr = (packed_t *)e_prime;
    e_ptr = (packed_t *)e;
    memset(e, 0, NTS_KEM_PARAM_CEIL_N_BYTE);
    for (i=0; i<NTS_KEM_PARAM_A; i++) {
        a = p[i];
        bit_value = bit_value(e_prime_ptr, a);
        bit_set_value(e_ptr, i, bit_value); /* Permute e_prime */
    }
    for (; i<NTS_KEM_PARAM_K; i++) {
        a = p[i];
        bit_value = bit_value(e_prime_ptr, a);
        bit_set_value(e_ptr, i, bit_value); /* Permute e_prime */
        bit_toggle_value((packed_t *)k_e, i-NTS_KEM_PARAM_A, bit_value); /* Step 10: recovering k_e */
    }
    for (; i<NTS_KEM_PARAM_N; i++) {
        a = p[i];
        bit_value = bit_value(e_prime_ptr, a);
        bit_set_value(e_ptr, i, bit_value); /* Permute e_prime */
    }
}

/**
 *  Create a random vector `e` of length `n` bits with
 *  Hamming weight `tau`
 *
 *  @param[in]  tau  The desired Hamming weight
 *  @param[in]  n    The length of the sequence in bits
 *  @param[out] e    The output vector
 **/
void random_vector(uint32_t tau, uint32_t n, uint8_t *e)
{
    int32_t i;
    uint8_t a, b;
    ff_unit index;
    
    /**
     * Create a vector with `tau` non-zeros in
     * the last `tau` coordinates
     **/
    memset(e, 0, (n + 7) >> 3);
    for (i=0; i<NTS_KEM_PARAM_T >> 3; i++)
        e[(NTS_KEM_PARAM_N>>3)-i-1] = 0xFF;
    
    /**
     * Shuffle the sequence e to randomise the location
     * of errors
     **/
    i = NTS_KEM_PARAM_N-1;
    while (i >= NTS_KEM_PARAM_N-NTS_KEM_PARAM_T) {
        index = random_uint16_bounded(i+1);
        a = (e[index >> 3] & (1 << (index & 7))) >> (index & 7);
        b = (e[i >> 3] & (1 << (i & 7))) >> (i & 7);
        e[index >> 3] &= ~(1 << (index & 7));
        e[index >> 3] |=  (b << (index & 7));
        e[i     >> 3] &= ~(1 << (i     & 7));
        e[i     >> 3] |=  (a << (i     & 7));
        --i;
    }
}

/**
 *  Load an input byte array of the ciphertext onto a
 *  vectorised array.
 *
 *  @note
 *  It performs unaligned SIMD load
 *
 *  @param[out] out  The vectorised array
 *  @param[in]  in   The pointer to input ciphertext
 **/
void load_input_ciphertext(vector *out, const uint8_t *in)
{
    int32_t i;
    const uint8_t *ptr = in;

    for (i=0; i<NTS_KEM_PARAM_BC_VEC-1; i++) {
        out[i] = _mm256_loadu_si256((const __m256i *)ptr);
        ptr += (UINT64_SIZE*sizeof(uint64_t));
    }
    out[NTS_KEM_PARAM_BC_VEC-1] = _mm256_setzero_si256();
    out[NTS_KEM_PARAM_BC_VEC-1] = _mm256_loadu_si256((const __m256i *)ptr);
}
