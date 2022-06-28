/*
 * decapsulation.h
 *
 *  Created on: May 4, 2018
 *      Author: vader
 */

#ifndef INCLUDE_DECAPSULATION_H_
#define INCLUDE_DECAPSULATION_H_

#include <stdio.h>
#include <stdlib.h>
#include "fips202.h"


#include "matrix_operations.h"

#include "gf.h"
#include "util.h"
#include "param.h"

#include "decoding.h"
#include "definitions.h"

extern int decapsulation(unsigned char *secret_shared,
		const unsigned char *cipher_text, const unsigned char *secret_key);

extern int decrypt(unsigned char *secret_shared,
		const unsigned char *cipher_text, const gf *v, const gf *y);

#endif /* INCLUDE_DECAPSULATION_H_ */
