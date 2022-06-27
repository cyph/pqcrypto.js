#include "crypto_aead.h"
#include "encrypt.h"
#include "crypto_uint8.h"
#include "crypto_uint16.h"
#include "crypto_uint32.h"
#include "crypto_uint64.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef FlexAEADv1_H
#define FlexAEADv1_H
// struct definition
#define SUBKEY0 0*BLOCKSIZE
#define SUBKEY1 2*BLOCKSIZE
#define SUBKEY2 4*BLOCKSIZE
#define SUBKEYA 0*BLOCKSIZE
#define SUBKEYB 1*BLOCKSIZE

struct FlexAEADv1;
// function definition
void FlexAEADv1_init(struct FlexAEADv1 * self, unsigned char *key);
void mwc32( unsigned char * state, unsigned char * add, unsigned blocklen);
void dirMixQuartersLayer( unsigned char * block, unsigned blocklen, unsigned char * state );
void dirShuffleLayer( unsigned char * block, unsigned blocklen, unsigned char * state );
void invShuffleLayer( unsigned char * block, unsigned blocklen, unsigned char * state );
void dirSBoxLayer( unsigned char * block, unsigned blocklen );
void invSBoxLayer( unsigned char * block, unsigned blocklen );
void dirPFK( unsigned char * block, unsigned blocklen, unsigned char *key_pfk,  unsigned nRounds, unsigned char * state );
void invPFK( unsigned char * block, unsigned blocklen, unsigned char *key_pfk,  unsigned nRounds, unsigned char * state );
void padBlock( unsigned char * block, unsigned blocklen, unsigned nBytes );
unsigned unpadBlock( unsigned char * block, unsigned blocklen );
void sumAD( struct FlexAEADv1 * self, unsigned char * ADblock, unsigned doublePFK );
void encryptBlock( struct FlexAEADv1 * self, unsigned char * block );
void decryptBlock( struct FlexAEADv1 * self, unsigned char * block );
int crypto_aead_encrypt( unsigned char *c,unsigned long long *clen, const unsigned char *m,unsigned long long mlen, 
const unsigned char *ad,unsigned long long adlen, const unsigned char *nsec, const unsigned char *npub, const unsigned char *k );
int crypto_aead_decrypt(unsigned char *m,unsigned long long *mlen,unsigned char *nsec,const unsigned char *c,unsigned long long clen,
const unsigned char *ad,unsigned long long adlen,const unsigned char *npub,const unsigned char *k);
void memcpyopt( void * a, const void * b, unsigned c);


#endif

struct FlexAEADv1 {
   unsigned char subkeys[BLOCKSIZE * 6];
   unsigned char counter[BLOCKSIZE];
   unsigned char checksum[BLOCKSIZE];
   unsigned char state[BLOCKSIZE];
   unsigned char sn[BLOCKSIZE];
   unsigned char add[BLOCKSIZE];
   unsigned nRounds;
   unsigned nBytes;
}; 

const unsigned char dirSBox0[256] = {
    0x63,0x7C,0x77,0x7B,0xF2,0x6B,0x6F,0xC5,0x30,0x01,0x67,0x2B,0xFE,0xD7,0xAB,0x76,
    0xCA,0x82,0xC9,0x7D,0xFA,0x59,0x47,0xF0,0xAD,0xD4,0xA2,0xAF,0x9C,0xA4,0x72,0xC0,
    0xB7,0xFD,0x93,0x26,0x36,0x3F,0xF7,0xCC,0x34,0xA5,0xE5,0xF1,0x71,0xD8,0x31,0x15,
    0x04,0xC7,0x23,0xC3,0x18,0x96,0x05,0x9A,0x07,0x12,0x80,0xE2,0xEB,0x27,0xB2,0x75,
    0x09,0x83,0x2C,0x1A,0x1B,0x6E,0x5A,0xA0,0x52,0x3B,0xD6,0xB3,0x29,0xE3,0x2F,0x84,
    0x53,0xD1,0x00,0xED,0x20,0xFC,0xB1,0x5B,0x6A,0xCB,0xBE,0x39,0x4A,0x4C,0x58,0xCF,
    0xD0,0xEF,0xAA,0xFB,0x43,0x4D,0x33,0x85,0x45,0xF9,0x02,0x7F,0x50,0x3C,0x9F,0xA8,
    0x51,0xA3,0x40,0x8F,0x92,0x9D,0x38,0xF5,0xBC,0xB6,0xDA,0x21,0x10,0xFF,0xF3,0xD2,
    0xCD,0x0C,0x13,0xEC,0x5F,0x97,0x44,0x17,0xC4,0xA7,0x7E,0x3D,0x64,0x5D,0x19,0x73,
    0x60,0x81,0x4F,0xDC,0x22,0x2A,0x90,0x88,0x46,0xEE,0xB8,0x14,0xDE,0x5E,0x0B,0xDB,
    0xE0,0x32,0x3A,0x0A,0x49,0x06,0x24,0x5C,0xC2,0xD3,0xAC,0x62,0x91,0x95,0xE4,0x79,
    0xE7,0xC8,0x37,0x6D,0x8D,0xD5,0x4E,0xA9,0x6C,0x56,0xF4,0xEA,0x65,0x7A,0xAE,0x08,
    0xBA,0x78,0x25,0x2E,0x1C,0xA6,0xB4,0xC6,0xE8,0xDD,0x74,0x1F,0x4B,0xBD,0x8B,0x8A,
    0x70,0x3E,0xB5,0x66,0x48,0x03,0xF6,0x0E,0x61,0x35,0x57,0xB9,0x86,0xC1,0x1D,0x9E,
    0xE1,0xF8,0x98,0x11,0x69,0xD9,0x8E,0x94,0x9B,0x1E,0x87,0xE9,0xCE,0x55,0x28,0xDF,
    0x8C,0xA1,0x89,0x0D,0xBF,0xE6,0x42,0x68,0x41,0x99,0x2D,0x0F,0xB0,0x54,0xBB,0x16
};

const unsigned char invSBox0[256] = {
    0x52,0x09,0x6A,0xD5,0x30,0x36,0xA5,0x38,0xBF,0x40,0xA3,0x9E,0x81,0xF3,0xD7,0xFB,
    0x7C,0xE3,0x39,0x82,0x9B,0x2F,0xFF,0x87,0x34,0x8E,0x43,0x44,0xC4,0xDE,0xE9,0xCB,
    0x54,0x7B,0x94,0x32,0xA6,0xC2,0x23,0x3D,0xEE,0x4C,0x95,0x0B,0x42,0xFA,0xC3,0x4E,
    0x08,0x2E,0xA1,0x66,0x28,0xD9,0x24,0xB2,0x76,0x5B,0xA2,0x49,0x6D,0x8B,0xD1,0x25,
    0x72,0xF8,0xF6,0x64,0x86,0x68,0x98,0x16,0xD4,0xA4,0x5C,0xCC,0x5D,0x65,0xB6,0x92,
    0x6C,0x70,0x48,0x50,0xFD,0xED,0xB9,0xDA,0x5E,0x15,0x46,0x57,0xA7,0x8D,0x9D,0x84,
    0x90,0xD8,0xAB,0x00,0x8C,0xBC,0xD3,0x0A,0xF7,0xE4,0x58,0x05,0xB8,0xB3,0x45,0x06, 
    0xD0,0x2C,0x1E,0x8F,0xCA,0x3F,0x0F,0x02,0xC1,0xAF,0xBD,0x03,0x01,0x13,0x8A,0x6B,
    0x3A,0x91,0x11,0x41,0x4F,0x67,0xDC,0xEA,0x97,0xF2,0xCF,0xCE,0xF0,0xB4,0xE6,0x73,
    0x96,0xAC,0x74,0x22,0xE7,0xAD,0x35,0x85,0xE2,0xF9,0x37,0xE8,0x1C,0x75,0xDF,0x6E,
    0x47,0xF1,0x1A,0x71,0x1D,0x29,0xC5,0x89,0x6F,0xB7,0x62,0x0E,0xAA,0x18,0xBE,0x1B,
    0xFC,0x56,0x3E,0x4B,0xC6,0xD2,0x79,0x20,0x9A,0xDB,0xC0,0xFE,0x78,0xCD,0x5A,0xF4,
    0x1F,0xDD,0xA8,0x33,0x88,0x07,0xC7,0x31,0xB1,0x12,0x10,0x59,0x27,0x80,0xEC,0x5F,
    0x60,0x51,0x7F,0xA9,0x19,0xB5,0x4A,0x0D,0x2D,0xE5,0x7A,0x9F,0x93,0xC9,0x9C,0xEF,
    0xA0,0xE0,0x3B,0x4D,0xAE,0x2A,0xF5,0xB0,0xC8,0xEB,0xBB,0x3C,0x83,0x53,0x99,0x61,
    0x17,0x2B,0x04,0x7E,0xBA,0x77,0xD6,0x26,0xE1,0x69,0x14,0x63,0x55,0x21,0x0C,0x7D
};

const unsigned char dirSBox1[256] = {
    0x95,0xA8,0x6C,0xC4,0x69,0x1F,0x3D,0xEC,0x8C,0xF8,0xB7,0x31,0xC1,0x3F,0x29,0x56,
    0x7E,0xD4,0x44,0xE0,0xE3,0x86,0xC7,0xF3,0xD8,0xF0,0xC0,0x0B,0xAC,0x4C,0x74,0xA1,
    0x60,0xC3,0x35,0x34,0x7D,0x87,0x2F,0x98,0xAE,0x97,0x1C,0x49,0xBC,0xA5,0xA6,0x1A,
    0x33,0xDF,0x27,0x55,0x58,0x03,0xDA,0x6E,0x09,0x48,0x1E,0x78,0x02,0x88,0x8F,0xDE,
    0x6F,0x53,0xD9,0x5E,0xA2,0xBD,0x22,0x61,0xE1,0xE2,0x9C,0x21,0xC8,0xCE,0x13,0x9F,
    0x08,0x75,0x94,0x16,0x36,0xD5,0xFB,0x40,0x01,0x79,0xEA,0x3A,0x6B,0xF2,0x52,0xE7,
    0xC6,0xBA,0xD7,0xA7,0xAB,0xB0,0xF5,0xFA,0x73,0x2B,0xB9,0x38,0x32,0xFE,0x68,0x9B,
    0xDB,0xAA,0x7B,0x43,0x37,0x9E,0x04,0x7A,0x39,0x1D,0x1B,0xD1,0xFF,0x64,0x57,0x2D,
    0xE8,0xFD,0x91,0x66,0xB3,0x59,0x17,0x7F,0x0E,0xDC,0x81,0x12,0x4E,0xA9,0xEF,0xF9,
    0xAF,0xCD,0x2E,0x80,0x76,0x62,0xCF,0x14,0x3B,0x8A,0x5F,0x2C,0xB1,0x41,0xF7,0xD6,
    0x5B,0x71,0x82,0xCA,0x15,0x3E,0x54,0x5C,0x23,0x4F,0xB5,0xFC,0xC5,0x7C,0x18,0xCC,
    0xB8,0x2A,0x84,0xD3,0x4D,0x4A,0x25,0xF6,0x8D,0x89,0x26,0x00,0x11,0x4B,0xCB,0xF1,
    0x3C,0xDD,0x65,0x28,0xB4,0x96,0xEB,0xBF,0xED,0x83,0x07,0x9A,0xC2,0x8E,0x45,0x72,
    0xE6,0x93,0xAD,0xBE,0xE4,0x9D,0x24,0x19,0x46,0xE9,0x20,0x47,0x0C,0x06,0x92,0xE5,
    0xB2,0xBB,0x6D,0x30,0x85,0x42,0x99,0x0D,0xA3,0x5A,0x77,0x8B,0x5D,0x0F,0x05,0xEE,
    0xA4,0x50,0xB6,0x70,0xD2,0x51,0xD0,0x90,0xA0,0x63,0x0A,0x67,0xF4,0x6A,0xC9,0x10
};

const unsigned char invSBox1[256] = {
    0xBB,0x58,0x3C,0x35,0x76,0xEE,0xDD,0xCA,0x50,0x38,0xFA,0x1B,0xDC,0xE7,0x88,0xED,
    0xFF,0xBC,0x8B,0x4E,0x97,0xA4,0x53,0x86,0xAE,0xD7,0x2F,0x7A,0x2A,0x79,0x3A,0x05,
    0xDA,0x4B,0x46,0xA8,0xD6,0xB6,0xBA,0x32,0xC3,0x0E,0xB1,0x69,0x9B,0x7F,0x92,0x26,
    0xE3,0x0B,0x6C,0x30,0x23,0x22,0x54,0x74,0x6B,0x78,0x5B,0x98,0xC0,0x06,0xA5,0x0D,
    0x57,0x9D,0xE5,0x73,0x12,0xCE,0xD8,0xDB,0x39,0x2B,0xB5,0xBD,0x1D,0xB4,0x8C,0xA9,
    0xF1,0xF5,0x5E,0x41,0xA6,0x33,0x0F,0x7E,0x34,0x85,0xE9,0xA0,0xA7,0xEC,0x43,0x9A,
    0x20,0x47,0x95,0xF9,0x7D,0xC2,0x83,0xFB,0x6E,0x04,0xFD,0x5C,0x02,0xE2,0x37,0x40,
    0xF3,0xA1,0xCF,0x68,0x1E,0x51,0x94,0xEA,0x3B,0x59,0x77,0x72,0xAD,0x24,0x10,0x87,
    0x93,0x8A,0xA2,0xC9,0xB2,0xE4,0x15,0x25,0x3D,0xB9,0x99,0xEB,0x08,0xB8,0xCD,0x3E,
    0xF7,0x82,0xDE,0xD1,0x52,0x00,0xC5,0x29,0x27,0xE6,0xCB,0x6F,0x4A,0xD5,0x75,0x4F,
    0xF8,0x1F,0x44,0xE8,0xF0,0x2D,0x2E,0x63,0x01,0x8D,0x71,0x64,0x1C,0xD2,0x28,0x90,
    0x65,0x9C,0xE0,0x84,0xC4,0xAA,0xF2,0x0A,0xB0,0x6A,0x61,0xE1,0x2C,0x45,0xD3,0xC7,
    0x1A,0x0C,0xCC,0x21,0x03,0xAC,0x60,0x16,0x4C,0xFE,0xA3,0xBE,0xAF,0x91,0x4D,0x96,
    0xF6,0x7B,0xF4,0xB3,0x11,0x55,0x9F,0x62,0x18,0x42,0x36,0x70,0x89,0xC1,0x3F,0x31,
    0x13,0x48,0x49,0x14,0xD4,0xDF,0xD0,0x5F,0x80,0xD9,0x5A,0xC6,0x07,0xC8,0xEF,0x8E,
    0x19,0xBF,0x5D,0x17,0xFC,0x66,0xB7,0x9E,0x09,0x8F,0x67,0x56,0xAB,0x81,0x6D,0x7C
};

const unsigned char dirSBox2[256] = {
    0xA6,0x9D,0x5F,0x08,0x3E,0x7B,0xF1,0xB0,0x8E,0xEC,0x2C,0x0C,0x69,0xB6,0xAD,0xED,
    0xB2,0x60,0xE7,0xF8,0xE3,0x39,0x97,0x11,0x41,0xDB,0xAE,0x27,0x23,0x3F,0x67,0x51,
    0xC8,0xB3,0xA1,0x4B,0x62,0xA9,0x89,0x2E,0x04,0x20,0x0D,0x72,0x5A,0x26,0x19,0x7C,
    0x55,0x36,0x18,0x1B,0xC6,0xD4,0x66,0x0A,0x00,0x34,0x0E,0x74,0x22,0xB9,0x5D,0xD3,
    0xF5,0xCD,0x48,0x84,0x25,0x73,0x50,0x14,0xC4,0x43,0x45,0x6F,0x31,0xE8,0x86,0xE9,
    0xF7,0x7A,0xE5,0xD6,0x17,0x32,0xCC,0xE0,0xD8,0xC2,0xE6,0x35,0x79,0x29,0xAF,0x77,
    0x3B,0x90,0xEE,0x12,0xF9,0x02,0x1C,0xBA,0x96,0xDE,0xFB,0xA4,0xA2,0xCB,0x94,0xA3,
    0x91,0x57,0x8B,0x3C,0xF2,0x2F,0xCF,0x61,0x80,0xE4,0x4D,0x9C,0x5B,0x15,0x78,0xB1,
    0x0F,0xAB,0x13,0xA7,0xB5,0x44,0xB7,0x70,0x03,0x83,0x4C,0x98,0xDD,0x4F,0xFF,0x8A,
    0xF3,0xFA,0x30,0x4E,0x33,0xD0,0x42,0xD5,0x6D,0x5C,0x81,0x95,0xD2,0x2B,0x01,0x99,
    0x6A,0x56,0xAC,0xB4,0x07,0xCA,0x9E,0xEF,0x1A,0xEA,0x88,0xC1,0x93,0x8D,0xE1,0x7D,
    0xFD,0xA5,0xF0,0x3A,0xE2,0xB8,0x0B,0xC5,0x49,0x6E,0x05,0x71,0x46,0x1F,0x2A,0x8F,
    0x68,0xF6,0xD9,0x38,0x82,0x47,0xFC,0x7E,0x09,0x37,0xF4,0x1D,0x9F,0xA0,0xA8,0x52,
    0xDA,0x24,0xFE,0x75,0x6C,0xBC,0xC3,0x63,0xC0,0x9B,0x10,0xBD,0xBF,0x1E,0x40,0x4A,
    0x59,0x16,0x5E,0xBB,0x54,0xC7,0xEB,0x64,0x8C,0x9A,0x06,0x3D,0x76,0x28,0x21,0xBE,
    0xD1,0x85,0x87,0xAA,0x53,0xCE,0xDF,0x65,0x58,0xDC,0x7F,0xD7,0xC9,0x6B,0x2D,0x92
};

const unsigned char invSBox2[256] = {
    0x38,0x9E,0x65,0x88,0x28,0xBA,0xEA,0xA4,0x03,0xC8,0x37,0xB6,0x0B,0x2A,0x3A,0x80,
    0xDA,0x17,0x63,0x82,0x47,0x7D,0xE1,0x54,0x32,0x2E,0xA8,0x33,0x66,0xCB,0xDD,0xBD,
    0x29,0xEE,0x3C,0x1C,0xD1,0x44,0x2D,0x1B,0xED,0x5D,0xBE,0x9D,0x0A,0xFE,0x27,0x75,
    0x92,0x4C,0x55,0x94,0x39,0x5B,0x31,0xC9,0xC3,0x15,0xB3,0x60,0x73,0xEB,0x04,0x1D,
    0xDE,0x18,0x96,0x49,0x85,0x4A,0xBC,0xC5,0x42,0xB8,0xDF,0x23,0x8A,0x7A,0x93,0x8D,
    0x46,0x1F,0xCF,0xF4,0xE4,0x30,0xA1,0x71,0xF8,0xE0,0x2C,0x7C,0x99,0x3E,0xE2,0x02,
    0x11,0x77,0x24,0xD7,0xE7,0xF7,0x36,0x1E,0xC0,0x0C,0xA0,0xFD,0xD4,0x98,0xB9,0x4B,
    0x87,0xBB,0x2B,0x45,0x3B,0xD3,0xEC,0x5F,0x7E,0x5C,0x51,0x05,0x2F,0xAF,0xC7,0xFA,
    0x78,0x9A,0xC4,0x89,0x43,0xF1,0x4E,0xF2,0xAA,0x26,0x8F,0x72,0xE8,0xAD,0x08,0xBF,
    0x61,0x70,0xFF,0xAC,0x6E,0x9B,0x68,0x16,0x8B,0x9F,0xE9,0xD9,0x7B,0x01,0xA6,0xCC,
    0xCD,0x22,0x6C,0x6F,0x6B,0xB1,0x00,0x83,0xCE,0x25,0xF3,0x81,0xA2,0x0E,0x1A,0x5E,
    0x07,0x7F,0x10,0x21,0xA3,0x84,0x0D,0x86,0xB5,0x3D,0x67,0xE3,0xD5,0xDB,0xEF,0xDC,
    0xD8,0xAB,0x59,0xD6,0x48,0xB7,0x34,0xE5,0x20,0xFC,0xA5,0x6D,0x56,0x41,0xF5,0x76,
    0x95,0xF0,0x9C,0x3F,0x35,0x97,0x53,0xFB,0x58,0xC2,0xD0,0x19,0xF9,0x8C,0x69,0xF6,
    0x57,0xAE,0xB4,0x14,0x79,0x52,0x5A,0x12,0x4D,0x4F,0xA9,0xE6,0x09,0x0F,0x62,0xA7,
    0xB2,0x06,0x74,0x90,0xCA,0x40,0xC1,0x50,0x13,0x64,0x91,0x6A,0xC6,0xB0,0xD2,0x8E
};

const unsigned char dirSBox3[256] = {
    0xD9,0xEE,0x83,0xB5,0xF4,0x02,0xEF,0x64,0x8E,0x4D,0x34,0x48,0xC2,0x29,0xC6,0x90,
    0xB3,0x9F,0x52,0x22,0x2F,0xE7,0xD0,0x76,0x95,0x8D,0xA1,0x2B,0x56,0xD7,0x7D,0x1C,
    0x2D,0x9A,0x3B,0x12,0xDD,0x00,0x24,0xA2,0x63,0x11,0x07,0x94,0x5D,0xF6,0x0E,0x7F,
    0xFF,0x5E,0xF3,0x65,0xE5,0xF1,0xA0,0x93,0x1E,0xBC,0xDE,0xA9,0x8B,0xF5,0xFA,0xB2,
    0x62,0x7E,0xB9,0x57,0x69,0x4C,0xFD,0x43,0x1A,0x08,0x35,0x05,0xE6,0x88,0xA5,0x44,
    0x45,0x01,0xBD,0x5B,0xB6,0xCC,0xBE,0xD3,0x9B,0x9E,0x8F,0x40,0x32,0xC3,0x8A,0x3E,
    0x0B,0x58,0xDB,0x99,0x0D,0xE1,0x87,0xB8,0x06,0x0F,0x0C,0x66,0xA4,0xFE,0x3D,0x10,
    0xFB,0xBB,0x6B,0x53,0x5A,0xC1,0x20,0x42,0x31,0x7C,0xCF,0xE0,0x89,0xE2,0x6C,0x09,
    0x04,0x17,0xCB,0xC0,0xE9,0xAC,0x5F,0x4E,0x81,0x8C,0x13,0xBA,0x0A,0xCE,0x55,0x23,
    0x38,0x4B,0xF0,0x79,0x6E,0x21,0xB7,0x82,0x46,0xD1,0x71,0xBF,0x26,0x86,0xD6,0x2E,
    0x97,0xC9,0x74,0xA6,0x2A,0x98,0x59,0xDA,0xAF,0x78,0x92,0x28,0x6A,0x6D,0x1D,0x4F,
    0xF8,0x61,0x7A,0x60,0xF2,0x6F,0x15,0xC4,0xED,0x16,0xD4,0xEA,0x70,0xCD,0xEB,0xDC,
    0xB0,0x77,0x19,0x3A,0xD8,0x5C,0xF9,0x27,0x72,0x50,0xC5,0x3C,0x37,0xE3,0xA8,0xAA,
    0xF7,0x2C,0x73,0x1F,0x33,0x75,0xC7,0x68,0x67,0x36,0x4A,0x96,0xAB,0xEC,0xFC,0x1B,
    0xC8,0x7B,0xE8,0xA3,0x80,0xB4,0x9C,0xAE,0x18,0x41,0xD5,0xE4,0x25,0x51,0x14,0x49,
    0xAD,0x3F,0xCA,0x91,0xD2,0xA7,0x84,0x9D,0x30,0xDF,0x85,0x47,0x03,0x39,0xB1,0x54
};

const unsigned char invSBox3[256] = {
    0x25,0x51,0x05,0xFC,0x80,0x4B,0x68,0x2A,0x49,0x7F,0x8C,0x60,0x6A,0x64,0x2E,0x69,
    0x6F,0x29,0x23,0x8A,0xEE,0xB6,0xB9,0x81,0xE8,0xC2,0x48,0xDF,0x1F,0xAE,0x38,0xD3,
    0x76,0x95,0x13,0x8F,0x26,0xEC,0x9C,0xC7,0xAB,0x0D,0xA4,0x1B,0xD1,0x20,0x9F,0x14,
    0xF8,0x78,0x5C,0xD4,0x0A,0x4A,0xD9,0xCC,0x90,0xFD,0xC3,0x22,0xCB,0x6E,0x5F,0xF1,
    0x5B,0xE9,0x77,0x47,0x4F,0x50,0x98,0xFB,0x0B,0xEF,0xDA,0x91,0x45,0x09,0x87,0xAF,
    0xC9,0xED,0x12,0x73,0xFF,0x8E,0x1C,0x43,0x61,0xA6,0x74,0x53,0xC5,0x2C,0x31,0x86,
    0xB3,0xB1,0x40,0x28,0x07,0x33,0x6B,0xD8,0xD7,0x44,0xAC,0x72,0x7E,0xAD,0x94,0xB5,
    0xBC,0x9A,0xC8,0xD2,0xA2,0xD5,0x17,0xC1,0xA9,0x93,0xB2,0xE1,0x79,0x1E,0x41,0x2F,
    0xE4,0x88,0x97,0x02,0xF6,0xFA,0x9D,0x66,0x4D,0x7C,0x5E,0x3C,0x89,0x19,0x08,0x5A,
    0x0F,0xF3,0xAA,0x37,0x2B,0x18,0xDB,0xA0,0xA5,0x63,0x21,0x58,0xE6,0xF7,0x59,0x11,
    0x36,0x1A,0x27,0xE3,0x6C,0x4E,0xA3,0xF5,0xCE,0x3B,0xCF,0xDC,0x85,0xF0,0xE7,0xA8,
    0xC0,0xFE,0x3F,0x10,0xE5,0x03,0x54,0x96,0x67,0x42,0x8B,0x71,0x39,0x52,0x56,0x9B,
    0x83,0x75,0x0C,0x5D,0xB7,0xCA,0x0E,0xD6,0xE0,0xA1,0xF2,0x82,0x55,0xBD,0x8D,0x7A,
    0x16,0x99,0xF4,0x57,0xBA,0xEA,0x9E,0x1D,0xC4,0x00,0xA7,0x62,0xBF,0x24,0x3A,0xF9,
    0x7B,0x65,0x7D,0xCD,0xEB,0x34,0x4C,0x15,0xE2,0x84,0xBB,0xBE,0xDD,0xB8,0x01,0x06,
    0x92,0x35,0xB4,0x32,0x04,0x3D,0x2D,0xD0,0xB0,0xC6,0x3E,0x70,0xDE,0x46,0x6D,0x30
};

int crypto_aead_encrypt(
unsigned char *c,unsigned long long *clen,
const unsigned char *m,unsigned long long mlen,
const unsigned char *ad,unsigned long long adlen,
const unsigned char *nsec,
const unsigned char *npub,
const unsigned char *k
)
{
/*
...
... the code for the cipher implementation goes here,
... generating a ciphertext c[0],c[1],...,c[*clen-1]
... from a plaintext m[0],m[1],...,m[mlen-1]
... and associated data ad[0],ad[1],...,ad[adlen-1]
... and secret message number nsec[0],nsec[1],...
... and public message number npub[0],npub[1],...
... and secret key k[0],k[1],...
...
*/
	
	unsigned long long i;

	unsigned blocklen;
	unsigned char state[(BLOCKSIZE>KEYSIZE)?BLOCKSIZE:KEYSIZE];
	unsigned char tag[BLOCKSIZE];
	struct FlexAEADv1 flexaeadv1;

	if(nsec == NULL) {}; // avoid compiling warnings
	
	//memcpy(state,k,KEYSIZE);
    switch(KEYSIZE)
    {
#if KEYSIZE == 32
        case 32:
            *(((crypto_uint64 *) state)+0) = *(((crypto_uint64 *) k)+0) ;
            *(((crypto_uint64 *) state)+1) = *(((crypto_uint64 *) k)+1) ;
            *(((crypto_uint64 *) state)+2) = *(((crypto_uint64 *) k)+2) ;
            *(((crypto_uint64 *) state)+3) = *(((crypto_uint64 *) k)+3) ;
            break;
#endif    
#if KEYSIZE >= 16  
        case 16:
            *(((crypto_uint64 *) state)+0) = *(((crypto_uint64 *) k)+0) ;
            *(((crypto_uint64 *) state)+1) = *(((crypto_uint64 *) k)+1) ;
            break;
#endif    
        case 8:
            *(((crypto_uint64 *) state)+0) = *(((crypto_uint64 *) k)+0) ;
            break;
    }
	FlexAEADv1_init( &flexaeadv1, state );

	// ### reset the counter and checksum	
	//memcpy(flexaeadv1.counter, npub, NONCESIZE);
    switch(NONCESIZE)
    {
#if NONCESIZE == 32
        case 32:
            *(((crypto_uint64 *) flexaeadv1.counter)+0) = *(((crypto_uint64 *) npub)+0) ;
            *(((crypto_uint64 *) flexaeadv1.counter)+1) = *(((crypto_uint64 *) npub)+1) ;
            *(((crypto_uint64 *) flexaeadv1.counter)+2) = *(((crypto_uint64 *) npub)+2) ;
            *(((crypto_uint64 *) flexaeadv1.counter)+3) = *(((crypto_uint64 *) npub)+3) ;
            break;
#endif    
#if NONCESIZE >= 16  
        case 16:
            *(((crypto_uint64 *) flexaeadv1.counter)+0) = *(((crypto_uint64 *) npub)+0) ;
            *(((crypto_uint64 *) flexaeadv1.counter)+1) = *(((crypto_uint64 *) npub)+1) ;
            break;
#endif    
        case 8:
            *(((crypto_uint64 *) flexaeadv1.counter)+0) = *(((crypto_uint64 *) npub)+0) ;
            break;
    }
	
	dirPFK( flexaeadv1.counter, flexaeadv1.nBytes, (flexaeadv1.subkeys + SUBKEY2),  flexaeadv1.nRounds, flexaeadv1.state );
    
    switch(NONCESIZE)
    {
#if NONCESIZE == 32
        case 32:
            *(((crypto_uint64 *) flexaeadv1.add)+0) = *(((crypto_uint64 *) flexaeadv1.counter)+0) ;
            *(((crypto_uint64 *) flexaeadv1.add)+1) = *(((crypto_uint64 *) flexaeadv1.counter)+1) ;
            *(((crypto_uint64 *) flexaeadv1.add)+2) = *(((crypto_uint64 *) flexaeadv1.counter)+2) ;
            *(((crypto_uint64 *) flexaeadv1.add)+3) = *(((crypto_uint64 *) flexaeadv1.counter)+3) ;
            break;
#endif    
#if NONCESIZE >= 16  
        case 16:
            *(((crypto_uint64 *) flexaeadv1.add)+0) = *(((crypto_uint64 *) flexaeadv1.counter)+0) ;
            *(((crypto_uint64 *) flexaeadv1.add)+1) = *(((crypto_uint64 *) flexaeadv1.counter)+1) ;
            break;
#endif    
        case 8:
            *(((crypto_uint64 *) flexaeadv1.add)+0) = *(((crypto_uint64 *) flexaeadv1.counter)+0) ;
            break;
    }
    for( i = 0; i < flexaeadv1.nBytes; i += 4)
    {
        if( *((crypto_uint32 *) (flexaeadv1.add+i)) == 0 ) 
        {
            *((crypto_uint32 *) (flexaeadv1.add+i)) = 0x11111111;
        }
    }

	dirPFK( flexaeadv1.counter, flexaeadv1.nBytes, (flexaeadv1.subkeys + SUBKEY2),  flexaeadv1.nRounds, flexaeadv1.state );
	
	// ### calculate ciphertext length
	if( (mlen%flexaeadv1.nBytes)==0)
		*clen = mlen;
	else
		*clen = ((mlen/flexaeadv1.nBytes)+1)*flexaeadv1.nBytes;
	*clen += TAGSIZE;
	
	// ### calculate the checksum from the AD  
	i = 0;
	while( (i+flexaeadv1.nBytes) <= adlen)
	{
		//memcpy( state, ad+i, flexaeadv1.nBytes);
        *(((crypto_uint64 *) state)+0) = *(((crypto_uint64 *) (ad+i))+0);
#if BLOCKSIZE >= 16  
        *(((crypto_uint64 *) state)+1) = *(((crypto_uint64 *) (ad+i))+1);
#endif
#if BLOCKSIZE == 32
        *(((crypto_uint64 *) state)+2) = *(((crypto_uint64 *) (ad+i))+2);
        *(((crypto_uint64 *) state)+3) = *(((crypto_uint64 *) (ad+i))+3);
#endif    
		mwc32( flexaeadv1.counter, flexaeadv1.add, flexaeadv1.nBytes );
		sumAD( &flexaeadv1, state, 0);
		i += flexaeadv1.nBytes;
	}
	if(i<adlen)
	{
		//memset(state, 0x00, flexaeadv1.nBytes);
        *(((crypto_uint64 *) state)+0) = 0;        
#if BLOCKSIZE >= 16  
        *(((crypto_uint64 *) state)+1) = 0;
#endif
#if BLOCKSIZE == 32
        *(((crypto_uint64 *) state)+2) = 0;
        *(((crypto_uint64 *) state)+3) = 0;
#endif    
        state[adlen-i]=0x80;
		memcpyopt(state,ad+i,(adlen-i));
		mwc32( flexaeadv1.counter, flexaeadv1.add, flexaeadv1.nBytes );
		sumAD( &flexaeadv1, state, 1);
	}

	// ### separation in between AD and M
	mwc32( flexaeadv1.counter, flexaeadv1.add, flexaeadv1.nBytes );
    for( i = 0; i<flexaeadv1.nBytes; i+=8)
    {
        *(((crypto_uint64 *) (flexaeadv1.checksum+i))) ^= *(((crypto_uint64 *) (flexaeadv1.counter+i)));
    }
    
	// ### encrypt the plaintext and calclulate the tag 
	i = 0;
	while( i+flexaeadv1.nBytes <= mlen)
	{
		//memcpy(c+i,m+i,flexaeadv1.nBytes);
        *(((crypto_uint64 *) (c+i))+0) = *(((crypto_uint64 *) (m+i))+0);
#if BLOCKSIZE >= 16  
        *(((crypto_uint64 *) (c+i))+1) = *(((crypto_uint64 *) (m+i))+1);
#endif
#if BLOCKSIZE == 32
        *(((crypto_uint64 *) (c+i))+2) = *(((crypto_uint64 *) (m+i))+2);
        *(((crypto_uint64 *) (c+i))+3) = *(((crypto_uint64 *) (m+i))+3);
#endif    
		mwc32( flexaeadv1.counter, flexaeadv1.add, flexaeadv1.nBytes );
		encryptBlock( &flexaeadv1, c+i);
		i += flexaeadv1.nBytes;
	}
	if(i==mlen)
	{
        *(((crypto_uint64 *) tag)+0) = *(((crypto_uint64 *) flexaeadv1.checksum)+0)^0xAAAAAAAAAAAAAAAA;
#if BLOCKSIZE >= 16  
        *(((crypto_uint64 *) tag)+1) = *(((crypto_uint64 *) flexaeadv1.checksum)+1)^0xAAAAAAAAAAAAAAAA;
#endif    
#if BLOCKSIZE == 32  
        *(((crypto_uint64 *) tag)+2) = *(((crypto_uint64 *) flexaeadv1.checksum)+2)^0xAAAAAAAAAAAAAAAA;
        *(((crypto_uint64 *) tag)+3) = *(((crypto_uint64 *) flexaeadv1.checksum)+3)^0xAAAAAAAAAAAAAAAA;
#endif    
	}
	else
	{
		blocklen = (unsigned char) (mlen-i);
		memcpyopt(c+i, m+i, blocklen);
		padBlock(c+i,blocklen, flexaeadv1.nBytes);
		mwc32( flexaeadv1.counter, flexaeadv1.add, flexaeadv1.nBytes );
		encryptBlock( &flexaeadv1, c+i);
        *(((crypto_uint64 *) tag)+0) = *(((crypto_uint64 *) flexaeadv1.checksum)+0)^0x5555555555555555;
#if BLOCKSIZE >= 16  
        *(((crypto_uint64 *) tag)+1) = *(((crypto_uint64 *) flexaeadv1.checksum)+1)^0x5555555555555555;
#endif    
#if BLOCKSIZE == 32  
        *(((crypto_uint64 *) tag)+2) = *(((crypto_uint64 *) flexaeadv1.checksum)+2)^0x5555555555555555;
        *(((crypto_uint64 *) tag)+3) = *(((crypto_uint64 *) flexaeadv1.checksum)+3)^0x5555555555555555;
#endif    
	}
	dirPFK( tag, flexaeadv1.nBytes, (flexaeadv1.subkeys + SUBKEY0),  flexaeadv1.nRounds, flexaeadv1.state );
	//memcpyopt( c+(*clen-TAGSIZE),tag,TAGSIZE);
    *(((crypto_uint64 *) (c+(*clen-TAGSIZE)))+0) = *(((crypto_uint64 *) tag)+0);
#if TAGSIZE >= 16  
    *(((crypto_uint64 *) (c+(*clen-TAGSIZE)))+1) = *(((crypto_uint64 *) tag)+1);
#endif    
#if TAGSIZE == 32  
    *(((crypto_uint64 *) (c+(*clen-TAGSIZE)))+2) = *(((crypto_uint64 *) tag)+2);
    *(((crypto_uint64 *) (c+(*clen-TAGSIZE)))+3) = *(((crypto_uint64 *) tag)+3);
#endif    
	return 0;
};

int crypto_aead_decrypt(
unsigned char *m,unsigned long long *mlen,
unsigned char *nsec,
const unsigned char *c,unsigned long long clen,
const unsigned char *ad,unsigned long long adlen,
const unsigned char *npub,
const unsigned char *k
)
{
/*
...
... the code for the cipher implementation goes here,
... generating a plaintext m[0],m[1],...,m[*mlen-1]
... and secret message number nsec[0],nsec[1],...
... from a ciphertext c[0],c[1],...,c[clen-1]
... and associated data ad[0],ad[1],...,ad[adlen-1]
... and public message number npub[0],npub[1],...
... and secret key k[0],k[1],...        
...
*/
	unsigned long long i;

	unsigned blocklen;
	unsigned char  state[(BLOCKSIZE>KEYSIZE)?BLOCKSIZE:KEYSIZE];
	unsigned char  tag[BLOCKSIZE];
	unsigned char  tag1[BLOCKSIZE];
	
	struct FlexAEADv1 flexaeadv1;

	if(nsec == NULL) {}; // avoid compiling warnings
	
	//memcpy(state,k,KEYSIZE);
    switch(KEYSIZE)
    {
#if KEYSIZE == 32
        case 32:
            *(((crypto_uint64 *) state)+0) = *(((crypto_uint64 *) k)+0) ;
            *(((crypto_uint64 *) state)+1) = *(((crypto_uint64 *) k)+1) ;
            *(((crypto_uint64 *) state)+2) = *(((crypto_uint64 *) k)+2) ;
            *(((crypto_uint64 *) state)+3) = *(((crypto_uint64 *) k)+3) ;
            break;
#endif    
#if KEYSIZE >= 16  
        case 16:
            *(((crypto_uint64 *) state)+0) = *(((crypto_uint64 *) k)+0) ;
            *(((crypto_uint64 *) state)+1) = *(((crypto_uint64 *) k)+1) ;
            break;
#endif    
        case 8:
            *(((crypto_uint64 *) state)+0) = *(((crypto_uint64 *) k)+0) ;
            break;
    }
	FlexAEADv1_init( &flexaeadv1, state );

	// ### reset the counter	
	//memcpy(flexaeadv1.counter, npub, NONCESIZE);
    switch(NONCESIZE)
    {
#if NONCESIZE == 32
        case 32:
            *(((crypto_uint64 *) flexaeadv1.counter)+0) = *(((crypto_uint64 *) npub)+0) ;
            *(((crypto_uint64 *) flexaeadv1.counter)+1) = *(((crypto_uint64 *) npub)+1) ;
            *(((crypto_uint64 *) flexaeadv1.counter)+2) = *(((crypto_uint64 *) npub)+2) ;
            *(((crypto_uint64 *) flexaeadv1.counter)+3) = *(((crypto_uint64 *) npub)+3) ;
            break;
#endif    
#if NONCESIZE >= 16  
        case 16:
            *(((crypto_uint64 *) flexaeadv1.counter)+0) = *(((crypto_uint64 *) npub)+0) ;
            *(((crypto_uint64 *) flexaeadv1.counter)+1) = *(((crypto_uint64 *) npub)+1) ;
            break;
#endif    
        case 8:
            *(((crypto_uint64 *) flexaeadv1.counter)+0) = *(((crypto_uint64 *) npub)+0) ;
            break;
    }
	dirPFK( flexaeadv1.counter, flexaeadv1.nBytes, (flexaeadv1.subkeys + SUBKEY2),  flexaeadv1.nRounds, flexaeadv1.state );
    switch(NONCESIZE)
    {
#if NONCESIZE == 32
        case 32:
            *(((crypto_uint64 *) flexaeadv1.add)+0) = *(((crypto_uint64 *) flexaeadv1.counter)+0) ;
            *(((crypto_uint64 *) flexaeadv1.add)+1) = *(((crypto_uint64 *) flexaeadv1.counter)+1) ;
            *(((crypto_uint64 *) flexaeadv1.add)+2) = *(((crypto_uint64 *) flexaeadv1.counter)+2) ;
            *(((crypto_uint64 *) flexaeadv1.add)+3) = *(((crypto_uint64 *) flexaeadv1.counter)+3) ;
            break;
#endif    
#if NONCESIZE >= 16  
        case 16:
            *(((crypto_uint64 *) flexaeadv1.add)+0) = *(((crypto_uint64 *) flexaeadv1.counter)+0) ;
            *(((crypto_uint64 *) flexaeadv1.add)+1) = *(((crypto_uint64 *) flexaeadv1.counter)+1) ;
            break;
#endif    
        case 8:
            *(((crypto_uint64 *) flexaeadv1.add)+0) = *(((crypto_uint64 *) flexaeadv1.counter)+0) ;
            break;
    }
    for( i = 0; i < flexaeadv1.nBytes; i += 4)
    {
        if( *((crypto_uint32 *) (flexaeadv1.add+i)) == 0 ) 
        {
            *((crypto_uint32 *) (flexaeadv1.add+i)) = 0x11111111;
        }
    }
    
	dirPFK( flexaeadv1.counter, flexaeadv1.nBytes, (flexaeadv1.subkeys + SUBKEY2),  flexaeadv1.nRounds, flexaeadv1.state );
		
	// ### remove the tag from ciphertext
	*mlen = clen;
	*mlen -= TAGSIZE;
	memcpyopt(tag,c+*mlen,TAGSIZE);
	
	// ### calculate the checksum from the AD  
	i = 0;
	while( i+flexaeadv1.nBytes <= adlen)
	{
		//memcpy( state, ad+i, flexaeadv1.nBytes);
        *(((crypto_uint64 *) state)+0) = *(((crypto_uint64 *) (ad+i))+0);
#if BLOCKSIZE >= 16  
        *(((crypto_uint64 *) state)+1) = *(((crypto_uint64 *) (ad+i))+1);
#endif
#if BLOCKSIZE == 32
        *(((crypto_uint64 *) state)+2) = *(((crypto_uint64 *) (ad+i))+2);
        *(((crypto_uint64 *) state)+3) = *(((crypto_uint64 *) (ad+i))+3);
#endif    
		mwc32( flexaeadv1.counter, flexaeadv1.add, flexaeadv1.nBytes );
		sumAD( &flexaeadv1, state, 0);
		i += flexaeadv1.nBytes;
	}
	if(i<adlen)
	{
		//memset(state, 0x00, flexaeadv1.nBytes);
        *(((crypto_uint64 *) state)+0) = 0;        
#if BLOCKSIZE >= 16  
        *(((crypto_uint64 *) state)+1) = 0;
#endif
#if BLOCKSIZE == 32
        *(((crypto_uint64 *) state)+2) = 0;
        *(((crypto_uint64 *) state)+3) = 0;
#endif    
		state[adlen-i]=0x80;
		memcpyopt(state,ad+i,(adlen-i));
		mwc32( flexaeadv1.counter, flexaeadv1.add, flexaeadv1.nBytes );
		sumAD( &flexaeadv1, state, 1);
	}
	
	
	// ### separation in between AD and M
    mwc32( flexaeadv1.counter, flexaeadv1.add, flexaeadv1.nBytes );
    for( i = 0; i<flexaeadv1.nBytes; i+=8)
    {
        *(((crypto_uint64 *) (flexaeadv1.checksum+i))) ^= *(((crypto_uint64 *) (flexaeadv1.counter+i)));
    }

	// ### decrypt the ciphertext and calclulate the tag 
	i = 0;
	while( i < *mlen)
	{
		//memcpy(m+i,c+i,flexaeadv1.nBytes);
        *(((crypto_uint64 *) (m+i))+0) = *(((crypto_uint64 *) (c+i))+0);
#if BLOCKSIZE >= 16  
        *(((crypto_uint64 *) (m+i))+1) = *(((crypto_uint64 *) (c+i))+1);
#endif
#if BLOCKSIZE == 32
        *(((crypto_uint64 *) (m+i))+2) = *(((crypto_uint64 *) (c+i))+2);
        *(((crypto_uint64 *) (m+i))+3) = *(((crypto_uint64 *) (c+i))+3);
#endif    
		mwc32( flexaeadv1.counter, flexaeadv1.add, flexaeadv1.nBytes );
		decryptBlock( &flexaeadv1, m+i);
		i += flexaeadv1.nBytes;
	}
	
    *(((crypto_uint64 *) tag1)+0) = *(((crypto_uint64 *) flexaeadv1.checksum)+0)^0xAAAAAAAAAAAAAAAA;
#if BLOCKSIZE >= 16  
    *(((crypto_uint64 *) tag1)+1) = *(((crypto_uint64 *) flexaeadv1.checksum)+1)^0xAAAAAAAAAAAAAAAA;
#endif    
#if BLOCKSIZE == 32  
    *(((crypto_uint64 *) tag1)+2) = *(((crypto_uint64 *) flexaeadv1.checksum)+2)^0xAAAAAAAAAAAAAAAA;
    *(((crypto_uint64 *) tag1)+3) = *(((crypto_uint64 *) flexaeadv1.checksum)+3)^0xAAAAAAAAAAAAAAAA;
#endif    
	dirPFK( tag1, flexaeadv1.nBytes, (flexaeadv1.subkeys + SUBKEY0),  flexaeadv1.nRounds, flexaeadv1.state );
	
	if(memcmp(tag1,tag,TAGSIZE))
	{
        *(((crypto_uint64 *) tag1)+0) = *(((crypto_uint64 *) flexaeadv1.checksum)+0)^0x5555555555555555;
#if BLOCKSIZE >= 16  
        *(((crypto_uint64 *) tag1)+1) = *(((crypto_uint64 *) flexaeadv1.checksum)+1)^0x5555555555555555;
#endif    
#if BLOCKSIZE == 32  
        *(((crypto_uint64 *) tag1)+2) = *(((crypto_uint64 *) flexaeadv1.checksum)+2)^0x5555555555555555;
        *(((crypto_uint64 *) tag1)+3) = *(((crypto_uint64 *) flexaeadv1.checksum)+3)^0x5555555555555555;
#endif    
		dirPFK( tag1, flexaeadv1.nBytes, (flexaeadv1.subkeys + SUBKEY0),  flexaeadv1.nRounds, flexaeadv1.state );
		if(memcmp(tag1,tag,TAGSIZE))
		{
			*mlen=0;
			return -1;
		}
		blocklen = flexaeadv1.nBytes;
		*mlen -= blocklen;
		blocklen = unpadBlock( m+*mlen, flexaeadv1.nBytes);
		*mlen += blocklen;
	}
	return 0;
};

void FlexAEADv1_init(struct FlexAEADv1 * self, unsigned char *key )
{
	unsigned char keystate[KEYSIZE];
	unsigned long long i;
	
	//memset((*self).checksum,0x00,BLOCKSIZE);
    *(((crypto_uint64 *) (*self).checksum)+0) = 0;
#if BLOCKSIZE >= 16  
    *(((crypto_uint64 *) (*self).checksum)+1) = 0;
#endif    
#if BLOCKSIZE == 32  
    *(((crypto_uint64 *) (*self).checksum)+2) = 0;
    *(((crypto_uint64 *) (*self).checksum)+3) = 0;
#endif    
    
	
	(*self).nBytes = BLOCKSIZE;
	(*self).nRounds = 0;
	i = 1;
	while( i < (KEYSIZE/2) )
	{
		i <<= 1;
		(*self).nRounds ++;
	}
	
	i=0;
	while( i < (*self).nBytes*6)
	{
		if(i==0)
        {
            //memset((*self).subkeys,0x00,KEYSIZE/2);
            *(((crypto_uint64 *) ((*self).subkeys))+0) = 0;
#if KEYSIZE == 32  
            *(((crypto_uint64 *) ((*self).subkeys))+1) = 0;
#endif    
        }
		else
        {
			//memcpy( (*self).subkeys+i, ((*self).subkeys+i-(KEYSIZE/2)), (KEYSIZE/2));	
#if KEYSIZE == 16  
            *(((crypto_uint64 *) ((*self).subkeys+i))+0) = *(((crypto_uint64 *) ((*self).subkeys+i-8))+0);
#endif    
#if KEYSIZE == 32  
            *(((crypto_uint64 *) ((*self).subkeys+i))+0) = *(((crypto_uint64 *) ((*self).subkeys+i-16))+0);
            *(((crypto_uint64 *) ((*self).subkeys+i))+1) = *(((crypto_uint64 *) ((*self).subkeys+i-16))+1);
#endif    
        }
		dirPFK( ((*self).subkeys+i), (KEYSIZE/2), key, (*self).nRounds, keystate );
		dirPFK( ((*self).subkeys+i), (KEYSIZE/2), key, (*self).nRounds, keystate );
		dirPFK( ((*self).subkeys+i), (KEYSIZE/2), key, (*self).nRounds, keystate );
		i += (KEYSIZE/2);
	}

	(*self).nRounds = 0;
	i = 1;
	while( i < BLOCKSIZE )
	{
		i <<= 1;
		(*self).nRounds ++;
	}
};


inline void memcpyopt( void * a, const void * b, unsigned c)
{
    unsigned i = 0;
    while( i < c )
    {
        if( (i+8) <= c)
        {
            *((crypto_uint64 *) (a+i)) = *((crypto_uint64 *) (b+i));
            i+=8;
        }
        else if( (i+4) <= c)
        {
            *((crypto_uint32 *) (a+i)) = *((crypto_uint32 *) (b+i));
            i+=4;
        }
        else if( (i+2) <= c)
        {
            *((crypto_uint16 *) (a+i)) = *((crypto_uint16 *) (b+i));
            i+=2;
        }
        else
        {
            *((crypto_uint8 *) (a+i)) = *((crypto_uint8 *) (b+i));
            i+=1;
        }
     
    }
};


inline void mwc32( unsigned char * state, unsigned char * add, unsigned blocklen)
{
    crypto_uint64 t;
    for( unsigned  i = 0; i < blocklen; i += 4)
    {
        t = 4294967220 * (crypto_uint64) (*((crypto_uint32 *) (state+i))) + *((crypto_uint32 *) (add+i));
        *((crypto_uint32 *) (add+i)) = (crypto_uint32) (t>>32);
        *((crypto_uint32 *) (state+i))  = (t&0xFFFFFFFF);
    }
    return;
}


inline void dirSBoxLayer( unsigned char * block, unsigned blocklen )
{
	//unsigned i = 0;
	//for( i=0*(blocklen/4); i<1*(blocklen/4); i++ )
	//	*(block+i) = dirSBox0[ *(block+i) ];
	//for( i=1*(blocklen/4); i<2*(blocklen/4); i++ )
	//	*(block+i) = dirSBox1[ *(block+i) ];
	//for( i=2*(blocklen/4); i<3*(blocklen/4); i++ )
	//	*(block+i) = dirSBox2[ *(block+i) ];
	//for( i=3*(blocklen/4); i<4*(blocklen/4); i++ )
	//	*(block+i) = dirSBox3[ *(block+i) ];
    switch(blocklen)
    {
        case 8:
            *(block+0) = dirSBox0[ *(block+0) ];
            *(block+1) = dirSBox0[ *(block+1) ];
            *(block+2) = dirSBox1[ *(block+2) ];
            *(block+3) = dirSBox1[ *(block+3) ];
            *(block+4) = dirSBox2[ *(block+4) ];
            *(block+5) = dirSBox2[ *(block+5) ];
            *(block+6) = dirSBox3[ *(block+6) ];
            *(block+7) = dirSBox3[ *(block+7) ];
            break;
#if BLOCKSIZE >= 16  
        case 16:
            *(block+0) = dirSBox0[ *(block+0) ];
            *(block+1) = dirSBox0[ *(block+1) ];
            *(block+2) = dirSBox0[ *(block+2) ];
            *(block+3) = dirSBox0[ *(block+3) ];
            *(block+4) = dirSBox1[ *(block+4) ];
            *(block+5) = dirSBox1[ *(block+5) ];
            *(block+6) = dirSBox1[ *(block+6) ];
            *(block+7) = dirSBox1[ *(block+7) ];
            *(block+8) = dirSBox2[ *(block+8) ];
            *(block+9) = dirSBox2[ *(block+9) ];
            *(block+10) = dirSBox2[ *(block+10) ];
            *(block+11) = dirSBox2[ *(block+11) ];
            *(block+12) = dirSBox3[ *(block+12) ];
            *(block+13) = dirSBox3[ *(block+13) ];
            *(block+14) = dirSBox3[ *(block+14) ];
            *(block+15) = dirSBox3[ *(block+15) ];
            break;
#endif    
#if BLOCKSIZE == 32  
        case 32:
            *(block+0) = dirSBox0[ *(block+0) ];
            *(block+1) = dirSBox0[ *(block+1) ];
            *(block+2) = dirSBox0[ *(block+2) ];
            *(block+3) = dirSBox0[ *(block+3) ];
            *(block+4) = dirSBox0[ *(block+4) ];
            *(block+5) = dirSBox0[ *(block+5) ];
            *(block+6) = dirSBox0[ *(block+6) ];
            *(block+7) = dirSBox0[ *(block+7) ];
            
            *(block+8) = dirSBox1[ *(block+8) ];
            *(block+9) = dirSBox1[ *(block+9) ];
            *(block+10) = dirSBox1[ *(block+10) ];
            *(block+11) = dirSBox1[ *(block+11) ];
            *(block+12) = dirSBox1[ *(block+12) ];
            *(block+13) = dirSBox1[ *(block+13) ];
            *(block+14) = dirSBox1[ *(block+14) ];
            *(block+15) = dirSBox1[ *(block+15) ];
            
            *(block+16) = dirSBox2[ *(block+16) ];
            *(block+17) = dirSBox2[ *(block+17) ];
            *(block+18) = dirSBox2[ *(block+18) ];
            *(block+19) = dirSBox2[ *(block+19) ];
            *(block+20) = dirSBox2[ *(block+20) ];
            *(block+21) = dirSBox2[ *(block+21) ];
            *(block+22) = dirSBox2[ *(block+22) ];
            *(block+23) = dirSBox2[ *(block+23) ];
            
            *(block+24) = dirSBox3[ *(block+24) ];
            *(block+25) = dirSBox3[ *(block+25) ];
            *(block+26) = dirSBox3[ *(block+26) ];
            *(block+27) = dirSBox3[ *(block+27) ];
            *(block+28) = dirSBox3[ *(block+28) ];
            *(block+29) = dirSBox3[ *(block+29) ];
            *(block+30) = dirSBox3[ *(block+30) ];
            *(block+31) = dirSBox3[ *(block+31) ];
            break;
#endif
    }
};

inline void invSBoxLayer( unsigned char * block, unsigned blocklen )
{
	//unsigned i = 0;
	//for( i=0*(blocklen/4); i<1*(blocklen/4); i++ )
	//	*(block+i) = invSBox0[ *(block+i) ];
	//for( i=1*(blocklen/4); i<2*(blocklen/4); i++ )
	//	*(block+i) = invSBox1[ *(block+i) ];
	//for( i=2*(blocklen/4); i<3*(blocklen/4); i++ )
	//	*(block+i) = invSBox2[ *(block+i) ];
	//for( i=3*(blocklen/4); i<4*(blocklen/4); i++ )
	//	*(block+i) = invSBox3[ *(block+i) ];
    switch(blocklen)
    {
        case 8:
            *(block+0) = invSBox0[ *(block+0) ];
            *(block+1) = invSBox0[ *(block+1) ];
            *(block+2) = invSBox1[ *(block+2) ];
            *(block+3) = invSBox1[ *(block+3) ];
            *(block+4) = invSBox2[ *(block+4) ];
            *(block+5) = invSBox2[ *(block+5) ];
            *(block+6) = invSBox3[ *(block+6) ];
            *(block+7) = invSBox3[ *(block+7) ];
            break;
#if BLOCKSIZE >= 16  
        case 16:
            *(block+0) = invSBox0[ *(block+0) ];
            *(block+1) = invSBox0[ *(block+1) ];
            *(block+2) = invSBox0[ *(block+2) ];
            *(block+3) = invSBox0[ *(block+3) ];
            *(block+4) = invSBox1[ *(block+4) ];
            *(block+5) = invSBox1[ *(block+5) ];
            *(block+6) = invSBox1[ *(block+6) ];
            *(block+7) = invSBox1[ *(block+7) ];
            *(block+8) = invSBox2[ *(block+8) ];
            *(block+9) = invSBox2[ *(block+9) ];
            *(block+10) = invSBox2[ *(block+10) ];
            *(block+11) = invSBox2[ *(block+11) ];
            *(block+12) = invSBox3[ *(block+12) ];
            *(block+13) = invSBox3[ *(block+13) ];
            *(block+14) = invSBox3[ *(block+14) ];
            *(block+15) = invSBox3[ *(block+15) ];
            break;
#endif    
#if BLOCKSIZE == 32  
        case 32:
            *(block+0) = invSBox0[ *(block+0) ];
            *(block+1) = invSBox0[ *(block+1) ];
            *(block+2) = invSBox0[ *(block+2) ];
            *(block+3) = invSBox0[ *(block+3) ];
            *(block+4) = invSBox0[ *(block+4) ];
            *(block+5) = invSBox0[ *(block+5) ];
            *(block+6) = invSBox0[ *(block+6) ];
            *(block+7) = invSBox0[ *(block+7) ];
            
            *(block+8) = invSBox1[ *(block+8) ];
            *(block+9) = invSBox1[ *(block+9) ];
            *(block+10) = invSBox1[ *(block+10) ];
            *(block+11) = invSBox1[ *(block+11) ];
            *(block+12) = invSBox1[ *(block+12) ];
            *(block+13) = invSBox1[ *(block+13) ];
            *(block+14) = invSBox1[ *(block+14) ];
            *(block+15) = invSBox1[ *(block+15) ];
            
            *(block+16) = invSBox2[ *(block+16) ];
            *(block+17) = invSBox2[ *(block+17) ];
            *(block+18) = invSBox2[ *(block+18) ];
            *(block+19) = invSBox2[ *(block+19) ];
            *(block+20) = invSBox2[ *(block+20) ];
            *(block+21) = invSBox2[ *(block+21) ];
            *(block+22) = invSBox2[ *(block+22) ];
            *(block+23) = invSBox2[ *(block+23) ];
            
            *(block+24) = invSBox3[ *(block+24) ];
            *(block+25) = invSBox3[ *(block+25) ];
            *(block+26) = invSBox3[ *(block+26) ];
            *(block+27) = invSBox3[ *(block+27) ];
            *(block+28) = invSBox3[ *(block+28) ];
            *(block+29) = invSBox3[ *(block+29) ];
            *(block+30) = invSBox3[ *(block+30) ];
            *(block+31) = invSBox3[ *(block+31) ];
            break;
#endif
    }
};

inline void dirMixQuartersLayer( unsigned char * block, unsigned blocklen, unsigned char * state )
{
    switch(blocklen)
    {
        case 8:
            *(((crypto_uint16 *)state)+3) = *(((crypto_uint16 *)block)+0)^
                                            *(((crypto_uint16 *)block)+1)^
                                            *(((crypto_uint16 *)block)+2)^
                                            *(((crypto_uint16 *)block)+3);
            *(((crypto_uint16 *)state)+0) = *(((crypto_uint16 *)state)+3)^
                                            *(((crypto_uint16 *)block)+0);
            *(((crypto_uint16 *)state)+1) = *(((crypto_uint16 *)state)+3)^
                                            *(((crypto_uint16 *)block)+1);
            *(((crypto_uint16 *)state)+2) = *(((crypto_uint16 *)state)+3)^
                                            *(((crypto_uint16 *)block)+2);
            *(((crypto_uint16 *)state)+3) = *(((crypto_uint16 *)state)+3)^
                                            *(((crypto_uint16 *)block)+3);
            break;
#if BLOCKSIZE >= 16  
        case 16:
            *(((crypto_uint32 *)state)+3) = *(((crypto_uint32 *)block)+0)^
                                            *(((crypto_uint32 *)block)+1)^
                                            *(((crypto_uint32 *)block)+2)^
                                            *(((crypto_uint32 *)block)+3);
            *(((crypto_uint32 *)state)+0) = *(((crypto_uint32 *)state)+3)^
                                            *(((crypto_uint32 *)block)+0);
            *(((crypto_uint32 *)state)+1) = *(((crypto_uint32 *)state)+3)^
                                            *(((crypto_uint32 *)block)+1);
            *(((crypto_uint32 *)state)+2) = *(((crypto_uint32 *)state)+3)^
                                            *(((crypto_uint32 *)block)+2);
            *(((crypto_uint32 *)state)+3) = *(((crypto_uint32 *)state)+3)^
                                            *(((crypto_uint32 *)block)+3);
            break;
#endif
#if BLOCKSIZE >= 32
        case 32:
            *(((crypto_uint64 *)state)+3) = *(((crypto_uint64 *)block)+0)^
                                            *(((crypto_uint64 *)block)+1)^
                                            *(((crypto_uint64 *)block)+2)^
                                            *(((crypto_uint64 *)block)+3);
            *(((crypto_uint64 *)state)+0) = *(((crypto_uint64 *)state)+3)^
                                            *(((crypto_uint64 *)block)+0);
            *(((crypto_uint64 *)state)+1) = *(((crypto_uint64 *)state)+3)^
                                            *(((crypto_uint64 *)block)+1);
            *(((crypto_uint64 *)state)+2) = *(((crypto_uint64 *)state)+3)^
                                            *(((crypto_uint64 *)block)+2);
            *(((crypto_uint64 *)state)+3) = *(((crypto_uint64 *)state)+3)^
                                            *(((crypto_uint64 *)block)+3);
            break;
#endif
    }
	memcpyopt( block, state, blocklen);
	return;
}

inline void dirShuffleLayer( unsigned char * block, unsigned blocklen, unsigned char * state )
{
	//unsigned i = 0;
	//for( i=0; i<blocklen/2; i++)
	//{
	//	*(state+(2*i+0)) = *(block+(0*(blocklen/2)+i));
	//	*(state+(2*i+1)) = *(block+(1*(blocklen/2)+i));
	//}
    crypto_uint64 x;
    switch(blocklen)
    {
        case 8:
            x = *(((crypto_uint64 *) block)+0);
            *(((crypto_uint64 *) state)+0) = ((x&0xff)<<0) // *(state+(0)) = *(block+(0))
                                            + ((x&0xff00000000)>>24) // *(state+(1)) = *(block+(4))
                                            + ((x&0xff00)<<8) // *(state+(2)) = *(block+(1))
                                            + ((x&0xff0000000000)>>16) // *(state+(3)) = *(block+(5))
                                            + ((x&0xff0000)<<16) // *(state+(4)) = *(block+(2))
                                            + ((x&0xff000000000000)>>8) // *(state+(5)) = *(block+(6))
                                            + ((x&0xff000000)<<24) // *(state+(6)) = *(block+(3))
                                            + ((x&0xff00000000000000)<<0); // *(state+(7)) = *(block+(7))
            *(((crypto_uint64 *) block)+0) = *(((crypto_uint64 *) state)+0);
        /*
            *(state+(0)) = *(block+(0));
            *(state+(1)) = *(block+(4));
            *(state+(2)) = *(block+(1));
            *(state+(3)) = *(block+(5));
            *(state+(4)) = *(block+(2));
            *(state+(5)) = *(block+(6));
            *(state+(6)) = *(block+(3));
            *(state+(7)) = *(block+(7));
        */
            /*
            *(state+(2*0+0)) = *(block+(0*4+0));
            *(state+(2*0+1)) = *(block+(1*4+0));
            *(state+(2*1+0)) = *(block+(0*4+1));
            *(state+(2*1+1)) = *(block+(1*4+1));
            *(state+(2*2+0)) = *(block+(0*4+2));
            *(state+(2*2+1)) = *(block+(1*4+2));
            *(state+(2*3+0)) = *(block+(0*4+3));
            *(state+(2*3+1)) = *(block+(1*4+3));
            */
            break;
#if BLOCKSIZE >= 16
        case 16:
            x = *(((crypto_uint64 *) block)+0);
            *(((crypto_uint64 *) state)+0) = ((x&0xff)<<0) // *(state+(0)) = *(block+(0))
                                            + ((x&0xff00)<<8) // *(state+(2)) = *(block+(1))
                                            + ((x&0xff0000)<<16) // *(state+(4)) = *(block+(2))
                                            + ((x&0xff000000)<<24); // *(state+(6)) = *(block+(3))

            *(((crypto_uint64 *) state)+1) =  ((x&0xff00000000)>>32) // *(state+(8+0)) = *(block+(4))
                                            + ((x&0xff0000000000)>>24) // *(state+(8+2)) = *(block+(5))
                                            + ((x&0xff000000000000)>>16) // *(state+(8+4)) = *(block+(6))
                                            + ((x&0xff00000000000000)>>8); // *(state+(8+6)) = *(block+(7))
            /*
            *(state+(0)) = *(block+(0));
            *(state+(2)) = *(block+(1));
            *(state+(4)) = *(block+(2));
            *(state+(6)) = *(block+(3));
            
            *(state+(8)) = *(block+(4));
            *(state+(10)) = *(block+(5));
            *(state+(12)) = *(block+(6));
            *(state+(14)) = *(block+(7));
            */

            x = *(((crypto_uint64 *) block)+1);
            *(((crypto_uint64 *) state)+0) += ((x&0xff)<<8) // *(state+(1)) = *(block+(8+0))
                                            + ((x&0xff00)<<16) // *(state+(3)) = *(block+(8+1))
                                            + ((x&0xff0000)<<24) // *(state+(5)) = *(block+(8+2))
                                            + ((x&0xff000000)<<32); // *(state+(7)) = *(block+(8+3))

            *(((crypto_uint64 *) state)+1) += ((x&0xff00000000)>>24) // *(state+(8+1)) = *(block+(8+4))
                                            + ((x&0xff0000000000)>>16) // *(state+(8+3)) = *(block+(8+5))
                                            + ((x&0xff000000000000)>>8) // *(state+(8+5)) = *(block+(8+6))
                                            + ((x&0xff00000000000000)>>0); // *(state+(8+8)) = *(block+(8+7))
            /*
            *(state+(1)) = *(block+(8));
            *(state+(3)) = *(block+(9));
            *(state+(5)) = *(block+(10));
            *(state+(7)) = *(block+(11));
            
            *(state+(9)) = *(block+(12));
            *(state+(11)) = *(block+(13));
            *(state+(13)) = *(block+(14));
            *(state+(15)) = *(block+(15));
            */
            *(((crypto_uint64 *) block)+0) = *(((crypto_uint64 *) state)+0);
            *(((crypto_uint64 *) block)+1) = *(((crypto_uint64 *) state)+1);
            /*
            *(state+(2*0+0)) = *(block+(0*8+0));
            *(state+(2*0+1)) = *(block+(1*8+0));
            *(state+(2*1+0)) = *(block+(0*8+1));
            *(state+(2*1+1)) = *(block+(1*8+1));
            *(state+(2*2+0)) = *(block+(0*8+2));
            *(state+(2*2+1)) = *(block+(1*8+2));
            *(state+(2*3+0)) = *(block+(0*8+3));
            *(state+(2*3+1)) = *(block+(1*8+3));
            *(state+(2*4+0)) = *(block+(0*8+4));
            *(state+(2*4+1)) = *(block+(1*8+4));
            *(state+(2*5+0)) = *(block+(0*8+5));
            *(state+(2*5+1)) = *(block+(1*8+5));
            *(state+(2*6+0)) = *(block+(0*8+6));
            *(state+(2*6+1)) = *(block+(1*8+6));
            *(state+(2*7+0)) = *(block+(0*8+7));
            *(state+(2*7+1)) = *(block+(1*8+7));
            */
            break;
#endif
#if BLOCKSIZE >= 32
        case 32:
            x = *(((crypto_uint64 *) block)+0);
            *(((crypto_uint64 *) state)+0)  = ((x&0xff)>>0)
                                            + ((x&0xff00)<<8)
                                            + ((x&0xff0000)<<16)
                                            + ((x&0xff000000)<<24);
            *(((crypto_uint64 *) state)+1)  = ((x&0xff00000000)>>32)
                                            + ((x&0xff0000000000)>>24)
                                            + ((x&0xff000000000000)>>16)
                                            + ((x&0xff00000000000000)>>8);
            /*
            *(state+(8*0+0)) = *(block+(00+0));
            *(state+(8*0+2)) = *(block+(00+1));
            *(state+(8*0+4)) = *(block+(00+2));
            *(state+(8*0+6)) = *(block+(00+3));
            */
            /*
            *(state+(8*1+0)) = *(block+(00+4));
            *(state+(8*1+2)) = *(block+(00+5));
            *(state+(8*1+4)) = *(block+(00+6));
            *(state+(8*1+6)) = *(block+(00+7));
            */
            x = *(((crypto_uint64 *) block)+1);
            *(((crypto_uint64 *) state)+2)  = ((x&0xff)>>0)
                                            + ((x&0xff00)<<8)
                                            + ((x&0xff0000)<<16)
                                            + ((x&0xff000000)<<24);
            *(((crypto_uint64 *) state)+3)  = ((x&0xff00000000)>>32)
                                            + ((x&0xff0000000000)>>24)
                                            + ((x&0xff000000000000)>>16)
                                            + ((x&0xff00000000000000)>>8);
            /*
            *(state+(8*2+0)) = *(block+(08+0));
            *(state+(8*2+2)) = *(block+(08+1));
            *(state+(8*2+4)) = *(block+(08+2));
            *(state+(8*2+6)) = *(block+(08+3));
            */
            /*
            *(state+(8*3+0)) = *(block+(08+4));
            *(state+(8*3+2)) = *(block+(08+5));
            *(state+(8*3+4)) = *(block+(08+6));
            *(state+(8*3+6)) = *(block+(08+7));
            */
            x = *(((crypto_uint64 *) block)+2);
            *(((crypto_uint64 *) state)+0) += ((x&0xff)<<8)
                                            + ((x&0xff00)<<16)
                                            + ((x&0xff0000)<<24)
                                            + ((x&0xff000000)<<32);
            *(((crypto_uint64 *) state)+1) += ((x&0xff00000000)>>24)
                                            + ((x&0xff0000000000)>>16)
                                            + ((x&0xff000000000000)>>8)
                                            + ((x&0xff00000000000000)>>0);
            
            /*
            *(state+(8*0+1)) = *(block+(16+0));
            *(state+(8*0+3)) = *(block+(16+1));
            *(state+(8*0+5)) = *(block+(16+2));
            *(state+(8*0+7)) = *(block+(16+3));
            */
            /*
            *(state+(8*1+1)) = *(block+(16+4));
            *(state+(8*1+3)) = *(block+(16+5));
            *(state+(8*1+5)) = *(block+(16+6));
            *(state+(8*1+7)) = *(block+(16+7));
            */
            x = *(((crypto_uint64 *) block)+3);
            *(((crypto_uint64 *) state)+2) += ((x&0xff)<<8)
                                            + ((x&0xff00)<<16)
                                            + ((x&0xff0000)<<24)
                                            + ((x&0xff000000)<<32);
            *(((crypto_uint64 *) state)+3) += ((x&0xff00000000)>>24)
                                            + ((x&0xff0000000000)>>16)
                                            + ((x&0xff000000000000)>>8)
                                            + ((x&0xff00000000000000)>>0);
            /*
            *(state+(8*2+1)) = *(block+(24+0));
            *(state+(8*2+3)) = *(block+(24+1));
            *(state+(8*2+5)) = *(block+(24+2));
            *(state+(8*2+7)) = *(block+(24+3));
            */
            /*
            *(state+(8*3+1)) = *(block+(24+4));
            *(state+(8*3+3)) = *(block+(24+5));
            *(state+(8*3+5)) = *(block+(24+6));
            *(state+(8*3+7)) = *(block+(24+7));
            */

            
        
            /*
            *(state+(2*0+0)) = *(block+(0*16+0));
            *(state+(2*0+1)) = *(block+(1*16+0));
            *(state+(2*1+0)) = *(block+(0*16+1));
            *(state+(2*1+1)) = *(block+(1*16+1));
            *(state+(2*2+0)) = *(block+(0*16+2));
            *(state+(2*2+1)) = *(block+(1*16+2));
            *(state+(2*3+0)) = *(block+(0*16+3));
            *(state+(2*3+1)) = *(block+(1*16+3));
            
            *(state+(2*4+0)) = *(block+(0*16+4));
            *(state+(2*4+1)) = *(block+(1*16+4));
            *(state+(2*5+0)) = *(block+(0*16+5));
            *(state+(2*5+1)) = *(block+(1*16+5));
            *(state+(2*6+0)) = *(block+(0*16+6));
            *(state+(2*6+1)) = *(block+(1*16+6));
            *(state+(2*7+0)) = *(block+(0*16+7));
            *(state+(2*7+1)) = *(block+(1*16+7));
            *(state+(2*8+0)) = *(block+(0*16+8));
            *(state+(2*8+1)) = *(block+(1*16+8));
            *(state+(2*9+0)) = *(block+(0*16+9));
            *(state+(2*9+1)) = *(block+(1*16+9));
            *(state+(2*10+0)) = *(block+(0*16+10));
            *(state+(2*10+1)) = *(block+(1*16+10));
            *(state+(2*11+0)) = *(block+(0*16+11));
            *(state+(2*11+1)) = *(block+(1*16+11));
            *(state+(2*12+0)) = *(block+(0*16+12));
            *(state+(2*12+1)) = *(block+(1*16+12));
            *(state+(2*13+0)) = *(block+(0*16+13));
            *(state+(2*13+1)) = *(block+(1*16+13));
            *(state+(2*14+0)) = *(block+(0*16+14));
            *(state+(2*14+1)) = *(block+(1*16+14));
            *(state+(2*15+0)) = *(block+(0*16+15));
            *(state+(2*15+1)) = *(block+(1*16+15));
            */
            *(((crypto_uint64 *) block)+0) = *(((crypto_uint64 *) state)+0);
            *(((crypto_uint64 *) block)+1) = *(((crypto_uint64 *) state)+1);
            *(((crypto_uint64 *) block)+2) = *(((crypto_uint64 *) state)+2);
            *(((crypto_uint64 *) block)+3) = *(((crypto_uint64 *) state)+3);
            break;
            
#endif
    }
	//memcpy( block, state, blocklen);
	return;
}

inline void invShuffleLayer( unsigned char * block, unsigned blocklen, unsigned char * state )
{
	//unsigned i = 0;
	//for( i=0; i<blocklen/2; i++)
	//{
    //    *(state+(0*(blocklen/2)+i)) = *(block+(2*i+0));
    //    *(state+(1*(blocklen/2)+i)) = *(block+(2*i+1));
	//}
    //print('switch(blocklen)')
    //print('{')
    //for blocklen in (8,16,32):
    //print('case {}:'.format(blocklen))
    //for i in range(int(blocklen/2)):
    //print('*(state+(0*{}+{})) = *(block+(2*{}+0));'.format(int(blocklen/2),i,i))
    //print('*(state+(1*{}+{})) = *(block+(2*{}+1));'.format(int(blocklen/2),i,i))
    //print('break;')
    //print('}')
    crypto_uint64 x;
    switch(blocklen)
    {
        case 8:
            x = *(((crypto_uint64 *) block)+0);
            *(((crypto_uint64 *) state)+0) = ((x&0xff)<<0) // *(state+(0)) = *(block+(0))
                                            + ((x&0xff0000)>>8) // *(state+(1)) = *(block+(2));
                                            + ((x&0xff00000000)>>16) // *(state+(2)) = *(block+(4));
                                            + ((x&0xff000000000000)>>24) // *(state+(3)) = *(block+(6));            
                                            + ((x&0xff00)<<24) // *(state+(4)) = *(block+(1));
                                            + ((x&0xff000000)<<16) // *(state+(5)) = *(block+(3));
                                            + ((x&0xff0000000000)<<8) // *(state+(6)) = *(block+(5));
                                            + ((x&0xff00000000000000)<<0); // *(state+(7)) = *(block+(7))
        /*
            *(state+(0)) = *(block+(0));
            *(state+(1)) = *(block+(2));
            *(state+(2)) = *(block+(4));
            *(state+(3)) = *(block+(6));            
            *(state+(4)) = *(block+(1));
            *(state+(5)) = *(block+(3));
            *(state+(6)) = *(block+(5));
            *(state+(7)) = *(block+(7));
        */
            *(((crypto_uint64 *) block)+0) = *(((crypto_uint64 *) state)+0);
            break;
#if BLOCKSIZE >= 16
        case 16:
            x = *(((crypto_uint64 *) block)+0);
            *(((crypto_uint64 *) state)+0)  = ((x&0xff)>>0)
                                            + ((x&0xff0000)>>8)
                                            + ((x&0xff00000000)>>16)
                                            + ((x&0xff000000000000)>>24);                                            
            *(((crypto_uint64 *) state)+1)  = ((x&0xff00)>>8)
                                            + ((x&0xff000000)>>16)
                                            + ((x&0xff0000000000)>>24)
                                            + ((x&0xff00000000000000)>>32);
            /*
            *(state+(0*8+0)) = *(block+(0+0));
            *(state+(0*8+1)) = *(block+(0+2));
            *(state+(0*8+2)) = *(block+(0+4));
            *(state+(0*8+3)) = *(block+(0+6));
            
            *(state+(1*8+0)) = *(block+(0+1));
            *(state+(1*8+1)) = *(block+(0+3));
            *(state+(1*8+2)) = *(block+(0+5));
            *(state+(1*8+3)) = *(block+(0+7));
            */
            x = *(((crypto_uint64 *) block)+1);
            *(((crypto_uint64 *) state)+0) += ((x&0xff)<<32)
                                            + ((x&0xff0000)<<24)
                                            + ((x&0xff00000000)<<16)
                                            + ((x&0xff000000000000)<<8);
            *(((crypto_uint64 *) state)+1) += ((x&0xff00)<<24)
                                            + ((x&0xff000000)<<16)
                                            + ((x&0xff0000000000)<<8)
                                            + ((x&0xff00000000000000)<<0);            
            /*
            *(state+(0*8+4)) = *(block+(8+0));
            *(state+(0*8+5)) = *(block+(8+2));
            *(state+(0*8+6)) = *(block+(8+4));
            *(state+(0*8+7)) = *(block+(8+6));
            
            *(state+(1*8+4)) = *(block+(8+1));
            *(state+(1*8+5)) = *(block+(8+3));
            *(state+(1*8+6)) = *(block+(8+5));
            *(state+(1*8+7)) = *(block+(9+7));
            */

            *(((crypto_uint64 *) block)+0) = *(((crypto_uint64 *) state)+0);
            *(((crypto_uint64 *) block)+1) = *(((crypto_uint64 *) state)+1);
            break;
#endif
#if BLOCKSIZE >= 32
        case 32:
            x = *(((crypto_uint64 *) block)+0);
            *(((crypto_uint64 *) state)+0)  = ((x&0xff)>>0)
                                            + ((x&0xff0000)>>8)
                                            + ((x&0xff00000000)>>16)
                                            + ((x&0xff000000000000)>>24);
            *(((crypto_uint64 *) state)+2)  = ((x&0xff00)>>8)
                                            + ((x&0xff000000)>>16)
                                            + ((x&0xff0000000000)>>24)
                                            + ((x&0xff00000000000000)>>32);

            /*
            *(state+(00+0)) = *(block+(00+0));
            *(state+(00+1)) = *(block+(00+2));
            *(state+(00+2)) = *(block+(00+4));
            *(state+(00+3)) = *(block+(00+6));
            
            *(state+(16+0)) = *(block+(00+1));
            *(state+(16+1)) = *(block+(00+3));
            *(state+(16+2)) = *(block+(00+5));
            *(state+(16+3)) = *(block+(00+7));
            */
            
            x = *(((crypto_uint64 *) block)+1);
            *(((crypto_uint64 *) state)+0) += ((x&0xff)<<32)
                                            + ((x&0xff0000)<<24)
                                            + ((x&0xff00000000)<<16)
                                            + ((x&0xff000000000000)<<8);
            *(((crypto_uint64 *) state)+2) += ((x&0xff00)<<24)
                                            + ((x&0xff000000)<<16)
                                            + ((x&0xff0000000000)<<8)
                                            + ((x&0xff00000000000000)<<0);
                                            
            /*
            *(state+(00+4)) = *(block+(08+0));
            *(state+(00+5)) = *(block+(08+2));
            *(state+(00+6)) = *(block+(08+4));
            *(state+(00+7)) = *(block+(08+6));
            
            *(state+(16+4)) = *(block+(08+1));
            *(state+(16+5)) = *(block+(08+3));
            *(state+(16+6)) = *(block+(08+5));
            *(state+(16+7)) = *(block+(08+7));
            */
            
            x = *(((crypto_uint64 *) block)+2);
            *(((crypto_uint64 *) state)+1)  = ((x&0xff)>>0)
                                            + ((x&0xff0000)>>8)
                                            + ((x&0xff00000000)>>16)
                                            + ((x&0xff000000000000)>>24);
            *(((crypto_uint64 *) state)+3)  = ((x&0xff00)>>8)
                                            + ((x&0xff000000)>>16)
                                            + ((x&0xff0000000000)>>24)
                                            + ((x&0xff00000000000000)>>32);

            /*
            *(state+(08+0)) = *(block+(16+0));
            *(state+(08+1)) = *(block+(16+2));
            *(state+(08+2)) = *(block+(16+4));
            *(state+(08+3)) = *(block+(16+6));
            
            *(state+(24+0)) = *(block+(16+1));
            *(state+(24+1)) = *(block+(16+3));
            *(state+(24+2)) = *(block+(16+5));
            *(state+(24+3)) = *(block+(16+7));
            */
            
            x = *(((crypto_uint64 *) block)+3);
            *(((crypto_uint64 *) state)+1) += ((x&0xff)<<32)
                                            + ((x&0xff0000)<<24)
                                            + ((x&0xff00000000)<<16)
                                            + ((x&0xff000000000000)<<8);
            *(((crypto_uint64 *) state)+3) += ((x&0xff00)<<24)
                                            + ((x&0xff000000)<<16)
                                            + ((x&0xff0000000000)<<8)
                                            + ((x&0xff00000000000000)<<0);

            /*
            *(state+(08+4)) = *(block+(24+0));
            *(state+(08+5)) = *(block+(24+2));
            *(state+(08+6)) = *(block+(24+4));
            *(state+(08+7)) = *(block+(24+6));
            
            *(state+(24+4)) = *(block+(24+1));
            *(state+(24+5)) = *(block+(24+3));
            *(state+(24+6)) = *(block+(24+5));
            *(state+(24+7)) = *(block+(24+7));
            */

            *(((crypto_uint64 *) block)+0) = *(((crypto_uint64 *) state)+0);
            *(((crypto_uint64 *) block)+1) = *(((crypto_uint64 *) state)+1);
            *(((crypto_uint64 *) block)+2) = *(((crypto_uint64 *) state)+2);
            *(((crypto_uint64 *) block)+3) = *(((crypto_uint64 *) state)+3);
            break;
#endif
    }
	return;
}

inline void dirPFK( unsigned char * block, unsigned blocklen, unsigned char *key_pfk,  unsigned nRounds, unsigned char * state )
{
	unsigned n = 0;
	// block XOR first half key_pfk
    switch(blocklen)
    {
        case 8:
            *(((crypto_uint64 *) block)+0) ^= *(((crypto_uint64 *)key_pfk)+0);
             break;
#if BLOCKSIZE >= 16  
        case 16:
            *(((crypto_uint64 *) block)+0) ^= *(((crypto_uint64 *)key_pfk)+0);
            *(((crypto_uint64 *) block)+1) ^= *(((crypto_uint64 *)key_pfk)+1);
             break;
#endif             
#if BLOCKSIZE >= 32
        case 32:
            *(((crypto_uint64 *) block)+0) ^= *(((crypto_uint64 *)key_pfk)+0);
            *(((crypto_uint64 *) block)+1) ^= *(((crypto_uint64 *)key_pfk)+1);
            *(((crypto_uint64 *) block)+2) ^= *(((crypto_uint64 *)key_pfk)+2);
            *(((crypto_uint64 *) block)+3) ^= *(((crypto_uint64 *)key_pfk)+3);
            break;
#endif             
    }
	for( n=0; n<nRounds; n++ )
	{
		// Shuffle Layer
		dirShuffleLayer( block, blocklen, state );
		// Mix Quarters Layer
		dirMixQuartersLayer( block, blocklen, state );
		// SBox Layer
		dirSBoxLayer(block,blocklen);
	}
	// block XOR second half key_pfk
    switch(blocklen)
    {
        case 8:
            *(((crypto_uint64 *) block)+0) ^= *(((crypto_uint64 *)key_pfk)+1);
            break;
#if BLOCKSIZE >= 16  
        case 16:
            *(((crypto_uint64 *) block)+0) ^= *(((crypto_uint64 *)key_pfk)+2);
            *(((crypto_uint64 *) block)+1) ^= *(((crypto_uint64 *)key_pfk)+3);
            break;
#endif             
#if BLOCKSIZE >= 32
        case 32:
            *(((crypto_uint64 *) block)+0) ^= *(((crypto_uint64 *)key_pfk)+4);
            *(((crypto_uint64 *) block)+1) ^= *(((crypto_uint64 *)key_pfk)+5);
            *(((crypto_uint64 *) block)+2) ^= *(((crypto_uint64 *)key_pfk)+6);
            *(((crypto_uint64 *) block)+3) ^= *(((crypto_uint64 *)key_pfk)+7);
            break;
#endif             
    }
	return;
};

inline void invPFK( unsigned char * block, unsigned blocklen, unsigned char *key_pfk,  unsigned nRounds, unsigned char * state )
{
	unsigned n = 0;
	// block XOR second half key_pfk
    switch(blocklen)
    {
        case 8:
            *(((crypto_uint64 *) block)+0) ^= *(((crypto_uint64 *)key_pfk)+1);
            break;
#if BLOCKSIZE >= 16
        case 16:
            *(((crypto_uint64 *) block)+0) ^= *(((crypto_uint64 *)key_pfk)+2);
            *(((crypto_uint64 *) block)+1) ^= *(((crypto_uint64 *)key_pfk)+3);
            break;
#endif             
#if BLOCKSIZE >= 32
        case 32:
            *(((crypto_uint64 *) block)+0) ^= *(((crypto_uint64 *)key_pfk)+4);
            *(((crypto_uint64 *) block)+1) ^= *(((crypto_uint64 *)key_pfk)+5);
            *(((crypto_uint64 *) block)+2) ^= *(((crypto_uint64 *)key_pfk)+6);
            *(((crypto_uint64 *) block)+3) ^= *(((crypto_uint64 *)key_pfk)+7);
            break;
#endif             
    }

	for( n=0; n<nRounds; n++ )
	{
		// Inv SBox Layer
		invSBoxLayer(block,blocklen);
		// Mix Quarters Layer
		dirMixQuartersLayer( block, blocklen, state );
		// Inv Shuffle Layer
		invShuffleLayer( block, blocklen, state );
	}

	// block XOR first half key_pfk
    switch(blocklen)
    {
        case 8:
            *(((crypto_uint64 *) block)+0) ^= *(((crypto_uint64 *)key_pfk)+0);
             break;
#if BLOCKSIZE >= 16
        case 16:
            *(((crypto_uint64 *) block)+0) ^= *(((crypto_uint64 *)key_pfk)+0);
            *(((crypto_uint64 *) block)+1) ^= *(((crypto_uint64 *)key_pfk)+1);
             break;
#endif             
#if BLOCKSIZE >= 32
        case 32:
            *(((crypto_uint64 *) block)+0) ^= *(((crypto_uint64 *)key_pfk)+0);
            *(((crypto_uint64 *) block)+1) ^= *(((crypto_uint64 *)key_pfk)+1);
            *(((crypto_uint64 *) block)+2) ^= *(((crypto_uint64 *)key_pfk)+2);
            *(((crypto_uint64 *) block)+3) ^= *(((crypto_uint64 *)key_pfk)+3);
            break;
#endif             
    }
	return;
};

void padBlock( unsigned char * block, unsigned blocklen, unsigned nBytes )
{
	*(block+blocklen) = 0x80; 
	blocklen++;
    while(blocklen<nBytes)
    {
        *(block+blocklen) = 0x00;
        blocklen++;
    }
//	if(blocklen<nBytes)
//		memset(block+blocklen,0x00,nBytes-blocklen);
	return;
};

unsigned unpadBlock( unsigned char * block, unsigned blocklen )
{	
	while(blocklen>0)
	{
		blocklen--;
		if( *(block+blocklen) == 0x80 )
			return blocklen;
	}
	return 0;
};


void sumAD( struct FlexAEADv1 * self, unsigned char * ADblock, unsigned doublePFK )
{
	//memcpy( (*self).sn, (*self).counter, (*self).nBytes);
#if BLOCKSIZE == 32
    *(((crypto_uint64 *) (*self).sn)+3) = *(((crypto_uint64 *) ADblock)+3);
    *(((crypto_uint64 *) (*self).sn)+2) = *(((crypto_uint64 *) ADblock)+2);
#endif    
#if BLOCKSIZE >= 16  
    *(((crypto_uint64 *) (*self).sn)+1) = *(((crypto_uint64 *) ADblock)+1);
#endif
    *(((crypto_uint64 *) (*self).sn)+0) = *(((crypto_uint64 *) ADblock)+0);
    // dirPFK1 
	dirPFK( (*self).sn, (*self).nBytes, ((*self).subkeys + SUBKEY1),  (*self).nRounds, (*self).state );
    if(doublePFK) {
        dirPFK( (*self).sn, (*self).nBytes, ((*self).subkeys + SUBKEY1),  (*self).nRounds, (*self).state );
    }
    // XOR block+sn  -> block    
    *(((crypto_uint64 *) (*self).sn)+0) ^= *(((crypto_uint64 *) (*self).counter)+0);
#if BLOCKSIZE >= 16  
    *(((crypto_uint64 *) (*self).sn)+1) ^= *(((crypto_uint64 *) (*self).counter)+1);
#endif    
#if BLOCKSIZE == 32  
    *(((crypto_uint64 *) (*self).sn)+2) ^= *(((crypto_uint64 *) (*self).counter)+2);
    *(((crypto_uint64 *) (*self).sn)+3) ^= *(((crypto_uint64 *) (*self).counter)+3);
#endif    
    // dir SBox Layer
    dirSBoxLayer((*self).sn,(*self).nBytes);
    // XOR block+checksum -> checksum  
    *(((crypto_uint64 *) (*self).checksum)+0) ^= *(((crypto_uint64 *) (*self).sn)+0);
#if BLOCKSIZE >= 16  
    *(((crypto_uint64 *) (*self).checksum)+1) ^= *(((crypto_uint64 *) (*self).sn)+1);
#endif    
#if BLOCKSIZE == 32  
    *(((crypto_uint64 *) (*self).checksum)+2) ^= *(((crypto_uint64 *) (*self).sn)+2);
    *(((crypto_uint64 *) (*self).checksum)+3) ^= *(((crypto_uint64 *) (*self).sn)+3);
#endif    
	return;
};	

void encryptBlock( struct FlexAEADv1 * self, unsigned char * block )
{
    // dirPFK1 
	dirPFK( block, (*self).nBytes, ((*self).subkeys + SUBKEY1),  (*self).nRounds, (*self).state );
    // XOR block+sn  -> block
    *(((crypto_uint64 *) block)+0) ^= *(((crypto_uint64 *) (*self).counter)+0);
#if BLOCKSIZE >= 16  
    *(((crypto_uint64 *) block)+1) ^= *(((crypto_uint64 *) (*self).counter)+1);
#endif    
#if BLOCKSIZE == 32  
    *(((crypto_uint64 *) block)+2) ^= *(((crypto_uint64 *) (*self).counter)+2);
    *(((crypto_uint64 *) block)+3) ^= *(((crypto_uint64 *) (*self).counter)+3);
#endif
    // dir SBox Layer
    dirSBoxLayer(block,(*self).nBytes);
    // XOR block+checksum -> checksum  
    *(((crypto_uint64 *) (*self).checksum)+0) ^= *(((crypto_uint64 *) block)+0);
#if BLOCKSIZE >= 16  
    *(((crypto_uint64 *) (*self).checksum)+1) ^= *(((crypto_uint64 *) block)+1);
#endif    
#if BLOCKSIZE == 32  
    *(((crypto_uint64 *) (*self).checksum)+2) ^= *(((crypto_uint64 *) block)+2);
    *(((crypto_uint64 *) (*self).checksum)+3) ^= *(((crypto_uint64 *) block)+3);
#endif    
    // dir SBox Layer
    dirSBoxLayer(block,(*self).nBytes);
    // XOR block+sn   -> block 
    *(((crypto_uint64 *) block)+0) ^= *(((crypto_uint64 *) (*self).counter)+0);
#if BLOCKSIZE >= 16  
    *(((crypto_uint64 *) block)+1) ^= *(((crypto_uint64 *) (*self).counter)+1);
#endif    
#if BLOCKSIZE == 32  
    *(((crypto_uint64 *) block)+2) ^= *(((crypto_uint64 *) (*self).counter)+2);
    *(((crypto_uint64 *) block)+3) ^= *(((crypto_uint64 *) (*self).counter)+3);
#endif
    // dirPFK0
	dirPFK( block, (*self).nBytes, ((*self).subkeys + SUBKEY0),  (*self).nRounds, (*self).state );
	return;
};

void decryptBlock( struct FlexAEADv1 * self, unsigned char * block )
{
    // invPFK0 
	invPFK( block, (*self).nBytes, ((*self).subkeys + SUBKEY0),  (*self).nRounds, (*self).state );
    // XOR block+sn -> block
    *(((crypto_uint64 *) block)+0) ^= *(((crypto_uint64 *) (*self).counter)+0);
#if BLOCKSIZE >= 16  
    *(((crypto_uint64 *) block)+1) ^= *(((crypto_uint64 *) (*self).counter)+1);
#endif    
#if BLOCKSIZE == 32  
    *(((crypto_uint64 *) block)+2) ^= *(((crypto_uint64 *) (*self).counter)+2);
    *(((crypto_uint64 *) block)+3) ^= *(((crypto_uint64 *) (*self).counter)+3);
#endif
    // Inv SBox Layer
    invSBoxLayer(block,(*self).nBytes);
    // XOR block+checksum -> checksum
    *(((crypto_uint64 *) (*self).checksum)+0) ^= *(((crypto_uint64 *) block)+0);
#if BLOCKSIZE >= 16  
    *(((crypto_uint64 *) (*self).checksum)+1) ^= *(((crypto_uint64 *) block)+1);
#endif    
#if BLOCKSIZE == 32  
    *(((crypto_uint64 *) (*self).checksum)+2) ^= *(((crypto_uint64 *) block)+2);
    *(((crypto_uint64 *) (*self).checksum)+3) ^= *(((crypto_uint64 *) block)+3);
#endif    
    // Inv SBox Layer
    invSBoxLayer(block,(*self).nBytes);
    // XOR block+sn -> block
    *(((crypto_uint64 *) block)+0) ^= *(((crypto_uint64 *) (*self).counter)+0);
#if BLOCKSIZE >= 16  
    *(((crypto_uint64 *) block)+1) ^= *(((crypto_uint64 *) (*self).counter)+1);
#endif    
#if BLOCKSIZE == 32  
    *(((crypto_uint64 *) block)+2) ^= *(((crypto_uint64 *) (*self).counter)+2);
    *(((crypto_uint64 *) block)+3) ^= *(((crypto_uint64 *) (*self).counter)+3);
#endif
    // invPFK1
	invPFK( block, (*self).nBytes, ((*self).subkeys + SUBKEY1),  (*self).nRounds, (*self).state );
	return;
};


