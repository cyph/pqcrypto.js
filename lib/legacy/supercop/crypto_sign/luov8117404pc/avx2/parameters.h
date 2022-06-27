#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <stdint.h>
#include "immintrin.h"

/*
We define the size of the finite field to use (options are 8, 16, 32, 48, 64 and 80 ),
OIL_VARS, the number oil variables and the number of equations in the UOV system,
VINEGAR_VARS, the number of vinegar variables,
SHAKEVENUM, the version of the shake XOF that is used (either 128 or 256)
and Wherther or not we are using Message Recovery or not
*/

// Enable / Disable message recovery mode
//#define MESSAGE_RECOVERY

// Enable / Disable precomputaion
#define PRECOMPUTE

// Choose a parameter set

/* SECURITY LEVEL 2
		#define FIELD_SIZE 8
		#define OIL_VARS 63
		#define VINEGAR_VARS 256 
		#define SHAKENUM 128 */

/* SECURITY LEVEL 4 
		#define FIELD_SIZE 8
		#define OIL_VARS 90
		#define VINEGAR_VARS 351 
		#define SHAKENUM 256 */

/* SECURITY LEVEL 5 */
		#define FIELD_SIZE 8
		#define OIL_VARS 117
		#define VINEGAR_VARS 404 
		#define SHAKENUM 256  

/* Custom parameter set */
 /*
	#define FIELD_SIZE 
	#define OIL_VARS 
	#define VINEGAR_VARS  
	#define SHAKENUM 
 */

/* derived parameters */
#define VARS (OIL_VARS+VINEGAR_VARS)

#if OIL_VARS > 64
	#define bitcontainer __m128i
	#define empty _mm_setzero_si128()
#else 
	#define bitcontainer uint64_t
	#define empty ((uint64_t) 0)
#endif

// The number of coefficients per polynomial of the Public map P
// that is not generated but stored in the public key
#define PRIVATE_SEED_BYTES ( 32 )
#define PUBLIC_SEED_BYTES ( 32 )

#define COL_BYTES ((OIL_VARS+63)/64*8)
#define COL_PRG_BYTES ((OIL_VARS+7)/8)

#define Q1_COLS ((VINEGAR_VARS+1)*(VINEGAR_VARS+2)/2 + OIL_VARS*(VINEGAR_VARS+1))
#define Q2_COLS (OIL_VARS*(OIL_VARS + 1) / 2)

#define T_BYTES  (COL_BYTES*(VINEGAR_VARS+1))
#define Q1_BYTES (COL_BYTES*Q1_COLS)
#define Q2_BYTES (COL_BYTES*Q2_COLS)

#define SECRET_KEY_BYTES PRIVATE_SEED_BYTES
#define BIG_SECRET_KEY_BYTES ( PRIVATE_SEED_BYTES + COL_BYTES*(VINEGAR_VARS+1) + Q1_BYTES )

#define PUBLIC_KEY_BYTES ( PUBLIC_SEED_BYTES + (((Q2_COLS*OIL_VARS)+7)/8))
#define BIG_PUBLIC_KEY_BYTES (Q1_BYTES+Q2_BYTES)  

#define OIL_BLOCKS (OIL_VARS/32 +1)
#define VIN_BLOCKS (VINEGAR_VARS/32+1)
#define VAR_BLOCKS (VARS/32 +1)

#ifdef MESSAGE_RECOVERY
	#define FIRST_PART_TARGET (SHAKENUM/4)
	#define SECOND_PART_TARGET (OIL_VARS-FIRST_PART_TARGET)
	#define RECOVERED_PART_MESSAGE (SECOND_PART_TARGET-1)
#else
	#define FIRST_PART_TARGET OIL_VARS
	#define SECOND_PART_TARGET 0
	#define RECOVERED_PART_MESSAGE 0
#endif

// Defines the appropriate Keccak_HashInitialize function based on the value of SHAKENUM
#define _Keccak_HashInitialize_SHAKE(NUM) Keccak_HashInitialize_SHAKE##NUM
#define __Keccak_HashInitialize_SHAKE(NUM) _Keccak_HashInitialize_SHAKE(NUM)
#define Keccak_HashInitialize_SHAKE __Keccak_HashInitialize_SHAKE(SHAKENUM)

#endif
