/**
 * @file code.c
 * @brief Implementation of tensor code
 */

#include "bch.h"
#include "code.h"
#include "gen_matrix.h"
#include "parameters.h"
#include "repetition.h"
#include <stdint.h>
#include <string.h>
#include <immintrin.h>
#include <x86intrin.h>
#ifdef VERBOSE
#include <stdio.h>
#include "vector.h"
#endif

static inline uint64_t mux(uint64_t a, uint64_t b, int64_t bit);

static inline uint64_t mux(uint64_t a, uint64_t b, int64_t bit) {
    uint64_t ret = a ^ b;
    return (ret & (-bit >> 63)) ^ a;
}



/**
 *
 * @brief Encoding the message m to a code word em using the tensor code
 *
 * We encode the message using the BCH code. For each bit obtained,
 * we duplicate the bit PARAM_N2 times to apply repetition code.
 * BCH encoding is done using the classical mG operation,
 * columns of the matrix are stored in 256-bit registers
 *
 * @param[out] em Pointer to an array that is the tensor code word
 * @param[in] m Pointer to an array that is the message
 */
void code_encode(uint64_t *em, const uint64_t *m) {
	uint64_t res;
	uint32_t i;
    static const uint64_t mask[2][2] = {{0x0UL,0x0UL}, {0x7FFFFFFFUL,0x3FFFFFFFUL}};

	#ifdef VERBOSE
		uint64_t tmp[VEC_N1_SIZE_64] = {0};
		uint64_t cpt_blocs = 0;
	#endif

	__m256i *colonne, y, aux0;
	__m256i msg = _mm256_lddqu_si256((const __m256i*) m);
	colonne = ((__m256i*) gen_matrix);

	for (i = 0 ; i < PARAM_N1 - PARAM_K ; i++) {
		// y is the and operation between m and ith column of G
		y = _mm256_and_si256(colonne[i], msg);
		// aux0 = (y2 y3 y0 y1)
		aux0 = _mm256_permute2x128_si256(y, y, 1);
		// y = (y0^y2 y1^y3 y2^y0 y3^y1)
		y = _mm256_xor_si256(y, aux0);
		// aux0 = (y1^y3 y0^y2 y1^y3 y0^y2)
		aux0 = _mm256_shuffle_epi32(y, 0x4e);
		// y = (y0^y1^y2^y3 repeated 4 times)
		y = _mm256_xor_si256(aux0, y);
		res = _popcnt64(_mm256_extract_epi64(y, 0)) & 1;

		#ifdef VERBOSE
			tmp[i / 64] |= (res << (cpt_blocs % 64));
			cpt_blocs = cpt_blocs + 1;
		#endif

		uint16_t pos_r = PARAM_N2 * i;
		uint16_t idx_r = (pos_r & 0x3f);
		uint64_t *p64 = em;
		p64 += pos_r >> 6;
		uint64_t select = mux(mask[0][0],mask[1][0],res);
		*p64 ^= select << idx_r;
		select = mux(mask[0][1],mask[1][1],res);
		*(p64 + 1) ^= select >> ((63 - idx_r));
	}

	/* now we add the message m */
	/* systematic encoding */
	for (int32_t i = 0 ; i < 4 ; i++) {
		for (int32_t j = 0 ; j < 64 ; j++) {
			uint8_t bit = (m[i] >> j) & 0x1;
			uint32_t pos_r = PARAM_N2 * ((PARAM_N1 - PARAM_K) + ((i << 6) + j));
			uint16_t idx_r = (pos_r & 0x3f);
			uint64_t *p64 = em;

			#ifdef VERBOSE
				tmp[cpt_blocs / 64] |= (((uint64_t) bit) << (cpt_blocs % 64));
				cpt_blocs = cpt_blocs + 1;
			#endif

			p64 += pos_r >> 6;
			uint64_t select = mux(mask[0][0],mask[1][0],bit);
			*p64 ^= select << idx_r;
			select = mux(mask[0][1],mask[1][1],bit);
			*(p64 + 1) ^= select >> ((63 - idx_r));
		}
	}

	#ifdef VERBOSE
		printf("\n\nBCH code word: "); vect_print(tmp, VEC_N1_SIZE_BYTES);
		printf("\n\nTensor code word: "); vect_print(em, VEC_N1N2_SIZE_BYTES);
	#endif
}


/**
 * @brief Decoding the code word em to a message m using the tensor code
 *
 * @param[out] m Pointer to an array that is the message
 * @param[in] em Pointer to an array that is the code word
 */
void code_decode(uint64_t *m, const uint64_t *em) {

	uint64_t tmp[VEC_N1_SIZE_64] = {0};

	repetition_code_decode(tmp, em);
	bch_code_decode(m, tmp);

	#ifdef VERBOSE
		printf("\n\nRepetition decoding result (the input for the BCH decoding algorithm): "); vect_print(tmp, VEC_N1_SIZE_BYTES);
	#endif
}
