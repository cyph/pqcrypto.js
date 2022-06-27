/**
 * \file repetition.cpp
 * \brief Implementation of repetition codes
 */

#include <stdlib.h>

#include "repetition.h"

/**
 * \fn void repetition_code_encode(uint8_t* em, uint8_t* m)
 * \brief Encoding each bit in the message m using the repetition code
 *
 * For reasons of clarity and comprehensibility, we do the encoding by storing the encoded bits in a String (each bit in an unsigned char),
 * then we parse the obtained string to an compact array using the function array_to_rep_codeword().
 *
 * \param[out] em Pointer to an array that is the code word
 * \param[in] m Pointer to an array that is the message
 */
void repetition_code_encode(uint8_t* em, uint8_t* m) {
  uint8_t tmp [PARAM_N1N2] = {0};
  uint8_t val;
  
  for(uint16_t i = 0; i < (VEC_N1_SIZE_BYTES - 1); ++i)  {
    for(uint8_t j = 0; j < 8; ++j) {
      val = (m[i]  >> j) & 0x01;
      if(val){
        uint32_t index = (i * 8 + j) * PARAM_N2; 
        for (uint8_t k = 0; k < PARAM_N2; ++k) {
          tmp[index + k] = 1;
        }
      }
    } 
  }
  
  for(uint8_t j = 0; j < (PARAM_N1 % 8); ++j) {
    uint8_t val = (m[VEC_N1_SIZE_BYTES - 1] >> j) & 0x01;
    if(val){
      uint32_t index = ((VEC_N1_SIZE_BYTES - 1) * 8 + j) * PARAM_N2; 
      for(uint8_t k = 0; k < PARAM_N2; ++k) {
        tmp[index + k] = 1;
      }
    }
  } 
  
  array_to_rep_codeword(em, tmp);
}

/**
 * \fn void repetition_code_decode(uint8_t* m, uint8_t* em)
 * \brief Decoding the code words to a message using the repetition code
 *
 * We use a majority decoding. In fact we have that PARAM_N2 = 2 * PARAM_T + 1, thus,
 * if the Hamming weight of the vector is greater than PARAM_T, the code word is decoded
 * to 1 and 0 otherwise.
 *
 * \param[out] m Pointer to an array that is the message
 * \param[in] em Pointer to an array that is the code word
 */
void repetition_code_decode(uint8_t* m, uint8_t* em) {
  int t = 0;
  int k = 1;
  int weight = 0;
  for(uint16_t i = 0; i < VEC_N1N2_SIZE_BYTES; ++i) {
    for(uint8_t j = 0; j < 8; ++j) {
      if((em[i] >> j) & 0x01) {
        weight ++;
      } 
      if(!(k % PARAM_N2)) {
        if(weight >= (PARAM_T + 1)) {
          int index = t / 8;
          m[index] |= 0x01 << (t % 8);
          weight = 0;
          t++;
        } else {
        weight = 0;
        t++;
        }
      }
      k++;
    }
  }
}

/**
 * \fn void array_to_rep_codeword(uint8_t* o, uint8_t* v)
 * \brief Parse an array to an compact array
 *
 * \param[out] o Pointer to an array
 * \param[in] v Pointer to an array 
 */
void array_to_rep_codeword(uint8_t* o, uint8_t* v) {
  for(uint16_t i = 0 ; i < (VEC_N1N2_SIZE_BYTES - 1) ; ++i) {
    for(uint8_t j = 0 ; j < 8 ; ++j) {
      o[i] |= v[j + i * 8] << j;
    }
  }
  
  for(uint8_t j = 0 ; j < PARAM_N1N2 % 8 ; ++j) {
    o[VEC_N1N2_SIZE_BYTES - 1] |= ((uint8_t) v[j + 8 * (VEC_N1N2_SIZE_BYTES - 1)]) << j;
  }
}