#pragma once

#ifndef COMMON_H
#define COMMON_H

#endif

#include <stdio.h>

typedef unsigned long long uint64_t;
typedef unsigned long uint32_t;

const uint64_t Z_VECTOR[5][62] = {
{ 1,1,1,1,1,0,1,0,0,0,1,0,0,1,0,1,0,1,1,0,0,0,0,1,1,1,0,0,1,1,0,1,1,1,1,1,0,1,0,0,0,1,0,0,1,0,1,0,1,1,0,0,0,0,1,1,1,0,0,1,1,0 },
{ 1,0,0,0,1,1,1,0,1,1,1,1,1,0,0,1,0,0,1,1,0,0,0,0,1,0,1,1,0,1,0,1,0,0,0,1,1,1,0,1,1,1,1,1,0,0,1,0,0,1,1,0,0,0,0,1,0,1,1,0,1,0 },
{ 1,0,1,0,1,1,1,1,0,1,1,1,0,0,0,0,0,0,1,1,0,1,0,0,1,0,0,1,1,0,0,0,1,0,1,0,0,0,0,1,0,0,0,1,1,1,1,1,1,0,0,1,0,1,1,0,1,1,0,0,1,1 },
{ 1,1,0,1,1,0,1,1,1,0,1,0,1,1,0,0,0,1,1,0,0,1,0,1,1,1,1,0,0,0,0,0,0,1,0,0,1,0,0,0,1,0,1,0,0,1,1,1,0,0,1,1,0,1,0,0,0,0,1,1,1,1 },
{ 1,1,0,1,0,0,0,1,1,1,1,0,0,1,1,0,1,0,1,1,0,1,1,0,0,0,1,0,0,0,0,0,0,1,0,1,1,1,0,0,0,0,1,1,0,0,1,0,1,0,0,1,0,0,1,1,1,0,1,1,1,1 }
};

#ifndef SIMON_WORD_SIZE
#define SIMON_WORD_SIZE 64
#endif 

/*byte length per block */

#ifndef BYTE_SIZE_PER_BLOCK
#define BYTE_SIZE_PER_BLOCK 16
#endif


/* -------- configuration -------------   */

#ifndef KEY_WORDS
#define KEY_WORDS  4
#endif

#ifndef ROUNDS
#define ROUNDS 72
#endif

/* -------- configuration -------------   */

#ifndef M
#define M 4
#endif

#ifndef MASK
#define MASK (0xffffffffffffffffULL >> (64 - SIMON_WORD_SIZE))
#endif

#ifndef XOR
#define XOR(x, y) ((x ^ y) & MASK)
#endif

#ifndef AND
#define AND(x, y) ((x & y) & MASK)
#endif

#ifndef LROT
#define LROT(x, r) (((x << r) | (x >> (SIMON_WORD_SIZE - r))) & MASK)
#endif

#ifndef RROT
#define RROT(x, r) (((x >> r) | (x << (SIMON_WORD_SIZE - r))) & MASK)
#endif

/*--- z vector for key shedule ----*/

#ifndef Z_VECTOR_CH
#define Z_VECTOR_CH 3
#endif

/*--- z vector for key shedule ----*/

/* ecryption constant for simon128 */

#ifndef ENC_CONSTANT
#define ENC_CONSTANT 0xfffffffffffffffc
#endif

/* simon round transform for short */

#ifndef ENTIRE_ROTATE
#define ENTIRE_ROTATE(v) ((LROT(v, 1) & LROT(v, 8)) ^ LROT(v, 2))
#endif


/*message must be 8 blocks for 64bits */

#ifndef MSG_BLOCK_SIZE
#define MSG_BLOCK_SIZE 8
#endif

void add_2sum_128(uint64_t input1[2], uint64_t input2[2], uint64_t output[2]);
void tho(uint64_t input1[2], uint64_t input2[2], uint64_t output[2]);
void tho_y(uint64_t input1[2], uint64_t input2[2], uint64_t output[2]);
void tho_reverse_y(uint64_t input1[2], uint64_t input2[2], uint64_t output[2]);
void cast_uint8_array_to_uint64(uint64_t *dest, const unsigned char *src);
void cast_uint64_to_uint8_array(unsigned char *dest, uint64_t src);
void key_schedule(uint64_t* key, uint64_t* dest);
void simon_round(uint64_t* p1, uint64_t* p2, uint64_t k);
void simon_encrypt(uint64_t p[2], uint64_t c[2], uint64_t k[ROUNDS]);
void simon_decrypt(uint64_t c[2], uint64_t p[2], uint64_t k[ROUNDS]);
void simon_backwards(uint64_t* p1, uint64_t* p2, uint64_t k);
void generate_s0_star(uint64_t nonce[2], uint64_t s0_star[2]);
void calculate_s0(const unsigned char *helper, uint64_t helper_byte_length, uint64_t *nonce, uint64_t *s0);
void copy_message(const unsigned char *src, unsigned char *dest, uint64_t src_byte_length, uint64_t require_byte_length);
void fill_plaintext(const unsigned char *message, uint64_t src_byte_length, uint64_t* block_messages);
void ae_encrypt_part_one(uint64_t *block_messages, uint64_t block_length, uint64_t s_output[2], uint64_t nonce[2]);
void fill_messages(const unsigned char *message, uint64_t src_byte_length, uint64_t* block_messages, uint64_t msg_block);
void ae_encrypt_part_two(uint64_t s0[2], uint64_t *block_messages, uint64_t block_length, uint64_t s_last[2], uint64_t* cipher_output);
void ae_encrypt_byte(const unsigned char *message, uint64_t src_byte_length, uint64_t nonce[2], const unsigned char *helper, uint64_t helper_byte_length, unsigned char *cipher);
void ae_decrypt_byte(const unsigned char *cipher, uint64_t cipher_byte_length, uint64_t nonce[2],const unsigned char *helper, uint64_t helper_byte_length, unsigned char* msg_output_bytes); 
void ae_encrypt_byte_nopad(const unsigned char *message, uint64_t src_byte_length, uint64_t nonce[2], const unsigned char *helper, uint64_t helper_byte_length, unsigned char *cipher);
void ae_decrypt_byte_nopad(const unsigned char *cipher, uint64_t cipher_byte_length, uint64_t nonce[2],const unsigned char *helper, uint64_t helper_byte_length, unsigned char* msg_output_bytes);
void encrypt_8_byte(const unsigned char *message, uint64_t nonce[2], const unsigned char *helper, uint64_t helper_byte_length, unsigned char *cipher);
void decrypt_8_byte(const unsigned char *cipher, uint64_t nonce[2], const unsigned char *helper, uint64_t helper_byte_length, unsigned char *msg);
void encrypt_16_byte(const unsigned char *message, uint64_t src_byte_length, uint64_t nonce[2], const unsigned char *helper, uint64_t helper_byte_length, unsigned char *cipher);
void decrypt_16_byte(const unsigned char *cipher, uint64_t cipher_byte_length, uint64_t nonce[2], const unsigned char *helper, uint64_t helper_byte_length, unsigned char *msg_output);
void encrypt_short_byte(const unsigned char *message, uint64_t src_byte_length, uint64_t nonce[2], const unsigned char *helper, uint64_t helper_byte_length, unsigned char *cipher);
void decrypt_short_byte(const unsigned char *cipher, uint64_t cipher_byte_length, uint64_t nonce[2], const unsigned char *helper, uint64_t helper_byte_length, unsigned char *msg_output);
int generate_keys(const unsigned char *k, uint64_t k_byte_length);
