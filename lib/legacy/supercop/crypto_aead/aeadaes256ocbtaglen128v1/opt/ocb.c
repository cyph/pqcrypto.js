/*-------------------------------------------------------------------------
/ OCB (RFC 7253) Reference Code (Optimized C)     Last modified 25-JUN-2019
/--------------------------------------------------------------------------
/ Copyright (c) 2019 Ted Krovetz.
/
/ Permission to use, copy, modify, and/or distribute this software for any
/ purpose with or without fee is hereby granted, provided that the above
/ copyright notice and this permission notice appear in all copies.
/
/ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
/ WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
/ MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
/ ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
/ WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
/ ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
/ OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
/
/ Phillip Rogaway holds patents relevant to OCB. See the following for
/ his patent grant: http://www.cs.ucdavis.edu/~rogaway/ocb/grant.htm
/
/ Special thanks to Keegan McAllister for suggesting several good improvements
/
/ Comments are welcome: Ted Krovetz <ted@krovetz.net> - Dedicated to Laurel K
/------------------------------------------------------------------------- */

/* ----------------------------------------------------------------------- */
/* Usage and implementation notes                                                             */
/* ----------------------------------------------------------------------- */

/* Usage notes:
/  - This implementation is intended for maximum speed using C (and intrinsics).
/  - Unsupported OCB features: 192-bit keys and nonces other than 12-bytes.
/  - Pointer alignment: loadu/storeu is used on SSE registers, but no other
/    support is given for unaligned pointers. On many systems this is not a
/    problem, but if your system cares, you should ensure aligned pointers.
/  - When AE_PENDING is passed as the 'final' parameter of any function,
/    the length parameters must be a multiple of BPI*16 (64 or 128 bytes).
/  - Plaintext and ciphertext pointers may be equal (ie, plaintext gets
/    encrypted in-place), but no other pointers may be equal or overlap.
/  - On systems with AES-NI or ARMv8 AES, this implementation uses those. On
/    other systems OpenSSL AES is used (so compile with its crypto library). A
/    slower reference AES is also supported (see below).
/
/  Implementation notes:
/  - If either of the compiler predefined macros __SSE2__ or __ARM_NEON are
/    set then 128-bit SIMD registers are used.
/  - If either of the compiler predefined macros __ARM_FEATURE_CRYPTO or
/    __AES__ are set then hardware AES is used.
/  - When _MSC_VER is set on an x86 or x64 system, __AES__, __SSE2__, and
/    __SSSE3__ are assumed to be true. Alter these #defines if they are wrong.
/  - This code uses loadu/storeu with SSE registers, but does native reads and
/    writes of NEON registers and of type uint64_t, so if such reads and writes
/    are a problem for your system, ensure pointers are properly aligned.
/  - This code was developed on little-endian ARM and x86 systems using gcc,
/    and has only been lightly tested on big-endian or Microsoft C.
*/

/* ----------------------------------------------------------------------- */
/* User configuration options                                              */
/* ----------------------------------------------------------------------- */

/* Set the AES key length to use and length of authentication tag to produce.
/  Setting either to 0 requires the value be set at runtime via ae_init().
/  Some optimizations occur for each when set to a fixed value.            */
#define OCB_KEY_LEN         32  /* 0, 16 or 32. 0 means set in ae_init     */
#define OCB_TAG_LEN         16  /* 0 to 16. 0 means set in ae_init         */

/* This implementation uses AES intrinsics if any of the macros __AES__,   */
/* __ARM_FEATURE_CRYPTO, or __ARM_FEATURE_AES are non-zero. The default    */
/* alternative is to use OpenSSL, which can be overridden by setting the   */
/* following to zero and compiling along with "rijndael-alg-fst.c".        */
#define OPENSSL_IS_AES_BACKUP    1

/* During encryption and decryption, various "L values" are required.
/  The L values can be precomputed during initialization (requiring extra
/  space in ae_ctx), generated as needed (slightly slowing encryption and
/  decryption), or some combination of the two. L_TABLE_SZ specifies how many
/  L values to precompute. L_TABLE_SZ must be at least 3. L_TABLE_SZ*16 bytes
/  are used for L values in ae_ctx. Plaintext and ciphertexts shorter than
/  2^L_TABLE_SZ blocks need no L values calculated dynamically.            */
#define L_TABLE_SZ          16

/* Set L_TABLE_SZ_IS_ENOUGH non-zero iff you know that all plaintexts
/  will be shorter than 2^(L_TABLE_SZ+4) bytes in length. This results
/  in better performance.                                                  */
#define L_TABLE_SZ_IS_ENOUGH 1

/* ----------------------------------------------------------------------- */
/* Includes and compiler specific definitions                              */
/* ----------------------------------------------------------------------- */

#include "ae.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* Compiler-specific intrinsics and fixes: bswap64, ntz                    */
#if _MSC_VER

	#define inline __inline        /* MSVC doesn't recognize "inline" in C */
    #define __SSE2__   (_M_IX86 || _M_AMD64 || _M_X64)    /* Assume SSE2   */
    #define __SSSE3__  (_M_IX86 || _M_AMD64 || _M_X64)    /* Assume SSSE3  */
    #define __AES__  (_M_IX86 || _M_AMD64 || _M_X64)      /* Assume AES-NI */
	#include <intrin.h>
	#include <malloc.h>
	#pragma intrinsic(_byteswap_uint64, _BitScanForward, memcpy)
	#define bswap64(x) _byteswap_uint64(x)
	static inline unsigned ntz(unsigned long x){_BitScanForward(&x,x);return x;}
	
#elif __GNUC__

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wimplicit-fallthrough"

	#define inline __inline__            /* No "inline" in GCC ansi C mode */
	#define bswap64(x) __builtin_bswap64(x)           /* Assuming GCC 4.3+ */
	#define ntz(x)     __builtin_ctz((unsigned)(x))   /* Assuming GCC 3.4+ */

#else                        /* Assume some C99 features: stdint.h, inline */

    static inline uint64_t bswap64(uint64_t x) {
        return ((x & UINT64_C(0xFF00000000000000)) >> 56) |
               ((x & UINT64_C(0x00FF000000000000)) >> 40) |
               ((x & UINT64_C(0x0000FF0000000000)) >> 24) |
               ((x & UINT64_C(0x000000FF00000000)) >>  8) |
               ((x & UINT64_C(0x00000000FF000000)) <<  8) |      
               ((x & UINT64_C(0x0000000000FF0000)) << 24) |
               ((x & UINT64_C(0x000000000000FF00)) << 40) |
               ((x & UINT64_C(0x00000000000000FF)) << 56);
	}

	#if (L_TABLE_SZ <= 9) && (L_TABLE_SZ_IS_ENOUGH)   /* < 2^13 byte texts */
	static inline unsigned ntz(unsigned x) {
		static const unsigned char tz_table[] = {0,
		2,3,2,4,2,3,2,5,2,3,2,4,2,3,2,6,2,3,2,4,2,3,2,5,2,3,2,4,2,3,2,7,
		2,3,2,4,2,3,2,5,2,3,2,4,2,3,2,6,2,3,2,4,2,3,2,5,2,3,2,4,2,3,2,8,
		2,3,2,4,2,3,2,5,2,3,2,4,2,3,2,6,2,3,2,4,2,3,2,5,2,3,2,4,2,3,2,7,
		2,3,2,4,2,3,2,5,2,3,2,4,2,3,2,6,2,3,2,4,2,3,2,5,2,3,2,4,2,3,2};
		return tz_table[x/4];
	}
	#else       /* From http://supertech.csail.mit.edu/papers/debruijn.pdf */
	static inline unsigned ntz(unsigned x) {
		static const unsigned char tz_table[32] =
		{ 0,  1, 28,  2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17,  4, 8,
		 31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18,  6, 11,  5, 10, 9};
		return tz_table[((uint32_t)((x & -x) * 0x077CB531u)) >> 27];
	}
	#endif

#endif

/* ----------------------------------------------------------------------- */
/* Define blocks and operations -- Patch if incorrect on your compiler.    */
/* ----------------------------------------------------------------------- */

#if __SSE2__

    #include <immintrin.h>
    typedef __m128i block;
    #define xor_block(x,y)       _mm_xor_si128(x,y)
    #define zero_block()         _mm_setzero_si128()
    #define unequal_blocks(x,y) (_mm_movemask_epi8(_mm_cmpeq_epi8(x,y))!=0xffff)
	#if __SSSE3__
    #define swap_if_le(b) \
      _mm_shuffle_epi8(b,_mm_set_epi8(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15))
	#else
    static inline block swap_if_le(block b) {
		block a = _mm_shuffle_epi32  (b, _MM_SHUFFLE(0,1,2,3));
		a = _mm_shufflehi_epi16(a, _MM_SHUFFLE(2,3,0,1));
		a = _mm_shufflelo_epi16(a, _MM_SHUFFLE(2,3,0,1));
		return _mm_xor_si128(_mm_srli_epi16(a,8), _mm_slli_epi16(a,8));
    }
	#endif
	static inline block gen_offset(uint64_t KtopStr[3], unsigned bot) {
		block hi = _mm_load_si128((__m128i *)(KtopStr+0));   /* hi = B A */
		block lo = _mm_loadu_si128((__m128i *)(KtopStr+1));  /* lo = C B */
		__m128i lshift = _mm_cvtsi32_si128(bot);
		__m128i rshift = _mm_cvtsi32_si128(64-bot);
		lo = _mm_xor_si128(_mm_sll_epi64(hi,lshift),_mm_srl_epi64(lo,rshift));
		#if __SSSE3__
		return _mm_shuffle_epi8(lo,_mm_set_epi8(8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7));
		#else
		return swap_if_le(_mm_shuffle_epi32(lo, _MM_SHUFFLE(1,0,3,2)));
		#endif
	}
	static inline block double_block(block bl) {
		const __m128i mask = _mm_set_epi32(135,1,1,1);
		__m128i tmp = _mm_srai_epi32(bl, 31);
		tmp = _mm_and_si128(tmp, mask);
		tmp = _mm_shuffle_epi32(tmp, _MM_SHUFFLE(2,1,0,3));
		bl = _mm_slli_epi32(bl, 1);
		return _mm_xor_si128(bl,tmp);
	}
	#define load(p)               _mm_loadu_si128((__m128i *)(p))
	#define store(p,x)            _mm_storeu_si128((__m128i *)(p),x)

#elif __ARM_NEON || __ARM_NEON__

    #include <arm_neon.h>
    typedef uint32x4_t block;
    #define xor_block(x,y)             veorq_u32(x,y)
    #define zero_block()               ((block){0})
    static inline int unequal_blocks(block a, block b) {
		int64x2_t t=veorq_s64((int64x2_t)a,(int64x2_t)b);
		return (vgetq_lane_s64(t,0)|vgetq_lane_s64(t,1))!=0;
    }
    #define swap_if_le(b)              (b)
	/* KtopStr is reg correct by 64 bits, return mem correct */
	block gen_offset(uint64_t KtopStr[3], unsigned bot) {
		const union { unsigned x; unsigned char endian; } little = { 1 };
		const int64x2_t k64 = {-64,-64};
		uint64x2_t hi = *(uint64x2_t *)(KtopStr+0);   /* hi = A B */
		uint64x2_t lo = *(uint64x2_t *)(KtopStr+1);   /* hi = B C */
		int64x2_t ls = vdupq_n_s64(bot);
		int64x2_t rs = vqaddq_s64(k64,ls);
		block rval = (block)veorq_u64(vshlq_u64(hi,ls),vshlq_u64(lo,rs));
		if (little.endian)
			rval = (block)vrev64q_s8((int8x16_t)rval);
		return rval;
	}
	static inline block double_block(block b)
	{
		const int8x16_t mask = {-121,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
		int8x16_t tmp = vshrq_n_s8((int8x16_t)b,7);
		tmp = vandq_s8(tmp, mask);
		tmp = vextq_s8(tmp, tmp, 1);  /* Rotate high byte to end */
		return (block)veorq_s8(tmp,vshlq_n_s8((int8x16_t)b,1));
	}
	#define load(p)               (*(block *)(p))
	#define store(p,x)            (*(block *)(p)) = (block)(x)

#else

    typedef struct { uint64_t l,r; } block;
    static inline block xor_block(block x, block y) {
    	x.l^=y.l; x.r^=y.r; return x;
    }
    static inline block zero_block(void) { const block t = {0,0}; return t; }
    #define unequal_blocks(x, y)         ((((x).l^(y).l)|((x).r^(y).r)) != 0)
    static inline block swap_if_le(block b) {
		const union { unsigned x; unsigned char endian; } little = { 1 };
    	if (little.endian) {
    		block r;
    		r.l = bswap64(b.l);
    		r.r = bswap64(b.r);
    		return r;
    	} else
    		return b;
    }

	/* KtopStr is reg correct by 64 bits, return mem correct */
	block gen_offset(uint64_t KtopStr[3], unsigned bot) {
        block rval;
        if (bot != 0) {
			rval.l = (KtopStr[0] << bot) | (KtopStr[1] >> (64-bot));
			rval.r = (KtopStr[1] << bot) | (KtopStr[2] >> (64-bot));
		} else {
			rval.l = KtopStr[0];
			rval.r = KtopStr[1];
		}
        return swap_if_le(rval);
	}

	#if __GNUC__ && __arm__
	static inline block double_block(block b) {
		__asm__ ("adds %1,%1,%1\n\t"
				 "adcs %H1,%H1,%H1\n\t"
				 "adcs %0,%0,%0\n\t"
				 "adcs %H0,%H0,%H0\n\t"
				 "it cs\n\t"
				 "eorcs %1,%1,#135"
		: "+r"(b.l), "+r"(b.r) : : "cc");
		return b;
	}
	#else
	static inline block double_block(block b) {
		uint64_t t = (uint64_t)((int64_t)b.l >> 63);
		b.l = (b.l + b.l) ^ (b.r >> 63);
		b.r = (b.r + b.r) ^ (t & 135);
		return b;
	}
	#endif
	#define load(p)               (*(block *)(p))
	#define store(p,x)            (*(block *)(p)) = (block)(x)

#endif

/* ----------------------------------------------------------------------- */
/* AES 128 and 256 - Uses intrinsics if available or OpenSSL.              */
/* ----------------------------------------------------------------------- */

#define USE_AES_INTRINSICS (__AES__||__ARM_FEATURE_CRYPTO||__ARM_FEATURE_AES)

/* ------------------------------------------------------------------------- */
#if USE_AES_INTRINSICS
/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
#if __AES__
/* ------------------------------------------------------------------------- */

#define enc_rnd(b,k)          _mm_aesenc_si128(b,k)
#define enc_last(b,k)         _mm_aesenclast_si128(b,k)
#define dec_rnd(b,k)          _mm_aesdec_si128(b,k)
#define dec_last(b,k)         _mm_aesdeclast_si128(b,k)
#define aes_imc(k)            _mm_aesimc_si128(k)

#define EXPAND_ASSIST(v1,v2,v3,v4,shuff_const,aes_const)                    \
    v2 = _mm_aeskeygenassist_si128(v4,aes_const);                           \
    v3 = _mm_castps_si128(_mm_shuffle_ps(_mm_castsi128_ps(v3),              \
                                         _mm_castsi128_ps(v1), 16));        \
    v1 = _mm_xor_si128(v1,v3);                                              \
    v3 = _mm_castps_si128(_mm_shuffle_ps(_mm_castsi128_ps(v3),              \
                                         _mm_castsi128_ps(v1), 140));       \
    v1 = _mm_xor_si128(v1,v3);                                              \
    v2 = _mm_shuffle_epi32(v2,shuff_const);                                 \
    v1 = _mm_xor_si128(v1,v2)

/* ------------------------------------------------------------------------- */
#elif __ARM_FEATURE_CRYPTO || __ARM_FEATURE_AES
/* ------------------------------------------------------------------------- */

#define enc_rnd(b,k)   (block)vaesmcq_u8(vaeseq_u8((uint8x16_t)b,(uint8x16_t)k))
#define enc_last(b,k)  (block)vaeseq_u8((uint8x16_t)b,(uint8x16_t)k)
#define dec_rnd(b,k)   (block)vaesimcq_u8(vaesdq_u8((uint8x16_t)b,(uint8x16_t)k))
#define dec_last(b,k)  (block)vaesdq_u8((uint8x16_t)b,(uint8x16_t)k)
#define aes_imc(k)     (block)vaesimcq_u8((uint8x16_t)k)

/* The following function is based on code from Michael Brase's blog         */
static inline block _mm_aeskeygenassist_si128 (block in, const int rcon)
{   /* AESE does ShiftRows and SubBytes. */
    uint8x16_t a = vaeseq_u8((uint8x16_t)in, (uint8x16_t){0});
    return xor_block(((block)(uint8x16_t){
        /* Undo ShiftRows step from AESE and extract X1 and X3 */
        a[0x4], a[0x1], a[0xE], a[0xB], /* SubBytes(X1)        */
        a[0x1], a[0xE], a[0xB], a[0x4], /* ROT(SubBytes(X1))   */
        a[0xC], a[0x9], a[0x6], a[0x3], /* SubBytes(X3)        */
        a[0x9], a[0x6], a[0x3], a[0xC], /* ROT(SubBytes(X3))   */
    }), ((block)(uint32x4_t){0, (unsigned)rcon, 0, (unsigned)rcon}));
}

#define EXPAND_ASSIST(v1,v2,v3,v4,i,aes_const)          \
    v2 = _mm_aeskeygenassist_si128(v4,aes_const);                 \
    v3 = (uint32x4_t){v3[0],v3[0],v1[1],v1[0]};                   \
    v1 = xor_block(v1, v3);                                       \
    v3 = (uint32x4_t){v3[0],v3[3],v1[0],v1[2]};                   \
    v1 = xor_block(v1, v3);                                       \
    v1 = xor_block(v1, ((uint32x4_t){v2[i&3],v2[i&3],v2[i&3],v2[i&3]}))

/* ------------------------------------------------------------------------- */
#endif                                          /* x86 or ARM AES intrinsics */
/* ------------------------------------------------------------------------- */

#if (OCB_KEY_LEN == 32)                 /* Set number of blocks per ECB call */
#define BPI 4                           /* 4 helps with register pressure    */
#else
#define BPI 8
#endif

#if (OCB_KEY_LEN == 0)
    typedef struct { block rd_key[15]; int rounds; } AES_KEY;
    #define ROUNDS(ctx) ((ctx)->rounds)
#else
    typedef struct { block rd_key[7+OCB_KEY_LEN/4]; } AES_KEY;
    #define ROUNDS(ctx) (6+OCB_KEY_LEN/4)
#endif

static void AES_128_Key_Expansion(const unsigned char *userkey, void *key) {
    block x0, x1, x2;
    block *kp = (block *)key;
    kp[0] = x0 = load(userkey);
    x2 = zero_block();
    EXPAND_ASSIST(x0,x1,x2,x0,255,1);   kp[1]  = x0;
    EXPAND_ASSIST(x0,x1,x2,x0,255,2);   kp[2]  = x0;
    EXPAND_ASSIST(x0,x1,x2,x0,255,4);   kp[3]  = x0;
    EXPAND_ASSIST(x0,x1,x2,x0,255,8);   kp[4]  = x0;
    EXPAND_ASSIST(x0,x1,x2,x0,255,16);  kp[5]  = x0;
    EXPAND_ASSIST(x0,x1,x2,x0,255,32);  kp[6]  = x0;
    EXPAND_ASSIST(x0,x1,x2,x0,255,64);  kp[7]  = x0;
    EXPAND_ASSIST(x0,x1,x2,x0,255,128); kp[8]  = x0;
    EXPAND_ASSIST(x0,x1,x2,x0,255,27);  kp[9]  = x0;
    EXPAND_ASSIST(x0,x1,x2,x0,255,54);  kp[10] = x0;
}

static void AES_256_Key_Expansion(const unsigned char *userkey, void *key)
{
    block x0,x1,x2,x3,*kp = (block *)key;
    kp[0] = x0 = load(userkey);
    kp[1] = x3 = load(userkey+16);
    x2 = zero_block();
    EXPAND_ASSIST(x0,x1,x2,x3,255,1);  kp[2]  = x0;
    EXPAND_ASSIST(x3,x1,x2,x0,170,1);  kp[3]  = x3;
    EXPAND_ASSIST(x0,x1,x2,x3,255,2);  kp[4]  = x0;
    EXPAND_ASSIST(x3,x1,x2,x0,170,2);  kp[5]  = x3;
    EXPAND_ASSIST(x0,x1,x2,x3,255,4);  kp[6]  = x0;
    EXPAND_ASSIST(x3,x1,x2,x0,170,4);  kp[7]  = x3;
    EXPAND_ASSIST(x0,x1,x2,x3,255,8);  kp[8]  = x0;
    EXPAND_ASSIST(x3,x1,x2,x0,170,8);  kp[9]  = x3;
    EXPAND_ASSIST(x0,x1,x2,x3,255,16); kp[10] = x0;
    EXPAND_ASSIST(x3,x1,x2,x0,170,16); kp[11] = x3;
    EXPAND_ASSIST(x0,x1,x2,x3,255,32); kp[12] = x0;
    EXPAND_ASSIST(x3,x1,x2,x0,170,32); kp[13] = x3;
    EXPAND_ASSIST(x0,x1,x2,x3,255,64); kp[14] = x0;
}

static void AES_set_decrypt_key_fast(AES_KEY *dkey, const AES_KEY *ekey)
{
    int j = 0, i = ROUNDS(ekey);
    #if (OCB_KEY_LEN == 0)
    dkey->rounds = i;
    #endif
    dkey->rd_key[i--] = ekey->rd_key[j++];
    while (i)
        dkey->rd_key[i--] = aes_imc(ekey->rd_key[j++]);
    dkey->rd_key[i] = ekey->rd_key[j];
}

static int AES_set_keys(const unsigned char *userKey, const int bits,
                        AES_KEY *ekey, AES_KEY *dkey)
{
    if (bits == 128)
        AES_128_Key_Expansion (userKey,ekey);
    else 
        AES_256_Key_Expansion (userKey,ekey);
    #if (OCB_KEY_LEN == 0)
    ekey->rounds = 6+bits/32;
    #endif
    AES_set_decrypt_key_fast(dkey, ekey);
    return 0;
}

static void AES_encrypt(const unsigned char *in,
                        unsigned char *out, const AES_KEY *key)
{
    int j, rnds=ROUNDS(key);
    const block *kp = ((block *)(key->rd_key));
    block tmp =  load(in);
    #if __AES__
        tmp = xor_block(tmp,kp[0]);
        for (j=1; j<rnds; j++)
            tmp = enc_rnd(tmp,kp[j]);
        tmp = enc_last(tmp,kp[j]);
    #else
        for (j=0; j<rnds-1; j++)
            tmp = enc_rnd(tmp,kp[j]);
        tmp = enc_last(tmp,kp[j]);
        tmp = xor_block(tmp,kp[j+1]);
    #endif
    store(out, tmp);
}


static inline void AES_ecb_encrypt_blks(block *blks, unsigned nblks, AES_KEY *key) {
    unsigned i,j,rnds=ROUNDS(key);
    block tmp[BPI+1];
    const block *kp = ((block *)(key->rd_key));
    #if __AES__
        for (i=0; i<nblks; ++i)
            tmp[i] = enc_rnd(xor_block(load(blks+i), kp[0]), kp[1]);
        for(j=2; j<rnds; ++j)
            for (i=0; i<nblks; ++i)
                tmp[i] = enc_rnd(tmp[i], kp[j]);
        for (i=0; i<nblks; ++i)
            store(blks+i, enc_last(tmp[i], kp[j]));
    #else
        for (i=0; i<nblks; ++i)
            tmp[i] = enc_rnd(load(blks+i), kp[0]);
        for(j=1; j<rnds-1; ++j)
            for (i=0; i<nblks; ++i)
                tmp[i] = enc_rnd(tmp[i], kp[j]);
        for (i=0; i<nblks; ++i)
            store(blks+i, xor_block(enc_last(tmp[i], kp[j]), kp[j+1]));
    #endif
}

static inline void AES_ecb_decrypt_blks(block *blks, unsigned nblks, AES_KEY *key) {
    unsigned i,j,rnds=ROUNDS(key);
    block tmp[BPI+1];
    const block *kp = ((block *)(key->rd_key));
    #if __AES__
        for (i=0; i<nblks; ++i)
            tmp[i] = dec_rnd(xor_block(load(blks+i), kp[0]), kp[1]);
        for(j=2; j<rnds; ++j)
            for (i=0; i<nblks; ++i)
                tmp[i] = dec_rnd(tmp[i], kp[j]);
        for (i=0; i<nblks; ++i)
            store(blks+i, dec_last(tmp[i], kp[j]));
    #else
        for (i=0; i<nblks; ++i)
            tmp[i] = dec_rnd(load(blks+i), kp[0]);
        for(j=1; j<rnds-1; ++j)
            for (i=0; i<nblks; ++i)
                tmp[i] = dec_rnd(tmp[i], kp[j]);
        for (i=0; i<nblks; ++i)
            store(blks+i, xor_block(dec_last(tmp[i], kp[j]), kp[j+1]));
    #endif
}

/*-------------------*/
#elif !OPENSSL_IS_AES_BACKUP
/*-------------------*/

#define BPI 4  /* Number of blocks in buffer per ECB call */

#include "rijndael-alg-fst.h"              /* Barreto's Public-Domain Code */
#if (OCB_KEY_LEN == 0)
	typedef struct { uint32_t rd_key[60]; int rounds; } AES_KEY;
	#define ROUNDS(ctx) ((ctx)->rounds)
#else
	typedef struct { uint32_t rd_key[OCB_KEY_LEN+28]; } AES_KEY;
	#define ROUNDS(ctx) (6+OCB_KEY_LEN/4)
#endif

static int AES_set_keys(const unsigned char *userKey, const int bits,
                        AES_KEY *ekey, AES_KEY *dkey)
{
    rijndaelKeySetupEnc(ekey->rd_key, userKey, bits);
    rijndaelKeySetupDec(dkey->rd_key, userKey, bits);
    #if (OCB_KEY_LEN == 0)
        ekey->rounds = dkey->rounds = bits/32+6;
    #endif
    return 0;
}

static void AES_ecb_encrypt_blks(block *blks, unsigned nblks, AES_KEY *key) {
	while (nblks) {
		--nblks;
		rijndaelEncrypt(key->rd_key, ROUNDS(key), (unsigned char *)(blks+nblks),
		                (unsigned char *)(blks+nblks));
	}
}

static void AES_ecb_decrypt_blks(block *blks, unsigned nblks, AES_KEY *key) {
	while (nblks) {
		--nblks;
		rijndaelDecrypt(key->rd_key, ROUNDS(key), (unsigned char *)(blks+nblks),
		                (unsigned char *)(blks+nblks));
	}
}

#define AES_encrypt(x,y,z) rijndaelEncrypt((z)->rd_key, ROUNDS(z), x, y)

/*---------------*/
#else
/*---------------*/

#define BPI 8  /* Number of blocks in buffer per ECB call */

#include <openssl/evp.h>

typedef EVP_CIPHER_CTX* AES_KEY;

static int AES_set_keys(const unsigned char *userKey, const int bits,
                        AES_KEY *ekey, AES_KEY *dkey)
{
    int a, b;
    *ekey = EVP_CIPHER_CTX_new();
    if (*ekey == NULL)
        return -1;
    *dkey = EVP_CIPHER_CTX_new();
    if (*dkey == NULL) {
        EVP_CIPHER_CTX_free(*ekey);
        return -1;
    }
    if (bits == 128) {
        a = EVP_EncryptInit_ex(*ekey, EVP_aes_128_ecb(), NULL, userKey, NULL);
        b = EVP_DecryptInit_ex(*dkey, EVP_aes_128_ecb(), NULL, userKey, NULL);
    } else {
        a = EVP_EncryptInit_ex(*ekey, EVP_aes_256_ecb(), NULL, userKey, NULL);
        b = EVP_DecryptInit_ex(*dkey, EVP_aes_256_ecb(), NULL, userKey, NULL);
    }
    if ((a==0) || (b==0)) {
        EVP_CIPHER_CTX_free(*ekey);
        EVP_CIPHER_CTX_free(*dkey);
        return -1;
    }
    EVP_CIPHER_CTX_set_padding(*ekey, 0);
    EVP_CIPHER_CTX_set_padding(*dkey, 0);
    return 0;
}

static void AES_encrypt(const unsigned char *in,
                        unsigned char *out, const AES_KEY *key)
{
    int len;
    EVP_EncryptUpdate(*key, out, &len, in, 16);
}

static void AES_ecb_encrypt_blks(block *blks, unsigned nblks, AES_KEY *key) {
    int len;
    EVP_EncryptUpdate(*key, (unsigned char *)blks, &len,
                        (unsigned char *)blks, nblks*16);
}

static void AES_ecb_decrypt_blks(block *blks, unsigned nblks, AES_KEY *key) {
    int len;
    EVP_DecryptUpdate(*key, (unsigned char *)blks, &len,
                        (unsigned char *)blks, nblks*16);
}

/* ------------------------------------------------------------------------- */
#endif
/* ------------------------------------------------------------------------- */

/* ----------------------------------------------------------------------- */
/* Define OCB context structure.                                           */
/* ----------------------------------------------------------------------- */

/*------------------------------------------------------------------------
/ Each item in the OCB context is stored either "memory correct" or
/ "register correct". On big-endian machines, this is identical. On
/ little-endian machines, one must choose whether the byte-string
/ is in the correct order when it resides in memory or in registers.
/ It must be register correct whenever it is to be manipulated
/ arithmetically, but must be memory correct whenever it interacts
/ with the plaintext or ciphertext.
/------------------------------------------------------------------------- */

struct _ae_ctx {
    block offset;                          /* Memory correct               */
    block checksum;                        /* Memory correct               */
    block Lstar;                           /* Memory correct               */
    block Ldollar;                         /* Memory correct               */
    block L[L_TABLE_SZ];                   /* Memory correct               */
    block ad_checksum;                     /* Memory correct               */
    block ad_offset;                       /* Memory correct               */
    block cached_Top;                      /* Memory correct               */
	uint64_t KtopStr[3];                   /* Register correct, each item  */
    uint32_t ad_blocks_processed;
    uint32_t blocks_processed;
    AES_KEY decrypt_key;
    AES_KEY encrypt_key;
    #if (OCB_TAG_LEN == 0)
    unsigned tag_len;
    #endif
};

/* ----------------------------------------------------------------------- */
/* L table lookup (or on-the-fly generation)                               */
/* ----------------------------------------------------------------------- */

#if L_TABLE_SZ_IS_ENOUGH
#define getL(_ctx, _tz) ((_ctx)->L[_tz])
#else
static block getL(const ae_ctx *ctx, unsigned tz)
{
    if (tz < L_TABLE_SZ)
        return ctx->L[tz];
    else {
        unsigned i;
        /* Bring L[MAX] into registers, make it register correct */
        block rval = swap_if_le(ctx->L[L_TABLE_SZ-1]);
        rval = double_block(rval);
        for (i=L_TABLE_SZ; i < tz; i++)
            rval = double_block(rval);
        return swap_if_le(rval);             /* To memory correct */
    }
}
#endif

/* ----------------------------------------------------------------------- */
/* Public functions                                                        */
/* ----------------------------------------------------------------------- */

/* 32-bit SSE2 systems need to be forced to allocate memory on 16-byte
   alignments. (I believe all major 64-bit systems do already.) */

ae_ctx* ae_allocate(void *misc)
{
    void *p;
	((void)misc);                            /* Suppress "unused variable" */
	#if _MSC_VER && _M_IX86                  /* 32-bit Microsoft x86       */
	    p = _aligned_malloc(sizeof(ae_ctx),16);
	#elif __SSE2__ && __i386                   /* 32-bit x86                 */
		if (posix_memalign(&p,16,sizeof(ae_ctx)) != 0) p = NULL;
	#else
		p = malloc(sizeof(ae_ctx));
	#endif
    return (ae_ctx *)p;
}

void ae_free(ae_ctx *ctx)
{
    #if !USE_AES_INTRINSICS && OPENSSL_IS_AES_BACKUP
        if (ctx->decrypt_key) EVP_CIPHER_CTX_free(ctx->decrypt_key);
        if (ctx->encrypt_key) EVP_CIPHER_CTX_free(ctx->encrypt_key);
    #endif
	#if _MSC_VER && _M_IX86                  /* 32-bit Windows             */
        _aligned_free(ctx);
	#else
		free(ctx);
	#endif
}

/* ----------------------------------------------------------------------- */

int ae_clear (ae_ctx *ctx) /* Zero ae_ctx and undo initialization          */
{
    #if !USE_AES_INTRINSICS && OPENSSL_IS_AES_BACKUP
        EVP_CIPHER_CTX_free(ctx->decrypt_key);
        EVP_CIPHER_CTX_free(ctx->encrypt_key);
    #endif
	memset(ctx, 0, sizeof(ae_ctx));
	return AE_SUCCESS;
}

int ae_ctx_sizeof(void) { return (int) sizeof(ae_ctx); }

/* ----------------------------------------------------------------------- */

int ae_init(ae_ctx *ctx, const void *key, int key_len, int nonce_len, int tag_len)
{
    unsigned i;
    block tmp_blk;

    if ((nonce_len != 12) ||
        (OCB_KEY_LEN != 0 && key_len != OCB_KEY_LEN) ||
        (OCB_TAG_LEN != 0 && tag_len != OCB_TAG_LEN) ||
        (tag_len < 1 || tag_len > 16) ||
        (key_len != 16 && key_len != 32))
    	return AE_NOT_SUPPORTED;

    /* Initialize encryption & decryption keys */
    AES_set_keys((unsigned char *)key, key_len*8, &ctx->encrypt_key, &ctx->decrypt_key);

    /* Zero things that need zeroing */
    ctx->cached_Top = ctx->ad_checksum = zero_block();
    ctx->ad_blocks_processed = 0;

    /* Compute key-dependent values */
    AES_encrypt((unsigned char *)&ctx->cached_Top,
                            (unsigned char *)&ctx->Lstar, &ctx->encrypt_key);
    tmp_blk = swap_if_le(ctx->Lstar);
    tmp_blk = double_block(tmp_blk);
    ctx->Ldollar = swap_if_le(tmp_blk);
    tmp_blk = double_block(tmp_blk);
    ctx->L[0] = swap_if_le(tmp_blk);
    for (i = 1; i < L_TABLE_SZ; i++) {
		tmp_blk = double_block(tmp_blk);
    	ctx->L[i] = swap_if_le(tmp_blk);
    }

    #if (OCB_TAG_LEN == 0)
    	ctx->tag_len = tag_len;
    #endif

    return AE_SUCCESS;
}

/* ----------------------------------------------------------------------- */

static block gen_offset_from_nonce(ae_ctx *ctx, const void *nonce)
{
	const union { unsigned x; unsigned char endian; } little = { 1 };
	union { uint32_t u32[4]; uint8_t u8[16]; block bl; } tmp;
	unsigned idx;
    #if (OCB_TAG_LEN > 0)
        unsigned tag_len = OCB_TAG_LEN;
    #else
        unsigned tag_len = ctx->tag_len;
    #endif
	/* Replace cached nonce Top if needed */
    if (little.endian) tmp.u32[0] = 0x01000000 + ((tag_len * 8 % 128) << 1);
    else               tmp.u32[0] = 0x00000001 + ((tag_len * 8 % 128) << 25);
	memcpy(tmp.u32+1,nonce,12);
	idx = (unsigned)(tmp.u8[15] & 0x3f);   /* Get low 6 bits of nonce  */
	tmp.u8[15] = tmp.u8[15] & 0xc0;        /* Zero low 6 bits of nonce */
	if ( unequal_blocks(tmp.bl,ctx->cached_Top) )   { /* Cached?       */
		ctx->cached_Top = tmp.bl;          /* Update cache, KtopStr    */
		AES_encrypt(tmp.u8, (unsigned char *)&ctx->KtopStr, &ctx->encrypt_key);
		if (little.endian) {               /* Make Register Correct    */
			ctx->KtopStr[0] = bswap64(ctx->KtopStr[0]);
			ctx->KtopStr[1] = bswap64(ctx->KtopStr[1]);
		}
		ctx->KtopStr[2] = ctx->KtopStr[0] ^
						 (ctx->KtopStr[0] << 8) ^ (ctx->KtopStr[1] >> 56);
	}
	return gen_offset(ctx->KtopStr, idx);
}

static void process_ad(ae_ctx *ctx, const void *ad, int ad_len, int final)
{
	union { uint32_t u32[4]; uint8_t u8[16]; block bl; } tmp;
    block ad_offset, ad_checksum;
    const block *  adp = (block *)ad;
	unsigned i,k,tz,remaining;

    ad_offset = ctx->ad_offset;
    ad_checksum = ctx->ad_checksum;
    i = ad_len/(BPI*16);
    if (i) {
		unsigned ad_block_num = ctx->ad_blocks_processed;
		do {
			block ta[BPI], oa[BPI];
			ad_block_num += BPI;
			tz = ntz(ad_block_num);
			oa[0] = xor_block(ad_offset, ctx->L[0]);
			ta[0] = xor_block(oa[0], load(adp+0));
			oa[1] = xor_block(oa[0], ctx->L[1]);
			ta[1] = xor_block(oa[1], load(adp+1));
			oa[2] = xor_block(ad_offset, ctx->L[1]);
			ta[2] = xor_block(oa[2], load(adp+2));
			#if BPI == 4
				ad_offset = xor_block(oa[2], getL(ctx, tz));
				ta[3] = xor_block(ad_offset, load(adp+3));
			#elif BPI == 8
				oa[3] = xor_block(oa[2], ctx->L[2]);
				ta[3] = xor_block(oa[3], load(adp+3));
				oa[4] = xor_block(oa[1], ctx->L[2]);
				ta[4] = xor_block(oa[4], load(adp+4));
				oa[5] = xor_block(oa[0], ctx->L[2]);
				ta[5] = xor_block(oa[5], load(adp+5));
				oa[6] = xor_block(ad_offset, ctx->L[2]);
				ta[6] = xor_block(oa[6], load(adp+6));
				ad_offset = xor_block(oa[6], getL(ctx, tz));
				ta[7] = xor_block(ad_offset, load(adp+7));
			#endif
			AES_ecb_encrypt_blks(ta,BPI,&ctx->encrypt_key);
			ad_checksum = xor_block(ad_checksum, ta[0]);
			ad_checksum = xor_block(ad_checksum, ta[1]);
			ad_checksum = xor_block(ad_checksum, ta[2]);
			ad_checksum = xor_block(ad_checksum, ta[3]);
			#if (BPI == 8)
			ad_checksum = xor_block(ad_checksum, ta[4]);
			ad_checksum = xor_block(ad_checksum, ta[5]);
			ad_checksum = xor_block(ad_checksum, ta[6]);
			ad_checksum = xor_block(ad_checksum, ta[7]);
			#endif
			adp += BPI;
		} while (--i);
		ctx->ad_blocks_processed = ad_block_num;
		ctx->ad_offset = ad_offset;
		ctx->ad_checksum = ad_checksum;
	}

    if (final) {
		block ta[BPI];

        /* Process remaining associated data, compute its tag contribution */
        remaining = ((unsigned)ad_len) % (BPI*16);
        if (remaining) {
			k=0;
			#if (BPI == 8)
			if (remaining >= 64) {
				tmp.bl = xor_block(ad_offset, ctx->L[0]);
				ta[0] = xor_block(tmp.bl, load(adp+0));
				tmp.bl = xor_block(tmp.bl, ctx->L[1]);
				ta[1] = xor_block(tmp.bl, load(adp+1));
				ad_offset = xor_block(ad_offset, ctx->L[1]);
				ta[2] = xor_block(ad_offset, load(adp+2));
				ad_offset = xor_block(ad_offset, ctx->L[2]);
				ta[3] = xor_block(ad_offset, load(adp+3));
				remaining -= 64;
				k=4;
			}
			#endif
			if (remaining >= 32) {
				ad_offset = xor_block(ad_offset, ctx->L[0]);
				ta[k] = xor_block(ad_offset, load(adp+k));
				ad_offset = xor_block(ad_offset, getL(ctx, ntz(k+2)));
				ta[k+1] = xor_block(ad_offset, load(adp+k+1));
				remaining -= 32;
				k+=2;
			}
			if (remaining >= 16) {
				ad_offset = xor_block(ad_offset, ctx->L[0]);
				ta[k] = xor_block(ad_offset, load(adp+k));
				remaining = remaining - 16;
				++k;
			}
			if (remaining) {
				ad_offset = xor_block(ad_offset,ctx->Lstar);
				tmp.bl = zero_block();
				memcpy(tmp.u8, adp+k, remaining);
				tmp.u8[remaining] = (unsigned char)0x80u;
				ta[k] = xor_block(ad_offset, tmp.bl);
				++k;
			}
			AES_ecb_encrypt_blks(ta,k,&ctx->encrypt_key);
			switch (k) {
				#if (BPI == 8)
				case 8: ad_checksum = xor_block(ad_checksum, ta[7]);
				case 7: ad_checksum = xor_block(ad_checksum, ta[6]);
				case 6: ad_checksum = xor_block(ad_checksum, ta[5]);
				case 5: ad_checksum = xor_block(ad_checksum, ta[4]);
				#endif
				case 4: ad_checksum = xor_block(ad_checksum, ta[3]);
				case 3: ad_checksum = xor_block(ad_checksum, ta[2]);
				case 2: ad_checksum = xor_block(ad_checksum, ta[1]);
				case 1: ad_checksum = xor_block(ad_checksum, ta[0]);
			}
			ctx->ad_checksum = ad_checksum;
		}
	}
}

/* ----------------------------------------------------------------------- */

int ae_encrypt(ae_ctx     *ctx,
               const void *nonce,
               const void *pt,
               int         pt_len,
               const void *ad,
               int         ad_len,
               void       *ct,
               void       *tag,
               int         final)
{
	union { uint32_t u32[4]; uint8_t u8[16]; block bl; } tmp;
    block offset, checksum;
    unsigned i, k;
    block       *ctp = (block *)ct;
    const block *ptp = (block *)pt;

    /* Non-null nonce means start of new message, init per-message values */
    if (nonce) {
        ctx->offset = gen_offset_from_nonce(ctx, nonce);
        ctx->ad_offset = ctx->checksum = zero_block();
        ctx->ad_blocks_processed = ctx->blocks_processed = 0;
        if (ad_len >= 0)
        	ctx->ad_checksum = zero_block();
    }
	/* Process associated data */
	if (ad_len > 0)
		process_ad(ctx, ad, ad_len, final);

	/* Encrypt plaintext data BPI blocks at a time */
    offset = ctx->offset;
    checksum  = ctx->checksum;
    i = pt_len/(BPI*16);
    if (i) {
    	block oa[BPI];
    	unsigned block_num = ctx->blocks_processed;
    	oa[BPI-1] = offset;
		do {
			block ta[BPI];
			block_num += BPI;
			oa[0] = xor_block(oa[BPI-1], ctx->L[0]);
			ta[0] = xor_block(oa[0], load(ptp+0));
			checksum = xor_block(checksum, load(ptp+0));
			oa[1] = xor_block(oa[0], ctx->L[1]);
			ta[1] = xor_block(oa[1], load(ptp+1));
			checksum = xor_block(checksum, load(ptp+1));
			oa[2] = xor_block(oa[1], ctx->L[0]);
			ta[2] = xor_block(oa[2], load(ptp+2));
			checksum = xor_block(checksum, load(ptp+2));
			#if BPI == 4
				oa[3] = xor_block(oa[2], getL(ctx, ntz(block_num)));
				ta[3] = xor_block(oa[3], load(ptp+3));
				checksum = xor_block(checksum, load(ptp+3));
			#elif BPI == 8
				oa[3] = xor_block(oa[2], ctx->L[2]);
				ta[3] = xor_block(oa[3], load(ptp+3));
				checksum = xor_block(checksum, load(ptp+3));
				oa[4] = xor_block(oa[1], ctx->L[2]);
				ta[4] = xor_block(oa[4], load(ptp+4));
				checksum = xor_block(checksum, load(ptp+4));
				oa[5] = xor_block(oa[0], ctx->L[2]);
				ta[5] = xor_block(oa[5], load(ptp+5));
				checksum = xor_block(checksum, load(ptp+5));
				oa[6] = xor_block(oa[7], ctx->L[2]);
				ta[6] = xor_block(oa[6], load(ptp+6));
				checksum = xor_block(checksum, load(ptp+6));
				oa[7] = xor_block(oa[6], getL(ctx, ntz(block_num)));
				ta[7] = xor_block(oa[7], load(ptp+7));
				checksum = xor_block(checksum, load(ptp+7));
			#endif
			AES_ecb_encrypt_blks(ta,BPI,&ctx->encrypt_key);
			store(ctp+0, xor_block(ta[0], oa[0]));
			store(ctp+1, xor_block(ta[1], oa[1]));
			store(ctp+2, xor_block(ta[2], oa[2]));
			store(ctp+3, xor_block(ta[3], oa[3]));
			#if (BPI == 8)
                store(ctp+4, xor_block(ta[4], oa[4]));
                store(ctp+5, xor_block(ta[5], oa[5]));
                store(ctp+6, xor_block(ta[6], oa[6]));
                store(ctp+7, xor_block(ta[7], oa[7]));
			#endif
			ptp += BPI;
			ctp += BPI;
		} while (--i);
    	ctx->offset = offset = oa[BPI-1];
	    ctx->blocks_processed = block_num;
		ctx->checksum = checksum;
    }

    if (final) {
		block ta[BPI+1], oa[BPI];

        /* Process remaining plaintext and compute its tag contribution    */
        unsigned remaining = ((unsigned)pt_len) % (BPI*16);
        k = 0;                      /* How many blocks in ta[] need ECBing */
        if (remaining) {
			#if (BPI == 8)
                if (remaining >= 64) {
                    oa[0] = xor_block(offset, ctx->L[0]);
                    ta[0] = xor_block(oa[0], load(ptp+0));
                    checksum = xor_block(checksum, load(ptp+0));
                    oa[1] = xor_block(oa[0], ctx->L[1]);
                    ta[1] = xor_block(oa[1], load(ptp+1));
                    checksum = xor_block(checksum, load(ptp+1));
                    oa[2] = xor_block(oa[1], ctx->L[0]);
                    ta[2] = xor_block(oa[2], load(ptp+2));
                    checksum = xor_block(checksum, load(ptp+2));
                    offset = oa[3] = xor_block(oa[2], ctx->L[2]);
                    ta[3] = xor_block(offset, load(ptp+3));
                    checksum = xor_block(checksum, load(ptp+3));
                    remaining -= 64;
                    k = 4;
                }
			#endif
			if (remaining >= 32) {
				oa[k] = xor_block(offset, ctx->L[0]);
				ta[k] = xor_block(oa[k], load(ptp+k));
				checksum = xor_block(checksum, load(ptp+k));
				offset = oa[k+1] = xor_block(oa[k], ctx->L[1]);
				ta[k+1] = xor_block(offset, load(ptp+k+1));
				checksum = xor_block(checksum, load(ptp+k+1));
				remaining -= 32;
				k+=2;
			}
			if (remaining >= 16) {
				offset = oa[k] = xor_block(offset, ctx->L[0]);
				ta[k] = xor_block(offset, load(ptp+k));
				checksum = xor_block(checksum, load(ptp+k));
				remaining -= 16;
				++k;
			}
			if (remaining) {
				tmp.bl = zero_block();
				memcpy(tmp.u8, ptp+k, remaining);
				tmp.u8[remaining] = (unsigned char)0x80u;
				checksum = xor_block(checksum, tmp.bl);
				ta[k] = offset = xor_block(offset,ctx->Lstar);
				++k;
			}
		}
        offset = xor_block(offset, ctx->Ldollar);      /* Part of tag gen */
        ta[k] = xor_block(offset, checksum);           /* Part of tag gen */
		AES_ecb_encrypt_blks(ta,k+1,&ctx->encrypt_key);
		offset = xor_block(ta[k], ctx->ad_checksum);   /* Part of tag gen */
		if (remaining) {
			--k;
			tmp.bl = xor_block(tmp.bl, ta[k]);
			memcpy(ctp+k, tmp.u8, remaining);
		}
		switch (k) {
			#if (BPI == 8)
                case 7: store(ctp+6, xor_block(ta[6], oa[6]));
                case 6: store(ctp+5, xor_block(ta[5], oa[5]));
                case 5: store(ctp+4, xor_block(ta[4], oa[4]));
                case 4: store(ctp+3, xor_block(ta[3], oa[3]));
			#endif
			case 3: store(ctp+2, xor_block(ta[2], oa[2]));
			case 2: store(ctp+1, xor_block(ta[1], oa[1]));
			case 1: store(ctp+0, xor_block(ta[0], oa[0]));
		}

        /* Tag is placed at the correct location
         */
        if (tag) {
			#if (OCB_TAG_LEN == 16)
            	store(tag, offset);
			#elif (OCB_TAG_LEN > 0)
	            memcpy((char *)tag, &offset, OCB_TAG_LEN);
			#else
	            memcpy((char *)tag, &offset, ctx->tag_len);
	        #endif
        } else {
			#if (OCB_TAG_LEN > 0)
	            memcpy((char *)ct + pt_len, &offset, OCB_TAG_LEN);
            	pt_len += OCB_TAG_LEN;
			#else
	            memcpy((char *)ct + pt_len, &offset, ctx->tag_len);
            	pt_len += ctx->tag_len;
	        #endif
        }
    }
    return (int) pt_len;
}

/* ----------------------------------------------------------------------- */

/* Compare two regions of memory, taking a constant amount of time for a
   given buffer size -- under certain assumptions about the compiler
   and machine, of course.

   Use this to avoid timing side-channel attacks.

   Returns 0 for memory regions with equal contents; non-zero otherwise. */
static int constant_time_memcmp(const void *av, const void *bv, int n) {
    const uint8_t *a = (const uint8_t *) av;
    const uint8_t *b = (const uint8_t *) bv;
    uint8_t result = 0;
    while (n >= 1) {
        n--;
        result |= a[n] ^ b[n];
    }
    return (int) result;
}

int ae_decrypt(ae_ctx     *ctx,
               const void *nonce,
               const void *ct,
               int         ct_len,
               const void *ad,
               int         ad_len,
               void       *pt,
               const void *tag,
               int         final)
{
	union { uint32_t u32[4]; uint8_t u8[16]; block bl; } tmp;
    block offset, checksum;
    unsigned i, k;
    block       *ctp = (block *)ct;
    block       *ptp = (block *)pt;

	/* Reduce ct_len tag bundled in ct */
	if ((final) && (!tag))
		#if (OCB_TAG_LEN > 0)
			ct_len -= OCB_TAG_LEN;
		#else
			ct_len -= ctx->tag_len;
		#endif

    /* Non-null nonce means start of new message, init per-message values */
    if (nonce) {
        ctx->offset = gen_offset_from_nonce(ctx, nonce);
        ctx->ad_offset = ctx->checksum = zero_block();
        ctx->ad_blocks_processed = ctx->blocks_processed = 0;
        if (ad_len >= 0)
        	ctx->ad_checksum = zero_block();
    }

	/* Process associated data */
	if (ad_len > 0)
		process_ad(ctx, ad, ad_len, final);

	/* Encrypt plaintext data BPI blocks at a time */
    offset = ctx->offset;
    checksum  = ctx->checksum;
    i = ct_len/(BPI*16);
    if (i) {
    	block oa[BPI];
    	unsigned block_num = ctx->blocks_processed;
    	oa[BPI-1] = offset;
		do {
			block ta[BPI];
			block_num += BPI;
			oa[0] = xor_block(oa[BPI-1], ctx->L[0]);
			ta[0] = xor_block(oa[0], load(ctp+0));
			oa[1] = xor_block(oa[0], ctx->L[1]);
			ta[1] = xor_block(oa[1], load(ctp+1));
			oa[2] = xor_block(oa[1], ctx->L[0]);
			ta[2] = xor_block(oa[2], load(ctp+2));
			#if BPI == 4
				oa[3] = xor_block(oa[2], getL(ctx, ntz(block_num)));
				ta[3] = xor_block(oa[3], load(ctp+3));
			#elif BPI == 8
				oa[3] = xor_block(oa[2], ctx->L[2]);
				ta[3] = xor_block(oa[3], load(ctp+3));
				oa[4] = xor_block(oa[1], ctx->L[2]);
				ta[4] = xor_block(oa[4], load(ctp+4));
				oa[5] = xor_block(oa[0], ctx->L[2]);
				ta[5] = xor_block(oa[5], load(ctp+5));
				oa[6] = xor_block(oa[7], ctx->L[2]);
				ta[6] = xor_block(oa[6], load(ctp+6));
				oa[7] = xor_block(oa[6], getL(ctx, ntz(block_num)));
				ta[7] = xor_block(oa[7], load(ctp+7));
			#endif
			AES_ecb_decrypt_blks(ta,BPI,&ctx->decrypt_key);
			store(ptp+0, xor_block(ta[0], oa[0]));
			checksum = xor_block(checksum, load(ptp+0));
			store(ptp+1, xor_block(ta[1], oa[1]));
			checksum = xor_block(checksum, load(ptp+1));
			store(ptp+2, xor_block(ta[2], oa[2]));
			checksum = xor_block(checksum, load(ptp+2));
			store(ptp+3, xor_block(ta[3], oa[3]));
			checksum = xor_block(checksum, load(ptp+3));
			#if (BPI == 8)
                store(ptp+4, xor_block(ta[4], oa[4]));
                checksum = xor_block(checksum, load(ptp+4));
                store(ptp+5, xor_block(ta[5], oa[5]));
                checksum = xor_block(checksum, load(ptp+5));
                store(ptp+6, xor_block(ta[6], oa[6]));
                checksum = xor_block(checksum, load(ptp+6));
                store(ptp+7, xor_block(ta[7], oa[7]));
                checksum = xor_block(checksum, load(ptp+7));
			#endif
			ptp += BPI;
			ctp += BPI;
		} while (--i);
    	ctx->offset = offset = oa[BPI-1];
	    ctx->blocks_processed = block_num;
		ctx->checksum = checksum;
    }

    if (final) {
		block ta[BPI+1], oa[BPI];

        /* Process remaining plaintext and compute its tag contribution    */
        unsigned remaining = ((unsigned)ct_len) % (BPI*16);
        k = 0;                      /* How many blocks in ta[] need ECBing */
        if (remaining) {
			#if (BPI == 8)
                if (remaining >= 64) {
                    oa[0] = xor_block(offset, ctx->L[0]);
                    ta[0] = xor_block(oa[0], load(ctp+0));
                    oa[1] = xor_block(oa[0], ctx->L[1]);
                    ta[1] = xor_block(oa[1], load(ctp+1));
                    oa[2] = xor_block(oa[1], ctx->L[0]);
                    ta[2] = xor_block(oa[2], load(ctp+2));
                    offset = oa[3] = xor_block(oa[2], ctx->L[2]);
                    ta[3] = xor_block(offset, load(ctp+3));
                    remaining -= 64;
                    k = 4;
                }
			#endif
			if (remaining >= 32) {
				oa[k] = xor_block(offset, ctx->L[0]);
				ta[k] = xor_block(oa[k], load(ctp+k));
				offset = oa[k+1] = xor_block(oa[k], ctx->L[1]);
				ta[k+1] = xor_block(offset, load(ctp+k+1));
				remaining -= 32;
				k+=2;
			}
			if (remaining >= 16) {
				offset = oa[k] = xor_block(offset, ctx->L[0]);
				ta[k] = xor_block(offset, load(ctp+k));
				remaining -= 16;
				++k;
			}
			if (remaining) {
				block pad;
				offset = xor_block(offset,ctx->Lstar);
				AES_encrypt((unsigned char *)&offset, tmp.u8, &ctx->encrypt_key);
				pad = tmp.bl;
				memcpy(tmp.u8,ctp+k,remaining);
				tmp.bl = xor_block(tmp.bl, pad);
				tmp.u8[remaining] = (unsigned char)0x80u;
				memcpy(ptp+k, tmp.u8, remaining);
				checksum = xor_block(checksum, tmp.bl);
			}
		}
		AES_ecb_decrypt_blks(ta,k,&ctx->decrypt_key);
		switch (k) {
			#if (BPI == 8)
                case 7: store(ptp+6, xor_block(ta[6], oa[6]));
                        checksum = xor_block(checksum, load(ptp+6));
                case 6: store(ptp+5, xor_block(ta[5], oa[5]));
                        checksum = xor_block(checksum, load(ptp+5));
                case 5: store(ptp+4, xor_block(ta[4], oa[4]));
                        checksum = xor_block(checksum, load(ptp+4));
                case 4: store(ptp+3, xor_block(ta[3], oa[3]));
                        checksum = xor_block(checksum, load(ptp+3));
			#endif
			case 3: store(ptp+2, xor_block(ta[2], oa[2]));
				    checksum = xor_block(checksum, load(ptp+2));
			case 2: store(ptp+1, xor_block(ta[1], oa[1]));
				    checksum = xor_block(checksum, load(ptp+1));
			case 1: store(ptp+0, xor_block(ta[0], oa[0]));
				    checksum = xor_block(checksum, load(ptp+0));
		}

		/* Calculate expected tag */
        offset = xor_block(offset, ctx->Ldollar);
        tmp.bl = xor_block(offset, checksum);
		AES_encrypt(tmp.u8, tmp.u8, &ctx->encrypt_key);
		tmp.bl = xor_block(tmp.bl, ctx->ad_checksum); /* Full tag */

		/* Compare with proposed tag, change ct_len if invalid */
		if ((OCB_TAG_LEN == 16) && tag) {
			if (unequal_blocks(tmp.bl, load(tag)))
				ct_len = AE_INVALID;
		} else {
			#if (OCB_TAG_LEN > 0)
				int len = OCB_TAG_LEN;
			#else
				int len = ctx->tag_len;
			#endif
			if (tag) {
				if (constant_time_memcmp(tag,tmp.u8,len) != 0)
					ct_len = AE_INVALID;
			} else {
				if (constant_time_memcmp((char *)ct + ct_len,tmp.u8,len) != 0)
					ct_len = AE_INVALID;
			}
		}
    }
    return ct_len;
 }

/* ----------------------------------------------------------------------- */
/* Test programs                                                           */
/* ----------------------------------------------------------------------- */

#if 0

#include <stdio.h>

/* If UNALIGNED==0, then all key/nonce/pt/ct/tag pointers are 16-byte aligned
 * for all calls during the test. Otherwise, key/nonce/tag are at odd addresses
 * and pt/ct are many different alignments over the test, including odd ones.
 */ 
#define UNALIGNED     1            /* MUST be 0 or 1 */

/* Attempt to generate RFC extended vector. If OCB_(TAG|LEN)_LEN are non-zero
/  the values passed in are ignored. */
static int ex_vector(int kbytes, int tbytes) {
    unsigned char *val_buf, *ct, *pt, *nonce, *tag, *key, *next;
    int i;
    ae_ctx *ctx = ae_allocate(NULL);

    if (OCB_TAG_LEN != 0) tbytes = OCB_TAG_LEN;
    if (OCB_KEY_LEN != 0) kbytes = OCB_KEY_LEN;

	#if _MSC_VER
	    val_buf = (unsigned char *)_aligned_malloc(22401,16) + UNALIGNED;
	    ct = (unsigned char *)_aligned_malloc(129+tbytes,16) + UNALIGNED;
	    pt = (unsigned char *)_aligned_malloc(129,16) + UNALIGNED;
	    nonce = (unsigned char *)_aligned_malloc(13,16) + UNALIGNED;
	    tag = (unsigned char *)_aligned_malloc(tbytes+1,16) + UNALIGNED;
	    key = (unsigned char *)_aligned_malloc(kbytes+1,16) + UNALIGNED;
	#else
		posix_memalign((void **)&val_buf,16,22401); val_buf += UNALIGNED;
		posix_memalign((void **)&ct,16,129+tbytes); ct += UNALIGNED;
		posix_memalign((void **)&pt,16,129);        pt += UNALIGNED;
		posix_memalign((void **)&nonce,16,13);      nonce += UNALIGNED;
		posix_memalign((void **)&tag,16,tbytes+1);  tag += UNALIGNED;
		posix_memalign((void **)&key,16,kbytes+1);  key += UNALIGNED;
	#endif

    next = val_buf;

    memset(key,0,kbytes);
    key[kbytes-1] = tbytes * 8;
    memset(nonce,0,11);
    nonce[11] = 1;
    memset(pt,0,128);
    ae_init(ctx, key, kbytes, 12, tbytes);

    /* RFC Vector test */
    for (i = 0; i < 128; i++) {
        #if UNALIGNED
            ae_encrypt(ctx,nonce,pt,i,pt,i,next,NULL,AE_FINALIZE);
        #else
            ae_encrypt(ctx,nonce,pt,i,pt,i,ct,NULL,AE_FINALIZE);
            memcpy(next,ct,(size_t)i+tbytes);
        #endif
        next = next+i+tbytes;
        nonce[11] += 1; if (nonce[11]==0) nonce[10] += 1;

        #if UNALIGNED
            ae_encrypt(ctx,nonce,pt,i,pt,0,next,NULL,AE_FINALIZE);
        #else
            ae_encrypt(ctx,nonce,pt,i,pt,0,ct,NULL,AE_FINALIZE);
            memcpy(next,ct,(size_t)i+tbytes);
        #endif
        next = next+i+tbytes;
        nonce[11] += 1; if (nonce[11]==0) nonce[10] += 1;

        #if UNALIGNED
            ae_encrypt(ctx,nonce,pt,0,pt,i,next,NULL,AE_FINALIZE);
        #else
            ae_encrypt(ctx,nonce,pt,0,pt,i,ct,NULL,AE_FINALIZE);
            memcpy(next,ct,tbytes);
        #endif
        next = next+tbytes;
        nonce[11] += 1; if (nonce[11]==0) nonce[10] += 1;
    }
    ae_encrypt(ctx,nonce,NULL,0,val_buf,next-val_buf,ct,tag,AE_FINALIZE);

	const char * tags[] = {
        "\x19\x2C\x9B\x7B\xD9\x0B\xA0\x6A",
        "\x77\xA3\xD8\xE7\x35\x89\x15\x8D\x25\xD0\x12\x09",
        "\x67\xE9\x44\xD2\x32\x56\xC5\xE0\xB6\xC6\x1F\xA2\x2F\xDF\x1E\xA2",
        "\x7D\x4E\xA5\xD4\x45\x50\x1C\xBE",
        "\x54\x58\x35\x9A\xC2\x3B\x0C\xBA\x9E\x63\x30\xDD",
        "\xD9\x0E\xB8\xE9\xC9\x77\xC8\x8B\x79\xDD\x79\x3D\x7F\xFA\x16\x1C",
    };
    unsigned tag_idx = ((kbytes/16)-1) * 3 + ((tbytes/4)-2);
    int result = (tag_idx < 6) && !memcmp(tag,tags[tag_idx],tbytes);

	ae_free(ctx);
	#if _MSC_VER
	    _aligned_free(val_buf - UNALIGNED);
	    _aligned_free(ct - UNALIGNED);
	    _aligned_free(pt - UNALIGNED);
	    _aligned_free(nonce - UNALIGNED);
	    _aligned_free(tag - UNALIGNED);
	    _aligned_free(key - UNALIGNED);
	#else
	    free(val_buf - UNALIGNED);
	    free(ct - UNALIGNED);
	    free(pt - UNALIGNED);
	    free(nonce - UNALIGNED);
	    free(tag - UNALIGNED);
	    free(key - UNALIGNED);
	#endif
	
    return result;
}

int main()
{
    printf("RFC vector %s\n",ex_vector(16,16)?"PASS":"FAIL");
    return 0;
}
#endif

#if USE_AES_INTRINSICS
char infoString[] = "OCB (AES Intrinsics)";
#elif OPENSSL_IS_AES_BACKUP
char infoString[] = "OCB (OpenSSL AES)";
#else
char infoString[] = "OCB (Reference AES)";
#endif

#if __GNUC__
#pragma GCC diagnostic pop
#endif