#include <stdint.h>

#define BLOCKBYTES  	16
#define TWEAKEYBYTES 	16

/**
 * Skinny-128-384+ simple (i.e. without operating mode) encryption function.
 * The tweakey schedule is computed on-the-fly. 
 */
void skinny128_384_plus(
	uint8_t out[BLOCKBYTES], const uint8_t in[BLOCKBYTES],
	const uint8_t tk1[TWEAKEYBYTES],
	const uint8_t tk2[TWEAKEYBYTES],
	const uint8_t tk3[TWEAKEYBYTES]);
