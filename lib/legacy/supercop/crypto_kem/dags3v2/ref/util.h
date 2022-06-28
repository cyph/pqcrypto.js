/*
 * util.h
 *
 *  Created on: May 30, 2018
 *      Author: vader
 */

#ifndef INCLUDE_UTIL_UTIL_H_
#define INCLUDE_UTIL_UTIL_H_

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "randombytes.h"

#include "matrix_operations.h"

#include "definitions.h"
#include "gf.h"
#include "api.h"

extern void store_public_key(matrix *src, unsigned char *dst);

extern void store_secret_key(const gf *v,const gf *y, unsigned char *sk);

extern void random_m(unsigned char *m);

extern void random_e(const unsigned char *sigma, unsigned char *error_array);

extern void recover_public_key(const unsigned char *public_key, matrix *G);

void recover_public_key_from_array(const unsigned char *public_key, matrix *G);

extern void set_vy_from_sk(gf* v, gf * y, const unsigned char * sk);

extern void recover_secret_key(const unsigned char * sk, gf* v, gf * y);

extern int compute_weight(unsigned char *r, int size);

extern void permute(gf *array, int i, int length);

extern int index_of_element(const gf *v, gf element);

extern int check_positions(const int *pos, const int size);

extern gf discrete_logarithm(const gf a, const gf b);

extern void generate_int_list_of_size(int *list, int length);

void generate_elements_in_order(gf * set_of_elements_in_F_q_m, int start_value,
		unsigned int size);

#endif /* INCLUDE_UTIL_UTIL_H_ */
