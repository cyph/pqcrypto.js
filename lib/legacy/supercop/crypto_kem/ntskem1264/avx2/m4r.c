/**
 *  m4r.c
 *  NTS-KEM
 *
 *  Parameter: NTS-KEM(12, 64)
 *  Platform: AVX2
 *
 *  Methods of Four Russians
 *
 *  References:
 *  [1]  Martin Albrecht, Gregory Bard and William Hart, (2010),
 *       "Algorithm 898 : efficient multiplication of dense matrices over GF(2)",
 *       ACM Transactions on Mathematical Software, Volume 37 (Number 1),
 *       Article: 9, ISSN 0098-3500
 *  [2]  Martin Albrecht, Gregory Bard and Clement Pernet, (2011),
 *       "Efficient Dense Gaussian Elimination over the Finite Field with Two Elements",
 *       Research Report, https://hal.inria.fr/hal-00796873
 *
 *  This file is part of the additional implemention of NTS-KEM
 *  submitted as part of NIST Post-Quantum Cryptography
 *  Standardization Process.
 **/

#include <stdlib.h>
#include <string.h>
#include "bits.h"
#include "random.h"
#include "m4r.h"

#define STRIPE_SIZE         8
#define STRIPE_SIZE_LOG     3
#define NUM_GRAY_TABLES     8
#define NUM_GRAY_TABLES_LOG 3
#define M4R_ROW(M, r)       ((uint8_t *)(M)->v + ((r) * (M)->stride))
#define GRAY_TO_BIN(x)      ((x) ^ ((x) >> 1))

const uint8_t _gray_codes_lut2[] = {
    0, 1, 0, 1
};
const uint8_t _gray_codes_lut3[] = {
    0, 1, 0, 2, 0, 1, 0, 2
};
const uint8_t _gray_codes_lut4[] = {
    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 3
};
const uint8_t _gray_codes_lut5[] = {
    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4
};
const uint8_t _gray_codes_lut6[] = {
    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5,
    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5
};
const uint8_t _gray_codes_lut7[] = {
    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5,
    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6,
    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5,
    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6
};
const uint8_t _gray_codes_lut8[] = {
    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5,
    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6,
    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5,
    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 7,
    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5,
    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6,
    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5,
    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 7
};
const uint8_t* _gray_codes_lut[] = {
    NULL, NULL, _gray_codes_lut2, _gray_codes_lut3,
    _gray_codes_lut4, _gray_codes_lut5, _gray_codes_lut6,
    _gray_codes_lut7, _gray_codes_lut8
};

static inline void zero_vector(uint8_t *row_ptr, uint32_t stride)
{
#if defined(__SSE2__) || defined(__AVX2__)
    int32_t i;
    vector *vec_ptr = NULL;
#endif

#if   defined(__AVX2__)
    vec_ptr = (vector *)row_ptr;
    for (i=0; i<stride; i+=64) {
        *vec_ptr = _mm256_setzero_si256(); vec_ptr++;
        *vec_ptr = _mm256_setzero_si256(); vec_ptr++;
    }
#elif defined(__SSE2__)
    vec_ptr = (vector *)row_ptr;
    for (i=0; i<stride; i+=64) {
        *vec_ptr = _mm_setzero_si128(); vec_ptr++;
        *vec_ptr = _mm_setzero_si128(); vec_ptr++;
        *vec_ptr = _mm_setzero_si128(); vec_ptr++;
        *vec_ptr = _mm_setzero_si128(); vec_ptr++;
    }
#else
    memset(row_ptr, 0, stride);
#endif
}

void _m4ri_make_table_rev(matrix_ff2* T,
                          const matrix_ff2* B,
                          uint32_t r_end,
                          uint32_t c_end,
                          uint32_t k)
{
    int32_t i, j, is_zero, nblocks;
    const uint8_t *gray_codes_lut;
#if defined(__SSE2__) || defined(__AVX2__)
    vector *B_ptr = NULL;
    vector *S_ptr = NULL;
    vector *T_ptr = NULL;
    vector *U_ptr = NULL;
    vector mask;
    packed_t u;
#else
    packed_t *B_ptr = NULL;
    packed_t *S_ptr = NULL;
    packed_t *T_ptr = NULL;
    packed_t *U_ptr = NULL;
    packed_t mask = 0;
#endif
#if   defined(__AVX2__)
    packed_t v[4] = { 0ULL, 0ULL, 0ULL, 0ULL };
#elif defined(__SSE2__)
    packed_t v[2] = { 0ULL, 0ULL };
#endif
    
#if   defined(__AVX2__)
    nblocks = (int32_t)((c_end + 255) >> 8);
    
    /**
     * This is equivalent to
     *    mask = (ONE << (c_end & MOD))-1;
     *    mask = (c_end & MOD) ? mask : ~mask;
     **/
    i = (c_end & 255) >> LOG2;
    is_zero = CT_is_equal_zero(c_end & 255);
    u = (ONE << (c_end & MOD))-1;
    v[i] = u ^ (-is_zero & (~u ^ u));
    u = CT_is_greater_than(i+1, 3) * 0xFFFFFFFFFFFFFFFFULL;
    v[(i+1) & 3] = u ^ (-is_zero & (~u ^ u));
    u = CT_is_greater_than(i+2, 3) * 0xFFFFFFFFFFFFFFFFULL;
    v[(i+2) & 3] = u ^ (-is_zero & (~u ^ u));
    u = CT_is_greater_than(i+3, 3) * 0xFFFFFFFFFFFFFFFFULL;
    v[(i+3) & 3] = u ^ (-is_zero & (~u ^ u));
    mask = _mm256_set_epi64x(v[3], v[2], v[1], v[0]);
#elif defined(__SSE2__)
    nblocks = (int32_t)((c_end + 127) >> 7);
    
    /**
     * This is equivalent to
     *    mask = (ONE << (c_end & MOD))-1;
     *    mask = (c_end & MOD) ? mask : ~mask;
     **/
    i = (c_end & 127) >> LOG2;
    is_zero = CT_is_equal_zero(c_end & 127);
    u = (ONE << (c_end & MOD))-1;
    v[i++] = u ^ (-is_zero & (~u ^ u));
    u = CT_is_equal_zero(i & 1) * 0xFFFFFFFFFFFFFFFFULL;
    v[i & 1] = u ^ (-is_zero & (~u ^ u));
    mask = _mm_set_epi64x(v[1], v[0]);
#else
    nblocks = ((c_end + MOD) >> LOG2);
    
    /**
     * This is equivalent to
     *    mask = (ONE << (c_end & MOD))-1;
     *    mask = (c_end & MOD) ? mask : ~mask;
     **/
    mask = (ONE << (c_end & MOD))-1;
    is_zero = ~((c_end & MOD) | -(c_end & MOD)) >> 31;
    mask = mask ^ (-is_zero & (~mask ^ mask));
#endif
    
    /* Note: the 0-th row of T is always 00...0 */
    zero_vector(M4R_ROW(T, 0), T->stride);
    
    /* 1-st row of T, starting at block l */
    zero_vector(M4R_ROW(T, 1), T->stride);
    
#if defined(__SSE2__) || defined(__AVX2__)
    S_ptr = T_ptr = ((vector *)M4R_ROW(T, 1));
    /* r_start-th row of B, starting at block l */
    B_ptr = ((vector *)M4R_ROW(B, r_end-k));
#else
    S_ptr = T_ptr = ((packed_t *)M4R_ROW(T, 1));
    /* r_start-th row of B, starting at block l */
    B_ptr = ((packed_t *)M4R_ROW(B, r_end-k));
#endif
    for (j=0; j<nblocks; j++) {
        *T_ptr = *B_ptr++;
        T_ptr++;
    }
#if   defined(__AVX2__)
    S_ptr[nblocks-1] = _mm256_and_si256(S_ptr[nblocks-1], mask);
#elif defined(__SSE2__)
    S_ptr[nblocks-1] = _mm_and_si128(S_ptr[nblocks-1], mask);
#else
    S_ptr[nblocks-1] &= mask; /* Clear unwanted bits */
#endif
    /* S_ptr is a pointer to the previous row of T */
    
    /* Subsequent rows of T */
    gray_codes_lut = _gray_codes_lut[k];
    for (i=2; i<(1 << k); i++) {
        /* i-th row of T, starting at block l */
        j = GRAY_TO_BIN(i);
        zero_vector(M4R_ROW(T, j), T->stride);
#if defined(__SSE2__) || defined(__AVX2__)
        U_ptr = T_ptr = ((vector *)M4R_ROW(T, j));
        /* (r+Gray[i])-th row of B, starting at block l */
        B_ptr = ((vector *)M4R_ROW(B, r_end - k + gray_codes_lut[i-1]));
#else
        U_ptr = T_ptr = ((packed_t *)M4R_ROW(T, j));
        /* (r+Gray[i])-th row of B, starting at block l */
        B_ptr = ((packed_t *)M4R_ROW(B, r_end - k + gray_codes_lut[i-1]));
#endif
        for (j=0; j<nblocks; j++) {
            *T_ptr = (*S_ptr++ ^ *B_ptr++);
            T_ptr++;
        }
#if   defined(__AVX2__)
        U_ptr[nblocks-1] = _mm256_and_si256(U_ptr[nblocks-1], mask);
#elif defined(__SSE2__)
        U_ptr[nblocks-1] = _mm_and_si128(U_ptr[nblocks-1], mask);
#else
        U_ptr[nblocks-1] &= mask; /* Clear unwanted bits */
#endif
        S_ptr = U_ptr; /* Pointer to the previous row of T */
    }
}

#if defined(__SSE2__) || defined(__AVX2__)
static inline void _m4ri_swap_rows(uint8_t *a, uint8_t *b, uint32_t stride)
{
    int32_t i;
    vector c, *a_ptr, *b_ptr;
    
    a_ptr = (vector *)a;
    b_ptr = (vector *)b;

    for (i=0; i<stride; i+=64) {
        c = *a_ptr; *a_ptr = *b_ptr; *b_ptr = c;
        a_ptr++; b_ptr++;
        c = *a_ptr; *a_ptr = *b_ptr; *b_ptr = c;
        a_ptr++; b_ptr++;
#if !defined(__AVX2__)
        c = *a_ptr; *a_ptr = *b_ptr; *b_ptr = c;
        a_ptr++; b_ptr++;
        c = *a_ptr; *a_ptr = *b_ptr; *b_ptr = c;
        a_ptr++; b_ptr++;
#endif
    }
}
#else
static inline void _m4ri_swap_rows(packed_t *a, packed_t *b, uint32_t nblocks)
{
    int32_t i;
    packed_t v = 0;
    packed_t *a_ptr = a, *b_ptr = b;
    
    for (i=0; i<(int32_t)nblocks; i++) {
        v = *a_ptr; *a_ptr = *b_ptr; *b_ptr = v;
        a_ptr++; b_ptr++;
    }
}
#endif

/**
 *  Add row `dst` with row `src` till column `c_end` and store
 *  the output at row `dst`
 **/
static inline void _m4ri_add_rows_rev(packed_t *dst,
                                      const packed_t *src,
                                      uint32_t c_end)
{
    int32_t i, is_zero, nblocks;
#if   defined(__AVX2__)
    packed_t v[4] = { 0ULL, 0ULL, 0ULL, 0ULL };
#elif defined(__SSE2__)
    packed_t v[2] = { 0ULL, 0ULL };
#endif
#if defined(__SSE2__) || defined(__AVX2__)
    packed_t u;
    vector mask, *dst_ptr = NULL;
    const vector *src_ptr = NULL;
#else
    packed_t mask, *dst_ptr = NULL;
    const packed_t *src_ptr = NULL;
#endif

#if   defined(__AVX2__)
    nblocks = (int32_t)((c_end + 255) >> 8);
    
    /**
     * This is equivalent to
     *    mask = (ONE << (c_end & MOD))-1;
     *    mask = (c_end & MOD) ? mask : ~mask;
     **/
    i = (c_end & 255) >> LOG2;
    is_zero = CT_is_equal_zero(c_end & 255);
    u = (ONE << (c_end & MOD))-1;
    v[i] = u ^ (-is_zero & (~u ^ u));
    u = CT_is_greater_than(i+1, 3) * 0xFFFFFFFFFFFFFFFFULL;
    v[(i+1) & 3] = u ^ (-is_zero & (~u ^ u));
    u = CT_is_greater_than(i+2, 3) * 0xFFFFFFFFFFFFFFFFULL;
    v[(i+2) & 3] = u ^ (-is_zero & (~u ^ u));
    u = CT_is_greater_than(i+3, 3) * 0xFFFFFFFFFFFFFFFFULL;
    v[(i+3) & 3] = u ^ (-is_zero & (~u ^ u));
    mask = _mm256_set_epi64x(v[3], v[2], v[1], v[0]);
    
    dst_ptr = (vector *)dst; src_ptr = (const vector *)src;
    for (i=0; i<(int32_t)(nblocks-1); i++) {
        *dst_ptr ^= *src_ptr++;
        dst_ptr++;
    }
    *dst_ptr = _mm256_xor_si256(*dst_ptr, _mm256_and_si256(*src_ptr, mask));
#elif defined(__SSE2__)
    nblocks = (int32_t)((c_end + 127) >> 7);
    
    /**
     * This is equivalent to
     *    mask = (ONE << (c_end & MOD))-1;
     *    mask = (c_end & MOD) ? mask : ~mask;
     **/
    i = (c_end & 127) >> LOG2;
    is_zero = CT_is_equal_zero(c_end & 127);
    u = (ONE << (c_end & MOD))-1;
    v[i++] = u ^ (-is_zero & (~u ^ u));
    u = CT_is_equal_zero(i & 1) * 0xFFFFFFFFFFFFFFFFULL;
    v[i & 1] = u ^ (-is_zero & (~u ^ u));
    mask = _mm_set_epi64x(v[1], v[0]);
    
    dst_ptr = (vector *)dst; src_ptr = (const vector *)src;
    for (i=0; i<(int32_t)(nblocks-1); i++) {
        *dst_ptr ^= *src_ptr++;
        dst_ptr++;
    }
    *dst_ptr = _mm_xor_si128(*dst_ptr, _mm_and_si128(*src_ptr, mask));
#else
    nblocks = (int32_t)((c_end + MOD) >> LOG2);
    
    /**
     * This is equivalent to
     *    mask = (ONE << (c_end & MOD))-1;
     *    mask = (c_end & MOD) ? mask : ~mask;
     **/
    mask = (ONE << (c_end & MOD))-1;
    is_zero = ~((c_end & MOD) | -(c_end & MOD)) >> 31;
    mask = mask ^ (-is_zero & (~mask ^ mask));
    
    dst_ptr = dst; src_ptr = src;
    for (i=0; i<(int32_t)(nblocks-1); i++) {
        *dst_ptr ^= *src_ptr++;
        dst_ptr++;
    }
    dst[nblocks-1] ^= (*src_ptr & mask);
#endif
}

static inline void _m4ri_add_rows_rev_from_gray_table(matrix_ff2* A,
                                                      const matrix_ff2 *T,
                                                      uint32_t r_start,
                                                      uint32_t r_end,
                                                      uint32_t c_end,
                                                      uint32_t k)
{
    int32_t i;
    uint8_t *ptr, x, x0, x1, mask;
    uint32_t hi, lo;
    uint8_t mask_hi = 0, mask_lo = 0;
    
    hi = (c_end-1) >> 3;
    lo = (c_end-k) >> 3;
    mask_hi = (1 << (c_end & 7))-1;
    mask_lo = ~((1 << ((c_end-k) & 7))-1);
    mask = (mask_hi & mask_lo);
    mask = CT_mux(CT_is_equal_zero(mask_hi), mask_lo, mask);
    
    for (i=(int32_t)r_start-1; i>=(int32_t)r_end; i--) {
        ptr = M4R_ROW(A, i) + hi;
        x0 = (((*ptr & mask_hi) << (STRIPE_SIZE - ((c_end-k) & 7))) |
              ((*(ptr-1) & mask_lo) >> ((c_end-k) & 7)));
        x1 = (*ptr & mask) >> ((c_end-k) & 7);
        x = (uint8_t)CT_mux(CT_is_equal(hi, lo), x1, x0);
        x &= ((1 << k)-1);
        _m4ri_add_rows_rev((packed_t *)M4R_ROW(A, i), (const packed_t *)M4R_ROW(T, x), c_end);
    }
}

uint32_t _m4ri_gauss_submatrix(matrix_ff2* A,
                               uint32_t r,
                               uint32_t c,
                               uint32_t r_end,
                               uint32_t k)
{
    int32_t i, j, l, found;
    uint32_t r_start = r;
    packed_t *u = NULL, *v = NULL;
    
    for (j=c-1; j>=(int32_t)(c-k); j--) {
        found = 0;
        for (i=r_start-1; i>=(int32_t)r_end; i--) {
            v = (packed_t *)M4R_ROW(A, i);
            for (l=0; l<(int32_t)(c-j-1); l++) {
                if (is_bit_set(v, (c-l-1))) {
                    _m4ri_add_rows_rev((packed_t *)M4R_ROW(A, i),
                                       (const packed_t *)M4R_ROW(A, (r-l-1)),
                                       c-l);
                }
            }
            if (is_bit_set(v, j)) {
#if defined(__SSE2__) || defined(__AVX2__)
                _m4ri_swap_rows(M4R_ROW(A, i),
                                M4R_ROW(A, r_start-1),
                                A->stride);
#else
                _m4ri_swap_rows((packed_t *)M4R_ROW(A, i),
                                (packed_t *)M4R_ROW(A, r_start-1),
                                A->nblocks);
#endif
                for (l=r-1; l>=(int32_t)r_start; l--) {
                    u = (packed_t *)M4R_ROW(A, l);
                    if (is_bit_set(u, j)) {
                        _m4ri_add_rows_rev((packed_t *)M4R_ROW(A, l),
                                           (const packed_t *)M4R_ROW(A, r_start-1),
                                           (uint32_t)j+1);
                    }
                }
                r_start--;
                found = 1;
                break;
            }
        }
        if (!found)
            return (uint32_t)(c-j-1);
    }
    
    return (uint32_t)(c-j-1);
}

uint32_t m4r_rref(matrix_ff2* A)
{
    int32_t r = 0, c = 0, rank = 0;
    int32_t k = STRIPE_SIZE, rk;
    matrix_ff2 *T = NULL;
    
    if (!(T = alloc_matrix_ff2(sizeof(_gray_codes_lut8), A->ncols)))
        return 0;
    
    r = A->nrows;
    c = A->ncols;
    while (c > 0) {
        if (c - k < 0) {
            k = c;
        }
        rk = _m4ri_gauss_submatrix(A, r, c, 0, k);
        if (rk > 0) {
            _m4ri_make_table_rev(T, A, r, c, rk);
            _m4ri_add_rows_rev_from_gray_table(A, T, r-rk, 0, c, rk);
            _m4ri_add_rows_rev_from_gray_table(A, T, A->nrows, r, c, rk);
        }
        r -= rk;
        c -= rk;
        rank += rk;
        if (rk != k)
            --c;
    }
    
    free_matrix_ff2(T);
    
    return (uint32_t)rank;

}
