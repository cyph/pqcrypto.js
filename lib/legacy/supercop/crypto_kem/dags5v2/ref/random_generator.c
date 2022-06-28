/*
 * random_generator.c
 *
 *  Created on: May 3, 2018
 *      Author: vader
 */

#include "random_generator.h"
#include "randombytes.h"


int get_random_int(int size) {
	unsigned char i[1] = { 0 };
	randombytes(i, 1);
	return (i[0] % size);

}

void random_elements(gf* set_of_elements_in_F_q_m) {
	int i, j, v;
	gf tmp;
	unsigned char *random_bytes = 0;
	random_bytes = malloc(F_q_m_size * sizeof(gf));
	randombytes(random_bytes, F_q_m_size * sizeof(gf));
	for (i = 1; i <= F_q_m_order; i++) {
		set_of_elements_in_F_q_m[i - 1] = i;
	}

	for (j = 0; j < F_q_m_order; j++) {
		v = (random_bytes)[j] % (j + 1);
		tmp = set_of_elements_in_F_q_m[j];
		set_of_elements_in_F_q_m[j] = set_of_elements_in_F_q_m[v];
		set_of_elements_in_F_q_m[v] = tmp;
	}

	//remove 0's
	for (i = 0; i < F_q_m_order - 1; i++) {
		if (set_of_elements_in_F_q_m[i] == 0) {
			set_of_elements_in_F_q_m[i] = set_of_elements_in_F_q_m[i + 1];
		}
	}
	free(random_bytes);
}
