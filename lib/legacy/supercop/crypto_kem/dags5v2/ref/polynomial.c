/*
 * polynomial.c
 *
 *  Created on: Jun 2, 2018
 *      Author: vader
 */

#include "polynomial.h"

gf sum_vector(gf* w, int length) {
	gf tmp = 0;
	int j = 0;
	for (j = 0; j < length; j++) {
		tmp = tmp ^ w[j];
	}
	return tmp;
}

int polynomial_get_update_degree(polynomial *p) {
	int d = p->size - 1;
	while ((d >= 0) && (p->coefficient[d] == 0))
		--d;
	p->degree = d;

	return d;
}

void compute_syndrom(const gf* v, const gf* y, const unsigned char *cipher_text,
		polynomial *s) {

	int i, j;
	gf result = 0;
	gf w[code_length] = { 0 };
	int st = signature_block_size * pol_deg; //"r = s*t"
	for (j = 0; j < code_length; j++) {
		gf temp[2] = { 0 };
		temp[0] = gf_mult(cipher_text[j],
				(relative_field_representation((y[j]), 0)));
		temp[1] = gf_mult(cipher_text[j],
				(relative_field_representation((y[j]), 1)));
		result = absolut_field_representation(temp);
		w[j] = result;

	}
	result = sum_vector(w, code_length);

	//gf result = sum_vect_element(w, code_length);
	s->coefficient[0] = result;
	for (i = 1; i < st+1; i++) {
		for (j = 0; j < code_length; j++) {
			w[j] = gf_q_m_mult(v[j], w[j]);
		}

		result = sum_vector(w, code_length);
		//s->coefficient[i] = tmp;
		s->coefficient[i] = result;
	}

	/*for(i = 0; i < 8; i++)
	 {
	 printf("v[i] %" PRIu16 " ", s->coefficient[i]);
	 printf("\n");
	 }*/

	polynomial_get_update_degree(s);

}

void poly_set(polynomial *p, polynomial *q) {
	int d = p->size - q->size;
	if (d < 0) {
		memcpy(p->coefficient, q->coefficient, p->size * sizeof(gf));
		polynomial_get_update_degree(p);
	} else {
		memcpy(p->coefficient, q->coefficient, q->size * sizeof(gf));
		memset(p->coefficient + q->size, 0, d * sizeof(gf));
		polynomial_get_update_degree(q);
		p->degree = q->degree;
	}
}

polynomial* create_polynomial(int degree) {
	struct polynomial* poly = malloc(sizeof(polynomial));
	poly->degree = 0;
	poly->size = degree + 1;
	poly->coefficient = (gf *) calloc(poly->size, sizeof(gf));

	return poly;
}
gf poly_eval_aux(gf * coeff, gf a, int d) {
	gf b;
	b = coeff[d--];
	for (; d >= 0; --d) {
		if (b != 0) {
			b = gf_add(gf_q_m_mult(b, a), coeff[d]);
		} else {
			b = coeff[d];
		}
	}
	return b;
}

gf polynomial_evaluation(polynomial *p, gf a) {
	polynomial_get_update_degree(p);
	return poly_eval_aux(p->coefficient, a, p->degree);
}

polynomial* poly_multiplication(polynomial* p, polynomial* q) {
	int i, j, dp, dq;
	polynomial* r;

	polynomial_get_update_degree(p);
	polynomial_get_update_degree(q);
	dp = polynomial_get_degree(p);
	dq = polynomial_get_degree(q);
	r = create_polynomial(dp + dq);
	for (i = 0; i <= dp; ++i) {
		for (j = 0; j <= dq; ++j) {
			polynomial_add_to_coefficient(r, i + j,
					gf_q_m_mult(polynomial_get_coefficient(p, i), polynomial_get_coefficient(q, j)));
		}
	}
	polynomial_get_update_degree(r);

	return r;
}

void poly_add_free(polynomial* r, polynomial* a, polynomial* b) {
	int i;
	if (a->degree == -1) {
		r->degree = b->degree;
		memmove(r->coefficient, b->coefficient, b->degree * sizeof(gf));
	} else if (b->degree == -1) {
		r->degree = a->degree;
		//memcpy(r->coeff, a->coeff, a->deg * sizeof(gf));
		memmove(r->coefficient, a->coefficient, a->degree * sizeof(gf));
	} else {
		if (a->degree == b->degree) {
			r->degree = a->degree;
			for (i = 0; i < a->degree + 1; i++) {
				r->coefficient[i] = (a->coefficient[i]) ^ (b->coefficient[i]);
			}
		}
		if (a->degree > b->degree) {
			r->degree = a->degree;
			for (i = 0; i < b->degree + 1; i++)
				r->coefficient[i] = (a->coefficient[i]) ^ (b->coefficient[i]);
			//memcpy(r->coeff + (b->degree + 1),
			memmove(r->coefficient + (b->degree + 1),
					a->coefficient + (b->degree + 1),
					(a->degree - b->degree) * sizeof(gf));
		}
		if (b->degree > a->degree) {

			r->degree = b->degree;
			for (i = 0; i < a->degree + 1; i++)
				r->coefficient[i] = (a->coefficient[i]) ^ (b->coefficient[i]);
			memcpy(r->coefficient + (a->degree + 1),
					b->coefficient + (a->degree + 1),
					(b->degree - a->degree) * sizeof(gf));
		}
	}
}

void polynomial_free(polynomial* p) {
	assert(p->coefficient);
	free(p->coefficient);
	free(p);
}

polynomial* poly_copy(polynomial* p) {
	polynomial* q;
	q = (polynomial*) malloc(sizeof(struct polynomial));
	q->degree = p->degree;
	q->size = p->size;
	q->coefficient = (gf *) calloc(q->size, sizeof(gf));
	memcpy(q->coefficient, p->coefficient, p->size * sizeof(gf));
	return q;
}
polynomial* poly_rem(polynomial* rest, polynomial* g) {
	polynomial * p = poly_copy(rest);
	int i, j, d;
	gf a, b;
	polynomial_get_update_degree(p);
	polynomial_get_update_degree(g);
	d = p->degree - g->degree;
	if (d >= 0) {
		a = gf_q_m_inv(poly_tete(g));
		for (i = p->degree; d >= 0; --i, --d) {
			if (polynomial_get_coefficient(p, i) != 0) {
				b = gf_q_m_mult(a, polynomial_get_coefficient(p, i));
				for (j = 0; j < g->degree; ++j) {
					polynomial_add_to_coefficient(p, j + d,
							gf_q_m_mult(b, polynomial_get_coefficient(g, j)));
				}
				polynomial_set_coefficient(p, i, 0);
			}
		}
		polynomial_set_degree(p, g->degree - 1);
		while ((p->degree >= 0)
				&& (polynomial_get_coefficient(p, p->degree) == 0)) {
			polynomial_set_degree(p, p->degree - 1);
		}
	}
	return p;
}

polynomial* poly_quo(polynomial* p, polynomial* d) {
	int i, j, dd, dp;
	gf a, b;
	polynomial *quo, *rem;

	dd = polynomial_get_update_degree(d);
	dp = polynomial_get_update_degree(p);
	rem = poly_copy(p);
	quo = create_polynomial(dp - dd);
	quo->degree = dp - dd;
	a = gf_q_m_inv(polynomial_get_coefficient(d, dd));
	for (i = dp; i >= dd; --i) {
		b = gf_q_m_mult(a, polynomial_get_coefficient(rem, i));
		quo->coefficient[i - dd] = b;
		if (b != 0) {
			rem->coefficient[i] = 0;
			for (j = i - 1; j >= i - dd; --j) {
				polynomial_add_to_coefficient(rem, j,
						gf_q_m_mult(b, polynomial_get_coefficient(d, dd - i + j)));
			}
		}
	}
	polynomial_free(rem);
	return quo;
}

void print_polynomial(polynomial *p) {
	for (int i = p->degree; i > -1; i--) {
		printf("(");
		print_F_q_m_element(p->coefficient[i]);
		printf(")*z^%d + ", i);
	}
	printf("\n");

}

polynomial* polyShiftRight(polynomial *p, int places) {
	if (places <= 0)
		return p;
	int pd = p->degree;

	polynomial *d = poly_copy(p);
	//double[] d = Arrays.copyOf(p, p.length);
	for (int i = pd; i >= 0; --i) {
		d->coefficient[i + places] = d->coefficient[i];
		d->coefficient[i] = 0;
	}
	return d;
}
polynomial* p_div(polynomial* divi, polynomial* divisor) {

	polynomial *dividend = poly_copy(divi);

	polynomial *quotient, *remainder;

	int degree_dividend = polynomial_get_update_degree(dividend);
	int degree_divisor = polynomial_get_update_degree(divisor);

	int degree_quotient = degree_dividend - degree_divisor;
	quotient = create_polynomial(degree_quotient);
	while (degree_dividend >= degree_divisor) {
		gf coeff_dividend = dividend->coefficient[degree_dividend];
		gf coeff_divisor = divisor->coefficient[degree_divisor];
		degree_quotient = degree_dividend - degree_divisor;

		gf coeff_quotient = gf_div_f_q_m(coeff_dividend, coeff_divisor);
		quotient->coefficient[degree_quotient] = coeff_quotient;
		polynomial_get_update_degree(quotient);

		//print_polynomial(quotient);

		polynomial *temp = poly_multiplication(divisor, quotient);

		//int degree_remainder = degree_dividend-temp->degree;
		remainder = create_polynomial(degree_dividend);

		for (int i = 0; i < dividend->degree; i++) {
			remainder->coefficient[i] = dividend->coefficient[i]
					^ temp->coefficient[i];

		}
		polynomial_free(dividend);

		polynomial_get_update_degree(remainder);
		dividend = poly_copy(remainder);
		polynomial_free(temp);
		polynomial_free(remainder);
		degree_dividend = polynomial_get_update_degree(dividend);
	}
	polynomial_free(dividend);
	return quotient;
}

polynomial* p_rem(polynomial* divi, polynomial* divisor) {

	polynomial *dividend = poly_copy(divi);

	polynomial *quotient, *remainder;

	int degree_dividend = polynomial_get_update_degree(dividend);
	int degree_divisor = polynomial_get_update_degree(divisor);

	int degree_quotient = degree_dividend - degree_divisor;
	quotient = create_polynomial(degree_quotient);
	while (degree_dividend >= degree_divisor) {
		gf coeff_dividend = dividend->coefficient[degree_dividend];
		gf coeff_divisor = divisor->coefficient[degree_divisor];
		degree_quotient = degree_dividend - degree_divisor;

		gf coeff_quotient = gf_div_f_q_m(coeff_dividend, coeff_divisor);
		quotient->coefficient[degree_quotient] = coeff_quotient;
		polynomial_get_update_degree(quotient);

		polynomial *temp = poly_multiplication(divisor, quotient);

		//int degree_remainder = degree_dividend-temp->degree;
		remainder = create_polynomial(degree_dividend);

		for (int i = 0; i < dividend->degree; i++) {
			remainder->coefficient[i] = dividend->coefficient[i] ^ temp->coefficient[i];

		}

		polynomial_get_update_degree(remainder);
		dividend = remainder;
		polynomial_free(temp);
		degree_dividend = polynomial_get_update_degree(dividend);
	}
	return dividend;
}

polynomial* polynomial_addition(polynomial *u, polynomial *app) {
	polynomial * sum;
	if (u->degree > app->degree) {
		sum = create_polynomial(u->degree);
		for (int i = 0; i < app->degree+1; i++) {
			sum->coefficient[i] = u->coefficient[i] ^ app->coefficient[i];
		}
		for (int i = app->degree+1; i < u->degree+1; i++) {
			sum->coefficient[i] = u->coefficient[i];
		}
	} else {
		sum = create_polynomial(app->degree);
		for (int i = 0; i < u->degree+1; i++) {
			sum->coefficient[i] = u->coefficient[i] ^ app->coefficient[i];
		}
		for (int i = u->degree+1; i < app->degree+1; i++) {
			sum->coefficient[i] = app->coefficient[i];
		}
	}
	polynomial_get_update_degree(sum);
	return sum;
}
