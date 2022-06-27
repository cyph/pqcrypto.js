#ifndef POLYMUL_PARAMS_H
#define POLYMUL_PARAMS_H

#include "SABER_params.h"

#define POLYMUL_SYMBYTES 32
#define POLYMUL_NAMESPACE(s) nttmul##s

#define KEM_N SABER_N
#define POLY_N SABER_N
#define NTT_N SABER_N
#define KEM_Q SABER_Q
#define KEM_K SABER_K

#define NEGACYCLIC

#endif
