/*
 * decoding.c
 *
 *  Created on: Jun 2, 2018
 *      Author: vader
 */

#include "decoding.h"
/*
 * Decoding:
 * 	This function is used to find the error vector and the code_word.
 *
 * Return:
 * 	Return EXIT_SUCCESS if the successful at decoding otherwise EXIT_FAILURE
 */

int decoding(const gf* v, const gf* y, const unsigned char *c,
		unsigned char *error, unsigned char *code_word) {
	int i, k, j, dr, position;
	int st = (signature_block_size * pol_deg);
	polynomial *syndrom;
	polynomial *omega, *sigma, *re, *uu, *u, *quotient;
	polynomial *rest, *app, *temp, *temp_sum,*delta, *pos;

	gf pol_gf, tmp, tmp1, o;
	gf alpha;

	int aux_position[weight];
	gf aux_perm[F_q_m_size];

	memset(aux_position, -1, weight * sizeof(int));

	gf exp_alpha = (F_q_m_size - 1) / (F_q_size - 1);
	alpha = gf_pow_f_q_m(2, exp_alpha); //b^((q^m)-1)/(q-1)


	//Compute Syndrome normally
	PRINT_DEBUG_DEC("Decoding:Computing syndrom\n");

	syndrom = create_polynomial(st);
	compute_syndrom(v, y, c, syndrom);

	if (syndrom->degree == -1) {
		return EXIT_FAILURE;
	}

	//Resolution of the key equation
	re = create_polynomial(st);
	re->coefficient[st] = 1;

	polynomial_get_update_degree(re);

	app = create_polynomial(st);
	uu = create_polynomial(st);
	u = create_polynomial(st);
	u->coefficient[0] = 1;
	u->degree = 0;

	dr = syndrom->degree;

	PRINT_DEBUG_DEC("Computing re,u,quotient\n");
	while (dr >= (st / 2)) {

		quotient = poly_quo(re, syndrom);
		rest = poly_rem(re, syndrom);

		poly_set(re, syndrom); // re = p

		poly_set(syndrom, rest); // p = mo2d_re

		poly_set(app, uu); // app = UU

		poly_set(uu, u); //UU = U

		temp = poly_multiplication(u, quotient);
		temp_sum = polynomial_addition(temp, app);

		poly_set(u, temp_sum); // U = (U*quotient) + app
		polynomial_free(temp);
		polynomial_free(temp_sum);
		polynomial_free(quotient);
		polynomial_free(rest);

		dr = syndrom->degree;
	}

	polynomial_free(re);
	polynomial_free(uu);
	polynomial_free(app);


	PRINT_DEBUG_DEC("Computing delta, omega, sigma\n");

	delta = create_polynomial(0);
	// u over z with z = 0
	delta->coefficient[0] = gf_q_m_inv(polynomial_evaluation(u, 0));
	omega = poly_multiplication(syndrom, delta);
	polynomial_free(syndrom);
	sigma = poly_multiplication(u, delta);

	polynomial_free(delta);
	polynomial_free(u);

	generate_elements_in_order(aux_perm, 0, F_q_m_size);

	j = 0;

	PRINT_DEBUG_DEC("Computing error position\n");

	for (i = 0; i < F_q_m_size; i++) {
		// If the polynomial evaluates to zero
		if (!polynomial_evaluation(sigma, aux_perm[i])) {
			position = index_of_element(v, gf_q_m_inv(aux_perm[i]));

			PRINT_DEBUG_DEC("%d, ", position);

			aux_position[j] = position;
			j += 1;
		}
	}

	PRINT_DEBUG_DEC("\n");

	polynomial_free(sigma);
	//Element for determining the value of errors
	if (check_positions(aux_position, weight)) {
		return -1;
	}
#ifdef DEBUG
	PRINT_DEBUG_DEC("decoding error_position: ");
	for (i = 0; i < weight; i++) {
		PRINT_DEBUG_DEC("%d, ", aux_position[i]);
	}
	PRINT_DEBUG_DEC("\n");
#endif
	pos = create_polynomial(st / 2);
	for (i = 0; i < st / 2; i++) {
		pos->coefficient[i] = (gf) aux_position[i];
	}
	polynomial_get_update_degree(pos);
	if (pos->degree == -1) {
		return EXIT_FAILURE;
	}

	PRINT_DEBUG_DEC("Computing evaluating error position\n");
	app = create_polynomial(pos->degree);
	for (j = 0; j <= pos->degree; j++) {
		pol_gf = 1;
		for (i = 0; i < j; i++) {
			tmp = gf_q_m_mult(v[pos->coefficient[i]],
					gf_q_m_inv(v[pos->coefficient[j]]));
			tmp = gf_add(1, tmp);
			pol_gf = gf_q_m_mult(pol_gf, tmp);
		}
		for (i = j + 1; i <= pos->degree; i++) {
			tmp = gf_q_m_mult(v[pos->coefficient[i]],
					gf_q_m_inv(v[pos->coefficient[j]]));
			tmp = gf_add(1, tmp);
			pol_gf = gf_q_m_mult(pol_gf, tmp);
		}
		o = polynomial_evaluation(omega, gf_q_m_inv(v[pos->coefficient[j]]));
		tmp1 = gf_q_m_mult(y[pos->coefficient[j]], pol_gf);
		if (tmp1 != 0) {
			app->coefficient[j] = gf_div_f_q_m(o, tmp1);
		}
	}
	polynomial_get_update_degree(app);
	polynomial_free(omega);


	PRINT_DEBUG_DEC("Reconstruction of the error vector\n");

	//Reconstruction of the error vector
	for (i = 0; i <= app->degree; i++) {
		k = discrete_logarithm(app->coefficient[i], alpha);
		error[pos->coefficient[i]] = gf_pow_f_q(2, k); // Correction
	}
	polynomial_free(app);
	polynomial_free(pos);

	//Reconstruction of code_word
	for (i = 0; i < code_length; i++) {
		code_word[i] = (c[i] ^ error[i]) & F_q_m_order;
	}
	return EXIT_SUCCESS;
}
