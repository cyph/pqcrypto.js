#ifndef LUOV_H
#define LUOV_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "immintrin.h"
#include <stdint.h>
#include <string.h>
#include <stdalign.h>

#include "parameters.h"
#include "LinearAlgebra.h"
#include "randombytes.h"
#include "keccakrng.h"
#include "api.h"
#include "intermediateValues.h"

void _sign(unsigned char *signature, const unsigned char *sk, const unsigned char* document, uint64_t len, int fast);

void generateKeyPair(unsigned char *pk, unsigned char *sk);
int verify(const unsigned char *pk, const unsigned char *signature, unsigned char* document, unsigned long long *len);
#define sign(sig,sk,document,doclen) _sign(sig,sk,document,doclen,0)

void generateBigKeyPair(unsigned char *pk, unsigned char *big_pk, unsigned char *big_sk);
void precompute_sign(unsigned char *big_sk, const unsigned char *sk);
#define sign_fast(sig,sk,document,doclen) _sign(sig,sk,document,doclen,1)
void precompute_verify(unsigned char *big_pk , const unsigned char *pk);
int verify_fast(const unsigned char *big_pk , const unsigned char *signature, unsigned char* document, unsigned long long *len);

#endif 
