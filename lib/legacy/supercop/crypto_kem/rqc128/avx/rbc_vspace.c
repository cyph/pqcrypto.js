/**
 * \file rbc_vspace.c
 * \brief Implementation of rbc_vspace.h
 */

#include "rbc.h"
#include "rbc_vspace.h"
#include "rbc_vec.h"


/**
 * \fn void rbc_vspace_init(rbc_vspace* vs, uint32_t size)
 * \brief This function allocates the memory for a rbc_vspace.
 *
 * \param[out] vs Pointer to the allocated rbc_vspace
 * \param[in] size Size of the rbc_vspace
 */
void rbc_vspace_init(rbc_vspace* vs, uint32_t size) {
  rbc_vec_init(vs, size);
}



/**
 * \fn void rbc_vspace_clear(rbc_vspace vs)
 * \brief This functions clears the memory allocated to a rbc_vspace.
 *
 * \param[in] v rbc_vspace
 * \param[in] size Size of the rbc_vspace
 */
void rbc_vspace_clear(rbc_vspace vs) {
  rbc_vec_clear(vs);
}



/**
 * \fn void rbc_vspace_set_zero(rbc_vspace o, uint32_t size)
 * \brief This function sets a rbc_vspace to zero.
 *
 * \param[out] o rbc_vspace
 * \param[in] size Size of the rbc_vspace
 */
void rbc_vspace_set_zero(rbc_vspace o, uint32_t size) {
  rbc_vec_set_zero(o, size);
}



/**
 * \fn void rbc_vspace_set(rbc_vspace o, const rbc_vspace vs, uint32_t size)
 * \brief This function copies a rbc_vspace to another one.
 *
 * \param[out] o rbc_vspace
 * \param[in] vs rbc_vspace
 * \param[in] size Size of the rbc_vspaces
 */
void rbc_vspace_set(rbc_vspace o, const rbc_vspace vs, uint32_t size) {
  rbc_vec_set(o, vs, size);
}



/** 
 * \fn void rbc_vspace_set_random_full_rank(AES_XOF_struct* ctx, rbc_vspace o, uint32_t size)
 * \brief This function sets a vector space with random values using the NIST seed expander. The returned vector space has full rank.
 *
 * \param[out] ctx NIST seed expander
 * \param[out] o rbc_vspace
 * \param[in] size Size of the rbc_vspace
 */
void rbc_vspace_set_random_full_rank(AES_XOF_struct* ctx, rbc_vspace o, uint32_t size) {
  rbc_vec_set_random_full_rank(ctx, o, size);
}



/** 
 * \fn void rbc_vspace_set_random_full_rank2(rbc_vspace o, uint32_t size)
 * \brief This function sets a vector space with random values using randombytes. The returned vector space has full rank.
 *
 * \param[out] o rbc_vspace
 * \param[in] size Size of the rbc_vspace
 */
void rbc_vspace_set_random_full_rank2(rbc_vspace o, uint32_t size) {
  rbc_vec_set_random_full_rank2(o, size);
}



/**
 * \fn void rbc_vspace_set_random_full_rank_with_one(AES_XOF_struct* ctx, rbc_vspace o, uint32_t size) {
 * \brief This function sets a rbc_vspace with random values using the NIST seed expander. The rbc_vspace returned by this function has full rank and contains one.
 *
 * \param[out] ctx NIST seed expander
 * \param[out] o rbc_vspace
 * \param[in] size Size of rbc_vspace
 */
void rbc_vspace_set_random_full_rank_with_one(AES_XOF_struct* ctx, rbc_vspace o, uint32_t size) {
  rbc_vec_set_random_full_rank_with_one(ctx, o, size);
}



/**
 * \fn void rbc_vspace_directsum(rbc_vspace o, const rbc_vspace vs1, const rbc_vspace vs2, uint32_t vs1_size, uint32_t vs2_size)
 * \brief This function computes the direct sum of two vector spaces.
 *
 * \param[out] o Direct sum of vs1 and vs2
 * \param[in] vs1 rbc_vspace
 * \param[in] vs2 rbc_vspace
 * \param[in] vs1_size Size of vs1
 * \param[in] vs2_size Size of vs2
 */
void rbc_vspace_directsum(rbc_vspace o, const rbc_vspace vs1, const rbc_vspace vs2, uint32_t vs1_size, uint32_t vs2_size) {
  for(size_t i = 0; i < vs1_size; i++) {
    rbc_elt_set(o[i], vs1[i]);
  }

  for(size_t i = 0; i < vs2_size; i++) {
    rbc_elt_set(o[i + vs1_size], vs2[i]);
  }
}



/**
 * \fn void rbc_vspace_product(rbc_vspace o, const rbc_vspace vs1, const rbc_vspace vs2, uint32_t vs1_size, uint32_t vs2_size)
 * \brief This function computes the product vector space of a and b.
 *
 * \param[out] o Product vector space
 * \param[in] vs1 rbc_vspace
 * \param[in] vs2 rbc_vspace
 * \param[in] vs1_size Size of a
 * \param[in] vs2_size Size of b
 */
void rbc_vspace_product(rbc_vspace o, const rbc_vspace vs1, const rbc_vspace vs2, uint32_t vs1_size, uint32_t vs2_size) {
  for(size_t i = 0 ; i < vs1_size ; i++) {
    for(size_t j = 0 ; j < vs2_size ; j++) {
      rbc_elt_mul(o[i * vs2_size + j], vs1[i], vs2[j]);
    }
  }
}

uint32_t rbc_vspace_intersection_constant_time(rbc_vspace o, const rbc_vspace vs1, const rbc_vspace vs2, uint32_t vs1_size, uint32_t vs2_size) {
  if(vs1_size == 0 || vs2_size == 0) {
    return 0;
  }

  rbc_vspace direct_sum;
  rbc_vspace result;  
  int32_t inter_dim;

  // Initialization
  rbc_vspace_init(&direct_sum, vs1_size + vs2_size);
  rbc_vspace_init(&result, vs1_size + vs2_size);

  rbc_vspace_directsum(direct_sum, vs1, vs2, vs1_size, vs2_size);
  rbc_vspace_set(result, vs1, vs1_size);

  // Perform Gauss on direct_sum and repeat operations on result
  int dimension = rbc_vec_gauss_constant_time(direct_sum, vs1_size + vs2_size, &result, 1);

  inter_dim = vs1_size + vs2_size - dimension;
  rbc_vspace_set_zero(o, max(vs1_size, vs2_size));
  for(int32_t i = 0 ; i < inter_dim ; i++) {
    rbc_elt_set(o[i], result[dimension + i]);
  }

  rbc_vspace_clear(direct_sum);
  rbc_vspace_clear(result);

  return inter_dim;
}



/**
 * \fn uint32_t rbc_vspace_intersection(rbc_vspace o, const rbc_vspace vs1, const rbc_vspace vs2, uint32_t vs1_size, uint32_t vs2_size)
 * \brief This function computes the intersection of vs1 and vs2.
 *
 * \param[out] o Intersection of vs1 and vs2
 * \param[in] vs1 rbc_vspace
 * \param[in] vs2 rbc_vspace
 * \param[in] vs1_size Size of vs1
 * \param[in] vs2_size Size of vs2
 *
 * \return Dimension of o
 */
uint32_t rbc_vspace_intersection(rbc_vspace o, const rbc_vspace vs1, const rbc_vspace vs2, uint32_t vs1_size, uint32_t vs2_size) {
  if(vs1_size == 0 || vs2_size == 0) {
    return 0;
  }

  rbc_vspace direct_sum;
  rbc_vspace result;  
  int32_t inter_dim;

  // Initialization
  rbc_vspace_init(&direct_sum, vs1_size + vs2_size);
  rbc_vspace_init(&result, vs1_size + vs2_size);

  rbc_vspace_directsum(direct_sum, vs1, vs2, vs1_size, vs2_size);
  rbc_vspace_set(result, vs1, vs1_size);

  // Perform Gauss on direct_sum and repeat operations on result
  int dimension = rbc_vec_gauss_other_matrices(direct_sum, vs1_size + vs2_size, &result, 1);

  inter_dim = vs1_size + vs2_size - dimension;
  rbc_vspace_set_zero(o, max(vs1_size, vs2_size));
  for(int32_t i = 0 ; i < inter_dim ; i++) {
    rbc_elt_set(o[i], result[dimension + i]);
  }

  rbc_vspace_clear(direct_sum);
  rbc_vspace_clear(result);

  return inter_dim;
}



/**
 * \fn void rbc_vspace_print(const rbc_vspace vs, uint32_t size)
 * \brief This function displays a rbc_vspace.
 *
 * \param[in] vs rbc_vspace
 * \param[in] size Size of the rbc_vspace
 */
void rbc_vspace_print(const rbc_vspace vs, uint32_t size) {
  rbc_vec_print(vs, size);
}

