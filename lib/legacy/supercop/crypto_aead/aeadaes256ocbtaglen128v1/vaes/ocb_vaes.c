/*-------------------------------------------------------------------------
/ OCB (RFC 7253) Reference Code (Beta VAES)  Last modified 05-AUG-2020
/--------------------------------------------------------------------------
/ Copyright (c) 2020 Ted Krovetz.
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
/ Comments are welcome: Ted Krovetz <ted@krovetz.net>
/------------------------------------------------------------------------- */

/* ----------------------------------------------------------------------- */
/* Usage and implementation notes                                          */
/* ----------------------------------------------------------------------- */

/* Usage notes:
/  - This implementation requires Intel AVX2 and is optimized for VAES.
/    It will work with AES-NI instead, but is not optimized for it.
/  - Requires: 128 or 256 bit key, 12-byte nonce, 4, 8, 12, or 16 byte tag.
/  - Pointer alignment: loadu/storeu is used, so any pointer alignment is ok.
/  - When AE_PENDING is passed as the 'final' parameter of any function,
/    the length parameters must be a multiple of 256 bytes.
/  - Plaintext and ciphertext pointers may be equal (ie, plaintext gets
/    encrypted in-place), but no other pointers may be equal or overlap.
*/

/* ----------------------------------------------------------------------- */
/* User configuration options                                              */
/* ----------------------------------------------------------------------- */

/* Set the AES key length to use and length of authentication tag to produce.
/  Setting either to 0 requires the value be set at runtime via ae_init().
/  Some optimizations occur for each when set to a fixed value.            */
#define OCB_KEY_LEN         32  /* 0, 16 or 32. 0 means set in ae_init     */
#define OCB_TAG_LEN         16  /* 0, 4, 8, 12, 16. 0 means set in ae_init */

/* During encryption and decryption, various "L values" are required.
/  The L values can be precomputed during initialization (requiring extra
/  space in ae_ctx), generated as needed (slightly slowing encryption and
/  decryption), or some combination of the two. L_TABLE_SZ specifies how many
/  L values to precompute. L_TABLE_SZ must be at least 4. L_TABLE_SZ*16 bytes
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
#include <immintrin.h>

/* Compiler-specific intrinsics and fixes                                  */
#if _MSC_VER

	#define inline __inline
	#define restrict __restrict__
	#include <intrin.h>
	#pragma intrinsic(_BitScanForward, memcpy)
	static inline int ntz(unsigned long x){_BitScanForward(&x,x);return (int)x;}
	
#elif __GNUC__

	#define inline __inline__
	#define restrict __restrict__
	#define ntz(x) __builtin_ctz((unsigned)(x))

#else

#error -- If not on GCC or MSVC system, you need to define ntz

#endif

/* ----------------------------------------------------------------------- */
/* Data structures.                                                        */
/* ----------------------------------------------------------------------- */

#if (OCB_KEY_LEN == 0)
    typedef struct { __m128i rd_key[15]; int rounds; } AES_KEY;
    #define ROUNDS(ctx) ((ctx)->rounds)
#else
    typedef struct { __m128i rd_key[7+OCB_KEY_LEN/4]; } AES_KEY;
    #define ROUNDS(ctx) (6+OCB_KEY_LEN/4)
#endif

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
    __m128i offset;                          /* Memory correct               */
    __m128i checksum;                        /* Memory correct               */
    __m128i Lstar;                           /* Memory correct               */
    __m128i Ldollar;                         /* Memory correct               */
    __m128i L[L_TABLE_SZ];                   /* Memory correct               */
    __m128i ad_checksum;                     /* Memory correct               */
    __m128i ad_offset;                       /* Memory correct               */
    __m128i cached_Top;                      /* Memory correct               */
	uint64_t KtopStr[3];                     /* Register correct, each item  */
    int32_t ad_blocks_processed;
    int32_t blocks_processed;
    AES_KEY decrypt_key;
    AES_KEY encrypt_key;
    #if (OCB_TAG_LEN == 0)
    int tag_len;
    #endif
};

/* ----------------------------------------------------------------------- */
/* Utility functions on blocks.                                            */
/* ----------------------------------------------------------------------- */

#define xor128(x,y)       _mm_xor_si128(x,y)
#define xor128x3(x,y,z)   xor128(xor128(x,y),z)
#define xor128x4(w,x,y,z) xor128(xor128(w,x),xor128(y,z))
#define xor256(x,y)       _mm256_xor_si256(x,y)
#define xor256x3(x,y,z)   xor256(xor256(x,y),z)
#define xor256x4(w,x,y,z) xor256(xor256(w,x),xor256(y,z))
#define load128(p)        _mm_loadu_si128((__m128i *)(p))
#define store128(p,x)     _mm_storeu_si128((__m128i *)(p),x)
#define load256(p)        _mm256_loadu_si256((__m256i *)(p))
#define store256(p,x)     _mm256_storeu_si256((__m256i *)(p),x)
#define zero128           _mm_setzero_si128
#define zero256           _mm256_setzero_si256

#define equal_blocks(x,y) (_mm_movemask_epi8(_mm_cmpeq_epi8(x,y))==0xffff)

#define reverse_bytes(b) \
    _mm_shuffle_epi8(b,_mm_set_epi8(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15))

#define reverse_each_half(b) \
    _mm_shuffle_epi8(b,_mm_set_epi8(8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7))

static inline __m128i gen_offset(uint64_t KtopStr[3], int bot) {
    __m128i hi = load128(KtopStr+0);  /* hi = B A */
    __m128i lo = load128(KtopStr+1);  /* lo = C B */
    lo = xor128(_mm_slli_epi64(hi,bot), _mm_srli_epi64(lo,64-bot));
    return reverse_each_half(lo);
}

static inline __m128i double_block(__m128i bl) {
    __m128i mask = _mm_set_epi32(135,1,1,1);
    __m128i tmp = _mm_srai_epi32(bl, 31);
    tmp = _mm_and_si128(tmp, mask);
    tmp = _mm_shuffle_epi32(tmp, _MM_SHUFFLE(2,1,0,3));
    bl = _mm_slli_epi32(bl, 1);
    return xor128(bl,tmp);
}

/* ----------------------------------------------------------------------- */
/* AES 128 and 256 - Uses AES intrinsics                                   */
/* ----------------------------------------------------------------------- */

#define EXPAND_ASSIST(v1,v2,v3,v4,shuff_const,aes_const)                    \
    v2 = _mm_aeskeygenassist_si128(v4,aes_const);                           \
    v3 = _mm_castps_si128(_mm_shuffle_ps(_mm_castsi128_ps(v3),              \
                                         _mm_castsi128_ps(v1), 16));        \
    v1 = xor128(v1,v3);                                                     \
    v3 = _mm_castps_si128(_mm_shuffle_ps(_mm_castsi128_ps(v3),              \
                                         _mm_castsi128_ps(v1), 140));       \
    v1 = xor128(v1,v3);                                                     \
    v2 = _mm_shuffle_epi32(v2,shuff_const);                                 \
    v1 = xor128(v1,v2)

static void AES_128_Key_Expansion(const void *userkey, void *key) {
    __m128i x0, x1, x2 = zero128();
    x0 = load128(userkey);
    store128((__m128i *)key+0, x0);
    EXPAND_ASSIST(x0,x1,x2,x0,255,1);   store128((__m128i *)key+1, x0);
    EXPAND_ASSIST(x0,x1,x2,x0,255,2);   store128((__m128i *)key+2, x0);
    EXPAND_ASSIST(x0,x1,x2,x0,255,4);   store128((__m128i *)key+3, x0);
    EXPAND_ASSIST(x0,x1,x2,x0,255,8);   store128((__m128i *)key+4, x0);
    EXPAND_ASSIST(x0,x1,x2,x0,255,16);  store128((__m128i *)key+5, x0);
    EXPAND_ASSIST(x0,x1,x2,x0,255,32);  store128((__m128i *)key+6, x0);
    EXPAND_ASSIST(x0,x1,x2,x0,255,64);  store128((__m128i *)key+7, x0);
    EXPAND_ASSIST(x0,x1,x2,x0,255,128); store128((__m128i *)key+8, x0);
    EXPAND_ASSIST(x0,x1,x2,x0,255,27);  store128((__m128i *)key+9, x0);
    EXPAND_ASSIST(x0,x1,x2,x0,255,54);  store128((__m128i *)key+10, x0);
}

static void AES_256_Key_Expansion(const void *userkey, void *key)
{
    __m128i x0,x1,x3,x2 = zero128();
    x0 = load128(userkey);
    x3 = load128((__m128i *)userkey+1);
    store128((__m128i *)key+0, x0);
    store128((__m128i *)key+1, x3);
    EXPAND_ASSIST(x0,x1,x2,x3,255,1);  store128((__m128i *)key+2, x0);
    EXPAND_ASSIST(x3,x1,x2,x0,170,1);  store128((__m128i *)key+3, x3);
    EXPAND_ASSIST(x0,x1,x2,x3,255,2);  store128((__m128i *)key+4, x0);
    EXPAND_ASSIST(x3,x1,x2,x0,170,2);  store128((__m128i *)key+5, x3);
    EXPAND_ASSIST(x0,x1,x2,x3,255,4);  store128((__m128i *)key+6, x0);
    EXPAND_ASSIST(x3,x1,x2,x0,170,4);  store128((__m128i *)key+7, x3);
    EXPAND_ASSIST(x0,x1,x2,x3,255,8);  store128((__m128i *)key+8, x0);
    EXPAND_ASSIST(x3,x1,x2,x0,170,8);  store128((__m128i *)key+9, x3);
    EXPAND_ASSIST(x0,x1,x2,x3,255,16); store128((__m128i *)key+10, x0);
    EXPAND_ASSIST(x3,x1,x2,x0,170,16); store128((__m128i *)key+11, x3);
    EXPAND_ASSIST(x0,x1,x2,x3,255,32); store128((__m128i *)key+12, x0);
    EXPAND_ASSIST(x3,x1,x2,x0,170,32); store128((__m128i *)key+13, x3);
    EXPAND_ASSIST(x0,x1,x2,x3,255,64); store128((__m128i *)key+14, x0);
}

static int AES_set_keys(const void *userKey, const int bits,
                        AES_KEY *ekey, AES_KEY *dkey)
{
    int i = ROUNDS(ekey);
    int j = 0;
    
    /* Setup ekey */
    if (bits == 128)
        AES_128_Key_Expansion (userKey,ekey);
    else 
        AES_256_Key_Expansion (userKey,ekey);
    #if (OCB_KEY_LEN == 0)
    dkey->rounds = ekey->rounds = 6+bits/32;
    #endif

    /* Use inverse mixcolumns to convert ekey to dkey */
    store128(dkey->rd_key+i, load128(ekey->rd_key+j));
    i--; j++;
    for ( ; i>0; i--, j++)
        store128(dkey->rd_key+i, _mm_aesimc_si128(load128(ekey->rd_key+j)));
    store128(dkey->rd_key+i, load128(ekey->rd_key+j));
    return 0;
}

static void AES_encrypt(const void *in, void *out, const AES_KEY *key)
{
    int j, rnds = ROUNDS(key);
    const __m128i *kp = ((__m128i *)(key->rd_key));
    __m128i tmp = xor128(load128(in),load128(kp+0));
    for (j=1; j<rnds; j++)
        tmp = _mm_aesenc_si128(tmp,load128(kp+j));
    tmp = _mm_aesenclast_si128(tmp,load128(kp+rnds));
    store128(out, tmp);
}

#if __VAES__

static inline void AES_ecb_encrypt_blks_exp16(__m128i * restrict blks,
                                              const __m256i * restrict k256) {
    int i, rnds=ROUNDS(key);
    __m256i tmp[8];
    tmp[0] = xor256(load256((__m256i *)blks+0), k256[0]);
    tmp[1] = xor256(load256((__m256i *)blks+1), k256[0]);
    tmp[2] = xor256(load256((__m256i *)blks+2), k256[0]);
    tmp[3] = xor256(load256((__m256i *)blks+3), k256[0]);
    tmp[4] = xor256(load256((__m256i *)blks+4), k256[0]);
    tmp[5] = xor256(load256((__m256i *)blks+5), k256[0]);
    tmp[6] = xor256(load256((__m256i *)blks+6), k256[0]);
    tmp[7] = xor256(load256((__m256i *)blks+7), k256[0]);
    for(i=1; i<rnds; i++) {
        tmp[0] = _mm256_aesenc_epi128(tmp[0], k256[i]);
        tmp[1] = _mm256_aesenc_epi128(tmp[1], k256[i]);
        tmp[2] = _mm256_aesenc_epi128(tmp[2], k256[i]);
        tmp[3] = _mm256_aesenc_epi128(tmp[3], k256[i]);
        tmp[4] = _mm256_aesenc_epi128(tmp[4], k256[i]);
        tmp[5] = _mm256_aesenc_epi128(tmp[5], k256[i]);
        tmp[6] = _mm256_aesenc_epi128(tmp[6], k256[i]);
        tmp[7] = _mm256_aesenc_epi128(tmp[7], k256[i]);
    }
    store256((__m256i *)blks+0, _mm256_aesenclast_epi128(tmp[0], k256[rnds]));
    store256((__m256i *)blks+1, _mm256_aesenclast_epi128(tmp[1], k256[rnds]));
    store256((__m256i *)blks+2, _mm256_aesenclast_epi128(tmp[2], k256[rnds]));
    store256((__m256i *)blks+3, _mm256_aesenclast_epi128(tmp[3], k256[rnds]));
    store256((__m256i *)blks+4, _mm256_aesenclast_epi128(tmp[4], k256[rnds]));
    store256((__m256i *)blks+5, _mm256_aesenclast_epi128(tmp[5], k256[rnds]));
    store256((__m256i *)blks+6, _mm256_aesenclast_epi128(tmp[6], k256[rnds]));
    store256((__m256i *)blks+7, _mm256_aesenclast_epi128(tmp[7], k256[rnds]));
}

static inline void AES_ecb_encrypt_blks_exp17(__m128i * restrict blks,
                                              const __m256i * restrict k256) {
    int i, rnds=ROUNDS(key);
    __m256i tmp[9];
    tmp[0] = xor256(load256((__m256i *)blks+0), k256[0]);
    tmp[1] = xor256(load256((__m256i *)blks+1), k256[0]);
    tmp[2] = xor256(load256((__m256i *)blks+2), k256[0]);
    tmp[3] = xor256(load256((__m256i *)blks+3), k256[0]);
    tmp[4] = xor256(load256((__m256i *)blks+4), k256[0]);
    tmp[5] = xor256(load256((__m256i *)blks+5), k256[0]);
    tmp[6] = xor256(load256((__m256i *)blks+6), k256[0]);
    tmp[7] = xor256(load256((__m256i *)blks+7), k256[0]);
    tmp[8] = xor256(load256((__m256i *)blks+8), k256[0]);
    for(i=1; i<rnds; i++) {
        tmp[0] = _mm256_aesenc_epi128(tmp[0], k256[i]);
        tmp[1] = _mm256_aesenc_epi128(tmp[1], k256[i]);
        tmp[2] = _mm256_aesenc_epi128(tmp[2], k256[i]);
        tmp[3] = _mm256_aesenc_epi128(tmp[3], k256[i]);
        tmp[4] = _mm256_aesenc_epi128(tmp[4], k256[i]);
        tmp[5] = _mm256_aesenc_epi128(tmp[5], k256[i]);
        tmp[6] = _mm256_aesenc_epi128(tmp[6], k256[i]);
        tmp[7] = _mm256_aesenc_epi128(tmp[7], k256[i]);
        tmp[8] = _mm256_aesenc_epi128(tmp[8], k256[i]);
    }
    store256((__m256i *)blks+0, _mm256_aesenclast_epi128(tmp[0], k256[rnds]));
    store256((__m256i *)blks+1, _mm256_aesenclast_epi128(tmp[1], k256[rnds]));
    store256((__m256i *)blks+2, _mm256_aesenclast_epi128(tmp[2], k256[rnds]));
    store256((__m256i *)blks+3, _mm256_aesenclast_epi128(tmp[3], k256[rnds]));
    store256((__m256i *)blks+4, _mm256_aesenclast_epi128(tmp[4], k256[rnds]));
    store256((__m256i *)blks+5, _mm256_aesenclast_epi128(tmp[5], k256[rnds]));
    store256((__m256i *)blks+6, _mm256_aesenclast_epi128(tmp[6], k256[rnds]));
    store256((__m256i *)blks+7, _mm256_aesenclast_epi128(tmp[7], k256[rnds]));
    store256((__m256i *)blks+8, _mm256_aesenclast_epi128(tmp[8], k256[rnds]));
}

static inline void AES_ecb_decrypt_blks_exp16(__m128i * restrict blks,
                                              const __m256i * restrict k256) {
    int i, rnds=ROUNDS(key);
    __m256i tmp[8];
    tmp[0] = xor256(load256((__m256i *)blks+0), k256[0]);
    tmp[1] = xor256(load256((__m256i *)blks+1), k256[0]);
    tmp[2] = xor256(load256((__m256i *)blks+2), k256[0]);
    tmp[3] = xor256(load256((__m256i *)blks+3), k256[0]);
    tmp[4] = xor256(load256((__m256i *)blks+4), k256[0]);
    tmp[5] = xor256(load256((__m256i *)blks+5), k256[0]);
    tmp[6] = xor256(load256((__m256i *)blks+6), k256[0]);
    tmp[7] = xor256(load256((__m256i *)blks+7), k256[0]);
    for(i=1; i<rnds; i++) {
        tmp[0] = _mm256_aesdec_epi128(tmp[0], k256[i]);
        tmp[1] = _mm256_aesdec_epi128(tmp[1], k256[i]);
        tmp[2] = _mm256_aesdec_epi128(tmp[2], k256[i]);
        tmp[3] = _mm256_aesdec_epi128(tmp[3], k256[i]);
        tmp[4] = _mm256_aesdec_epi128(tmp[4], k256[i]);
        tmp[5] = _mm256_aesdec_epi128(tmp[5], k256[i]);
        tmp[6] = _mm256_aesdec_epi128(tmp[6], k256[i]);
        tmp[7] = _mm256_aesdec_epi128(tmp[7], k256[i]);
    }
    store256((__m256i *)blks+0, _mm256_aesdeclast_epi128(tmp[0], k256[rnds]));
    store256((__m256i *)blks+1, _mm256_aesdeclast_epi128(tmp[1], k256[rnds]));
    store256((__m256i *)blks+2, _mm256_aesdeclast_epi128(tmp[2], k256[rnds]));
    store256((__m256i *)blks+3, _mm256_aesdeclast_epi128(tmp[3], k256[rnds]));
    store256((__m256i *)blks+4, _mm256_aesdeclast_epi128(tmp[4], k256[rnds]));
    store256((__m256i *)blks+5, _mm256_aesdeclast_epi128(tmp[5], k256[rnds]));
    store256((__m256i *)blks+6, _mm256_aesdeclast_epi128(tmp[6], k256[rnds]));
    store256((__m256i *)blks+7, _mm256_aesdeclast_epi128(tmp[7], k256[rnds]));
}

#else

static inline void AES_ecb_encrypt_blks_exp(__m128i * restrict blks, int nblks,
                                            const __m256i * restrict k256) {
    int i,j,rnds=ROUNDS(key);
    __m128i tmp[17];
    for (i=0; i<nblks; ++i)
        tmp[i] = xor128(load128(blks+i), *(__m128i *)(k256+0));
    for(i=1; i<rnds; i++)
        for (j=0; j<nblks; j++)
            tmp[j] = _mm_aesenc_si128(tmp[j], *(__m128i *)(k256+i));
    for (i=0; i<nblks; ++i) {
        tmp[i] = _mm_aesenclast_si128(tmp[i], *(__m128i *)(k256+rnds));
        store128(blks+i, tmp[i]);
    }
}

static inline void AES_ecb_decrypt_blks_exp(__m128i * restrict blks, int nblks,
                                            const __m256i * restrict k256) {
    int i,j,rnds=ROUNDS(key);
    __m128i tmp[17];
    for (i=0; i<nblks; ++i)
        tmp[i] = xor128(load128(blks+i), *(__m128i *)(k256+0));
    for(i=1; i<rnds; i++)
        for (j=0; j<nblks; j++)
            tmp[j] = _mm_aesdec_si128(tmp[j], *(__m128i *)(k256+i));
    for (i=0; i<nblks; ++i) {
        tmp[i] = _mm_aesdeclast_si128(tmp[i], *(__m128i *)(k256+rnds));
        store128(blks+i, tmp[i]);
    }
}

#define AES_ecb_encrypt_blks_exp17(b,k) AES_ecb_encrypt_blks_exp(b,17,k)
#define AES_ecb_encrypt_blks_exp16(b,k) AES_ecb_encrypt_blks_exp(b,16,k)
#define AES_ecb_decrypt_blks_exp16(b,k) AES_ecb_decrypt_blks_exp(b,16,k)

#endif


/* ----------------------------------------------------------------------- */
/* L table lookup (or on-the-fly generation)                               */
/* ----------------------------------------------------------------------- */

#if L_TABLE_SZ_IS_ENOUGH
#define getL(_ctx, _tz) load128((_ctx)->L+(_tz))
#else
static __m128i getL(const ae_ctx *ctx, int tz)
{
    if (tz < L_TABLE_SZ)
        return load128(ctx->L+tz);
    else {
        int i;
        /* Bring L[MAX] into registers, make it register correct */
        __m128i rval = reverse_bytes(load128(ctx->L+L_TABLE_SZ-1));
        rval = double_block(rval);
        for (i=L_TABLE_SZ; i < tz; i++)
            rval = double_block(rval);
        return reverse_bytes(rval);             /* To memory correct */
    }
}
#endif

/* ----------------------------------------------------------------------- */
/* Public functions                                                        */
/* ----------------------------------------------------------------------- */

ae_ctx* ae_allocate(void *misc)
{
	(void)misc;                              /* Suppress "unused variable" */
    return (ae_ctx *)malloc(sizeof(ae_ctx));
}

void ae_free(ae_ctx *ctx)
{
    free(ctx);
}

/* ----------------------------------------------------------------------- */

int ae_clear (ae_ctx *ctx) /* Zero ae_ctx and undo initialization          */
{
	memset(ctx, 0, sizeof(ae_ctx));
	return AE_SUCCESS;
}

int ae_ctx_sizeof(void) { return (int) sizeof(ae_ctx); }

/* ----------------------------------------------------------------------- */

int ae_init(ae_ctx *ctx, const void *key, int key_len,
            int nonce_len, int tag_len)
{
    int i;
    __m128i tmp_blk;

    if ((nonce_len != 12) ||
        (OCB_KEY_LEN != 0 && key_len != OCB_KEY_LEN) ||
        (OCB_TAG_LEN != 0 && tag_len != OCB_TAG_LEN) ||
        (tag_len != 4 && tag_len != 8 && tag_len != 12 && tag_len != 16) ||
        (key_len != 16 && key_len != 32))
    	return AE_NOT_SUPPORTED;

    /* Initialize encryption & decryption keys */
    AES_set_keys(key, key_len*8, &ctx->encrypt_key, &ctx->decrypt_key);

    /* Zero things that need zeroing */
    store128(&ctx->cached_Top, zero128());
    store128(&ctx->ad_checksum, zero128());

    /* Compute key-dependent values */
    AES_encrypt(&ctx->cached_Top, &ctx->Lstar, &ctx->encrypt_key);
    tmp_blk = reverse_bytes(load128(&ctx->Lstar));
    tmp_blk = double_block(tmp_blk);
    store128(&ctx->Ldollar, reverse_bytes(tmp_blk));
    tmp_blk = double_block(tmp_blk);
    store128(ctx->L+0, reverse_bytes(tmp_blk));
    for (i = 1; i < L_TABLE_SZ; i++) {
		tmp_blk = double_block(tmp_blk);
    	store128(ctx->L+i, reverse_bytes(tmp_blk));
    }

    #if (OCB_TAG_LEN == 0)
    	ctx->tag_len = tag_len;
    #endif

    return AE_SUCCESS;
}

/* ----------------------------------------------------------------------- */

static __m128i gen_offset_from_nonce(ae_ctx *ctx, const void *nonce)
{
	union { uint32_t u32[4]; uint8_t u8[16]; __m128i bl; } tmp;
	int idx;
    #if (OCB_TAG_LEN > 0)
        unsigned tag_len = OCB_TAG_LEN;
    #else
        unsigned tag_len = ctx->tag_len;
    #endif
	/* Replace cached nonce Top if needed */
    tmp.u32[0] = 0x01000000u + ((tag_len * 8 % 128) << 1);
	memcpy(tmp.u32+1,nonce,12);
	idx = (int)(tmp.u8[15] & 0x3f);            /* Get low 6 bits of nonce  */
	tmp.u8[15] = tmp.u8[15] & 0xc0;            /* Zero low 6 bits of nonce */
	if ( ! equal_blocks(tmp.bl, load128(&ctx->cached_Top)) ) {  /* Cached? */
		store128(&ctx->cached_Top, tmp.bl);    /* Update cache, KtopStr    */
		AES_encrypt(tmp.u8, ctx->KtopStr, &ctx->encrypt_key);
        tmp.bl = load128((__m128i *)ctx->KtopStr);
        tmp.bl = reverse_each_half(tmp.bl);    /* Make Register Correct    */
        store128((__m128i *)ctx->KtopStr, tmp.bl);
		ctx->KtopStr[2] = ctx->KtopStr[0] ^
						 (ctx->KtopStr[0] << 8) ^ (ctx->KtopStr[1] >> 56);
	}
	return gen_offset(ctx->KtopStr, idx);
}

/* Masks are made and stored in an array. These symbols are array indices */
#define M01 0    /* Index for mask containing L[0] xor L[1]               */
#define M02 1    /* Index for mask containing L[0] xor L[2]               */
#define M03 2    /* Index for mask containing L[0] xor L[3]               */
#define M0_01 3  /* Index for mask containing (L[0], L[0] xor L[1])       */

static void process_ad(ae_ctx     * restrict ctx,
                       __m256i               k256[],
                       __m256i               m[],
                       const void * restrict ad,
                       int                   ad_len,
                       int                   final)
{
    int32_t i, tz, ad_blocks_processed;
    __m256i oa[8], ta[8], ad_checksum, next, tmp256;
    __m128i lo, hi, ad_offset, prev_offset, tmp128;

    /* Initialize local variables. Ctx will be updated at end, if final */
    ad_offset = load128(&ctx->ad_offset);
    ad_checksum = zero256();
    ad_blocks_processed = ctx->ad_blocks_processed;
    tz = ntz(ad_blocks_processed+16);

    /* Setup next 16 offsets based on prior one used. */
    oa[0] = xor256(_mm256_broadcastsi128_si256(ad_offset), m[M0_01]);
    oa[2] = xor256x3(oa[0], m[M01], m[M02]);
    oa[4] = xor256x3(oa[0], m[M02], m[M03]);
    oa[6] = xor256x3(oa[0], m[M01], m[M03]);
    oa[1] = xor256(oa[0], _mm256_blend_epi32(m[M01], m[M02], 0xf0));
    oa[3] = xor256(oa[2], _mm256_blend_epi32(m[M01], m[M03], 0xf0));
    oa[5] = xor256(oa[4], _mm256_blend_epi32(m[M01], m[M02], 0xf0));
    lo = xor128(_mm256_castsi256_si128(oa[6]), _mm256_castsi256_si128(m[M01]));
    hi = xor128(lo, getL(ctx, tz));
    oa[7] = _mm256_set_m128i(hi, lo);

    /* Do chunks of 256 bytes */
    while (ad_len >= 256) {
        ad_blocks_processed += 16;
        tz = ntz(ad_blocks_processed+16);
        /* Calc mask for updating offsets, save last offset used this round */
        tmp128 = _mm256_extracti128_si256(oa[7],1);
        next = _mm256_broadcastsi128_si256(xor128(tmp128, ad_offset));
        ad_offset = tmp128;
        /* Do a 256 byte round, update offsets for next round */
        ta[0] = xor256(oa[0], load256((__m256i *)ad+0));
        ta[1] = xor256(oa[1], load256((__m256i *)ad+1));
        ta[2] = xor256(oa[2], load256((__m256i *)ad+2));
        ta[3] = xor256(oa[3], load256((__m256i *)ad+3));
        ta[4] = xor256(oa[4], load256((__m256i *)ad+4));
        ta[5] = xor256(oa[5], load256((__m256i *)ad+5));
        ta[6] = xor256(oa[6], load256((__m256i *)ad+6));
        ta[7] = xor256(oa[7], load256((__m256i *)ad+7));
        oa[0] = xor256(oa[0], next);
        oa[1] = xor256(oa[1], next);
        oa[2] = xor256(oa[2], next);
        oa[3] = xor256(oa[3], next);
        oa[4] = xor256(oa[4], next);
        oa[5] = xor256(oa[5], next);
        oa[6] = xor256(oa[6], next);
        lo = xor128(_mm256_castsi256_si128(oa[7]), _mm256_castsi256_si128(next));
        hi = xor128(lo, getL(ctx, tz));
        oa[7] = _mm256_set_m128i(hi, lo);
        AES_ecb_encrypt_blks_exp16((__m128i *)ta,k256);
        tmp256 = xor256(xor256x4(ta[0],ta[1],ta[2],ta[3]),
                        xor256x4(ta[4],ta[5],ta[6],ta[7]));
        ad_checksum = xor256(ad_checksum, tmp256);
        ad = (char *)ad + 256;
        ad_len = ad_len - 256;
    }
    	
    /* a non-multiple of 256 is only allowed when final. Process any tail */
    if (final) {
        int blks_remaining = (ad_len + 15) / 16;  /* Blocks after any padding */
        int extra_bytes = ad_len % 32;            /* Needs extra handling     */
        /* If ad_len is < 16, we need oa[7] hi to hold previous offset */
        oa[7] = _mm256_inserti128_si256(oa[7], ad_offset, 1);
        /* Any full 32-byte chunks are handled easily. */
        for (i=0; i<ad_len/32; i++)
            ta[i] = xor256(oa[i], load256((__m256i *)ad+i));
        /* i is now index of next available ta */

        /* If bytes remain, process */
        if (extra_bytes) {
            if (extra_bytes == 16)
                ta[i] = xor256(oa[i],
                            _mm256_castsi128_si256(load128((__m256i *)ad+i)));
            else if (extra_bytes < 16) {
                ta[i] = zero256();
                memcpy(ta+i, (__m256i *)ad+i, (unsigned)extra_bytes);
                ((uint8_t *)(ta+i))[extra_bytes] = 0x80;
                /* (i+7)%8 is index of prior offset (-1 == +7 mod 8) */
                prev_offset = _mm256_extracti128_si256(oa[(i+7)%8],1);
                ta[i] = xor256(ta[i], _mm256_castsi128_si256(
                                    xor128(load128(&ctx->Lstar), prev_offset)));
            } else {
                prev_offset = _mm256_castsi256_si128(oa[i]);
                lo = xor128(prev_offset, load128((__m256i *)ad+i));
                hi = zero128();
                memcpy(&hi, (__m128i *)ad+i*2+1, (unsigned)extra_bytes-16);
                ((uint8_t *)(&hi))[extra_bytes-16] = 0x80;
                hi = xor128x3(hi, prev_offset, load128(&ctx->Lstar));
                ta[i] = _mm256_set_m128i(hi, lo);
            }
        }
        /* Encipher prepared blocks, xor together with ctx->ad_checksum */
        AES_ecb_encrypt_blks_exp16((__m128i *)ta, k256);
        for (i=0; i < blks_remaining/2; i++)
            ad_checksum = xor256(ad_checksum, ta[i]);
        tmp128 = xor128(_mm256_extracti128_si256(ad_checksum,1),
                        _mm256_castsi256_si128(ad_checksum));
        if (blks_remaining%2 != 0)
            tmp128 = xor128(tmp128, _mm256_castsi256_si128(ta[i]));
        store128(&ctx->ad_checksum, xor128(tmp128, load128(&ctx->ad_checksum)));
    } else {
        /* Not final, so update ctx with changes */
        store128(&ctx->ad_offset, ad_offset);
        ctx->ad_blocks_processed = ad_blocks_processed;
        tmp128 = xor128(_mm256_extracti128_si256(ad_checksum,1),
                        _mm256_castsi256_si128(ad_checksum));
        store128(&ctx->ad_checksum, xor128(tmp128, load128(&ctx->ad_checksum)));
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
#if (OCB_KEY_LEN == 0)
    __m256i k256[15];
#else
    __m256i k256[7+OCB_KEY_LEN/4];
#endif
    __m256i oa[8], ta[9], m[4], next, checksum;        /* ta[8] for tag gen */
    __m128i prev_offset, tag_result, tail, hi, lo, tmp128, offset, checksum128;
    int32_t i, tz, blocks_processed, orig_pt_len = pt_len;

    /* VAES needs duplicate keys, first 16 offsets created with aid of masks. */
    for (i=0; i<=ROUNDS(ctx->encrypt_key); i++)
        k256[i] = _mm256_broadcastsi128_si256(load128(ctx->encrypt_key.rd_key+i));
    m[M01] = _mm256_broadcastsi128_si256(xor128(load128(ctx->L+0), load128(ctx->L+1)));
    m[M02] = _mm256_broadcastsi128_si256(xor128(load128(ctx->L+0), load128(ctx->L+2)));
    m[M03] = _mm256_broadcastsi128_si256(xor128(load128(ctx->L+0), load128(ctx->L+3)));
    m[M0_01] = _mm256_set_m128i(xor128(load128(ctx->L+0), load128(ctx->L+1)), load128(ctx->L+0));

    /* Non-null nonce means start of new message, init per-message values */
    if (nonce) {
        offset = gen_offset_from_nonce(ctx, nonce);
        checksum = zero256();
        ctx->ad_blocks_processed = blocks_processed = 0;
        store128(&ctx->ad_offset, zero128());
        if (ad_len >= 0)
        	store128(&ctx->ad_checksum, zero128());
    } else {
        blocks_processed = ctx->blocks_processed;
        offset = load128(&ctx->offset);
        checksum = _mm256_set_m128i(zero128(), load128(&ctx->checksum));
    }
	/* Process associated data */
	if (ad_len > 0)
		process_ad(ctx, k256, m, ad, ad_len, final);

    /* Setup first 16 offsets based on last one used. */
    tz = ntz(blocks_processed+16);
    oa[0] = xor256(_mm256_broadcastsi128_si256(offset), m[M0_01]);
    oa[2] = xor256x3(oa[0], m[M01], m[M02]);
    oa[4] = xor256x3(oa[0], m[M02], m[M03]);
    oa[6] = xor256x3(oa[0], m[M01], m[M03]);
    oa[1] = xor256(oa[0], _mm256_blend_epi32(m[M01], m[M02], 0xf0));
    oa[3] = xor256(oa[2], _mm256_blend_epi32(m[M01], m[M03], 0xf0));
    oa[5] = xor256(oa[4], _mm256_blend_epi32(m[M01], m[M02], 0xf0));
    lo = xor128(_mm256_castsi256_si128(oa[6]), _mm256_castsi256_si128(m[M01]));
    hi = xor128(lo, getL(ctx, tz));
    oa[7] = _mm256_set_m128i(hi, lo);

    /* Do chunks of 256 bytes */
    while (pt_len >= 256) {
        blocks_processed += 16;
        tz = ntz(blocks_processed+16);
        /* Calc mask for updating offsets, save last offset used this round */
        tmp128 = _mm256_extracti128_si256(oa[7],1);
        next = _mm256_broadcastsi128_si256(xor128(tmp128, offset));
        offset = tmp128;
        /* Do a 256 byte round, update offsets for next round */
        ta[0] = xor256(oa[0], load256((__m256i *)pt+0));
        ta[1] = xor256(oa[1], load256((__m256i *)pt+1));
        ta[2] = xor256(oa[2], load256((__m256i *)pt+2));
        ta[3] = xor256(oa[3], load256((__m256i *)pt+3));
        ta[4] = xor256(oa[4], load256((__m256i *)pt+4));
        ta[5] = xor256(oa[5], load256((__m256i *)pt+5));
        ta[6] = xor256(oa[6], load256((__m256i *)pt+6));
        ta[7] = xor256(oa[7], load256((__m256i *)pt+7));
        checksum = xor256(checksum, load256((__m256i *)pt+0));
        checksum = xor256(checksum, load256((__m256i *)pt+1));
        checksum = xor256(checksum, load256((__m256i *)pt+2));
        checksum = xor256(checksum, load256((__m256i *)pt+3));
        checksum = xor256(checksum, load256((__m256i *)pt+4));
        checksum = xor256(checksum, load256((__m256i *)pt+5));
        checksum = xor256(checksum, load256((__m256i *)pt+6));
        checksum = xor256(checksum, load256((__m256i *)pt+7));
        AES_ecb_encrypt_blks_exp16((__m128i *)ta,k256);
        store256((__m256i *)ct+0, xor256(ta[0], oa[0]));
        store256((__m256i *)ct+1, xor256(ta[1], oa[1]));
        store256((__m256i *)ct+2, xor256(ta[2], oa[2]));
        store256((__m256i *)ct+3, xor256(ta[3], oa[3]));
        store256((__m256i *)ct+4, xor256(ta[4], oa[4]));
        store256((__m256i *)ct+5, xor256(ta[5], oa[5]));
        store256((__m256i *)ct+6, xor256(ta[6], oa[6]));
        store256((__m256i *)ct+7, xor256(ta[7], oa[7]));
        oa[0] = xor256(oa[0], next);
        oa[1] = xor256(oa[1], next);
        oa[2] = xor256(oa[2], next);
        oa[3] = xor256(oa[3], next);
        oa[4] = xor256(oa[4], next);
        oa[5] = xor256(oa[5], next);
        oa[6] = xor256(oa[6], next);
        lo = xor128(_mm256_castsi256_si128(oa[7]), _mm256_castsi256_si128(next));
        hi = xor128(lo, getL(ctx, tz));
        oa[7] = _mm256_set_m128i(hi, lo);

        pt = (char *)pt + 256;
        ct = (char *)ct + 256;
        pt_len = pt_len - 256;
	}

    /* If final, encrypt any tail and generate tag. Quite different cases   */
    /* depending on extra_bytes of 0, 1-15, 16, or 17-31.                   */
    if (final) {
        int extra_bytes = pt_len % 32;   /* Bytes left after 32-byte chunks */
        /* If pt_len is < 16, we need oa[7] hi to hold previous offset */
        oa[7] = _mm256_inserti128_si256(oa[7], offset, 1);
        /* Any full 32-byte chunks are handled easily. */
        for (i=0; i<pt_len/32; i++) {             /* Pre-process AVX blocks */
            ta[i] = xor256(oa[i], load256((__m256i *)pt+i));
            checksum = xor256(checksum, load256((__m256i *)pt+i));
        }
        /* i is now index of next available ta */

        /* If bytes remain, process */
        checksum128 = xor128(_mm256_extracti128_si256(checksum,1),
                             _mm256_castsi256_si128(checksum));
        if (extra_bytes == 0) {
            prev_offset = _mm256_extracti128_si256(oa[(i+7)%8],1);
            ta[i] = _mm256_castsi128_si256(xor128x3(checksum128, prev_offset, load128(&ctx->Ldollar)));
        } else if (extra_bytes == 16) {
            prev_offset = _mm256_castsi256_si128(oa[i]);
            tail = load128((__m256i *)pt+i);
            lo = xor128(tail, prev_offset);
            hi = xor128x4(tail, checksum128, prev_offset, load128(&ctx->Ldollar));
            ta[i] = _mm256_set_m128i(hi, lo);
        } else if (extra_bytes < 16) {
            prev_offset = xor128(_mm256_extracti128_si256(oa[(i+7)%8],1), load128(&ctx->Lstar));
            tail = zero128();
            memcpy(&tail, (__m256i *)pt+i, (unsigned)extra_bytes);
            ((uint8_t *)(&tail))[extra_bytes] = 0x80;
            hi = xor128x4(tail, checksum128, prev_offset, load128(&ctx->Ldollar));
            ta[i] = _mm256_set_m128i(hi, prev_offset);
        } else {
            lo = load128((__m256i *)pt+i);
            tail = zero128();
            memcpy(&tail, (__m128i *)pt+i*2+1, (unsigned)extra_bytes-16);
            ((unsigned char *)&tail)[extra_bytes-16] = 0x80u;
            checksum128 = xor128x3(checksum128, lo, tail);
            prev_offset = _mm256_castsi256_si128(oa[i]);
            ta[i] = _mm256_set_m128i(xor128(prev_offset, load128(&ctx->Lstar)), xor128(lo, prev_offset));
            ta[i+1] = _mm256_castsi128_si256(xor128x4(prev_offset, load128(&ctx->Lstar), load128(&ctx->Ldollar), checksum128));
        }
        /* Encipher prepared blocks. Always 17 blocks avoids conditionals*/
        AES_ecb_encrypt_blks_exp17((__m128i *)ta, k256);

        /* Write out full completed 32-byte chunks. */
        for (i=0; i<pt_len/32; i++)
            store256((__m256i *)ct+i, xor256(ta[i], oa[i]));

        /* Write out any remaining tail, extract tag result. */
        if (extra_bytes == 0) {
            tag_result = _mm256_castsi256_si128(ta[i]);
        } else if (extra_bytes == 16) {
            store128((__m256i *)ct+i,
                     _mm256_castsi256_si128(xor256(ta[i], oa[i])));
            tag_result = _mm256_extracti128_si256(ta[i],1);
        } else if (extra_bytes < 16) {
            tail = xor128(tail, _mm256_castsi256_si128(ta[i]));
            memcpy((__m256i *)ct+i, &tail, (unsigned)extra_bytes);
            tag_result = _mm256_extracti128_si256(ta[i],1);
        } else {
            store128((__m256i *)ct+i,
                     _mm256_castsi256_si128(xor256(ta[i], oa[i])));
            tail = xor128(tail, _mm256_extracti128_si256(ta[i],1));
            memcpy((__m128i *)ct+i*2+1, &tail, (unsigned)extra_bytes-16);
            tag_result = _mm256_castsi256_si128(ta[i+1]);
        }
        tag_result = xor128(tag_result, load128(&ctx->ad_checksum));

        /* Tag is placed at the correct location */
        if (tag == 0) {
            tag = (char *)ct + pt_len;
			#if (OCB_TAG_LEN > 0)
            	orig_pt_len += OCB_TAG_LEN;
			#else
            	orig_pt_len += ctx->tag_len;
	        #endif
        }
        #if (OCB_TAG_LEN == 16)
            store128(tag, tag_result);
        #elif (OCB_TAG_LEN > 0)
            memcpy((char *)tag, &tag_result, OCB_TAG_LEN);
        #else
            memcpy((char *)tag, &tag_result, ctx->tag_len);
        #endif
    } else {
        checksum128 = xor128(_mm256_extracti128_si256(checksum,1),
                             _mm256_castsi256_si128(checksum));
        store128(&ctx->checksum, checksum128);
        store128(&ctx->offset, offset);
        ctx->blocks_processed = blocks_processed;
    }
    return orig_pt_len;
}


/* ----------------------------------------------------------------------- */

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
#if (OCB_KEY_LEN == 0)
    __m256i k256[15];
#else
    __m256i k256[7+OCB_KEY_LEN/4];
#endif
    __m256i oa[8], ta[8], m[4], next, checksum;
    __m128i prev_offset, tag_result, tail, hi, lo, tmp128, offset, checksum128;
    int32_t i, tz, blocks_processed, orig_ct_len;

	/* Reduce ct_len tag bundled in ct */
	if (final && tag==0)
		#if (OCB_TAG_LEN > 0)
			ct_len -= OCB_TAG_LEN;
		#else
			ct_len -= ctx->tag_len;
		#endif
	orig_ct_len = ct_len;

    /* VAES needs duplicate keys, first 16 offsets created with aid of masks. */
    m[M01] = _mm256_broadcastsi128_si256(xor128(load128(ctx->L+0), load128(ctx->L+1)));
    m[M02] = _mm256_broadcastsi128_si256(xor128(load128(ctx->L+0), load128(ctx->L+2)));
    m[M03] = _mm256_broadcastsi128_si256(xor128(load128(ctx->L+0), load128(ctx->L+3)));
    m[M0_01] = _mm256_set_m128i(xor128(load128(ctx->L+0), load128(ctx->L+1)),
                                load128(ctx->L+0));

    /* Non-null nonce means start of new message, init per-message values */
    if (nonce) {
        offset = gen_offset_from_nonce(ctx, nonce);
        checksum = zero256();
        ctx->ad_blocks_processed = blocks_processed = 0;
        store128(&ctx->ad_offset, zero128());
        if (ad_len >= 0)
        	store128(&ctx->ad_checksum, zero128());
    } else {
        blocks_processed = ctx->blocks_processed;
        offset = load128(&ctx->offset);
        checksum = _mm256_set_m128i(zero128(), load128(&ctx->checksum));
    }
	/* Process associated data. Since AD uses encryption keys, prepare them */
	if (ad_len > 0) {
        for (i=0; i<=ROUNDS(ctx->encrypt_key); i++)
            k256[i] = _mm256_broadcastsi128_si256(load128(ctx->encrypt_key.rd_key+i));
		process_ad(ctx, k256, m, ad, ad_len, final);
	}
    /* Switch key from encryption key for AD to decryption key for decryption */
    for (i=0; i<=ROUNDS(ctx->decrypt_key); i++)
        k256[i] = _mm256_broadcastsi128_si256(load128(ctx->decrypt_key.rd_key+i));

    /* Setup next 16 offsets based on last one used. */
    tz = ntz(blocks_processed+16);
    oa[0] = xor256(_mm256_broadcastsi128_si256(offset), m[M0_01]);
    oa[2] = xor256x3(oa[0], m[M01], m[M02]);
    oa[4] = xor256x3(oa[0], m[M02], m[M03]);
    oa[6] = xor256x3(oa[0], m[M01], m[M03]);
    oa[1] = xor256(oa[0], _mm256_blend_epi32(m[M01], m[M02], 0xf0));
    oa[3] = xor256(oa[2], _mm256_blend_epi32(m[M01], m[M03], 0xf0));
    oa[5] = xor256(oa[4], _mm256_blend_epi32(m[M01], m[M02], 0xf0));
    lo = xor128(_mm256_castsi256_si128(oa[6]), _mm256_castsi256_si128(m[M01]));
    hi = xor128(lo, getL(ctx, tz));
    oa[7] = _mm256_set_m128i(hi, lo);

    /* Do chunks of 256 bytes */
    while (ct_len >= 256) {
        blocks_processed += 16;
        tz = ntz(blocks_processed+16);
        /* Calc mask for updating offsets, save last offset used this round */
        tmp128 = _mm256_extracti128_si256(oa[7],1);
        next = _mm256_broadcastsi128_si256(xor128(tmp128, offset));
        offset = tmp128;
        /* Do a 256 byte round, update offsets for next round */
        ta[0] = xor256(oa[0], load256((__m256i *)ct+0));
        ta[1] = xor256(oa[1], load256((__m256i *)ct+1));
        ta[2] = xor256(oa[2], load256((__m256i *)ct+2));
        ta[3] = xor256(oa[3], load256((__m256i *)ct+3));
        ta[4] = xor256(oa[4], load256((__m256i *)ct+4));
        ta[5] = xor256(oa[5], load256((__m256i *)ct+5));
        ta[6] = xor256(oa[6], load256((__m256i *)ct+6));
        ta[7] = xor256(oa[7], load256((__m256i *)ct+7));
        AES_ecb_decrypt_blks_exp16((__m128i *)ta,k256);
        store256((__m256i *)pt+0, xor256(ta[0], oa[0]));
        checksum = xor256(checksum, load256((__m256i *)pt+0));
        store256((__m256i *)pt+1, xor256(ta[1], oa[1]));
        checksum = xor256(checksum, load256((__m256i *)pt+1));
        store256((__m256i *)pt+2, xor256(ta[2], oa[2]));
        checksum = xor256(checksum, load256((__m256i *)pt+2));
        store256((__m256i *)pt+3, xor256(ta[3], oa[3]));
        checksum = xor256(checksum, load256((__m256i *)pt+3));
        store256((__m256i *)pt+4, xor256(ta[4], oa[4]));
        checksum = xor256(checksum, load256((__m256i *)pt+4));
        store256((__m256i *)pt+5, xor256(ta[5], oa[5]));
        checksum = xor256(checksum, load256((__m256i *)pt+5));
        store256((__m256i *)pt+6, xor256(ta[6], oa[6]));
        checksum = xor256(checksum, load256((__m256i *)pt+6));
        store256((__m256i *)pt+7, xor256(ta[7], oa[7]));
        checksum = xor256(checksum, load256((__m256i *)pt+7));
        oa[0] = xor256(oa[0], next);
        oa[1] = xor256(oa[1], next);
        oa[2] = xor256(oa[2], next);
        oa[3] = xor256(oa[3], next);
        oa[4] = xor256(oa[4], next);
        oa[5] = xor256(oa[5], next);
        oa[6] = xor256(oa[6], next);
        lo = xor128(_mm256_castsi256_si128(oa[7]), _mm256_castsi256_si128(next));
        hi = xor128(lo, getL(ctx, tz));
        oa[7] = _mm256_set_m128i(hi, lo);

        pt = (char *)pt + 256;
        ct = (char *)ct + 256;
        ct_len = ct_len - 256;
	}

    /* If final, decrypt any tail and generate tag. Quite different cases   */
    /* depending on extra_bytes of 0, 1-15, 16, or 17-31.                   */
    if (final) {
        int extra_bytes = ct_len % 32;            /* Left after AVX blocks  */
        /* If ct_len is < 16, we need oa[7] hi to hold previous offset */
        oa[7] = _mm256_inserti128_si256(oa[7], offset, 1);
        /* Any full 32-byte chunks are handled easily. */
        for (i=0; i<ct_len/32; i++)               /* Pre-process AVX blocks */
            ta[i] = xor256(oa[i], load256((__m256i *)ct+i));
        /* i is now index of next available ta */

        /* If another 16-byte block available, handle it too.               */
        if (extra_bytes >= 16) {
            offset = _mm256_castsi256_si128(oa[i]);
            tail = load128((__m256i *)ct+i);
            ta[i] = _mm256_castsi128_si256(xor128(tail, offset));
        }
        /* Decipher prepared blocks. Always 16 blocks avoids conditionals*/
        AES_ecb_decrypt_blks_exp16((__m128i *)ta, k256);

        /* Write out full completed 32-byte chunks. */
        for (i=0; i<ct_len/32; i++) {
            ta[i] = xor256(ta[i], oa[i]);
            store256((__m256i *)pt+i, ta[i]);
            checksum = xor256(checksum, ta[i]);
        }
        /* i is now index of the tail, if there is one */
        pt = (char *)pt + sizeof(__m256i) * i;
        ct = (char *)ct + sizeof(__m256i) * i;
        ct_len = ct_len - sizeof(__m256i) * i;
        /* Note ct_len == extra_bytes at this point */
        /* Handle 0-31 byte remainder, and calculate tag */
        checksum128 = xor128(_mm256_extracti128_si256(checksum,1),
                             _mm256_castsi256_si128(checksum));
        /* If another 16-byte block available, process and update vars.     */
        if (extra_bytes >= 16) {
            tmp128 = xor128(_mm256_castsi256_si128(ta[i]), offset);
            store128(pt, tmp128);
            checksum128 = xor128(checksum128, tmp128);
            prev_offset = offset;
            pt = (char *)pt + sizeof(__m128i);
            ct = (char *)ct + sizeof(__m128i);
            ct_len = ct_len - sizeof(__m128i);
        } else
            prev_offset = _mm256_extracti128_si256(oa[(i+7)%8],1);
        if (ct_len > 0) {
            prev_offset = xor128(prev_offset, load128(&ctx->Lstar));
            AES_encrypt((unsigned char *)&prev_offset, &tmp128, &ctx->encrypt_key);
            tail = tmp128;
            memcpy(&tail, ct, ct_len);
            tail = xor128(tail, tmp128); /* Leaves bytes beyond data zero */
            ((uint8_t *)(&tail))[ct_len] = 0x80;
            memcpy(pt, &tail, ct_len);
            checksum128 = xor128(checksum128, tail);
        }
        tmp128 = xor128x3(checksum128, load128(&ctx->Ldollar), prev_offset);
        AES_encrypt((unsigned char *)&tmp128, &tag_result, &ctx->encrypt_key);
        tag_result = xor128(tag_result, load128(&ctx->ad_checksum));

		/* Compare with proposed tag, change ct_len if invalid */
		if (tag == 0)
		    tag = (char *)ct + ct_len;
		if (OCB_TAG_LEN == 16 && ! equal_blocks(tag_result, load128(tag)))
				orig_ct_len = AE_INVALID;
		else {
			#if (OCB_TAG_LEN > 0)
				int len = OCB_TAG_LEN;
			#else
				int len = ctx->tag_len;
			#endif
			uint32_t sum = 0;
			for (i=0; i<len/32; i++)
			    sum = sum | (((uint32_t *)&tag_result)[i] ^ ((uint32_t *)tag)[i]);
			if (sum != 0)
			    orig_ct_len = AE_INVALID;
		}
    } else {
        checksum128 = xor128(_mm256_extracti128_si256(checksum,1),
                             _mm256_castsi256_si128(checksum));
        store128(&ctx->checksum, checksum128);
        store128(&ctx->offset, offset);
        ctx->blocks_processed = blocks_processed;
    }
    return orig_ct_len;
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
#define UNALIGNED     0            /* MUST be 0 or 1 */

/* Attempt to generate RFC extended vector. If OCB_(TAG|LEN)_LEN are non-zero
/  the values passed in are ignored. */
static int ex_vector(int kbytes, int tbytes) {
	const char * tags[] = {
        "\x19\x2C\x9B\x7B\xD9\x0B\xA0\x6A",
        "\x77\xA3\xD8\xE7\x35\x89\x15\x8D\x25\xD0\x12\x09",
        "\x67\xE9\x44\xD2\x32\x56\xC5\xE0\xB6\xC6\x1F\xA2\x2F\xDF\x1E\xA2",
        "\x7D\x4E\xA5\xD4\x45\x50\x1C\xBE",
        "\x54\x58\x35\x9A\xC2\x3B\x0C\xBA\x9E\x63\x30\xDD",
        "\xD9\x0E\xB8\xE9\xC9\x77\xC8\x8B\x79\xDD\x79\x3D\x7F\xFA\x16\x1C",
    };
    unsigned char *val_buf, *ct, *pt, *nonce, *tag, *key, *next;
    int i, result, tag_idx;
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
		posix_memalign((void **)&ct,16,129+(unsigned)tbytes); ct += UNALIGNED;
		posix_memalign((void **)&pt,16,129);        pt += UNALIGNED;
		posix_memalign((void **)&nonce,16,13);      nonce += UNALIGNED;
		posix_memalign((void **)&tag,16,(unsigned)tbytes+1);  tag += UNALIGNED;
		posix_memalign((void **)&key,16,(unsigned)kbytes+1);  key += UNALIGNED;
	#endif

    next = val_buf;

    memset(key,0,(unsigned)kbytes);
    key[kbytes-1] = (unsigned char)(tbytes * 8);
    memset(nonce,0,11);
    nonce[11] = 1;
    memset(pt,0,128);
    ae_init(ctx, key, kbytes, 12, tbytes);

    /* RFC Vector test */
    for (i = 0; i <= 127; i++) {
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
    ae_encrypt(ctx,nonce,NULL,0,val_buf,(int)(next-val_buf),ct,tag,AE_FINALIZE);
    
    tag_idx = ((kbytes/16)-1) * 3 + ((tbytes/4)-2);
    result = (tag_idx < 6) && !memcmp(tag,tags[tag_idx],(size_t)tbytes);

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

/* ----------------------------------------------------------------------- */
/* Simple test program                                                     */
/* ----------------------------------------------------------------------- */

#define BPI 16

#if 0

#include <stdio.h>
#include <time.h>

#if __GNUC__
	#define ALIGN(n) __attribute__ ((aligned(n)))
#elif _MSC_VER
	#define ALIGN(n) __declspec(align(n))
#else /* Not GNU/Microsoft: delete alignment uses.     */
	#define ALIGN(n)
#endif

static void pbuf(void *p, unsigned len, const void *s)
{
    unsigned i;
    if (s)
        printf("%s", (char *)s);
    for (i = 0; i < len; i++)
        printf("%02X", (unsigned)(((unsigned char *)p)[i]));
    printf("\n");
}

static void vectors(ae_ctx *ctx, int len)
{
    ALIGN(16) char pt[128];
    ALIGN(16) char ct[144];
    ALIGN(16) char nonce[] = {0,1,2,3,4,5,6,7,8,9,10,11};
    int i;
    for (i=0; i < 128; i++) pt[i] = i;
    i = ae_encrypt(ctx,nonce,pt,len,pt,len,ct,NULL,AE_FINALIZE);
    printf("P=%d,A=%d: ",len,len); pbuf(ct, i, NULL);
    i = ae_encrypt(ctx,nonce,pt,0,pt,len,ct,NULL,AE_FINALIZE);
    printf("P=%d,A=%d: ",0,len); pbuf(ct, i, NULL);
    i = ae_encrypt(ctx,nonce,pt,len,pt,0,ct,NULL,AE_FINALIZE);
    printf("P=%d,A=%d: ",len,0); pbuf(ct, i, NULL);
}

void validate()
{
    ALIGN(16) char pt[1024];
    ALIGN(16) char ct[1024+16];
    ALIGN(16) char tag[16];
    ALIGN(16) char nonce[12] = {0,};
    ALIGN(16) char key[32] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    ALIGN(16) char valid[] = {0xB2,0xB4,0x1C,0xBF,0x9B,0x05,0x03,0x7D,
                              0xA7,0xF1,0x6C,0x24,0xA3,0x5C,0x1C,0x94};
    ae_ctx ctx;
    char *val_buf, *next;
    int i, len;

    val_buf = (char *)malloc(22400 + 16);
    next = val_buf = (char *)(((size_t)val_buf + 16) & ~((size_t)15));

    if (0) {
		ae_init(&ctx, key, 16, 12, 16);
		/* pbuf(&ctx, sizeof(ctx), "CTX: "); */
		vectors(&ctx,0);
		vectors(&ctx,8);
		vectors(&ctx,16);
		vectors(&ctx,24);
		vectors(&ctx,32);
		vectors(&ctx,40);
    }

    memset(key,0,32);
    memset(pt,0,128);
    ae_init(&ctx, key, 16, 12, 16);

    /* RFC Vector test */
    for (i = 0; i < 128; i++) {
        int first = ((i/3)/(BPI*16))*(BPI*16);
        int second = first;
        int third = i - (first + second);

        nonce[11] = i;

        if (0) {
            ae_encrypt(&ctx,nonce,pt,i,pt,i,ct,NULL,AE_FINALIZE);
            memcpy(next,ct,(size_t)i+16);
            next = next+i+16;

            ae_encrypt(&ctx,nonce,pt,i,pt,0,ct,NULL,AE_FINALIZE);
            memcpy(next,ct,(size_t)i+16);
            next = next+i+16;

            ae_encrypt(&ctx,nonce,pt,0,pt,i,ct,NULL,AE_FINALIZE);
            memcpy(next,ct,16);
            next = next+16;
        } else {
            ae_encrypt(&ctx,nonce,pt,first,pt,first,ct,NULL,AE_PENDING);
            ae_encrypt(&ctx,NULL,pt+first,second,pt+first,second,ct+first,NULL,AE_PENDING);
            ae_encrypt(&ctx,NULL,pt+first+second,third,pt+first+second,third,ct+first+second,NULL,AE_FINALIZE);
            memcpy(next,ct,(size_t)i+16);
            next = next+i+16;

            ae_encrypt(&ctx,nonce,pt,first,pt,0,ct,NULL,AE_PENDING);
            ae_encrypt(&ctx,NULL,pt+first,second,pt,0,ct+first,NULL,AE_PENDING);
            ae_encrypt(&ctx,NULL,pt+first+second,third,pt,0,ct+first+second,NULL,AE_FINALIZE);
            memcpy(next,ct,(size_t)i+16);
            next = next+i+16;

            ae_encrypt(&ctx,nonce,pt,0,pt,first,ct,NULL,AE_PENDING);
            ae_encrypt(&ctx,NULL,pt,0,pt+first,second,ct,NULL,AE_PENDING);
            ae_encrypt(&ctx,NULL,pt,0,pt+first+second,third,ct,NULL,AE_FINALIZE);
            memcpy(next,ct,16);
            next = next+16;
        }

    }
    nonce[11] = 0;
    ae_encrypt(&ctx,nonce,NULL,0,val_buf,next-val_buf,ct,tag,AE_FINALIZE);
    pbuf(tag,16,0);
    if (memcmp(valid,tag,16) == 0)
    	printf("Vectors: PASS\n");
    else
    	printf("Vectors: FAIL\n");


    /* Encrypt/Decrypt test */
    for (i = 0; i < 1024; i++) {
        int first = ((i/3)/(BPI*16))*(BPI*16);
        int second = first;
        int third = i - (first + second);

        nonce[11] = i%128;

        if (1) {
            len = ae_encrypt(&ctx,nonce,val_buf,i,val_buf,i,ct,tag,AE_FINALIZE);
            len = ae_encrypt(&ctx,nonce,val_buf,i,val_buf,-1,ct,tag,AE_FINALIZE);
            len = ae_decrypt(&ctx,nonce,ct,len,val_buf,-1,pt,tag,AE_FINALIZE);
            if (len == -1) { printf("Authentication error: %d\n", i); return; }
            if (len != i) { printf("Length error: %d\n", i); return; }
            if (memcmp(val_buf,pt,i)) { printf("Decrypt error: %d\n", i); return; }
        } else {
            len = ae_encrypt(&ctx,nonce,val_buf,i,val_buf,i,ct,NULL,AE_FINALIZE);
            ae_decrypt(&ctx,nonce,ct,first,val_buf,first,pt,NULL,AE_PENDING);
            ae_decrypt(&ctx,NULL,ct+first,second,val_buf+first,second,pt+first,NULL,AE_PENDING);
            len = ae_decrypt(&ctx,NULL,ct+first+second,len-(first+second),val_buf+first+second,third,pt+first+second,NULL,AE_FINALIZE);
            if (len == -1) { printf("Authentication error: %d\n", i); return; }
            if (memcmp(val_buf,pt,i)) { printf("Decrypt error: %d\n", i); return; }
        }

    }
    printf("Decrypt: PASS\n");
}

int main()
{
    validate();
    return 0;
}
#endif

char infoString[] = "OCB (Beta VAES)";
