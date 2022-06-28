#ifndef MPFQ_2_192_H_
#define MPFQ_2_192_H_

/* MPFQ generated file -- do not edit */

#include "mpfq.h"
#include "mpfq_gf2n_common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <ctype.h>
#include <emmintrin.h>

#ifdef	MPFQ_LAST_GENERATED_TAG
#undef	MPFQ_LAST_GENERATED_TAG
#endif
#define MPFQ_LAST_GENERATED_TAG      2_192

#ifndef GMP_LIMB_BITS
#error "Please arrange so that GMP_LIMB_BITS is defined before including this file"
#endif

#if !(GMP_LIMB_BITS == 64)
#error "Constraints not met for this file: GMP_LIMB_BITS == 64"
#endif
/* Active handler: field */
/* Automatically generated code for GF(2^192) */
/* Definition polynomial P = X^192 + X^7 + X^2 + X + 1 */
/* Active handler: trivialities */
/* Active handler: io */
/* Active handler: linearops */
/* Active handler: inversion */
/* Active handler: reduction */
/* Active handler: mul */
/* Options used: slice=4 coeffs=[192, 7, 2, 1, 0] tag=2_192 helper=/home/mark/mpfq10rc3/gf2n/helper/helper w=64 n=192 table=/home/mark/mpfq10rc3/gf2x/wizard.table output_path=/home/mark/mpfq10rc3/gf2n */

typedef mpfq_2_field mpfq_2_192_field;
typedef mpfq_2_dst_field mpfq_2_192_dst_field;

typedef unsigned long mpfq_2_192_elt[3];
typedef unsigned long * mpfq_2_192_dst_elt;
typedef const unsigned long * mpfq_2_192_src_elt;

typedef unsigned long mpfq_2_192_elt_ur[6];
typedef unsigned long * mpfq_2_192_dst_elt_ur;
typedef const unsigned long * mpfq_2_192_src_elt_ur;


/* Functions operating on the field structure */
#define mpfq_2_192_field_characteristic(f, x)	mpz_set_ui(x,2);
#define mpfq_2_192_field_degree(f)	192
static inline
void mpfq_2_192_field_init(mpfq_2_192_dst_field);
#define mpfq_2_192_field_clear(f)	/**/
#define mpfq_2_192_field_specify(f, x, y)	/**/
static inline
void mpfq_2_192_field_setopt(mpfq_2_192_dst_field, unsigned long, void *);

/* Element allocation functions */
#define mpfq_2_192_init(f, px)	/**/
#define mpfq_2_192_clear(f, px)	/**/

/* Elementary assignment functions */
static inline
void mpfq_2_192_set(mpfq_2_192_dst_field, mpfq_2_192_dst_elt, mpfq_2_192_src_elt);
static inline
void mpfq_2_192_set_ui(mpfq_2_192_dst_field, mpfq_2_192_dst_elt, unsigned long);
static inline
unsigned long mpfq_2_192_get_ui(mpfq_2_192_dst_field, mpfq_2_192_src_elt);
static inline
void mpfq_2_192_set_mpn(mpfq_2_192_dst_field, mpfq_2_192_dst_elt, mp_limb_t *, size_t);
static inline
void mpfq_2_192_set_mpz(mpfq_2_192_dst_field, mpfq_2_192_dst_elt, mpz_t);
static inline
void mpfq_2_192_get_mpn(mpfq_2_192_dst_field, mp_limb_t *, mpfq_2_192_src_elt);
static inline
void mpfq_2_192_get_mpz(mpfq_2_192_dst_field, mpz_t, mpfq_2_192_src_elt);
static inline
void mpfq_2_192_set_uipoly(mpfq_2_192_dst_field, mpfq_2_192_dst_elt, unsigned long);
static inline
void mpfq_2_192_set_uipoly_wide(mpfq_2_192_dst_field, mpfq_2_192_dst_elt, const unsigned long *, unsigned int);
static inline
unsigned long mpfq_2_192_get_uipoly(mpfq_2_192_dst_field, mpfq_2_192_src_elt);
static inline
void mpfq_2_192_get_uipoly_wide(mpfq_2_192_dst_field, unsigned long *, mpfq_2_192_src_elt);

/* Assignment of random values */
static inline
void mpfq_2_192_random(mpfq_2_192_dst_field, mpfq_2_192_dst_elt);
static inline
void mpfq_2_192_random2(mpfq_2_192_dst_field, mpfq_2_192_dst_elt);

/* Arithmetic operations on elements */
static inline
void mpfq_2_192_add(mpfq_2_192_dst_field, mpfq_2_192_dst_elt, mpfq_2_192_src_elt, mpfq_2_192_src_elt);
#define mpfq_2_192_sub(K, r, s1, s2)	mpfq_2_192_add(K,r,s1,s2)
#define mpfq_2_192_neg(K, r, s)	mpfq_2_192_set(K,r,s)
static inline
void mpfq_2_192_mul(mpfq_2_192_dst_field, mpfq_2_192_dst_elt, mpfq_2_192_src_elt, mpfq_2_192_src_elt);
static inline
void mpfq_2_192_sqr(mpfq_2_192_dst_field, mpfq_2_192_dst_elt, mpfq_2_192_src_elt);
#define mpfq_2_192_is_sqr(f, p)	1
static inline
int mpfq_2_192_sqrt(mpfq_2_192_dst_field, mpfq_2_192_dst_elt, mpfq_2_192_src_elt);
static inline
void mpfq_2_192_pow(mpfq_2_192_dst_field, mpfq_2_192_dst_elt, mpfq_2_192_src_elt, unsigned long *, size_t);
#define mpfq_2_192_frobenius(K, r, s)	mpfq_2_192_sqr(K,r,s)
static inline
void mpfq_2_192_add_ui(mpfq_2_192_dst_field, mpfq_2_192_dst_elt, mpfq_2_192_src_elt, unsigned long);
#define mpfq_2_192_sub_ui(K, r, s1, s2)	mpfq_2_192_add_ui(K,r,s1,s2)
static inline
void mpfq_2_192_mul_ui(mpfq_2_192_dst_field, mpfq_2_192_dst_elt, mpfq_2_192_src_elt, unsigned long);
static inline
void mpfq_2_192_add_uipoly(mpfq_2_192_dst_field, mpfq_2_192_dst_elt, mpfq_2_192_src_elt, unsigned long);
#define mpfq_2_192_sub_uipoly(K, r, s1, s2)	mpfq_2_192_add_uipoly(K,r,s1,s2)
static inline
void mpfq_2_192_mul_uipoly(mpfq_2_192_dst_field, mpfq_2_192_dst_elt, mpfq_2_192_src_elt, unsigned long);
static inline
int mpfq_2_192_inv(mpfq_2_192_dst_field, mpfq_2_192_dst_elt, mpfq_2_192_src_elt);
static inline
void mpfq_2_192_longaddshift_left(unsigned long *, const unsigned long *, int, int);
static inline
void mpfq_2_192_longshift_left(unsigned long *, const unsigned long *, int, int);
static inline
void mpfq_2_192_as_solve(mpfq_2_192_dst_field, mpfq_2_192_dst_elt, mpfq_2_192_src_elt);
static inline
unsigned long mpfq_2_192_trace(mpfq_2_192_dst_field, mpfq_2_192_src_elt);

/* Operations involving unreduced elements */
#define mpfq_2_192_elt_ur_init(f, px)	/**/
#define mpfq_2_192_elt_ur_clear(f, px)	/**/
static inline
void mpfq_2_192_elt_ur_set(mpfq_2_192_dst_field, mpfq_2_192_dst_elt_ur, mpfq_2_192_src_elt_ur);
static inline
void mpfq_2_192_elt_ur_add(mpfq_2_192_dst_field, mpfq_2_192_dst_elt_ur, mpfq_2_192_src_elt_ur, mpfq_2_192_src_elt_ur);
#define mpfq_2_192_elt_ur_neg(K, r, s)	mpfq_2_192_elt_ur_set(K,r,s)
#define mpfq_2_192_elt_ur_sub(K, r, s1, s2)	mpfq_2_192_elt_ur_add(K,r,s1,s2)
static inline
void mpfq_2_192_mul_ur(mpfq_2_192_dst_field, mpfq_2_192_dst_elt_ur, mpfq_2_192_src_elt, mpfq_2_192_src_elt);
static inline
void mpfq_2_192_addmul_192x128(unsigned long *, const unsigned long *, const unsigned long *);
static inline
void mpfq_2_192_addmul_192x64(unsigned long *, const unsigned long *, const unsigned long *);
static inline
void mpfq_2_192_sqr_ur(mpfq_2_192_dst_field, mpfq_2_192_dst_elt_ur, mpfq_2_192_src_elt);
static inline
void mpfq_2_192_reduce(mpfq_2_192_dst_field, mpfq_2_192_dst_elt, mpfq_2_192_dst_elt_ur);

/* Comparison functions */
static inline
int mpfq_2_192_cmp(mpfq_2_192_dst_field, mpfq_2_192_src_elt, mpfq_2_192_src_elt);
static inline
int mpfq_2_192_cmp_ui(mpfq_2_192_dst_field, mpfq_2_192_src_elt, unsigned long);

/* Vector allocation functions */
#define mpfq_2_192_vec_init(f, px, n)	/**/
#define mpfq_2_192_vec_clear(f, px, n)	/**/

/* Montgomery representation conversion functions */

/* Input/output functions */
void mpfq_2_192_asprint(mpfq_2_192_dst_field, char * *, mpfq_2_192_src_elt);
void mpfq_2_192_fprint(mpfq_2_192_dst_field, FILE *, mpfq_2_192_src_elt);
#define mpfq_2_192_print(k, x)	mpfq_2_192_fprint(k,stdout,x)
int mpfq_2_192_sscan(mpfq_2_192_dst_field, mpfq_2_192_dst_elt, const char *);
int mpfq_2_192_fscan(mpfq_2_192_dst_field, FILE *, mpfq_2_192_dst_elt);
#define mpfq_2_192_scan(k, x)	mpfq_2_192_fscan(k,stdout,x)

/* Implementations for inlines */
static inline
void mpfq_2_192_field_init(mpfq_2_192_dst_field f)
{
    f->io_type=16;
}

static inline
void mpfq_2_192_field_setopt(mpfq_2_192_dst_field f, unsigned long x, void * y)
{
    assert(x == MPFQ_IO_TYPE);
    f->io_type=((unsigned long*)y)[0];
}

static inline
void mpfq_2_192_set(mpfq_2_192_dst_field K MAYBE_UNUSED, mpfq_2_192_dst_elt r, mpfq_2_192_src_elt s)
{
    memcpy(r, s, sizeof(mpfq_2_192_elt));
}

static inline
void mpfq_2_192_set_ui(mpfq_2_192_dst_field K MAYBE_UNUSED, mpfq_2_192_dst_elt r, unsigned long x)
{
    r[0] = x & 1UL;
    memset(r + 1, 0, sizeof(mpfq_2_192_elt) - sizeof(unsigned long));
}

static inline
unsigned long mpfq_2_192_get_ui(mpfq_2_192_dst_field K MAYBE_UNUSED, mpfq_2_192_src_elt r)
{
    return r[0] & 1UL;
}

static inline
void mpfq_2_192_set_mpn(mpfq_2_192_dst_field K MAYBE_UNUSED, mpfq_2_192_dst_elt r, mp_limb_t * x, size_t n)
{
    r[0] = LIKELY(n > 0) ? (x[0] & 1UL) : 0;
    memset(r + 1, 0, sizeof(mpfq_2_192_elt) - sizeof(unsigned long));
}

static inline
void mpfq_2_192_set_mpz(mpfq_2_192_dst_field K MAYBE_UNUSED, mpfq_2_192_dst_elt r, mpz_t z)
{
    r[0] = mpz_getlimbn(z,0) & 1UL;
    memset(r + 1, 0, sizeof(mpfq_2_192_elt) - sizeof(unsigned long));
}

static inline
void mpfq_2_192_get_mpn(mpfq_2_192_dst_field K MAYBE_UNUSED, mp_limb_t * p, mpfq_2_192_src_elt r)
{
    p[0] = r[0] & 1UL;
    memset(p + 1, 0, (3 - 1) * sizeof(mp_limb_t));
}

static inline
void mpfq_2_192_get_mpz(mpfq_2_192_dst_field K MAYBE_UNUSED, mpz_t z, mpfq_2_192_src_elt r)
{
    mpz_set_ui(z, r[0] & 1UL);
}

static inline
void mpfq_2_192_set_uipoly(mpfq_2_192_dst_field K MAYBE_UNUSED, mpfq_2_192_dst_elt r, unsigned long x)
{
    r[0] = x;
    memset(r + 1, 0, sizeof(mpfq_2_192_elt) - sizeof(unsigned long));
}

static inline
void mpfq_2_192_set_uipoly_wide(mpfq_2_192_dst_field K MAYBE_UNUSED, mpfq_2_192_dst_elt r, const unsigned long * x, unsigned int n)
{
    unsigned int i;
    for (i = 0 ; i < n && i < 3 ; i++)
        r[i] = x[i];
}

static inline
unsigned long mpfq_2_192_get_uipoly(mpfq_2_192_dst_field K MAYBE_UNUSED, mpfq_2_192_src_elt r)
{
    return r[0];
}

static inline
void mpfq_2_192_get_uipoly_wide(mpfq_2_192_dst_field K MAYBE_UNUSED, unsigned long * r, mpfq_2_192_src_elt x)
{
    unsigned int i;
    for(i = 0 ; i < 3 ; i++) r[i] = x[i];
}

static inline
void mpfq_2_192_random(mpfq_2_192_dst_field K MAYBE_UNUSED, mpfq_2_192_dst_elt r)
{
    mpn_random(r, 3);
}

static inline
void mpfq_2_192_random2(mpfq_2_192_dst_field K MAYBE_UNUSED, mpfq_2_192_dst_elt r)
{
    mpn_random2(r, 3);
}

static inline
void mpfq_2_192_add(mpfq_2_192_dst_field K MAYBE_UNUSED, mpfq_2_192_dst_elt r, mpfq_2_192_src_elt s1, mpfq_2_192_src_elt s2)
{
    int i;
    for(i = 0 ; i < 3 ; i++)
        r[i] = s1[i] ^ s2[i];
}

static inline
void mpfq_2_192_mul(mpfq_2_192_dst_field K MAYBE_UNUSED, mpfq_2_192_dst_elt r, mpfq_2_192_src_elt s1, mpfq_2_192_src_elt s2)
{
    mpfq_2_192_elt_ur t;
    mpfq_2_192_mul_ur(K, t, s1, s2);
    mpfq_2_192_reduce(K, r, t);
}

static inline
void mpfq_2_192_sqr(mpfq_2_192_dst_field K MAYBE_UNUSED, mpfq_2_192_dst_elt r, mpfq_2_192_src_elt s)
{
    mpfq_2_192_elt_ur t;
    mpfq_2_192_sqr_ur(K, t, s);
    mpfq_2_192_reduce(K, r, t);
}

static inline
int mpfq_2_192_sqrt(mpfq_2_192_dst_field K, mpfq_2_192_dst_elt r, mpfq_2_192_src_elt s)
{
    static const mp_limb_t shuffle_table[256] = {
        0, 1, 16, 17, 2, 3, 18, 19,
        32, 33, 48, 49, 34, 35, 50, 51,
        4, 5, 20, 21, 6, 7, 22, 23,
        36, 37, 52, 53, 38, 39, 54, 55,
        64, 65, 80, 81, 66, 67, 82, 83,
        96, 97, 112, 113, 98, 99, 114, 115,
        68, 69, 84, 85, 70, 71, 86, 87,
        100, 101, 116, 117, 102, 103, 118, 119,
        8, 9, 24, 25, 10, 11, 26, 27,
        40, 41, 56, 57, 42, 43, 58, 59,
        12, 13, 28, 29, 14, 15, 30, 31,
        44, 45, 60, 61, 46, 47, 62, 63,
        72, 73, 88, 89, 74, 75, 90, 91,
        104, 105, 120, 121, 106, 107, 122, 123,
        76, 77, 92, 93, 78, 79, 94, 95,
        108, 109, 124, 125, 110, 111, 126, 127,
        128, 129, 144, 145, 130, 131, 146, 147,
        160, 161, 176, 177, 162, 163, 178, 179,
        132, 133, 148, 149, 134, 135, 150, 151,
        164, 165, 180, 181, 166, 167, 182, 183,
        192, 193, 208, 209, 194, 195, 210, 211,
        224, 225, 240, 241, 226, 227, 242, 243,
        196, 197, 212, 213, 198, 199, 214, 215,
        228, 229, 244, 245, 230, 231, 246, 247,
        136, 137, 152, 153, 138, 139, 154, 155,
        168, 169, 184, 185, 170, 171, 186, 187,
        140, 141, 156, 157, 142, 143, 158, 159,
        172, 173, 188, 189, 174, 175, 190, 191,
        200, 201, 216, 217, 202, 203, 218, 219,
        232, 233, 248, 249, 234, 235, 250, 251,
        204, 205, 220, 221, 206, 207, 222, 223,
        236, 237, 252, 253, 238, 239, 254, 255,
    };
    
    mpfq_2_192_elt sqrt_t ={ 0xdb6db6db6db6db27UL, 0x24924924b6db6db6UL, 0x9249249249249249UL, }
    ;
    
    mpfq_2_192_elt odd, even;
    mpfq_2_192_elt_ur odd_t;
    mp_limb_t t;
#define	EVEN_MASK	(((mp_limb_t)-1)/3UL)
#define	ODD_MASK	((EVEN_MASK)<<1)
    unsigned int i;
    for(i = 0 ; i < 3 ; i++) {
        even[i] = s[i] & EVEN_MASK;
    }
    for(i = 0 ; i < 1 ; i++) {
        t = even[2*i];   t |= t >> 7;
              even[i]  = shuffle_table[t & 255];
        t >>= 16; even[i] |= shuffle_table[t & 255] << 8;
        t >>= 16; even[i] |= shuffle_table[t & 255] << 16;
        t >>= 16; even[i] |= shuffle_table[t & 255] << 24;
        t = even[2*i+1]; t |= t >> 7;
              even[i] |= shuffle_table[t & 255] << 32;
        t >>= 16; even[i] |= shuffle_table[t & 255] << 40;
        t >>= 16; even[i] |= shuffle_table[t & 255] << 48;
        t >>= 16; even[i] |= shuffle_table[t & 255] << 56;
    }
    t = even[2];   t |= t >> 7;
    even[1]  = shuffle_table[t & 255];
    t >>= 16; even[1] |= shuffle_table[t & 255] << 8;
    t >>= 16; even[1] |= shuffle_table[t & 255] << 16;
    t >>= 16; even[1] |= shuffle_table[t & 255] << 24;
    memset(even + 2, 0, 1 * sizeof(mp_limb_t));
    for(i = 0 ; i < 3 ; i++) {
        odd[i] = (s[i] & ODD_MASK) >> 1;
    }
    for(i = 0 ; i < 1 ; i++) {
        t = odd[2*i];   t |= t >> 7;
              odd[i]  = shuffle_table[t & 255];
        t >>= 16; odd[i] |= shuffle_table[t & 255] << 8;
        t >>= 16; odd[i] |= shuffle_table[t & 255] << 16;
        t >>= 16; odd[i] |= shuffle_table[t & 255] << 24;
        t = odd[2*i+1]; t |= t >> 7;
              odd[i] |= shuffle_table[t & 255] << 32;
        t >>= 16; odd[i] |= shuffle_table[t & 255] << 40;
        t >>= 16; odd[i] |= shuffle_table[t & 255] << 48;
        t >>= 16; odd[i] |= shuffle_table[t & 255] << 56;
    }
    t = odd[2];   t |= t >> 7;
    odd[1]  = shuffle_table[t & 255];
    t >>= 16; odd[1] |= shuffle_table[t & 255] << 8;
    t >>= 16; odd[1] |= shuffle_table[t & 255] << 16;
    t >>= 16; odd[1] |= shuffle_table[t & 255] << 24;
    memset(odd + 2, 0, 1 * sizeof(mp_limb_t));
    mpfq_2_192_mul_ur(K, odd_t, odd, sqrt_t);
    for(i = 0 ; i < (3+1)/2 ; i++) {
        odd_t[i] ^= even[i];
    }
    /* mpfq_2_192_print(K, stdout, odd_t); */
    mpfq_2_192_reduce(K, r, odd_t);
    /* mpfq_2_192_print(K, stdout, r); */
    /* fprintf(stdout, "\n"); */
    /* fflush(stdout); */
    return 1;
}

static inline
void mpfq_2_192_pow(mpfq_2_192_dst_field k, mpfq_2_192_dst_elt res, mpfq_2_192_src_elt r, unsigned long * x, size_t n)
{
    mpfq_2_192_elt u, a;
    long i, j, lead;     /* it is a signed type */
    unsigned long mask;
    
    assert (n>0);
    
    /* get the correct (i,j) position of the most significant bit in x */
    for(i = n-1; i>=0 && x[i]==0; i--)
        ;
    if (i < 0) {
        mpfq_2_192_set_ui(k, res, 0);
        return;
    }
    j = 64 - 1;
    mask = (1UL<<j);
    for( ; (x[i]&mask)==0 ;j--, mask>>=1)
        ;
    lead = i*64+j;      /* Ensured. */
    
    mpfq_2_192_init(k, &u);
    mpfq_2_192_init(k, &a);
    mpfq_2_192_set(k, a, r);
    for( ; lead > 0; lead--) {
        if (j-- == 0) {
            i--;
            j = 64-1;
            mask = (1UL<<j);
        } else {
            mask >>= 1;
        }
        if (x[i]&mask) {
            mpfq_2_192_sqr(k, u, a);
            mpfq_2_192_mul(k, a, u, r);
        } else {
            mpfq_2_192_sqr(k, a,a);
        }
    }
    mpfq_2_192_set(k, res, a);
    mpfq_2_192_clear(k, &u);
    mpfq_2_192_clear(k, &a);
}

static inline
void mpfq_2_192_add_ui(mpfq_2_192_dst_field K MAYBE_UNUSED, mpfq_2_192_dst_elt r, mpfq_2_192_src_elt s, unsigned long x)
{
    mpfq_2_192_set(K, r, s);
    r[0] ^= x & 1UL;
}

static inline
void mpfq_2_192_mul_ui(mpfq_2_192_dst_field K MAYBE_UNUSED, mpfq_2_192_dst_elt r, mpfq_2_192_src_elt s, unsigned long x)
{
    if (x & 1UL) {
        mpfq_2_192_set(K, r, s);
    } else {
        memset(r, 0, sizeof(mpfq_2_192_elt));
    }
}

static inline
void mpfq_2_192_add_uipoly(mpfq_2_192_dst_field K MAYBE_UNUSED, mpfq_2_192_dst_elt r, mpfq_2_192_src_elt s, unsigned long x)
{
    mpfq_2_192_set(K, r, s);
    r[0] ^= x;
}

static inline
void mpfq_2_192_mul_uipoly(mpfq_2_192_dst_field k, mpfq_2_192_dst_elt r, mpfq_2_192_src_elt s, unsigned long x)
{
    mpfq_2_192_elt xx;
    mpfq_2_192_init(k, &xx);
    mpfq_2_192_set_uipoly(k, xx, x);
    mpfq_2_192_mul(k, r, s, xx);
    mpfq_2_192_clear(k, &xx);
}

static inline
int mpfq_2_192_inv(mpfq_2_192_dst_field K MAYBE_UNUSED, mpfq_2_192_dst_elt r, mpfq_2_192_src_elt s)
{
    mp_limb_t a[4] = { 0x8000000000000000UL, 0x43UL, 0x0UL, 0x8000000000000000UL, };
    mp_limb_t b[4];
    mp_limb_t u[4] = { 0, };
    mp_limb_t v[4] = { 1, 0, };
    mp_limb_t x;
    mp_size_t ia, ib;
    int i,d;
    
    if (mpfq_2_192_cmp_ui(K, s, 0UL) == 0)
        return 0;
    {
        unsigned long z;
        z = s[0] << 63;
        b[0] = z;
        z = s[0] >>  1 ^ s[1] << 63;
        b[1] = z;
        z = s[1] >>  1 ^ s[2] << 63;
        b[2] = z;
        z = s[2] >>  1;
        b[3] = z;
    }
    ib = clzlx(b, 4);
    ia = 0;
    
    mpfq_2_192_longshift_left(b,b,4,ib);
    
    for(d = ib - ia ; ; ) {
            if (d == 0) {
                    for(i = 0 ; i < 4 ; i++) v[i] ^= u[i];
            b[0] ^= a[0]; x = b[0];
            b[1] ^= a[1]; x |= b[1];
            b[2] ^= a[2]; x |= b[2];
            b[3] ^= a[3]; x |= b[3];
                    if (!x) { memcpy(r,u,3 * sizeof(mp_limb_t)); return 1; }
                    mp_limb_t t = clzlx(b,4);
                    ib += t;
                    d += t;
                    mpfq_2_192_longshift_left(b,b,4,t);
            }
            for(;d > 0;) {
                    mpfq_2_192_longaddshift_left(u,v,4,d);
            a[0] ^= b[0]; x = a[0];
            a[1] ^= b[1]; x |= a[1];
            a[2] ^= b[2]; x |= a[2];
            a[3] ^= b[3]; x |= a[3];
                    if (!x) { memcpy(r,v,3 * sizeof(mp_limb_t)); return 1; }
                    mp_limb_t t = clzlx(a,4);
                    ia += t;
                    d -= t;
                    mpfq_2_192_longshift_left(a,a,4,t);
            } 
            if (d == 0) {
                    for(i = 0 ; i < 4 ; i++) u[i] ^= v[i];
            a[0] ^= b[0]; x = a[0];
            a[1] ^= b[1]; x |= a[1];
            a[2] ^= b[2]; x |= a[2];
            a[3] ^= b[3]; x |= a[3];
                    if (!x) { memcpy(r,v,3 * sizeof(mp_limb_t)); return 1; }
                    mp_limb_t t = clzlx(a,4);
                    ia += t;
                    d -= t;
                    mpfq_2_192_longshift_left(a,a,4,t);
            }
            for(;d < 0;) {
                    mpfq_2_192_longaddshift_left(v,u,4,-d);
            b[0] ^= a[0]; x = b[0];
            b[1] ^= a[1]; x |= b[1];
            b[2] ^= a[2]; x |= b[2];
            b[3] ^= a[3]; x |= b[3];
                    if (!x) { memcpy(r,u,3 * sizeof(mp_limb_t)); return 1; }
                    mp_limb_t t = clzlx(b,4);
                    ib += t;
                    d += t;
                    mpfq_2_192_longshift_left(b,b,4,t);
            }
    }
}

static inline
void mpfq_2_192_longaddshift_left(unsigned long * dst, const unsigned long * src, int n, int s)
{
    int m = s / 64;
    int i;
    s %= 64;
    if (s>0) {
        for(i = n-m-1 ; i > 0 ; i--) {
            dst[m+i] ^= src[i] << s ^ src[i-1] >> (64-s);
        }
        dst[m] ^= src[0] << s;
    } else {
        for(i = n-m-1 ; i > 0 ; i--) {
            dst[m+i] ^= src[i];
        }
        dst[m] ^= src[0];
    }
}

static inline
void mpfq_2_192_longshift_left(unsigned long * dst, const unsigned long * src, int n, int s)
{
    int m = s / 64;
    int i;
    s %= 64;
    if (s > 0) {
        for(i = n-m-1 ; i > 0 ; i--) {
            dst[m+i] = src[i] << s ^ src[i-1] >> (64-s);
        }
        dst[m] = src[0] << s;
    } else {
        for(i = n-m-1 ; i > 0 ; i--) {
            dst[m+i] = src[i];
        }
        dst[m] = src[0];
    }
    for(i = m-1 ; i>= 0 ; i--) {
        dst[i] = 0UL;
    }
    
}

static inline
void mpfq_2_192_as_solve(mpfq_2_192_dst_field K, mpfq_2_192_dst_elt r, mpfq_2_192_src_elt s)
{
    static const mpfq_2_192_elt t[192] = {
        { 0xde3f68d1db2e8362UL, 0xeb9ecf108dd13746UL, 0x824529f438793366UL, },
        { 0x824ef9328c9871a6UL, 0x7dc4332c0e6aa1d2UL, 0x5ccaddd7f44f1aUL, },
        { 0x824ef9328c9871a4UL, 0x7dc4332c0e6aa1d2UL, 0x5ccaddd7f44f1aUL, },
        { 0x620e4336294a637cUL, 0x66b4294274f22e1eUL, 0x4e9d3b31bc0d9af0UL, },
        { 0x824ef9328c9871a0UL, 0x7dc4332c0e6aa1d2UL, 0x5ccaddd7f44f1aUL, },
        { 0xac22b24301b97a08UL, 0xe0afda9df7b62519UL, 0xce9823d61aa33c2dUL, },
        { 0x620e4336294a6374UL, 0x66b4294274f22e1eUL, 0x4e9d3b31bc0d9af0UL, },
        { 0xbc302be7f364f054UL, 0x8d2ae653f9231958UL, 0xccd812c58474a996UL, },
        { 0x824ef9328c9871b0UL, 0x7dc4332c0e6aa1d2UL, 0x5ccaddd7f44f1aUL, },
        { 0x5e6a4c685412e4faUL, 0x6b96003c62cbb84aUL, 0x294b031e47dbbce0UL, },
        { 0xac22b24301b97a28UL, 0xe0afda9df7b62519UL, 0xce9823d61aa33c2dUL, },
        { 0x53640a6375bbb50UL, 0xb7423d891417e3UL, 0x8255fecc0190cdb5UL, },
        { 0x620e4336294a6334UL, 0x66b4294274f22e1eUL, 0x4e9d3b31bc0d9af0UL, },
        { 0x90746c1079dfa7b0UL, 0xed29287c14df7bc0UL, 0x3a8acb6e0d60147UL, },
        { 0xbc302be7f364f0d4UL, 0x8d2ae653f9231958UL, 0xccd812c58474a996UL, },
        { 0xabeb3f668b064290UL, 0x71d43035889debc2UL, 0x80481091d36417aaUL, },
        { 0x824ef9328c9870b0UL, 0x7dc4332c0e6aa1d2UL, 0x5ccaddd7f44f1aUL, },
        { 0x88872b6ae009ac56UL, 0xa10f1b1fe5a64d50UL, 0xa95e989976b0774dUL, },
        { 0x5e6a4c685412e6faUL, 0x6b96003c62cbb84aUL, 0x294b031e47dbbce0UL, },
        { 0xcc672cce2752978aUL, 0x7bd79958623420d9UL, 0x2b57f8d00ee96641UL, },
        { 0xac22b24301b97e28UL, 0xe0afda9df7b62519UL, 0xce9823d61aa33c2dUL, },
        { 0xdcc0a54266ce0334UL, 0x7a5267027beeef7eUL, 0x4ec0e47ff02fd554UL, },
        { 0x53640a6375bb350UL, 0xb7423d891417e3UL, 0x8255fecc0190cdb5UL, },
        { 0xc4c7f220455cbdacUL, 0x5a7401739dd82f08UL, 0x83e1fdec064985f0UL, },
        { 0x620e4336294a7334UL, 0x66b4294274f22e1eUL, 0x4e9d3b31bc0d9af0UL, },
        { 0xdbe9387f5035adf2UL, 0x8729d9a8138ad743UL, 0x67c7fa8459e8d87dUL, },
        { 0x90746c1079df87b0UL, 0xed29287c14df7bc0UL, 0x3a8acb6e0d60147UL, },
        { 0x20d79b53cc6044d2UL, 0x4114a4019535110dUL, 0xe5cf9e1b6dada400UL, },
        { 0xbc302be7f364b0d4UL, 0x8d2ae653f9231958UL, 0xccd812c58474a996UL, },
        { 0x282643a7a34c2aa4UL, 0x60b51047edec7398UL, 0xcf70bf736524a8d1UL, },
        { 0xabeb3f668b06c290UL, 0x71d43035889debc2UL, 0x80481091d36417aaUL, },
        { 0x9048c0ac4c01da4UL, 0x4da06c97e6929390UL, 0xcf3df318689ee7caUL, },
        { 0x824ef9328c9970b0UL, 0x7dc4332c0e6aa1d2UL, 0x5ccaddd7f44f1aUL, },
        { 0x9fad1c77216303e0UL, 0xce04099186d5093cUL, 0x820870dbeb681120UL, },
        { 0x88872b6ae00bac56UL, 0xa10f1b1fe5a64d50UL, 0xa95e989976b0774dUL, },
        { 0x74275be8f967973aUL, 0xf66ac434778d2eabUL, 0x28fe40f5747f67b8UL, },
        { 0x5e6a4c685416e6faUL, 0x6b96003c62cbb84aUL, 0x294b031e47dbbce0UL, },
        { 0x9bbff273058b8406UL, 0xce3d931276d06ddbUL, 0x2ae3ffb014590a45UL, },
        { 0xcc672cce275a978aUL, 0x7bd79958623420d9UL, 0x2b57f8d00ee96641UL, },
        { 0x2854e5c8ae956fd2UL, 0x9fe4c7f1943afce6UL, 0xe7d2305ace5880e1UL, },
        { 0xac22b24301a97e28UL, 0xe0afda9df7b62519UL, 0xce9823d61aa33c2dUL, },
        { 0xcd821afb882f543cUL, 0x17de32b61fd5b402UL, 0x1f95b5e9143dc89UL, },
        { 0xdcc0a54266ee0334UL, 0x7a5267027beeef7eUL, 0x4ec0e47ff02fd554UL, },
        { 0x75018012ea521c00UL, 0x9327020053f7318UL, 0x20d2c6020bf6d42UL, },
        { 0x53640a6371bb350UL, 0xb7423d891417e3UL, 0x8255fecc0190cdb5UL, },
        { 0xf1ae4dfe42c551aUL, 0xb07ead6deed80a5dUL, 0xa913d4f23f1f384fUL, },
        { 0xc4c7f22045dcbdacUL, 0x5a7401739dd82f08UL, 0x83e1fdec064985f0UL, },
        { 0x91464e5d6c240384UL, 0xed869f6e7e610dfdUL, 0x4ed123841eeb9d21UL, },
        { 0x620e4336284a7334UL, 0x66b4294274f22e1eUL, 0x4e9d3b31bc0d9af0UL, },
        { 0x495648e19330d38cUL, 0xfb6882ca043abd36UL, 0x20c2cf670ca2472UL, },
        { 0xdbe9387f5235adf2UL, 0x8729d9a8138ad743UL, 0x67c7fa8459e8d87dUL, },
        { 0x8225280f84a3a3daUL, 0x7d607e75fb3a6c5fUL, 0xaaba7d92d6465460UL, },
        { 0x90746c107ddf87b0UL, 0xed29287c14df7bc0UL, 0x3a8acb6e0d60147UL, },
        { 0x5523ccf49c4c8a9eUL, 0xdb5e5fa39079cc89UL, 0xe7c2a77e86b180ccUL, },
        { 0x20d79b53c46044d2UL, 0x4114a4019535110dUL, 0xe5cf9e1b6dada400UL, },
        { 0x4b5a0cce2249cfdaUL, 0x6a214215ebdfafa9UL, 0xab1efdd2464b7155UL, },
        { 0xbc302be7e364b0d4UL, 0x8d2ae653f9231958UL, 0xccd812c58474a996UL, },
        { 0x7eb312c79c95e976UL, 0xd74a7a5673108650UL, 0x2af2281e7dd4bda6UL, },
        { 0x282643a7834c2aa4UL, 0x60b51047edec7398UL, 0xcf70bf736524a8d1UL, },
        { 0x44ce7e561248f83cUL, 0x979d9056012e5ee9UL, 0x21daa40ee80246aUL, },
        { 0xabeb3f66cb06c290UL, 0x71d43035889debc2UL, 0x80481091d36417aaUL, },
        { 0x814e45e137bbb1c0UL, 0xe4fe438810bdf8adUL, 0x3b83f93e54a01e1UL, },
        { 0x9048c0a44c01da4UL, 0x4da06c97e6929390UL, 0xcf3df318689ee7caUL, },
        { 0xa3a1f68db87d6ea2UL, 0x6775e0d5814d7f00UL, 0xe66776b0f41a5b2bUL, },
        { 0x824ef9338c9970b0UL, 0x7dc4332c0e6aa1d2UL, 0x5ccaddd7f44f1aUL, },
        { 0xeec9ca86e1676c82UL, 0x385cad0c176df8c8UL, 0x65cb872b9fa96f3aUL, },
        { 0x9fad1c75216303e0UL, 0xce04099186d5093cUL, 0x820870dbeb681120UL, },
        { 0x56UL, 0xffffffffffffffffUL, 0xaaaaaaaaaaaaaaaaUL, },
        { 0x88872b6ee00bac56UL, 0xa10f1b1fe5a64d50UL, 0xa95e989976b0774dUL, },
        { 0xad1172dc15d07fdaUL, 0x1ff5fed5ead0a447UL, 0xa9035393b1aa55adUL, },
        { 0x74275be0f967973aUL, 0xf66ac434778d2eabUL, 0x28fe40f5747f67b8UL, },
        { 0x7164d6f3c0046f62UL, 0xf658a49d91b8f1f5UL, 0xe7c3f7f074c17e1bUL, },
        { 0x5e6a4c785416e6faUL, 0x6b96003c62cbb84aUL, 0x294b031e47dbbce0UL, },
        { 0x918438ded136575eUL, 0x81287f4b8a948dc5UL, 0xe62a7ec680d4ea5cUL, },
        { 0x9bbff253058b8406UL, 0xce3d931276d06ddbUL, 0x2ae3ffb014590a45UL, },
        { 0xbf78c8d4eddb2834UL, 0x70cc8cf26f963f76UL, 0x4d680cd42ccc2a66UL, },
        { 0xcc672c8e275a978aUL, 0x7bd79958623420d9UL, 0x2b57f8d00ee96641UL, },
        { 0xd3bae97d8b65276cUL, 0x3556140b88496493UL, 0x8048418964b6a048UL, },
        { 0x2854e548ae956fd2UL, 0x9fe4c7f1943afce6UL, 0xe7d2305ace5880e1UL, },
        { 0xcbfc9a80a1c83642UL, 0xea87e5f4185a516cUL, 0x678af2b3383e4dc9UL, },
        { 0xac22b34301a97e28UL, 0xe0afda9df7b62519UL, 0xce9823d61aa33c2dUL, },
        { 0x6fc05639c388576UL, 0x91ff5f3666d102cfUL, 0x295b857bc8f99827UL, },
        { 0xcd8218fb882f543cUL, 0x17de32b61fd5b402UL, 0x1f95b5e9143dc89UL, },
        { 0x378920bb02aadf62UL, 0x2e9ab55d81b8475bUL, 0xe66723a90eadec02UL, },
        { 0xdcc0a14266ee0334UL, 0x7a5267027beeef7eUL, 0x4ec0e47ff02fd554UL, },
        { 0x98143b872fdbd1cUL, 0x21272e1f837ec2c5UL, 0x8219b330099dcb11UL, },
        { 0x75018812ea521c00UL, 0x9327020053f7318UL, 0x20d2c6020bf6d42UL, },
        { 0xefdb85b5f5b50334UL, 0xc781ac026b14de94UL, 0x4f64656a35a90eceUL, },
        { 0x53650a6371bb350UL, 0xb7423d891417e3UL, 0x8255fecc0190cdb5UL, },
        { 0xc16150bf0c78858UL, 0x4d923ee9ea26210aUL, 0xcf6560e9682a8d09UL, },
        { 0xf1ac4dfe42c551aUL, 0xb07ead6deed80a5dUL, 0xa913d4f23f1f384fUL, },
        { 0xd511834eb91ddf2UL, 0x4db2c7e8177ffac4UL, 0x67d728bc68a722aaUL, },
        { 0xc4c7b22045dcbdacUL, 0x5a7401739dd82f08UL, 0x83e1fdec064985f0UL, },
        { 0xe8e1577dc695946aUL, 0x3ad9eec9e5653ed4UL, 0xab42660763385bf1UL, },
        { 0x9146ce5d6c240384UL, 0xed869f6e7e610dfdUL, 0x4ed123841eeb9d21UL, },
        { 0x721dda92da97f928UL, 0xb31158dfa67125fUL, 0xccdd0a2222da0f4bUL, },
        { 0x620f4336284a7334UL, 0x66b4294274f22e1eUL, 0x4e9d3b31bc0d9af0UL, },
        { 0x3c640f5e7d588782UL, 0xd22297c16399654UL, 0x67d6382ffbd62610UL, },
        { 0x495448e19330d38cUL, 0xfb6882ca043abd36UL, 0x20c2cf670ca2472UL, },
        { 0xcc04fdf32f694520UL, 0x7b73d4499764ed54UL, 0x81b14f9f0f5b7d3bUL, },
        { 0xdbed387f5235adf2UL, 0x8729d9a8138ad743UL, 0x67c7fa8459e8d87dUL, },
        { 0x439d9ab9ffa10e1aUL, 0x27a3aaddeff651b4UL, 0xab0e7eb2d18e1c25UL, },
        { 0x822d280f84a3a3daUL, 0x7d607e75fb3a6c5fUL, 0xaaba7d92d6465460UL, },
        { 0x495548f19234c1c0UL, 0xfb6882c8043abc36UL, 0x20c2cf670cb2472UL, },
        { 0x90646c107ddf87b0UL, 0xed29287c14df7bc0UL, 0x3a8acb6e0d60147UL, },
        { 0xe5c6ea9a22cca2aaUL, 0x77c79851e4c36ac9UL, 0xab43229c0a793ef1UL, },
        { 0x5503ccf49c4c8a9eUL, 0xdb5e5fa39079cc89UL, 0xe7c2a77e86b180ccUL, },
        { 0x8bd9b217f801576cUL, 0x5cc2724b8dd2656dUL, 0x80598762def07a64UL, },
        { 0x20979b53c46044d2UL, 0x4114a4019535110dUL, 0xe5cf9e1b6dada400UL, },
        { 0x933ec0d367b87056UL, 0xef1343bffe1c7aa9UL, 0xa8b750b3e2671df1UL, },
        { 0x4bda0cce2249cfdaUL, 0x6a214215ebdfafa9UL, 0xab1efdd2464b7155UL, },
        { 0xbb1eb82dc6d42b82UL, 0x8f8d7c9c16b5b45bUL, 0x65cad6e47857b53fUL, },
        { 0xbd302be7e364b0d4UL, 0x8d2ae653f9231958UL, 0xccd812c58474a996UL, },
        { 0x6f482611c55589aaUL, 0x2b2feda1ef356fb3UL, 0xa912802cd457709cUL, },
        { 0x7cb312c79c95e976UL, 0xd74a7a5673108650UL, 0x2af2281e7dd4bda6UL, },
        { 0x470b9f9a6c4ad36aUL, 0xb4ea2959ef60f37bUL, 0xab0e3faee50de2c2UL, },
        { 0x2c2643a7834c2aa4UL, 0x60b51047edec7398UL, 0xcf70bf736524a8d1UL, },
        { 0xe5a12d1728977fe8UL, 0x889fd7d5f3ff36b3UL, 0xcc941a25a1ca51d1UL, },
        { 0x4cce7e561248f83cUL, 0x979d9056012e5ee9UL, 0x21daa40ee80246aUL, },
        { 0x62ac33bf9dfe4782UL, 0xa98e8fc0732c062UL, 0x646f4227221e02edUL, },
        { 0xbbeb3f66cb06c290UL, 0x71d43035889debc2UL, 0x80481091d36417aaUL, },
        { 0xb70b6ea737dc60eeUL, 0xc22af5f78f7f468cUL, 0xe42703be12ac30fcUL, },
        { 0xa14e45e137bbb1c0UL, 0xe4fe438810bdf8adUL, 0x3b83f93e54a01e1UL, },
        { 0x56f3f99b32f68a1cUL, 0x4ae143cf97745f51UL, 0x81b14e9f4ad87d22UL, },
        { 0x49048c0a44c01da4UL, 0x4da06c97e6929390UL, 0xcf3df318689ee7caUL, },
        { 0x29cac811bb657c8aUL, 0x9ea01d2873323e7bUL, 0x28ee8289c769f42bUL, },
        { 0x23a1f68db87d6ea2UL, 0x6775e0d5814d7f00UL, 0xe66776b0f41a5b2bUL, },
        { 0x85faf16865f47c6cUL, 0xc8e441bb8365e905UL, 0x80051ce6fadaa656UL, },
        { 0x824ef9338c9970b0UL, 0x7dc4332c0e6aa1d3UL, 0x5ccaddd7f44f1aUL, },
        { 0x86ae0fc5105ef8beUL, 0x11dc923a16183656UL, 0x67d63c2eb7a62699UL, },
        { 0xeec9ca86e1676c82UL, 0x385cad0c176df8caUL, 0x65cb872b9fa96f3aUL, },
        { 0x88932816e61039ceUL, 0x5ef2749e0cb70387UL, 0x663ef59223d30503UL, },
        { 0x9fad1c75216303e0UL, 0xce04099186d50938UL, 0x820870dbeb681120UL, },
        { 0xee5c136a5332beceUL, 0x54d9d98e1da370bbUL, 0x65878ecffe85de65UL, },
        { 0x56UL, 0xfffffffffffffff7UL, 0xaaaaaaaaaaaaaaaaUL, },
        { 0x460b8f337c099aa4UL, 0xb4ebd687f8a84a56UL, 0xccd9520fe5583a94UL, },
        { 0x88872b6ee00bac56UL, 0xa10f1b1fe5a64d40UL, 0xa95e989976b0774dUL, },
        { 0x2add8c091cac0032UL, 0xe84de000fa53d2dUL, 0x663bfc6107eb6ad6UL, },
        { 0xad1172dc15d07fdaUL, 0x1ff5fed5ead0a467UL, 0xa9035393b1aa55adUL, },
        { 0x770d0a7cee69380eUL, 0xf4dd70d6025a84bbUL, 0x647ed1c177befdc8UL, },
        { 0x74275be0f967973aUL, 0xf66ac434778d2eebUL, 0x28fe40f5747f67b8UL, },
        { 0xfae861953739b2c6UL, 0xaa9a428a724649c5UL, 0x2af3398daa279905UL, },
        { 0x7164d6f3c0046f62UL, 0xf658a49d91b8f175UL, 0xe7c3f7f074c17e1bUL, },
        { 0xc2c9cd4061588782UL, 0xa7a0097c1f026008UL, 0x679f6c2551b294bfUL, },
        { 0x5e6a4c785416e6faUL, 0x6b96003c62cbb94aUL, 0x294b031e47dbbce0UL, },
        { 0xbb7b67f7cca099f4UL, 0x70f5177a6417ede8UL, 0x4f39bb712c02bfc5UL, },
        { 0x918438ded136575eUL, 0x81287f4b8a948fc5UL, 0xe62a7ec680d4ea5cUL, },
        { 0xc822cc3d0cf095fcUL, 0x8432977e15c3d100UL, 0x3a9ad61a4346ca4UL, },
        { 0x9bbff253058b8406UL, 0xce3d931276d069dbUL, 0x2ae3ffb014590a45UL, },
        { 0x9105c3a16578108aUL, 0xeda7644867abd75aUL, 0x2b463e2be19b2e34UL, },
        { 0xbf78c8d4eddb2834UL, 0x70cc8cf26f963776UL, 0x4d680cd42ccc2a66UL, },
        { 0xa855b2132f6e2b64UL, 0xe0190f0ff7b63419UL, 0xce9823d61b223c2dUL, },
        { 0xcc672c8e275a978aUL, 0x7bd79958623430d9UL, 0x2b57f8d00ee96641UL, },
        { 0xa095a05d444f7056UL, 0xc13fb6bffee3403fUL, 0xaaabab7ded51c749UL, },
        { 0xd3bae97d8b65276cUL, 0x3556140b88494493UL, 0x8048418964b6a048UL, },
        { 0xbc43ba9effd501a4UL, 0x727155b7fdb34b32UL, 0xced47a7fd093c506UL, },
        { 0x2854e548ae956fd2UL, 0x9fe4c7f1943abce6UL, 0xe7d2305ace5880e1UL, },
        { 0x38173fb551d3dec6UL, 0xf263feea7743023aUL, 0x2ae2be3a50aabd90UL, },
        { 0xcbfc9a80a1c83642UL, 0xea87e5f4185ad16cUL, 0x678af2b3383e4dc9UL, },
        { 0xca49edbc0222dcb0UL, 0x868c4bcc11e1476dUL, 0x3b92e44a7b16c02UL, },
        { 0xac22b34301a97e28UL, 0xe0afda9df7b72519UL, 0xce9823d61aa33c2dUL, },
        { 0xd32a72ec3ab8569eUL, 0xa62e0e0392aacf58UL, 0xe7c6fe0150f911cfUL, },
        { 0x6fc05639c388576UL, 0x91ff5f3666d302cfUL, 0x295b857bc8f99827UL, },
        { 0x3464c5c8ab3b8462UL, 0x408170ed90b7b124UL, 0xe5de59b993657e76UL, },
        { 0xcd8218fb882f543cUL, 0x17de32b61fd1b402UL, 0x1f95b5e9143dc89UL, },
        { 0x154044b7ce7e491aUL, 0x6db75d4dfb10e3dfUL, 0xaaba79f69e697018UL, },
        { 0x378920bb02aadf62UL, 0x2e9ab55d81b0475bUL, 0xe66723a90eadec02UL, },
        { 0x7cc2946a90321e90UL, 0xd7c9c9958bdaff6dUL, 0x8251a66083337870UL, },
        { 0xdcc0a14266ee0334UL, 0x7a5267027bfeef7eUL, 0x4ec0e47ff02fd554UL, },
        { 0x628ca6ed9c0236f2UL, 0xa18cd9817647721UL, 0x65cb856adde84bf4UL, },
        { 0x98143b872fdbd1cUL, 0x21272e1f835ec2c5UL, 0x8219b330099dcb11UL, },
        { 0xe35d69fcb43c4a1cUL, 0x1961e44f8d57318cUL, 0x824561e869adcd9aUL, },
        { 0x75018812ea521c00UL, 0x9327020057f7318UL, 0x20d2c6020bf6d42UL, },
        { 0x9487c447e09d1f84UL, 0x7eed884e736a7c1fUL, 0x4e88d5ad2a84f460UL, },
        { 0xefdb85b5f5b50334UL, 0xc781ac026b94de94UL, 0x4f64656a35a90eceUL, },
        { 0xb8d780c86c3dddc0UL, 0xe11329e8051c50c7UL, 0x1106d71b456dfeUL, },
        { 0x53650a6371bb350UL, 0xb7423d881417e3UL, 0x8255fecc0190cdb5UL, },
        { 0x9d967ebd232e63daUL, 0xccb8d0f5fed170c8UL, 0xa8b502f61737712cUL, },
        { 0xc16150bf0c78858UL, 0x4d923ee9e826210aUL, 0xcf6560e9682a8d09UL, },
        { 0x30cd1d414ebdd42UL, 0xfdec938402fe2b57UL, 0x6476b41b5735b546UL, },
        { 0xf1ac4dfe42c551aUL, 0xb07ead6dead80a5dUL, 0xa913d4f23f1f384fUL, },
        { 0x24bdceb0fbd88daUL, 0xfdcc6a85f5a7f099UL, 0xaac4fc4e57b81ae5UL, },
        { 0xd511834eb91ddf2UL, 0x4db2c7e81f7ffac4UL, 0x67d728bc68a722aaUL, },
        { 0x50718930a12ee57eUL, 0xdbe9ad52133b6929UL, 0x6764613f78c4b460UL, },
        { 0xc4c7b22045dcbdacUL, 0x5a7401738dd82f08UL, 0x83e1fdec064985f0UL, },
        { 0xbd602b00ef6d2a42UL, 0x8d2b70d476dc1c21UL, 0x7672b86f7b9a4320UL, },
        { 0xe8e1577dc695946aUL, 0x3ad9eec9c5653ed4UL, 0xab42660763385bf1UL, },
        { 0xe8e1577dc6959458UL, 0x3ad9eec9c5653ed4UL, 0xcb42660763385bf1UL, },
        { 0x9146ce5d6c240384UL, 0xed869f6e3e610dfdUL, 0x4ed123841eeb9d21UL, },
        { 0x71067459c9f61178UL, 0xf6f6850044f98231UL, 0x4010d268751248cbUL, },
        { 0x721dda92da97f928UL, 0xb31158d7a67125fUL, 0xccdd0a2222da0f4bUL, },
        { 0x0UL, 0x0UL, 0x0UL, },
    };
    const mpfq_2_192_elt * ptr = t;
    unsigned int i,j;
    memset(r, 0, sizeof(mpfq_2_192_elt));
    for(i = 0 ; i < 3 ; i++) {
        mp_limb_t a = s[i];
        for(j = 0 ; j < 64 && ptr != &t[192]; j++, ptr++) {
            if (a & 1UL) {
                mpfq_2_192_add(K, r, r, *ptr);
            }
            a >>= 1;
        }
    }
}

static inline
unsigned long mpfq_2_192_trace(mpfq_2_192_dst_field K MAYBE_UNUSED, mpfq_2_192_src_elt s)
{
    return ((s[2]>>63) ^ (s[2]>>57)) & 1;
}

static inline
void mpfq_2_192_elt_ur_set(mpfq_2_192_dst_field K MAYBE_UNUSED, mpfq_2_192_dst_elt_ur r, mpfq_2_192_src_elt_ur s)
{
    memcpy(r, s, sizeof(mpfq_2_192_elt_ur));
}

static inline
void mpfq_2_192_elt_ur_add(mpfq_2_192_dst_field K MAYBE_UNUSED, mpfq_2_192_dst_elt_ur r, mpfq_2_192_src_elt_ur s1, mpfq_2_192_src_elt_ur s2)
{
    int i;
    for(i = 0 ; i < 6 ; i++)
        r[i] = s1[i] ^ s2[i];
}

static inline
void mpfq_2_192_mul_ur(mpfq_2_192_dst_field K MAYBE_UNUSED, mpfq_2_192_dst_elt_ur t, mpfq_2_192_src_elt s1, mpfq_2_192_src_elt s2)
{
    /* 192x192 schoolbook split=128 w=64 */
    memset(t, 0, 6 * sizeof(unsigned long));
    mpfq_2_192_addmul_192x128(t+0,s1,s2+0);
    mpfq_2_192_addmul_192x64(t+2,s1,s2+2);
}

static inline
void mpfq_2_192_addmul_192x128(unsigned long * t, const unsigned long * s1, const unsigned long * s2)
{
    /* 192x128 basecase sse2=64 w=64 slicenet=sequence add=1 slice=4 */
    typedef union { __v2di s;
    unsigned long x[2];
     } __v2di_proxy;
#define SHL(x, r) _mm_slli_epi64((x), (r))
#define SHR(x, r) _mm_srli_epi64((x), (r))
#define SHLD(x, r) _mm_slli_si128((x), (r) >> 3)
#define SHRD(x, r) _mm_srli_si128((x), (r) >> 3)
    __v2di u;
    __v2di t0;
    __v2di t1;
    __v2di t2;
    __v2di t3;
    
    __v2di g[16];
    /* sequence update walk */
    g[0] = (__v2di) { 0, };
    g[1] = (__v2di) { s2[0], s2[1], };
    g[2] = SHL(g[1], 1);
    g[3] = g[2] ^ g[1];
    g[4] = SHL(g[2], 1);
    g[5] = g[4] ^ g[1];
    g[6] = SHL(g[3], 1);
    g[7] = g[6] ^ g[1];
    g[8] = SHL(g[4], 1);
    g[9] = g[8] ^ g[1];
    g[10] = SHL(g[5], 1);
    g[11] = g[10] ^ g[1];
    g[12] = SHL(g[6], 1);
    g[13] = g[12] ^ g[1];
    g[14] = SHL(g[7], 1);
    g[15] = g[14] ^ g[1];
    
    /* round 0 */
    u = g[s1[0]       & 15];
    t0  = u;
    u = g[s1[0] >>  4 & 15];
    t0 ^= SHL(u,  4); t1  = SHR(u, 60);
    u = g[s1[0] >>  8 & 15];
    t0 ^= SHL(u,  8); t1 ^= SHR(u, 56);
    u = g[s1[0] >> 12 & 15];
    t0 ^= SHL(u, 12); t1 ^= SHR(u, 52);
    u = g[s1[0] >> 16 & 15];
    t0 ^= SHL(u, 16); t1 ^= SHR(u, 48);
    u = g[s1[0] >> 20 & 15];
    t0 ^= SHL(u, 20); t1 ^= SHR(u, 44);
    u = g[s1[0] >> 24 & 15];
    t0 ^= SHL(u, 24); t1 ^= SHR(u, 40);
    u = g[s1[0] >> 28 & 15];
    t0 ^= SHL(u, 28); t1 ^= SHR(u, 36);
    u = g[s1[0] >> 32 & 15];
    t0 ^= SHL(u, 32); t1 ^= SHR(u, 32);
    u = g[s1[0] >> 36 & 15];
    t0 ^= SHL(u, 36); t1 ^= SHR(u, 28);
    u = g[s1[0] >> 40 & 15];
    t0 ^= SHL(u, 40); t1 ^= SHR(u, 24);
    u = g[s1[0] >> 44 & 15];
    t0 ^= SHL(u, 44); t1 ^= SHR(u, 20);
    u = g[s1[0] >> 48 & 15];
    t0 ^= SHL(u, 48); t1 ^= SHR(u, 16);
    u = g[s1[0] >> 52 & 15];
    t0 ^= SHL(u, 52); t1 ^= SHR(u, 12);
    u = g[s1[0] >> 56 & 15];
    t0 ^= SHL(u, 56); t1 ^= SHR(u,  8);
    u = g[s1[0] >> 60 & 15];
    t0 ^= SHL(u, 60); t1 ^= SHR(u,  4);
    
    /* round 1 */
    u = g[s1[1]       & 15];
    t1 ^= u;
    u = g[s1[1] >>  4 & 15];
    t1 ^= SHL(u,  4); t2  = SHR(u, 60);
    u = g[s1[1] >>  8 & 15];
    t1 ^= SHL(u,  8); t2 ^= SHR(u, 56);
    u = g[s1[1] >> 12 & 15];
    t1 ^= SHL(u, 12); t2 ^= SHR(u, 52);
    u = g[s1[1] >> 16 & 15];
    t1 ^= SHL(u, 16); t2 ^= SHR(u, 48);
    u = g[s1[1] >> 20 & 15];
    t1 ^= SHL(u, 20); t2 ^= SHR(u, 44);
    u = g[s1[1] >> 24 & 15];
    t1 ^= SHL(u, 24); t2 ^= SHR(u, 40);
    u = g[s1[1] >> 28 & 15];
    t1 ^= SHL(u, 28); t2 ^= SHR(u, 36);
    u = g[s1[1] >> 32 & 15];
    t1 ^= SHL(u, 32); t2 ^= SHR(u, 32);
    u = g[s1[1] >> 36 & 15];
    t1 ^= SHL(u, 36); t2 ^= SHR(u, 28);
    u = g[s1[1] >> 40 & 15];
    t1 ^= SHL(u, 40); t2 ^= SHR(u, 24);
    u = g[s1[1] >> 44 & 15];
    t1 ^= SHL(u, 44); t2 ^= SHR(u, 20);
    u = g[s1[1] >> 48 & 15];
    t1 ^= SHL(u, 48); t2 ^= SHR(u, 16);
    u = g[s1[1] >> 52 & 15];
    t1 ^= SHL(u, 52); t2 ^= SHR(u, 12);
    u = g[s1[1] >> 56 & 15];
    t1 ^= SHL(u, 56); t2 ^= SHR(u,  8);
    u = g[s1[1] >> 60 & 15];
    t1 ^= SHL(u, 60); t2 ^= SHR(u,  4);
    
    /* round 2 */
    u = g[s1[2]       & 15];
    t2 ^= u;
    u = g[s1[2] >>  4 & 15];
    t2 ^= SHL(u,  4); t3  = SHR(u, 60);
    u = g[s1[2] >>  8 & 15];
    t2 ^= SHL(u,  8); t3 ^= SHR(u, 56);
    u = g[s1[2] >> 12 & 15];
    t2 ^= SHL(u, 12); t3 ^= SHR(u, 52);
    u = g[s1[2] >> 16 & 15];
    t2 ^= SHL(u, 16); t3 ^= SHR(u, 48);
    u = g[s1[2] >> 20 & 15];
    t2 ^= SHL(u, 20); t3 ^= SHR(u, 44);
    u = g[s1[2] >> 24 & 15];
    t2 ^= SHL(u, 24); t3 ^= SHR(u, 40);
    u = g[s1[2] >> 28 & 15];
    t2 ^= SHL(u, 28); t3 ^= SHR(u, 36);
    u = g[s1[2] >> 32 & 15];
    t2 ^= SHL(u, 32); t3 ^= SHR(u, 32);
    u = g[s1[2] >> 36 & 15];
    t2 ^= SHL(u, 36); t3 ^= SHR(u, 28);
    u = g[s1[2] >> 40 & 15];
    t2 ^= SHL(u, 40); t3 ^= SHR(u, 24);
    u = g[s1[2] >> 44 & 15];
    t2 ^= SHL(u, 44); t3 ^= SHR(u, 20);
    u = g[s1[2] >> 48 & 15];
    t2 ^= SHL(u, 48); t3 ^= SHR(u, 16);
    u = g[s1[2] >> 52 & 15];
    t2 ^= SHL(u, 52); t3 ^= SHR(u, 12);
    u = g[s1[2] >> 56 & 15];
    t2 ^= SHL(u, 56); t3 ^= SHR(u,  8);
    u = g[s1[2] >> 60 & 15];
    t2 ^= SHL(u, 60); t3 ^= SHR(u,  4);
    /* end */
    
    /* repair steps */
    /* repair section 200711-200803 */
    __v2di v1 = (__v2di) { s1[0] >> 1, s1[0] >> 1, };
    __v2di v2 = (__v2di) { s1[1] >> 1, s1[1] >> 1, };
    __v2di v3 = (__v2di) { s1[2] >> 1, s1[2] >> 1, };
    __v2di w;
    __v2di m = (__v2di) { 0x7777777777777777UL, 0x7777777777777777UL, };
    w = -SHR(g[1],63);
    v1 = v1 & m;
    t1 ^= v1 & w;
    v2 = v2 & m;
    t2 ^= v2 & w;
    v3 = v3 & m;
    t3 ^= v3 & w;
    w = -SHR(g[2],63);
    v1 = SHR(v1, 1) & m;
    t1 ^= v1 & w;
    v2 = SHR(v2, 1) & m;
    t2 ^= v2 & w;
    v3 = SHR(v3, 1) & m;
    t3 ^= v3 & w;
    w = -SHR(g[4],63);
    v1 = SHR(v1, 1) & m;
    t1 ^= v1 & w;
    v2 = SHR(v2, 1) & m;
    t2 ^= v2 & w;
    v3 = SHR(v3, 1) & m;
    t3 ^= v3 & w;
    
    /* store result */
    {
        {
        __v2di_proxy r;
        r.s = t0 ^ SHLD(t1, 64);
        t[0] ^= r.x[0];
        t[1] ^= r.x[1];
    }
        {
        __v2di_proxy r;
        r.s = t2 ^ SHRD(t1, 64) ^ SHLD(t3, 64);
        t[2] ^= r.x[0];
        t[3] ^= r.x[1];
    }
        {
        __v2di_proxy r;
        r.s = SHRD(t3, 64);
        t[4] ^= r.x[0];
    }
    }
#undef SHL
#undef SHR
#undef SHLD
#undef SHRD
}

static inline
void mpfq_2_192_addmul_192x64(unsigned long * t, const unsigned long * s1, const unsigned long * s2)
{
    /* 192x64 basecase w=64 sse2=64 slice=4 add=1 slicenet=sequence */
    typedef union { __v2di s;
    unsigned long x[2];
     } __v2di_proxy;
#define SHL(x, r) _mm_slli_epi64((x), (r))
#define SHR(x, r) _mm_srli_epi64((x), (r))
#define SHLD(x, r) _mm_slli_si128((x), (r) >> 3)
#define SHRD(x, r) _mm_srli_si128((x), (r) >> 3)
    __v2di u;
    __v2di t0;
    __v2di t1;
    __v2di t2;
    __v2di t3;
    
    __v2di g[16];
    /* sequence update walk */
    g[0] = (__v2di) { 0, };
    g[1] = (__v2di) { s2[0], };
    g[2] = SHL(g[1], 1);
    g[3] = g[2] ^ g[1];
    g[4] = SHL(g[2], 1);
    g[5] = g[4] ^ g[1];
    g[6] = SHL(g[3], 1);
    g[7] = g[6] ^ g[1];
    g[8] = SHL(g[4], 1);
    g[9] = g[8] ^ g[1];
    g[10] = SHL(g[5], 1);
    g[11] = g[10] ^ g[1];
    g[12] = SHL(g[6], 1);
    g[13] = g[12] ^ g[1];
    g[14] = SHL(g[7], 1);
    g[15] = g[14] ^ g[1];
    
    /* round 0 */
    u = g[s1[0]       & 15];
    t0  = u;
    u = g[s1[0] >>  4 & 15];
    t0 ^= SHL(u,  4); t1  = SHR(u, 60);
    u = g[s1[0] >>  8 & 15];
    t0 ^= SHL(u,  8); t1 ^= SHR(u, 56);
    u = g[s1[0] >> 12 & 15];
    t0 ^= SHL(u, 12); t1 ^= SHR(u, 52);
    u = g[s1[0] >> 16 & 15];
    t0 ^= SHL(u, 16); t1 ^= SHR(u, 48);
    u = g[s1[0] >> 20 & 15];
    t0 ^= SHL(u, 20); t1 ^= SHR(u, 44);
    u = g[s1[0] >> 24 & 15];
    t0 ^= SHL(u, 24); t1 ^= SHR(u, 40);
    u = g[s1[0] >> 28 & 15];
    t0 ^= SHL(u, 28); t1 ^= SHR(u, 36);
    u = g[s1[0] >> 32 & 15];
    t0 ^= SHL(u, 32); t1 ^= SHR(u, 32);
    u = g[s1[0] >> 36 & 15];
    t0 ^= SHL(u, 36); t1 ^= SHR(u, 28);
    u = g[s1[0] >> 40 & 15];
    t0 ^= SHL(u, 40); t1 ^= SHR(u, 24);
    u = g[s1[0] >> 44 & 15];
    t0 ^= SHL(u, 44); t1 ^= SHR(u, 20);
    u = g[s1[0] >> 48 & 15];
    t0 ^= SHL(u, 48); t1 ^= SHR(u, 16);
    u = g[s1[0] >> 52 & 15];
    t0 ^= SHL(u, 52); t1 ^= SHR(u, 12);
    u = g[s1[0] >> 56 & 15];
    t0 ^= SHL(u, 56); t1 ^= SHR(u,  8);
    u = g[s1[0] >> 60 & 15];
    t0 ^= SHL(u, 60); t1 ^= SHR(u,  4);
    
    /* round 1 */
    u = g[s1[1]       & 15];
    t1 ^= u;
    u = g[s1[1] >>  4 & 15];
    t1 ^= SHL(u,  4); t2  = SHR(u, 60);
    u = g[s1[1] >>  8 & 15];
    t1 ^= SHL(u,  8); t2 ^= SHR(u, 56);
    u = g[s1[1] >> 12 & 15];
    t1 ^= SHL(u, 12); t2 ^= SHR(u, 52);
    u = g[s1[1] >> 16 & 15];
    t1 ^= SHL(u, 16); t2 ^= SHR(u, 48);
    u = g[s1[1] >> 20 & 15];
    t1 ^= SHL(u, 20); t2 ^= SHR(u, 44);
    u = g[s1[1] >> 24 & 15];
    t1 ^= SHL(u, 24); t2 ^= SHR(u, 40);
    u = g[s1[1] >> 28 & 15];
    t1 ^= SHL(u, 28); t2 ^= SHR(u, 36);
    u = g[s1[1] >> 32 & 15];
    t1 ^= SHL(u, 32); t2 ^= SHR(u, 32);
    u = g[s1[1] >> 36 & 15];
    t1 ^= SHL(u, 36); t2 ^= SHR(u, 28);
    u = g[s1[1] >> 40 & 15];
    t1 ^= SHL(u, 40); t2 ^= SHR(u, 24);
    u = g[s1[1] >> 44 & 15];
    t1 ^= SHL(u, 44); t2 ^= SHR(u, 20);
    u = g[s1[1] >> 48 & 15];
    t1 ^= SHL(u, 48); t2 ^= SHR(u, 16);
    u = g[s1[1] >> 52 & 15];
    t1 ^= SHL(u, 52); t2 ^= SHR(u, 12);
    u = g[s1[1] >> 56 & 15];
    t1 ^= SHL(u, 56); t2 ^= SHR(u,  8);
    u = g[s1[1] >> 60 & 15];
    t1 ^= SHL(u, 60); t2 ^= SHR(u,  4);
    
    /* round 2 */
    u = g[s1[2]       & 15];
    t2 ^= u;
    u = g[s1[2] >>  4 & 15];
    t2 ^= SHL(u,  4); t3  = SHR(u, 60);
    u = g[s1[2] >>  8 & 15];
    t2 ^= SHL(u,  8); t3 ^= SHR(u, 56);
    u = g[s1[2] >> 12 & 15];
    t2 ^= SHL(u, 12); t3 ^= SHR(u, 52);
    u = g[s1[2] >> 16 & 15];
    t2 ^= SHL(u, 16); t3 ^= SHR(u, 48);
    u = g[s1[2] >> 20 & 15];
    t2 ^= SHL(u, 20); t3 ^= SHR(u, 44);
    u = g[s1[2] >> 24 & 15];
    t2 ^= SHL(u, 24); t3 ^= SHR(u, 40);
    u = g[s1[2] >> 28 & 15];
    t2 ^= SHL(u, 28); t3 ^= SHR(u, 36);
    u = g[s1[2] >> 32 & 15];
    t2 ^= SHL(u, 32); t3 ^= SHR(u, 32);
    u = g[s1[2] >> 36 & 15];
    t2 ^= SHL(u, 36); t3 ^= SHR(u, 28);
    u = g[s1[2] >> 40 & 15];
    t2 ^= SHL(u, 40); t3 ^= SHR(u, 24);
    u = g[s1[2] >> 44 & 15];
    t2 ^= SHL(u, 44); t3 ^= SHR(u, 20);
    u = g[s1[2] >> 48 & 15];
    t2 ^= SHL(u, 48); t3 ^= SHR(u, 16);
    u = g[s1[2] >> 52 & 15];
    t2 ^= SHL(u, 52); t3 ^= SHR(u, 12);
    u = g[s1[2] >> 56 & 15];
    t2 ^= SHL(u, 56); t3 ^= SHR(u,  8);
    u = g[s1[2] >> 60 & 15];
    t2 ^= SHL(u, 60); t3 ^= SHR(u,  4);
    /* end */
    
    /* repair steps */
    /* repair section 200711-200803 */
    __v2di v1 = (__v2di) { s1[0] >> 1, s1[0] >> 1, };
    __v2di v2 = (__v2di) { s1[1] >> 1, s1[1] >> 1, };
    __v2di v3 = (__v2di) { s1[2] >> 1, s1[2] >> 1, };
    __v2di w;
    __v2di m = (__v2di) { 0x7777777777777777UL, 0x7777777777777777UL, };
    w = -SHR(g[1],63);
    v1 = v1 & m;
    t1 ^= v1 & w;
    v2 = v2 & m;
    t2 ^= v2 & w;
    v3 = v3 & m;
    t3 ^= v3 & w;
    w = -SHR(g[2],63);
    v1 = SHR(v1, 1) & m;
    t1 ^= v1 & w;
    v2 = SHR(v2, 1) & m;
    t2 ^= v2 & w;
    v3 = SHR(v3, 1) & m;
    t3 ^= v3 & w;
    w = -SHR(g[4],63);
    v1 = SHR(v1, 1) & m;
    t1 ^= v1 & w;
    v2 = SHR(v2, 1) & m;
    t2 ^= v2 & w;
    v3 = SHR(v3, 1) & m;
    t3 ^= v3 & w;
    
    /* store result */
    {
        {
        __v2di_proxy r;
        r.s = t0 ^ SHLD(t1, 64);
        t[0] ^= r.x[0];
        t[1] ^= r.x[1];
    }
        {
        __v2di_proxy r;
        r.s = t2 ^ SHRD(t1, 64) ^ SHLD(t3, 64);
        t[2] ^= r.x[0];
        t[3] ^= r.x[1];
    }
    }
#undef SHL
#undef SHR
#undef SHLD
#undef SHRD
}

static inline
void mpfq_2_192_sqr_ur(mpfq_2_192_dst_field K MAYBE_UNUSED, mpfq_2_192_dst_elt_ur t, mpfq_2_192_src_elt s)
{
    static const mp_limb_t g[16] = {
        0, 1, 4, 5, 16, 17, 20, 21,
        64, 65, 68, 69, 80, 81, 84, 85,
    };
    {
        mp_limb_t u;
        u = g[s[0]       & 15];
    t[0]  = u;
        u = g[s[0] >>  4 & 15];
    t[0] ^= u <<  8;
        u = g[s[0] >>  8 & 15];
    t[0] ^= u << 16;
        u = g[s[0] >> 12 & 15];
    t[0] ^= u << 24;
        u = g[s[0] >> 16 & 15];
    t[0] ^= u << 32;
        u = g[s[0] >> 20 & 15];
    t[0] ^= u << 40;
        u = g[s[0] >> 24 & 15];
    t[0] ^= u << 48;
        u = g[s[0] >> 28 & 15];
    t[0] ^= u << 56;
        u = g[s[0] >> 32 & 15];
    t[1]  = u;
        u = g[s[0] >> 36 & 15];
    t[1] ^= u <<  8;
        u = g[s[0] >> 40 & 15];
    t[1] ^= u << 16;
        u = g[s[0] >> 44 & 15];
    t[1] ^= u << 24;
        u = g[s[0] >> 48 & 15];
    t[1] ^= u << 32;
        u = g[s[0] >> 52 & 15];
    t[1] ^= u << 40;
        u = g[s[0] >> 56 & 15];
    t[1] ^= u << 48;
        u = g[s[0] >> 60 & 15];
    t[1] ^= u << 56;
        u = g[s[1]       & 15];
    t[2]  = u;
        u = g[s[1] >>  4 & 15];
    t[2] ^= u <<  8;
        u = g[s[1] >>  8 & 15];
    t[2] ^= u << 16;
        u = g[s[1] >> 12 & 15];
    t[2] ^= u << 24;
        u = g[s[1] >> 16 & 15];
    t[2] ^= u << 32;
        u = g[s[1] >> 20 & 15];
    t[2] ^= u << 40;
        u = g[s[1] >> 24 & 15];
    t[2] ^= u << 48;
        u = g[s[1] >> 28 & 15];
    t[2] ^= u << 56;
        u = g[s[1] >> 32 & 15];
    t[3]  = u;
        u = g[s[1] >> 36 & 15];
    t[3] ^= u <<  8;
        u = g[s[1] >> 40 & 15];
    t[3] ^= u << 16;
        u = g[s[1] >> 44 & 15];
    t[3] ^= u << 24;
        u = g[s[1] >> 48 & 15];
    t[3] ^= u << 32;
        u = g[s[1] >> 52 & 15];
    t[3] ^= u << 40;
        u = g[s[1] >> 56 & 15];
    t[3] ^= u << 48;
        u = g[s[1] >> 60 & 15];
    t[3] ^= u << 56;
        u = g[s[2]       & 15];
    t[4]  = u;
        u = g[s[2] >>  4 & 15];
    t[4] ^= u <<  8;
        u = g[s[2] >>  8 & 15];
    t[4] ^= u << 16;
        u = g[s[2] >> 12 & 15];
    t[4] ^= u << 24;
        u = g[s[2] >> 16 & 15];
    t[4] ^= u << 32;
        u = g[s[2] >> 20 & 15];
    t[4] ^= u << 40;
        u = g[s[2] >> 24 & 15];
    t[4] ^= u << 48;
        u = g[s[2] >> 28 & 15];
    t[4] ^= u << 56;
        u = g[s[2] >> 32 & 15];
    t[5]  = u;
        u = g[s[2] >> 36 & 15];
    t[5] ^= u <<  8;
        u = g[s[2] >> 40 & 15];
    t[5] ^= u << 16;
        u = g[s[2] >> 44 & 15];
    t[5] ^= u << 24;
        u = g[s[2] >> 48 & 15];
    t[5] ^= u << 32;
        u = g[s[2] >> 52 & 15];
    t[5] ^= u << 40;
        u = g[s[2] >> 56 & 15];
    t[5] ^= u << 48;
        u = g[s[2] >> 60 & 15];
    t[5] ^= u << 56;
    }
}

static inline
void mpfq_2_192_reduce(mpfq_2_192_dst_field K MAYBE_UNUSED, mpfq_2_192_dst_elt r, mpfq_2_192_dst_elt_ur t)
{
    {
        mp_limb_t s[4];
        /* 191 excess bits */
        {
            unsigned long z;
            z = t[0];
            s[0] = z;
            z = t[1];
            s[1] = z;
            z = t[2];
            s[2] = z;
        }
        memset(s + 3, 0, 1 * sizeof(mp_limb_t));
        {
            unsigned long z;
            z = t[3];
            s[0]^= z <<  7;
            s[0]^= z <<  2;
            s[0]^= z <<  1;
            s[0]^= z;
            z >>= 57;
            z^= t[4] <<  7;
            s[1]^= z;
            z >>= 5;
            z^= t[4] >> 57 << 59;
            s[1]^= z;
            z >>= 1;
            z^= t[4] >> 62 << 63;
            s[1]^= z;
            z >>= 1;
            z^= (t[4] & ~0x7fffffffffffffffUL);
            s[1]^= z;
            z >>= 57;
            z^= t[5] <<  7;
            s[2]^= z;
            z >>= 5;
            z^= t[5] >> 57 << 59;
            s[2]^= z;
            z >>= 1;
            z^= t[5] >> 62 << 63;
            s[2]^= z;
            z >>= 1;
            s[2]^= z;
            z >>= 57;
            s[3]^= z;
            z >>= 5;
            s[3]^= z;
        }
        /* 6 excess bits */
        {
            unsigned long z;
            z = s[0];
            r[0] = z;
            z = s[1];
            r[1] = z;
            z = s[2];
            r[2] = z;
        }
        {
            unsigned long z;
            z = s[3];
            r[0]^= z <<  7;
            r[0]^= z <<  2;
            r[0]^= z <<  1;
            r[0]^= z;
        }
    }
}

static inline
int mpfq_2_192_cmp(mpfq_2_192_dst_field K MAYBE_UNUSED, mpfq_2_192_src_elt a, mpfq_2_192_src_elt b)
{
    return mpn_cmp(a, b, 3);
}

static inline
int mpfq_2_192_cmp_ui(mpfq_2_192_dst_field K MAYBE_UNUSED, mpfq_2_192_src_elt r, unsigned long x)
{
    int i;
    if (r[0] < (x & 1UL)) return -1;
    if (r[0] > (x & 1UL)) return 1;
    for(i = 1 ; i < 3 ; i++) {
        if (r[i]) return 1;
    }
    return 0;
}


#endif  /* MPFQ_2_192_H_ */

/* vim:set ft=cpp: */
