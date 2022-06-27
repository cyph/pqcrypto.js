/*
	Finite field of order 2^16 implemented as polynomial ring F_2[x] mod x^16+x^12 + x^3 + 1
*/

#ifndef F8FIELD_H
#define F8FIELD_H

#include <stdint.h>
#include <stdio.h>
#include "parameters.h"

enum { twoPow8 = 256, f8units = twoPow8 - 1 };
typedef uint8_t FELT;

/* Field operations */

static inline
void printFELT(FELT a) {
	/* printf("%.5u ", a); */
	if(a==0){
		printf(".");
	}
	else{
		printf("X");
	}
}

static inline
void multiply_by_x_ct(FELT *b){
	FELT a = (*b&(1<<7))>>7;
	*b <<= 1;
	*b ^= a*(0x1b);
}

static inline
FELT multiply_ct(FELT a, FELT b){
	FELT out = 0;
	out ^= b * (a&1);
	multiply_by_x_ct(&b);
    a >>= 1;
	out ^= b * (a&1);
	multiply_by_x_ct(&b);
	a >>= 1;
	out ^= b * (a&1);
	multiply_by_x_ct(&b);
	a >>= 1;
	out ^= b * (a&1);
	multiply_by_x_ct(&b);
	a >>= 1;
	out ^= b * (a&1);
	multiply_by_x_ct(&b);
	a >>= 1;
	out ^= b * (a&1);
	multiply_by_x_ct(&b);
	a >>= 1;
	out ^= b * (a&1);
	multiply_by_x_ct(&b);
	a >>= 1;
	out ^= b * (a&1);
	return out;
}

static inline
FELT square_ct(FELT a){
	FELT out = 0;
	out ^= (a&1)*1;
	a >>=1;
	out ^= (a&1)*4;
	a >>=1;
	out ^= (a&1)*16;
	a >>=1;
	out ^= (a&1)*64;
	a >>=1;
	out ^= (a&1)*27;
	a >>=1;
	out ^= (a&1)*108;
	a >>=1;
	out ^= (a&1)*171;
	a >>=1;
	out ^= (a&1)*154;
	return out;
}

static inline
FELT inverse_ct(FELT a){
	a = square_ct(a);
	FELT out = a; 
	a = square_ct(a);
	out = multiply_ct(out,a);
	a = square_ct(a);
	out = multiply_ct(out,a);
	a = square_ct(a);
	out = multiply_ct(out,a);
	a = square_ct(a);
	out = multiply_ct(out,a);
	a = square_ct(a);
	out = multiply_ct(out,a);
	a = square_ct(a);
	out = multiply_ct(out,a);
	return out;
}

//#define scalarMultiply DEFINE_OPERATION(scalarMultiply)

#endif