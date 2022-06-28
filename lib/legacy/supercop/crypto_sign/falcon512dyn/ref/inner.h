#ifndef FALCON_INNER_H__
#define FALCON_INNER_H__

/*
 * Internal functions for Falcon. This is not the API intended to be
 * used by applications; instead, this internal API provides all the
 * primitives on which wrappers build to provide external APIs.
 *
 * ==========================(LICENSE BEGIN)============================
 *
 * Copyright (c) 2017-2019  Falcon Project
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * ===========================(LICENSE END)=============================
 *
 * @author   Thomas Pornin <thomas.pornin@nccgroup.com>
 */

#define FALCON_FPEMU          1
#define FALCON_FPNATIVE       0
#define FALCON_ASM_CORTEXM4   0
#define FALCON_AVX2           0
#define FALCON_LE             0
#define FALCON_UNALIGNED      0

#include <stdint.h>
#include <stdlib.h>
#include <string.h>


/*
 * On MSVC, disable warning about applying unary minus on an unsigned
 * type: this is perfectly defined standard behaviour and we do it
 * quite often.
 */
#if defined _MSC_VER && _MSC_VER
#pragma warning( disable : 4146 )
#endif


#if defined __i386__ || defined _M_IX86 \
	|| defined __x86_64__ || defined _M_X64 || \
	(defined _ARCH_PWR8 && \
		(defined __LITTLE_ENDIAN || defined __LITTLE_ENDIAN__))

#ifndef FALCON_LE
#define FALCON_LE     1
#endif
#ifndef FALCON_UNALIGNED
#define FALCON_UNALIGNED   1
#endif

#elif defined FALCON_ASM_CORTEXM4 && FALCON_ASM_CORTEXM4

#ifndef FALCON_LE
#define FALCON_LE     1
#endif
#ifndef FALCON_UNALIGNED
#define FALCON_UNALIGNED   0
#endif

#elif (defined __LITTLE_ENDIAN__ && __LITTLE_ENDIAN__) \
	|| (defined __BYTE_ORDER__ && defined __ORDER_LITTLE_ENDIAN__ \
		&& __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)

#ifndef FALCON_LE
#define FALCON_LE     1
#endif
#ifndef FALCON_UNALIGNED
#define FALCON_UNALIGNED   0
#endif

#else

#ifndef FALCON_LE
#define FALCON_LE     0
#endif
#ifndef FALCON_UNALIGNED
#define FALCON_UNALIGNED   0
#endif

#endif

/*
 * We ensure that both FALCON_FPEMU and FALCON_FPNATIVE are defined,
 * with compatible values (exactly one of them must be non-zero).
 * If none is defined, then default FP implementation is 'native'
 * except on ARM Cortex M4.
 */
#if !defined FALCON_FPEMU && !defined FALCON_FPNATIVE

#if (defined __ARM_FP && ((__ARM_FP & 0x08) == 0x08)) \
	|| (!defined __ARM_FP && defined __ARM_VFPV2__)
#define FALCON_FPEMU      0
#define FALCON_FPNATIVE   1
#elif defined FALCON_ASM_CORTEXM4 && FALCON_ASM_CORTEXM4
#define FALCON_FPEMU      1
#define FALCON_FPNATIVE   0
#else
#define FALCON_FPEMU      0
#define FALCON_FPNATIVE   1
#endif

#elif defined FALCON_FPEMU && !defined FALCON_FPNATIVE

#if FALCON_FPEMU
#define FALCON_FPNATIVE   0
#else
#define FALCON_FPNATIVE   1
#endif

#elif defined FALCON_FPNATIVE && !defined FALCON_FPEMU

#if FALCON_FPNATIVE
#define FALCON_FPEMU   0
#else
#define FALCON_FPEMU   1
#endif

#endif

#if (FALCON_FPEMU && FALCON_FPNATIVE) || (!FALCON_FPEMU && !FALCON_FPNATIVE)
#error Exactly one of FALCON_FPEMU and FALCON_FPNATIVE must be selected
#endif


/*
 * For still undefined compile-time macros, define them to 0 to avoid
 * warnings with -Wundef.
 */
#ifndef FALCON_AVX2
#define FALCON_AVX2   0
#endif
#ifndef FALCON_FMA
#define FALCON_FMA   0
#endif
#ifndef FALCON_KG_CHACHA20
#define FALCON_KG_CHACHA20   0
#endif
#ifndef FALCON_CT_HASH
#define FALCON_CT_HASH   0
#endif



/*
 * Some computations with floating-point elements, in particular
 * rounding to the nearest integer, rely on operations using _exactly_
 * the precision of IEEE-754 binary64 type (i.e. 52 bits). On 32-bit
 * x86, the 387 FPU may be used (depending on the target OS) and, in
 * that case, may use more precision bits (i.e. 64 bits, for an 80-bit
 * total type length); to prevent miscomputations, we define an explicit
 * function that modifies the precision in the FPU control word.
 *
 * set_fpu_cw() sets the precision to the provided value, and returns
 * the previously set precision; callers are supposed to restore the
 * previous precision on exit. The correct (52-bit) precision is
 * configured with the value "2". On unsupported compilers, or on
 * targets other than 32-bit x86, or when the native 'double' type is
 * not used, the set_fpu_cw() function does nothing at all.
 */
static inline unsigned
set_fpu_cw(unsigned x)
{
	return x;
}



/*
 * MSVC 2015 does not know the C99 keyword 'restrict'.
 */
#if defined _MSC_VER && _MSC_VER
#ifndef restrict
#define restrict   __restrict
#endif
#endif

/* ==================================================================== */
/*
 * SHAKE256 implementation (shake.c).
 *
 * API is defined to be easily replaced with the fips202.h API defined
 * as part of PQ Clean.
 */

typedef struct {
	union {
		uint64_t A[25];
		uint8_t dbuf[200];
	} st;
	uint64_t dptr;
} shake256_context;

#define shake256_init      falcon512dyn_ref_i_shake256_init
#define shake256_inject    falcon512dyn_ref_i_shake256_inject
#define shake256_flip      falcon512dyn_ref_i_shake256_flip
#define shake256_extract   falcon512dyn_ref_i_shake256_extract

void falcon512dyn_ref_i_shake256_init(shake256_context *sc);
void falcon512dyn_ref_i_shake256_inject(shake256_context *sc, const uint8_t *in, size_t len);
void falcon512dyn_ref_i_shake256_flip(shake256_context *sc);
void falcon512dyn_ref_i_shake256_extract(shake256_context *sc, uint8_t *out, size_t len);

/*
 */

/* ==================================================================== */
/*
 * Encoding/decoding functions (codec.c).
 *
 * Encoding functions take as parameters an output buffer (out) with
 * a given maximum length (max_out_len); returned value is the actual
 * number of bytes which have been written. If the output buffer is
 * not large enough, then 0 is returned (some bytes may have been
 * written to the buffer). If 'out' is NULL, then 'max_out_len' is
 * ignored; instead, the function computes and returns the actual
 * required output length (in bytes).
 *
 * Decoding functions take as parameters an input buffer (in) with
 * its maximum length (max_in_len); returned value is the actual number
 * of bytes that have been read from the buffer. If the provided length
 * is too short, then 0 is returned.
 *
 * Values to encode or decode are vectors of integers, with N = 2^logn
 * elements.
 *
 * Three encoding formats are defined:
 *
 *   - modq: sequence of values modulo 12289, each encoded over exactly
 *     14 bits. The encoder and decoder verify that integers are within
 *     the valid range (0..12288). Values are arrays of uint16.
 *
 *   - trim: sequence of signed integers, a specified number of bits
 *     each. The number of bits is provided as parameter and includes
 *     the sign bit. Each integer x must be such that |x| < 2^(bits-1)
 *     (which means that the -2^(bits-1) value is forbidden); encode and
 *     decode functions check that property. Values are arrays of
 *     int16_t or int8_t, corresponding to names 'trim_i16' and
 *     'trim_i8', respectively.
 *
 *   - comp: variable-length encoding for signed integers; each integer
 *     uses a minimum of 9 bits, possibly more. This is normally used
 *     only for signatures.
 *
 */

size_t falcon512dyn_ref_modq_encode(void *out, size_t max_out_len,
	const uint16_t *x, unsigned logn);
size_t falcon512dyn_ref_trim_i16_encode(void *out, size_t max_out_len,
	const int16_t *x, unsigned logn, unsigned bits);
size_t falcon512dyn_ref_trim_i8_encode(void *out, size_t max_out_len,
	const int8_t *x, unsigned logn, unsigned bits);
size_t falcon512dyn_ref_comp_encode(void *out, size_t max_out_len,
	const int16_t *x, unsigned logn);

size_t falcon512dyn_ref_modq_decode(uint16_t *x, unsigned logn,
	const void *in, size_t max_in_len);
size_t falcon512dyn_ref_trim_i16_decode(int16_t *x, unsigned logn, unsigned bits,
	const void *in, size_t max_in_len);
size_t falcon512dyn_ref_trim_i8_decode(int8_t *x, unsigned logn, unsigned bits,
	const void *in, size_t max_in_len);
size_t falcon512dyn_ref_comp_decode(int16_t *x, unsigned logn,
	const void *in, size_t max_in_len);

/*
 * Number of bits for key elements, indexed by logn (1 to 10). This
 * is at most 8 bits for all degrees, but some degrees may have shorter
 * elements.
 */
extern const uint8_t falcon512dyn_ref_max_fg_bits[];
extern const uint8_t falcon512dyn_ref_max_FG_bits[];

/*
 * Maximum size, in bits, of elements in a signature, indexed by logn
 * (1 to 10). The size includes the sign bit.
 */
extern const uint8_t falcon512dyn_ref_max_sig_bits[];

/* ==================================================================== */
/*
 * Support functions used for both signature generation and signature
 * verification (common.c).
 */

/*
 * From a SHAKE256 context (must be already flipped), produce a new
 * point. The temporary buffer (tmp) must have room for 2*2^logn bytes.
 */
void falcon512dyn_ref_hash_to_point(shake256_context *sc,
	uint16_t *x, unsigned logn, uint8_t *tmp);

/*
 * Tell whether a given vector (2N coordinates, in two halves) is
 * acceptable as a signature. This compares the appropriate norm of the
 * vector with the acceptance bound. Returned value is 1 on success
 * (vector is short enough to be acceptable), 0 otherwise.
 */
int falcon512dyn_ref_is_short(const int16_t *s1, const int16_t *s2, unsigned logn);

/*
 * Tell whether a given vector (2N coordinates, in two halves) is
 * acceptable as a signature. Instead of the first half s1, this
 * function receives the "saturated squared norm" of s1, i.e. the
 * sum of the squares of the coordinates of s1 (saturated at 2^32-1
 * if the sum exceeds 2^31-1).
 *
 * Returned value is 1 on success (vector is short enough to be
 * acceptable), 0 otherwise.
 */
int falcon512dyn_ref_is_short_half(uint32_t sqn, const int16_t *s2, unsigned logn);

/* ==================================================================== */
/*
 * Signature verification functions (vrfy.c).
 */

/*
 * Convert a public key to NTT + Montgomery format. Conversion is done
 * in place.
 */
void falcon512dyn_ref_to_ntt_monty(uint16_t *h, unsigned logn);

/*
 * Internal signature verification code:
 *   c0[]      contains the hashed nonce+message
 *   s2[]      is the decoded signature
 *   h[]       contains the public key, in NTT + Montgomery format
 *   logn      is the degree log
 *   tmp[]     temporary, must have at least 2*2^logn bytes
 * Returned value is 1 on success, 0 on error.
 */
int falcon512dyn_ref_verify_raw(const uint16_t *c0, const int16_t *s2,
	const uint16_t *h, unsigned logn, uint8_t *tmp);

/*
 * Compute the public key h[], given the private key elements f[] and
 * g[]. This computes h = g/f mod phi mod q, where phi is the polynomial
 * modulus. This function returns 1 on success, 0 on error (an error is
 * reported if f is not invertible mod phi mod q).
 *
 * The tmp[] array must have room for at least 2*2^logn elements.
 */
int falcon512dyn_ref_compute_public(uint16_t *h,
	const int8_t *f, const int8_t *g, unsigned logn, uint8_t *tmp);

/*
 * Recompute the fourth private key element. Private key consists in
 * four polynomials with small coefficients f, g, F and G, which are
 * such that fG - gF = q mod phi; furthermore, f is invertible modulo
 * phi and modulo q. This function recomputes G from f, g and F.
 *
 * The tmp[] array must have room for at least 4*2^logn bytes.
 *
 * Returned value is 1 in success, 0 on error (f not invertible).
 */
int falcon512dyn_ref_complete_private(int8_t *G,
	const int8_t *f, const int8_t *g, const int8_t *F,
	unsigned logn, uint8_t *tmp);

/* ==================================================================== */
/*
 * Implementation of floating-point real numbers (fpr.h, fpr.c).
 */

/*
 * Real numbers are implemented by an extra header file, included below.
 * This is meant to support pluggable implementations. The default
 * implementation relies on the C type 'double'.
 *
 * The included file must define the following types, functions and
 * constants:
 *
 *   fpr
 *         type for a real number
 *
 *   fpr fpr_of(int64_t i)
 *         cast an integer into a real number; source must be in the
 *         -(2^63-1)..+(2^63-1) range
 *
 *   fpr fpr_scaled(int64_t i, int sc)
 *         compute i*2^sc as a real number; source 'i' must be in the
 *         -(2^63-1)..+(2^63-1) range
 *
 *   fpr fpr_ldexp(fpr x, int e)
 *         compute x*2^e
 *
 *   int64_t fpr_rint(fpr x)
 *         round x to the nearest integer; x must be in the -(2^63-1)
 *         to +(2^63-1) range
 *
 *   int64_t fpr_trunc(fpr x)
 *         round to an integer; this rounds towards zero; value must
 *         be in the -(2^63-1) to +(2^63-1) range
 *
 *   fpr fpr_add(fpr x, fpr y)
 *         compute x + y
 *
 *   fpr fpr_sub(fpr x, fpr y)
 *         compute x - y
 *
 *   fpr fpr_neg(fpr x)
 *         compute -x
 *
 *   fpr fpr_half(fpr x)
 *         compute x/2
 *
 *   fpr fpr_double(fpr x)
 *         compute x*2
 *
 *   fpr fpr_mul(fpr x, fpr y)
 *         compute x * y
 *
 *   fpr fpr_sqr(fpr x)
 *         compute x * x
 *
 *   fpr fpr_inv(fpr x)
 *         compute 1/x
 *
 *   fpr fpr_div(fpr x, fpr y)
 *         compute x/y
 *
 *   fpr fpr_sqrt(fpr x)
 *         compute the square root of x
 *
 *   int fpr_lt(fpr x, fpr y)
 *         return 1 if x < y, 0 otherwise
 *
 *   uint64_t fpr_expm_p63(fpr x)
 *         return exp(x), assuming that 0 <= x < log(2). Returned value
 *         is scaled to 63 bits (i.e. it really returns 2^63*exp(-x),
 *         rounded to the nearest integer). Computation should have a
 *         precision of at least 45 bits.
 *
 *   const fpr fpr_gm_tab[]
 *         array of constants for FFT / iFFT
 *
 *   const fpr fpr_p2_tab[]
 *         precomputed powers of 2 (by index, 0 to 10)
 *
 * Constants of type 'fpr':
 *
 *   fpr fpr_q                 12289
 *   fpr fpr_inverse_of_q      1/12289
 *   fpr fpr_inv_2sqrsigma0    1/(2*(1.8205^2))
 *   fpr fpr_inv_sigma         1/(1.55*sqrt(12289))
 *   fpr fpr_sigma_min_9       1.291500756233514568549480827642
 *   fpr fpr_sigma_min_10      1.311734375905083682667395805765
 *   fpr fpr_log2              log(2)
 *   fpr fpr_inv_log2          1/log(2)
 *   fpr fpr_bnorm_max         16822.4121
 *   fpr fpr_zero              0
 *   fpr fpr_one               1
 *   fpr fpr_two               2
 *   fpr fpr_onehalf           0.5
 *   fpr fpr_ptwo31            2^31
 *   fpr fpr_ptwo31m1          2^31-1
 *   fpr fpr_mtwo31m1          -(2^31-1)
 *   fpr fpr_ptwo63m1          2^63-1
 *   fpr fpr_mtwo63m1          -(2^63-1)
 *   fpr fpr_ptwo63            2^63
 */
#include "fpr.h"

/* ==================================================================== */
/*
 * RNG (rng.c).
 *
 * A PRNG based on ChaCha20 is implemented; it is seeded from a SHAKE256
 * context (flipped) and is used for bulk pseudorandom generation.
 * A system-dependent seed generator is also provided.
 */

/*
 * Obtain a random seed from the system RNG.
 *
 * Returned value is 1 on success, 0 on error.
 */
int falcon512dyn_ref_get_seed(void *seed, size_t seed_len);

/*
 * Structure for a PRNG. This includes a large buffer so that values
 * get generated in advance. The 'state' is used to keep the current
 * PRNG algorithm state (contents depend on the selected algorithm).
 *
 * The unions with 'dummy_u64' are there to ensure proper alignment for
 * 64-bit direct access.
 */
typedef struct {
	union {
		uint8_t d[512]; /* MUST be 512, exactly */
		uint64_t dummy_u64;
	} buf;
	size_t ptr;
	union {
		uint8_t d[256];
		uint64_t dummy_u64;
	} state;
	int type;
} prng;

/*
 * Instantiate a PRNG. That PRNG will feed over the provided SHAKE256
 * context (in "flipped" state) to obtain its initial state.
 */
void falcon512dyn_ref_prng_init(prng *p, shake256_context *src);

/*
 * Refill the PRNG buffer. This is normally invoked automatically, and
 * is declared here only so that prng_get_u64() may be inlined.
 */
void falcon512dyn_ref_prng_refill(prng *p);

/*
 * Get some bytes from a PRNG.
 */
void falcon512dyn_ref_prng_get_bytes(prng *p, void *dst, size_t len);

/*
 * Get a 64-bit random value from a PRNG.
 */
static inline uint64_t
prng_get_u64(prng *p)
{
	size_t u;

	/*
	 * If there are less than 9 bytes in the buffer, we refill it.
	 * This means that we may drop the last few bytes, but this allows
	 * for faster extraction code. Also, it means that we never leave
	 * an empty buffer.
	 */
	u = p->ptr;
	if (u >= (sizeof p->buf.d) - 9) {
		falcon512dyn_ref_prng_refill(p);
		u = 0;
	}
	p->ptr = u + 8;

	/*
	 * On systems that use little-endian encoding and allow
	 * unaligned accesses, we can simply read the data where it is.
	 */
	return (uint64_t)p->buf.d[u + 0]
		| ((uint64_t)p->buf.d[u + 1] << 8)
		| ((uint64_t)p->buf.d[u + 2] << 16)
		| ((uint64_t)p->buf.d[u + 3] << 24)
		| ((uint64_t)p->buf.d[u + 4] << 32)
		| ((uint64_t)p->buf.d[u + 5] << 40)
		| ((uint64_t)p->buf.d[u + 6] << 48)
		| ((uint64_t)p->buf.d[u + 7] << 56);
}

/*
 * Get an 8-bit random value from a PRNG.
 */
static inline unsigned
prng_get_u8(prng *p)
{
	unsigned v;

	v = p->buf.d[p->ptr ++];
	if (p->ptr == sizeof p->buf.d) {
		falcon512dyn_ref_prng_refill(p);
	}
	return v;
}

/* ==================================================================== */
/*
 * FFT (falcon-fft.c).
 *
 * A real polynomial is represented as an array of N 'fpr' elements.
 * The FFT representation of a real polynomial contains N/2 complex
 * elements; each is stored as two real numbers, for the real and
 * imaginary parts, respectively. See falcon-fft.c for details on the
 * internal representation.
 */

/*
 * Compute FFT in-place: the source array should contain a real
 * polynomial (N coefficients); its storage area is reused to store
 * the FFT representation of that polynomial (N/2 complex numbers).
 *
 * 'logn' MUST lie between 1 and 10 (inclusive).
 */
void falcon512dyn_ref_FFT(fpr *f, unsigned logn);

/*
 * Compute the inverse FFT in-place: the source array should contain the
 * FFT representation of a real polynomial (N/2 elements); the resulting
 * real polynomial (N coefficients of type 'fpr') is written over the
 * array.
 *
 * 'logn' MUST lie between 1 and 10 (inclusive).
 */
void falcon512dyn_ref_iFFT(fpr *f, unsigned logn);

/*
 * Add polynomial b to polynomial a. a and b MUST NOT overlap. This
 * function works in both normal and FFT representations.
 */
void falcon512dyn_ref_poly_add(fpr *restrict a, const fpr *restrict b, unsigned logn);

/*
 * Subtract polynomial b from polynomial a. a and b MUST NOT overlap. This
 * function works in both normal and FFT representations.
 */
void falcon512dyn_ref_poly_sub(fpr *restrict a, const fpr *restrict b, unsigned logn);

/*
 * Negate polynomial a. This function works in both normal and FFT
 * representations.
 */
void falcon512dyn_ref_poly_neg(fpr *a, unsigned logn);

/*
 * Compute adjoint of polynomial a. This function works only in FFT
 * representation.
 */
void falcon512dyn_ref_poly_adj_fft(fpr *a, unsigned logn);

/*
 * Multiply polynomial a with polynomial b. a and b MUST NOT overlap.
 * This function works only in FFT representation.
 */
void falcon512dyn_ref_poly_mul_fft(fpr *restrict a, const fpr *restrict b, unsigned logn);

/*
 * Multiply polynomial a with the adjoint of polynomial b. a and b MUST NOT
 * overlap. This function works only in FFT representation.
 */
void falcon512dyn_ref_poly_muladj_fft(fpr *restrict a, const fpr *restrict b, unsigned logn);

/*
 * Multiply polynomial with its own adjoint. This function works only in FFT
 * representation.
 */
void falcon512dyn_ref_poly_mulselfadj_fft(fpr *a, unsigned logn);

/*
 * Multiply polynomial with a real constant. This function works in both
 * normal and FFT representations.
 */
void falcon512dyn_ref_poly_mulconst(fpr *a, fpr x, unsigned logn);

/*
 * Divide polynomial a by polynomial b, modulo X^N+1 (FFT representation).
 * a and b MUST NOT overlap.
 */
void falcon512dyn_ref_poly_div_fft(fpr *restrict a, const fpr *restrict b, unsigned logn);

/*
 * Given f and g (in FFT representation), compute 1/(f*adj(f)+g*adj(g))
 * (also in FFT representation). Since the result is auto-adjoint, all its
 * coordinates in FFT representation are real; as such, only the first N/2
 * values of d[] are filled (the imaginary parts are skipped).
 *
 * Array d MUST NOT overlap with either a or b.
 */
void falcon512dyn_ref_poly_invnorm2_fft(fpr *restrict d,
	const fpr *restrict a, const fpr *restrict b, unsigned logn);

/*
 * Given F, G, f and g (in FFT representation), compute F*adj(f)+G*adj(g)
 * (also in FFT representation). Destination d MUST NOT overlap with
 * any of the source arrays.
 */
void falcon512dyn_ref_poly_add_muladj_fft(fpr *restrict d,
	const fpr *restrict F, const fpr *restrict G,
	const fpr *restrict f, const fpr *restrict g, unsigned logn);

/*
 * Multiply polynomial a by polynomial b, where b is autoadjoint. Both
 * a and b are in FFT representation. Since b is autoadjoint, all its
 * FFT coefficients are real, and the array b contains only N/2 elements.
 * a and b MUST NOT overlap.
 */
void falcon512dyn_ref_poly_mul_autoadj_fft(fpr *restrict a,
	const fpr *restrict b, unsigned logn);

/*
 * Divide polynomial a by polynomial b, where b is autoadjoint. Both
 * a and b are in FFT representation. Since b is autoadjoint, all its
 * FFT coefficients are real, and the array b contains only N/2 elements.
 * a and b MUST NOT overlap.
 */
void falcon512dyn_ref_poly_div_autoadj_fft(fpr *restrict a,
	const fpr *restrict b, unsigned logn);

/*
 * Perform an LDL decomposition of an auto-adjoint matrix G, in FFT
 * representation. On input, g00, g01 and g11 are provided (where the
 * matrix G = [[g00, g01], [adj(g01), g11]]). On output, the d00, l10
 * and d11 values are written in g00, g01 and g11, respectively
 * (with D = [[d00, 0], [0, d11]] and L = [[1, 0], [l10, 1]]).
 * (In fact, d00 = g00, so the g00 operand is left unmodified.)
 */
void falcon512dyn_ref_poly_LDL_fft(const fpr *restrict g00,
	fpr *restrict g01, fpr *restrict g11, unsigned logn);

/*
 * Perform an LDL decomposition of an auto-adjoint matrix G, in FFT
 * representation. This is identical to poly_LDL_fft() except that
 * g00, g01 and g11 are unmodified; the outputs d11 and l10 are written
 * in two other separate buffers provided as extra parameters.
 */
void falcon512dyn_ref_poly_LDLmv_fft(fpr *restrict d11, fpr *restrict l10,
	const fpr *restrict g00, const fpr *restrict g01,
	const fpr *restrict g11, unsigned logn);

/*
 * Apply "split" operation on a polynomial in FFT representation:
 * f = f0(x^2) + x*f1(x^2), for half-size polynomials f0 and f1
 * (polynomials modulo X^(N/2)+1). f0, f1 and f MUST NOT overlap.
 */
void falcon512dyn_ref_poly_split_fft(fpr *restrict f0, fpr *restrict f1,
	const fpr *restrict f, unsigned logn);

/*
 * Apply "merge" operation on two polynomials in FFT representation:
 * given f0 and f1, polynomials moduo X^(N/2)+1, this function computes
 * f = f0(x^2) + x*f1(x^2), in FFT representation modulo X^N+1.
 * f MUST NOT overlap with either f0 or f1.
 */
void falcon512dyn_ref_poly_merge_fft(fpr *restrict f,
	const fpr *restrict f0, const fpr *restrict f1, unsigned logn);

/* ==================================================================== */
/*
 * Key pair generation.
 */

/*
 * Required sizes of the temporary buffer (in bytes).
 */
#define FALCON_KEYGEN_TEMP_1      136
#define FALCON_KEYGEN_TEMP_2      272
#define FALCON_KEYGEN_TEMP_3      224
#define FALCON_KEYGEN_TEMP_4      448
#define FALCON_KEYGEN_TEMP_5      896
#define FALCON_KEYGEN_TEMP_6     1792
#define FALCON_KEYGEN_TEMP_7     3584
#define FALCON_KEYGEN_TEMP_8     7168
#define FALCON_KEYGEN_TEMP_9    14336
#define FALCON_KEYGEN_TEMP_10   28672

/*
 * Generate a new key pair. Randomness is extracted from the provided
 * SHAKE256 context, which must have already been seeded and flipped.
 * The tmp[] array must have suitable size (see FALCON_KEYGEN_TEMP_*
 * macros) and be aligned for the uint32_t, uint64_t and fpr types.
 *
 * The private key elements are written in f, g, F and G, and the
 * public key is written in h. Either or both of G and h may be NULL,
 * in which case the corresponding element is not returned (they can
 * be recomputed from f, g and F).
 */
void falcon512dyn_ref_keygen(shake256_context *rng,
	int8_t *f, int8_t *g, int8_t *F, int8_t *G, uint16_t *h,
	unsigned logn, uint8_t *tmp);

/* ==================================================================== */
/*
 * Signature generation.
 */

/*
 * Expand a private key into the B0 matrix in FFT representation and
 * the LDL tree. All the values are written in 'expanded_key', for
 * a total of (8*logn+40)*2^logn bytes.
 *
 * The tmp[] array must have room for at least 48*2^logn bytes.
 */
void falcon512dyn_ref_expand_privkey(fpr *restrict expanded_key,
	const int8_t *f, const int8_t *g, const int8_t *F, const int8_t *G,
	unsigned logn, uint8_t *restrict tmp);

/*
 * Compute a signature over the provided hashed message (hm); the
 * signature value is one short vector. This function uses an
 * expanded key (as generated by falcon512dyn_ref_expand_privkey()).
 *
 * The sig[] and hm[] buffers may overlap.
 *
 * The minimal size (in bytes) of tmp[] is 48*2^logn bytes.
 */
void falcon512dyn_ref_sign_tree(int16_t *sig, shake256_context *rng,
	const fpr *restrict expanded_key,
	const uint16_t *hm, unsigned logn, uint8_t *tmp);

/*
 * Compute a signature over the provided hashed message (hm); the
 * signature value is one short vector. This function uses a raw
 * key and dynamically recompute the B0 matrix and LDL tree; this
 * saves RAM since there is no needed for an expanded key, but
 * increases the signature cost.
 *
 * The sig[] and hm[] buffers may overlap.
 *
 * The minimal size (in bytes) of tmp[] is 72*2^logn bytes.
 */
void falcon512dyn_ref_sign_dyn(int16_t *sig, shake256_context *rng,
	const int8_t *restrict f, const int8_t *restrict g,
	const int8_t *restrict F, const int8_t *restrict G,
	const uint16_t *hm, unsigned logn, uint8_t *tmp);

/* ==================================================================== */

#endif
