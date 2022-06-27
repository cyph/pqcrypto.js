/**
 * \file tensor.cpp
 * \brief Implementation of tensor code
 */

#include "tensor.h"

/**
 * \fn void tensor_code_encode(uint8_t* em, uint8_t* m)
 * \brief Encoding the message m to a code word em using the tensor code
 *
 * First we encode the message using the BCH code, then with the repetition code to obtain
 * a tensor code word. 
 *
 * \param[out] em Pointer to an array that is the tensor code word
 * \param[in] m Pointer to an array that is the message
 */
void tensor_code_encode(uint8_t* em, uint8_t* m) {
	uint8_t tmp[VEC_N1_SIZE_BYTES] = {0};
	bch_code_encode(tmp, m);
  repetition_code_encode(em, tmp);

  #ifdef VERBOSE
    printf("\n\nBCH code word: "); vect_print(tmp, VEC_N1_SIZE_BYTES);
    printf("\n\nTensor code word: "); vect_print(em, VEC_N1N2_SIZE_BYTES);
  #endif
}

/**
 * \fn void tensor_code_decode(uint8_t* m, uint8_t* em)
 * \brief Decoding the code word em to a message m using the tensor code
 *
 * \param[out] m Pointer to an array that is the message
 * \param[in] em Pointer to an array that is the code word
 */
void tensor_code_decode(uint8_t* m, uint8_t* em) {
	uint8_t tmp[VEC_N1_SIZE_BYTES] = {0};
	repetition_code_decode(tmp, em);
	bch_code_decode(m, tmp);

  #ifdef VERBOSE
    printf("\n\nRepetition decoding result (the input for the BCH decoding algorithm): "); vect_print(tmp, VEC_N1_SIZE_BYTES);
  #endif
}