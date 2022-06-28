/**
 * \file ffi_vec.cpp
 * \brief Implementation of ffi_vec.h using NTL library
 */

#include <NTL/vec_vec_GF2.h>
#include <NTL/mat_GF2.h>
#include <cstring>
#include "ffi.h"
#include "ffi_elt.h"
#include "ffi_vec.h"
#include "parameters.h"

using namespace std;


/** 
 * \fn void ffi_vec_set(ffi_vec& o, const ffi_vec& v, unsigned int size)
 * \brief This function copies a vector over a finite field into another one
 *
 * \param[out] o Vector over a finite field
 * \param[in] v Vector over a finite field
 * \param[in] size Size of the vectors
 */
void ffi_vec_set(ffi_vec& o, const ffi_vec& v, unsigned int size) {
  o.SetLength(size);
  o = v;
}



/** 
 * \fn void ffi_vec_set_zero(ffi_vec& v, unsigned int size)
 * \brief This function sets a vector over a finite field to zero
 *
 * \param[out] o Vector over a finite field
 * \param[in] size Size of the vector <b>v</b>
 */
void ffi_vec_set_zero(ffi_vec& o, unsigned int size) {
  o.SetLength(size);
  clear(o);
}



/** 
 * \fn void ffi_vec_set_random_using_seedexpander(ffi_vec& o, unsigned int size, AES_XOF_struct* ctx)
 * \brief This function sets a vector with random values using the NIST seed expander
 *
 * \param[out] o Vector over a finite field
 * \param[in] size Size of the vector <b>v</b>
 * \param[in] ctx NIST seed expander
 */
void ffi_vec_set_random_using_seedexpander(ffi_vec& o, unsigned int size, AES_XOF_struct* ctx) {
	o.SetLength(size);
	for(unsigned int i = 0 ; i < size ; ++i) {
		ffi_elt_set_random_using_seedexpander(o[i], ctx);
	}
}


/** 
 * \fn void ffi_vec_set_random_full_rank_using_seedexpander(ffi_vec& o, unsigned int size, AES_XOF_struct* ctx)
 * \brief This function sets a vector with random values using the NIST seed expander. In addition, the vector returned by this function has full rank and contains one.
 *
 * \param[out] v Full rank vector over a finite field containing one
 * \param[in] size Size of the vector <b>v</b>
 * \param[in] ctx NIST seed expander
 */
void ffi_vec_set_random_full_rank_using_seedexpander(ffi_vec& o, unsigned int size, AES_XOF_struct* ctx) {
  unsigned int rank_max = FIELD_M < size ? FIELD_M : size;

  unsigned int rank = -1;
  while(rank != rank_max) {
    ffi_vec_set_random_using_seedexpander(o, size, ctx);
    rank = ffi_vec_get_rank(o, size);
  }
}

/** 
 * \fn void ffi_vec_set_random_full_rank_with_one_using_seedexpander(ffi_vec& o, unsigned int size, AES_XOF_struct* ctx)
 * \brief This function sets a vector with random values using the NIST seed expander. In addition, the vector returned by this function has full rank and contains one.
 *
 * \param[out] o Full rank vector over a finite field containing one
 * \param[in] size Size of the vector <b>v</b>
 * \param[in] ctx NIST seed expander
 */
void ffi_vec_set_random_full_rank_with_one_using_seedexpander(ffi_vec& o, unsigned int size, AES_XOF_struct* ctx) {
  o.SetLength(size);

  ffi_elt one;
  ffi_elt_set_one(one);

  unsigned int rank = -1;
  while(rank != size) {
    ffi_vec_set_random_using_seedexpander(o, size - 1, ctx);
    ffi_vec_set_coeff(o, one, size - 1);
    rank = ffi_vec_get_rank(o, size);
  }
}


/** 
 * \fn void ffi_vec_set_random_from_support_using_seedexpander(ffi_vec& o, unsigned int size, const ffi_vec& support, unsigned int rank, AES_XOF_struct* ctx)
 * \brief This function sets a vector with random values using the NIST seed expander. In addition, the support of the vector returned by this function is the one given in input.
 *
 * This function copies the support vector in rank random positions of <b>v</b>. Next, all the remaining coordinates of <b>v</b> are set using random linear combinations of the support coordinates.
 *
 * \param[out] o Vector of given support
 * \param[in] size Size of the vector <b>v</b>
 * \param[in] support Support of <b>v</b>
 * \param[in] rank Size of the vector support
 * \param[in] ctx NIST seed expander
 */
void ffi_vec_set_random_from_support_using_seedexpander(ffi_vec& o, unsigned int size, const ffi_vec& support, unsigned int rank, AES_XOF_struct* ctx) {
  o.SetLength(size);

  // Copy the support vector in rank random positions of o
  int random_position_size = 2 * rank;
  unsigned char random_position[random_position_size];
  seedexpander(ctx, random_position, random_position_size);

  unsigned int i = 0;
  unsigned int j = 0;
  unsigned int position;
  while(i != rank) {
    position = random_position[j];
    // Perform rejection sampling and check that the position is not already taken
    if(position < size * (256 / size) && ffi_elt_is_zero(coeff(o, position % size)) == 1) {
      ffi_vec_set_coeff(o, support[i], position % size);
      i++;
    }

    // Get more randomness if necessary
    j++;
    if(j % random_position_size == 0 && i != rank) {
      seedexpander(ctx, random_position, random_position_size);
      j = 0;
    }
  }

  // Set all the remaining coordinates with random linear combinations of the support coordinates
  int random_lin_comb_size = rank * (size - rank);
  unsigned char random_lin_comb[random_lin_comb_size];
  seedexpander(ctx, random_lin_comb, random_lin_comb_size);

  int k = 0;
  for(i = 0 ; i < size ; ++i) {
    if(ffi_elt_is_zero(coeff(o, i))) {
      for(j = 0 ; j < rank ; ++j) {
        if((random_lin_comb[k * rank + j] % FIELD_Q) == 1) {
          ffi_elt tmp;
          ffi_elt_add(tmp, coeff(support, j), coeff(o, i));
          ffi_vec_set_coeff(o, tmp, i);
        }
      }
      k++;
    }
  }
}



/** 
 * \fn void ffi_vec_set_random_using_rng(ffi_vec& o, unsigned int size)
 * \brief This function sets a vector with random values using the NIST randombytes function
 *
 * \param[out] v Vector over a finite field
 * \param[in] size Size of the vector <b>v</b>
 * \param[in] ctx NIST seed expander
 */
void ffi_vec_set_random_using_rng(ffi_vec& o, unsigned int size) {
	o.SetLength(size);
	for(unsigned int i = 0 ; i < size ; ++i) {
		ffi_elt_set_random_using_rng(o[i]);
	}
}



/** 
 * \fn void ffi_vec_set_random_full_rank_using_rng(ffi_vec& o, unsigned int size)
 * \brief This function sets a vector with random values using the NIST randombytes function. In addition, the vector returned by this function has full rank.
 *
 * \param[out] o Full rank vector over a finite field
 * \param[in] size Size of the vector <b>v</b>
 */
void ffi_vec_set_random_full_rank_using_rng(ffi_vec& o, unsigned int size) {
  unsigned int rank_max = FIELD_M < size ? FIELD_M : size;

  unsigned int rank = -1;
  while(rank != rank_max) {
	  ffi_vec_set_random_using_rng(o, size);
    rank = ffi_vec_get_rank(o, size);
	}
}

/** 
 * \fn void ffi_vec_set_random_full_rank_with_one_using_seedexpander(ffi_vec& o, unsigned int size, AES_XOF_struct* ctx)
 * \brief This function sets a vector with random values using the NIST randombytes function. In addition, the vector returned by this function has full rank and contains one.
 *
 * \param[out] o Full rank vector over a finite field containing one
 * \param[in] size Size of the vector <b>v</b>
 */
void ffi_vec_set_random_full_rank_with_one_using_rng(ffi_vec& o, unsigned int size) {
  o.SetLength(size);

  ffi_elt one;
  ffi_elt_set_one(one);

  unsigned int rank = -1;
  while(rank != size) {
    ffi_vec_set_random_using_rng(o, size - 1);
    ffi_vec_set_coeff(o, one, size - 1);
    rank = ffi_vec_get_rank(o, size);
  }
}


/** 
 * \fn void ffi_vec_set_random_from_support_using_rng(ffi_vec& o, unsigned int size, const ffi_vec& support, unsigned int rank)
 * \brief This function sets a vector with random values using the NIST seed expander. In addition, the support of the vector returned by this function is the one given in input.
 *
 * This function copies the support vector in rank random positions of <b>v</b>. Next, all the remaining coordinates of <b>v</b> are set using random linear combinations of the support coordinates.
 *
 * \param[out] o Vector of given support
 * \param[in] size Size of the vector <b>v</b>
 * \param[in] support Support of <b>v</b>
 * \param[in] rank Size of the vector support
 */
void ffi_vec_set_random_from_support_using_rng(ffi_vec& o, unsigned int size, const ffi_vec& support, unsigned int rank) {
  o.SetLength(size);

  // Copy the support vector in rank random positions of o
  int random_position_size = 2 * rank;
  unsigned char random_position[random_position_size];
  randombytes(random_position, random_position_size);

  unsigned int i = 0;
  unsigned int j = 0;
  unsigned int position;
  while(i != rank) {
    position = random_position[j];
    // Perform rejection sampling and check that the position is not already taken
    if(position < size * (256 / size) && ffi_elt_is_zero(coeff(o, position % size)) == 1) {
      ffi_vec_set_coeff(o, support[i], position % size);
      i++;
    }

    // Get more randomness if necessary
    j++;
    if(j % random_position_size == 0 && i != rank) {
      randombytes(random_position, random_position_size);
      j = 0;
    }
  }

  // Set all the remaining coordinates with random linear combinations of the support coordinates
  int random_lin_comb_size = rank * (size - rank);
  unsigned char random_lin_comb[random_lin_comb_size];
  randombytes(random_lin_comb, random_lin_comb_size);

  int k = 0;
  for(i = 0 ; i < size ; ++i) {
    if(ffi_elt_is_zero(coeff(o, i))) {
      for(j = 0 ; j < rank ; ++j) {
        if((random_lin_comb[k * rank + j] % FIELD_Q) == 1) {
          ffi_elt tmp;
          ffi_elt_add(tmp, coeff(support, j), coeff(o, i));
          ffi_vec_set_coeff(o, tmp, i);
        }
      }
      k++;
    }
  }
}



/** 
 * \fn void ffi_vec_set_coeff(ffi_vec& o, const ffi_elt& e, unsigned int position)
 * \brief This function sets the coordinate of a vector at a given position
 *
 * \param[out] o Vector to set
 * \param[in] e Finite field element
 * \param[in] position Position of the vector to set
 */
void ffi_vec_set_coeff(ffi_vec &o, const ffi_elt& e, unsigned int position) {
  if(o.rep.length() <= position) {
		o.SetLength(position + 1);
	}

  SetCoeff(o, position, e);
}



/** 
 * \fn void ffi_vec_get_coeff(ffi_elt& o, const ffi_vec& v, unsigned int position)
 * \brief This function returns the coordinate of a vector at a given position
 *
 * \param[out] o Finite field element
 * \param[in] v Vector over a finite field element
 * \param[in] position Position of the vector to read
 */
void ffi_vec_get_coeff(ffi_elt& o, const ffi_vec& v, unsigned int position) {
	o = coeff(v, position);
}

/**
 * @brief      Test equality of two ffi_vec
 *
 * @param[in]  v1    Vector over a finite field element
 * @param[in]  v2    Vector over a finite field element
 *
 * @return     1 if the vectors are equal, 0 otherwise
 */
int ffi_vec_cmp(const ffi_vec &v1, const ffi_vec &v2) {
  return v1 == v2;
}


/** 
 * \fn void ffi_vec_echelonize(ffi_vec& o, unsigned int size)
 * \brief This function computes the row reduced echelon form of the matrix induced by a given vector over GF(2).
 *
 * \param[out] o Vector over a finite field
 * \param[in] size Size of the vector
 */
void ffi_vec_echelonize(ffi_vec &o, unsigned int size) {
	int ref_size = ffi_vec_gauss(o, size);

	int lowest_coeff_position;
  GF2X polynomial;

	for(int i = 1 ; i < ref_size ; ++i) {
		lowest_coeff_position = i;
		polynomial = rep(o[i]);
		while(polynomial[lowest_coeff_position] == 0) {
			lowest_coeff_position++;
		}

		for(int j = i - 1 ; j >= 0 ; --j) {
			polynomial = rep(o[j]);
			if(polynomial[lowest_coeff_position] == 1) {
				ffi_elt_add(o[j], o[i], o[j]);
			}
		}
	}
}



/** 
 * \fn unsigned int ffi_vec_gauss(ffi_vec& o, unsigned int size)
 * \brief This function computes gaussian elimination on the matrix induced by a given vector over GF(2).
 *
 * \param[out] o Vector over a finite field
 * \param[in] size Size of the vector
 * \return The rank of the matrix
 */
unsigned int ffi_vec_gauss(ffi_vec& o, unsigned int size) {
	mat_GF2 matrix;
	vec_vec_GF2 vectors;
	vectors.SetLength(size);
  ffi_elt zeroElt;
  ffi_elt_set_zero(zeroElt);

  vec_GF2 tmp;
	for(unsigned int i = 0 ; i < size ; ++i) {
    conv(tmp, rep(o[i]));
    tmp.SetLength(FIELD_M);
		vectors[i] = tmp;
	}

	conv(matrix, vectors);
	gauss(matrix);

	GF2X polynomial;
  o.SetLength(size);
	for(unsigned int i = 0 ; i < size ; ++i) {
		conv(polynomial, matrix[i]);
		conv(o[i], polynomial);
	}

  o.normalize();
  return o.rep.length();
}



/** 
 * \fn unsigned int ffi_vec_get_rank(ffi_vec v, unsigned int size)
 * \brief This function computes the rank of a vector over a finite field namely the rank of the matrix induced by the vector over GF(2).
 *
 * \param[out] v Vector over a finite field
 * \param[in] size Size of the vector
 * \return The rank of the matrix
 */
unsigned int ffi_vec_get_rank(ffi_vec v, unsigned int size) {
	return ffi_vec_gauss(v, size);
}



/** 
 * \fn void ffi_vec_add(ffi_vec& o, const ffi_vec& v1, const ffi_vec& v2, unsigned int size)
 * \brief This functions adds two vectors over a finite field
 *
 * \param[out] o Sum of <b>v1</b> and <b>v2</b>
 * \param[in] v1 Vector over a finite field
 * \param[in] v2 Vector over a finite field
 * \param[in] size Size of the vectors
 */
void ffi_vec_add(ffi_vec &o, const ffi_vec& v1, const ffi_vec& v2, unsigned int size) {
  add(o, v1, v2);
}



/** 
 * \fn void ffi_vec_mul(ffi_vec& o, const ffi_vec& v1, const ffi_vec& v2, unsigned int size)
 * \brief Multiply two vector over a finite field
 *
 * Vector multiplication is defined as polynomial multiplication over \f$ GF(2^m)/(X^{size} - 1) \f$.
 *
 * \param[in] f Finite field
 * \param[out] o Product of <b>v1</b> and <b>v2</b>
 * \param[in] v1 Vector over a finite field
 * \param[in] v2 Vector over a finite field
 * \param[in] size Size of the vectors
 */
void ffi_vec_mul(ffi_vec& o, const ffi_vec& v1, const ffi_vec& v2, unsigned int size) {
	MulMod(o, v1, v2, ideal_modulo);
}



/** 
 * \fn void ffi_vec_scalar_mul(ffi_vec& o, const ffi_vec& v, const ffi_elt& e, unsigned int size)
 * \brief This functions multiplies a vector over a finite field by a scalar
 *
 * \param[out] o Vector equal to \f$ e \times v \f$
 * \param[in] v Vector over a finite field
 * \param[in] e Finite field element
 * \param[in] size Size of the vectors
 */
void ffi_vec_scalar_mul(ffi_vec& o, const ffi_vec& v, const ffi_elt& e, unsigned int size) {
  o.SetLength(size);
	for(unsigned int i = 0 ; i < size ; ++i) {
		ffi_elt_mul(o[i], v[i], e);
	}
}



/** 
 * \fn void ffi_vec_tensor_mul(ffi_vec& o, const ffi_vec& v1, unsigned int size1, const ffi_vec& v2, unsigned int size2)
 * \brief This functions computes the tensor product of two vectors over a finite field by a scalar
 *
 * \param[out] o Tensor product of <b>v1</b> and <b>v2</b>
 * \param[in] v1 Vector over a finite field
 * \param[in] size1 Size of the vector <b>v1</b>
 * \param[in] v2 Vector over a finite field
 * \param[in] size2 Size of the vector <b>v2</b>
 */
void ffi_vec_tensor_mul(ffi_vec& o, const ffi_vec& v1, unsigned int size1, const ffi_vec& v2, unsigned int size2) {
	o.SetLength(size1 * size2);
	for(unsigned int i = 0 ; i < size1 ; ++i) {
		for(unsigned int j = 0 ; j < size2 ; ++j) {
			ffi_elt_mul(o[i * size2 + j], v1[i], v2[j]);
		}
	}
}



/** 
 * \fn void ffi_vec_directsum(ffi_vec& o, const ffi_vec& v1, unsigned int size1, const ffi_vec& v2, unsigned int size2)
 * \brief This functions computes the direct sum of two vectors over a finite field
 *
 * \param[out] o Union of <b>v1</b> and <b>v2</b>
 * \param[in] v1 Vector over a finite field
 * \param[in] size1 Size of the vector <b>v1</b>
 * \param[in] v2 Vector over a finite field
 * \param[in] size2 Size of the vector <b>v2</b>
 */
void ffi_vec_directsum(ffi_vec& o, const ffi_vec& v1, unsigned int size1, const ffi_vec& v2, unsigned int size2) {
	o.SetLength(size1 + size2);

	for(unsigned int i = 0 ; i < size1 ; ++i) {
		ffi_elt_set(o[i], v1[i]);
	}

	for(unsigned int i = 0 ; i < size2 ; ++i) {
		ffi_elt_set(o[size1 + i], v2[i]);
	}
}



/** 
 * \fn void ffi_vec_intersection(ffi_vec& o, unsigned int& size, const ffi_vec& v1, unsigned int size1, const ffi_vec& v2, unsigned int size2)
 * \brief This functions computes the intersection of two vectors over a finite field.
 *
 * This function uses the Zassenhauss algorithm in order to compute a basis for the intersection of the spans of <b>v1</b> and <b>v2</b>.
 *
 * \param[out] o Intersection of <b>v1</b> and <b>v2</b>
 * \param[in] size Size of the vector <b>o</b>
 * \param[in] v1 Vector over a finite field
 * \param[in] size1 Size of the vector <b>v1</b>
 * \param[in] v2 Vector over a finite field
 * \param[in] size2 Size of the vector <b>v2</b>
 */
void ffi_vec_intersection(ffi_vec& o, unsigned int& size, const ffi_vec& v1, unsigned int size1, const ffi_vec& v2, unsigned int size2) {
	if(size1 == 0 || size2 == 0) {
    size = 0;
    ffi_vec_set_zero(o, size);
    return;
  }

  // Compute the block matrix:  
  //   v1 | v1
  //   v2 |  0
  
	mat_GF2 matrix;
	vec_vec_GF2 vectors;
	vectors.SetLength(size1 + size2);
  vec_GF2 tmp;

	for(unsigned int i = 0 ; i < size1 ; ++i) {
		conv(tmp, rep(v1[i]));
		tmp.SetLength(FIELD_M);
		vectors[i] = tmp;
		append(vectors[i], tmp);
	}

	vec_GF2 zero;
	zero.SetLength(FIELD_M);
	for(unsigned int i = size1 ; i < size1 + size2 ; ++i) {
		conv(tmp, rep(v2[i - size1]));
		tmp.SetLength(FIELD_M);
		vectors[i] = tmp;
		append(vectors[i], zero);
	}

  // Transform matrix into row echelon form
	conv(matrix, vectors);
	gauss(matrix);

  // Extract the basis of the intersection
	int index = -1;
	for(unsigned int i = 0 ; i < size1 + size2 ; ++i) {
		shift(tmp, matrix[i], FIELD_M);

		if(IsZero(tmp)) {
			index = i;
			break;
		}
	}

	if(index == -1) {
    size = 0;
    ffi_vec_set_zero(o, size);
  } 
  else {
		GF2X polynomial;
		o.SetLength(size1 + size2 - index);
		for(unsigned int i = index ; i < size1 + size2 ; ++i) {
			shift(tmp, matrix[i], - FIELD_M);
			tmp.SetLength(FIELD_M);
			conv(polynomial, tmp);
			conv(o[i - index], polynomial);
		}

   		o.normalize();
		size = o.rep.length();
	}
}

/**
 * @fn         void get_bit(unsigned char* b, unsigned char c, unsigned int position)
 *
 * @brief      This function reads a bit from a byte
 *
 * @param[in]  c         Byte to read from
 * @param[in]  position  Position of the bit to read
 *
 * @return     The bit that was read
 */
static unsigned char get_bit(const unsigned char c, unsigned int position) {
  return (c >> position) & 0x01;
}



/** 
 * \fn void set_bit(unsigned char* c, unsigned int position)
 * \brief This function sets a bit to 1 in a byte
 *
 * \param[out] c Byte to modify
 * \param[in] position Position of the bit to set
 */
static void set_bit(unsigned char* c, unsigned int position) {
  *c = *c | (1 << position);
}


/** 
 * \fn void ffi_vec_to_string(unsigned char* str2, const ffi_vec v, unsigned int size)
 * \brief This function parses a vector into a string
 *
 * This function only copies the bits of <b>v</b> containing information into <b>str2</b>. Indeed, there are only <b>FIELD_M</b> of them while the ffi_vec bit representation is larger due to library storing elements differently.
 *
 * \param[out] str2 Output string
 * \param[in] v Vector to parse
 * \param[in] size Size of the vector
 */
void ffi_vec_to_string(unsigned char* str2, const ffi_vec &v, unsigned int size) {
  unsigned int size2 = (FIELD_M % 8 == 0) ? size * FIELD_M / 8 : size * FIELD_M / 8 + 1;

  memset(str2, 0, size2);

  unsigned int position = 0;
  unsigned int position1 = 0;
  unsigned int position2 = 0;

  unsigned char bit = 0;
  for(position1 = 0 ; position1 < size ; position1++) {
    for(position2 = 0 ; position2 < FIELD_M ; position2++) {
      if(coeff(rep(v[position1]), position2) == 1) bit = 1;
      else bit = 0;
      if(bit == 1) set_bit(&str2[position / 8], position % 8);
      position++;
    }  
  }
}



/** 
 * \fn void ffi_vec_from_string(ffi_vec v, unsigned int size, const unsigned char* str2)
 * \brief This function parses a string into a vector
 *
 * This function turns the <b>FIELD_M</b> bits of information required to describe <b>v</b> into a ffi_vec bit representation which is larger due to MPFQ storing elements as pointer of unsigned longs.
 *
 * \param[out] v Output vector
 * \param[in] size Size of the vector <b>v</b>
 * \param[in] str2 String to parse
 */
void ffi_vec_from_string(ffi_vec &v, unsigned int size, const unsigned char* str2) {
  unsigned int position = 0;
  unsigned int position1 = 0;
  unsigned int position2 = 0;

  v.SetLength(size);

  for(position1 = 0 ; position1 < size ; position1++) {
    for(position2 = 0 ; position2 < FIELD_M ; position2++) {
      if(get_bit(str2[position / 8], position % 8)) SetCoeff(v[position1].LoopHole(), position2);
      position++;
    }
  }
}



/** 
 * \fn void ffi_vec_print(const ffi_vec& v, unsigned int size)
 * \brief This function prints the value of a vector over a finite field
 *
 * \param[in] v Vector over a finite field
 * \param[in] size Size of the vector <b>v</b>
 */
void ffi_vec_print(const ffi_vec& v, unsigned int size) {
	size = v.rep.length();
	printf("[\n");
	for(unsigned int i = 0 ; i < size ; i++) {
		ffi_elt_print(v[i]);
	}
	printf("]\n");
}


/**
 * @brief      Computes the inverse of the polynomal A
 *
 * @param      inv   Inverse polynomial
 * @param[in]  A     Polynomial
 *
 * @return     0 if the inversion is possible, 1 otherwise
 */
long ffi_vec_inv(ffi_vec &inv, ffi_vec A) {
  A.normalize();
  return InvModStatus(inv, A, ideal_modulo);
}
