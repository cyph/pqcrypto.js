/** 
 * \file bch.h
 * \brief Header file for bch.cpp
 */

#ifndef BCH_H
#define BCH_H

#include "vector.h"

/**
 * \struct gf_tables
 * \brief a structure of a Galois Field Log and Anti-Log tables
 *
 * This structure allows to storage Log and Anti-Log tables.
 */
typedef struct gf_tables {
  uint16_t size; /*!< The size of the arrays of this structure*/
  int16_t* log_tab; /*!< An array that contains the Log values */
  int16_t* antilog_tab; /*!< An array that contains the Anti-Log values */
} gf_tables;

/**
 * \struct sigma_poly
 * \brief a structure of a Galois Field polynomial
 *
 * This structure allows to storage of a polynomial with coordinates in \f$ GF(2^{10}) \f$. We use 
 * tis structure to compute the error location polynomial in the decoding phase of BCH code.
 */
typedef struct sigma_poly {
  uint16_t dim; /*!< The size of the array value of this structure*/
  uint16_t deg; /*!< The degree of the polynomial stored in the array value*/
  int16_t* value; /*!< An array that contains the coordinates of the polynomial*/
} sigma_poly;

/**
 * \struct syndrome_set
 * \brief a structure of a Syndromes set
 *
 * This structure allows to storage of a set of syndromes.
 */
typedef struct syndrome_set {
uint16_t size; /*!< The size of the array tab of this structure*/
int16_t* tab; /*!< An array that contains the values of syndromes*/
} syndrome_set;

/**
 * \struct cyclotomic_sets
 * \brief a structure of Cyclotomic sets
 *
 * This structure allows the storage of an array of cyclotomic sets
 */
typedef struct cyclotomic_sets {
  int8_t nb_c ;
  int8_t* tab ;
} cyclotomic_sets;

int16_t gf_get_antilog(gf_tables* tables, int16_t i);

int16_t gf_get_log(gf_tables* tables, int16_t i);

int16_t gf_mod(int16_t i);

void bch_code_encode(uint8_t* em, uint8_t* m);

void message_to_array(uint8_t* o, uint8_t* v);

void get_generator_poly(uint8_t* g);

void lfsr_encoder(uint8_t* em, uint8_t* g, uint8_t* m);

void array_to_codeword(uint8_t* v, uint8_t* c);

void bch_code_decode(uint8_t* m, uint8_t* em);

gf_tables* gf_tables_init();

void gf_generation(gf_tables* gf_tables);

syndrome_set* syndrome_init();

void syndrome_gen(syndrome_set* synd_set, gf_tables* tables, uint8_t* v);

sigma_poly* sigma_poly_init(int16_t dim);

void get_error_location_poly(sigma_poly* sigma, gf_tables* tables, syndrome_set* synd_set);

void sigma_poly_copy(sigma_poly* p1, sigma_poly* p2);

void chien_search(uint16_t* error_pos, uint16_t* size, gf_tables* tables, sigma_poly* sigma);

void error_poly_gen(uint8_t* e, uint16_t* error_pos, uint16_t size);

void message_from_codeword(uint8_t* o, uint8_t* v);

void gf_tables_clear(gf_tables* gf_tables);

void syndrome_clear(syndrome_set* synd_set);

void sigma_poly_clear(sigma_poly* poly);

int16_t gf_mult(gf_tables* tables, int16_t a, int16_t b);

cyclotomic_sets* cyclotomic_init();

void cyclotomic_clear(cyclotomic_sets* c_tab);

void cyclotomic_gen(cyclotomic_sets* c_tab);

uint8_t* polynomial_hex(int16_t* g);

void compute_generator_poly(int16_t* g);

#endif
