#ifndef PACK_UNPACK_H
#define PACK_UNPACK_H

#include <stdio.h>
#include <stdint.h>
#include "SABER_params.h"
#include "poly.h"

void POLT2BS(uint8_t bytes[SABER_SCALEBYTES_KEM], const uint16_t data[SABER_N]);

void BS2POLT(const uint8_t bytes[SABER_SCALEBYTES_KEM], uint16_t data[SABER_N]);

void POL2MSG(uint16_t *message_dec_unpacked, unsigned char *message_dec);

void BS2POLq(const unsigned char *bytes, uint16_t data[SABER_N]);


void POLVECp2BS(uint8_t *bytes, polyvec *data);
void POLVECq2BS(uint8_t *bytes, polyvec *data);
void POLVEC2BS(uint8_t *bytes, polyvec *data, uint16_t modulus);

void BS2POLVECp(const unsigned char *bytes, polyvec *data);
void BS2POLVECq(const unsigned char *bytes, polyvec *data);
void BS2POLVEC(const unsigned char *bytes, polyvec *data, uint16_t modulus);



#endif
