
#ifndef RBC_H
#define RBC_H

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <x86intrin.h>

#define RBC_FIELD_Q 2
#define RBC_FIELD_M 151
#define RBC_ELT_UINT64 3
#define RBC_ELT_UR_UINT64 5

typedef uint64_t rbc_elt[RBC_ELT_UINT64];
typedef uint64_t rbc_elt_ur[RBC_ELT_UR_UINT64];

typedef rbc_elt* rbc_vec;
typedef rbc_vec rbc_vspace;

typedef struct {
	rbc_vec v;
	int32_t degree;
	int32_t max_degree;
} rbc_poly_struct;

typedef struct {
	uint32_t coeffs_nb;
	uint32_t* coeffs;
} rbc_poly_sparse_struct;

typedef rbc_poly_struct* rbc_poly;
typedef rbc_poly_sparse_struct* rbc_poly_sparse;

typedef rbc_poly rbc_qre;

uint64_t RBC_SQR_LOOKUP_TABLE[256];
static const rbc_elt RBC_ELT_MODULUS = {0x0000000000000009, 0x0000000000000000, 0x0000000000800000};


#ifndef min
  #define min(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef max
  #define max(a,b) (((a)>(b))?(a):(b))
#endif

#endif

