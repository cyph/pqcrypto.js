/**
 *
 * Optimized ISO-C11 Implementation of LEDAcrypt using GCC built-ins.
 *
 * @version 3.0 (May 2020)
 *
 * In alphabetical order:
 *
 * @author Marco Baldi <m.baldi@univpm.it>
 * @author Alessandro Barenghi <alessandro.barenghi@polimi.it>
 * @author Franco Chiaraluce <f.chiaraluce@univpm.it>
 * @author Gerardo Pelosi <gerardo.pelosi@polimi.it>
 * @author Paolo Santini <p.santini@pm.univpm.it>
 *
 * This code is hereby placed in the public domain.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ''AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 **/

#include "architecture_detect.h"
#include "gf2x_arith_mod_xPplusOne.h"
#include <string.h>  // memcpy(...), memset(...)
#include <stdalign.h>
#include "djbsort.h"



/*----------------------------------------------------------------------------*/

#if (defined(DIGIT_IS_UINT8) || defined(DIGIT_IS_UINT16))
static
uint8_t byte_reverse_with_less32bitDIGIT(uint8_t b)
{
    uint8_t r = b;
    int s = (sizeof(b) << 3) - 1;
    for (b >>= 1; b; b >>= 1) {
        r <<= 1;
        r |= b & 1;
        s--;
    }
    r <<= s;
    return r;
} // end byte_reverse_less32bitDIGIT
#endif

#if defined(DIGIT_IS_UINT32)
static
uint8_t byte_reverse_with_32bitDIGIT(uint8_t b)
{
    b = ( (b * 0x0802LU & 0x22110LU) | (b * 0x8020LU & 0x88440LU)
        ) * 0x10101LU >> 16;
    return b;
} // end byte_reverse_32bitDIGIT
#endif

#if defined(DIGIT_IS_UINT64)
static
uint8_t byte_reverse_with_64bitDIGIT(uint8_t b)
{
    b = (b * 0x0202020202ULL & 0x010884422010ULL) % 1023;
    return b;
} // end byte_reverse_64bitDIGIT
#endif

/*----------------------------------------------------------------------------*/

static
DIGIT reverse_digit(const DIGIT b)
{
    int i;
    union toReverse_t {
        uint8_t inByte[DIGIT_SIZE_B];
        DIGIT digitValue;
    } toReverse;

    toReverse.digitValue = b;
#if defined(DIGIT_IS_UINT64)
    for (i = 0; i < DIGIT_SIZE_B; i++)
    { toReverse.inByte[i] = byte_reverse_with_64bitDIGIT(toReverse.inByte[i]); }
    return __builtin_bswap64(toReverse.digitValue);
#elif defined(DIGIT_IS_UINT32)
    for (i = 0; i < DIGIT_SIZE_B; i++)
    { toReverse.inByte[i] = byte_reverse_with_32bitDIGIT(toReverse.inByte[i]); }
    return __builtin_bswap32(toReverse.digitValue);
#elif defined(DIGIT_IS_UINT16)
    for (i = 0; i < DIGIT_SIZE_B; i++)
    { toReverse.inByte[i] = byte_reverse_with_less32bitDIGIT(toReverse.inByte[i]); }
    reversed = __builtin_bswap16(toReverse.digitValue);
#elif defined(DIGIT_IS_UINT8)
    return byte_reverse_with_less32bitDIGIT(toReverse.inByte[0]);
#else
#error "Missing implementation for reverse_digit(...) \
with this CPU word bitsize !!! "
#endif
    return toReverse.digitValue;
} // end reverse_digit

/*----------------------------------------------------------------------------*/

void gf2x_transpose_in_place(DIGIT A[])
{
    /* it keeps the lsb in the same position and
     * inverts the sequence of the remaining bits
     */

    DIGIT mask = (DIGIT)0x1;
    DIGIT rev1, rev2, a00;
    int i, slack_bits_amount = NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_b - P;

    if (NUM_DIGITS_GF2X_ELEMENT == 1) {
        a00 = A[0] & mask;
        A[0] = A[0] >> 1;
        rev1 = reverse_digit(A[0]);
#if (NUM_DIGITS_GF2X_MOD_P_ELEMENT*DIGIT_SIZE_b - P)
        rev1 >>= (DIGIT_SIZE_b-(P%DIGIT_SIZE_b));
#endif
        A[0] = (rev1 & (~mask)) | a00;
        return;
    }

    a00 = A[NUM_DIGITS_GF2X_ELEMENT-1] & mask;
    right_bit_shift_n(NUM_DIGITS_GF2X_ELEMENT, A,1);

    for (i = NUM_DIGITS_GF2X_ELEMENT-1; i >= (NUM_DIGITS_GF2X_ELEMENT+1)/2; i--) {
        rev1 = reverse_digit(A[i]);
        rev2 = reverse_digit(A[NUM_DIGITS_GF2X_ELEMENT-1-i]);
        A[i] = rev2;
        A[NUM_DIGITS_GF2X_ELEMENT-1-i] = rev1;
    }
    if (NUM_DIGITS_GF2X_ELEMENT % 2 == 1)
    { A[NUM_DIGITS_GF2X_ELEMENT/2] = reverse_digit(A[NUM_DIGITS_GF2X_ELEMENT/2]); }

    if (slack_bits_amount)
    { right_bit_shift_n(NUM_DIGITS_GF2X_ELEMENT, A,slack_bits_amount); }
    A[NUM_DIGITS_GF2X_ELEMENT-1] = (A[NUM_DIGITS_GF2X_ELEMENT-1] & (~mask)) | a00;
} // end transpose_in_place

/*----------------------------------------------------------------------------*/
/* computes poly times digit multiplication as a support for KTT inverse */
/* PRE : nr = na + 1 */

#if (defined HIGH_PERFORMANCE_X86_64)
#define GF2X_DIGIT_TIMES_POLY_MUL gf2x_digit_times_poly_mul_avx
static
void gf2x_digit_times_poly_mul_avx(const int nr,
                                   DIGIT Res[NUM_DIGITS_GF2X_ELEMENT+1],
                                   const int na, const DIGIT A[],
                                   const DIGIT B)
{

    __m128i prodRes0,prodRes1,
            accumRes,loopCarriedWord,lowToHighWord,
            wideB,wideA;

    int i;
    wideB=_mm_set_epi64x(0, B);
    loopCarriedWord = _mm_set_epi64x(0,0);

    for (i = na-1; i >= 1 ; i=i-2) {
        /*wideA contains [ A[i] A[i-1] ] */
        wideA = _mm_lddqu_si128((__m128i *)&A[i-1]);

        prodRes0 = _mm_clmulepi64_si128(wideA, wideB, 1);
        prodRes1 = _mm_clmulepi64_si128(wideA, wideB, 0);

        accumRes = _mm_xor_si128(loopCarriedWord,prodRes0);
        lowToHighWord = _mm_slli_si128(prodRes1,8);
        accumRes = _mm_xor_si128(accumRes,lowToHighWord);

        accumRes = (__m128i) _mm_shuffle_pd( (__m128d) accumRes,
                                             (__m128d) accumRes, 1);
        _mm_storeu_si128((__m128i *)(&Res[i]), accumRes);

        loopCarriedWord = _mm_srli_si128(prodRes1,8);
    }
    if (i == 0) { /*skipped last iteration i=0, compensate*/
        prodRes0 = _mm_clmulepi64_si128(_mm_set_epi64x(0, A[0]), wideB, 0);

        accumRes = loopCarriedWord;
        accumRes = _mm_xor_si128(accumRes,prodRes0);
        accumRes = (__m128i) _mm_shuffle_pd( (__m128d) accumRes,
                                             (__m128d) accumRes, 1);
        _mm_storeu_si128((__m128i *)(&Res[0]), accumRes);
    } else { /*i == 1*/
        /*regular exit condition, do nothing*/
    }

}

#else
#define GF2X_DIGIT_TIMES_POLY_MUL gf2x_digit_times_poly_mul

void gf2x_digit_times_poly_mul(const int nr, DIGIT Res[NUM_DIGITS_GF2X_ELEMENT+1],
                               const int na, const DIGIT A[],
                               const DIGIT B)
{

    DIGIT pres[2];
    Res[nr-1]=0;
    for (int i = (nr-1)-1; i >= 0 ; i--) {
        GF2X_MUL(2, pres, 1, &A[i], 1, &B);
        Res[i+1] = Res[i+1] ^ pres[1];
        Res[i] =  pres[0];
    }
}
#endif

/*----------------------------------------------------------------------------
*
* Based on: K. Kobayashi, N. Takagi and K. Takagi, "Fast inversion algorithm in
* GF(2m) suitable for implementation with a polynomial multiply instruction on
* GF(2)," in IET Computers & Digital Techniques, vol. 6, no. 3, pp. 180-185,
* May 2012. doi: 10.1049/iet-cdt.2010.0006
*/

int gf2x_mod_inverse_KTT(DIGIT out[], const DIGIT in[])   /* in^{-1} mod x^P-1 */
{

#if NUM_DIGITS_GF2X_MODULUS == NUM_DIGITS_GF2X_ELEMENT
    DIGIT s[NUM_DIGITS_GF2X_ELEMENT+1] = {0},
                                         r[NUM_DIGITS_GF2X_ELEMENT+1];
    r[0]=0;
    memcpy(r+1,in, NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B);

    /* S starts set to the modulus */
    s[NUM_DIGITS_GF2X_ELEMENT+1-1] = 1;
    s[0+1] |= ((DIGIT)1) << MSb_POSITION_IN_MSB_DIGIT_OF_MODULUS;

    DIGIT v[2*NUM_DIGITS_GF2X_ELEMENT] = {0},
                                         u[2*NUM_DIGITS_GF2X_ELEMENT] = {0};

    u[2*NUM_DIGITS_GF2X_ELEMENT-1] = (DIGIT) 2; /* x */

    int deg_r = NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_b -1;
    int deg_s = NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_b -1;

    DIGIT c,d;
    DIGIT h00,h01,h10,h11;

    DIGIT hibitmask = ( (DIGIT) 1) << (DIGIT_SIZE_b-1);

    DIGIT r_h00[NUM_DIGITS_GF2X_ELEMENT+2];
    DIGIT s_h01[NUM_DIGITS_GF2X_ELEMENT+2];
    DIGIT r_h10[NUM_DIGITS_GF2X_ELEMENT+2];
    DIGIT s_h11[NUM_DIGITS_GF2X_ELEMENT+2];
    DIGIT u_h00[2*NUM_DIGITS_GF2X_ELEMENT+1];
    DIGIT v_h01[2*NUM_DIGITS_GF2X_ELEMENT+1];
    DIGIT u_h10[2*NUM_DIGITS_GF2X_ELEMENT+1];
    DIGIT v_h11[2*NUM_DIGITS_GF2X_ELEMENT+1];

    while(deg_r > 0) {
        c=r[1];
        d=s[1];
        if(c == 0) {
            left_DIGIT_shift_n(NUM_DIGITS_GF2X_ELEMENT+1,r,1);
            left_DIGIT_shift_n(2*NUM_DIGITS_GF2X_ELEMENT,u,1);
            deg_r = deg_r - DIGIT_SIZE_b;
        } else {
            /* H = I */
            h00 = 1;
            h01 = 0;
            h10 = 0;
            h11 = 1;
            for(int j = 1 ; (j < DIGIT_SIZE_b) && (deg_r > 0) ; j++) {
                if ( (c & hibitmask) == 0) { /* */
                    c = c << 1;

                    h00 = h00 << 1;
                    h01 = h01 << 1;
                    deg_r--;
                } else { /* hibit r[0] set */
                    if (deg_r == deg_s) {
                        deg_r--;
                        if ( (d & hibitmask) == hibitmask) { /* hibit r[0],s[0] set, deg_r == deg_s */
                            DIGIT temp = c;
                            c = (c^d) << 1; /* (c-d)*x */
                            d = temp;
                            /*mult H*/
                            DIGIT r00;
                            r00 = (h00 << 1) ^ (h10 << 1);
                            DIGIT r01;
                            r01 = (h01 << 1) ^ (h11 << 1);
                            h10 = h00;
                            h11 = h01;
                            h00 = r00;
                            h01 = r01;
                        } else { /* hibit r[0] set, s[0] unset, deg_r == deg_s */
                            DIGIT temp;
                            temp = c;
                            c = d << 1;
                            d = temp;
                            /*mult H*/
                            DIGIT r00;
                            r00 = (h10 << 1);
                            DIGIT r01;
                            r01 = (h11 << 1);
                            h10 = h00;
                            h11 = h01;
                            h00 = r00;
                            h01 = r01;
                        }
                    } else { /* if (deg_r != deg_s) */
                        deg_s--;
                        if ( (d & hibitmask) == hibitmask) { /* hibit r[0],s[0] set, deg_r != deg_s */
                            d = (c^d) << 1; /* (c-d) * x*/
                            /* mult H */
                            h10 = (h00 << 1) ^ (h10 << 1);
                            h11 = (h01 << 1) ^ (h11 << 1);
                        } else { /* hibit r[0] set, s[0] unset, deg_r != deg_s */
                            d = d << 1;
                            /*mul H*/

                            h10 = h10 << 1;
                            h11 = h11 << 1;
                        }
                    } /*(deg_r == deg_s)*/
                } /* if ( (c & ((DIGIT 1) << (DIGIT_SIZE_b-1))) == 0) */
            } /* while */
            /*update r , s */

            GF2X_DIGIT_TIMES_POLY_MUL(NUM_DIGITS_GF2X_ELEMENT+2, r_h00,
                                      NUM_DIGITS_GF2X_ELEMENT+1, r,
                                      h00);
            GF2X_DIGIT_TIMES_POLY_MUL(NUM_DIGITS_GF2X_ELEMENT+2, s_h01,
                                      NUM_DIGITS_GF2X_ELEMENT+1, s,
                                      h01);
            GF2X_DIGIT_TIMES_POLY_MUL(NUM_DIGITS_GF2X_ELEMENT+2, r_h10,
                                      NUM_DIGITS_GF2X_ELEMENT+1, r,
                                      h10);
            GF2X_DIGIT_TIMES_POLY_MUL(NUM_DIGITS_GF2X_ELEMENT+2, s_h11,
                                      NUM_DIGITS_GF2X_ELEMENT+1, s,
                                      h11);

            gf2x_add(NUM_DIGITS_GF2X_ELEMENT+1, r,
                     NUM_DIGITS_GF2X_ELEMENT+1, r_h00+1,
                     NUM_DIGITS_GF2X_ELEMENT+1, s_h01+1);

            gf2x_add(NUM_DIGITS_GF2X_ELEMENT+1, s,
                     NUM_DIGITS_GF2X_ELEMENT+1, r_h10+1,
                     NUM_DIGITS_GF2X_ELEMENT+1, s_h11+1);

            /* *********************** update u, v *************************/
            GF2X_DIGIT_TIMES_POLY_MUL(2*NUM_DIGITS_GF2X_ELEMENT+1, u_h00,
                                      2*NUM_DIGITS_GF2X_ELEMENT, u,
                                      h00);
            GF2X_DIGIT_TIMES_POLY_MUL(2*NUM_DIGITS_GF2X_ELEMENT+1, v_h01,
                                      2*NUM_DIGITS_GF2X_ELEMENT, v,
                                      h01);
            GF2X_DIGIT_TIMES_POLY_MUL(2*NUM_DIGITS_GF2X_ELEMENT+1, u_h10,
                                      2*NUM_DIGITS_GF2X_ELEMENT, u,
                                      h10);
            GF2X_DIGIT_TIMES_POLY_MUL(2*NUM_DIGITS_GF2X_ELEMENT+1, v_h11,
                                      2*NUM_DIGITS_GF2X_ELEMENT, v,
                                      h11);

            gf2x_add(2*NUM_DIGITS_GF2X_ELEMENT, u,
                     2*NUM_DIGITS_GF2X_ELEMENT, u_h00+1,
                     2*NUM_DIGITS_GF2X_ELEMENT, v_h01+1);
            gf2x_add(2*NUM_DIGITS_GF2X_ELEMENT, v,
                     2*NUM_DIGITS_GF2X_ELEMENT, u_h10+1,
                     2*NUM_DIGITS_GF2X_ELEMENT, v_h11+1);
        }
    }
    if (deg_r == 0) {
        memcpy(out,u,NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B);
    } else {
        memcpy(out,v,NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B);
    }
#else
#error IMPLEMENT MEMCPY INTO A LARGER OPERAND
#endif

    return 0;
}


void gf2x_mod_mul(DIGIT Res[], const DIGIT A[], const DIGIT B[])
{

    DIGIT aux[2*NUM_DIGITS_GF2X_ELEMENT];
    GF2X_MUL(2*NUM_DIGITS_GF2X_ELEMENT, aux,
             NUM_DIGITS_GF2X_ELEMENT, A,
             NUM_DIGITS_GF2X_ELEMENT, B);
    gf2x_mod(Res, 2*NUM_DIGITS_GF2X_ELEMENT, aux);

} // end gf2x_mod_mul

/*----------------------------------------------------------------------------*/
/* Obtains fresh randomness and seed-expands it until all the required positions
 * for the '1's in the circulant block are obtained */

void rand_circulant_sparse_block(POSITION_T *pos_ones,
                                 const int countOnes,
                                 AES_XOF_struct *seed_expander_ctx)
{

    int duplicated, placedOnes = 0;

    while (placedOnes < countOnes) {
        int p = rand_range(NUM_BITS_GF2X_ELEMENT,
                           BITS_TO_REPRESENT(P),
                           seed_expander_ctx);
        duplicated = 0;
        for (int j = 0; j < placedOnes; j++) {
            if (pos_ones[j] == p) {
                duplicated = 1;
            }
        }
        if (duplicated == 0) {
            pos_ones[placedOnes] = p;
            placedOnes++;
        }
    }
} // rand_circulant_sparse_block

/*----------------------------------------------------------------------------*/

void rand_error_pos(POSITION_T errorPos[NUM_ERRORS_T],
                    AES_XOF_struct *seed_expander_ctx)
{

    int duplicated, counter = 0;

    while (counter < NUM_ERRORS_T) {
        int p = rand_range(N0*NUM_BITS_GF2X_ELEMENT,BITS_TO_REPRESENT(P),
                           seed_expander_ctx);
        duplicated = 0;
        for (int j = 0; j < counter; j++) {
            if (errorPos[j] == p) {
                duplicated = 1;
            }
        }
        if (duplicated == 0) {
            errorPos[counter] = p;
            counter++;
        }
    }
} // end rand_error_pos

/*----------------------------------------------------------------------------*/

void rand_error_pos_shake(POSITION_T errorPos[NUM_ERRORS_T],
                          xof_shake_t *state)
{

    int duplicated, counter = 0;

    while (counter < NUM_ERRORS_T) {
        int p = rand_range_shake(N0*NUM_BITS_GF2X_ELEMENT,BITS_TO_REPRESENT(P),
                                 state);
        duplicated = 0;
        for (int j = 0; j < counter; j++) {
               if (errorPos[j] == p) {
                   duplicated = 1;
            }
        }
        if (duplicated == 0) {
            errorPos[counter] = p;
            counter++;
        }
    }
} // end rand_error_pos_shake

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*-----------------------------CONSTANT TIME ARITHMETIC-----------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#define LO_SHIFT_AMT_BITS (BITS_TO_REPRESENT(DIGIT_SIZE_b-1))
#define HI_SHIFT_AMT_BITS (BITS_TO_REPRESENT(P) - LO_SHIFT_AMT_BITS)
/* intended as *left* *cyclic* shift, acting in-place,
 * i.e. multiply by a monomial x^shift_amt */
#if (defined HIGH_PERFORMANCE_X86_64)
#define CTIME_IF_256(mask,then,else)  _mm256_blendv_epi8(else, then, mask)
#endif
#define CTIME_SWAP(condmask,true_val,false_val) ((true_val & condmask) | (false_val & ~condmask))


#if (defined CONSTANT_TIME)
/* same as gf2x_mul_monom_inplace_CT, just acts out-of-place to ease
 * sparse2dense mul, shifted variable does not need to be clean before use*/
static inline
void word_level_shift_CT(DIGIT * restrict shifted_param,
                         POSITION_T high_shift_amt,
                         DIGIT * restrict to_shift)
{
    DIGIT current_buf[NUM_DIGITS_GF2X_ELEMENT];
    DIGIT mask;
    DIGIT *restrict shifted, * restrict current, * restrict tmp_for_ptrswap;
    shifted = shifted_param;
    current = current_buf;
    memcpy(current,to_shift,NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B);

    for (int i = 0; i < HI_SHIFT_AMT_BITS; i++) {
        /* power-of-two matching current inter-word shift bit */
        int word_shift_amt = (1 << i);

        /* extract and remove high shift amount LSB */
        DIGIT lsb_of_high_shift_amt = (high_shift_amt & 1);
        high_shift_amt >>= 1;
        /* conditional mask = 0xFF..FF if current bit of inter-word shift idx is set */
        mask = 0 - lsb_of_high_shift_amt;

#if (defined HIGH_PERFORMANCE_X86_64)
        __m256i vector_mask = _mm256_set_epi64x ((DIGIT)0-lsb_of_high_shift_amt,
                              (DIGIT)0-lsb_of_high_shift_amt,
                              (DIGIT)0-lsb_of_high_shift_amt,
                              (DIGIT)0-lsb_of_high_shift_amt);
        /* condit-pull whole digits towards the MSB, starting from the
         *word_shift_amt - th  one, that is including the one which will have slack */
        {
            int j = 0;
            __m256i tmp,tmp2;
            for (; j < NUM_DIGITS_GF2X_ELEMENT-word_shift_amt-3; j = j+4) {
                tmp = _mm256_lddqu_si256 ((__m256i *) (current+j+word_shift_amt));
                tmp2 = _mm256_lddqu_si256 ((__m256i *) (current+j));
                tmp = CTIME_IF_256(vector_mask,tmp,tmp2);
                _mm256_storeu_si256(  (__m256i *) (&shifted[j]), tmp);
            }
            for (; j < NUM_DIGITS_GF2X_ELEMENT-word_shift_amt; j++) {
                shifted[j] = CTIME_SWAP(mask,current[j+word_shift_amt],current[j]);
            }
        }
#else
        /* condit-pull whole digits towards the MSB, starting from the word_shift_amt - th
         * one, that is including the one which will have slack */
        for (int j = 0; j < NUM_DIGITS_GF2X_ELEMENT-word_shift_amt; j++) {
            shifted[j] = CTIME_SWAP(mask,current[j+word_shift_amt],current[j]);
        }
#endif
        /* collect the slack carryover */
        DIGIT slack_carryover = SLACK_EXTRACT(shifted[0]);
        shifted[0] &= SLACK_CLEAR_MASK;

        /* move the remaining topmost word_shift_amt words (0th to word_shift_amt-1
         * one) taking care of tucking in the slack carryover */
#if (defined HIGH_PERFORMANCE_X86_64)
        if(word_shift_amt >=4) { /* at least enough bits to fill an AVX2 register */
            /* I have enough word material to move to gain something substantial
             * via AVX2*/
            DIGIT next_carryover;
            for (int j = word_shift_amt-4 ; j >=0 ; j = j-4) {

                __m256i in_motion = _mm256_lddqu_si256((__m256i *) (current+j));
                __m256i hi_tgt_part  = _mm256_slli_epi64(in_motion, SLACK_SIZE  );
                __m256i low_tgt_part = _mm256_srli_epi64(in_motion,(DIGIT_SIZE_b - SLACK_SIZE));

                next_carryover = _mm256_extract_epi64 (low_tgt_part, 0);
                low_tgt_part = _mm256_insert_epi64 (low_tgt_part,slack_carryover, 0);
                slack_carryover = next_carryover;
                low_tgt_part = _mm256_permute4x64_epi64 (low_tgt_part, 0x39);

                int target_idx = NUM_DIGITS_GF2X_ELEMENT-word_shift_amt+j;
                __m256i old = _mm256_lddqu_si256 ((__m256i *) (current+target_idx));
                __m256i final = CTIME_IF_256(vector_mask,_mm256_or_si256(hi_tgt_part,low_tgt_part),old);
                _mm256_storeu_si256( (__m256i *) (shifted+target_idx),final);
            }
        } else {
            for (int j = word_shift_amt-1 ; j >=0 ; j--) {
                int target_idx = NUM_DIGITS_GF2X_ELEMENT-word_shift_amt+j;
                DIGIT to_write = slack_carryover;
                to_write = to_write | (current[j] << SLACK_SIZE);
                slack_carryover = SLACK_EXTRACT(current[j]);
                shifted[target_idx] = CTIME_SWAP(mask,to_write,current[target_idx]);
            }
        }
#else
        for (int j = word_shift_amt-1 ; j >=0 ; j--) {
            int target_idx = NUM_DIGITS_GF2X_ELEMENT-1-(word_shift_amt-1)+j;
            DIGIT to_write = slack_carryover;
            to_write = to_write | (current[j] << SLACK_SIZE);
            slack_carryover = SLACK_EXTRACT(current[j]);
            shifted[target_idx] = CTIME_SWAP(mask,to_write,current[target_idx]);
        }
#endif
        tmp_for_ptrswap = current;
        current = shifted;
        shifted = tmp_for_ptrswap;
    }
    /* if the number of iterations is  even, the data is placed in the*/
    if ((HI_SHIFT_AMT_BITS) %2 ==0) {
        memcpy(shifted, current, NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B);
    }
}
#else
#if (defined HIGH_PERFORMANCE_X86_64)
static inline
void word_level_shift_VT(DIGIT * restrict shifted_param,
                         POSITION_T high_shift_amt,
                         DIGIT * restrict to_shift)
{

    /* condit-pull whole digits towards the MSB, starting from the
     *high_shift_amt - th  one, that is including the one which will have slack */
    {
        int j = 0;
        __m256i tmp;
        /*if there is enough material to have at least a full AVX2 reg to shift*/
        if(NUM_DIGITS_GF2X_ELEMENT-high_shift_amt >=4) {
            for (; j < NUM_DIGITS_GF2X_ELEMENT-high_shift_amt-3; j = j+4) {
                tmp = _mm256_lddqu_si256 ((__m256i *) (to_shift+j+high_shift_amt));
                _mm256_storeu_si256(  (__m256i *) (shifted_param+j), tmp);
            }
        }
        for (; j < NUM_DIGITS_GF2X_ELEMENT-high_shift_amt; j++) {
            shifted_param[j] = to_shift[j+high_shift_amt];
        }
    }
    /* collect the slack carryover */
    DIGIT slack_carryover = SLACK_EXTRACT(shifted_param[0]);
    shifted_param[0] &= SLACK_CLEAR_MASK;

    /* move the remaining topmost high_shift_amt words (0th to high_shift_amt-1
     * one) taking care of tucking in the slack carryover */
    if(high_shift_amt >=4) {
        /* I have enough word material to move to gain something substantial
         * via AVX2*/
        DIGIT next_carryover;
        int j;
        for (j = high_shift_amt-4 ; j >=0 ; j = j-4) {

            __m256i in_motion = _mm256_lddqu_si256((__m256i *) (to_shift+j));
            __m256i hi_tgt_part  = _mm256_slli_epi64(in_motion, SLACK_SIZE  );
            __m256i low_tgt_part = _mm256_srli_epi64(in_motion,(DIGIT_SIZE_b - SLACK_SIZE));

            next_carryover = _mm256_extract_epi64 (low_tgt_part, 0);
            low_tgt_part = _mm256_insert_epi64 (low_tgt_part,slack_carryover, 0);
            slack_carryover = next_carryover;
            low_tgt_part = _mm256_permute4x64_epi64 (low_tgt_part, 0x39);

            int target_idx = NUM_DIGITS_GF2X_ELEMENT-high_shift_amt+j;
            __m256i final = _mm256_or_si256(hi_tgt_part,low_tgt_part);
            _mm256_storeu_si256( (__m256i *) (shifted_param+target_idx),final);
        }

        j+=3;
        for (; j >=0 ; j--) {
            int target_idx = NUM_DIGITS_GF2X_ELEMENT-high_shift_amt+j;
            DIGIT to_write = slack_carryover;
            to_write = to_write | (to_shift[j] << SLACK_SIZE);
            slack_carryover = SLACK_EXTRACT(to_shift[j]);
            shifted_param[target_idx] = to_write;
        }
        /* handle trailing words */
    } else {
        for (int j = high_shift_amt-1 ; j >=0 ; j--) {
            int target_idx = NUM_DIGITS_GF2X_ELEMENT-high_shift_amt+j;
            DIGIT to_write = slack_carryover;
            to_write = to_write | (to_shift[j] << SLACK_SIZE);
            slack_carryover = SLACK_EXTRACT(to_shift[j]);
            shifted_param[target_idx] = to_write;
        }
    }
}
#else
static inline
void word_level_shift_VT(DIGIT * restrict shifted_param,
                         POSITION_T high_shift_amt,
                         DIGIT * restrict to_shift)
{
    /* condit-pull whole digits towards the MSB, starting from the word_shift_amt - th
     * one, that is including the one which will have slack */
    for (int j = 0; j < NUM_DIGITS_GF2X_ELEMENT-high_shift_amt; j++) {
        shifted_param[j] = to_shift[j+high_shift_amt];
    }

    /* collect the slack carryover */
    DIGIT slack_carryover = SLACK_EXTRACT(shifted_param[0]);
    shifted_param[0] &= SLACK_CLEAR_MASK;

    /* move the remaining topmost word_shift_amt words (0th to word_shift_amt-1
     * one) taking care of tucking in the slack carryover */

    for (int j = high_shift_amt-1 ; j >=0 ; j--) {
        int target_idx = NUM_DIGITS_GF2X_ELEMENT-1-(high_shift_amt-1)+j;
        DIGIT to_write = slack_carryover;
        to_write = to_write | (to_shift[j] << SLACK_SIZE);
        slack_carryover = SLACK_EXTRACT(to_shift[j]);
        shifted_param[target_idx] = to_write;
    }
}
/* end of selector between high performance and compatible*/
#endif
/* end of selector between constant and variable time */
#endif

#if (defined CONSTANT_TIME)
#define WORD_LEVEL_SHIFT word_level_shift_CT
#else
#define WORD_LEVEL_SHIFT word_level_shift_VT
#endif

void gf2x_mod_mul_monom(DIGIT shifted[],
                        POSITION_T shift_amt,
                        const DIGIT to_shift[])
{
    DIGIT mask;
    /*shift_amt is split bitwise :  |------------------shift_amt------------------|
     *                              | inter word shift amt | intra word shift amt |
     *                                  HI_SHIFT_AMT_BITS     LO_SHIFT_AMT_BITS
     */

    /* inter word shifting, done speculatively shifting the entire operand by a
     * power of two, and conditionally committing the result */
    POSITION_T high_shift_amt = shift_amt >> LO_SHIFT_AMT_BITS;
    POSITION_T low_shift_amt = shift_amt & (((POSITION_T)1 << LO_SHIFT_AMT_BITS) -1);

    WORD_LEVEL_SHIFT(shifted,high_shift_amt,(DIGIT * restrict)to_shift);

    /* cyclic shifts inside DIGITs */
    /* extract low_shift_amt MSB for cyclic shift */
    DIGIT carryover = (shifted[0] << SLACK_SIZE) | (shifted[1] >> (DIGIT_SIZE_b -SLACK_SIZE));
    carryover = carryover >> (DIGIT_SIZE_b - low_shift_amt);

    /* pure shift, carried over from left_bit_shift_n*/
    mask = ~(( (DIGIT)1 << (DIGIT_SIZE_b - low_shift_amt) ) - 1);
    /* must deal with C99 UB when shifting by variable size*/
    DIGIT zeroshift_mask = (DIGIT)0 - (!!(low_shift_amt));

#if (defined HIGH_PERFORMANCE_X86_64)
    {
        int j;
        __m256i a,b;
        for(j = 0 ; j < NUM_DIGITS_GF2X_ELEMENT-4; j = j+4) {
            a = _mm256_lddqu_si256( (__m256i *) &shifted[0] + j/4);  //load from in[j] to in[j+3]
            b = _mm256_lddqu_si256( (__m256i *) &shifted[1] + j/4);  //load from in[j+1] to in[j+4]
            a = _mm256_slli_epi64(a, low_shift_amt);
            b = _mm256_srli_epi64(b, (DIGIT_SIZE_b-low_shift_amt));
            /* no need to zeromask, the srli behavior is well defined for amount > 63 */
            _mm256_storeu_si256( (__m256i *) &shifted[0] + j/4, _mm256_or_si256(a,b));
        }
        for (; j < NUM_DIGITS_GF2X_ELEMENT-1 ; j++) {
            shifted[j] = (shifted[j] << low_shift_amt) |
                         ( zeroshift_mask & (shifted[j+1] & mask) >> (DIGIT_SIZE_b - low_shift_amt) );
        }
    }
#else
    for (int j = 0 ; j < NUM_DIGITS_GF2X_ELEMENT-1 ; j++) {
        shifted[j] = (shifted[j] << low_shift_amt) | ( zeroshift_mask &
                     (shifted[j+1] & mask) >> (DIGIT_SIZE_b - low_shift_amt) );
    }
#endif
    shifted[NUM_DIGITS_GF2X_ELEMENT-1] = (shifted[NUM_DIGITS_GF2X_ELEMENT-1] << low_shift_amt);
    shifted[NUM_DIGITS_GF2X_ELEMENT-1] |=(zeroshift_mask & carryover);
    shifted[0] &= SLACK_CLEAR_MASK;
}

void gf2x_mod_fmac(DIGIT result[],
                   POSITION_T shift_amt,
                   const DIGIT to_shift[]) {
    DIGIT shifted_temp[NUM_DIGITS_GF2X_ELEMENT] = {0};

    DIGIT mask;
    /*shift_amt is split bitwise :  |------------------shift_amt------------------|
     *                              | inter word shift amt | intra word shift amt |
     *                                  HI_SHIFT_AMT_BITS     LO_SHIFT_AMT_BITS
     */

    /* inter word shifting, done speculatively shifting the entire operand by a
     * power of two, and conditionally committing the result */
    POSITION_T high_shift_amt = shift_amt >> LO_SHIFT_AMT_BITS;
    POSITION_T low_shift_amt = shift_amt & (((POSITION_T)1 << LO_SHIFT_AMT_BITS) -1);

    WORD_LEVEL_SHIFT(shifted_temp,high_shift_amt,(DIGIT * restrict)to_shift);

    /* cyclic shifts inside DIGITs */
    /* extract low_shift_amt MSB for cyclic shift */
    DIGIT carryover = (shifted_temp[0] << SLACK_SIZE) | (shifted_temp[1] >> (DIGIT_SIZE_b -SLACK_SIZE));
    carryover = carryover >> (DIGIT_SIZE_b - low_shift_amt);

    /* pure shift, carried over from left_bit_shift_n*/
    mask = ~(( (DIGIT)1 << (DIGIT_SIZE_b - low_shift_amt) ) - 1);
    /* must deal with C99 UB when shifting by variable size*/
    DIGIT zeroshift_mask = (DIGIT)0 - (!!(low_shift_amt));

#if (defined HIGH_PERFORMANCE_X86_64)
    {
        int j;
        __m256i a,b,accu;
        for(j = 0 ; j < NUM_DIGITS_GF2X_ELEMENT-4; j = j+4) {
            a = _mm256_lddqu_si256( (__m256i *) &shifted_temp[0] + j/4); //load from in[j] to in[j+3]
            b = _mm256_lddqu_si256( (__m256i *) &shifted_temp[1] + j/4); //load from in[j+1] to in[j+4]
            accu = _mm256_lddqu_si256( (__m256i *) &result[0] + j/4);
            a = _mm256_slli_epi64(a, low_shift_amt);
            b = _mm256_srli_epi64(b, (DIGIT_SIZE_b-low_shift_amt));
            /* no need to zeromask, the srli behavior is well defined for amount > 63 */
            _mm256_storeu_si256( (__m256i *) &result[0] + j/4,
                                 _mm256_xor_si256(_mm256_or_si256(a,b),accu) );
        }
        for (; j < NUM_DIGITS_GF2X_ELEMENT-1 ; j++) {
            result[j] ^= (shifted_temp[j] << low_shift_amt) | ( zeroshift_mask &
                         (shifted_temp[j+1] & mask) >> (DIGIT_SIZE_b - low_shift_amt) );
        }
    }
#else
    for (int j = 0 ; j < NUM_DIGITS_GF2X_ELEMENT-1 ; j++) {
        result[j] ^= (shifted_temp[j] << low_shift_amt) | ( zeroshift_mask &
                     (shifted_temp[j+1] & mask) >> (DIGIT_SIZE_b - low_shift_amt) );
    }
#endif
    result[NUM_DIGITS_GF2X_ELEMENT-1] ^= (shifted_temp[NUM_DIGITS_GF2X_ELEMENT-1] << low_shift_amt) | (zeroshift_mask & carryover);
    result[0] &= SLACK_CLEAR_MASK;
}

/**********************************************************************************
   MONOMIAL MULTIPLICATION DERIVATIVES: MUL DENSE TO SPARSE AND LIFTED MUL D2S
 **********************************************************************************/

/* computes a sparse to dense multiplication using gf2x_mod_mul_monom */
void gf2x_mod_mul_dense_to_sparse(DIGIT Res[],
                                  const DIGIT dense[],
                                  const POSITION_T sparse[],
                                  unsigned int nPos)
{

    memset(Res,0,NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B);
    for(int i = 0; i < nPos; i++) {
        gf2x_mod_fmac(Res,sparse[i],dense);
    }
}
