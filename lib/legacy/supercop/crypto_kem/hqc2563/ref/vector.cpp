/**
 * \file vector.cpp
 * \brief Implementation of vectors sampling and some utilities for the HQC scheme
 */

#include "vector.h"

/**
 * \fn void vect_fixed_weight(uint8_t* v, const uint16_t weight, AES_XOF_struct* ctx)
 * \brief Generates a vector of a given Hamming weight
 *
 * This function generates uniformly at random a binary vector of a Hamming weight equal to the parameter <b>weight</b>. The vector 
 * is stored by position. 
 * To generate the vector we have to sample uniformly at random values in the interval [0, PARAM_N -1]. Suppose the PARAM_N is equal to \f$ 70853 \f$, to select a position \f$ r\f$ the function works as follow:
 *  1. It makes a call to the seedexpander function to obtain a random number \f$ x\f$ in \f$ [0, 2^{24}[ \f$.
 *  2. Let \f$ t = \lfloor {2^{24} \over 70853} \rfloor \times  70853\f$
 *  3. If \f$ x \geq t\f$, go to 1
 *  4. It return \f$ r = x \mod 70853\f$ 
 *
 * The parameter \f$ t \f$ is precomputed and it's denoted by UTILS_REJECTION_THRESHOLD (see the file parameters.h).
 *
 * \param[in] v Pointer to an array
 * \param[in] weight Integer that is the Hamming weight
 * \param[in] ctx Pointer to the context of the seed expander
 */
void vect_fixed_weight(uint8_t* v, const uint16_t weight, AES_XOF_struct* ctx) {
  unsigned long random_bytes_size = 3 * weight;
  unsigned char* rand_bytes = (unsigned char*) calloc(random_bytes_size, sizeof(unsigned char));
  uint32_t* tmp = (uint32_t*) calloc(weight, sizeof(uint32_t));

  seedexpander(ctx, rand_bytes, random_bytes_size);
  
  unsigned long j =0;
  uint32_t random_data;
  uint8_t exist;

  for(uint32_t i = 0; i < weight ; ++i) {
    exist = 0;
    do {
      if(j == random_bytes_size) {
        seedexpander(ctx, rand_bytes, random_bytes_size);
        j = 0;
      }

      random_data  = ((uint32_t) rand_bytes[j++]) << 16;
      random_data |= ((uint32_t) rand_bytes[j++]) << 8;
      random_data |= rand_bytes[j++]; 

    } while(random_data >= UTILS_REJECTION_THRESHOLD);

    random_data = random_data % PARAM_N;

    for(uint32_t k = 0 ; k < i ; k++) {
      if(tmp[k] == random_data) exist = 1;
    }
    if(exist == 1) {
      i--;
    } else {
      tmp[i] = random_data;
    }
  }

  for (uint16_t i = 0; i < weight; ++i) {
  	int index = tmp[i] / 8;
  	int pos = tmp[i] % 8;
  	v[index] |= 1 << pos;
  }

  free(rand_bytes);
  free(tmp);
}

/**
 * \fn void vect_set_random(uint8_t* v, AES_XOF_struct* ctx)
 * \brief Generates a random vector of dimension <b>PARAM_N</b>
 *
 * This function generates a random binary vector of dimension <b>PARAM_N</b>. It generates a random 
 * array of bytes using the seedexpander function, and drop the extra bits using a mask.
 *
 * \param[in] v Pointer to an array
 * \param[in] ctx Pointer to the context of the seed expander
 */
void vect_set_random(uint8_t* v, AES_XOF_struct* ctx) {
  uint8_t rand_bytes[VEC_N_SIZE_BYTES] = {0};

  seedexpander(ctx, rand_bytes, VEC_N_SIZE_BYTES);

  memcpy(v, rand_bytes, VEC_N_SIZE_BYTES);
  v[VEC_N_SIZE_BYTES - 1] &= BITMASK(PARAM_N, 8);
}

/**
 * \fn void vect_set_random_from_randombytes(uint8_t* v)
 * \brief Generates a random vector
 *
 * This function generates a random binary vector. It uses the the randombytes function.
 *
 * \param[in] v Pointer to an array
 */
void vect_set_random_from_randombytes(uint8_t* v) {
  uint8_t rand_bytes [VEC_K_SIZE_BYTES] = {0};

  randombytes(rand_bytes, VEC_K_SIZE_BYTES);
  memcpy(v, rand_bytes, VEC_K_SIZE_BYTES);
} 

/**
 * \fn void vect_add(uint8_t* o, uint8_t* v1, uint8_t* v2, uint32_t size)
 * \brief Adds two vectors
 *
 * \param[out] o Pointer to an array that is the result
 * \param[in] v1 Pointer to an array that is the first vector 
 * \param[in] v2 Pointer to an array that is the second vector
 * \param[in] size Integer that is the size of the vectors
 */
void vect_add(uint8_t* o, uint8_t* v1, uint8_t* v2, uint32_t size) {
  for (uint32_t i = 0; i < size; ++i) {
    o[i] = v1[i] ^ v2[i];
  }
}

/**
 * \fn int vect_compare(uint8_t* v1, uint8_t* v2, uint32_t size)
 * \brief Compares two vectors
 *
 * \param[in] v1 Pointer to an array that is first vector 
 * \param[in] v2 Pointer to an array that is second vector
 * \param[in] size Integer that is the size of the vectors
 * \return 0 if the vectors are equals and a negative/psotive value otherwise
 */
int vect_compare(uint8_t* v1, uint8_t* v2, uint32_t size) {
  return memcmp(v1, v2, size);
}

/**
 * \fn void vect_resize(uint8_t* o, uint32_t size_o, uint8_t* v, uint32_t size_v)
 * \brief Resize a vector so that it contains <b>size_o</b> bits
 *
 * \param[out] o Pointer to the output vector
 * \param[in] size_o Integer that is the size of the output vector in bits
 * \param[in] v Pointer to the input vector
 * \param[in] size_v Integer that is the size of the input vector in bits
 */
void vect_resize(uint8_t* o, uint32_t size_o, uint8_t* v, uint32_t size_v) {
  if(size_o < size_v) {
    memcpy(o, v, VEC_N1N2_SIZE_BYTES); 
    uint8_t mask = 0x7F;
    int val = 8 - (size_o % 8); 

    for(int i = 0; i < val; ++i) {
      o[VEC_N1N2_SIZE_BYTES - 1] &= (mask >> i);
    }
  } else { 
    memcpy(o, v, VEC_N_SIZE_BYTES);
  }
}

/**
 * \fn void vect_print(uint8_t* v, uint32_t size)
 * \brief Prints a given number of bytes
 *
 * \param[in] v Pointer to an array of bytes
 * \param[in] size Integer that is number of bytes to be displayed
 */
void vect_print(uint8_t* v, uint32_t size) {
  for (uint32_t i = 0; i < size; ++i) {
    printf("%02x", v[i]);
  }
}