/**
 * \file repetition.h
 * \brief Header file for repetition.cpp
 */

#ifndef REPETITION_H
#define REPETITION_H

#include <string.h>

#include "vector.h"

void repetition_code_encode(uint8_t* em, uint8_t* m);
void repetition_code_decode(uint8_t* m, uint8_t* em);

void array_to_rep_codeword(uint8_t* o, uint8_t* v);

#endif