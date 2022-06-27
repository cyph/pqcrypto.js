/*
 * cilipadi.c
 *
 *  Created on: 25 Feb 2019
 *      Author: mrz
 */

#include <stdio.h>
#include "cilipadi.h"
#include "led.h"
#include "crypto_aead.h"
#include <string.h> // for memcpy
#include <stdlib.h> // for malloc(), free()
#include "api.h"

int xor_bytes(unsigned char *x, const unsigned char *y, int len) {
	int i;

	for (i = 0; i < len; ++i) {
		x[i]^=y[i];
	}

	return 0;
}


int permutation_256(unsigned char *state, int rounds) {
	unsigned char x1[8];
	unsigned char x2[8];
	unsigned char x3[8];
	unsigned char x4[8];
	unsigned char temp[8];
	int i;
#ifdef DEBUG
	int j;
#endif

	// divide the input into 4 branches
	for (i = 0; i < 8; ++i) {
		x1[i] = state[i];
		x2[i] = state[i+8];
		x3[i] = state[i+16];
		x4[i] = state[i+24];
	}

	for (i = 0; i < rounds; ++i) {

#ifdef DEBUG
		printf("\n  state (input  to round %d): ", i+1);

		for (j=0; j<8; j++) printf("%02x", x1[j]); printf(" ");
		for (j=0; j<8; j++) printf("%02x", x2[j]); printf(" ");
		for (j=0; j<8; j++) printf("%02x", x3[j]); printf(" ");
		for (j=0; j<8; j++) printf("%02x", x4[j]); printf("\n");
#endif

		memcpy(temp, x1, 8);
		f_function(temp, 1, i);
		xor_bytes(x2, temp, 8);

		memcpy(temp, x3, 8);
		f_function(temp, 2, i);
		xor_bytes(x4, temp, 8);

		// shuffle
		memcpy(temp, x1, 8);
		memcpy(x1, x2, 8); // x2 -> x1
		memcpy(x2, x3, 8); // x3 -> x2
		memcpy(x3, x4, 8); // x4 -> x3
		memcpy(x4, temp, 8); // temp -> x4


#ifdef DEBUG
		printf("  state (output of round %d): ", i+1);

		for (j=0; j<8; j++) printf("%02x", x1[j]); printf(" ");
		for (j=0; j<8; j++) printf("%02x", x2[j]); printf(" ");
		for (j=0; j<8; j++) printf("%02x", x3[j]); printf(" ");
		for (j=0; j<8; j++) printf("%02x", x4[j]); printf("\n");
#endif
	}

	// put value back to state
	for (i = 0; i < 8; ++i) {
		state[i   ] = x1[i];
		state[i+ 8] = x2[i];
		state[i+16] = x3[i];
		state[i+24] = x4[i];
	}

	//printf("state (output of permutation_a_n)\n");
	//for (j=0; j<32; j++) printf("%02x", state[j]); printf("\n");

	return 0;
}


int permutation_384(unsigned char *state, int rounds) {
	unsigned char x1[8];
	unsigned char x2[8];
	unsigned char x3[8];
	unsigned char x4[8];
	unsigned char x5[8];
	unsigned char x6[8];
	unsigned char temp[8];
	int i;

	// divide the input into 6 branches
	for (i = 0; i < 8; ++i) {
		x1[i] = state[i];
		x2[i] = state[i+8];
		x3[i] = state[i+16];
		x4[i] = state[i+24];
		x5[i] = state[i+32];
		x6[i] = state[i+40];
	}

	//printf("state (input to round 1) = \n");
	//for (i=0; i<32; i++) printf("%02x ", state[i]); printf("\n");

	for (i = 0; i < rounds; ++i) {
		/*
		printf("round %d\n", i);

		for (j=0; j<8; j++) printf("%02x", x1[j]); printf(" ");
		for (j=0; j<8; j++) printf("%02x", x2[j]); printf(" ");
		for (j=0; j<8; j++) printf("%02x", x3[j]); printf(" ");
		for (j=0; j<8; j++) printf("%02x", x4[j]); printf("\n");
		 */

		memcpy(temp, x1, 8);
		f_function(temp, 1, i);
		xor_bytes(x2, temp, 8);

		memcpy(temp, x3, 8);
		f_function(temp, 2, i);
		xor_bytes(x4, temp, 8);

		memcpy(temp, x5, 8);
		f_function(temp, 3, i);
		xor_bytes(x6, temp, 8);


		// shuffle
		memcpy(temp, x1, 8);
		memcpy(x1, x2, 8); // x2 -> x1
		memcpy(x2, x3, 8); // x3 -> x2
		memcpy(x3, x6, 8); // x6 -> x3
		memcpy(x6, x5, 8); // x5 -> x6
		memcpy(x5, x4, 8); // x4 -> x5
		memcpy(x4, temp, 8); // temp -> x4

		/*
		printf("-\n");
		for (j=0; j<8; j++) printf("%02x", x1[j]); printf(" ");
		for (j=0; j<8; j++) printf("%02x", x2[j]); printf(" ");
		for (j=0; j<8; j++) printf("%02x", x3[j]); printf(" ");
		for (j=0; j<8; j++) printf("%02x", x4[j]); printf("\n");
		*/
	}

	// put value back to state
	for (i = 0; i < 8; ++i) {
		state[i   ] = x1[i];
		state[i+ 8] = x2[i];
		state[i+16] = x3[i];
		state[i+24] = x4[i];
		state[i+32] = x5[i];
		state[i+40] = x6[i];
	}

	//printf("state (output of permutation_a_n)\n");
	//for (j=0; j<32; j++) printf("%02x", state[j]); printf("\n");

	return 0;
}

int f_function(unsigned char *x, int l, int pround) {
	unsigned char led_state[4][4];
	int i, j, k, rounds=2;
	const unsigned char RC[48] = {
		0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3E, 0x3D, 0x3B, 0x37, 0x2F,
		0x1E, 0x3C, 0x39, 0x33, 0x27, 0x0E, 0x1D, 0x3A, 0x35, 0x2B,
		0x16, 0x2C, 0x18, 0x30, 0x21, 0x02, 0x05, 0x0B, 0x17, 0x2E,
		0x1C, 0x38, 0x31, 0x23, 0x06, 0x0D, 0x1B, 0x36, 0x2D, 0x1A,
		0x34, 0x29, 0x12, 0x24, 0x08, 0x11, 0x22, 0x04
	};
	unsigned RC_state[4][4] = {
		{ 0, 0, 0, 0 },
		{ 0, 0, 0, 0 },
		{ 2, 0, 0, 0 },
		{ 3, 0, 0, 0 }
	};

#ifdef DEBUG
	printf("    -- F%2d --\n", l);
	printf("    input: ");
	for (i=0; i<8; i++) printf("%02x", x[i]); printf("\n");
#endif
	// decompose input into LED 4x4 state bytes
	for (i = 0; i < 16; ++i) {
		if(i%2) led_state[i/4][i%4] = x[i>>1]&0xF;
		else led_state[i/4][i%4] = (x[i>>1]>>4)&0xF;
	}

	for (i = 0; i < rounds; ++i) {

#ifdef DEBUG
		printf("    LED round %d\n", i+1);
		printf("    input:\n");
		for (j=0; j<4; j++) {
			printf("    ");
			for (k=0; k<4; k++) {
				printf("%x ", led_state[j][k]);
			}
			printf("\n");
		}
#endif
		// note that the implemented LED is v2 which require the XOR of the key length (i.e. 64 or 128 bits)
		// to the first column of the state.
		// We do not require this and hence, we have modified LED's source code so that we only use round constants for round 1 LED
		//AddConstantsCiliPadi(led_state, i, l);

		RC_state[0][0] ^= ((l>>2) & 0x3);
		RC_state[1][0] ^= ( l     & 0x3);

		unsigned char tmp = (RC[pround] >> 3) & 7;

		RC_state[0][1] ^= tmp;
		RC_state[2][1] ^= tmp;
		tmp =  RC[pround] & 7;
		RC_state[1][1] ^= tmp;
		RC_state[3][1] ^= tmp;

		if (i > 0) {
			for (j=0; j<4; j++) for (k=0; k<4; k++) RC_state[j][k] = 0;
		}

		// AddConstants CiliPadi
		for (j = 0; j < 4; ++j) {
			for (k = 0; k < 2; ++k) {
				led_state[j][k] ^= RC_state[j][k];
			}
		}

#ifdef DEBUG
		printf("    round constants:\n");
		for (j=0; j<4; j++) {
			printf("    ");
			for (k=0; k<4; k++) {
				printf("%x ", RC_state[j][k]);
			}
			printf("\n");
		}

		printf("    after AC:\n");
		for (j=0; j<4; j++) {
			printf("    ");
			for (k=0; k<4; k++) {
				printf("%x ", led_state[j][k]);
			}
			printf("\n");
		}
#endif
		SubCell(led_state);
#ifdef DEBUG
		printf("    after SC:\n");
		for (j=0; j<4; j++) {
			printf("    ");
			for (k=0; k<4; k++) {
				printf("%x ", led_state[j][k]);
			}
			printf("\n");
		}
#endif
		ShiftRow(led_state);
#ifdef DEBUG
		printf("    after SR:\n");
		for (j=0; j<4; j++) {
			printf("    ");
			for (k=0; k<4; k++) {
				printf("%x ", led_state[j][k]);
			}
			printf("\n");
		}
#endif
		MixColumn(led_state);
#ifdef DEBUG
		printf("    after MCS:\n");
		for (j=0; j<4; j++) {
			printf("    ");
			for (k=0; k<4; k++) {
				printf("%x ", led_state[j][k]);
			}
			printf("\n");
		}
#endif
	}

	// put back into x
	for (i = 0; i < 4; ++i) {
		for (j = 0; j < 2; ++j) {
			x[i*2+j]  = led_state[i][j*2  ] << 4;
			x[i*2+j] |= led_state[i][j*2+1];
		}
	}

#ifdef DEBUG
	printf("    output: ");
	for (i=0; i<8; ++i) printf("%02x", x[i]); printf("\n");
#endif

	return 0;
}

/*
int main() {
	//unsigned char c[32]; // 16-byte ciphertext + 16-byte tag
	unsigned char *c;
	unsigned long long *clen;

	const unsigned char m[32] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			  	  	  	  	  	  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			  	  	  	  	  	  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	// test vector value
	//const unsigned char  m[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	//						 	   0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };

	unsigned char *m_dec;
	unsigned long long mlen = BYTERATE;
	//unsigned long long mlen = 8;

	//const unsigned char ad[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	const unsigned char ad[16] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
							 	   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
	// test vector value
	//const unsigned char ad[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	//						 	   0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
	unsigned long long adlen = BYTERATE;

	const unsigned char npub[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
									 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	// test vector value
	//const unsigned char npub[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	//						 	     0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };

	const unsigned char k[32] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	// test vector value
	//const unsigned char k[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	//						 	  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };

	int i;

	//clen = &mlen;
	clen = malloc((size_t)(1));
	c = malloc((size_t)(mlen + CRYPTO_ABYTES));
	crypto_aead_encrypt(c, clen, m, mlen, ad, adlen, NULL, npub, k);


	printf("\nENCRYPTION\n");

	printf("\nPlaintext =");
	for (i = 0; i < mlen; ++i) {
		if ((i%BYTERATE) == 0)
			printf("\n");
		printf("%02x", m[i]);
	}

	printf("\nAD =");
	for (i = 0; i < adlen; ++i) {
		if ((i%BYTERATE) == 0)
			printf("\n");
		printf("%02x", ad[i]);
	}

	//printf("\nt_mlen = %2d\n", t_mlen);
	printf("\nCiphertext =");
	for (i = 0; i < (*clen - CRYPTO_ABYTES); ++i) {
		if ((i%BYTERATE) == 0)
			printf("\n");
		printf("%02x", c[i]);
	}

	printf("\nTag = \n");
	for (i = 0; i < CRYPTO_ABYTES; ++i) {
		printf("%02x", c[(*clen - CRYPTO_ABYTES)+i]);
	}


	printf("\n\nDECRYPTION\n");

	//printf("\nt_mlen = %2d\n", t_mlen);
	printf("\nCiphertext =");
	for (i = 0; i < (*clen - CRYPTO_ABYTES); ++i) {
		if ((i%BYTERATE) == 0)
			printf("\n");
		printf("%02x", c[i]);
	}

	// tamper
	//c[0] ^=1;

	m_dec = malloc((size_t)(*clen));
	if (crypto_aead_decrypt(m_dec, &mlen, NULL, c, *clen, ad, adlen, npub, k) == 0) {
		printf("\nPlaintext =");
		for (i = 0; i < mlen; ++i) {
			if ((i%BYTERATE) == 0)
				printf("\n");
			printf("%02x", m_dec[i]);
		}

		printf("\nAD =");
		for (i = 0; i < adlen; ++i) {
			if ((i%BYTERATE) == 0)
				printf("\n");
			printf("%02x", ad[i]);
		}
	}
	else {
		printf("Decryption failed\n");
	}


	free(clen);
	free(c);
	free(m_dec);

	return 0;
}
*/
