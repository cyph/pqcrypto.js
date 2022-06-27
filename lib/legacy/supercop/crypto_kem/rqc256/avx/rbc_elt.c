/**
 * \file rbc_elt.c
 * \brief Implementation of rbc_elt.h
 */

#include "rbc.h"
#include "rbc_elt.h"
#include "randombytes.h"
#include "seedexpander.h"


/**
 * \fn void rbc_field_init()
 * \brief This function initializes various constants used to perform finite field arithmetic.
 *
 */
void rbc_field_init() {

}



/**
 * \fn void rbc_elt_set_zero(rbc_elt o)
 * \brief This function sets a finite field element to zero.
 *
 * \param[out] o rbc_elt
 */
void rbc_elt_set_zero(rbc_elt o) {
  o[0] = 0;
  o[1] = 0;
  o[2] = 0;
  o[3] = 0;
}



/**
 * \fn void rbc_elt_set_one(rbc_elt o)
 * \brief This function sets a finite field element to one.
 *
 * \param[out] o rbc_elt
 */
void rbc_elt_set_one(rbc_elt o) {
  o[0] = 1;
  o[1] = 0;
  o[2] = 0;
  o[3] = 0;
}



/**
 * \fn void rbc_elt_set(rbc_elt o, const rbc_elt e)
 * \brief This function copies a finite field element into another one.
 *
 * \param[out] o rbc_elt
 * \param[in] e rbc_elt
 */
void rbc_elt_set(rbc_elt o, const rbc_elt e) {
  o[0] = e[0];
  o[1] = e[1];
  o[2] = e[2];
  o[3] = 0;
}



/**
 * \fn void rbc_elt_set_mask1(rbc_elt o, const rbc_elt e1, const rbc_elt e2, uint32_t mask)
 * \brief This function copies either e1 or e2 into o depending on the mask value
 *
 * \param[out] o rbc_elt
 * \param[in] e1 rbc_elt
 * \param[in] e2 rbc_elt
 * \param[in] mask 1 to copy e1 and 0 to copy e2
 */
void rbc_elt_set_mask1(rbc_elt o, const rbc_elt e1, const rbc_elt e2, uint32_t mask) {
  o[0] = mask * e1[0] + (1 - mask) * e2[0];
  o[1] = mask * e1[1] + (1 - mask) * e2[1];
  o[2] = mask * e1[2] + (1 - mask) * e2[2];
  o[3] = 0;
}



/**
 * \fn void rbc_elt_set(rbc_elt o1, rbc_elt o2, const rbc_elt e, uint32_t mask)
 * \brief This function copies e either into o1 or o2 depending on the mask value
 *
 * \param[out] o1 rbc_elt
 * \param[out] o2 rbc_elt
 * \param[in] e rbc_elt
 * \param[in] mask 1 to copy into o1 and 0 to copy into o2
 */
void rbc_elt_set_mask2(rbc_elt o1, rbc_elt o2, const rbc_elt e, uint32_t mask) {
  o1[0] = mask * e[0] + (1 - mask) * o1[0];
  o2[0] = mask * e[0] + (1 - mask) * o2[0];

  o1[1] = mask * e[1] + (1 - mask) * o1[1];
  o2[1] = mask * e[1] + (1 - mask) * o2[1];

  o1[2] = mask * e[2] + (1 - mask) * o1[2];
  o2[2] = mask * e[2] + (1 - mask) * o2[2];

  o1[3] = 0;
  o2[3] = 0;
}



/**
 * \fn void rbc_elt_set_from_uint64(rbc_elt o, uint64_t* rbc_elt e)
 * \brief This function set a finite field element from a pointer to uint64_t.
 *
 * \param[out] o rbc_elt
 * \param[in] e Pointer to uint64_t
 */
void rbc_elt_set_from_uint64(rbc_elt o, const uint64_t* e) {
  o[0] = e[0];
  o[1] = e[1];
  o[2] = e[2];
  o[3] = 0;
}



/**
 * \fn void rbc_elt_set_random(rbc_elt o, AES_XOF_struct* ctx)
 * \brief This function sets a finite field element with random values using the NIST seed expander.
 *
 * \param[out] o rbc_elt
 * \param[out] ctx NIST seed expander
 */
void rbc_elt_set_random(rbc_elt o, AES_XOF_struct* ctx) {
  uint32_t bytes = (RBC_FIELD_M % 8 == 0) ? RBC_FIELD_M / 8 : RBC_FIELD_M / 8 + 1;
  uint8_t random[bytes];
  uint8_t mask = (1 << RBC_FIELD_M % 8) - 1;

  rbc_elt_set_zero(o);
  seedexpander(ctx, random, bytes);
  random[bytes - 1] &= mask;
  memcpy((uint64_t*) o, random, bytes);
}



/**
 * \fn void rbc_elt_set_random2(rbc_elt o)
 * \brief This function sets a finite field element with random values using randombytes.
 *
 * \param[out] o rbc_elt
 */
void rbc_elt_set_random2(rbc_elt o) {
  uint32_t bytes = (RBC_FIELD_M % 8 == 0) ? RBC_FIELD_M / 8 : RBC_FIELD_M / 8 + 1;
  uint8_t random[bytes];
  uint8_t mask = (1 << RBC_FIELD_M % 8) - 1;

  rbc_elt_set_zero(o);
  randombytes(random, bytes);
  random[bytes - 1] &= mask;
  memcpy((uint64_t*) o, random, bytes);
}



/**
 * \fn uint8_t rbc_elt_is_zero(const rbc_elt e)
 * \brief This functions tests if a finite field element is equal to zero.
 *
 * \param[in] e rbc_elt
 * \return 1 if <b>e</b> is equal to zero, 0 otherwise
 */
uint8_t rbc_elt_is_zero(const rbc_elt e) {
  return e[2] == 0 && e[1] == 0 && e[0] == 0;
}



/**
 * \fn uint8_t rbc_elt_is_equal_to(const rbc_elt e1, const rbc_elt e2)
 * \brief This functions tests if two finite field elements are equal.
 *
 * \param[in] e1 rbc_elt
 * \param[in] e2 rbc_elt
 * \return 1 if <b>e1</b> and <b>e2</b> are equal, 0 otherwise
 */
uint8_t rbc_elt_is_equal_to(const rbc_elt e1, const rbc_elt e2) {
  return e1[0] == e2[0] && e1[1] == e2[1] && e1[2] == e2[2];
}



/**
 * \fn uint8_t rbc_elt_is_greater_than(const rbc_elt e1, const rbc_elt e2)
 * \brief This functions compares two finite field elements.
 *
 * \param[in] e1 rbc_elt
 * \param[in] e2 rbc_elt
 * \return 1 if <b>e1</b> > <b>e2</b>, 0 otherwise
 */
uint8_t rbc_elt_is_greater_than(const rbc_elt e1, const rbc_elt e2) {
  if(e1[2] > e2[2]) {
    return 1;
  }

  if(e1[2] < e2[2]) {
    return 0;
  }

  if(e1[2] == e2[2] && e1[1] > e2[1]) {
    return 1;
  }

  if(e1[2] == e2[2] && e1[1] == e2[1] && e1[0] > e2[0]) {
    return 1;
  }

  return 0;
}



/**
 * \fn rbc_elt_add(rbc_elt o, const rbc_elt e1, const rbc_elt e2)
 * \brief This function adds two finite field elements.
 *
 * \param[out] o Sum of <b>e1</b> and <b>e2</b>
 * \param[in] e1 rbc_elt
 * \param[in] e2 rbc_elt
 */
void rbc_elt_add(rbc_elt o, const rbc_elt e1, const rbc_elt e2) {
  __m128i tmp1 = _mm_load_si128((__m128i*) e1);
  __m128i tmp2 = _mm_load_si128((__m128i*) e2);
  tmp1 = _mm_xor_si128(tmp1, tmp2);
  _mm_store_si128((__m128i*) o, tmp1);

  tmp1 = _mm_load_si128((__m128i*) (e1 + 2));
  tmp2 = _mm_load_si128((__m128i*) (e2 + 2));
  tmp1 = _mm_xor_si128(tmp1, tmp2);
  _mm_store_si128((__m128i*) (o + 2), tmp1);
}



/**
 * \fn void rbc_elt_mul(rbc_elt o, const rbc_elt e1, const rbc_elt e2)
 * \brief This function multiplies two finite field elements.
 *
 * \param[out] o Product of <b>e1</b> and <b>e2</b>
 * \param[in] e1 rbc_elt
 * \param[in] e2 rbc_elt
 */
void rbc_elt_mul(rbc_elt o, const rbc_elt e1, const rbc_elt e2) {
  rbc_elt_ur tmp;
  rbc_elt_ur_set_zero(tmp);
  rbc_elt_ur_mul(tmp, e1, e2);
  rbc_elt_reduce(o, tmp);
}



/**
 * \fn int32_t rbc_elt_get_degree(const rbc_elt e)
 * \brief This function returns the degree of a finite field element.
 *
 * \param[out] degree of <b>e</b>
 * \param[in] e rbc_elt
 */
int32_t rbc_elt_get_degree(const rbc_elt e) {
  int64_t index = 0;

  if(e[2] != 0) {
    __asm__ volatile("bsr %1,%0;" : "=r"(index) : "r"(e[2]));
    return index + 128;
  }

  if(e[1] != 0) {
    __asm__ volatile("bsr %1,%0;" : "=r"(index) : "r"(e[1]));
    return index + 64;
  }

  if(e[0] != 0) {
    __asm__ volatile("bsr %1,%0;" : "=r"(index) : "r"(e[0]));
    return index + 0;
  }

  return -1;
}



/**
 * \fn uint8_t rbc_elt_get_coefficient(const rbc_elt e, uint32_t index)
 * \brief This function returns the coefficient of the polynomial <b>e</b> at a given index.
 *
 * \param[in] e rbc_elt
 * \param[in] index Index of the coefficient
 * \return Coefficient of <b>e</b> at the given index
 */
uint8_t rbc_elt_get_coefficient(const rbc_elt e, uint32_t index) {
  size_t position = index / 64;
  return (e[position] >> (index % 64)) & 0x01;
}



/**
 * \fn void rbc_elt_set_coefficient(rbc_elt o, uint32_t index, uint64_t bit)
 * \brief This function set a coefficient of the polynomial <b>e</b>.
 *
 * \param[in] e rbc_elt
 * \param[in] index Index of the coefficient
 * \param[in] bit Value of the coefficient
 */
void rbc_elt_set_coefficient(rbc_elt o, uint32_t index, uint8_t bit) {
  size_t position = index / 64;
  o[position] |= (uint64_t) bit << (index % 64);
}



/**
 * \fn rbc_elt_inv(rbc_elt o, const rbc_elt e)
 * \brief This function computes the multiplicative inverse of a finite field element.
 *
 * \param[out] o rbc_elt
 * \param[in] e rbc_elt
 */
void rbc_elt_inv(rbc_elt o, const rbc_elt e) {
  rbc_elt u, v, g, tmp;
  uint32_t u_degree, v_degree;
  uint64_t carry;
  int32_t j;

  rbc_elt_set(u, e);
  rbc_elt_set(v, RBC_ELT_MODULUS);

  rbc_elt_set_one(o);
  rbc_elt_set_zero(g);

  while(u[0] != 1) {
    u_degree = rbc_elt_get_degree(u);
    v_degree = rbc_elt_get_degree(v);
    j = u_degree - v_degree;

    if(j < 0) {
      rbc_elt_set(tmp, u);
      rbc_elt_set(u, v);
      rbc_elt_set(v, tmp);

      rbc_elt_set(tmp, o);
      rbc_elt_set(o, g);
      rbc_elt_set(g, tmp);

      j -= j;
    }

    if(j == 0) {
      rbc_elt_add(u, u, v);
      rbc_elt_add(o, o, g);
    }

    // 0 < j <= 64
    else if (j <= 64) {
      carry = v[0] >> (64 - j);
      u[0] ^= (v[0] << j);
      u[1] ^= (v[1] << j) ^ carry;
      carry = v[1] >> (64 - j);
      u[2] ^= (v[2] << j) ^ carry;

      carry = g[0] >> (64 - j);
      o[0] ^= g[0] << j;
      o[1] ^= (g[1] << j) ^ carry;
      carry = g[1] >> (64 - j);
      o[2] ^= (g[2] << j) ^ carry;
    }

    // 64 < j <= 128
    else if(j > 64 && j <= 128) {
      u[1] ^= v[0] << (j - 64);
      u[2] ^= v[0] >> (j - 128);

      o[1] ^= g[0] << (j - 64);
      o[2] ^= g[0] >> (j - 128);
    }

    // 128 < j <= M
    else {
      u[2] ^= v[0] << (j - 128);
      o[2] ^= g[0] << (j - 128);
    }
  }
}



/**
 * \fn void rbc_elt_sqr(rbc_elt o, const rbc_elt e)
 * \brief This function computes the square of a finite field element.
 *
 * \param[out] o rbc_elt equal to \f$ e^{2} \f$
 * \param[in] e rbc_elt
 */
void rbc_elt_sqr(rbc_elt o, const rbc_elt e) {
  rbc_elt_ur tmp;
  rbc_elt_ur_sqr(tmp, e);
  rbc_elt_reduce(o, tmp);
}



/**
 * \fn void rbc_elt_nth_root(rbc_elt o, const rbc_elt e, uint32_t n)
 * \brief This function computes the nth root of a finite field element.
 *
 * \param[out] o Nth root of <b>e</b>
 * \param[in] e rbc_elt
 * \param[in] n Parameter defining the nth root
 */
void rbc_elt_nth_root(rbc_elt o, const rbc_elt e, uint32_t n) {
  uint32_t exp = n * (RBC_FIELD_M - 1) % RBC_FIELD_M;

  rbc_elt_sqr(o, e);
  for(size_t i = 0 ; i < exp - 1 ; ++i) {
    rbc_elt_sqr(o, o);
  }
}



/**
 * \fn void rbc_elt_reduce(rbc_elt o, const rbc_elt_ur e)
 * \brief This function reduces a finite field element.
 *
 * \param[out] o rbc_elt equal to \f$ e \pmod f \f$
 * \param[in] e rbc_elt
 */
void rbc_elt_reduce(rbc_elt o, const rbc_elt_ur e) {
  uint64_t tmp = e[5] << 11;
  uint64_t tmp2 = e[4] >> 46;

  o[2] = e[2]
    ^ tmp ^ (tmp << 1) ^ (tmp << 6) ^ (tmp << 7)
    ^ tmp2 ^ (tmp2 >> 1) ^ (tmp2 >> 6) ^ (tmp2 >> 7);

  tmp = e[4] << 11;
  tmp2 = e[3] >> 46;

  o[1] = e[1]
    ^ tmp ^ (tmp << 1) ^ (tmp << 6) ^ (tmp << 7)
    ^ tmp2 ^ (tmp2 >> 1) ^ (tmp2 >> 6) ^ (tmp2 >> 7);

  tmp = (e[3] << 11) ^ (o[2] >> 53);
  o[0] = e[0] ^ tmp ^ (tmp << 1) ^ (tmp << 6) ^ (tmp << 7);

  o[2] &= 0x001FFFFFFFFFFFFF;
}




/**
 * \fn void rbc_elt_print(const rbc_elt e)
 * \brief This function displays a finite field element.
 *
 * \param[in] e rbc_elt
 */
void rbc_elt_print(const rbc_elt e) {
  printf("[ ");
  printf("%lu %lu %lu", e[0], e[1], e[2]);
  printf(" ]");
}



/**
 * \fn void rbc_elt_set_zero(rbc_elt o)
 * \brief This function sets a finite field element to zero.
 *
 * \param[out] o rbc_elt
 */
void rbc_elt_ur_set_zero(rbc_elt_ur o) {
  o[0] = 0;
  o[1] = 0;
  o[2] = 0;
  o[3] = 0;
  o[4] = 0;
  o[5] = 0;
}



/**
 * \fn void rbc_elt_ur_set_from_uint64(rbc_elt_ur o, const uint64_t* e)
 * \brief This function set a finite field element from a pointer to uint64_t.
 *
 * \param[out] o rbc_elt_ur
 * \param[in] e Pointer to uint64_t
 */
void rbc_elt_ur_set_from_uint64(rbc_elt_ur o, const uint64_t* e) {
  o[0] = e[0];
  o[1] = e[1];
  o[2] = e[2];
  o[3] = e[3];
  o[4] = e[4];
  o[5] = e[5];
}



/**
 * \fn void rbc_elt_ur_mul(rbc_elt_ur o, const rbc_elt e1, const rbc_elt e2)
 * \brief This function computes the unreduced multiplication of two finite field elements.
 *
 * \param[out] o rbc_elt equal to \f$ e_1 \times e_2 \f$
 * \param[in] e1 rbc_elt
 * \param[in] e2 rbc_elt
 */
void rbc_elt_ur_mul(rbc_elt_ur o, const rbc_elt e1, const rbc_elt e2) {
  __m128i a = _mm_load_si128((__m128i*) e1);
  __m128i b = _mm_load_si128((__m128i*) e2);
  __m128i c = _mm_load_si128((__m128i*) (e1 + 2));
  __m128i d = _mm_load_si128((__m128i*) (e2 + 2));

  __m128i a0_b0 = _mm_clmulepi64_si128(a, b, 0x00);

  __m128i a0_b1 = _mm_clmulepi64_si128(a, b, 0x10);
  __m128i a1_b0 = _mm_clmulepi64_si128(a, b, 0x01);

  __m128i a0_b2 = _mm_clmulepi64_si128(a, d, 0x00);
  __m128i a1_b1 = _mm_clmulepi64_si128(a, b, 0x11);
  __m128i a2_b0 = _mm_clmulepi64_si128(c, b, 0x00);

  __m128i a1_b2 = _mm_clmulepi64_si128(a, d, 0x01);
  __m128i a2_b1 = _mm_clmulepi64_si128(c, b, 0x10);

  __m128i a2_b2 = _mm_clmulepi64_si128(c, d, 0x00);

  __m128i c1 = _mm_xor_si128(a0_b1, a1_b0);
  __m128i c2 = _mm_xor_si128(a0_b2, _mm_xor_si128(a1_b1, a2_b0));
  __m128i c3 = _mm_xor_si128(a1_b2, a2_b1);

  o[0] = _mm_extract_epi64(a0_b0, 0);
  o[1] = _mm_extract_epi64(a0_b0, 1) ^ _mm_extract_epi64(c1, 0);
  o[2] = _mm_extract_epi64(c1, 1) ^ _mm_extract_epi64(c2, 0);
  o[3] = _mm_extract_epi64(c2, 1) ^ _mm_extract_epi64(c3, 0);
  o[4] = _mm_extract_epi64(c3, 1) ^ _mm_extract_epi64(a2_b2, 0);
  o[5] = _mm_extract_epi64(a2_b2, 1);
}



/**
 * \fn void rbc_elt_ur_sqr(rbc_elt o, const rbc_elt e)
 * \brief This function computes the unreduced square of a finite field element.
 *
 * \param[out] o rbc_elt_ur equal to \f$ e^{2} \f$
 * \param[in]  e rbc_elt
 */
void rbc_elt_ur_sqr(rbc_elt_ur o, const rbc_elt e) {
  __m128i e_0, e_l, e_h;
  __m128i* tmp = (__m128i*) malloc(2 * sizeof(__m128i));

  e_0 = _mm_load_si128((__m128i*) e);
  e_l = _mm_and_si128(e_0, RBC_ELT_SQR_MASK_128);
  e_h = _mm_and_si128(_mm_srli_epi64(e_0, 4), RBC_ELT_SQR_MASK_128);

  e_l = _mm_shuffle_epi8(RBC_ELT_SQR_LOOKUP_TABLE_128, e_l);
  e_h = _mm_shuffle_epi8(RBC_ELT_SQR_LOOKUP_TABLE_128, e_h);

  tmp[0] = _mm_unpacklo_epi8(e_l, e_h);
  tmp[1] = _mm_unpackhi_epi8(e_l, e_h);

  _mm_store_si128((__m128i*) o, tmp[0]);
  _mm_store_si128((__m128i*) (o + 2), tmp[1]);

  e_0 = _mm_load_si128((__m128i *) (e + 2));
  e_l = _mm_and_si128(e_0, RBC_ELT_SQR_MASK_128);
  e_h = _mm_and_si128(_mm_srli_epi64(e_0, 4), RBC_ELT_SQR_MASK_128);

  e_l = _mm_shuffle_epi8(RBC_ELT_SQR_LOOKUP_TABLE_128, e_l);
  e_h = _mm_shuffle_epi8(RBC_ELT_SQR_LOOKUP_TABLE_128, e_h);

  tmp[0] = _mm_unpacklo_epi8(e_l, e_h);
  _mm_store_si128((__m128i*) (o + 4), tmp[0]);

  free(tmp);
}



/**
 * \fn void rbc_elt_ur_print(const rbc_elt_ur e)
 * \brief This function displays an unreduced finite field element.
 *
 * \param[in] e rbc_elt_ur
 */
void rbc_elt_ur_print(const rbc_elt_ur e) {
  printf("[ ");
  printf("%lu %lu %lu %lu %lu %lu", e[0], e[1], e[2], e[3], e[4], e[5]);
  printf(" ]");
}

