/*
 * encrypt.c
 *
 *  Created on: 22 Feb 2019
 *      Author: mrz
 */

#include "crypto_aead.h"
#include "led.h"
#include "cilipadi.h"
#include "api.h"
#include <stdio.h>
#include <stdlib.h> // malloc() and free()

/*
 * pad the message and AD if necessary
 */
int pad(
		const unsigned char *m,
		unsigned long long mlen,
		const unsigned char *ad,
		unsigned long long adlen,
		unsigned char *mx,
		unsigned long long mxlen,
		unsigned char *adx,
		unsigned long long adxlen) {
	int i;

	// original message
	for (i = 0; i < mlen; ++i) {
		mx[i] = m[i];
	}

	// pad only if length is not a multiple of r bits
	if (mxlen > mlen) {
		// bit 1 followed by zeros
		mx[mlen] = 0x80;

		for (i = mlen+1; i < mxlen; ++i) {
			mx[i] = 0;
		}
	}

	// original AD
	for (i = 0; i < adlen; ++i) {
		adx[i] = ad[i];
	}

	// pad only if length is not a multiple of r bits
	if (adxlen > adlen) {
		// bit 1 followed by zeros
		adx[adlen] = 0x80;

		for (i = adlen+1; i < adxlen; ++i) {
			adx[i] = 0;
		}
	}

	return 0;
}


/*
 * Check the length of the message and AD whether they need padding
 */
int padding_len_check(
		unsigned long long mlen,
		unsigned long long adlen,
		unsigned long long *mxlen,
		unsigned long long *adxlen) {
	unsigned long long x;

	// check padding on the plaintext
	x = mlen % BYTERATE;

	// if mlen is already a multiple of r bits, i.e. x=0, then set x = r to offset BYTERATE in the equation below
	if (x==0) x = BYTERATE;

	*mxlen = mlen + BYTERATE - x;

	// check padding on the AD
	x = adlen % BYTERATE;

	// if length is already a multiple of r bits, i.e. x=0, then set x = r to offset BYTERATE in the equation below
	if (x==0) x = BYTERATE;

	*adxlen = adlen + BYTERATE - x;

	return 0;
}

/*
 * Initializataion Phase
 */
int init_phase(unsigned char *state, const unsigned char *npub, const unsigned char *k) {
	int i;

	// fill in the key
	for (i=0; i<CRYPTO_KEYBYTES; i++) {
		state[i] = k[i];
	}

	// fill in the nonce
	for (i=CRYPTO_KEYBYTES; i<STATELEN; i++) {
		state[i] = npub[i-CRYPTO_KEYBYTES];
	}

	//printf("initial state = \n");
	//for (i=0; i<STATELEN; i++) printf("%02x", state[i]); printf("\n");

	permutation_384(state, AROUNDS);

	return 0;
}

int ad_phase(unsigned char *state, unsigned char *state_r, const unsigned char *ad, unsigned long long adlen) {
	int i, j, s_adlen;

	s_adlen = adlen / BYTERATE;

#ifdef DEBUG
	printf("  state = ");
	for (j=0; j<STATELEN; j++) printf("%02x", state[j]); printf("\n");
#endif

	for (i = 0; i < s_adlen; ++i) {
		for (j = 0; j < BYTERATE; ++j) {
			state_r[j] = ad[i*BYTERATE+j];
		}

		// XOR with AD
		xor_bytes(state, state_r, BYTERATE);

#ifdef DEBUG
		printf("  AD_{%d} = ", i+1);
		for (j=0; j<BYTERATE; j++) printf("%02x", ad[i*BYTERATE+j]); printf("\n");
		printf("  after XOR with AD = ");
		for (j=0; j<STATELEN; j++) printf("%02x", state[j]); printf("\n");
#endif

		permutation_384(state, BROUNDS);
	}

	// XOR the last bit of the state with '1' to indicate completion of AD phase
	state[STATELEN-1] ^= 1;

#ifdef DEBUG
		printf("  end of AD Phase = ");
		for (j=0; j<STATELEN; j++) printf("%02x", state[j]); printf("\n");
#endif
	return 0;
}

/*
 * enc = { 0, 1 }. 0 = decrypt, 1 = encrypt
 */
int ciphering_phase(unsigned char *state,
		unsigned char *state_r,
		const unsigned char *in,
		unsigned long long inlen,
		unsigned long long unpadded_inlen,
		unsigned char *out,
		int enc) {
	int i, j;
	int t_inlen;

	// for decryption, t_inlen has to be deducted by one when processing the ciphertext to not count the tag
	t_inlen = inlen/BYTERATE;
	//if (enc==0) t_inlen--;

	//printf("t_mlen = %d\n", t_inlen);

	// allocate array for ciphertext
	//c = malloc((size_t)(mlen + taglen)); // ciphertext + tag

	// encryption
	if (enc) {
		for (i = 0; i < t_inlen; ++i) {

#ifdef DEBUG
			printf("  M%2d: ", i+1);
			// if this is the last message, print only the actual message
			if (i == (t_inlen-1)) {
				// it is the last message and the message is already in multiple of r bits
				if ((unpadded_inlen % BYTERATE)==0) {
					for (j = 0; j < BYTERATE; ++j) printf("%02x", in[i*BYTERATE+j]);
					printf("\n");
				}
				else {
					for (j = 0; j < (unpadded_inlen % BYTERATE); ++j) printf("%02x", in[i*BYTERATE+j]);
					printf("\n");
				}
			}
			else {
				for (j = 0; j < BYTERATE; ++j) printf("%02x", in[i*BYTERATE+j]);
				printf("\n");
			}
			printf("  state (before XOR with M%2d= ", i+1); for (j = 0; j < STATELEN; ++j) printf("%02x", state[j]);
			printf("\n");
#endif
			for (j = 0; j < BYTERATE; ++j) {
				state_r[j] = in[i*BYTERATE+j];
			}

			// XOR message with bitrate part of the state
			xor_bytes(state, state_r, BYTERATE);

			// output ciphertext
			// if this is the last message, only use unpadded one
			if (i == (t_inlen-1)) {
				if ((unpadded_inlen % BYTERATE)==0) {
					for (j = 0; j < BYTERATE; ++j) {
						out[i*BYTERATE+j] = state[j];
					}
				}
				else {
					for (j = 0; j < (unpadded_inlen % BYTERATE); ++j) {
						out[i*BYTERATE+j] = state[j];
					}
				}
			}
			else {
				for (j = 0; j < BYTERATE; ++j) {
					out[i*BYTERATE+j] = state[j];
				}
			}

#ifdef DEBUG
			printf("  state (after XOR with M%2d= ", i+1); for (j = 0; j < STATELEN; ++j) printf("%02x", state[j]);
			printf("\n");
			printf("  C%2d: ", i+1);
			// if this is the last message, print only the actual message
			if (i == (t_inlen-1)) {
				// it is the last message and the message is already in multiple of r bits
				if ((unpadded_inlen % BYTERATE)==0) {
					for (j = 0; j < BYTERATE; ++j) printf("%02x", out[i*BYTERATE+j]);
					printf("\n");
				}
				else {
					for (j = 0; j < (unpadded_inlen % BYTERATE); ++j) printf("%02x", out[i*BYTERATE+j]);
					printf("\n");
				}
			}
			else {
				for (j = 0; j < BYTERATE; ++j) printf("%02x", out[i*BYTERATE+j]);
				printf("\n");
			}
#endif

			//printf("state (after XOR with message %2d) \n", i+1);
			//for (j = 0; j < STATELEN; ++j) printf("%02x", state[j]); printf("\n");

			if ((i+1) < t_inlen) {
				permutation_384(state, BROUNDS);
			}
		}
	}
	// decryption
	else {
		for (i = 0; i <t_inlen; ++i) {

#ifdef DEBUG
			printf("  C%2d: ", i+1);
			// if this is the last message, print only the actual message
			if (i == (t_inlen-1)) {
				if ((unpadded_inlen % BYTERATE)==0) {
					for (j = 0; j < BYTERATE; ++j) printf("%02x", in[i*BYTERATE+j]);
					printf("\n");
				}
				else {
					for (j = 0; j < (unpadded_inlen % BYTERATE); ++j) printf("%02x", in[i*BYTERATE+j]);
					printf("\n");
				}
			}
			else {
				for (j = 0; j < BYTERATE; ++j) printf("%02x", in[i*BYTERATE+j]);
				printf("\n");
			}
#endif
			// XOR ciphertext with bitrate part of the state to obtain message
			// if this is the last message, only use unpadded one
			if (i == (t_inlen-1)) {
				if ((unpadded_inlen % BYTERATE)==0) {
					for (j = 0; j < BYTERATE; ++j) {
						out[i*BYTERATE+j] = in[i*BYTERATE+j] ^ state[j];
					}
				}
				else {
					for (j = 0; j < (unpadded_inlen % BYTERATE); ++j) {
						out[i*BYTERATE+j] = in[i*BYTERATE+j] ^ state[j];
					}
				}
			}
			else {
				for (j = 0; j < BYTERATE; ++j) {
					out[i*BYTERATE+j] = in[i*BYTERATE+j] ^ state[j];
				}
			}

#ifdef DEBUG
			printf("  M%2d: ", i+1);
			// if this is the last message, print only the actual message
			if (i == (t_inlen-1)) {
				if ((unpadded_inlen % BYTERATE)==0) {
					for (j = 0; j < BYTERATE; ++j) printf("%02x", out[i*BYTERATE+j]);
					printf("\n");
				}
				else {
					for (j = 0; j < (unpadded_inlen % BYTERATE); ++j) printf("%02x", out[i*BYTERATE+j]);
					printf("\n");
				}
			}
			else {
				for (j = 0; j < BYTERATE; ++j) printf("%02x", out[i*BYTERATE+j]);
				printf("\n");
			}

			printf("  state (before replace with ciphertext = ");
			for (j = 0; j < STATELEN; ++j) printf("%02x", state[j]);
			printf("\n");
#endif

			// replace bitrate part of the state with the current ciphertext block
			// if this is the last message, we need to handle it differently
			if (i == (t_inlen-1)) {
				if ((unpadded_inlen % BYTERATE)==0) {
					for (j = 0; j < BYTERATE; ++j) {
						state[j] = in[i*BYTERATE+j];
					}
				}
				else {
					for (j = 0; j < (unpadded_inlen % BYTERATE); ++j) {
						state[j] = in[i*BYTERATE+j];
					}
					// XOR state with the padded value (0x80)
					state[unpadded_inlen % BYTERATE] ^= in[i*BYTERATE+(unpadded_inlen % BYTERATE)];
				}
			}
			else {
				for (j = 0; j < BYTERATE; ++j) {
					state[j] = in[i*BYTERATE+j];
				}
			}
#ifdef DEBUG
			printf("  state (after replace with ciphertext = ");
			for (j = 0; j < STATELEN; ++j) printf("%02x", state[j]);
			printf("\n");
#endif

			//printf("state (after XOR with ciphertext %2d) \n", i+1);
			//for (j = 0; j < STATELEN; ++j) printf("%02x", state[j]); printf("\n");

			if ((i+1) < t_inlen) {
				permutation_384(state, BROUNDS);
			}
		}
	}

	//printf("state (after permutation) \n");
	//for (i = 0; i < STATELEN; ++i) printf("%02x", state[i]); printf("\n");

	/*
	if (enc) {
		for (i = 0; i < BYTERATE; ++i) {
			state_r[i] = in[(t_inlen-1)*BYTERATE+i];
		}

		// XOR message with bitrate part of the state
		xor_bytes(state, state_r, BYTERATE);

		// output ciphertext
		for (i = 0; i < BYTERATE; ++i) {
			out[inlen-CRYPTO_ABYTES+i] = state[i];
		}
	}
	else {

		// output message
		for (j = 0; j < BYTERATE; ++j) {
			out[inlen-BYTERATE+j] = in[(t_inlen-1)*BYTERATE+j] ^ state[j];
		}

		// replace bitrate part of the state with the current ciphertext block
		for (j = 0; j < BYTERATE; ++j) {
			state[j] = in[(t_inlen-1)*BYTERATE+j];
		}
	}
	*/

	//printf("state (after XOR with last input) \n");
	//for (i = 0; i < STATELEN; ++i) printf("%02x", state[i]); printf("\n");

	return 0;
}

/*
 * Finalization Phase
 */
int finalization_phase(unsigned char *state, const unsigned char *k) {
	//int i;

#ifdef DEBUG
	int j;
	printf("  state = ");
	for (j=0; j<STATELEN; j++) printf("%02x", state[j]); printf("\n");
#endif

	permutation_384(state, AROUNDS);

	//printf("state (after applying p^a_n) \n");
	//for (i = 0; i < STATELEN; ++i) printf("%02x", state[i]); printf("\n");

	// XOR with key
	xor_bytes(state, k, CRYPTO_KEYBYTES);

	//printf("state (after XOR with key) = \n");
	//for (i = 0; i < STATELEN; ++i) printf("%02x", state[i]); printf("\n");
	return 0;
}

/*
 * the code for the AEAD implementation goes here,
 *
 * generating a ciphertext c[0],c[1],...,c[*clen-1]
 * from a plaintext m[0],m[1],...,m[mlen-1]
 * and associated data ad[0],ad[1],...,ad[adlen-1]
 * and nonce npub[0],npub[1],...
 * and secret key k[0],k[1],...
 * the implementation shall not use nsec
 *
 */
int crypto_aead_encrypt(
	unsigned char *c,
	unsigned long long *clen,
	const unsigned char *m,
	unsigned long long mlen,
	const unsigned char *ad,
	unsigned long long adlen,
	const unsigned char *nsec,
	const unsigned char *npub,
	const unsigned char *k) {

	unsigned char state[STATELEN]; // state
	unsigned char state_r[BYTERATE]; // rate part of the state
	unsigned char *mx; // padded message
	unsigned long long mxlen = 0; // length of padded message
	unsigned char *adx; // padded AD
	unsigned long long adxlen = 0; // length of padded AD
	int i;

	/*
	 * Initialization
	 */
#ifdef DEBUG
	printf("-- INIT PHASE --\n");
#endif
	init_phase(state, npub, k);

	/*
	 * Padding check
	 */
	//printf("mlen = %llu, adlen = %llu, ", mlen, adlen);
	padding_len_check(mlen, adlen, &mxlen, &adxlen);
	//printf("mxlen = %llu, adxlen = %llu\n", mxlen, adxlen);

	// initialize padded message and AD
	mx = malloc((size_t)(mxlen));
	adx = malloc((size_t)(adxlen));

	pad(m, mlen, ad, adlen, mx, mxlen, adx, adxlen);

#ifdef DEBUG
	printf("  Padding Check\n");
	printf("   M = ");
	for (i = 0; i < mlen; ++i) printf("%02x", m[i]);
	printf("\n");

	printf("   M (padded) = ");
	for (i = 0; i < mxlen; ++i) printf("%02x", mx[i]);
	printf("\n");
#endif


	/*
	 * Processing the associated data
	 */
#ifdef DEBUG
	printf("-- AD PHASE --\n");
#endif
	//printf("state (before AD phase) = \n");
	//for (i=0; i<STATELEN; i++) printf("%02x", state[i]); printf("\n");

	ad_phase(state, state_r, adx, adxlen);

	//printf("state (after AD phase) = \n");
	//for (i=0; i<STATELEN; i++) printf("%02x", state[i]); printf("\n");


	/*
	 * Processing the plaintext
	 */
#ifdef DEBUG
	printf("-- MESSAGE ENCRYPTION PHASE --\n");
#endif
	ciphering_phase(state, state_r, mx, mxlen, mlen, c, 1);

	/*
	 * Finalization Phase
	 */
#ifdef DEBUG
	printf("-- FINALIZATION PHASE --\n");
#endif
	finalization_phase(state, k);

	// output the tag
	*clen = mlen + CRYPTO_ABYTES;
	for (i = 0; i < CRYPTO_ABYTES; ++i) {
		c[*clen-CRYPTO_ABYTES+i] = state[i];
	}

	free(mx);
	free(adx);

	return 0;
}

/*
 * the code for the AEAD implementation goes here,
 *
... generating a plaintext m[0],m[1],...,m[*mlen-1]
... and secret message number nsec[0],nsec[1],...
... from a ciphertext c[0],c[1],...,c[clen-1]
... and associated data ad[0],ad[1],...,ad[adlen-1]
... and nonce number npub[0],npub[1],...
... and secret key k[0],k[1],... ...
 */

int crypto_aead_decrypt(
	unsigned char *m,
	unsigned long long *mlen,
	unsigned char *nsec,
	const unsigned char *c,
	unsigned long long clen,
	const unsigned char *ad,
	unsigned long long adlen,
	const unsigned char *npub,
	const unsigned char *k) {

	unsigned char state[STATELEN]; // 16-byte state
	unsigned char state_r[BYTERATE]; // 8-byte rate part of the state
	unsigned char *cx; // padded message (+ tag)
	unsigned long long cxlen = 0; // length of padded message + tag
	unsigned long long c_onlylen = clen - CRYPTO_ABYTES; // length of ciphertext without tag
	unsigned char *adx; // padded AD
	unsigned long long adxlen = 0; // length of padded AD
	int i;
	unsigned char tag[CRYPTO_ABYTES]; // computed tag

	/*
	 * Initialization
	 */
#ifdef DEBUG
	int j;
	printf("-- INIT PHASE --\n");
#endif
	init_phase(state, npub, k);

	/*
	 * Padding check
	 */
	padding_len_check(c_onlylen, adlen, &cxlen, &adxlen);

	// initialize padded message and AD
	cx = malloc((size_t)(cxlen));
	adx = malloc((size_t)(adxlen));

	pad(c, c_onlylen, ad, adlen, cx, cxlen, adx, adxlen);

#ifdef DEBUG
	printf("  C = ");
	for (i = 0; i < c_onlylen; ++i) printf("%02x", c[i]);
	printf("\n");

	printf("  C (padded) = ");
	for (i = 0; i < cxlen; ++i) printf("%02x", cx[i]);
	printf("\n");
#endif

	/*
	 * Processing the associated data
	 */

#ifdef DEBUG
	printf("-- AD PHASE --\n");
#endif
	//printf("state (before AD phase) = \n");
	//for (i=0; i<STATELEN; i++) printf("%02x", state[i]); printf("\n");

	ad_phase(state, state_r, adx, adxlen);

	//printf("state (after AD phase) = \n");
	//for (i=0; i<STATELEN; i++) printf("%02x", state[i]); printf("\n");


	/*
	 * Processing the ciphertext
	 */
#ifdef DEBUG
	printf("-- MESSAGE DECRYPTION PHASE --\n");
#endif
	ciphering_phase(state, state_r, cx, cxlen, c_onlylen, m, 0);

	/*
	 * Finalization Phase
	 */
#ifdef DEBUG
	printf("-- FINALIZATION PHASE --\n");
#endif
	finalization_phase(state, k);

	// output the tag
	*mlen = clen - CRYPTO_ABYTES;
#ifdef DEBUG
	printf("\nComputed Tag =\n");
#endif
	for (i = 0; i < CRYPTO_ABYTES; ++i) {
		tag[i] = state[i];
#ifdef DEBUG
		printf("%02x", tag[i]);
#endif
	}

#ifdef DEBUG
	printf("\n");
#endif

	// compare computed tag with the one received
	for (i = 0; i < CRYPTO_ABYTES; ++i) {
		if (tag[i] != c[clen-CRYPTO_ABYTES+i]) {

#ifdef DEBUG
			printf("Ciphertext not authenticated!\n");
			printf("mlen: %llu,  clen: %llu, adlen: %llu\n", *mlen, clen, adlen);
			printf("cxlen: %llu, adxlen: %llu\n", cxlen, adxlen);
			printf("Message: "); for (j=0; j<*mlen; j++) printf("%02x", m[j]); printf("\n");
			printf("AD: "); for (j=0; j<adlen; j++) printf("%02x", ad[j]); printf("\n");
			printf("Key: "); for (j=0; j<CRYPTO_KEYBYTES; j++) printf("%02x", k[j]); printf("\n");
			printf("Ciphertext: "); for (j=0; j<c_onlylen; j++) printf("%02x", c[j]); printf("\n");
			printf("Tag in ciphertext: ");
			for (j = 0; j < CRYPTO_ABYTES; ++j) {
				printf("%02x", c[clen-CRYPTO_ABYTES]);
			}
			printf("\n");
#endif
			return -1;
		}
	}

	free(cx);
	free(adx);

	return 0;
}
