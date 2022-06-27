#include "crypto_aead.h"
#include "encrypt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef FlexAEADv1_H
#define FlexAEADv1_H
// struct definition
struct FlexAEADv1;
// function definition
void FlexAEADv1_init(struct FlexAEADv1 * self, unsigned char *key);
void dirShuffleLayer( unsigned char * block, unsigned long long blocklen, unsigned char * state );
void invShuffleLayer( unsigned char * block, unsigned long long blocklen, unsigned char * state );
void inc32( unsigned char * block, unsigned long long blocklen, unsigned char inc );
void dirSBoxLayer( unsigned char * block, unsigned long long blocklen );
void invSBoxLayer( unsigned char * block, unsigned long long blocklen );
void dirPFK( unsigned char * block, unsigned long long blocklen, unsigned char *key_pfk,  unsigned long long nRounds, unsigned char * state );
void invPFK( unsigned char * block, unsigned long long blocklen, unsigned char *key_pfk,  unsigned long long nRounds, unsigned char * state );
void padBlock( unsigned char * block, unsigned long long blocklen, unsigned long long nBytes );
unsigned long long unpadBlock( unsigned char * block, unsigned long long blocklen );
void sumAD( struct FlexAEADv1 * self, unsigned char * ADblock );
void encryptBlock( struct FlexAEADv1 * self, unsigned char * block );
void decryptBlock( struct FlexAEADv1 * self, unsigned char * block );
int crypto_aead_encrypt( unsigned char *c,unsigned long long *clen, const unsigned char *m,unsigned long long mlen, 
const unsigned char *ad,unsigned long long adlen, const unsigned char *nsec, const unsigned char *npub, const unsigned char *k );
int crypto_aead_decrypt(unsigned char *m,unsigned long long *mlen,unsigned char *nsec,const unsigned char *c,unsigned long long clen,
const unsigned char *ad,unsigned long long adlen,const unsigned char *npub,const unsigned char *k);
#endif

struct FlexAEADv1 {
   unsigned char subkeys[BLOCKSIZE * 8];
   unsigned char counter[BLOCKSIZE];
   unsigned char checksum[BLOCKSIZE];
   unsigned char state[BLOCKSIZE];
   unsigned char sn[BLOCKSIZE];
   unsigned long long nRounds;
   unsigned long long nBytes;
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

	unsigned char blocklen;
	unsigned char state[(BLOCKSIZE>KEYSIZE)?BLOCKSIZE:KEYSIZE];
	unsigned char tag[BLOCKSIZE];
	struct FlexAEADv1 flexaeadv1;

	if(nsec == NULL) {}; // avoid compiling warnings
	 
	memcpy(state,k,KEYSIZE);
	FlexAEADv1_init( &flexaeadv1, state );

	// ### reset the counter and checksum	
	memcpy(flexaeadv1.counter, npub, NONCESIZE);
	dirPFK( flexaeadv1.counter, flexaeadv1.nBytes, (flexaeadv1.subkeys + (4*flexaeadv1.nBytes)),  flexaeadv1.nRounds, flexaeadv1.state );
	
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
		inc32( flexaeadv1.counter, flexaeadv1.nBytes, 1 );
		memcpy( state, ad+i, flexaeadv1.nBytes);
		sumAD( &flexaeadv1, state);
		i += flexaeadv1.nBytes;
	}
	if(i<adlen)
	{
		memset(state, 0x00, flexaeadv1.nBytes);
		memcpy(state,ad+i,(adlen-i));
		inc32( flexaeadv1.counter, flexaeadv1.nBytes, 1 );
		sumAD( &flexaeadv1, state );
	}

	// ### encrypt the plaintext and calclulate the tag 
	i = 0;
	while( i+flexaeadv1.nBytes <= mlen)
	{
		memcpy(c+i,m+i,flexaeadv1.nBytes);
		inc32( flexaeadv1.counter, flexaeadv1.nBytes, 1 );
		encryptBlock( &flexaeadv1, c+i);
		i += flexaeadv1.nBytes;
	}
	if(i==mlen)
	{
		for( i = 0; i<flexaeadv1.nBytes; i++)
			*(tag+i) =  *(flexaeadv1.checksum+i)^0xAA;		
	}
	else
	{
		blocklen = (unsigned char) (mlen-i);
		memcpy(c+i, m+i, blocklen);
		padBlock(c+i,blocklen, flexaeadv1.nBytes);
		inc32( flexaeadv1.counter, flexaeadv1.nBytes, 1 );
		encryptBlock( &flexaeadv1, c+i);
		for( i = 0; i<flexaeadv1.nBytes; i++)
			*(tag+i) =  *(flexaeadv1.checksum+i)^0x55;
	}
	dirPFK( tag, flexaeadv1.nBytes, (flexaeadv1.subkeys + (0*flexaeadv1.nBytes)),  flexaeadv1.nRounds, flexaeadv1.state );
	memcpy( c+(*clen-TAGSIZE),tag,TAGSIZE);
	
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

	unsigned char blocklen;
	unsigned char  state[(BLOCKSIZE>KEYSIZE)?BLOCKSIZE:KEYSIZE];
	unsigned char  tag[BLOCKSIZE];
	unsigned char  tag1[BLOCKSIZE];
	
	struct FlexAEADv1 flexaeadv1;

	if(nsec == NULL) {}; // avoid compiling warnings
	
	memcpy(state,k,KEYSIZE);
	FlexAEADv1_init( &flexaeadv1, state );

	// ### reset the counter	
	memcpy(flexaeadv1.counter, npub, NONCESIZE);
	dirPFK( flexaeadv1.counter, flexaeadv1.nBytes, (flexaeadv1.subkeys + (4*flexaeadv1.nBytes)),  flexaeadv1.nRounds, flexaeadv1.state );
	
	// ### remove the tag from ciphertext
	*mlen = clen;
	*mlen -= TAGSIZE;
	memcpy(tag,c+*mlen,TAGSIZE);
	
	// ### calculate the checksum from the AD  
	i = 0;
	while( i+flexaeadv1.nBytes <= adlen)
	{
		inc32( flexaeadv1.counter, flexaeadv1.nBytes, 1 );
		memcpy( state, ad+i, flexaeadv1.nBytes);
		sumAD( &flexaeadv1, state);
		i += flexaeadv1.nBytes;
	}
	if(i<adlen)
	{
		memset(state, 0x00, flexaeadv1.nBytes);
		memcpy(state,ad+i,(adlen-i));
		inc32( flexaeadv1.counter, flexaeadv1.nBytes, 1 );
		sumAD( &flexaeadv1, state );
	}
	
	
	// ### decrypt the ciphertext and calclulate the tag 
	i = 0;
	while( i < *mlen)
	{
		memcpy(m+i,c+i,flexaeadv1.nBytes);
		inc32( flexaeadv1.counter, flexaeadv1.nBytes, 1 );
		decryptBlock( &flexaeadv1, m+i);
		i += flexaeadv1.nBytes;
	}
	
	for( i = 0; i<flexaeadv1.nBytes; i++)
		*(tag1+i) =  *(flexaeadv1.checksum+i)^0xAA;
	dirPFK( tag1, flexaeadv1.nBytes, (flexaeadv1.subkeys + (0*flexaeadv1.nBytes)),  flexaeadv1.nRounds, flexaeadv1.state );
	
	if(memcmp(tag1,tag,TAGSIZE))
	{
		for( i = 0; i<flexaeadv1.nBytes; i++)
			*(tag1+i) =  *(flexaeadv1.checksum+i)^0x55;
		dirPFK( tag1, flexaeadv1.nBytes, (flexaeadv1.subkeys + (0*flexaeadv1.nBytes)),  flexaeadv1.nRounds, flexaeadv1.state );
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
	
	memset((*self).checksum,0x00,BLOCKSIZE);
	
	(*self).nBytes = BLOCKSIZE;
	(*self).nRounds = 2;
	i = 1;
	while( i < (KEYSIZE/2) )
	{
		i <<= 1;
		(*self).nRounds ++;
	}
	
	i=0;
	while( i < (*self).nBytes*8)
	{
		if(i==0)
			memset((*self).subkeys,0x00,KEYSIZE/2);
		else
			memcpy( (*self).subkeys+i, ((*self).subkeys+i-(KEYSIZE/2)), (KEYSIZE/2));	
		dirPFK( ((*self).subkeys+i), (KEYSIZE/2), key, (*self).nRounds, keystate );
		dirPFK( ((*self).subkeys+i), (KEYSIZE/2), key, (*self).nRounds, keystate );
		dirPFK( ((*self).subkeys+i), (KEYSIZE/2), key, (*self).nRounds, keystate );
		i += (KEYSIZE/2);
	}

	(*self).nRounds = 2;
	i = 1;
	while( i < BLOCKSIZE )
	{
		i <<= 1;
		(*self).nRounds ++;
	}
};

inline void inc32( unsigned char * block, unsigned long long blocklen, unsigned char inc )
{
	unsigned long long i = 0;
	unsigned * u ;
	//unsigned char * c;
	//c = (unsigned char *) &u;
	for( i=4; i<=blocklen; i+=4)
	{
		u = (unsigned *) (block+i-4);
		*u += inc;
	/*	c[0] = block[i-1];
		c[1] = block[i-2];
		c[2] = block[i-3];
		c[3] = block[i-4];
		u +=  inc;
		block[i-1] = c[0];
		block[i-2] = c[1];
		block[i-3] = c[2];
		block[i-4] = c[3]; */
	}
};

inline void dirSBoxLayer( unsigned char * block, unsigned long long blocklen )
{
	unsigned long long i = 0;
	for( i=0; i<blocklen; i++ )
		*(block+i) = dirSBox0[ *(block+i) ];
};

inline void invSBoxLayer( unsigned char * block, unsigned long long blocklen )
{
	unsigned long long i = 0;
	for( i=0; i<blocklen; i++ )
		*(block+i) = invSBox0[ *(block+i) ];
};

inline void dirShuffleLayer( unsigned char * block, unsigned long long blocklen, unsigned char * state )
{
	unsigned long long i = 0;
	for( i=0; i<blocklen/2; i++)
	{
		*(state+2*i+0) = ( (*(block+i+0)) & 0xf0)    + ((*(block+i+(blocklen/2))&0xf0)>>4);
		*(state+2*i+1) = ( (*(block+i+0)  & 0x0f)<<4)  + ((*(block+i+(blocklen/2))&0x0f)); 
	}
	memcpy( block, state, blocklen);
	return;
}

inline void invShuffleLayer( unsigned char * block, unsigned long long blocklen, unsigned char * state )
{
	unsigned long long i = 0;
	for( i=0; i<blocklen/2; i++)
	{
		*(state+i+0)            = ((*(block+2*i+0)&0xf0)) + ((*(block+2*i+1)&0xf0)>>4);
		*(state+i+(blocklen/2)) = ((*(block+2*i+1)&0x0f)) + ((*(block+2*i+0)&0x0f)<<4);
	}
	memcpy( block, state, blocklen);
	return;
}

inline void dirPFK( unsigned char * block, unsigned long long blocklen, unsigned char *key_pfk,  unsigned long long nRounds, unsigned char * state )
{
	unsigned long long i = 0;
	unsigned long long n = 0;
	// block XOR first half key_pfk
	for( i=0; i<blocklen; i++ )
		*(block+i) = (*(block+i))^(*(key_pfk+i));
	for( n=0; n<nRounds; n++ )
	{
		// Shuffle Layer
		dirShuffleLayer( block, blocklen, state );
		// SBox Layer (right)
		dirSBoxLayer(block+(blocklen/2),(blocklen/2));
		// XOR L + R -> L
		for( i=0; i<blocklen/2; i++ )
			*(block+i) = (*(block+i))^(*(block+i+(blocklen/2)));
		// SBox Layer (left)
		dirSBoxLayer(block,(blocklen/2));
        // XOR L + R -> R
		for( i=0; i<blocklen/2; i++ )
			*(block+i+(blocklen/2)) = (*(block+i))^(*(block+i+(blocklen/2)));
		//SBox Layer (right)
		dirSBoxLayer(block+(blocklen/2),(blocklen/2));
	}
	// block XOR second half key_pfk
	for( i=0; i<blocklen; i++ )
		*(block+i) = (*(block+i))^(*(key_pfk+blocklen+i));	
	return;
};

inline void invPFK( unsigned char * block, unsigned long long blocklen, unsigned char *key_pfk,  unsigned long long nRounds, unsigned char * state )
{
	unsigned long long i = 0;
	unsigned long long n = 0;
	// block XOR second half key_pfk
	for( i=0; i<blocklen; i++ )
		*(block+i) = (*(block+i))^(*(key_pfk+blocklen+i));	
	for( n=0; n<nRounds; n++ )
	{
		// Inv SBox Layer (right)
		invSBoxLayer(block+(blocklen/2),(blocklen/2));
        // XOR L + R -> R
		for( i=0; i<blocklen/2; i++ )
			*(block+i+(blocklen/2)) = (*(block+i))^(*(block+i+(blocklen/2)));
		// Inv  SBox Layer (left)
		invSBoxLayer(block,(blocklen/2));
		// XOR L + R -> L
		for( i=0; i<blocklen/2; i++ )
			*(block+i) = (*(block+i))^(*(block+i+(blocklen/2)));
		// Inv SBox Layer (right)
		invSBoxLayer(block+(blocklen/2),(blocklen/2));
		// Inv Shuffle Layer
		invShuffleLayer( block, blocklen, state );
	}
	// block XOR first half key_pfk
	for( i=0; i<blocklen; i++ )
		*(block+i) = (*(block+i))^(*(key_pfk+i));
	return;
};

void padBlock( unsigned char * block, unsigned long long blocklen, unsigned long long nBytes )
{
	*(block+blocklen) = 0x80; 
	blocklen++;
	if(blocklen<nBytes)
		memset(block+blocklen,0x00,nBytes-blocklen);
	return;
};

unsigned long long unpadBlock( unsigned char * block, unsigned long long blocklen )
{	
	while(blocklen>0)
	{
		blocklen--;
		if( *(block+blocklen) == 0x80 )
			return blocklen;
	}
	return 0;
};

void sumAD( struct FlexAEADv1 * self, unsigned char * ADblock )
{
	unsigned long long i;
	memcpy( (*self).sn, (*self).counter, (*self).nBytes);
	dirPFK( (*self).sn, (*self).nBytes, ((*self).subkeys + (6*(*self).nBytes)),  (*self).nRounds, (*self).state );
	for( i = 0; i<(*self).nBytes; i++)
		*((*self).sn+i) = *((*self).sn+i)^*(ADblock+i);
	dirPFK( (*self).sn, (*self).nBytes, ((*self).subkeys + (4*(*self).nBytes)),  (*self).nRounds, (*self).state );
	for( i = 0; i<(*self).nBytes; i++)
		*((*self).checksum+i) = *((*self).checksum+i)^*((*self).sn+i);
	return;
};	

void encryptBlock( struct FlexAEADv1 * self, unsigned char * block )
{
	unsigned long long i;
	memcpy( (*self).sn, (*self).counter, (*self).nBytes);
	dirPFK( (*self).sn, (*self).nBytes, ((*self).subkeys + (6*(*self).nBytes)),  (*self).nRounds, (*self).state );
	for( i = 0; i<(*self).nBytes; i++)
		*(block+i) = *((*self).sn+i)^*(block+i);
	dirPFK( block, (*self).nBytes, ((*self).subkeys + (4*(*self).nBytes)),  (*self).nRounds, (*self).state );
	for( i = 0; i<(*self).nBytes; i++)
		*((*self).checksum+i) = *((*self).checksum+i)^*(block+i);
	dirPFK( block, (*self).nBytes, ((*self).subkeys + (2*(*self).nBytes)),  (*self).nRounds, (*self).state );
	for( i = 0; i<(*self).nBytes; i++)
		*(block+i) = *((*self).sn+i)^*(block+i);
	dirPFK( block, (*self).nBytes, ((*self).subkeys + (0*(*self).nBytes)),  (*self).nRounds, (*self).state );
	return;
};

void decryptBlock( struct FlexAEADv1 * self, unsigned char * block )
{
	unsigned long long i;
	memcpy( (*self).sn, (*self).counter, (*self).nBytes);
	dirPFK( (*self).sn, (*self).nBytes, ((*self).subkeys + (6*(*self).nBytes)),  (*self).nRounds, (*self).state );
	invPFK( block, (*self).nBytes, ((*self).subkeys + (0*(*self).nBytes)),  (*self).nRounds, (*self).state );
	for( i = 0; i<(*self).nBytes; i++)
		*(block+i) = *((*self).sn+i)^*(block+i);
	invPFK( block, (*self).nBytes, ((*self).subkeys + (2*(*self).nBytes)),  (*self).nRounds, (*self).state );
	for( i = 0; i<(*self).nBytes; i++)
		*((*self).checksum+i) = *((*self).checksum+i)^*(block+i);
	invPFK( block, (*self).nBytes, ((*self).subkeys + (4*(*self).nBytes)),  (*self).nRounds, (*self).state );
	for( i = 0; i<(*self).nBytes; i++)
		*(block+i) = *((*self).sn+i)^*(block+i);
	return;
};


