/**
 * encrypt.c
 *
 * @version 1.0 (March 2019)
 *
 * Reference ANSI C code for the Qarma tweakable block cipher
 *
 * @author Roberto Avanzi <roberto.avanzi@gmail.com>, <roberto.avanzi@arm.com>
 * @author Subhadeep Banik <subhadeep.banik@epfl.ch>
 * @author Francesco Regazzoni <regazzoni@alari.ch>
 *
 * This code is hereby placed in the public domain.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ''AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.	IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "api.h"
#include "crypto_aead.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "qarma64tc_7.h"

#define MSB_AD				  (0x2)
#define MSB_AD_LAST			  (0x3)
#define MSB_M				  (0x0)
#define MSB_MB				  (0x8)
#define MSB_NE				  (0xF)
#define MSB_LAST			  (0x1)
#define MSB_LASTB			  (0x9)

#define MSB_CHKSUM			  (0x4)
#define MSB_CHKSUMB			  (0xC)
#define ROUNDS				  7
#define TAGLENGTH			  16

// taglength in nibbles

void
set_nonce_in_tweak(u8 * tweak, const u8 * nonce)
{
	int		i;
	for (i = 0; i < 16; i++)
		tweak[i + 16] = nonce[i];
}

/*
 * * Modifiy the block number in the tweak value
 */
void
set_block_number_in_tweak(u8 * tweak, const u64 block_no)
{
	tweak[1] = (block_no >> 56ULL) & 0xf;
	tweak[2] = (block_no >> 52ULL) & 0xf;
	tweak[3] = (block_no >> 48ULL) & 0xf;
	tweak[4] = (block_no >> 44ULL) & 0xf;
	tweak[5] = (block_no >> 40ULL) & 0xf;
	tweak[6] = (block_no >> 36ULL) & 0xf;
	tweak[7] = (block_no >> 32ULL) & 0xf;
	tweak[8] = (block_no >> 28ULL) & 0xf;
	tweak[9] = (block_no >> 24ULL) & 0xf;
	tweak[10] = (block_no >> 20ULL) & 0xf;
	tweak[11] = (block_no >> 16ULL) & 0xf;
	tweak[12] = (block_no >> 12ULL) & 0xf;
	tweak[13] = (block_no >> 8ULL) & 0xf;
	tweak[14] = (block_no >> 4ULL) & 0xf;
	tweak[15] = (block_no >> 0ULL) & 0xf;
}

/*
 * * Modifiy the stage value in the tweak value
 */
void
set_stage_in_tweak(u8 * tweak, const u8 value)
{
	tweak[0] = (tweak[0] & 0xf) ^ value;
}

int
crypto_aead_encrypt(
		    unsigned char *c, unsigned long long *clen,
		    const unsigned char *m, unsigned long long mlen,
		    const unsigned char *ad, unsigned long long adlen,
		    const unsigned char *nsec,
		    const unsigned char *npub,
		    const unsigned char *k
)
{
	unsigned long long int i;
	unsigned long long int j;

	unsigned char	tweak[16], tweakl[32], k0[16], w0[16], k1[16], k1_M[16], w1 [16], whitekey[16];

	unsigned char	last_block[16];
	unsigned char	Checksum[16];
	unsigned char	Final[16];

	unsigned char	Pad[16];

	(void)nsec;
	(void)ad;
	if (adlen != 0)
		return -1;
	for (i = 0; i < 16; i++) {
		w0[i] = k[i];
		k0[i] = k[16 + i];
	}

	for (i = 0; i < mlen + TAGLENGTH; i++) {
		c[i] = 0;
	}
	*clen = 0;
	for (j = 0; j < 16; j++)
		Checksum[j] = 0;
	KeySpecialisation(k0, w0, k1, k1_M, w1);

	for (j = 0; j < 16; j++)
		tweak[j] = 0;

	if (mlen) {
		/* Message */

		for (j = 0; j < 32; j++)
			tweakl[j] = 0;
		set_nonce_in_tweak(tweakl, npub);

		for (j = 0; j < 16; j++)
			Checksum[j] = 0;
		set_stage_in_tweak(tweakl, MSB_M);

		i = 0;
		while (16 * (i + 1) < mlen) {

			for (j = 0; j < 16; j++)
				Checksum[j] ^= m[16 * i + j];
			set_block_number_in_tweak(tweakl, i);

			tweakcompression(tweakl, k0, k1, tweak, whitekey);

			qarma64(m + 16 * i, w0, w1, k0, k1, tweak, whitekey, ROUNDS, 1, c + 16 * i);

			i++;
		}

		/* Process last block */

		for (j = 0; j < 16; j++)
			last_block[j] = 0;

		for (j = 0; j < mlen - 16 * i; j++)
			last_block[j] = *(m + 16 * i + j);

		//xor_values(Checksum, last_block);
		for (j = 0; j < 16; j++)
			Checksum[j] ^= last_block[j];

		/* Encrypt it */

		set_stage_in_tweak(tweakl, MSB_LAST);

		set_block_number_in_tweak(tweakl, mlen / 2);

		tweakcompression(tweakl, k0, k1, tweak, whitekey);

		qarma64(last_block, w0, w1, k0, k1, tweak, whitekey, ROUNDS, 1, Pad);

		/* Write the ciphertext block */
		for (j = 0; j < 16; j++) {
			c[16 * i + j] = Pad[j];
		}
		*clen = 16 * i + 16;
	}			/* if message length>0 */
	set_stage_in_tweak(tweakl, MSB_CHKSUM);

	set_nonce_in_tweak(tweakl, npub);
	set_block_number_in_tweak(tweakl, 0);

	tweakcompression(tweakl, k0, k1, tweak, whitekey);

	qarma64(Checksum, w0, w1, k0, k1, tweak, whitekey, ROUNDS, 1, Final);

	for (i = 0; i < TAGLENGTH; i++) {
		c[*clen + i] = Final[i];

	}
	*clen = *clen + TAGLENGTH;
	return 0;
}

int
crypto_aead_decrypt(
		    unsigned char *m, unsigned long long *outputmlen,
		    unsigned char *nsec,
		    const unsigned char *c, unsigned long long clen,
		    const unsigned char *ad, unsigned long long adlen,
		    const unsigned char *npub,
		    const unsigned char *k
)
{

	unsigned char	tweak[16], tweakl[32], k0[16], w0[16], k1[16], k1_M[16],
			w1            [16], w0d[16], k0d[16], w1d[16], whitekey[16];

	unsigned long long int i;
	unsigned long long int j;

	unsigned char	Checksum[16];
	unsigned char	Final[16];

	unsigned char	Pad[16];
	unsigned char	Tag[TAGLENGTH], Tagc[TAGLENGTH];

	(void)nsec;
	(void)ad;
	if (adlen != 0)
		return -1;
	if (clen % 128 != TAGLENGTH)
		return -1;

	for (i = 0; i < 16; i++) {
		w0[i] = k[i];
		k0[i] = k[16 + i];
	}

	KeySpecialisation_dec(k0, w0, k1, k1_M, w1, w0d, w1d, k0d);

	/* Get the tag from the last 16 nibbles of the ciphertext */
	for (i = 0; i < TAGLENGTH; i++)
		Tag[i] = c[clen - TAGLENGTH + i];

	/*
	 * Update c_len to the actual size of the ciphertext (i.e., without
	 * the tag)
	 */
	*outputmlen = clen - TAGLENGTH;
	clen -= TAGLENGTH;

	/* Fill the tweak from nonce */

	for (j = 0; j < 16; j++)
		tweak[j] = Checksum[j] = 0;

	i = 0;

	if (clen) {
		/* Message */

		for (j = 0; j < 32; j++)
			tweakl[j] = 0;
		set_nonce_in_tweak(tweakl, npub);

		set_stage_in_tweak(tweakl, MSB_M);

		i = 0;

		while (16 * (i + 1) < clen) {

			set_block_number_in_tweak(tweakl, i);
			tweakcompression(tweakl, k0, k1, tweak, whitekey);
			qarma64(c + 16 * i, w0d, w1d, k0d, k1_M, tweak, whitekey, ROUNDS, 0, m + 16 * i);

			for (j = 0; j < 16; j++)
				Checksum[j] ^= m[16 * i + j];

			i++;
		}

		/* Process last block */

		/* Encrypt it */

		set_stage_in_tweak(tweakl, MSB_LAST);

		set_block_number_in_tweak(tweakl, *outputmlen / 2);
		tweakcompression(tweakl, k0, k1, tweak, whitekey);

		qarma64(c + 16 * i, w0d, w1d, k0d, k1_M, tweak, whitekey, ROUNDS, 0, Pad);

		for (j = 0; j < 16; j++)
			Checksum[j] ^= Pad[j];

		/* Write the ciphertext block */
		for (j = 0; j < *outputmlen - 16 * i; j++) {
			m[16 * i + j] = Pad[j];
		}

	}
	/* if message length>0 */

	set_stage_in_tweak(tweakl, MSB_CHKSUM);

	set_nonce_in_tweak(tweakl, npub);
	set_block_number_in_tweak(tweakl, 0);
	tweakcompression(tweakl, k0, k1, tweak, whitekey);

	qarma64(Checksum, w0, w1, k0, k1, tweak, whitekey, ROUNDS, 1, Final);
	for (j = 0; j < TAGLENGTH; j++)
		Tagc[j] = Final[j];

	if (0 != memcmp(Tagc, Tag, TAGLENGTH)) {
		memset(m, 0, clen);
		return -1;
	}
	return 0;
}
