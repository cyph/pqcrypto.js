/*
 * Deoxys-I-256 Bit-Sliced C Implementation
 *
 * Copyright 2016:
 *     Jeremy Jean <JJean@ntu.edu.sg>
 *     Ivica Nikolic <inikolic@ntu.edu.sg>
 *     Haoyang Wang <wang1153@e.ntu.edu.sg>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <tmmintrin.h>
#include "deoxysi256.macros"
#include "tweakableBC.h"

/* Define the three MSB of the tweak (that depend on the stage) */
#define MSB_AD                (0x2<<4)
#define MSB_AD_LAST           (0x6<<4)
#define MSB_M                 (0x0<<4)
#define MSB_M_LAST_NONZERO    (0x4<<4)
#define MSB_CHKSUM_FULL       (0x1<<4)
#define MSB_CHKSUM_NON_FULL   (0x5<<4)

/* Number of bits in the TWEAKEY state (256 or 384) */
#define TWEAKEY_STATE_SIZE    384

void DeoxysDecrypt_Checksum(__m128i rk[17][8], const uint8_t* ct, uint8_t pt[128], __m128i* Checksum);
void DeoxysEncrypt_Checksum(__m128i rk[17][8], const uint8_t* pt, uint8_t ct[128], __m128i* Checksum);
void DeoxysEncrypt_Auth(__m128i rk[17][8], const uint8_t* pt, __m128i* Auth);

/**********************************************************************************
*** In Deoxys-i-128, the tweak is on 128 bits:
***     tweak = <stage> || <nonce> || <blockNumber>
***  where we use:
***      4 bits for stage
***     64 bits for nonce
***     60 bits for blockNumber
***********************************************************************************/

/*
** Modifiy the nonce part in the tweak value
*/
static void set_nonce_in_tweak(uint8_t *tweak, const uint8_t *nonce) {
    tweak[0] = (tweak[0]&0xf0)     ^ (nonce[0] >> 4);
    tweak[1] = (nonce[0]&0xf) << 4 ^ (nonce[1] >> 4);
    tweak[2] = (nonce[1]&0xf) << 4 ^ (nonce[2] >> 4);
    tweak[3] = (nonce[2]&0xf) << 4 ^ (nonce[3] >> 4);
    tweak[4] = (nonce[3]&0xf) << 4 ^ (nonce[4] >> 4);
    tweak[5] = (nonce[4]&0xf) << 4 ^ (nonce[5] >> 4);
    tweak[6] = (nonce[5]&0xf) << 4 ^ (nonce[6] >> 4);
    tweak[7] = (nonce[6]&0xf) << 4 ^ (nonce[7] >> 4);
    tweak[8] = (nonce[7]&0xf) << 4;
}

/*
** Modifiy the block number in the tweak value
*/
static void set_block_number_in_tweak(uint8_t *tweak, const uint64_t block_no) {
    tweak[ 8] = (tweak[8]&0xf0) ^ ((block_no >> 56ULL) & 0xf);
    tweak[ 9] = ((block_no >> 48ULL) & 0xff);
    tweak[10] = ((block_no >> 40ULL) & 0xff);
    tweak[11] = ((block_no >> 32ULL) & 0xff);
    tweak[12] = ((block_no >> 24ULL) & 0xff);
    tweak[13] = ((block_no >> 16ULL) & 0xff);
    tweak[14] = ((block_no >>  8ULL) & 0xff);
    tweak[15] = ((block_no >>  0ULL) & 0xff);
}

/*
** Modifiy the stage value in the tweak value
*/
static void set_stage_in_tweak(uint8_t *tweak, const uint8_t value) {
    tweak[0]=(tweak[0] & 0xf) ^ value ;
}

/*
** Update the tweak value in the tweakey word.
** In the case of Deoxys-BC-256, the tweakey word is composed of KEY || TWEAK.
** In the case of Deoxys-BC-384, the tweakey word is composed of KEY_2 || KEY_1 || TWEAK.
*/
static void set_tweak_in_tweakey(uint8_t *tweakey, uint8_t *tweak) {
#if TWEAKEY_STATE_SIZE==256
    memcpy(tweakey+16, tweak, 16);
#elif TWEAKEY_STATE_SIZE==384
    memcpy(tweakey+32, tweak, 16);
#endif
}

/*
** Constant-time memcmp function
*/
static int memcmp_const(const void * a, const void *b, const size_t size)  {

    size_t i;
    unsigned char result = 0;
    const unsigned char *_a = (const unsigned char *) a;
    const unsigned char *_b = (const unsigned char *) b;

    for (i = 0; i < size; i++) {
        result |= _a[i] ^ _b[i];
    }

    /* returns 0 if equal, nonzero otherwise */
    return result;
}

/*
** XOR an input block to another input block
*/
static void xor_values(uint8_t *v1, const uint8_t *v2) {
    int i;
    for (i=0; i<16; i++) v1[i] ^= v2[i];
}

/*
** Deoxys encryption function
*/
void deoxys_aead_encrypt_8(const uint8_t *ass_data, size_t ass_data_len,
                           const uint8_t *message, size_t m_len,
                           const uint8_t *key,
                           const uint8_t *nonce,
                           uint8_t *ciphertext, size_t *c_len)
{

    uint64_t i;
    uint64_t j;
    uint64_t t;
    uint8_t tweak[16];
    uint8_t tweakey[TWEAKEY_STATE_SIZE/8];
    uint8_t Auth[16];
    uint8_t last_block[16];
    uint8_t Checksum[16];
    uint8_t Final[16];
    uint8_t zero_block[16];
    uint8_t Pad[16];
    uint8_t temp[16];
    __m128i rk[17][8];
    __m128i subkey[17][8];
    __m128i Tweak;
    __m128i Tweaks[17][8];
    __m128i AUTH;
    __m128i CHECKSUM;
    __m128i *AUTH_P;
    __m128i *CHECKSUM_P;


    /* Fill the tweak with zeros */
    memset(tweak, 0, sizeof(tweak));

    /* Fill the key(s) in the tweakey state */
    memcpy(tweakey, key, 32);

    /* Pre-expansion of the subkeys in the subtweakeys for encryption */
    prepare_subkeys(tweakey, TWEAKEY_STATE_SIZE/128);

    /* Associated data */
    memset(Auth, 0, 16);
    AUTH=XOR(AUTH,AUTH);
    AUTH_P=&AUTH;

    /* Key Schedule */
    KEY_SCHEDULE(key, subkey);

    if(ass_data_len) {
        set_stage_in_tweak(tweak, MSB_AD);
        Tweak=LOAD(tweak);

        i=0;
        /* bitsliced implementation */
        if(128<=ass_data_len) {
            FIRST_ROUNDKEYS_ASS(subkey, rk);
            DeoxysEncrypt_Auth(rk, ass_data+0, AUTH_P);
            i=1;
        }

        while (128*(i+1) <= ass_data_len) {
            if(0==(i%32)) {
                ADD_TO_TWEAK(Tweak, CARRY);
                RENEW_TWEAKS(Tweaks, Tweak);
                ROUNDKEY(Tweaks, subkey, rk);
            } else {
                UPDATE_SUBKEYS(rk,i);
            }

            DeoxysEncrypt_Auth(rk, ass_data+128*i, AUTH_P);
            i++;
        }

        /* Table-based implementation */
        j=0;
        while (16*(j+1) <= (ass_data_len-128*i)) {

            /* Encrypt the current block */
            set_block_number_in_tweak(tweak, j+8*i);
            set_tweak_in_tweakey(tweakey, tweak);
            aesTweakEncrypt(TWEAKEY_STATE_SIZE, ass_data+(128*i+16*j), tweakey, temp);

            /* Update Auth value */
            AUTH=XOR(AUTH,LOAD(temp));

            /* Go on with the next block */
            j++;
        }

        /* Last block if incomplete */
        if (ass_data_len > 128*i+16*j) {

            /* Prepare the last padded block */
            memset(last_block, 0, 16);
            memcpy(last_block, ass_data+(128*i+16*j), ass_data_len-(128*i+16*j));
            last_block[ass_data_len-(128*i+16*j)]=0x80;

            /* Encrypt the last block */
            set_stage_in_tweak(tweak, MSB_AD_LAST);
            set_block_number_in_tweak(tweak, j+8*i);
            set_tweak_in_tweakey(tweakey, tweak);
            aesTweakEncrypt(TWEAKEY_STATE_SIZE, last_block, tweakey, temp);

            /* Update the Auth value */
            AUTH=XOR(AUTH,LOAD(temp));
        }

        STORE(Auth,AUTH);

    }/* if ass_data_len>0 */


    /* Message */
    memset(tweak, 0, sizeof(tweak));
    set_nonce_in_tweak(tweak, nonce);
    memset(Checksum, 0, 16);
    CHECKSUM=XOR(CHECKSUM,CHECKSUM);
    CHECKSUM_P=&CHECKSUM;

    set_stage_in_tweak(tweak, MSB_M);
    Tweak=LOAD(tweak);

    /* bitsliced implementation */

    i=0;
    if(128 <= m_len) {
        RENEW_TWEAKS(Tweaks, Tweak);
        ROUNDKEY(Tweaks, subkey, rk);
        DeoxysEncrypt_Checksum(rk, message, ciphertext, CHECKSUM_P);
        i=1;
    }

    while (128*(i+1) <= m_len) {
        if(0==(i%32)) {
            ADD_TO_TWEAK(Tweak, CARRY);
            RENEW_TWEAKS(Tweaks, Tweak);
            ROUNDKEY(Tweaks, subkey, rk);
        } else {
            UPDATE_SUBKEYS(rk,i);
        }

        DeoxysEncrypt_Checksum(rk, message+128*i, ciphertext+128*i, CHECKSUM_P);
        i++;
    }


    /* Table-based implementation */
    j=0;
    while (16*(j+1) <= (m_len-128*i)) {

        /* Encrypt the current block */
        CHECKSUM=XOR(CHECKSUM, LOAD(message+(128*i+16*j)));
        set_block_number_in_tweak(tweak, j+8*i);
        set_tweak_in_tweakey(tweakey, tweak);
        aesTweakEncrypt(TWEAKEY_STATE_SIZE, message+(128*i+16*j), tweakey, ciphertext+(128*i+16*j));

        /* Go on with the next block */
        j++;
    }


    /* Process incomplete block */
    if (m_len > 128*i+16*j) {

        memset(last_block, 0, 16);
        memcpy(last_block, message +(128*i+16*j), m_len-(128*i+16*j));
        last_block[m_len-(128*i+16*j)]=0x80;
        CHECKSUM=XOR(CHECKSUM,LOAD(last_block));
        STORE(Checksum,CHECKSUM);

        /* Create the zero block for encryption */
        memset(zero_block, 0, 16);

        /* Encrypt it */
        set_stage_in_tweak(tweak, MSB_M_LAST_NONZERO);
        set_block_number_in_tweak(tweak, j+8*i);
        set_tweak_in_tweakey(tweakey, tweak);
        aesTweakEncrypt(TWEAKEY_STATE_SIZE, zero_block, tweakey, Pad);

        for (t=0; t<m_len-(128*i+16*j); t++) {
            ciphertext[(128*i+16*j)+t] = last_block[t] ^ Pad[t];
        }

        j++;
        set_block_number_in_tweak(tweak, j+8*i);
        set_stage_in_tweak(tweak, MSB_CHKSUM_NON_FULL);
        set_tweak_in_tweakey(tweakey, tweak);
        aesTweakEncrypt(TWEAKEY_STATE_SIZE, Checksum, tweakey, Final);

    } else {

        STORE(Checksum,CHECKSUM);
        set_block_number_in_tweak(tweak, j+8*i);
        set_stage_in_tweak(tweak, MSB_CHKSUM_FULL);
        set_tweak_in_tweakey(tweakey, tweak);
        aesTweakEncrypt(TWEAKEY_STATE_SIZE, Checksum, tweakey, Final);

    }

    /* Append the authentication tag to the ciphertext */
    for (i=0; i<16; i++) {
        ciphertext[m_len+i]=Final[i] ^ Auth[i];
    }

    *c_len=m_len+16;
}

/*
** Deoxys decryption function
*/
int deoxys_aead_decrypt_8(const uint8_t *ass_data, size_t ass_data_len,
                          uint8_t *message, size_t *m_len,
                          const uint8_t *key,
                          const uint8_t *nonce,
                          const uint8_t *ciphertext, size_t c_len)
{

    uint64_t i;
    uint64_t j;
    uint64_t t;
    uint8_t tweak[16];
    uint8_t tweakey[TWEAKEY_STATE_SIZE/8];
    uint8_t Auth[16];
    uint8_t last_block[16];
    uint8_t Checksum[16];
    uint8_t Final[16];
    uint8_t zero_block[16];
    uint8_t Pad[16];
    uint8_t Tag[16];
    uint8_t temp[16];
    __m128i rk[17][8];
    __m128i subkey[17][8];
    __m128i Tweak;
    __m128i Tweaks[17][8];
    __m128i AUTH,CHECKSUM;
    __m128i *AUTH_P;
    __m128i *CHECKSUM_P;

    /* Get the tag from the last 16 bytes of the ciphertext */
    memcpy(Tag, ciphertext+c_len-16, 16);

    /* Update c_len to the actual size of the ciphertext (i.e., without the tag) */
    c_len-=16;

    /* Fill the tweak with zeros */
    memset(tweak, 0, sizeof(tweak));

    /* Fill the key(s) in the tweakey state */
    memcpy(tweakey, key, 32);

    /* Pre-expansion of the subkeys in the subtweakeys for encryption */
    prepare_subkeys(tweakey, TWEAKEY_STATE_SIZE/128);

    /* Associated data */
    memset(Auth, 0, 16);
    AUTH=XOR(AUTH,AUTH);
    AUTH_P=&AUTH;

    /* Key schedule */
    KEY_SCHEDULE(key, subkey);

    /* Associated data */
    if(ass_data_len) {

        set_stage_in_tweak(tweak, MSB_AD);
        Tweak=LOAD(tweak);
        i=0;

        /* bitsliced implementation*/
        if(128<=ass_data_len) {
            FIRST_ROUNDKEYS_ASS(subkey, rk);
            DeoxysEncrypt_Auth(rk, ass_data, AUTH_P);
            i=1;
        }

        while (128*(i+1) <= ass_data_len) {
            if(0==(i%32)) {
                ADD_TO_TWEAK(Tweak, CARRY);
                RENEW_TWEAKS(Tweaks, Tweak);
                ROUNDKEY(Tweaks, subkey, rk);
            } else {
                UPDATE_SUBKEYS(rk,i);
            }

            DeoxysEncrypt_Auth(rk, ass_data+128*i, AUTH_P);
            i++;
        }

        /* Table-based implementation */
        j=0;
        while (16*(j+1) <= (ass_data_len-128*i)) {

            /* Encrypt the current block */
            set_block_number_in_tweak(tweak, j+8*i);
            set_tweak_in_tweakey(tweakey, tweak);
            aesTweakEncrypt(TWEAKEY_STATE_SIZE, ass_data+(128*i+16*j), tweakey, temp);

            /* Update Auth value */
            AUTH=XOR(AUTH,LOAD(temp));

            /* Go on with the next block */
            j++;
        }


        /* Last block if incomplete */
        if (ass_data_len > 128*i+16*j) {

            memset(last_block, 0, 16);
            memcpy(last_block, ass_data+(128*i+16*j), ass_data_len-(128*i+16*j));
            last_block[ass_data_len-(128*i+16*j)]=0x80;
            set_stage_in_tweak(tweak, MSB_AD_LAST);
            set_block_number_in_tweak(tweak, j+8*i);
            set_tweak_in_tweakey(tweakey, tweak);
            aesTweakEncrypt(TWEAKEY_STATE_SIZE, last_block, tweakey, temp);
            AUTH=XOR(AUTH,LOAD(temp));
        }

        STORE(Auth, AUTH);

    }/* if ass_data_len>0 */



    /* Ciphertext */
    memset(tweak, 0, sizeof(tweak));
    set_nonce_in_tweak(tweak, nonce);
    memset(Checksum, 0, 16);
    CHECKSUM=XOR(CHECKSUM,CHECKSUM);
    CHECKSUM_P=&CHECKSUM;

    set_stage_in_tweak(tweak, MSB_M);
    Tweak=LOAD(tweak);

    i=0;
    /* bitsliced implementation */
    if(128<=c_len) {
        RENEW_TWEAKS(Tweaks, Tweak);
        ROUNDKEY(Tweaks, subkey, rk);
        DeoxysDecrypt_Checksum(rk, ciphertext, message, CHECKSUM_P);
        i=1;
    }

    while (128*(i+1) <= c_len) {
        if(0==(i%32)) {
            ADD_TO_TWEAK(Tweak, CARRY);
            RENEW_TWEAKS(Tweaks, Tweak);
            ROUNDKEY(Tweaks, subkey, rk);
        } else {
            UPDATE_SUBKEYS(rk,i);
        }

        DeoxysDecrypt_Checksum(rk, ciphertext+128*i, message+128*i, CHECKSUM_P);
        i++;
    }

    /* Table-based implementation */
    j=0;
    while (16*(j+1) <= (c_len-128*i)) {

        /* Encrypt the current block */
        set_block_number_in_tweak(tweak, j+8*i);
        set_tweak_in_tweakey(tweakey, tweak);
        aesTweakDecrypt(TWEAKEY_STATE_SIZE, ciphertext+(128*i+16*j), tweakey, message+(128*i+16*j));

        /* Update Checksum value */
        CHECKSUM=XOR(CHECKSUM, LOAD(message+(128*i+16*j)));

        /* Go on with the next block */
        j++;
    }


    /* Last block */
    /* If the block is full, i.e. M_last=epsilon */
    if (c_len == 128*i+16*j) {
        STORE(Checksum,CHECKSUM);
        set_block_number_in_tweak(tweak, j+8*i);
        set_stage_in_tweak(tweak, MSB_CHKSUM_FULL);
        set_tweak_in_tweakey(tweakey, tweak);
        aesTweakEncrypt(TWEAKEY_STATE_SIZE, Checksum, tweakey, Final);
        xor_values(Final, Auth);

        /* If the tags does not match, return error -1 */
        if(0 != memcmp_const(Final, Tag, sizeof(Tag))) {
            memset(message, 0, c_len);
            return -1;
        }

    } else {

        /* Prepare the full-zero block */
        memset(zero_block, 0, 16);

        /* Prepare the tweak */
        set_stage_in_tweak(tweak, MSB_M_LAST_NONZERO);
        set_block_number_in_tweak(tweak, j+8*i);
        set_tweak_in_tweakey(tweakey, tweak);

        /* Encrypt */
        aesTweakEncrypt(TWEAKEY_STATE_SIZE, zero_block, tweakey, Pad);

        /* XOR the partial ciphertext */
        memset(last_block, 0, 16);
        memcpy(last_block, ciphertext+(128*i+16*j), c_len-(128*i+16*j));

        memset(Pad+(c_len-128*i-16*j), 0, 16-c_len+128*i+16*j);
        xor_values(last_block, Pad);
        last_block[c_len-(128*i+16*j)]=0x80;

        for (t=0; t<c_len-128*i-16*j; t++) {
            message[128*i+16*j+t]=last_block[t];
        }

        /* Update checksum */
        CHECKSUM=XOR(CHECKSUM, LOAD(last_block));
        STORE(Checksum,CHECKSUM);

        /* Verify the tag */
        j++;
        set_block_number_in_tweak(tweak, j+8*i);
        set_stage_in_tweak(tweak, MSB_CHKSUM_NON_FULL);
        set_tweak_in_tweakey(tweakey, tweak);
        aesTweakEncrypt(TWEAKEY_STATE_SIZE, Checksum, tweakey, Final);
        xor_values(Final, Auth);

        /* If the tags does not match, return error -1 */
        if(0 != memcmp_const(Final, Tag, sizeof(Tag))) {
            memset(message, 0, c_len);
            return -1;
        }
    }

    *m_len=c_len;
    return 0;
}
