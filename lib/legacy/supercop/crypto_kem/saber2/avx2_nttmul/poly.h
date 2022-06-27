/*---------------------------------------------------------------------
This file has been adapted from the implementation 
(available at, Public Domain https://github.com/pq-crystals/kyber) 
of "CRYSTALS – Kyber: a CCA-secure module-lattice-based KEM"
by : Joppe Bos, Leo Ducas, Eike Kiltz, Tancrede Lepoint, 
Vadim Lyubashevsky, John M. Schanck, Peter Schwabe & Damien stehle
----------------------------------------------------------------------*/

#ifndef POLY_H
#define POLY_H

#include <stdint.h>
#include "polmul_params.h"
#include "SABER_params.h"

uint64_t clock_mul, clock_matrix, clock_secret, count_mul;

uint64_t clock_mv_vv_mul;

uint64_t count_enc;

typedef struct
{
  __attribute__((aligned(32)))
  int16_t coeffs[SABER_N];
} poly;

typedef struct{
  __attribute__((aligned(32)))
  int16_t coeffs[SABER_N];
} nttpoly;


typedef struct{
  poly vec[SABER_K];
} polyvec;

typedef struct{
  nttpoly vec[SABER_K];
} nttpolyvec;


void poly_getnoise(uint16_t *r,const unsigned char *seed, unsigned char nonce);

void poly_getnoise4x(uint16_t *r0, uint16_t *r1, uint16_t *r2, const unsigned char *seed, unsigned char nonce0, unsigned char nonce1, unsigned char nonce2, unsigned char nonce3);


#define poly_ntt POLYMUL_NAMESPACE(_poly_ntt)
void poly_ntt(nttpoly *r, const poly *a, const int16_t *pdata);
#define poly_invntt_tomont POLYMUL_NAMESPACE(_poly_invntt_tomont)
void poly_invntt_tomont(nttpoly *r, const nttpoly *a, const int16_t *pdata);

#define poly_basemul_montgomery POLYMUL_NAMESPACE(_poly_basemul_montgomery)
void poly_basemul_montgomery(nttpoly *r, const nttpoly *a, const nttpoly *b, const int16_t *pdata);
#define poly_crt POLYMUL_NAMESPACE(_poly_crt)
void poly_crt(poly *r, const nttpoly *a, const nttpoly *b);

#define poly_add POLYMUL_NAMESPACE(_poly_add)
void poly_add(poly *r, const poly *a, const poly *b);
#define poly_sub POLYMUL_NAMESPACE(_poly_sub)
void poly_sub(poly *r, const poly *a, const poly *b);
#define poly_mul POLYMUL_NAMESPACE(_poly_mul)
void poly_mul(poly *r, const poly *a, const poly *b);
#define polysmall_mul POLYMUL_NAMESPACE(_polysmall_mul)
void polysmall_mul(uint8_t *r, const uint8_t *a, const int8_t *b);


#endif
