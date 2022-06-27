/**
 * \file gf2x_avx2.c
 * \brief AVX2 implementation of multiplication of two polynomials
 */

#include "gf2x_avx2.h"

/**
 * \struct aligned_block
 * \brief a structure to allocate aligned space in memory
 */
typedef struct aligned_block {
  char * contents; /*!< Points to aligned data*/
  char filler[1]; /*!< for alignment; actually variable size*/
  char* aligned_contents; /*!< Pointed to by contents; aligned*/
} aligned_block;

/**
 * \fn static inline aligned_block * aligned_malloc(size_t alignment, size_t size)
 * \brief Allocate aligned memory
 *
 * \param[in] alignment a value that is the number of bytes by block
 * \param[in] size a value that is the total number of needed bytes
 * \return a pointer to a structure
 */
static inline aligned_block * aligned_malloc(size_t alignment, size_t size) {
  // Allocate block that is big enough for structur
  aligned_block* block = (aligned_block*) malloc(sizeof(size_t) + alignment-1 + size);
  // Find place to start allocated result; start with highest possible
  char* contents = ((char *)block + sizeof(size_t) + alignment-1);
  // Compute filler size
  size_t filler_size = (long int)contents % alignment;
  // Round down to multiple of alignment
  contents -= filler_size;
  // Fill block. unused space is filled on purpose
  block->contents = contents;
  memset(&block->filler, 0xEE, (long int)contents - (long int)&block->filler);
  memset(contents, 0, size);
  memset(contents+size, 0xFF, filler_size);
  return block;
}

/**
 * \fn void shiftXor(v8uint32 dest[], v8uint32 source[], unsigned int length, unsigned int distance)
 * \brief Computes dest ^= source>>distance,
 *
 * Source and dest are bignumbers
 * stored little endian in memory and read as vectors
 * Routine is optimized to efficiently use vector registers
 * we assume source is copied twice, you can use it to rotate
 * in picture form (with four bit words):
 * say distance = 11 and, v2 has length 14 bits
 * v2:       aaaa bbbb cccc dd..
 * we made a double vector as make_double_vector
 * to do that, shiftxor will read three vectors and write:
 * <pre>
 * (0000,0000)aaaa,bbbb cccc,ddaa aabb,bbcc ccdd,aaaa bbbb,cccc
 *                         \ |||\ |||\ |||\ |
 * v2:                     aaaa bbbb cccc dd..
 * </pre>
 *
 * \param[out] dest Array that is the result
 * \param[in] source Array that is the source of data
 * \param[in] length Integer that is size of the source
 * \param[in] distance Integer that is the amount of shits to do
 */
void shiftXor(v8uint32 dest[], v8uint32 source[], unsigned int length, unsigned int distance) {
  // Split the distance in number of bits, ints, and vectors
  // there are 32 bits in an int, and 8 ints in a vector
  const int distBits = distance & 0x1f;
  const int minDistBits = BITSPERINT - distBits;
  const int distInts = ( distance >> 5 ) & 0x7;
  const int distVectors = distance >> 8;
  // Prepare for shuffling integers in the vectors
  const v8uint32 countVector = {0,1,2,3,4,5,6,7};
  const v8uint32 rightPerm = countVector + distInts;
  const v8uint32 leftPerm = countVector + distInts + 1;

  v8uint32 previousVector, currentVector, rightVector, leftVector, shifted;
  v8uint32 *sourcePtr = source+distVectors;
  v8uint32 *destPtr = dest;
 
  previousVector = *sourcePtr++;
  for (uint32_t offset=0; offset<length; offset++) {
    currentVector = *sourcePtr++;
    // get the appropriate integers from the last two vectors
    rightVector = __builtin_shuffle(
    previousVector, currentVector, rightPerm);
    leftVector = __builtin_shuffle(
    previousVector, currentVector, leftPerm);
    // get the appropriate bits from the integers
    shifted = rightVector>>distBits | leftVector<<minDistBits;
    // do the xor
    *destPtr++ ^= shifted;    
    previousVector = currentVector;
  } 
}

/**
 * \fn static inline v8uint32* make_double_vector(const uint8_t* v, aligned_block* a)
 * \brief Construct vector concatenated with itself in aligned memory
 *
 * First we have to make two concatenated copies of the vector
 * in picture form, where we take:
 * PARAM_N = 13                 BYTESPERVECTOR = 2
 * VEC_N_ARRAY_SIZE_VEC = 2     VECTORSIZE = 8
 * VEC_N_ARRAY_SIZE_INT = 4
 * we have a number of 13 bits, written little-endian in words:
 * <pre>
 * v2:        aaaa bbbb cccc d...
 * we want to concatenate with itself to get:
 *            aaaa bbbb cccc daaa abbb bccc cd..
 * let's say a vector is two of these mini-ints, so we see this as:
 *            aaaa,bbbb cccc,d...
 * first we make room for twice the number of vectors, plus one:
 *  0000,0000 0000,0000 0000,0000 0000,0000 0000,0000
 * copy the data, skipping one initial vector:
 *  0000,0000 aaaa,bbbb cccc,d... 0000,0000 0000,0000
 *  now we use shiftXor with a distance of 2*8-13=3, and a length of 3:
 * source:   ^   dest: ^ 
 * 0000,0000 aaaa,bbbb cccc,d... 0000,0000 0000,0000
 *     ^ ^^^^ ^^^       vvvv vvvv      xor action 1
 *  0000,0000 aaaa,bbbb cccc,daaa 0000,0000 0000,0000
 *  xor action 2:^ ^^^^ ^^^       vvvv vvvv
 *  0000,0000 aaaa,bbbb cccc,daaa abbb,bccc 0000,0000
 *  xor action 3:          ^ ^^^^ ^^^       vvvv vvvv
 *  0000,0000 aaaa,bbbb cccc,daaa abbb,bccc cdaa,abbb
 *  (note the extra wraparound).
 *  The output is at result+1
 * </pre> 
 *
 * \param[in] v Pointer to the vector
 * \param[in] a Pointer to the aligned allocated memory
 * \return a pointer to the double vector
 */
static inline v8uint32* make_double_vector(const uint8_t* v, aligned_block* a) {
  v8uint32* result = (v8uint32*) a->contents;
  memcpy(result+1, v, VEC_N_SIZE_BYTES);
  shiftXor(result+VEC_N_ARRAY_SIZE_VEC, result, VEC_N_ARRAY_SIZE_VEC + 1, VEC_N_ARRAY_SIZE_VEC * VECTORSIZE - PARAM_N);
  
  return result;
}

/**
 * \fn void vect_sparse_from_bytes(uint32_t* o, const uint8_t* a)
 * \brief Get the support of the spare vector
 *
 * \param[out] o Pointer to an array that is the support
 * \param[in] a Pointer to the vector
 */
void vect_sparse_from_bytes(uint32_t* o, const uint8_t* a) {
  uint16_t k = 0;
  for (uint32_t i = 0; i < VEC_N_SIZE_BYTES; ++i) {
    for (int j = 0; j < 8; ++j) {
      uint8_t mask = 1 << j;
      if (mask & a[i]) {
        o[k] = (i << 3) + j;
        k++;
      }
    }
  }
}

/**
 * \fn void sparse_dense_mul(uint8_t* o, const uint8_t* v1, const uint8_t* v2, const uint32_t weight)
 * \brief Multiply two vectors
 *
 * This functions multiplies a sparse vector <b>v1</b> (of Hamming weight equal to <b>weight</b>)
 * and a dense vector <b>v2</b>. The multiplication is done using the matrix-vector form of the multiplication of two
 * polynomials modulo \f$ X^n - 1\f$. where the matrix is generated using the dense vector. 
 *
 * \param[out] o Pointer to the result
 * \param[in] v1 Pointer to the first vector
 * \param[in] v2 Pointer to the first vector
 * \param[in] weight Integer that is the weigt of the sparse vector
 */
void sparse_dense_mul(uint8_t* o, const uint8_t* v1, const uint8_t* v2, const uint32_t weight) {

  uint32_t* positions = (uint32_t*) calloc(weight, sizeof(uint32_t));
  vect_sparse_from_bytes(positions, v1);

  int resultLengthAsVectors = VEC_N_ARRAY_SIZE_VEC * 2 +1;
  aligned_block* s = aligned_malloc(BYTESPERVECTOR,
  resultLengthAsVectors * BYTESPERVECTOR);
  v8uint32* double_v2_minus_1 = make_double_vector(v2, s);

  aligned_block* a = aligned_malloc(sizeof(v8uint32), VEC_N_ARRAY_SIZE_VEC * BYTESPERVECTOR);
  v8uint32* result = (v8uint32*) a->contents;

  for(uint16_t i = 0 ; i < weight ; ++i) {
    shiftXor(result, double_v2_minus_1+1, VEC_N_ARRAY_SIZE_VEC, PARAM_N - positions[i]);
  }
  
  uint8_t * resultAsByte = (uint8_t *) result;
  resultAsByte[VEC_N_SIZE_BYTES - 1] &= BITMASK(PARAM_N, 8);
  memcpy(o, result, VEC_N_SIZE_BYTES);

  free(positions);
  free(s);
  free(a);
}