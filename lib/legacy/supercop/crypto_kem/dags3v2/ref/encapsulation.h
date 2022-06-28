/*
 * encapsulation.h
 *
 *  Created on: May 4, 2018
 *      Author: vader
 */

#ifndef INCLUDE_ENCAPSULATION_H_
#define INCLUDE_ENCAPSULATION_H_

#include "fips202.h"

#include "util.h"
#include "matrix_operations.h"

#include "definitions.h"
#include "param.h"

extern int encapsulation(unsigned char *ciphert_text,
		unsigned char *secret_shared, const unsigned char *public_key);

extern int encrypt(unsigned char *ciphert_text, unsigned char *secret_shared,
		matrix *G);

#endif /* INCLUDE_ENCAPSULATION_H_ */
