#include <stdint.h>

#define BLOCKBYTES 				16
#define TWEAKEYBYTES 			16
#define SKINNY128_384_ROUNDS	40

/**
 * Skinny-128-384+ simple (i.e. without operating mode) encryption function.
 * The tweakey schedule for TK1 is computed on-the-fly while it is assumed to
 * be precomputed for TK2 and TK3.
 */
void skinny128_384_plus(
	uint8_t in[BLOCKBYTES], const uint8_t out[BLOCKBYTES],
	const uint8_t tk1[TWEAKEYBYTES],
	const uint8_t rtk_23[SKINNY128_384_ROUNDS*BLOCKBYTES/2]);

/**
 * Precomputation of round tweakeys for TK2 and TK3 (also include a part of the
 * round constants).
 */			
void tk_schedule_23(
	uint8_t rtk_23[SKINNY128_384_ROUNDS*BLOCKBYTES/2],
	const uint8_t tk2[TWEAKEYBYTES],
	const uint8_t tk3[TWEAKEYBYTES]);

