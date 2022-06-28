/*
 * definitions.h
 *
 *  Created on: May 3, 2018
 *      Author: vader
 */

#ifndef SRC_DEFINITIONS_H_
#define SRC_DEFINITIONS_H_

#include <stdint.h>

#define ERROR_ZERO_IN_A_POSITION -1

typedef uint16_t gf;
typedef uint32_t gf_p;

typedef int bool;
#define true 1
#define false 0
#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

struct signature_block {
	gf* signature;
};

struct matrix {
    int rows; // number of rows
    int cols; // number of columns
    gf* data;
};
typedef struct matrix matrix;


struct polynomial {
  int degree;
  int size;
  gf * coefficient;
};
typedef struct polynomial polynomial;

#ifdef DEBUG
	#define PRINT_DEBUG(s, ...) fprintf(stdout, s, ##__VA_ARGS__);
#else
	#define PRINT_DEBUG(s, ...) do {} while (0)
#endif

#ifdef DEBUG_DECAP
	#define PRINT_DEBUG_DECAP(s, ...) fprintf(stdout, s, ##__VA_ARGS__);
#else
	#define PRINT_DEBUG_DECAP(s, ...) do {} while (0)
#endif

#ifdef DEBUG_DECODING
	#define PRINT_DEBUG_DEC(s, ...) fprintf(stdout, s, ##__VA_ARGS__);
#else
	#define PRINT_DEBUG_DEC(s, ...) do {} while (0)
#endif

#ifdef DEBUG_ENCAP
	#define PRINT_DEBUG_ENCAP(s, ...) fprintf(stdout, s, ##__VA_ARGS__);
#else
	#define PRINT_DEBUG_ENCAP(s, ...) do {} while (0)
#endif

#ifdef DEBUG_UTIL
	#define PRINT_DEBUG_UTIL(s, ...) fprintf(stdout, s, ##__VA_ARGS__);
#else
	#define PRINT_DEBUG_UTIL(s, ...) do {} while (0)
#endif


#ifdef DEBUG
	#define print_vector(vector, size) {\
		PRINT_DEBUG("[");\
		for (int i = 0; i < size; i++) {\
			PRINT_DEBUG(" %" PRIu16 " ,", ((gf*)vector)[i]);\
		}\
		PRINT_DEBUG("]\n");\
	}
#else
	#define print_vector(vector, size){return EXIT_SUCCESS;}
#endif

#endif /* SRC_DEFINITIONS_H_ */
