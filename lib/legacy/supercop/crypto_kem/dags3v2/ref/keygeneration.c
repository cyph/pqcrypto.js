/*
 * keygeneration.c
 *
 *  Created on: May 3, 2018
 *      Author: Gustavo Banegas
 */

#include "keygeneration.h"
#include "time.h"

static int build_dyadic_signature_part_1(gf *signature_h);
static int build_dyadic_signature_part2(gf *signature_h, int * block_position);


/* copied from libsodium */
static uint32_t
randombytes_random(void)
{
    uint32_t r;

    randombytes((void *) &r, sizeof r);

    return r;
}

/* copied from libsodium; not constant-time */
uint32_t
randombytes_uniform(const uint32_t upper_bound)
{
    uint32_t min;
    uint32_t r;

    if (upper_bound < 2) {
        return 0;
    }
    min = (1U + ~upper_bound) % upper_bound; /* = 2**32 mod upper_bound */
    do {
        r = randombytes_random();
    } while (r < min);
    /* r is now clamped to a set whose size mod upper_bound == 0
     * the worst case (2**31+1) requires ~ 2 attempts */

    return r % upper_bound;
}


/*
 * contains_zero:
 * 	Check to see if the array does not contain zeros
 * exit:
 * 	Return EXIT_SUCCESS if the array does not contain zeros
 * 	and EXIT_FAILURE if the array does contain zeros.
 */

int contains_zero(gf *list, int length) {
	for (int i = 0; i < length; i++) {
		if (list[i] == 0)
			return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}



/*
 * This function goes through and checks all of the entries in the length to make
 * sure that they do not contain @random_e up to length.
 * Returns:
 * 	EXIT_SUCCESS if the element is not present
 * 	EXIT_FAILURE if the element is present
 */
int vector_contains(const gf *signature_h, const gf random_e, int length) {
	int i;
	for (i = 0; i < length; i++) {
		if (signature_h[i] == random_e)
			return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}


/*
 * build_dyadic_signature function
 *   This function is used to create the dyadic matrix required for the key_gen.
 *
 * Param:
 * 	dyadic_signature  The dyadic_signature is returned in this provided array.
 *
 * Return:
 * 	EXIT_SUCCESS if the signature was created correctly. If the matrix will fail
 * 	to be dyadic return EXIT_FAILURE so additional checks do not have to be performed.
 */
int build_dyadic_signature(gf *dyadic_signature) {
	PRINT_DEBUG("Build_dyadic_signature start\n");

	int block_position[n0] = { 0 };
	gf signature_h[F_q_m_size] = { 0 };
	int i, aux_count_transfer_block = 0;

	PRINT_DEBUG("Start build_dyadic_signature_part_1\n");
	if (EXIT_SUCCESS != build_dyadic_signature_part_1(signature_h))
	{
		PRINT_DEBUG("build_dyadic_signature_part_1 failed\n");
		return EXIT_FAILURE;
	}
	PRINT_DEBUG("Start build_dyadic_signature_part_2\n");
	if(EXIT_SUCCESS != build_dyadic_signature_part2(signature_h, block_position))
	{
		PRINT_DEBUG("build_dyadic_signature_part_2 failed\n");
		return EXIT_FAILURE;
	}

	for (i = 0; i < n0; i++) {
		memcpy(&dyadic_signature[aux_count_transfer_block],&signature_h[signature_block_size * block_position[i]],signature_block_size * sizeof(gf));
		aux_count_transfer_block += signature_block_size;
	}

	return EXIT_SUCCESS;
}

int build_dyadic_signature_part_1(gf *signature_h)
{

	gf h0 = 0, h0_inverse, i_inverse;
	gf random_e, temp;

	int t, i, j; //for loop variables

	//Set h0 to anything but 0
	do { h0 = randombytes_uniform(F_q_m_size - 1); }
	while (h0 == 0);

	h0_inverse = gf_q_m_inv(h0);

	signature_h[0] = h0;

	for (t = 0; t < field_extension; t++) {
		i = 1 << t;
		random_e = 0;

		//random_e must be not in signature_h or equal 0
		do { random_e = randombytes_uniform(F_q_m_size - 1);}
		while (random_e == 0 || vector_contains(signature_h + i, random_e, code_length));

		//Set signature_h[1<<t] to random_e
		signature_h[i] = random_e;

		//For loop through values from 1 to i setting signature_h values to the
		i_inverse = gf_q_m_inv(signature_h[i]);

		for (j = 1; j < i; j++) {
			if (signature_h[j] != 0) {
				// 1/h_i + 1/h_j + h/h0
				temp = i_inverse ^ gf_q_m_inv(signature_h[j]) ^ h0_inverse;
				if (temp != 0) {
					signature_h[i + j] = gf_q_m_inv(temp);
				} else {
					signature_h[i + j] = 0;
				}
			} else {
				signature_h[i + j] = 0;
			}
		}
	}

	if (EXIT_SUCCESS == contains_zero(signature_h, signature_block_size)){
		return EXIT_SUCCESS;
	}
	else{
		print_vector(signature_h, signature_block_size);
		return EXIT_FAILURE;
	}
}

int build_dyadic_signature_part2(gf *signature_h, int * block_position)
{
	int rand_num;
	int upper_bound = (F_q_m_size / signature_block_size) - 1;

	// A hash table to generate distinct random number
	gf tmp[(F_q_m_size / signature_block_size) - 1] = {0};
	// Mark zero as used
	tmp[0] = 1;


	block_position[0] = 0;
	int ll = 1;
	while (ll  <  n0) {
		rand_num = randombytes_uniform(upper_bound-1);
		if (tmp[rand_num] !=1){
			// rand_num is not used before, make as used
			tmp[rand_num] = 1;
			if (EXIT_SUCCESS == contains_zero(&signature_h[(rand_num * signature_block_size)], signature_block_size)){
				block_position[ll] = rand_num;
				ll++;
			}
			else{
				PRINT_DEBUG("vector contain zero %d\n", rand_num);
			}
		}
	}
	return EXIT_SUCCESS;
}

/*
 * is_vectors_disjoint:
 * 	Goes through value of u and ensure that no value of v is the same
 * param:
 * 		u	vector u from Cauchy support
 * 		v vector v from the Cauchy support
 * 	Return:
 * 		Return EXIT_SUCCESS if there are no matches otherwise EXIT_FAILURE
 */
int is_vectors_disjoint(gf *u, gf *v) {
	int i, j;
	//PRINT_DEBUG("is_vector_disjoint\n");
	for (i = 0; i < (signature_block_size); i++) {
		for (j = 0; j < code_length; j++) {
			if (u[i] == v[j]) {
				return EXIT_FAILURE;
			}
		}
	}
	return EXIT_SUCCESS;
}

/*
 * is_vector_disjoint:
 *   Go through the vector and ensure that there are no duplicate entries
 * param:
 * 	list 	provide a vector
 * 	size	provide the size of the vector to check
 *
 * Return:
 * 	Return EXIT_SUCCESS if the values are not the same otherwise EXIT_FAILURE
 *
 */
int is_vector_disjoint(gf *list, int size) { //TODO consider wrapping these tests to one function
	int i, j;
	for (i = 0; i < size; i++) {
		for (j = i + 1; j < size; j++) {
			if (list[i] == list[j]) {
				return EXIT_FAILURE;
			}
		}
	}
	return EXIT_SUCCESS;
}


/*
 * For loop through both arrays and if remove elements from the to_remove array
 * exists in they elements array then remove it.
 */
void remove_elements(gf *to_remove, gf *elements, int length) {
	int i, j;
	for (i = 0; i < length; i++) {
		for (j = 0; j < length; j++) {
			if (to_remove[i] == elements[j]) {
				to_remove[i] = 0;
			}

		}
	}
}

/*
 * build_support
 * 	This function is used to generate the vector_u and vector_v vectors from the signature_h
 *
 * params:
 * 	signature_h  Provide the signature vector
 * 	vector_u and vector_v			 Provide allocated vectors
 * 	elements		 Provide the generated elements vector to be copied from.
 *
 * 	Returns:
 * 	Vectors vector_u and vector_v are filled in appropriately
 */
void build_support(gf* restrict vector_u, gf* restrict vector_v,
				const gf* restrict signature_h, const gf* restrict elements) {
	// [1, F_q_m_size-1]
	gf elements_in_F_q_m[code_length];
	gf signature_h_inv[code_length];
	gf aux[code_length] = { 0 };
	gf h0_inv = gf_q_m_inv(signature_h[0]);
	int i;
	gf omega = 0;

	memcpy(elements_in_F_q_m, elements, code_length*sizeof(gf));

	PRINT_DEBUG("build_support start\n");
	for (i = 0; i < code_length; i++) {
		signature_h_inv[i] = gf_q_m_inv(signature_h[i]);
		aux[i] = h0_inv ^ signature_h_inv[i];
	}
	remove_elements(elements_in_F_q_m, aux, code_length);


	do {
		omega = elements_in_F_q_m[randombytes_uniform(code_length - 1)];
	} while (omega == 0);

	for (i = 0; i < code_length; i++) {
		if (signature_h[i] != 0) {
			if (i < signature_block_size){
				vector_u[i] = signature_h_inv[i] ^ omega;
				vector_v[i] = vector_u[i] ^ h0_inv;
			}
			else{
				vector_v[i] =  signature_h_inv[i] ^ h0_inv ^ omega;
			}
		}
	}
}

/*
 * build_cauchy_matrix
 * 	This function builds the cauchy matrix using the provided u and v vectors
 *
 * 	param:
 * 		v				Provide the v vector used to generate the cauchy matrix
 * 		u				Provide the u vector used to generate the cauchy matrix
 * 		H_cauchy Provide the cauchy matrix to be filled in
 *
 * 	Results:
 * 		The cauchy matrix is calculated and stored in the pointer
 */
void build_cauchy_matrix(gf *u, gf *v, matrix *H_cauchy) {
	int i, j, k;

	PRINT_DEBUG("Building the cauchy matrix\n");
	for (k = 0; k < pol_deg; k++) {
		for (i = 0; i < signature_block_size; i++) {
			for (j = 0; j < code_length; j++) {
				H_cauchy->data[(k * signature_block_size + i) * code_length + j] =
						gf_pow_f_q_m(gf_q_m_inv((u[i] ^ v[j])), k + 1);
			}
		}
	}
}

/*
 * build_trapdoor:
 * 	This function builds the trap vector y and returns int in the pointer y
 *
 * param:
 * 	H_cauchy 			Provide the cauchy matrix
 * 	v and u				Provide the vectors v and u
 * 	y							A pointer to return the trapdoor in
 * 	H							A matrix to be filled out with the data
 *
 * Return:
 * 	EXIT_SUCCES if trapdoor is build otherwise EXIT_FAILURE
 */
int build_trapdoor(const matrix* restrict H_cauchy, const gf* restrict v,
		const gf* restrict u,  gf* restrict y,	matrix* restrict H) {
	gf random_el = 0;
	gf pol, sum_u_v, result;
	int i, j, ret_val = EXIT_FAILURE;

	gf z_short[n0] = {0};
	gf z[code_length] = {0};

	PRINT_DEBUG("build_trapdoor start\n");

	for (i = 0; i < n0; i++) {
		do {
			random_el = randombytes_uniform(F_q_m_size - 1);
			//TODO this vector_contains function could be optimized but very little gained
			// only need to check every signature_block_size element to see if it matches
		} while (random_el == 0 || vector_contains(z, random_el, code_length));

		for (j = 0; j < signature_block_size; j++) {
			z[(i * signature_block_size) + j] = random_el;
		}
		z_short[i] = random_el;
	}
	// ******
	// TODO: apply this
	// random_distinct(&z_short, n0);
	/*
	for (i = 0; i< n0; i++){
		printf("%d ", z_short[i]);
	}
	*/
	// ******


	matrix_multiply(H, H_cauchy, z_short);

	for (i = 0; i < code_length; i++) {
		pol = 1;
		for (j = 0; j < signature_block_size; j++) {
			sum_u_v = (v[i] ^ u[j]);
			result = gf_pow_f_q_m(sum_u_v, pol_deg);
			pol = gf_q_m_mult(pol, result);
		}
		// PRINT_DEBUG("-- %d / %d", z[i], pol);
		// TODO: efficient cache for diff.
		y[i] = gf_div_f_q_m(z[i], pol);
	}
	ret_val = EXIT_SUCCESS;

	return ret_val;
}

void project_H_on_F_q(const matrix* restrict H, matrix* restrict Hbase) {
	int k, i, j;
	int nr_rows = H->rows;
	int nr_cols = H->cols;
	PRINT_DEBUG("project_H on f_q\n");
	for (k = 0; k < extension; k++) {
		for (i = 0; i < nr_rows; i++) {
			for (j = 0; j < nr_cols; j++) {
				Hbase->data[(k * nr_rows + i) * nr_cols + j] =
						relative_field_representation(H->data[i * nr_cols + j], k);
			}
		}
	}

}

int generate_systematic_matrix(const matrix* Hbase) {

	int mm, i, j, l = 0;
	int num_cols = Hbase->cols;
	int num_rows = Hbase->rows;
	gf invPiv = 1;
	gf piv_align;

	for (i = 0; i < num_rows; i++) {
		l = 0;
		j = i + num_cols - num_rows;
		if (Hbase->data[(i * num_cols) + j] == 0) { //We're looking for a non-zero pivot

			//printf("search Pivot\n");
			for (l = i + 1; l < num_rows; l++) {
				if (Hbase->data[l * num_cols + j]) {
					//printf("Find Pivot\n");
					for (mm = 0; mm < num_cols; mm++) {
						Hbase->data[(l * num_cols) + mm] ^= Hbase->data[(i * num_cols) + mm];
						Hbase->data[(i * num_cols) + mm] ^= Hbase->data[(l * num_cols) + mm];
						Hbase->data[(l * num_cols) + mm] ^= Hbase->data[(i * num_cols) + mm];
					}
					break;
				}
			}
		}
		if (l == num_rows && (i != (num_rows - 1))) {
			// printf("Non systematic Matrix %d\num_cols", l);
			return EXIT_FAILURE;
		}
		// if (test == 1) { // We switches the lines l and i
		// 	test = 0;
		// 	//printf("Permut line\n");
		// 	//temp=P[i+n-num_rows];
		// 	//P[i+n-num_rows]=P[j];
		// 	//P[j]=temp;


		// }


		//   Matrix standardization
		if (Hbase->data[(i * num_cols) + j] != 1) {
			invPiv = gf_inv(Hbase->data[(i * num_cols) + j]);
			Hbase->data[(i * num_cols) + j] = 1;

			// for (mm = 0; mm < num_cols; mm++) {
			// 	if (mm != j) {
			// 		// continue;
			// 		Hbase->data[(i * num_cols) + mm] = gf_mult(
			// 				Hbase->data[(i * num_cols) + mm], invPiv);
			// 	}
			// }
			for (mm = 0; mm < j; mm++) {
				Hbase->data[(i * num_cols) + mm] = gf_mult(Hbase->data[(i * num_cols) + mm], invPiv);
			}
			for (mm = j+1; mm < num_cols; mm++) {
				Hbase->data[(i * num_cols) + mm] = gf_mult(Hbase->data[(i * num_cols) + mm], invPiv);
			}

		}

		for (l = 0; l < i; l++) {
			if (Hbase->data[(l * num_cols) + j]) {
				piv_align = Hbase->data[(l * num_cols) + j];
				for (mm = 0; mm < num_cols; mm++) {
					Hbase->data[(l * num_cols) + mm] ^= gf_mult(piv_align,Hbase->data[(i * num_cols) + mm]);
				}
			}
		}
		for (l = i+1; l < num_rows; l++) {
			if (Hbase->data[(l * num_cols) + j]) {
				piv_align = Hbase->data[(l * num_cols) + j];
				for (mm = 0; mm < num_cols; mm++) {
					Hbase->data[(l * num_cols) + mm] ^= gf_mult(piv_align,Hbase->data[(i * num_cols) + mm]);
				}
			}
		}
	}
	return 0;


}

/*
 * generate_public_key
 * 	Function used to generate the public key
 */
int generate_public_key(const matrix *Hbase, matrix *G) {
	int num_cols, num_rows, i;
	matrix *M, *m_temp, *m_transposed, *final;

	if(EXIT_FAILURE == generate_systematic_matrix(Hbase)){
		PRINT_DEBUG("Failed to generate_systematic_matrix\n");
		return EXIT_FAILURE;
	}

	num_cols = Hbase->cols;
	num_rows = Hbase->rows;


	M = submatrix(Hbase, 0, 0, num_rows, (num_cols - num_rows));

	m_transposed = transpose_matrix(M);

	free_matrix(M);


	m_temp = make_matrix((num_cols - num_rows), (num_cols - num_rows));
	for (i = 0; i < (num_cols - num_rows); i++)
		m_temp->data[i * (num_cols - num_rows) + i] = 1;


	// TODO augment is only used here. We could save memory usages by passing in
	// G to this function
	final = augment(m_temp, m_transposed);
	free_matrix(m_temp);
	free_matrix(m_transposed);

	memcpy(G->data, final->data, final->rows * final->cols * sizeof(gf));

	free_matrix(final);
	return EXIT_SUCCESS;

}

int key_pair_generation(unsigned char *pk, unsigned char *sk) {
	gf v[code_length] = { 0 };
	gf y[code_length] = { 0 };
	matrix G;
	G.cols = code_length;
	G.rows = code_length - ((signature_block_size * pol_deg) * extension);
	gf data_G[code_length * (code_length - ((signature_block_size * pol_deg) * extension))] = { 0 };
	G.data = data_G;
	key_gen(v, y, &G);
	store_public_key(&G, pk);
	store_secret_key(v, y, sk);

	return 0;
}



void key_gen(gf *vector_v, gf *y, matrix *G) {
	int ret_value = 0;
	gf signature_h[code_length] = {0};
	gf vector_u[signature_block_size] = {0};

	long build_support_failures = 0;
	long build_dyadic_sig_failures =0;

	matrix H_cauchy;
	gf data_cauchy[signature_block_size * pol_deg * code_length] = { 0 };
	H_cauchy.rows = signature_block_size * pol_deg;
	H_cauchy.cols = code_length;
	H_cauchy.data = data_cauchy;

	matrix H; // TODO H.data is filled allocated in build_trapdoor
	gf data_H[signature_block_size * pol_deg * code_length] = { 0 };
	H.rows = signature_block_size * pol_deg;
	H.cols = code_length;
	H.data = data_H;
	// PRINT_DEBUG("H row col = %d %d\n", H.rows, H.cols);

	matrix Hbase;
	Hbase.rows = (signature_block_size * pol_deg) * extension;
	Hbase.cols = code_length;
	gf data_Hbase[signature_block_size * pol_deg * code_length * extension] =
			{ 0 };
	Hbase.data = data_Hbase;


	PRINT_DEBUG("Key Gen start\n");

	// Only generate_elements in F_q_m once and copied when used in build_support()
	// With starting value of 1
	gf start_value = 1;
	gf elements_in_F_q_m_constant[F_q_m_size];
	for (int i = 0; i < F_q_m_size; i++){
		elements_in_F_q_m_constant[i] = start_value + i;
	}

	do {

		do {
			build_support_failures ++;
			ret_value = build_dyadic_signature(signature_h);
			if (ret_value == EXIT_SUCCESS){
				build_support(vector_u, vector_v, signature_h, elements_in_F_q_m_constant);
			}
			else
			{

				PRINT_DEBUG("Build dyadic failed!\n");
				build_dyadic_sig_failures ++;
				build_support_failures--;
			}
			if (build_support_failures %100 == 0){
				PRINT_DEBUG("interations %ld vs %ld\n",build_support_failures, build_dyadic_sig_failures);
			}

		} while (ret_value != EXIT_SUCCESS || is_vectors_disjoint(vector_u, vector_v) || is_vector_disjoint(vector_v, code_length)
				|| is_vector_disjoint(vector_u, signature_block_size));


		build_cauchy_matrix(vector_u, vector_v, &H_cauchy);

		if (EXIT_FAILURE == (ret_value = build_trapdoor(&H_cauchy, vector_v, vector_u, y, &H))){
			PRINT_DEBUG("Failed to build_trapdoor\n");
			continue;
		}

		project_H_on_F_q(&H, &Hbase);

		PRINT_DEBUG("Generating public_key\n");
		ret_value = generate_public_key(&Hbase, G);

	} while (ret_value);
	return;
}
