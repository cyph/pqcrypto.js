/*
 * encapsulation.c
 *
 *  Created on: May 4, 2018
 *      Author: vader
 */
#include <limits.h>
#include "encapsulation.h"
#include "apiorig.h"

/*
 * encapsulation:
 *   This function produces the ciphert_text given the shared secret and
 *   public key
 *
 * Params:
 * 	cipher_text: Provide an array that can hold the size of the ciphertext
 * 	secret_shared: Provide the shared secret
 * 	public_key: Provide the public_key
 */
int encapsulation(unsigned char *ciphert_text, unsigned char *secret_shared,
		const unsigned char *public_key) {
	int result;
	matrix *M = make_matrix((code_length - ((signature_block_size * pol_deg) * extension)), code_length);
	recover_public_key_from_array(public_key, M);
	result = encrypt(ciphert_text, secret_shared, M);
	free_matrix(M);
	M = NULL;
	return result;
}

int encrypt(unsigned char *ciphert_text, unsigned char *secret_shared,
		matrix *G) {

	int i;
	unsigned char m[code_dimension] = { 0 }; //[k_prime] = { 0 };
	unsigned char d[k_prime] = { 0 };
	unsigned char error_array[code_length] = { 0 };
	unsigned char sigma[(code_dimension - k_sec)] = { 0 };
	unsigned char hash_sigma[code_length] = { 0 };

	unsigned char u[code_dimension] = { 0 };
	gf c[code_length] = { 0 };
	unsigned char r[code_dimension] = { 0 };
	unsigned char K[ss_length] = { 0 };

	PRINT_DEBUG_ENCAP("Generation Random M: \n");

	random_m(m);
	// Only required for DAGs_1 at the moment but do not want to hard code it.
	if (F_q_size < UCHAR_MAX){
		for (i = 0; i < k_prime; i++) {
			m[i] = m[i] % F_q_size;
		}
	}

#ifdef DEBUG_ENCAP
	for (i = 0; i < k_prime; i++) {
		PRINT_DEBUG_ENCAP(" %" PRIu16 ", ", m[i]);
	}
	PRINT_DEBUG_ENCAP("\nStarting hashing: \n");
#endif

	shake256(r, code_dimension, m, k_prime);
	shake256(d, k_prime, m, k_prime);


	// Type conversion
	if (F_q_size < UCHAR_MAX) {
		for (i = 0; i < k_prime; i++) {
			d[i] = d[i] & (F_q_size - 1);
		}
		for (i = 0; i < code_dimension; i++) {
			r[i] = r[i] & (F_q_size - 1);
		}
	}

	PRINT_DEBUG_ENCAP("Generating sigma and rho: \n");
	memcpy(sigma, &r[k_sec], code_dimension - k_sec);
	PRINT_DEBUG_ENCAP("Expanding m: \n");
	memcpy(u, r, k_sec );
	memcpy(&u[k_sec], m, code_dimension - k_sec);

	PRINT_DEBUG_ENCAP("Generating error_array: \n");
#if defined(DAGS_3) || defined(DAGS_5)
	shake256(hash_sigma, code_length, sigma, k_prime);
#else
	shake128(hash_sigma, code_length, sigma, k_prime);
#endif

	random_e(hash_sigma, error_array);

#ifdef DEBUG_ENCAP
	PRINT_DEBUG_ENCAP("message:\n");
	for (i = 0; i < code_dimension - k_sec; i++)
		PRINT_DEBUG_ENCAP(" %" PRIu16 ", ", u[i]);
	PRINT_DEBUG_ENCAP("\nEncaps_error_array:\n");
	for (i = 0; i < code_length; i++) {
		printf(" %" PRIu16 ", ", error_array[i]);
	}
	PRINT_DEBUG_ENCAP("\n");
#endif

	PRINT_DEBUG_ENCAP("Computing m*G: \n");
	multiply_vector_matrix(u, G, c);//c = message*G

	PRINT_DEBUG_ENCAP("Computing (m*G) + error: \n");
	for (i = 0; i < code_length; i++) {
		ciphert_text[i] = gf_add(c[i] , error_array[i]); //c + error
	}
	memcpy(&ciphert_text[code_length], d, k_prime);

#ifdef DEBUG_ENCAP
	for (i = 0; i < code_length; i++)
		PRINT_DEBUG_ENCAP(" %" PRIu16 ", ", ciphert_text[i]);
	PRINT_DEBUG_ENCAP("|\nHashing (m*G) + error: \n");
#endif


	shake256(K, ss_length, m, k_prime);


	memcpy(secret_shared, K, ss_length);
	return EXIT_SUCCESS;
}
