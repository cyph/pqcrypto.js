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
  static uint32_t init_field = 0;
  uint8_t bit = 0;
  uint64_t mask = 0;

  if(init_field == 0) {
    memset(RBC_SQR_LOOKUP_TABLE, 0, 8*256);
    for(size_t i = 0 ; i < 256 ; ++i) {
      for(size_t j = 0 ; j < 8 ; ++j) {
        mask = 1 << j;
        bit = (mask & i) >> j;
        RBC_SQR_LOOKUP_TABLE[i] ^= bit << (2 * j);
      }
    }

    init_field++;
  }
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
  return e[1] == 0 && e[0] == 0;
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
  return e1[0] == e2[0] && e1[1] == e2[1];
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
  if(e1[1] > e2[1]) {
    return 1;
  }

  if(e1[1] < e2[1]) {
    return 0;
  }

  if(e1[1] == e2[1] && e1[0] > e2[0]) {
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
  o[0] = e1[0] ^ e2[0];
  o[1] = e1[1] ^ e2[1];
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

      carry = g[0] >> (64 - j);
      o[0] ^= g[0] << j;
      o[1] ^= (g[1] << j) ^ carry;
    }

    // 64 < j < M
    else {
      u[1] ^= v[0] << (j - 64);
      o[1] ^= g[0] << (j - 64);
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
  uint64_t tmp = (e[1] >> 62) ^ (e[2] << 2);
  o[1] = e[1] ^ tmp ^ (tmp >> 3) ^ (tmp >> 4) ^ (tmp >> 5);

  tmp = (o[1] >> 3) ^ (e[2] << 61);
  o[0] = e[0] ^ tmp ^ (tmp << 1) ^ (tmp << 2) ^ (tmp << 5);

  o[1] &= 0x0000000000000007;
}




/**
 * \fn void rbc_elt_print(const rbc_elt e)
 * \brief This function displays a finite field element.
 *
 * \param[in] e rbc_elt
 */
void rbc_elt_print(const rbc_elt e) {
  printf("[ ");
  printf("%lu %lu", e[0], e[1]);
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
  uint32_t word_size = 64;
  uint32_t window_size = 4;
  uint32_t bu_number = 2 << (window_size - 1);
  uint32_t bu_size = (RBC_FIELD_M + window_size) % 64 == 0 ? (RBC_FIELD_M + window_size) / 64 : (RBC_FIELD_M + window_size) / 64 + 1;
  uint64_t bu[bu_number][bu_size];
  uint64_t u, tmp;

  // Precomputation of u x b
  memset(bu, 0, sizeof bu);
  rbc_elt_set(bu[1], e2);

  for(size_t i = 2; i < bu_number ; i += 2) {
    bu[i][0] = bu[i >> 1][0] << 1;
    bu[i + 1][0] = bu[i][0] ^ e2[0];

    for(size_t j = 1 ; j < bu_size ; ++j) {
      tmp = bu[i >> 1][j - 1] >> (word_size - 1);
      bu[i][j] = (bu[i >> 1][j] << 1) ^ tmp;
      bu[i + 1][j] = bu[i][j] ^ e2[j];
    }
  }

  // Multiplication
  for(int32_t k = word_size / window_size - 1 ; k >= 0 ; --k) {
    for(size_t j = 0 ; j < RBC_ELT_UINT64 ; ++j) {
      u = (e1[j] >> (window_size * k)) & 0xF;
      for(size_t i = 0 ; i < bu_size ; ++i) {
        o[j + i] ^= bu[u][i];
      }
    }

    if(k != 0) {
      for(size_t i = RBC_ELT_UR_UINT64 - 1 ; i > 0 ; --i) {
        o[i] = (o[i] << window_size) ^ (o[i - 1] >> (word_size - 4));
      }
      o[0] = o[0] << window_size;
    }
  }
}



/**
 * \fn void rbc_elt_ur_sqr(rbc_elt o, const rbc_elt e)
 * \brief This function computes the unreduced square of a finite field element.
 *
 * \param[out] o rbc_elt_ur equal to \f$ e^{2} \f$
 * \param[in]  e rbc_elt
 */
void rbc_elt_ur_sqr(rbc_elt_ur o, const rbc_elt e) {
uint64_t u0, u1, u2, u3, u4, u5, u6, u7;

  for(size_t i = 0 ; i < RBC_ELT_UINT64 - 1 ; ++i) {
    u0 = e[i] & 0xff;
    u1 = (e[i] & 0xff00) >> 8;
    u2 = (e[i] & 0xff0000) >> 16;
    u3 = (e[i] & 0xff000000) >> 24;
    u4 = (e[i] & 0xff00000000) >> 32;
    u5 = (e[i] & 0xff0000000000) >> 40;
    u6 = (e[i] & 0xff000000000000) >> 48;
    u7 = (e[i] & 0xff00000000000000) >> 56;

    o[2 * i] = (RBC_SQR_LOOKUP_TABLE[u3] << 48) ^ (RBC_SQR_LOOKUP_TABLE[u2] << 32) ^ (RBC_SQR_LOOKUP_TABLE[u1] << 16) ^ RBC_SQR_LOOKUP_TABLE[u0];
    o[2 * i + 1] = (RBC_SQR_LOOKUP_TABLE[u7] << 48) ^ (RBC_SQR_LOOKUP_TABLE[u6] << 32) ^ (RBC_SQR_LOOKUP_TABLE[u5] << 16) ^ RBC_SQR_LOOKUP_TABLE[u4];
  }

  u0 = e[RBC_ELT_UINT64 - 1] & 0xff;
  u1 = (e[RBC_ELT_UINT64 - 1] & 0xff00) >> 8;
  u2 = (e[RBC_ELT_UINT64 - 1] & 0xff0000) >> 16;
  u3 = (e[RBC_ELT_UINT64 - 1] & 0xff000000) >> 24;
  o[2 * (RBC_ELT_UINT64 - 1)] = (RBC_SQR_LOOKUP_TABLE[u3] << 48) ^ (RBC_SQR_LOOKUP_TABLE[u2] << 32) ^ (RBC_SQR_LOOKUP_TABLE[u1] << 16) ^ RBC_SQR_LOOKUP_TABLE[u0];
}



/**
 * \fn void rbc_elt_ur_print(const rbc_elt_ur e)
 * \brief This function displays an unreduced finite field element.
 *
 * \param[in] e rbc_elt_ur
 */
void rbc_elt_ur_print(const rbc_elt_ur e) {
  printf("[ ");
  printf("%lu %lu %lu", e[0], e[1], e[2]);
  printf(" ]");
}

