/*
 * util.c
 *
 *  Created on: May 30, 2018
 *      Author: vader
 */

#include "util.h"

#define min(a,b) (((a)<(b))?(a):(b))


/*
 * store_public_key:
 * Store matrix src that is the public key into array.
 */

void store_public_key(matrix *src, unsigned char *dst) {
	int counter = 0;
	for (int i = 0; i < src->rows; i++) {
		for (int j = code_dimension; j < src->cols; j++) {
			dst[counter] = src->data[i * src->cols + j];
			counter++;
		}
	}
}

/*
 * store_secret_key:
 * Store two vectors that is the secret key into array.
 * The vectors are 16bits and the array is 8 bits.
 */

void store_secret_key(const gf *v, const gf *y, unsigned char *sk) {
	for (int i = 0; i < 2 * code_length; i = i + 2) {
		gf a = v[i / 2] >> 8;
		gf b = v[i / 2] & 0xFF;
		sk[i] = a;
		sk[i + 1] = b;
	}
	for (int i = 2 * code_length; i < 4 * code_length; i = i + 2) {
		gf a = y[(i / 2) - code_length] >> 8;
		gf b = y[(i / 2) - code_length] & 0xFF;
		sk[i] = a;
		sk[i + 1] = b;
	}



}

/*
 * recover_sk:
 * Recover two vectors that is the secret key from array.
 * The vectors are 16bits and the array is 8 bits.
 */
void recover_secret_key(const unsigned char* sk, gf* v, gf* y) {

	for (int i = 0; i < 2 * code_length; i = i + 2) {
		v[i / 2] = sk[i + 1] | (sk[i] << 8);
	}
	for (int i = 2 * code_length; i < 4 * code_length; i = i + 2) {
		y[(i / 2) - code_length] = sk[i + 1] | (sk[i] << 8);

	}

}

/*
 * recover_public_key_from_array:
 * Recover the matrix G (that is the public key) from an array.
 */
void recover_public_key_from_array(const unsigned char *public_key, matrix *G) {
	gf z[code_dimension] = { 0 };
	for (int i = 0; i < code_dimension; i++) {
		z[i] = 1;
	}

	for (int i = 0;
			i
					< min(
							code_length - ((signature_block_size * pol_deg) * extension),
							code_dimension); i++) {
		G->data[i * G->cols + i] = z[i];
	}

	int counter = 0;
	for (int i = 0; i < G->rows; i++) {
		for (int j = code_dimension; j < G->cols; j++) {
			G->data[i * G->cols + j] = public_key[counter];
			counter++;
		}
	}

}

/*void recover_public_key(const unsigned char *public_key, matrix *G) {
	int a = 0;
	int i, j, k, p, l, m, q;
	matrix *M = make_matrix(code_dimension, code_length - code_dimension);
	k = code_dimension / (signature_block_size);
	p = (code_length - code_dimension) / 4;
	gf c1 = 0, c2 = 0, c3 = 0, c4 = 0, tmp1 = 0, tmp2 = 0;
	q = (code_length - code_dimension) / (signature_block_size);
	unsigned char c = 0;
	gf sig[signature_block_size] = { 0 };
	gf signature_all_line[(code_length - code_dimension)] = { 0 };
	for (i = 0; i < k; i++) {
		for (j = 0; j < p; j++) {
			c = public_key[a];
			//printf("--c= %d \t",c);
			c1 = c >> 2;
			signature_all_line[4 * j] = c1;
			tmp1 = (c & 3);
			a += 1;
			c = public_key[a];
			//printf("--c= %d \t",c);
			c2 = (tmp1 << 4) ^ (c >> 4);
			signature_all_line[4 * j + 1] = c2;
			tmp2 = c & 15;
			a += 1;
			c = public_key[a];
			a += 1;
			//printf("--c= %d \t",c);
			c3 = (tmp2 << 2) ^ (c >> 6);
			signature_all_line[4 * j + 2] = c3;
			c4 = c & 63;
			signature_all_line[4 * j + 3] = c4;
		}
		for (l = 0; l < q; l++) {
			for (m = 0; m < (signature_block_size); m++) {
				sig[m] = signature_all_line[l * (signature_block_size) + m];
			}
			//affiche_vecteur(sig,order);
			quasi_dyadic_bloc_matrix(M, sig, l * (signature_block_size),
					i * (signature_block_size));
		}
	}
	for (i = 0; i < G->rows; i++) {
		G->data[i * G->cols + i] = 1;
		for (j = M->rows; j < G->cols; j++) {
			G->data[i * G->cols + j] = M->data[(i * M->cols) + j - M->rows];
		}
	}
	free_matrix(M);
}*/

void generate_int_list_of_size(int *list, int length) {
	for (int i = 0; i < length; i++) {
		list[i] = i;
	}
}

/*
 * generate_elements_in_F_q_m:
 * 	The list that is generated is a list of elements 1,2,3 ...
 * 	all the way up to F_q_m
 */
void generate_elements_in_order(gf * set_of_elements_in_F_q_m, int start_value,
		unsigned int size) {
	int i;
	for (i = 0; i < size; i++) {
		set_of_elements_in_F_q_m[i] = start_value + i;
	}
}

/*
 * random_m:
 * 	Currently this function is used to allow changes to the random byte function
 * 	called without having to affect change multiple location in code.
 */
void random_m(unsigned char *m) {
	randombytes(m, k_prime);
}

/*
 *	element_in_vector:
 *		This function goes through and checks if "value_to_check" in array v up to
 *		"size" elements in the array.
 *
 *	Param:
 *		v:	Provide the array to check for the value in
 *		value_to_check: Provide the value that needs to be checked for
 *		size:	Provide the number of elements in the array that need to be checked.
 *
 *	Return:
 *		Return EXIT_SUCCESS if the value is not contained in the array otherwise
 *		EXIT_FAILURE if it is.
 */
int element_in_vector(unsigned int *v, int value_to_check, int size) {
	int i;
	for (i = 0; i < size; i++) {
		if (v[i] == value_to_check) {
			return EXIT_FAILURE;
		}
	}
	return EXIT_SUCCESS;
}

/*
 * random_e:
 * 	Put errors into the error array based on the sigma provided
 *
 * Param:
 * 	sigma:  Provide the hash_sigma
 * 	error_array:	Provide an array to hold the values and locations of the errors
 */
void random_e(const unsigned char *sigma, unsigned char *error_array) {
	int i, j = 0, k = 0, jeton = 0;
	unsigned char gf_sigma;
	unsigned int v[weight] = { 0 };

	PRINT_DEBUG_UTIL("error_position: ");
	for (i = 0; i < code_length; i++) {
		gf_sigma = sigma[i] % F_q_size;
		if (gf_sigma == 0) {
			continue;
		}
		do {
			jeton = (sigma[k + 1] ^ (sigma[k] << 4)) % code_length;
			k++;
		} while (element_in_vector(v, jeton, j + 1) == 1); //Only check j elements
		v[j] = jeton;
		error_array[jeton] = gf_sigma;
		PRINT_DEBUG_UTIL("%d, ", jeton);
		j++;

		// Onlyl need to find weight errors
		if (j == weight) {
			break;
		}
	}
#ifdef DEBUG_UTIL
	PRINT_DEBUG_UTIL("\n");
	for (int i = 0; i < code_length; i++) {
		PRINT_DEBUG_UTIL("%d, ", error_array[i]);
	}
	PRINT_DEBUG_UTIL("\n");
#endif
}

void set_vy_from_sk(gf* v, gf * y, const unsigned char * sk) {
	int i, a = 0;
	unsigned char c;
	gf c1, c2, c3;
	for (i = 0; i < (code_length / 2); i++) {
		c = sk[a];
		c1 = c;
		a += 1;
		c = sk[a];
		c2 = c;
		a += 1;
		c = sk[a];
		c3 = c;
		a += 1;
		v[2 * i] = (c1 << 4) ^ (c2 >> 4);
		c1 = c2 & 15;
		v[2 * i + 1] = (c1 << 8) ^ c3;
	}
	for (i = 0; i < (code_length / 2); i++) {
		c = sk[a];
		c1 = c;
		a += 1;
		c = sk[a];
		c2 = c;
		a += 1;
		c = sk[a];
		c3 = c;
		a += 1;
		y[2 * i] = (c1 << 4) ^ (c2 >> 4);
		c1 = c2 & 15;
		y[2 * i + 1] = (c1 << 8) ^ c3;
	}

}

int compute_weight(unsigned char *vector, int size) {
	int i = 0, w = 0;
	for (i = 0; i < size; i++) {
		if (vector[i] != 0)
			w++;
	}
	return w;
}

int index_of_element(const gf *v, gf element) {
	for (int i = 0; i < code_length; i++) {
		if (v[i] == element) {
			return i;
		}
	}
	return -1;

}

void swap(gf* str, int i, int j) {
	char temp = str[i];
	str[i] = str[j];
	str[j] = temp;
}
void permute(gf *array, int i, int length) {
	int j = i;
	for (j = i; j < length; j++) {
		swap(array, i, j);
		permute(array, i + 1, length);
		swap(array, i, j);
	}
	return;
}

/*
 * check_positions
 * 	This function checks all of the values of an array to ensure that they
 * 	are no longer set to -1
 *
 * 	param:
 * 		pos		  Provide an interger array
 * 		size		Provide the number of elements in the array to check
 *
 * 	Results:
 * 		Returns EXIT_SUCCESS if there are no longer any values that are -1.
 * 		Otherwise EXIT_FAILURE
 */

int check_positions(const int *pos, const int size) {
	int i = 0;
	for (i = size-1; i > -1; i--) {
		if (pos[i] == -1) {
			return EXIT_FAILURE;
		}
	}
	return EXIT_SUCCESS;
}

int multiplicative_order(const int a) {
	int order = 1;
	gf result = gf_pow_f_q_m(a, order);
	while (result != 1) {
		order++;
		result = gf_pow_f_q_m(a, order);

	}
	return order;
}
gf discrete_logarithm(const gf a, const gf b) {
	int p = multiplicative_order(b); // multiplicative order of base
	gf y = a;

	int N = sqrt(p) + 1;
	gf tbl[N+1];
	for (int i = 0; i <= N; i++) {
		tbl[i] = gf_pow_f_q_m(b, i);
	}
	gf temp = gf_pow_f_q_m(b, N);
	gf inv_a_m = gf_q_m_inv(temp);
	for (int j = 0; j < N; j++) {
		for (int i = 0; i < N; i++) {
			if (y == tbl[i]) {
				gf result = i + (N * j);

				return result;
			}
		}
		y = gf_q_m_mult(y, inv_a_m);
	}

	return -1;
}
