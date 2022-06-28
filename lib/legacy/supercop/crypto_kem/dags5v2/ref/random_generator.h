/*
 * random_generator.h
 *
 *  Created on: May 3, 2018
 *      Author: vader
 */

#ifndef INCLUDE_RANDOM_RANDOM_GENERATOR_H_
#define INCLUDE_RANDOM_RANDOM_GENERATOR_H_
#include <stdlib.h>

#include "definitions.h"
#include "param.h"

extern void random_elements(gf* set_of_elements_in_F_q_m);

extern int get_random_int(int size);

#endif /* INCLUDE_RANDOM_RANDOM_GENERATOR_H_ */
