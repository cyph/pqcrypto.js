/*
 * polynomial.h
 *
 *  Created on: Jun 2, 2018
 *      Author: vader
 */

#ifndef INCLUDE_STRUCTURES_POLYNOMIAL_H_
#define INCLUDE_STRUCTURES_POLYNOMIAL_H_

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "util.h"
#include "gf.h"

#include "definitions.h"

#include "matrix_operations.h"

#define polynomial_get_degree(p) ((p)->degree)
#define polynomial_get_size(p) ((p)->size)
#define polynomial_set_degree(p, d) ((p)->degree = (d))
#define polynomial_get_coefficient(p, i) ((p)->coefficient[i])
#define polynomial_set_coefficient(p, i, a) ((p)->coefficient[i] = (a))
#define polynomial_add_to_coefficient(p, i, a) ((p)->coefficient[i] = (((p)->coefficient[i])^(a)))
#define poly_tete(p) ((p)->coefficient[(p)->degree])

extern void compute_syndrom(const gf* v, const gf* y, const unsigned char *word, polynomial *s);

extern int polynomial_get_update_degree(polynomial *p);

extern polynomial* create_polynomial(int degree);

extern void poly_set(polynomial *p, polynomial *q);

extern gf polynomial_evaluation(polynomial *p, gf a);

extern polynomial* poly_multiplication(polynomial* p, polynomial* q);

extern void poly_add_free(polynomial* r, polynomial* a, polynomial* b);

extern void polynomial_free(polynomial* p);

extern polynomial* poly_rem(polynomial* p, polynomial* g);

//extern polynomial* poly_quo(polynomial* p, polynomial* d);

extern polynomial* poly_quo(polynomial* p, polynomial* d);

extern void print_polynomial(polynomial *p);

extern gf sum_vector(gf* w, int length);

extern polynomial* p_div(polynomial* n, polynomial* d);

extern polynomial* p_rem(polynomial* n, polynomial* d);

extern polynomial* polynomial_addition(polynomial *u, polynomial *app);


#endif /* INCLUDE_STRUCTURES_POLYNOMIAL_H_ */
