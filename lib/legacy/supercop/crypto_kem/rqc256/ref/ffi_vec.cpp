/**
 * \file ffi_vec.cpp
 * \brief Implementation of ffi_vec.h using the NTL library
 */

#include "ffi.h"
#include "ffi_elt.h"
#include "ffi_vec.h"
#include <cstring>

static unsigned int ffi_vec_get_rank(ffi_vec v, unsigned int size);
static unsigned char get_bit(const unsigned char c, unsigned int position);
static void set_bit(unsigned char* c, unsigned int position);



/** 
 * \fn ffi_elt ffi_vec_get_coeff(const ffi_vec& v, unsigned int position)
 * \brief This function returns the coordinate of a vector at a given position
 *
 * \param[out] Finite field element
 * \param[in] v Vector over a finite field element
 * \param[in] position Position of the vector to retrieve
 */
ffi_elt ffi_vec_get_coeff(const ffi_vec& v, unsigned int position) {
  return coeff(v, position);
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
  SetCoeff(o, position, e);
}



/** 
 * \fn void ffi_vec_set_length(ffi_vec& o, unsigned int size)
 * \brief This function sets the length of a vector
 *
 * \param[in] o Vector
 * \param[in] size Size of the vector
 */
void ffi_vec_set_length(ffi_vec& o, unsigned int size) {
  o.SetLength(size);
}



/** 
 * \fn void ffi_vec_set(ffi_vec& o, const ffi_vec& v, unsigned int size)
 * \brief This function copies a vector over a finite field into another one
 *
 * \param[out] o Vector over a finite field
 * \param[in] v Vector over a finite field
 * \param[in] size Size of the vectors
 */
void ffi_vec_set(ffi_vec& o, const ffi_vec& v, unsigned int size) {
  ffi_vec_set_length(o, size);
  o = v;
}



/** 
 * \fn void ffi_vec_set_zero(ffi_vec& v, unsigned int size)
 * \brief This function sets a vector over a finite field to zero
 *
 * \param[out] o Vector over a finite field
 * \param[in] size Size of the vector
 */
void ffi_vec_set_zero(ffi_vec& o, unsigned int size) {
  ffi_vec_set_length(o, size);
  clear(o);
}



/** 
 * \fn void ffi_vec_set_random(ffi_vec& o, unsigned int size, AES_XOF_struct* ctx)
 * \brief This function sets a vector with random values using the NIST seed expander
 *
 * \param[out] o Vector over a finite field
 * \param[in] size Size of the vector
 * \param[in] ctx NIST seed expander
 */
void ffi_vec_set_random(ffi_vec& o, unsigned int size, AES_XOF_struct* ctx) {
  ffi_vec_set_length(o, size);
	for(unsigned int i = 0 ; i < size ; ++i) {
		ffi_elt_set_random(o[i], ctx);
	}
}



/** 
 * \fn void ffi_vec_set_random2(ffi_vec& o, unsigned int size)
 * \brief This function sets a vector with random values using the NIST randombytes function
 *
 * \param[out] o Vector over a finite field
 * \param[in] size Size of the vector <b>v</b>
 * \param[in] ctx NIST seed expander
 */
void ffi_vec_set_random2(ffi_vec& o, unsigned int size) {
  ffi_vec_set_length(o, size);
	for(unsigned int i = 0 ; i < size ; ++i) {
		ffi_elt_set_random2(o[i]);
	}
}



/** 
 * \fn void ffi_vec_set_random_full_rank(ffi_vec& o, unsigned int size, AES_XOF_struct* ctx)
 * \brief This function sets a vector with random values using the NIST seed expander. The vector returned by this function has full rank.
 *
 * \param[out] o Full rank vector over a finite field
 * \param[in] size Size of the vector
 * \param[in] ctx NIST seed expander
 */
void ffi_vec_set_random_full_rank(ffi_vec& o, unsigned int size, AES_XOF_struct* ctx) {
  unsigned int rank_max = FIELD_M < size ? FIELD_M : size;

  unsigned int rank = -1;
  while(rank != rank_max) {
    ffi_vec_set_random(o, size, ctx);
    rank = ffi_vec_get_rank(o, size);
  }
}



/** 
 * \fn void ffi_vec_set_random_full_rank_with_one(ffi_vec& o, unsigned int size, AES_XOF_struct* ctx)
 * \brief This function sets a vector with random values using the NIST seed expander. In addition, the vector returned by this function has full rank and contains one.
 *
 * \param[out] o Full rank vector over a finite field containing one
 * \param[in] size Size of the vector
 * \param[in] ctx NIST seed expander
 */
void ffi_vec_set_random_full_rank_with_one(ffi_vec& o, unsigned int size, AES_XOF_struct* ctx) {
  unsigned int rank_max = FIELD_M < size ? FIELD_M : size;

  unsigned int rank = -1;
  while(rank != rank_max) {
    ffi_vec_set_random(o, size - 1, ctx);
    ffi_vec_set_coeff(o, ffi_elt_get_one(), size - 1);
    rank = ffi_vec_get_rank(o, size);
  }
}



/** 
 * \fn void ffi_vec_set_random_from_support(ffi_vec& o, unsigned int size, const ffi_vec& support, unsigned int rank, AES_XOF_struct* ctx)
 * \brief This function sets a vector with random values using the NIST seed expander. In addition, the support of the vector returned by this function is the one given in input.
 *
 * This function copies the support vector in rank random positions of <b>o</b>. Next, all the remaining coordinates of <b>v</b> are set using random linear combinations of the support coordinates.
 *
 * This function assumes <b>FIELD_Q</b> = 2.
 *
 * \param[out] o Vector of given support
 * \param[in] size Size of the vector <b>v</b>
 * \param[in] support Support of <b>v</b>
 * \param[in] rank Size of the vector support
 * \param[in] ctx NIST seed expander
 */
void ffi_vec_set_random_from_support(ffi_vec& o, unsigned int size, const ffi_vec& support, unsigned int rank, AES_XOF_struct* ctx) {
  ffi_vec_set_length(o, size);

  // Copy the support vector in rank random positions of o
  int random_position_size = 2 * rank;
  unsigned char* random_position = (unsigned char*) malloc(random_position_size * sizeof(unsigned char));
  seedexpander(ctx, random_position, random_position_size);

  unsigned int i = 0;
  unsigned int j = 0;
  unsigned int position;
  while(i != rank) {
    position = random_position[j];
    // Check that the position is not already taken
    if(position < size * (256 / size) && ffi_elt_is_zero(o[position % size]) == 1) {
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
  unsigned char* random_lin_comb = (unsigned char*) malloc(random_lin_comb_size * sizeof(unsigned char));
  seedexpander(ctx, random_lin_comb, random_lin_comb_size);

  int k = 0;
  for(i = 0 ; i < size ; ++i) {
    if(ffi_elt_is_zero(o[i])) {
      for(j = 0 ; j < rank ; ++j) {
        if((random_lin_comb[k * rank + j] % FIELD_Q) == 1) {
          ffi_elt_add(o[i], support[j], o[i]);
        }
      }
      k++;
    }
  }

  free(random_position);
  free(random_lin_comb);
}



/** 
 * \fn unsigned int ffi_vec_get_rank(ffi_vec v, unsigned int size)
 * \brief This function computes the rank of a vector over a finite field namely the rank of the matrix induced by the vector over GF(2).
 *
 * \param[out] v Vector over a finite field
 * \param[in] size Size of the vector
 * \return The rank of the matrix
 */
static unsigned int ffi_vec_get_rank(ffi_vec v, unsigned int size) {
	mat_GF2 matrix;
	vec_vec_GF2 vectors;
	vectors.SetLength(size);

  vec_GF2 tmp;
	for(unsigned int i = 0 ; i < size ; ++i) {
    conv(tmp, rep(v[i]));
    tmp.SetLength(FIELD_M);
		vectors[i] = tmp;
	}

	conv(matrix, vectors);
	return gauss(matrix);
}



/** 
 * \fn int ffi_vec_is_equal_to(const ffi_vec& v1, const ffi_vec& v2, unsigned int size)
 * \brief This function tests if two vectors over a finite field are equal
 *
 * \param[in] v1 Vector over a finite field
 * \param[in] v2 Vector over a finite field
 * \param[in] size Size of the vectors
 * \return The rank of the matrix
 * \return 1 if <b>v1</b> = <b>v2</b>, 0 otherwise
 */
int ffi_vec_is_equal_to(const ffi_vec& v1, const ffi_vec& v2, unsigned int size) {
  return v1 == v2;
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
void ffi_vec_add(ffi_vec& o, const ffi_vec& v1, const ffi_vec& v2, unsigned int size) {
  add(o, v1, v2);
}



/** 
 * \fn void ffi_vec_mul(ffi_vec& o, const ffi_vec& v1, const ffi_vec& v2, unsigned int size)
 * \brief Multiply two vector over a finite field
 *
 * Vector multiplication is defined as polynomial multiplication performed modulo the polynomial <b>ideal_modulo</b>.
 *
 * \param[out] o Product of <b>v1</b> and <b>v2</b>
 * \param[in] v1 Vector over a finite field
 * \param[in] v2 Vector over a finite field
 * \param[in] size Size of the vectors
 */
void ffi_vec_mul(ffi_vec& o, const ffi_vec& v1, const ffi_vec& v2, unsigned int size) {
	MulMod(o, v1, v2, ideal_modulo);
}



/** 
 * \fn unsigned char get_bit(const unsigned char* c, unsigned int position)
 * \brief This function reads a bit in a byte
 *
 * \param[out] Byte read
 * \param[in] c Byte to read
 * \param[in] position Position to read
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
 * \fn void ffi_vec_to_string(unsigned char* str, const ffi_vec& v, unsigned int size)
 * \brief This function parses a vector into a string
 *
 * This function stores the <b>FIELD_N * FIELD_M</b> bits required to describe <b>v</b> into a string.
 * The result contains unnecessary zeros due to NTL internal representation of vectors.
 *
 * \param[out] str Output string
 * \param[in] v Vector to parse
 * \param[in] size Size of the vector
 */
void ffi_vec_to_string(unsigned char* str, const ffi_vec& v, unsigned int size) {
  size = deg(v);
	for(unsigned int i = 0 ; i <= size ; i++) {
		ffi_elt_to_string(str + i * FIELD_ELT_BYTES, v[i]);
	}
}



/** 
 * \fn void ffi_vec_to_string_compact(unsigned char* str, const ffi_vec& v, unsigned int size)
 * \brief This function parses a vector into a string
 *
 * This function stores the <b>FIELD_N * FIELD_M</b> bits required to describe <b>v</b> into the smallest possible string.
 *
 * \param[out] str Output string
 * \param[in] v Vector to parse
 * \param[in] size Size of the vector
 */
void ffi_vec_to_string_compact(unsigned char* str, const ffi_vec& v, unsigned int size) {
  unsigned int size2 = (FIELD_M % 8 == 0) ? size * FIELD_M / 8 : size * FIELD_M / 8 + 1;
  memset(str, 0, size2);

  unsigned int position = 0;
  unsigned int position1 = 0;
  unsigned int position2 = 0;

  unsigned char bit = 0;
  for(position1 = 0 ; position1 < size ; position1++) {
    for(position2 = 0 ; position2 < FIELD_M ; position2++) {
      if(coeff(rep(v[position1]), position2) == 1) bit = 1;
      else bit = 0;
      if(bit == 1) set_bit(&str[position / 8], position % 8);
      position++;
    }  
  }
}



/** 
 * \fn void ffi_vec_from_string(ffi_vec& v, unsigned int size, const unsigned char* str)
 * \brief This function parses a string into a vector
 *
 * The input string is expected to be an uncompacted string generated by the ffi_vec_to_string function.
 *
 * \param[out] v Output vector
 * \param[in] size Size of the vector <b>v</b>
 * \param[in] str String to parse
 */
void ffi_vec_from_string(ffi_vec& v, unsigned int size, const unsigned char *str) {
  ffi_vec_set_length(v, size);
	for(unsigned int i = 0 ; i < size ; i++) {
		ffi_elt_from_string(v[i], str + i * FIELD_ELT_BYTES);
	}
}






/** 
 * \fn void ffi_vec_from_string_compact(ffi_vec& v, unsigned int size, const unsigned char* str)
 * \brief This function parses a string into a vector
 *
 * The input string is expected to be a compacted string generated by the ffi_vec_to_string_compact function.
 *
 * \param[out] v Output vector
 * \param[in] size Size of the vector <b>v</b>
 * \param[in] str String to parse
 */
void ffi_vec_from_string_compact(ffi_vec &v, unsigned int size, const unsigned char* str) {
  unsigned int position = 0;
  unsigned int position1 = 0;
  unsigned int position2 = 0;

  v.SetLength(size);

  for(position1 = 0 ; position1 < size ; position1++) {
    for(position2 = 0 ; position2 < FIELD_M ; position2++) {
      if(get_bit(str[position / 8], position % 8)) SetCoeff(v[position1].LoopHole(), position2);
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

	printf("[ ");
	for(unsigned int i = 0 ; i < size ; i++) {
		ffi_elt_print(v[i]);
	}
	printf("]\n");
}

