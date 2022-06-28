#include "keccakrng.h"

/* 
	Initializes a Sponge object, absorbs a seed and finalizes the absorbing phase

	 sponge  : The sponge object
	 seed    : The seed to absorb
	 len     : The length of the seed
*/
void initializeAndAbsorb(Sponge *sponge ,const unsigned char * seed , int len ) {
	Keccak_HashInitialize_SHAKE(sponge);
	Keccak_HashUpdate(sponge, seed, len*8 );
	Keccak_HashFinal(sponge, 0 );
}

/* 
	Squeezes a uint64_t from the sponge object

	sponge : The sponge object
	bytes  : The number of bytes to squeeze from the sponge (should be between 1 and 8)
*/
void squeezeuint64_t(Sponge *sponge, int bytes, uint64_t *a){
	Keccak_HashSqueeze(sponge,(unsigned char *) a, bytes*8);
}

/* 
	Squeeze a list of Field elements from the sponge

	sponge : The sponge object
	vector : receives the list of field elements
	length : The length of the list of elements
*/
void squeezeVector(Sponge *sponge, FELT *vector, int length) {
	// Squeeze the appropriate number of bytes from the sponge
	Keccak_HashSqueeze(sponge ,vector , length*8);
}

void calculateQ1(const unsigned char *seed, bitcontainer *Q1){
	Sponge sponge;
	initializeAndAbsorb(&sponge , seed , 32);
	squeezeCols(&sponge,Q1,Q1_COLS);
}

#if OIL_VARS == 63

/*
	Generates an array of bitcontainers

	sponge : pointer to a Sponge object
	arr    : the array that will receive the generated bitcontainers
	size   : the number of bitcontainers that is generated
*/
void squeezeCols(Sponge *sponge, bitcontainer *arr, int size) {
	Keccak_HashSqueeze(sponge,(void *)arr, size*64);
}

#elif (OIL_VARS == 90) || (OIL_VARS == 117)

void squeezeCols(Sponge *sponge, bitcontainer *arr, int size) {
	uint8_t *buffer = malloc(COL_PRG_BYTES*size);
	Keccak_HashSqueeze(sponge,(void *) buffer,8*COL_PRG_BYTES*size);
	int i;
	for(i=0 ; i<size ; i++){
		arr[i] = _mm_loadu_si128((__m128i *)(buffer + i*COL_PRG_BYTES));
	}
	free(buffer);
}

#endif

static inline uint64_t rotl(const uint64_t x, int k) {
	return (x << k) | (x >> (64 - k));
}
