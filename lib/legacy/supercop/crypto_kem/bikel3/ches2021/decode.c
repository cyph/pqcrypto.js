/* Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0"
 *
 * Otiginally written by Nir Drucker, Shay Gueron and Dusan Kostic,
 * AWS Cryptographic Algorithms Group.
 *
 * Modified by Ming-Shing Chen and Tung Chou
 *
 * [1] The optimizations are based on the description developed in the paper:
 *     Drucker, Nir, and Shay Gueron. 2019. “A Toolbox for Software Optimization
 *     of QC-MDPC Code-Based Cryptosystems.” Journal of Cryptographic Engineering,
 *     January, 1–17. https://doi.org/10.1007/s13389-018-00200-4.
 *
 * [2] The decoder algorithm is the Black-Gray decoder in
 *     the early submission of CAKE (due to N. Sandrier and R Misoczki).
 *
 * [3] The analysis for the constant time implementation is given in
 *     Drucker, Nir, Shay Gueron, and Dusan Kostic. 2019.
 *     “On Constant-Time QC-MDPC Decoding with Negligible Failure Rate.”
 *     Cryptology EPrint Archive, 2019. https://eprint.iacr.org/2019/1289.
 *
 * [4] it was adapted to BGF in:
 *     Drucker, Nir, Shay Gueron, and Dusan Kostic. 2019.
 *     “QC-MDPC decoders with several shades of gray.”
 *     Cryptology EPrint Archive, 2019. To be published.
 *
 * [5] Chou, T.: QcBits: Constant-Time Small-Key Code-Based Cryptography.
 *     In: Gier-lichs, B., Poschmann, A.Y. (eds.) Cryptographic Hardware
 *     and Embedded Systems– CHES 2016. pp. 280–300. Springer Berlin Heidelberg,
 *     Berlin, Heidelberg (2016)
 *
 * [6] The rotate512_small funciton is a derivative of the code described in:
 *     Guimarães, Antonio, Diego F Aranha, and Edson Borin. 2019.
 *     “Optimized Implementation of QC-MDPC Code-Based Cryptography.”
 *     Concurrency and Computation: Practice and Experience 31 (18):
 *     e5089. https://doi.org/10.1002/cpe.5089.
 */

#include "decode.h"
#include "cleanup.h"
#include "gf2x.h"
#include "utilities.h"

#include <stdio.h>

// Decoding (bit-flipping) parameter
#if defined(BG_DECODER)
#  if(LEVEL == 1)
#    define MAX_IT 3
#  elif(LEVEL == 3)
#    define MAX_IT 4
#  else
#    error "Level can only be 1/3"
#  endif
#elif defined(BGF_DECODER)
#  if(LEVEL == 1)
#    define MAX_IT 5
#  elif(LEVEL == 3)
#    define MAX_IT 5
#  else
#    error "Level can only be 1/3"
#  endif
#endif

// Duplicates the first R_BITS of the syndrome three times
// |------------------------------------------|
// |  Third copy | Second copy | first R_BITS |
// |------------------------------------------|
// This is required by the rotate functions.
_INLINE_ void dup(IN OUT syndrome_t *s)
{
  s->qw[R_QWORDS - 1] =
    (s->qw[0] << LAST_R_QWORD_LEAD) | (s->qw[R_QWORDS - 1] & LAST_R_QWORD_MASK);

  for(size_t i = 0; i < (2 * R_QWORDS) - 1; i++) {
    s->qw[R_QWORDS + i] =
      (s->qw[i] >> LAST_R_QWORD_TRAIL) | (s->qw[i + 1] << LAST_R_QWORD_LEAD);
  }
}

ret_t compute_syndrome(OUT syndrome_t *syndrome,
                       IN const pad_r_t *c0,
                       IN const pad_r_t *h0)
{
  DEFER_CLEANUP(pad_r_t pad_s, pad_r_cleanup);

  gf2x_mod_mul(&pad_s, c0, h0);

  bike_memcpy((uint8_t *)syndrome->qw, pad_s.val.raw, R_BYTES);
  dup(syndrome);

  return SUCCESS;
}

_INLINE_ ret_t recompute_syndrome(OUT syndrome_t *syndrome,
                                  IN const pad_r_t *c0,
                                  IN const pad_r_t *h0,
                                  IN const pad_r_t *pk,
                                  IN const e_t *e)
{
  DEFER_CLEANUP(pad_r_t tmp_c0, pad_r_cleanup);
  DEFER_CLEANUP(pad_r_t e0 = {0}, pad_r_cleanup);
  DEFER_CLEANUP(pad_r_t e1 = {0}, pad_r_cleanup);

  e0.val = e->val[0];
  e1.val = e->val[1];

  // tmp_c0 = pk * e1 + c0 + e0
  gf2x_mod_mul(&tmp_c0, &e1, pk);
  gf2x_mod_add(&tmp_c0, &tmp_c0, c0);
  gf2x_mod_add(&tmp_c0, &tmp_c0, &e0);

  // Recompute the syndrome using the updated ciphertext
  GUARD(compute_syndrome(syndrome, &tmp_c0, h0));

  return SUCCESS;
}

_INLINE_ uint8_t get_threshold(IN const syndrome_t *s)
{
  bike_static_assert(sizeof(*s) >= sizeof(r_t), syndrome_is_large_enough);

  const uint32_t syndrome_weight = r_bits_vector_weight((const r_t *)s->qw);

  // The equations below are defined in BIKE's specification p. 16, Section 5.2
  uint32_t       thr  = THRESHOLD_COEFF0 + (THRESHOLD_COEFF1 * syndrome_weight);
  const uint32_t mask = secure_l32_mask(thr, THRESHOLD_MIN);
  thr = (u32_barrier(mask) & thr) | (u32_barrier(~mask) & THRESHOLD_MIN);

  DMSG("    Threshold: %d\n", thr);
  return thr;
}

// Use half-adder as described in [5].
_INLINE_ void bit_sliced_adder(OUT upc_t *upc,
                               IN OUT syndrome_t *rotated_syndrome,
                               IN const size_t    num_of_slices)
{
  // From cache-memory perspective this loop should be the outside loop
  for(size_t j = 0; j < num_of_slices; j++) {
    for(size_t i = 0; i < R_QWORDS; i++) {
      const uint64_t carry = (upc->slice[j].u.qw[i] & rotated_syndrome->qw[i]);
      upc->slice[j].u.qw[i] ^= rotated_syndrome->qw[i];
      rotated_syndrome->qw[i] = carry;
    }
  }
}

_INLINE_ void adder_size_k(OUT upc_t *upc,
                          IN OUT syndrome_t *rotated_syndrome, 
                          IN int off,
                          IN int k)
{
  // from cache-memory perspective this loop should be the OUTside loop
  int i, j;
  __m256i carry, x, y, tmp;

  for(i = 0; i < R_YMM; i++) 
  {
    carry = LOAD(&rotated_syndrome->qw[4*i]);

    for(j = 0; j < k; j++) 
    {
      x = LOAD(&upc->slice[off + 0 + j].u.qw[4*i]);
      y = LOAD(&upc->slice[off + k + j].u.qw[4*i]);
      tmp = x^y;
 
      STORE(&upc->slice[off + 0 + j].u.qw[4*i], tmp ^ carry);
      carry = (tmp & carry) | (x & y);
    }

    STORE(&upc->slice[off + 0 + j].u.qw[4*i], carry);
  }
}


_INLINE_ void adder_size_63(OUT upc_t *upc,
                          IN OUT syndrome_t *rotated_syndrome, 
                          IN int off)
{
  // from cache-memory perspective this loop should be the outside loop
  int i, j;
  uint64_t carry, x, y, tmp;

  for(i = 0; i < R_QWORDS; i++) 
  {
    carry = rotated_syndrome->qw[i];

    for(j = 0; j < 3; j++) 
    {
      x = upc->slice[off + 0 + j].u.qw[i];
      y = upc->slice[off + 6 + j].u.qw[i];
      tmp = x^y;
 
      upc->slice[off + 0 + j].u.qw[i] = tmp ^ carry;
      carry = (tmp & carry) | (x & y);
    }

    for(j = 3; j < 6; j++) 
    {
      x = upc->slice[off + 0 + j].u.qw[i];
 
      upc->slice[off + 0 + j].u.qw[i] = x ^ carry;
      carry &= x;
    }

    upc->slice[off + 0 + j].u.qw[i] = carry;
  }
}

_INLINE_ void adder_size_53(OUT upc_t *upc,
                          IN OUT syndrome_t *rotated_syndrome, 
                          IN int off)
{
  // from cache-memory perspective this loop should be the outside loop
  int i, j;
  uint64_t carry, x, y, tmp;

  for(i = 0; i < R_QWORDS; i++) 
  {
    carry = rotated_syndrome->qw[i];

    for(j = 0; j < 3; j++) 
    {
      x = upc->slice[off + 0 + j].u.qw[i];
      y = upc->slice[off + 5 + j].u.qw[i];
      tmp = x^y;
 
      upc->slice[off + 0 + j].u.qw[i] = tmp ^ carry;
      carry = (tmp & carry) | (x & y);
    }

    for(j = 3; j < 5; j++) 
    {
      x = upc->slice[off + 0 + j].u.qw[i];
 
      upc->slice[off + 0 + j].u.qw[i] = x ^ carry;
      carry &= x;
    }

    upc->slice[off + 0 + j].u.qw[i] = carry;
  }
}

// Use half-adder as described in [5].
_INLINE_ void bit_sliced_adder_test(OUT upc_t *upc,
                               IN OUT syndrome_t *rotated_syndrome,
                               IN const size_t    num_of_slices)
{
  // From cache-memory perspective this loop should be the outside loop
  for(size_t j = 0; j < num_of_slices; j++) {
    for(size_t i = 0; i < R_YMM; i++) {
      __m256i x = LOAD(&(upc->slice[j].u.qw[i*4]));
      __m256i y = LOAD(&(rotated_syndrome->qw[i*4]));
      __m256i carry = x & y;
      STORE(&(upc->slice[j].u.qw[i*4]), x^y);
      STORE(&(rotated_syndrome->qw[i*4]), carry);
    }
  }
}

_INLINE_ void bit_slice_full_subtract(OUT upc_t *upc, IN uint8_t val)
{
  // Borrow
  uint64_t br[R_QWORDS] = {0};

  for(size_t j = 0; j < SLICES; j++) {

    const uint64_t lsb_mask = 0 - (val & 0x1);
    val >>= 1;

    // Perform a - b with c as the input/output carry
    // br = 0 0 0 0 1 1 1 1
    // a  = 0 0 1 1 0 0 1 1
    // b  = 0 1 0 1 0 1 0 1
    // -------------------
    // o  = 0 1 1 0 0 1 1 1
    // c  = 0 1 0 0 1 1 0 1
    //
    // o  = a^b^c
    //            _     __    _ _   _ _     _
    // br = abc + abc + abc + abc = abc + ((a+b))c

    for(size_t i = 0; i < R_QWORDS; i++) {
      const uint64_t a      = upc->slice[j].u.qw[i];
      const uint64_t b      = lsb_mask;
      const uint64_t tmp    = ((~a) & b & (~br[i])) | ((((~a) | b) & br[i]));
      upc->slice[j].u.qw[i] = a ^ b ^ br[i];
      br[i]                 = tmp;
    }
  }
}

#if defined(CRYPTO_NAMESPACE)
#define transpose_64x256_sp_asm  CRYPTO_NAMESPACE(transpose_64x256_sp_asm)
#endif

extern void transpose_64x256_sp_asm(__m256i *);


void dump1(__m256i v)
{
	printf("%.16llX", _mm256_extract_epi64(v, 0));
	printf(" %.16llX", _mm256_extract_epi64(v, 1));
	printf(" %.16llX", _mm256_extract_epi64(v, 2));
	printf(" %.16llX\n", _mm256_extract_epi64(v, 3));
}

// Calculate the Unsatisfied Parity Checks (UPCs) and update the errors
// vector (e) accordingly. In addition, update the black and gray errors vector
// with the relevant values.
_INLINE_ void find_err1(OUT e_t *e,
                        OUT e_t *black_e,
                        OUT e_t *gray_e,
                        IN syndrome_t *          syndrome,
                        IN const compressed_idx_d_ar_t wlist,
                        IN const uint8_t               threshold)
{
  // This function uses the bit-slice-adder methodology of [5]:
  DEFER_CLEANUP(syndrome_t rotated_syndrome = {0}, syndrome_cleanup);
  DEFER_CLEANUP(upc_t upc, upc_cleanup);

#  if(LEVEL == 1)

	__m256i buf[128];

	{
		int j;

		for (j = 0; j < 97; j++)
			buf[j] = LOAD(&syndrome->qw[4 * j]);

		for (j = 97; j < 128; j++)
			_mm256_setzero_si256();

		transpose_64x256_sp_asm(buf);
		transpose_64x256_sp_asm(buf + 64);
	}

#elif(LEVEL == 3)

	__m256i buf[384];

	{
		int i, j;
		__m256i x, y, v[3][2];
	
		//
	
		for (j = 0; j < 193; j++)
			buf[j] = LOAD(&syndrome->qw[4 * j]);
	
		for (j = 193; j < 256; j++)
			buf[j] = _mm256_setzero_si256();
	
		for (j = 0; j < 128; j++)
		{
			x = _mm256_permute2x128_si256(buf[j], buf[j+128], 0x20);
			y = _mm256_permute2x128_si256(buf[j], buf[j+128], 0x31);
			buf[j] = x;
			buf[j+128] = y;
		}

		for (j = 0; j < 64; j++)
		{
			x = _mm256_unpacklo_epi64(buf[j], buf[j+64]);
			y = _mm256_unpackhi_epi64(buf[j], buf[j+64]);
			buf[j] = x;
			buf[j+64] = y;
		}

		for (j = 128; j < 192; j++)
		{
			x = _mm256_unpacklo_epi64(buf[j], buf[j+64]);
			y = _mm256_unpackhi_epi64(buf[j], buf[j+64]);
			buf[j] = x;
			buf[j+64] = y;
		}

		transpose_64x256_sp_asm(buf);
		transpose_64x256_sp_asm(buf + 64);
		transpose_64x256_sp_asm(buf + 128);
		transpose_64x256_sp_asm(buf + 192);

		for (j = 0; j < 128; j++)
		{
			v[0][0] = buf[j];
			v[1][0] = PERM_I64(buf[j], 0x39);
			v[2][0] = PERM_I64(buf[j], 0x4E);
			v[0][1] = buf[j+128];
			v[1][1] = PERM_I64(buf[j+128], 0x39);
			v[2][1] = PERM_I64(buf[j+128], 0x4E);

			buf[j]     = _mm256_permute2x128_si256(v[0][0], v[0][1], 0x20);
			buf[j+128] = _mm256_permute2x128_si256(v[1][0], v[1][1], 0x20);
			buf[j+256] = _mm256_permute2x128_si256(v[2][0], v[2][1], 0x20);
		}

		for (i = 0; i < 384; i += 128)
		for (j = 0; j < 64; j++)
		{
			x = _mm256_unpacklo_epi64(buf[i+j], buf[i+j+64]);
			y = _mm256_unpackhi_epi64(buf[i+j], buf[i+j+64]);
			buf[i+j] = x;
			buf[i+j+64] = y;
		}
	}
#endif

  for(uint32_t i = 0; i < N0; i++) {
    // UPC must start from zero at every iteration
    bike_memset(&upc, 0, sizeof(upc));


    // 1) Right-rotate the syndrome for every secret key set bit index
    //    Then slice-add it to the UPC array.

#  if(LEVEL == 1)
    rotate_right(&rotated_syndrome, buf, wlist[i].val[0]);
    adder_size_k(&upc, &rotated_syndrome, 0, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[1]);
    adder_size_k(&upc, &rotated_syndrome, 1, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[2]);
    adder_size_k(&upc, &rotated_syndrome, 0, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[3]);
    adder_size_k(&upc, &rotated_syndrome, 2, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[4]);
    adder_size_k(&upc, &rotated_syndrome, 3, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[5]);
    adder_size_k(&upc, &rotated_syndrome, 2, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[6]);
    adder_size_k(&upc, &rotated_syndrome, 0, 2);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[7]);
    adder_size_k(&upc, &rotated_syndrome, 3, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[8]);
    adder_size_k(&upc, &rotated_syndrome, 4, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[9]);
    adder_size_k(&upc, &rotated_syndrome, 3, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[10]);
    adder_size_k(&upc, &rotated_syndrome, 5, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[11]);
    adder_size_k(&upc, &rotated_syndrome, 6, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[12]);
    adder_size_k(&upc, &rotated_syndrome, 5, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[13]);
    adder_size_k(&upc, &rotated_syndrome, 3, 2);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[14]);
    adder_size_k(&upc, &rotated_syndrome, 0, 3);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[15]);
    adder_size_k(&upc, &rotated_syndrome, 4, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[16]);
    adder_size_k(&upc, &rotated_syndrome, 5, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[17]);
    adder_size_k(&upc, &rotated_syndrome, 4, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[18]);
    adder_size_k(&upc, &rotated_syndrome, 6, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[19]);
    adder_size_k(&upc, &rotated_syndrome, 7, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[20]);
    adder_size_k(&upc, &rotated_syndrome, 6, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[21]);
    adder_size_k(&upc, &rotated_syndrome, 4, 2);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[22]);
    adder_size_k(&upc, &rotated_syndrome, 7, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[23]);
    adder_size_k(&upc, &rotated_syndrome, 8, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[24]);
    adder_size_k(&upc, &rotated_syndrome, 7, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[25]);
    adder_size_k(&upc, &rotated_syndrome, 9, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[26]);
    adder_size_k(&upc, &rotated_syndrome, 10, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[27]);
    adder_size_k(&upc, &rotated_syndrome, 9, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[28]);
    adder_size_k(&upc, &rotated_syndrome, 7, 2);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[29]);
    adder_size_k(&upc, &rotated_syndrome, 4, 3);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[30]);
    adder_size_k(&upc, &rotated_syndrome, 0, 4);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[31]);
    adder_size_k(&upc, &rotated_syndrome, 5, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[32]);
    adder_size_k(&upc, &rotated_syndrome, 6, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[33]);
    adder_size_k(&upc, &rotated_syndrome, 5, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[34]);
    adder_size_k(&upc, &rotated_syndrome, 7, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[35]);
    adder_size_k(&upc, &rotated_syndrome, 8, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[36]);
    adder_size_k(&upc, &rotated_syndrome, 7, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[37]);
    adder_size_k(&upc, &rotated_syndrome, 5, 2);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[38]);
    adder_size_k(&upc, &rotated_syndrome, 8, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[39]);
    adder_size_k(&upc, &rotated_syndrome, 9, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[40]);
    adder_size_k(&upc, &rotated_syndrome, 8, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[41]);
    adder_size_k(&upc, &rotated_syndrome, 10, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[42]);
    adder_size_k(&upc, &rotated_syndrome, 11, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[43]);
    adder_size_k(&upc, &rotated_syndrome, 10, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[44]);
    adder_size_k(&upc, &rotated_syndrome, 8, 2);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[45]);
    adder_size_k(&upc, &rotated_syndrome, 5, 3);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[46]);
    adder_size_k(&upc, &rotated_syndrome, 9, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[47]);
    adder_size_k(&upc, &rotated_syndrome, 10, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[48]);
    adder_size_k(&upc, &rotated_syndrome, 9, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[49]);
    adder_size_k(&upc, &rotated_syndrome, 11, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[50]);
    adder_size_k(&upc, &rotated_syndrome, 12, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[51]);
    adder_size_k(&upc, &rotated_syndrome, 11, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[52]);
    adder_size_k(&upc, &rotated_syndrome, 9, 2);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[53]);
    adder_size_k(&upc, &rotated_syndrome, 12, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[54]);
    adder_size_k(&upc, &rotated_syndrome, 13, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[55]);
    adder_size_k(&upc, &rotated_syndrome, 12, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[56]);
    adder_size_k(&upc, &rotated_syndrome, 14, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[57]);
    adder_size_k(&upc, &rotated_syndrome, 15, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[58]);
    adder_size_k(&upc, &rotated_syndrome, 14, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[59]);
    adder_size_k(&upc, &rotated_syndrome, 12, 2);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[60]);
    adder_size_k(&upc, &rotated_syndrome, 9, 3);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[61]);
    adder_size_k(&upc, &rotated_syndrome, 5, 4);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[62]);
    adder_size_k(&upc, &rotated_syndrome, 0, 5);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[63]);
    adder_size_k(&upc, &rotated_syndrome, 6, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[64]);
    adder_size_k(&upc, &rotated_syndrome, 7, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[65]);
    adder_size_k(&upc, &rotated_syndrome, 6, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[66]);
    adder_size_k(&upc, &rotated_syndrome, 8, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[67]);
    adder_size_k(&upc, &rotated_syndrome, 9, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[68]);
    adder_size_k(&upc, &rotated_syndrome, 8, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[69]);
    adder_size_k(&upc, &rotated_syndrome, 6, 2);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[70]);
    adder_size_63(&upc, &rotated_syndrome, 0);
#  elif(LEVEL == 3)
    rotate_right(&rotated_syndrome, buf, wlist[i].val[0]);
    adder_size_k(&upc, &rotated_syndrome, 0, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[1]);
    adder_size_k(&upc, &rotated_syndrome, 1, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[2]);
    adder_size_k(&upc, &rotated_syndrome, 0, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[3]);
    adder_size_k(&upc, &rotated_syndrome, 2, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[4]);
    adder_size_k(&upc, &rotated_syndrome, 3, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[5]);
    adder_size_k(&upc, &rotated_syndrome, 2, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[6]);
    adder_size_k(&upc, &rotated_syndrome, 0, 2);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[7]);
    adder_size_k(&upc, &rotated_syndrome, 3, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[8]);
    adder_size_k(&upc, &rotated_syndrome, 4, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[9]);
    adder_size_k(&upc, &rotated_syndrome, 3, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[10]);
    adder_size_k(&upc, &rotated_syndrome, 5, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[11]);
    adder_size_k(&upc, &rotated_syndrome, 6, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[12]);
    adder_size_k(&upc, &rotated_syndrome, 5, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[13]);
    adder_size_k(&upc, &rotated_syndrome, 3, 2);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[14]);
    adder_size_k(&upc, &rotated_syndrome, 0, 3);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[15]);
    adder_size_k(&upc, &rotated_syndrome, 4, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[16]);
    adder_size_k(&upc, &rotated_syndrome, 5, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[17]);
    adder_size_k(&upc, &rotated_syndrome, 4, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[18]);
    adder_size_k(&upc, &rotated_syndrome, 6, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[19]);
    adder_size_k(&upc, &rotated_syndrome, 7, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[20]);
    adder_size_k(&upc, &rotated_syndrome, 6, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[21]);
    adder_size_k(&upc, &rotated_syndrome, 4, 2);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[22]);
    adder_size_k(&upc, &rotated_syndrome, 7, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[23]);
    adder_size_k(&upc, &rotated_syndrome, 8, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[24]);
    adder_size_k(&upc, &rotated_syndrome, 7, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[25]);
    adder_size_k(&upc, &rotated_syndrome, 9, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[26]);
    adder_size_k(&upc, &rotated_syndrome, 10, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[27]);
    adder_size_k(&upc, &rotated_syndrome, 9, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[28]);
    adder_size_k(&upc, &rotated_syndrome, 7, 2);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[29]);
    adder_size_k(&upc, &rotated_syndrome, 4, 3);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[30]);
    adder_size_k(&upc, &rotated_syndrome, 0, 4);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[31]);
    adder_size_k(&upc, &rotated_syndrome, 5, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[32]);
    adder_size_k(&upc, &rotated_syndrome, 6, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[33]);
    adder_size_k(&upc, &rotated_syndrome, 5, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[34]);
    adder_size_k(&upc, &rotated_syndrome, 7, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[35]);
    adder_size_k(&upc, &rotated_syndrome, 8, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[36]);
    adder_size_k(&upc, &rotated_syndrome, 7, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[37]);
    adder_size_k(&upc, &rotated_syndrome, 5, 2);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[38]);
    adder_size_k(&upc, &rotated_syndrome, 8, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[39]);
    adder_size_k(&upc, &rotated_syndrome, 9, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[40]);
    adder_size_k(&upc, &rotated_syndrome, 8, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[41]);
    adder_size_k(&upc, &rotated_syndrome, 10, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[42]);
    adder_size_k(&upc, &rotated_syndrome, 11, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[43]);
    adder_size_k(&upc, &rotated_syndrome, 10, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[44]);
    adder_size_k(&upc, &rotated_syndrome, 8, 2);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[45]);
    adder_size_k(&upc, &rotated_syndrome, 5, 3);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[46]);
    adder_size_k(&upc, &rotated_syndrome, 9, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[47]);
    adder_size_k(&upc, &rotated_syndrome, 10, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[48]);
    adder_size_k(&upc, &rotated_syndrome, 9, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[49]);
    adder_size_k(&upc, &rotated_syndrome, 11, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[50]);
    adder_size_k(&upc, &rotated_syndrome, 12, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[51]);
    adder_size_k(&upc, &rotated_syndrome, 11, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[52]);
    adder_size_k(&upc, &rotated_syndrome, 9, 2);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[53]);
    adder_size_k(&upc, &rotated_syndrome, 12, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[54]);
    adder_size_k(&upc, &rotated_syndrome, 13, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[55]);
    adder_size_k(&upc, &rotated_syndrome, 12, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[56]);
    adder_size_k(&upc, &rotated_syndrome, 14, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[57]);
    adder_size_k(&upc, &rotated_syndrome, 15, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[58]);
    adder_size_k(&upc, &rotated_syndrome, 14, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[59]);
    adder_size_k(&upc, &rotated_syndrome, 12, 2);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[60]);
    adder_size_k(&upc, &rotated_syndrome, 9, 3);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[61]);
    adder_size_k(&upc, &rotated_syndrome, 5, 4);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[62]);
    adder_size_k(&upc, &rotated_syndrome, 0, 5);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[63]);
    adder_size_k(&upc, &rotated_syndrome, 6, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[64]);
    adder_size_k(&upc, &rotated_syndrome, 7, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[65]);
    adder_size_k(&upc, &rotated_syndrome, 6, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[66]);
    adder_size_k(&upc, &rotated_syndrome, 8, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[67]);
    adder_size_k(&upc, &rotated_syndrome, 9, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[68]);
    adder_size_k(&upc, &rotated_syndrome, 8, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[69]);
    adder_size_k(&upc, &rotated_syndrome, 6, 2);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[70]);
    adder_size_k(&upc, &rotated_syndrome, 9, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[71]);
    adder_size_k(&upc, &rotated_syndrome, 10, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[72]);
    adder_size_k(&upc, &rotated_syndrome, 9, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[73]);
    adder_size_k(&upc, &rotated_syndrome, 11, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[74]);
    adder_size_k(&upc, &rotated_syndrome, 12, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[75]);
    adder_size_k(&upc, &rotated_syndrome, 11, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[76]);
    adder_size_k(&upc, &rotated_syndrome, 9, 2);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[77]);
    adder_size_k(&upc, &rotated_syndrome, 6, 3);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[78]);
    adder_size_k(&upc, &rotated_syndrome, 10, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[79]);
    adder_size_k(&upc, &rotated_syndrome, 11, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[80]);
    adder_size_k(&upc, &rotated_syndrome, 10, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[81]);
    adder_size_k(&upc, &rotated_syndrome, 12, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[82]);
    adder_size_k(&upc, &rotated_syndrome, 13, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[83]);
    adder_size_k(&upc, &rotated_syndrome, 12, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[84]);
    adder_size_k(&upc, &rotated_syndrome, 10, 2);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[85]);
    adder_size_k(&upc, &rotated_syndrome, 13, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[86]);
    adder_size_k(&upc, &rotated_syndrome, 14, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[87]);
    adder_size_k(&upc, &rotated_syndrome, 13, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[88]);
    adder_size_k(&upc, &rotated_syndrome, 15, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[89]);
    adder_size_k(&upc, &rotated_syndrome, 16, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[90]);
    adder_size_k(&upc, &rotated_syndrome, 15, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[91]);
    adder_size_k(&upc, &rotated_syndrome, 13, 2);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[92]);
    adder_size_k(&upc, &rotated_syndrome, 10, 3);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[93]);
    adder_size_k(&upc, &rotated_syndrome, 6, 4);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[94]);
    adder_size_k(&upc, &rotated_syndrome, 11, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[95]);
    adder_size_k(&upc, &rotated_syndrome, 12, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[96]);
    adder_size_k(&upc, &rotated_syndrome, 11, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[97]);
    adder_size_k(&upc, &rotated_syndrome, 13, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[98]);
    adder_size_k(&upc, &rotated_syndrome, 14, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[99]);
    adder_size_k(&upc, &rotated_syndrome, 13, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[100]);
    adder_size_k(&upc, &rotated_syndrome, 11, 2);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[101]);
    adder_size_53(&upc, &rotated_syndrome, 6);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[102]);
    adder_size_k(&upc, &rotated_syndrome, 0, 6);
#endif
    for(int j = 0; j < R_QWORDS; j++) 
    {
      upc.slice[7].u.qw[j] = 0;
    }

    // 2) Subtract the threshold from the UPC counters
    bit_slice_full_subtract(&upc, threshold);

    // 3) Update the errors and the black errors vectors.
    //    The last slice of the UPC array holds the MSB of the accumulated values
    //    minus the threshold. Every zero bit indicates a potential error bit.
    //    The errors values are stored in the black array and xored with the
    //    errors Of the previous iteration.
    const r_t *last_slice = &(upc.slice[SLICES - 1].u.r.val);
    for(size_t j = 0; j < R_BYTES; j++) {
      const uint8_t sum_msb  = (~last_slice->raw[j]);
      black_e->val[i].raw[j] = sum_msb;
      e->val[i].raw[j] ^= sum_msb;
    }

    // Ensure that the padding bits (upper bits of the last byte) are zero so
    // they will not be included in the multiplication and in the hash function.
    e->val[i].raw[R_BYTES - 1] &= LAST_R_BYTE_MASK;

    // 4) Calculate the gray error array by adding "DELTA" to the UPC array.
    //    For that we reuse the rotated_syndrome variable setting it to all "1".
    for(size_t l = 0; l < DELTA; l++) {
      bike_memset((uint8_t *)rotated_syndrome.qw, 0xff, R_BYTES);
      bit_sliced_adder(&upc, &rotated_syndrome, SLICES);
    }

    // 5) Update the gray list with the relevant bits that are not
    //    set in the black list.
    for(size_t j = 0; j < R_BYTES; j++) {
      const uint8_t sum_msb = (~last_slice->raw[j]);
      gray_e->val[i].raw[j] = (~(black_e->val[i].raw[j])) & sum_msb;
    }
  }
}

// Recalculate the UPCs and update the errors vector (e) according to it
// and to the black/gray vectors.
_INLINE_ void find_err2(OUT e_t *e,
                        IN e_t * pos_e,
                        IN syndrome_t *          syndrome,
                        IN const compressed_idx_d_ar_t wlist,
                        IN const uint8_t               threshold)
{
  DEFER_CLEANUP(syndrome_t rotated_syndrome = {0}, syndrome_cleanup);
  DEFER_CLEANUP(upc_t upc, upc_cleanup);

#  if(LEVEL == 1)

	__m256i buf[128];

	{
		int j;

		for (j = 0; j < 97; j++)
			buf[j] = LOAD(&syndrome->qw[4 * j]);

		for (j = 97; j < 128; j++)
			_mm256_setzero_si256();

		transpose_64x256_sp_asm(buf);
		transpose_64x256_sp_asm(buf + 64);
	}

#elif(LEVEL == 3)
	__m256i buf[384];

	{
		int i, j;
		__m256i x, y, v[3][2];
	
		//
	
		for (j = 0; j < 193; j++)
			buf[j] = LOAD(&syndrome->qw[4 * j]);
	
		for (j = 193; j < 256; j++)
			buf[j] = _mm256_setzero_si256();
	
		for (j = 0; j < 128; j++)
		{
			x = _mm256_permute2x128_si256(buf[j], buf[j+128], 0x20);
			y = _mm256_permute2x128_si256(buf[j], buf[j+128], 0x31);
			buf[j] = x;
			buf[j+128] = y;
		}

		for (j = 0; j < 64; j++)
		{
			x = _mm256_unpacklo_epi64(buf[j], buf[j+64]);
			y = _mm256_unpackhi_epi64(buf[j], buf[j+64]);
			buf[j] = x;
			buf[j+64] = y;
		}

		for (j = 128; j < 192; j++)
		{
			x = _mm256_unpacklo_epi64(buf[j], buf[j+64]);
			y = _mm256_unpackhi_epi64(buf[j], buf[j+64]);
			buf[j] = x;
			buf[j+64] = y;
		}

		transpose_64x256_sp_asm(buf);
		transpose_64x256_sp_asm(buf + 64);
		transpose_64x256_sp_asm(buf + 128);
		transpose_64x256_sp_asm(buf + 192);

		for (j = 0; j < 128; j++)
		{
			v[0][0] = buf[j];
			v[1][0] = PERM_I64(buf[j], 0x39);
			v[2][0] = PERM_I64(buf[j], 0x4E);
			v[0][1] = buf[j+128];
			v[1][1] = PERM_I64(buf[j+128], 0x39);
			v[2][1] = PERM_I64(buf[j+128], 0x4E);

			buf[j]     = _mm256_permute2x128_si256(v[0][0], v[0][1], 0x20);
			buf[j+128] = _mm256_permute2x128_si256(v[1][0], v[1][1], 0x20);
			buf[j+256] = _mm256_permute2x128_si256(v[2][0], v[2][1], 0x20);
		}

		for (i = 0; i < 384; i += 128)
		for (j = 0; j < 64; j++)
		{
			x = _mm256_unpacklo_epi64(buf[i+j], buf[i+j+64]);
			y = _mm256_unpackhi_epi64(buf[i+j], buf[i+j+64]);
			buf[i+j] = x;
			buf[i+j+64] = y;
		}
	}
#endif

  for(uint32_t i = 0; i < N0; i++) {
    // UPC must start from zero at every iteration
    bike_memset(&upc, 0, sizeof(upc));

    // 1) Right-rotate the syndrome, for every index of a set bit in the secret
    // key. Then slice-add it to the UPC array.

#  if(LEVEL == 1)
    rotate_right(&rotated_syndrome, buf, wlist[i].val[0]);
    adder_size_k(&upc, &rotated_syndrome, 0, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[1]);
    adder_size_k(&upc, &rotated_syndrome, 1, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[2]);
    adder_size_k(&upc, &rotated_syndrome, 0, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[3]);
    adder_size_k(&upc, &rotated_syndrome, 2, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[4]);
    adder_size_k(&upc, &rotated_syndrome, 3, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[5]);
    adder_size_k(&upc, &rotated_syndrome, 2, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[6]);
    adder_size_k(&upc, &rotated_syndrome, 0, 2);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[7]);
    adder_size_k(&upc, &rotated_syndrome, 3, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[8]);
    adder_size_k(&upc, &rotated_syndrome, 4, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[9]);
    adder_size_k(&upc, &rotated_syndrome, 3, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[10]);
    adder_size_k(&upc, &rotated_syndrome, 5, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[11]);
    adder_size_k(&upc, &rotated_syndrome, 6, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[12]);
    adder_size_k(&upc, &rotated_syndrome, 5, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[13]);
    adder_size_k(&upc, &rotated_syndrome, 3, 2);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[14]);
    adder_size_k(&upc, &rotated_syndrome, 0, 3);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[15]);
    adder_size_k(&upc, &rotated_syndrome, 4, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[16]);
    adder_size_k(&upc, &rotated_syndrome, 5, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[17]);
    adder_size_k(&upc, &rotated_syndrome, 4, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[18]);
    adder_size_k(&upc, &rotated_syndrome, 6, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[19]);
    adder_size_k(&upc, &rotated_syndrome, 7, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[20]);
    adder_size_k(&upc, &rotated_syndrome, 6, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[21]);
    adder_size_k(&upc, &rotated_syndrome, 4, 2);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[22]);
    adder_size_k(&upc, &rotated_syndrome, 7, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[23]);
    adder_size_k(&upc, &rotated_syndrome, 8, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[24]);
    adder_size_k(&upc, &rotated_syndrome, 7, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[25]);
    adder_size_k(&upc, &rotated_syndrome, 9, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[26]);
    adder_size_k(&upc, &rotated_syndrome, 10, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[27]);
    adder_size_k(&upc, &rotated_syndrome, 9, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[28]);
    adder_size_k(&upc, &rotated_syndrome, 7, 2);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[29]);
    adder_size_k(&upc, &rotated_syndrome, 4, 3);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[30]);
    adder_size_k(&upc, &rotated_syndrome, 0, 4);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[31]);
    adder_size_k(&upc, &rotated_syndrome, 5, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[32]);
    adder_size_k(&upc, &rotated_syndrome, 6, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[33]);
    adder_size_k(&upc, &rotated_syndrome, 5, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[34]);
    adder_size_k(&upc, &rotated_syndrome, 7, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[35]);
    adder_size_k(&upc, &rotated_syndrome, 8, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[36]);
    adder_size_k(&upc, &rotated_syndrome, 7, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[37]);
    adder_size_k(&upc, &rotated_syndrome, 5, 2);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[38]);
    adder_size_k(&upc, &rotated_syndrome, 8, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[39]);
    adder_size_k(&upc, &rotated_syndrome, 9, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[40]);
    adder_size_k(&upc, &rotated_syndrome, 8, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[41]);
    adder_size_k(&upc, &rotated_syndrome, 10, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[42]);
    adder_size_k(&upc, &rotated_syndrome, 11, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[43]);
    adder_size_k(&upc, &rotated_syndrome, 10, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[44]);
    adder_size_k(&upc, &rotated_syndrome, 8, 2);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[45]);
    adder_size_k(&upc, &rotated_syndrome, 5, 3);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[46]);
    adder_size_k(&upc, &rotated_syndrome, 9, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[47]);
    adder_size_k(&upc, &rotated_syndrome, 10, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[48]);
    adder_size_k(&upc, &rotated_syndrome, 9, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[49]);
    adder_size_k(&upc, &rotated_syndrome, 11, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[50]);
    adder_size_k(&upc, &rotated_syndrome, 12, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[51]);
    adder_size_k(&upc, &rotated_syndrome, 11, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[52]);
    adder_size_k(&upc, &rotated_syndrome, 9, 2);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[53]);
    adder_size_k(&upc, &rotated_syndrome, 12, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[54]);
    adder_size_k(&upc, &rotated_syndrome, 13, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[55]);
    adder_size_k(&upc, &rotated_syndrome, 12, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[56]);
    adder_size_k(&upc, &rotated_syndrome, 14, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[57]);
    adder_size_k(&upc, &rotated_syndrome, 15, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[58]);
    adder_size_k(&upc, &rotated_syndrome, 14, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[59]);
    adder_size_k(&upc, &rotated_syndrome, 12, 2);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[60]);
    adder_size_k(&upc, &rotated_syndrome, 9, 3);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[61]);
    adder_size_k(&upc, &rotated_syndrome, 5, 4);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[62]);
    adder_size_k(&upc, &rotated_syndrome, 0, 5);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[63]);
    adder_size_k(&upc, &rotated_syndrome, 6, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[64]);
    adder_size_k(&upc, &rotated_syndrome, 7, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[65]);
    adder_size_k(&upc, &rotated_syndrome, 6, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[66]);
    adder_size_k(&upc, &rotated_syndrome, 8, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[67]);
    adder_size_k(&upc, &rotated_syndrome, 9, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[68]);
    adder_size_k(&upc, &rotated_syndrome, 8, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[69]);
    adder_size_k(&upc, &rotated_syndrome, 6, 2);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[70]);
    adder_size_63(&upc, &rotated_syndrome, 0);
#  elif(LEVEL == 3)
    rotate_right(&rotated_syndrome, buf, wlist[i].val[0]);
    adder_size_k(&upc, &rotated_syndrome, 0, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[1]);
    adder_size_k(&upc, &rotated_syndrome, 1, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[2]);
    adder_size_k(&upc, &rotated_syndrome, 0, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[3]);
    adder_size_k(&upc, &rotated_syndrome, 2, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[4]);
    adder_size_k(&upc, &rotated_syndrome, 3, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[5]);
    adder_size_k(&upc, &rotated_syndrome, 2, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[6]);
    adder_size_k(&upc, &rotated_syndrome, 0, 2);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[7]);
    adder_size_k(&upc, &rotated_syndrome, 3, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[8]);
    adder_size_k(&upc, &rotated_syndrome, 4, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[9]);
    adder_size_k(&upc, &rotated_syndrome, 3, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[10]);
    adder_size_k(&upc, &rotated_syndrome, 5, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[11]);
    adder_size_k(&upc, &rotated_syndrome, 6, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[12]);
    adder_size_k(&upc, &rotated_syndrome, 5, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[13]);
    adder_size_k(&upc, &rotated_syndrome, 3, 2);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[14]);
    adder_size_k(&upc, &rotated_syndrome, 0, 3);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[15]);
    adder_size_k(&upc, &rotated_syndrome, 4, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[16]);
    adder_size_k(&upc, &rotated_syndrome, 5, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[17]);
    adder_size_k(&upc, &rotated_syndrome, 4, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[18]);
    adder_size_k(&upc, &rotated_syndrome, 6, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[19]);
    adder_size_k(&upc, &rotated_syndrome, 7, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[20]);
    adder_size_k(&upc, &rotated_syndrome, 6, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[21]);
    adder_size_k(&upc, &rotated_syndrome, 4, 2);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[22]);
    adder_size_k(&upc, &rotated_syndrome, 7, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[23]);
    adder_size_k(&upc, &rotated_syndrome, 8, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[24]);
    adder_size_k(&upc, &rotated_syndrome, 7, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[25]);
    adder_size_k(&upc, &rotated_syndrome, 9, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[26]);
    adder_size_k(&upc, &rotated_syndrome, 10, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[27]);
    adder_size_k(&upc, &rotated_syndrome, 9, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[28]);
    adder_size_k(&upc, &rotated_syndrome, 7, 2);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[29]);
    adder_size_k(&upc, &rotated_syndrome, 4, 3);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[30]);
    adder_size_k(&upc, &rotated_syndrome, 0, 4);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[31]);
    adder_size_k(&upc, &rotated_syndrome, 5, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[32]);
    adder_size_k(&upc, &rotated_syndrome, 6, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[33]);
    adder_size_k(&upc, &rotated_syndrome, 5, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[34]);
    adder_size_k(&upc, &rotated_syndrome, 7, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[35]);
    adder_size_k(&upc, &rotated_syndrome, 8, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[36]);
    adder_size_k(&upc, &rotated_syndrome, 7, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[37]);
    adder_size_k(&upc, &rotated_syndrome, 5, 2);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[38]);
    adder_size_k(&upc, &rotated_syndrome, 8, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[39]);
    adder_size_k(&upc, &rotated_syndrome, 9, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[40]);
    adder_size_k(&upc, &rotated_syndrome, 8, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[41]);
    adder_size_k(&upc, &rotated_syndrome, 10, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[42]);
    adder_size_k(&upc, &rotated_syndrome, 11, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[43]);
    adder_size_k(&upc, &rotated_syndrome, 10, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[44]);
    adder_size_k(&upc, &rotated_syndrome, 8, 2);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[45]);
    adder_size_k(&upc, &rotated_syndrome, 5, 3);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[46]);
    adder_size_k(&upc, &rotated_syndrome, 9, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[47]);
    adder_size_k(&upc, &rotated_syndrome, 10, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[48]);
    adder_size_k(&upc, &rotated_syndrome, 9, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[49]);
    adder_size_k(&upc, &rotated_syndrome, 11, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[50]);
    adder_size_k(&upc, &rotated_syndrome, 12, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[51]);
    adder_size_k(&upc, &rotated_syndrome, 11, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[52]);
    adder_size_k(&upc, &rotated_syndrome, 9, 2);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[53]);
    adder_size_k(&upc, &rotated_syndrome, 12, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[54]);
    adder_size_k(&upc, &rotated_syndrome, 13, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[55]);
    adder_size_k(&upc, &rotated_syndrome, 12, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[56]);
    adder_size_k(&upc, &rotated_syndrome, 14, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[57]);
    adder_size_k(&upc, &rotated_syndrome, 15, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[58]);
    adder_size_k(&upc, &rotated_syndrome, 14, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[59]);
    adder_size_k(&upc, &rotated_syndrome, 12, 2);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[60]);
    adder_size_k(&upc, &rotated_syndrome, 9, 3);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[61]);
    adder_size_k(&upc, &rotated_syndrome, 5, 4);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[62]);
    adder_size_k(&upc, &rotated_syndrome, 0, 5);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[63]);
    adder_size_k(&upc, &rotated_syndrome, 6, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[64]);
    adder_size_k(&upc, &rotated_syndrome, 7, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[65]);
    adder_size_k(&upc, &rotated_syndrome, 6, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[66]);
    adder_size_k(&upc, &rotated_syndrome, 8, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[67]);
    adder_size_k(&upc, &rotated_syndrome, 9, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[68]);
    adder_size_k(&upc, &rotated_syndrome, 8, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[69]);
    adder_size_k(&upc, &rotated_syndrome, 6, 2);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[70]);
    adder_size_k(&upc, &rotated_syndrome, 9, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[71]);
    adder_size_k(&upc, &rotated_syndrome, 10, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[72]);
    adder_size_k(&upc, &rotated_syndrome, 9, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[73]);
    adder_size_k(&upc, &rotated_syndrome, 11, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[74]);
    adder_size_k(&upc, &rotated_syndrome, 12, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[75]);
    adder_size_k(&upc, &rotated_syndrome, 11, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[76]);
    adder_size_k(&upc, &rotated_syndrome, 9, 2);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[77]);
    adder_size_k(&upc, &rotated_syndrome, 6, 3);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[78]);
    adder_size_k(&upc, &rotated_syndrome, 10, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[79]);
    adder_size_k(&upc, &rotated_syndrome, 11, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[80]);
    adder_size_k(&upc, &rotated_syndrome, 10, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[81]);
    adder_size_k(&upc, &rotated_syndrome, 12, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[82]);
    adder_size_k(&upc, &rotated_syndrome, 13, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[83]);
    adder_size_k(&upc, &rotated_syndrome, 12, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[84]);
    adder_size_k(&upc, &rotated_syndrome, 10, 2);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[85]);
    adder_size_k(&upc, &rotated_syndrome, 13, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[86]);
    adder_size_k(&upc, &rotated_syndrome, 14, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[87]);
    adder_size_k(&upc, &rotated_syndrome, 13, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[88]);
    adder_size_k(&upc, &rotated_syndrome, 15, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[89]);
    adder_size_k(&upc, &rotated_syndrome, 16, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[90]);
    adder_size_k(&upc, &rotated_syndrome, 15, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[91]);
    adder_size_k(&upc, &rotated_syndrome, 13, 2);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[92]);
    adder_size_k(&upc, &rotated_syndrome, 10, 3);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[93]);
    adder_size_k(&upc, &rotated_syndrome, 6, 4);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[94]);
    adder_size_k(&upc, &rotated_syndrome, 11, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[95]);
    adder_size_k(&upc, &rotated_syndrome, 12, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[96]);
    adder_size_k(&upc, &rotated_syndrome, 11, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[97]);
    adder_size_k(&upc, &rotated_syndrome, 13, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[98]);
    adder_size_k(&upc, &rotated_syndrome, 14, 0);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[99]);
    adder_size_k(&upc, &rotated_syndrome, 13, 1);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[100]);
    adder_size_k(&upc, &rotated_syndrome, 11, 2);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[101]);
    adder_size_53(&upc, &rotated_syndrome, 6);
    rotate_right(&rotated_syndrome, buf, wlist[i].val[102]);
    adder_size_k(&upc, &rotated_syndrome, 0, 6);
#endif

    for(int j = 0; j < R_QWORDS; j++) 
    {
      upc.slice[7].u.qw[j] = 0;
    }

    // 2) Subtract the threshold from the UPC counters
    bit_slice_full_subtract(&upc, threshold);

    // 3) Update the errors vector.
    //    The last slice of the UPC array holds the MSB of the accumulated values
    //    minus the threshold. Every zero bit indicates a potential error bit.
    const r_t *last_slice = &(upc.slice[SLICES - 1].u.r.val);
    for(size_t j = 0; j < R_BYTES; j++) {
      const uint8_t sum_msb = (~last_slice->raw[j]);
      e->val[i].raw[j] ^= (pos_e->val[i].raw[j] & sum_msb);
    }

    // Ensure that the padding bits (upper bits of the last byte) are zero, so
    // they are not included in the multiplication, and in the hash function.
    e->val[i].raw[R_BYTES - 1] &= LAST_R_BYTE_MASK;
  }
}

ret_t decode(OUT e_t *e, IN const ct_t *ct, IN const sk_t *sk)
{
  DEFER_CLEANUP(e_t black_e = {0}, e_cleanup);
  DEFER_CLEANUP(e_t gray_e = {0}, e_cleanup);

  DEFER_CLEANUP(pad_r_t c0 = {0}, pad_r_cleanup);
  DEFER_CLEANUP(pad_r_t h0 = {0}, pad_r_cleanup);
  pad_r_t pk = {0};

  // Pad ciphertext (c0), secret key (h0), and public key (h)
  c0.val = ct->c0;
  h0.val = sk->bin[0];
  pk.val = sk->pk;

  DEFER_CLEANUP(syndrome_t s = {0}, syndrome_cleanup);
  DMSG("  Computing s.\n");
  GUARD(compute_syndrome(&s, &c0, &h0));
  dup(&s);

  // Reset (init) the error because it is xored in the find_err functions.
  bike_memset(e, 0, sizeof(*e));

  for(uint32_t iter = 0; iter < MAX_IT; iter++) {
    const uint8_t threshold = get_threshold(&s);

    DMSG("    Iteration: %d\n", iter);
    DMSG("    Weight of e: %lu\n",
         r_bits_vector_weight(&e->val[0]) + r_bits_vector_weight(&e->val[1]));
    DMSG("    Weight of syndrome: %lu\n", r_bits_vector_weight((r_t *)s.qw));

    find_err1(e, &black_e, &gray_e, &s, sk->wlist, threshold);
    GUARD(recompute_syndrome(&s, &c0, &h0, &pk, e));
#if defined(BGF_DECODER)
    if(iter >= 1) {
      continue;
    }
#endif
    DMSG("    Weight of e: %lu\n",
         r_bits_vector_weight(&e->val[0]) + r_bits_vector_weight(&e->val[1]));
    DMSG("    Weight of syndrome: %lu\n", r_bits_vector_weight((r_t *)s.qw));

    find_err2(e, &black_e, &s, sk->wlist, ((D + 1) / 2) + 1);
    GUARD(recompute_syndrome(&s, &c0, &h0, &pk, e));

    DMSG("    Weight of e: %lu\n",
         r_bits_vector_weight(&e->val[0]) + r_bits_vector_weight(&e->val[1]));
    DMSG("    Weight of syndrome: %lu\n", r_bits_vector_weight((r_t *)s.qw));

    find_err2(e, &gray_e, &s, sk->wlist, ((D + 1) / 2) + 1);
    GUARD(recompute_syndrome(&s, &c0, &h0, &pk, e));
  }

  if(r_bits_vector_weight((r_t *)s.qw) > 0) {
    BIKE_ERROR(E_DECODING_FAILURE);
  }

  return SUCCESS;
}
