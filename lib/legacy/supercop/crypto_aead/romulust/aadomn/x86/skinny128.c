/******************************************************************************
 * Intel SSSE3 Skinny-128-384+ implementation fine tuned for Romulus-T (lower
 * half of TK1 is assumed to be null)
 * 
 * @author  Alexandre Adomnicai
 *          alex.adomnicai@gmail.com
 * 
 * @date    March 2022.
 *****************************************************************************/
#include "immintrin.h"
#include "skinny128.h"

/**
 * Apply the S-box, Add Round Tweakey, and Add Round Constants to the internal
 * state 'state'.
 * The half of the TK1 which is added is non-null.
 */ 
#define SBOX_ARK(rtk_1, rtk_23)                                                   \
    rtk   = _mm_loadl_epi64((const __m128i*)(rtk_23)); /* load roundtweakey */  \
    tmp0  = _mm_srli_epi16(state, 4);       /* extract high nibbles (1/2) */    \
    state = _mm_and_si128(state, mask_nib); /* extract low nibbles */           \
    tmp0  = _mm_and_si128(tmp0, mask_nib);  /* extract high nibbles (2/2) */    \
    state = _mm_shuffle_epi8(s1, state);    /* apply inner S-box S1 */          \
    tmp0  = _mm_shuffle_epi8(s0, tmp0);     /* apply inner S-box S0 */          \
    rtk   = _mm_xor_si128(rtk, c2);         /* add rconst c2 */                 \
    state = _mm_xor_si128(tmp0, state);     /* recombine S-boxes' outputs */    \
    tk_1  = _mm_loadl_epi64((const __m128i*)(rtk_1)); /* load roundtweakey */  \
    tmp0  = _mm_srli_epi16(state, 4);       /* extract high nibbles (1/2) */    \
    tmp1  = _mm_and_si128(state, mask_lsb); /* extract LSB */                   \
    tmp0  = _mm_and_si128(tmp0, mask_nib);  /* extract high nibbles (2/2) */    \
    state = _mm_and_si128(state, mask_nib); /* extract low nibbles */           \
    tmp0  = _mm_shuffle_epi8(s3, tmp0);     /* apply inner S-box S3 */          \
    state = _mm_shuffle_epi8(s2, state);    /* apply inner S-box S2 */          \
    tmp0  = _mm_or_si128(tmp1, tmp0);       /* additional OR with LSB */        \
    rtk   = _mm_xor_si128(rtk, tk_1);       /* rtk_123 = rtk_23 ^ rtk_1 */      \
    state = _mm_xor_si128(state, tmp0);     /* recombine S-boxes' outputs */    \
    state = _mm_xor_si128(state, rtk);      /* add rtweakey and rconsts */      \

/**
 * Apply the linear layer (comprising ShiftRows and MixColumns) to the internal
 * state 'state'.
 */
#define SR_MC()                                                                 \
    tmp0   = _mm_shuffle_epi8(state, m0);   /* tmp0 <- (r3, r0, r1, r2) */      \
    tmp1   = _mm_and_si128(state, mask_row);/* tmp1 <- r0, - , - , - ) */       \
    state = _mm_shuffle_epi8(state, m1);    /* state <- (r2, - , r2, r0) */     \
    tmp0   = _mm_xor_si128(tmp0, tmp1);     /* (r3^r0, r0, r1, r2) */           \
    state = _mm_xor_si128(tmp0, state);     /* (r3^r0^r2, r0, r1^r2, r2^r0) */  \

/**
 * Apply 2 rounds of Skinny-128-384+ to the internal state 'state'.
 */
#define DOUBLE_ROUND(rtk_1, rtk_23)     \
    SBOX_ARK(rtk_1, rtk_23);            \
    SR_MC();                            \
    SBOX_ARK(rtk_1+8, rtk_23+8);        \
    SR_MC();                            \

/**
 * Skinny-128-384+ encryption of a single 128-bit block w/o any operation mode.
 * 
 * The round tweakeys are assumed to be precompute for TK2 and TK3 tweakey
 * states while it is computed on-the-fly for TK1 (which changes for each call
 * to Skinny in Romulus-N/M).
 */
void skinny128_384_plus(
    unsigned char *out,
    const unsigned char *in,
    const unsigned char *tk1,
    const unsigned char *rtk_23)
{

    unsigned char rtk_1[BLOCKBYTES/2*16];

    // use 16 XMM registers to avoid stack usage
    __m128i tmp0;
    __m128i tmp1;
    __m128i rtk;
    __m128i state = _mm_loadu_si128((const __m128i*)in);
    __m128i tk_1  = _mm_loadu_si128((const __m128i*)tk1);
    __m128i s0 = {0xb090a08010300020, 0xb898a88838182808};
    __m128i s1 = {0x45044405004181c0, 0x470746064303c282};
    __m128i s2 = {0x1810080019110901, 0x1a130a031b120b02};
    __m128i s3 = {0xe063a033c0431380, 0xe464a434c4441484};
    __m128i m0 = {0x030201000c0f0e0d, 0x09080b0a06050407};
    __m128i m1 = {0x8080808009080b0a, 0x0302010009080b0a};
    __m128i c2 = {0x0000000000000000,0x0000000000000002};
    __m128i mask_row = {0x00000000ffffffff, 0x0000000000000000};
    __m128i mask_nib = {0x0f0f0f0f0f0f0f0f, 0x0f0f0f0f0f0f0f0f};
    __m128i mask_lsb = {0x0101010101010101, 0x0101010101010101};
    __m128i perm_tk  = {0x0304060205000701, 0x0b0c0e0a0d080f09};

    // put internal state into output buffer
    _mm_storeu_si64((__m128i*)rtk_1, tk_1);
    tk_1 = _mm_shuffle_epi8(tk_1, _mm_set_epi32(0x03040602, 0x05000701, 0x0b0c0e0a, 0x0d080f09));
    _mm_storeu_si128((__m128i*)(rtk_1+8), tk_1);
    tk_1 = _mm_shuffle_epi8(tk_1, perm_tk);
    _mm_storeu_si128((__m128i*)(rtk_1+24), tk_1);
    tk_1 = _mm_shuffle_epi8(tk_1, perm_tk);
    _mm_storeu_si128((__m128i*)(rtk_1+40), tk_1);
    tk_1 = _mm_shuffle_epi8(tk_1, perm_tk);
    _mm_storeu_si128((__m128i*)(rtk_1+56), tk_1);
    tk_1 = _mm_shuffle_epi8(tk_1, perm_tk);
    _mm_storeu_si128((__m128i*)(rtk_1+72), tk_1);
    tk_1 = _mm_shuffle_epi8(tk_1, perm_tk);
    _mm_storeu_si128((__m128i*)(rtk_1+88), tk_1);
    tk_1 = _mm_shuffle_epi8(tk_1, perm_tk);
    _mm_storeu_si128((__m128i*)(rtk_1+104), tk_1);
    tk_1 = _mm_shuffle_epi8(tk_1, perm_tk);
    _mm_storeu_si64((__m128i*)(rtk_1+120), tk_1);

    // skinny-128-384+ has 40 rounds
    DOUBLE_ROUND(rtk_1,     rtk_23);
    DOUBLE_ROUND(rtk_1+16,  rtk_23+16);
    DOUBLE_ROUND(rtk_1+32,  rtk_23+32);
    DOUBLE_ROUND(rtk_1+48,  rtk_23+48);
    DOUBLE_ROUND(rtk_1+64,  rtk_23+64);
    DOUBLE_ROUND(rtk_1+80,  rtk_23+80);
    DOUBLE_ROUND(rtk_1+96,  rtk_23+96);
    DOUBLE_ROUND(rtk_1+112, rtk_23+112);
    DOUBLE_ROUND(rtk_1,     rtk_23+128);
    DOUBLE_ROUND(rtk_1+16,  rtk_23+144);
    DOUBLE_ROUND(rtk_1+32,  rtk_23+160);
    DOUBLE_ROUND(rtk_1+48,  rtk_23+176);
    DOUBLE_ROUND(rtk_1+64,  rtk_23+192);
    DOUBLE_ROUND(rtk_1+80,  rtk_23+208);
    DOUBLE_ROUND(rtk_1+96,  rtk_23+224);
    DOUBLE_ROUND(rtk_1+112, rtk_23+240);
    DOUBLE_ROUND(rtk_1,     rtk_23+256);
    DOUBLE_ROUND(rtk_1+16,  rtk_23+272);
    DOUBLE_ROUND(rtk_1+32,  rtk_23+288);
    DOUBLE_ROUND(rtk_1+48,  rtk_23+304);

    // put internal state into output buffer
    _mm_storeu_si128((__m128i*)out, state);
}

/**
 * Double update of the tweakey states TK2 and TK3.
 * The corresponding round tweakeys 'rtk_2' and 'rtk_3' are XORed together w/
 * the round constants c0,c1 and stored in the 'rtk' output buffer.
 */ 
#define DOUBLE_TK23_UPDATE(c00, c10, c01, c11, perm)                                \
    rtk_3   = _mm_shuffle_epi8(rtk_3, perm);    /* permute tk3 */                   \
    rtk_2   = _mm_shuffle_epi8(rtk_2, perm);    /* permute tk2 */                   \
    tmp0    = _mm_srli_epi16(rtk_3, 6);         /* ( -, -, -, -, -, -,x7,x6) */     \
    tmp1    = _mm_srli_epi16(rtk_3, 1);         /* ( -, -, -, -, -, -, -,x7) */     \
    tmp2    = _mm_slli_epi16(rtk_2, 2);         /* (x5,x4,x3,x2,x1,x0, -, -) */     \
    tmp3    = _mm_slli_epi16(rtk_2, 1);         /* (x6,x5,x4,x3,x2,x1,x0, -) */     \
    tmp0    = _mm_and_si128(tmp0, mask_03);     /* discard adjacent bits */         \
    tmp1    = _mm_andnot_si128(mask_80, tmp1);  /* discard adjacent bits */         \
    tmp2    = _mm_andnot_si128(mask_03, tmp2);  /* discard adjacent bits */         \
    tmp3    = _mm_andnot_si128(mask_01, tmp3);  /* discard adjacent bits */         \
    rtk_3   = _mm_xor_si128(rtk_3, tmp0);       /* (-,-,-,-,-,-,x7^x1,x6^x0) */     \
    tmp2    = _mm_xor_si128(rtk_2, tmp2);       /*(x5^x7,x4^x6, -,-,-,...,-) */     \
    tmp0    = _mm_set_epi32(c11, c01, c10, c00);/* build rconst c0,c1 */            \
    rtk_3   = _mm_slli_epi16(rtk_3, 7);         /* (x6^x5,-,-,-,-,-,-,-) */         \
    tmp2    = _mm_srli_epi16(tmp2, 7);          /* (-,-,-,-,-,-,-,x7^x5) */         \
    rtk_3   = _mm_and_si128(rtk_3, mask_80);    /* discard adjacent bits */         \
    rtk_2   = _mm_and_si128(tmp2, mask_01);     /* discard adjacent bits */         \
    rtk_3   = _mm_or_si128(rtk_3, tmp1);        /* LFSR3(rtk3) */                   \
    rtk_2   = _mm_or_si128(rtk_2, tmp3);        /* LFSR2(rtk2) */                   \
    tmp0    = _mm_xor_si128(tmp0, rtk_3);       /* rtk3 ^ rconst */                 \
    tmp0    = _mm_xor_si128(tmp0, rtk_2);       /* rtk2 ^ rtk3 ^ rconst */          \
    _mm_storeu_si128((__m128i*)rtk_23, tmp0);   /* store 2 rtk at once */           \
    rtk_23  += 16;                              /* now points to the next rtk */    \

/**
 * Double update of the tweakey states TK3.
 * The corresponding round tweakeys rtk_3' are XORed together w/ the round
 * constants c0,c1 and stored in the 'rtk' output buffer.
 * Useful for Romulus-T where many calls to Skinny are done with a null TK2.
 */ 
#define DOUBLE_TK3_UPDATE(c00, c10, c01, c11, perm)                                 \
    rtk_3   = _mm_shuffle_epi8(rtk_3, perm);    /* permute tk3 */                   \
    tmp0    = _mm_srli_epi16(rtk_3, 6);         /* ( -, -, -, -, -, -,x7,x6) */     \
    tmp1    = _mm_srli_epi16(rtk_3, 1);         /* ( -, -, -, -, -, -, -,x7) */     \
    tmp0    = _mm_and_si128(tmp0, mask_03);     /* discard adjacent bits */         \
    tmp1    = _mm_andnot_si128(mask_80, tmp1);  /* discard adjacent bits */         \
    rtk_3   = _mm_xor_si128(rtk_3, tmp0);       /* (-,-,-,-,-,-,x7^x1,x6^x0) */     \
    tmp0    = _mm_set_epi32(c11, c01, c10, c00);/* build rconst c0,c1 */            \
    rtk_3   = _mm_slli_epi16(rtk_3, 7);         /* (x6^x5,-,-,-,-,-,-,-) */         \
    rtk_3   = _mm_and_si128(rtk_3, mask_80);    /* discard adjacent bits */         \
    rtk_3   = _mm_or_si128(rtk_3, tmp1);        /* LFSR3(rtk3) */                   \
    tmp0    = _mm_xor_si128(tmp0, rtk_3);       /* rtk3 ^ rconst */                 \
    _mm_storeu_si128((__m128i*)rtk, tmp0);      /* store 2 rtk at once */           \
    rtk     += 16;                              /* now points to the next rtk */    \

/**
 * Precompute the round tweakeys for TK2 and TK3 tweakey states, including the
 * round constants c0,c1.
 */
void tk_schedule_23(
    unsigned char *rtk_23,
    const unsigned char *tk2,
    const unsigned char *tk3)
{
    __m128i tmp0;
    __m128i tmp1;
    __m128i tmp2;
    __m128i tmp3    = {0x0000000000000001, 0x0000000000000000};
    __m128i rtk_2   = _mm_loadu_si128((const __m128i*)tk2);
    __m128i rtk_3   = _mm_loadu_si128((const __m128i*)tk3);
    __m128i perm_0  = {0x0b0c0e0a0d080f09, 0x0304060205000701};
    __m128i perm_tk = {0x0304060205000701, 0x0b0c0e0a0d080f09};
    __m128i mask_01 = {0x0101010101010101, 0x0101010101010101}; // not(mask_fe)
    __m128i mask_03 = {0x0303030303030303, 0x0303030303030303}; // not(mask_fc)
    __m128i mask_80 = {0x8080808080808080, 0x8080808080808080}; // not(mask_7f)

    // first round tweakeys is simply extracted from the initial tweakey states
    tmp0    = _mm_xor_si128(tmp3, rtk_3);
    tmp0    = _mm_xor_si128(tmp0, rtk_2);
    _mm_storeu_si64((__m128i*)rtk_23, tmp0);
    rtk_23  += 8;
    // next round tweakeys are computed using double updates to save cycles
    DOUBLE_TK23_UPDATE(0x03, 0x00, 0x07, 0x00, perm_0);
    DOUBLE_TK23_UPDATE(0x0f, 0x00, 0x0f, 0x01, perm_tk);
    DOUBLE_TK23_UPDATE(0x0e, 0x03, 0x0d, 0x03, perm_tk);
    DOUBLE_TK23_UPDATE(0x0b, 0x03, 0x07, 0x03, perm_tk);
    DOUBLE_TK23_UPDATE(0x0f, 0x02, 0x0e, 0x01, perm_tk);
    DOUBLE_TK23_UPDATE(0x0c, 0x03, 0x09, 0x03, perm_tk);
    DOUBLE_TK23_UPDATE(0x03, 0x03, 0x07, 0x02, perm_tk);
    DOUBLE_TK23_UPDATE(0x0e, 0x00, 0x0d, 0x01, perm_tk);
    DOUBLE_TK23_UPDATE(0x0a, 0x03, 0x05, 0x03, perm_tk);
    DOUBLE_TK23_UPDATE(0x0b, 0x02, 0x06, 0x01, perm_tk);
    DOUBLE_TK23_UPDATE(0x0c, 0x02, 0x08, 0x01, perm_tk);
    DOUBLE_TK23_UPDATE(0x00, 0x03, 0x01, 0x02, perm_tk);
    DOUBLE_TK23_UPDATE(0x02, 0x00, 0x05, 0x00, perm_tk);
    DOUBLE_TK23_UPDATE(0x0b, 0x00, 0x07, 0x01, perm_tk);
    DOUBLE_TK23_UPDATE(0x0e, 0x02, 0x0c, 0x01, perm_tk);
    DOUBLE_TK23_UPDATE(0x08, 0x03, 0x01, 0x03, perm_tk);
    DOUBLE_TK23_UPDATE(0x03, 0x02, 0x06, 0x00, perm_tk);
    DOUBLE_TK23_UPDATE(0x0d, 0x00, 0x0b, 0x01, perm_tk);
    DOUBLE_TK23_UPDATE(0x06, 0x03, 0x0d, 0x02, perm_tk);
    // do not use the macro since we only need to store 64-bit for the last rtk
    rtk_3   = _mm_shuffle_epi8(rtk_3, perm_tk);
    rtk_2   = _mm_shuffle_epi8(rtk_2, perm_tk);
    tmp0    = _mm_srli_epi16(rtk_3, 6);
    tmp1    = _mm_srli_epi16(rtk_3, 1);
    tmp2    = _mm_slli_epi16(rtk_2, 2);
    tmp3    = _mm_slli_epi16(rtk_2, 1);
    tmp0    = _mm_and_si128(tmp0, mask_03);
    tmp1    = _mm_andnot_si128(mask_80, tmp1);
    tmp2    = _mm_andnot_si128(mask_03, tmp2);
    tmp3    = _mm_andnot_si128(mask_01, tmp3);
    rtk_3   = _mm_xor_si128(rtk_3, tmp0);
    tmp2    = _mm_xor_si128(rtk_2, tmp2);
    tmp0    = _mm_set_epi32(0x0, 0x0, 0x1, 0xa);
    rtk_3   = _mm_slli_epi16(rtk_3, 7);
    tmp2    = _mm_srli_epi16(tmp2, 7);
    rtk_3   = _mm_and_si128(rtk_3, mask_80);
    tmp2    = _mm_and_si128(tmp2, mask_01);
    rtk_3   = _mm_or_si128(rtk_3, tmp1);
    rtk_2   = _mm_or_si128(tmp2, tmp3);
    tmp0    = _mm_xor_si128(tmp0, rtk_3);
    tmp0    = _mm_xor_si128(tmp0, rtk_2);
    _mm_storeu_si64((__m128i*)rtk_23, tmp0);
}

/**
 * Precompute the round tweakeys for TK3 tweakey states, including the
 * round constants c0,c1.
 */
void tk_schedule_3(
    unsigned char *rtk,
    const unsigned char *tk3)
{
    __m128i tmp0    = {0x0000000000000001, 0x0000000000000000};
    __m128i tmp1;
    __m128i rtk_3   = _mm_loadu_si128((const __m128i*)tk3);
    __m128i perm_0  = {0x0b0c0e0a0d080f09, 0x0304060205000701};
    __m128i perm_tk = {0x0304060205000701, 0x0b0c0e0a0d080f09};
    __m128i mask_03 = {0x0303030303030303, 0x0303030303030303}; // not(mask_fc)
    __m128i mask_80 = {0x8080808080808080, 0x8080808080808080}; // not(mask_7f)

    // first round tweakeys is simply extracted from the initial tweakey states
    tmp0    = _mm_xor_si128(tmp0, rtk_3);
    _mm_storeu_si64((__m128i*)rtk, tmp0);
    rtk += 8;
    // next round tweakeys are computed using double updates to save cycles
    DOUBLE_TK3_UPDATE(0x03, 0x00, 0x07, 0x00, perm_0);
    DOUBLE_TK3_UPDATE(0x0f, 0x00, 0x0f, 0x01, perm_tk);
    DOUBLE_TK3_UPDATE(0x0e, 0x03, 0x0d, 0x03, perm_tk);
    DOUBLE_TK3_UPDATE(0x0b, 0x03, 0x07, 0x03, perm_tk);
    DOUBLE_TK3_UPDATE(0x0f, 0x02, 0x0e, 0x01, perm_tk);
    DOUBLE_TK3_UPDATE(0x0c, 0x03, 0x09, 0x03, perm_tk);
    DOUBLE_TK3_UPDATE(0x03, 0x03, 0x07, 0x02, perm_tk);
    DOUBLE_TK3_UPDATE(0x0e, 0x00, 0x0d, 0x01, perm_tk);
    DOUBLE_TK3_UPDATE(0x0a, 0x03, 0x05, 0x03, perm_tk);
    DOUBLE_TK3_UPDATE(0x0b, 0x02, 0x06, 0x01, perm_tk);
    DOUBLE_TK3_UPDATE(0x0c, 0x02, 0x08, 0x01, perm_tk);
    DOUBLE_TK3_UPDATE(0x00, 0x03, 0x01, 0x02, perm_tk);
    DOUBLE_TK3_UPDATE(0x02, 0x00, 0x05, 0x00, perm_tk);
    DOUBLE_TK3_UPDATE(0x0b, 0x00, 0x07, 0x01, perm_tk);
    DOUBLE_TK3_UPDATE(0x0e, 0x02, 0x0c, 0x01, perm_tk);
    DOUBLE_TK3_UPDATE(0x08, 0x03, 0x01, 0x03, perm_tk);
    DOUBLE_TK3_UPDATE(0x03, 0x02, 0x06, 0x00, perm_tk);
    DOUBLE_TK3_UPDATE(0x0d, 0x00, 0x0b, 0x01, perm_tk);
    DOUBLE_TK3_UPDATE(0x06, 0x03, 0x0d, 0x02, perm_tk);
    // do not use the macro since we only need to store 64-bit for the last rtk
    rtk_3   = _mm_shuffle_epi8(rtk_3, perm_tk);
    tmp0    = _mm_srli_epi16(rtk_3, 6);
    tmp1    = _mm_srli_epi16(rtk_3, 1);
    tmp0    = _mm_and_si128(tmp0, mask_03);
    tmp1    = _mm_andnot_si128(mask_80, tmp1);
    rtk_3   = _mm_xor_si128(rtk_3, tmp0);
    tmp0    = _mm_set_epi32(0x0, 0x0, 0x1, 0xa);
    rtk_3   = _mm_slli_epi16(rtk_3, 7);
    rtk_3   = _mm_and_si128(rtk_3, mask_80);
    rtk_3   = _mm_or_si128(rtk_3, tmp1);
    tmp0    = _mm_xor_si128(tmp0, rtk_3);
    _mm_storeu_si128((__m128i*)rtk, tmp0);
}
