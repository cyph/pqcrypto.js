/*
 * gf.h
 *
 *  Created on: Dec 1, 2017
 *      Author: vader
 */

#ifndef GF_H_
#define GF_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <inttypes.h>

#include "definitions.h"
#include "param.h"

gf relative_field_representation(gf a, int k);
gf absolut_field_representation(gf* element);

gf gf_q_m_inv(gf in);
gf gf_pow_f_q_m(gf in, gf power);
gf gf_pow_f_q(gf in, gf power);

gf gf_inv(gf in);
gf gf_div(gf a, gf b);
gf gf_div_f_q_m(gf a, gf b);
void print_F_q_element(gf a);
void print_F_q_m_element(gf a);


static inline uint8_t gf_mult(const gf in0, const  gf in1) {
	gf reduction;
	gf tmp;
	gf t0 = in0, t1 = in1;
	int i;

	//Multiplication
	tmp = 0;

	for (i = 0; i < 10; i++){
		tmp ^= (t0 * (t1 & (1 << i)));
	}

	//first step of reduction
	//second step of reduction
	for (i=0; i < 2; i++){
		reduction = (tmp >> 8) & 0x7F;
		tmp = tmp & 0xFF;
		tmp ^= reduction;
		tmp ^= reduction << 2;
		tmp ^= reduction << 3;
		tmp ^= reduction << 4;
	}

	return tmp;
}

static inline gf gf_q_m_mult(const gf in0, const  gf in1) {
	gf reduction;
	uint32_t tmp;
	gf t0 = in0, t1 = in1;
	int i;

	//Multiplication
	tmp = 0;

	for (i = 0; i < 18; i++){
		tmp ^= (t0 * (t1 & (1 << i)));
	}

	//first step of reduction
	\

	for (i = 0; i < 2; i++){
		reduction = (tmp >> 16) & 0x7FFF;
		tmp = tmp & 0xFFFF;
		tmp ^= reduction;
		tmp ^= reduction << 2;
		tmp ^= reduction << 3;
		tmp ^= reduction << 5;
	}

	return tmp;
}

static inline gf gf_add(gf a, gf b) {
	return (a ^ b);
}

static inline gf gf_sum(gf a, gf b) {
	return (a ^ b);
}

#endif /* GF_H_ */
