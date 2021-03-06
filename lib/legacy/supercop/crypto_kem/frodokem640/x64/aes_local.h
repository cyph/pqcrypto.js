/**
 * \file aes_local.h
 * \brief Header defining additional internal functions for AES
 */

#ifndef AES_LOCAL_H
#define AES_LOCAL_H

#include <stdint.h>
#include <stdlib.h>

void aes128_load_schedule_ni(const uint8_t *key, void **schedule);
void aes256_load_schedule_ni(const uint8_t *key, void **schedule);
void aes_free_schedule_ni(void *schedule);
void aes128_enc_ni(const uint8_t *plaintext, const void *schedule, uint8_t *ciphertext);
void aes128_dec_ni(const uint8_t *ciphertext, const void *schedule, uint8_t *plaintext);
void aes256_enc_ni(const uint8_t *plaintext, const void *schedule, uint8_t *ciphertext);
void aes256_dec_ni(const uint8_t *ciphertext, const void *schedule, uint8_t *plaintext);

void aes128_load_schedule_c(const uint8_t *key, void **schedule);
void aes256_load_schedule_c(const uint8_t *key, void **schedule);
void aes_free_schedule_c(void *schedule);
void aes128_enc_c(const uint8_t *plaintext, const void *schedule, uint8_t *ciphertext);
void aes128_dec_c(const uint8_t *ciphertext, const void *schedule, uint8_t *plaintext);
void aes256_enc_c(const uint8_t *plaintext, const void *schedule, uint8_t *ciphertext);
void aes256_dec_c(const uint8_t *ciphertext, const void *schedule, uint8_t *plaintext);

#endif
