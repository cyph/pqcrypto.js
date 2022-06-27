#ifndef GF_H
#define GF_H

/**
 * @file gf.h
 * Header file of gf.c
 */

#include <stddef.h>
#include <stdint.h>

void gf_generate(uint16_t *exp, uint16_t *log, const int16_t m);

uint16_t gf_log(uint16_t elt);
uint16_t gf_mul(uint16_t a, uint16_t b);
uint16_t gf_square(uint16_t a);
uint16_t gf_inverse(uint16_t a);
uint16_t gf_mod(uint16_t i);

#endif
