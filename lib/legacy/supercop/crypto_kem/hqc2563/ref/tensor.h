/**
 * \file tensor.h
 * \brief Header file for tensor.cpp
 */

#ifndef TENSOR_H
#define TENSOR_H

#include "bch.h"
#include "repetition.h"

void tensor_code_encode(uint8_t* em, uint8_t* m);
void tensor_code_decode(uint8_t* m, uint8_t* em);

#endif