/*
 * cilipadi.h
 *
 *  Created on: 25 Feb 2019
 *      Author: mrz
 */

#ifndef CILIPADI128V1_REF_CILIPADI_H_
#define CILIPADI128V1_REF_CILIPADI_H_

#define BYTERATE 12 // bitrate in bytes
#define AROUNDS 18 // number of rounds for P_{a,n}
#define BROUNDS 16 // number of rounds for P_{b,n}
#define STATELEN 48 // state size in bytes
//#define DEBUG

int permutation_256(unsigned char *state, int rounds);
int permutation_384(unsigned char *state, int rounds);
int f_function(unsigned char *x, int l, int pround);
int xor_bytes(unsigned char *x, const unsigned char *y, int len);

#endif /* CILIPADI128V1_REF_CILIPADI_H_ */
