/**
 * encrypt.c
 *
 * @version 1.0 (March 2019)
 *
 * Reference ANSI C code for the Qameleon AEAD cipher
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
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE
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

#include "qarma128_14.h"

#define MSB_AD                (0x2<<4)
#define MSB_AD_LAST           (0x3<<4)
#define MSB_M                 (0x0<<4)
#define MSB_MB                (0x8<<4)
#define MSB_NE                (0xF<<4)
#define MSB_LAST              (0x1<<4)
#define MSB_LASTB             (0x9<<4)

#define MSB_CHKSUM            (0x4<<4)
#define MSB_CHKSUMB           (0xC<<4)
#define ROUNDS                14
#define TAGLENGTH             16

void
set_nonce_in_tweak(u8 * tweak, const u8 * nonce)
{
    int     i;

    for (i = 0; i < 8; i++)
        tweak[i + 8] = nonce[i];
}

/*
 * * Modifiy the block number in the tweak value
 */
void
set_block_number_in_tweak(u8 * tweak, const u64 block_no)
{

    tweak[0] = (tweak[0] & 0xf0) ^ ((block_no >> 56ULL) & 0xf);
    tweak[1] = ((block_no >> 48ULL) & 0xff);
    tweak[2] = ((block_no >> 40ULL) & 0xff);
    tweak[3] = ((block_no >> 32ULL) & 0xff);
    tweak[4] = ((block_no >> 24ULL) & 0xff);
    tweak[5] = ((block_no >> 16ULL) & 0xff);
    tweak[6] = ((block_no >> 8ULL) & 0xff);
    tweak[7] = ((block_no >> 0ULL) & 0xff);

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

    unsigned char   tweak[16], k0[16], w0[16], k1[16], k1_M[16], w1[16];

    unsigned char   Auth[16];
    unsigned char   last_block[16];
    unsigned char   Checksum[16];
    unsigned char   Final[16];

    unsigned char   Pad[16];
    unsigned char   temp[16];

    (void)nsec;
    for (i = 0; i < 16; i++) {

        w0[i] = k[i];
        k0[i] = k[16 + i];

    }

    for (i = 0; i < mlen + 16; i++) {
        c[i] = 0;
    }
    *clen = 0;
    for (j = 0; j < 16; j++)
        Checksum[j] = 0;
    KeySpecialisation(k0, w0, k1, k1_M, w1);

    for (j = 0; j < 16; j++)
        tweak[j] = 0;

    for (i = 0; i < 16; i++)
        Auth[i] = 0;

    if (adlen) {
        set_stage_in_tweak(tweak, MSB_AD);

        /* For each full input blocks */
        i = 0;
        while (16 * (i + 1) < adlen) {

            /* Encrypt the current block */
            set_block_number_in_tweak(tweak, i);

            qarma128(ad + 16 * i, w0, w1, k0, k1, tweak, ROUNDS, temp);

            /* Update Auth value */
            for (j = 0; j < 16; j++)
                Auth[j] ^= temp[j];

            /* Go on with the next block */
            i++;
        }

        /* Prepare the last padded block */

        for (j = 0; j < 16; j++)
            last_block[j] = 0;
        for (j = 0; j < adlen - 16 * i; j++)
            last_block[j] = *(ad + 16 * i + j);

        if (adlen % 16 != 0)
            last_block[adlen - 16 * i] = 0x80;

        /* Encrypt the last block */
        set_stage_in_tweak(tweak, MSB_AD_LAST);
        set_block_number_in_tweak(tweak, adlen);

        qarma128(last_block, w0, w1, k0, k1, tweak, ROUNDS, temp);

        /* Update the Auth value */
        /* xor_values(Auth, temp); */

        for (j = 0; j < 16; j++)
            Auth[j] ^= temp[j];

    }           /* if ass_data_len>0 */
    if (mlen) {
        /* Message */

        for (j = 0; j < 16; j++)
            tweak[j] = 0;
        set_nonce_in_tweak(tweak, npub);

        set_stage_in_tweak(tweak, MSB_M);

        i = 0;
        while (16 * (i + 1) < mlen) {

            for (j = 0; j < 16; j++)
                Checksum[j] ^= m[16 * i + j];
            set_block_number_in_tweak(tweak, i);

            qarma128(m + 16 * i, w0, w1, k0, k1, tweak, ROUNDS, c + 16 * i);

            i++;
        }

        /* Process last block */

        for (j = 0; j < 16; j++)
            last_block[j] = 0;

        for (j = 0; j < mlen - 16 * i; j++)
            last_block[j] = *(m + 16 * i + j);

        if (mlen % 16 != 0)
            last_block[mlen - 16 * i] = 0x80;

        /* xor_values(Checksum, last_block); */
        for (j = 0; j < 16; j++)
            Checksum[j] ^= last_block[j];

        /* Encrypt it */

        set_stage_in_tweak(tweak, MSB_LAST);

        set_block_number_in_tweak(tweak, mlen);

        qarma128(last_block, w0, w1, k0, k1, tweak, ROUNDS, Pad);

        /* Write the ciphertext block */
        for (j = 0; j < 16; j++) {

            c[16 * i + j] = Pad[j];
        }
        *clen = 16 * i + 16;
    }           /* if message length>0 */
    set_stage_in_tweak(tweak, MSB_CHKSUM);
    set_nonce_in_tweak(tweak, npub);
    set_block_number_in_tweak(tweak, 0);

    qarma128(Checksum, w0, w1, k0, k1, tweak, ROUNDS, Final);

    for (i = 0; i < TAGLENGTH; i++) {
        c[*clen + i] = Final[i] ^ Auth[i];

    }
    *clen = *clen + TAGLENGTH;

    c[*clen] = ((mlen >> 56ULL) & 0xf);
    c[*clen + 1] = ((mlen >> 48ULL) & 0xff);
    c[*clen + 2] = ((mlen >> 40ULL) & 0xff);
    c[*clen + 3] = ((mlen >> 32ULL) & 0xff);
    c[*clen + 4] = ((mlen >> 24ULL) & 0xff);
    c[*clen + 5] = ((mlen >> 16ULL) & 0xff);
    c[*clen + 6] = ((mlen >> 8ULL) & 0xff);
    c[*clen + 7] = ((mlen >> 0ULL) & 0xff);
    *clen = *clen + 8;
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

    unsigned char   tweak[16], k0[16], w0[16], k1[16], k1_M[16], w1[16], w0d[16],
            k0d           [16], w1d[16];

    unsigned long long int i;
    unsigned long long int j;

    unsigned char   Auth[16];
    unsigned char   last_block[16];
    unsigned char   Checksum[16];
    unsigned char   Final[16];

    unsigned char   Pad[16];
    unsigned char   Tag[16], Tagc[16];
    unsigned char   temp[16];

    (void)nsec;
    if (clen % 16 != 8)
        return -1;

    for (i = 0; i < 16; i++) {

        w0[i] = k[i];
        k0[i] = k[16 + i];

    }

    KeySpecialisation_dec(k0, w0, k1, k1_M, w1, w0d, w1d, k0d);

    /* Get the tag from the last 16 bytes of the ciphertext */
    memcpy(Tag, c + clen - TAGLENGTH - 8, 16);
    *outputmlen = ((unsigned long long)c[clen - 8] << 56) ^
        ((unsigned long long)c[clen - 7] << 48) ^
        ((unsigned long long)c[clen - 6] << 40) ^
        ((unsigned long long)c[clen - 5] << 32) ^
        ((unsigned long long)c[clen - 4] << 24) ^
        ((unsigned long long)c[clen - 3] << 16) ^
        ((unsigned long long)c[clen - 2] << 8) ^
        ((unsigned long long)c[clen - 1]);

    /*
     * Update c_len to the actual size of the ciphertext (i.e., without
     * the tag)
     */
    clen -= (TAGLENGTH + 8);

    /* Fill the tweak from nonce */
    memset(tweak, 0, 16);
    memset(Auth, 0, 16);
    for (j = 0; j < 16; j++)
        Checksum[j] = 0;
    i = 0;

    if (adlen) {
        set_stage_in_tweak(tweak, MSB_AD);

        /* For each full input blocks */
        i = 0;
        while (16 * (i + 1) < adlen) {

            /* Encrypt the current block */
            set_block_number_in_tweak(tweak, i);

            qarma128(ad + 16 * i, w0, w1, k0, k1, tweak, ROUNDS, temp);

            /* Update Auth value */
            for (j = 0; j < 16; j++)
                Auth[j] ^= temp[j];

            /* Go on with the next block */
            i++;
        }

        /* Prepare the last padded block */

        for (j = 0; j < 16; j++)
            last_block[j] = 0;
        for (j = 0; j < adlen - 16 * i; j++)
            last_block[j] = *(ad + 16 * i + j);

        if (adlen % 16 != 0)
            last_block[adlen - 16 * i] = 0x80;

        /* Encrypt the last block */
        set_stage_in_tweak(tweak, MSB_AD_LAST);
        set_block_number_in_tweak(tweak, adlen);

        qarma128(last_block, w0, w1, k0, k1, tweak, ROUNDS, temp);

        /* Update the Auth value */
        //xor_values(Auth, temp);
        for (j = 0; j < 16; j++)
            Auth[j] ^= temp[j];

    }           /* if ass_data_len>0 */
    if (clen) {
        /* Message */

        for (j = 0; j < 16; j++)
            tweak[j] = 0;
        set_nonce_in_tweak(tweak, npub);

        set_stage_in_tweak(tweak, MSB_M);

        i = 0;

        while (16 * (i + 1) < clen) {

            set_block_number_in_tweak(tweak, i);

            qarma128(c + 16 * i, w0d, w1d, k0d, k1_M, tweak, ROUNDS, m + 16 * i);

            for (j = 0; j < 16; j++)
                Checksum[j] ^= m[16 * i + j];

            i++;
        }

        /* Process last block */

        /* Encrypt it */

        set_stage_in_tweak(tweak, MSB_LAST);

        set_block_number_in_tweak(tweak, *outputmlen);

        qarma128(c + 16 * i, w0d, w1d, k0d, k1_M, tweak, ROUNDS, Pad);

        for (j = 0; j < 16; j++)
            Checksum[j] ^= Pad[j];

        /* Write the ciphertext block */
        for (j = 0; j < *outputmlen - 16 * i; j++) {

            m[16 * i + j] = Pad[j];
        }

        /* if(*outputmlen%16!=0) {m[*outputmlen-16*i]=0x80; */

    }
    /* if message length>0 */

    set_stage_in_tweak(tweak, MSB_CHKSUM);

    set_nonce_in_tweak(tweak, npub);
    set_block_number_in_tweak(tweak, 0);

    qarma128(Checksum, w0, w1, k0, k1, tweak, ROUNDS, Final);
    for (j = 0; j < 16; j++)
        Tagc[j] = Final[j] ^ Auth[j];

    if (0 != memcmp(Tagc, Tag, 16)) {
        memset(m, 0, clen);
        return -1;

    }
    return 0;

}
