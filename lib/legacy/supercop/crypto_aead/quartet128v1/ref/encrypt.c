#include <string.h>
#include <stdio.h>
#include "crypto_aead.h"

#define const1  0xff
#define const2  0x3f
#define const3  0x00
#define const4  0x80


#define ROTR64(x,n) (((x)>>(n))|((x)<<(64-(n))))
#define ROTL32(x,n)   ( ((x) << (n))  | ((x) >> (32 - (n))) )

#define n1 5 
#define n4 26 
#define n7 20
#define n10 23
#define n13 11


unsigned char round_const[24] = { 0xf0, 0xe1, 0xd2, 0xc3, 0xb4, 0xa5, 0x96, 0x87, 0x78, 0x69, 0x5a, 0x4b, 0x13, 0x26, 0x0c, 0x19, 0x32, 0x25, 0x0a, 0x15, 0x2a, 0x1d, 0x3a, 0x2b };

void load64(unsigned long long* x, const unsigned char* S)
{
	int i;
	*x = 0;
	for (i = 0; i < 8; ++i)
		*x |= ((unsigned long long)S[i]) << (i * 8);
}

void load64_2(unsigned long long* x, unsigned char* S)
{
	int i;
	*x = 0;
	for (i = 0; i <8; ++i)
		*x |= ((unsigned long long)S[i]) << (i * 8);
}


void store64(unsigned char* S, unsigned long long x)
{
	int i;
	for (i = 0; i < 8; ++i)
		S[i] = (unsigned char)(x >> (56 - i * 8));
}



void nistcipher_stateupdate(unsigned long long *state)       // consists of 1 step
{
	unsigned long long t1, tt1 = 0, tt2 = 0, tt3 = 0, tt4 = 0;
	unsigned int t2, t3;

	t1 = (~state[2]) & state[1];
	state[0] ^= t1;
	t2 = state[2] & 0xffffffff;
	t3 = (state[2] >> 32) & 0xffffffff;
	t2 = ROTL32(t2, n1);
	t3 = ROTL32(t3, n1);
	tt1 |= ((unsigned long long)(t3) << 32);
	tt1 |= (unsigned long long)(t2);
	state[2] = tt1;
	state[3] ^= ROTR64(state[3], 61) ^ ROTR64(state[3], 39);


	t1 = (~state[3]) & state[2];
	state[1] ^= t1;
	t2 = state[3] & 0xffffffff;
	t3 = (state[3] >> 32) & 0xffffffff;
	t2 = ROTL32(t2, n4);
	t3 = ROTL32(t3, n4);
	tt2 |= ((unsigned long long)(t3) << 32);
	tt2 |= (unsigned long long)(t2);
	state[3] = tt2;
	state[0] ^= ROTR64(state[0], 1) ^ ROTR64(state[0], 6);



	t1 = (~state[0]) & state[3];
	state[2] ^= t1;
	t2 = state[0] & 0xffffffff;
	t3 = (state[0] >> 32) & 0xffffffff;
	t2 = ROTL32(t2, n7);
	t3 = ROTL32(t3, n7);
	tt3 |= ((unsigned long long)(t3) << 32);
	tt3 |= (unsigned long long)(t2);
	state[0] = tt3;
	state[1] ^= ROTR64(state[1], 10) ^ ROTR64(state[1], 17);

	t1 = (~state[1]) & state[0];
	state[3] ^= t1;
	t2 = state[1] & 0xffffffff;
	t3 = (state[1] >> 32) & 0xffffffff;
	t2 = ROTL32(t2, n10);
	t3 = ROTL32(t3, n10);
	tt4 |= ((unsigned long long)(t3) << 32);
	tt4 |= (unsigned long long)(t2);
	state[1] = tt4;
	state[2] ^= ROTR64(state[2], 7) ^ ROTR64(state[2], 41);

}

void nistcipher_stateupdate_ad(unsigned long long *state)       // consists of 1 step
{
	unsigned long long t1, tt1 = 0, tt2 = 0, tt3 = 0, tt4 = 0;
	unsigned int t2, t3;

	t1 = (~state[2]) & state[1];
	state[0] ^= t1;
	t2 = state[2] & 0xffffffff;
	t3 = (state[2] >> 32) & 0xffffffff;
	t2 = ROTL32(t2, n1);
	t3 = ROTL32(t3, n1);
	tt1 |= ((unsigned long long)(t3) << 32);
	tt1 |= (unsigned long long)(t2);
	state[2] = tt1;
	state[3] ^= ROTR64(state[3], 19) ^ ROTR64(state[3], 28);


	t1 = (~state[3]) & state[2];
	state[1] ^= t1;
	t2 = state[3] & 0xffffffff;
	t3 = (state[3] >> 32) & 0xffffffff;
	t2 = ROTL32(t2, n4);
	t3 = ROTL32(t3, n4);
	tt2 |= ((unsigned long long)(t3) << 32);
	tt2 |= (unsigned long long)(t2);
	state[3] = tt2;
	state[0] ^= ROTR64(state[0], 1) ^ ROTR64(state[0], 6);



	t1 = (~state[0]) & state[3];
	state[2] ^= t1;
	t2 = state[0] & 0xffffffff;
	t3 = (state[0] >> 32) & 0xffffffff;
	t2 = ROTL32(t2, n7);
	t3 = ROTL32(t3, n7);
	tt3 |= ((unsigned long long)(t3) << 32);
	tt3 |= (unsigned long long)(t2);
	state[0] = tt3;
	state[1] ^= ROTR64(state[1], 10) ^ ROTR64(state[1], 17);

	t1 = (~state[1]) & state[0];
	state[3] ^= t1;
	t2 = state[1] & 0xffffffff;
	t3 = (state[1] >> 32) & 0xffffffff;
	t2 = ROTL32(t2, n10);
	t3 = ROTL32(t3, n10);
	tt4 |= ((unsigned long long)(t3) << 32);
	tt4 |= (unsigned long long)(t2);
	state[1] = tt4;
	state[2] ^= ROTR64(state[2], 7) ^ ROTR64(state[2], 41);

}


void nistcipher_stateupdate_tag(unsigned long long *state)       // consists of 1 step
{
	unsigned long long t1, tt1 = 0, tt2 = 0, tt3 = 0, tt4 = 0;
	unsigned int t2, t3;

	t1 = (~state[2]) & state[1];
	state[0] ^= t1;
	t2 = state[2] & 0xffffffff;
	t3 = (state[2] >> 32) & 0xffffffff;
	t2 = ROTL32(t2, n1);
	t3 = ROTL32(t3, n1);
	tt1 |= ((unsigned long long)(t3) << 32);
	tt1 |= (unsigned long long)(t2);
	state[2] = tt1;
	state[3] ^= ROTR64(state[3], 19) ^ ROTR64(state[3], 28);


	t1 = (~state[3]) & state[2];
	state[1] ^= t1;
	t2 = state[3] & 0xffffffff;
	t3 = (state[3] >> 32) & 0xffffffff;
	t2 = ROTL32(t2, n4);
	t3 = ROTL32(t3, n4);
	tt2 |= ((unsigned long long)(t3) << 32);
	tt2 |= (unsigned long long)(t2);
	state[3] = tt2;
	state[0] ^= ROTR64(state[0], 61) ^ ROTR64(state[0], 39);



	t1 = (~state[0]) & state[3];
	state[2] ^= t1;
	t2 = state[0] & 0xffffffff;
	t3 = (state[0] >> 32) & 0xffffffff;
	t2 = ROTL32(t2, n7);
	t3 = ROTL32(t3, n7);
	tt3 |= ((unsigned long long)(t3) << 32);
	tt3 |= (unsigned long long)(t2);
	state[0] = tt3;
	state[1] ^= ROTR64(state[1], 10) ^ ROTR64(state[1], 17);

	t1 = (~state[1]) & state[0];
	state[3] ^= t1;
	t2 = state[1] & 0xffffffff;
	t3 = (state[1] >> 32) & 0xffffffff;
	t2 = ROTL32(t2, n10);
	t3 = ROTL32(t3, n10);
	tt4 |= ((unsigned long long)(t3) << 32);
	tt4 |= (unsigned long long)(t2);
	state[1] = tt4;
	state[2] ^= ROTR64(state[2], 7) ^ ROTR64(state[2], 41);

}


void Null_state(unsigned long long *state)
{
	state[0] = 0x0;
	state[1] = 0x0;
	state[2] = 0x0;
	state[3] = 0x0;

}

void Keystream_generation(unsigned long long *state, unsigned long long *z)
{
	unsigned long long t1, tt1;

	t1 = ~state[1];
	t1 &= ROTR64(state[0], n13);
	tt1 = state[3] ^ state[2];
	z[0] = t1 ^ tt1;
}





void EncryptMessage(unsigned long long *state, const unsigned char *message, unsigned char *ciphertext, unsigned long long msglength)
{
	unsigned long long i;
	unsigned int j;
	unsigned char load[8] = {0,0,0,0,0,0,0,0};

	unsigned long long keystream, t1=0, tt1=0;
	unsigned long long msgblk;
	

	/*encrypt a message, each time 8 bytes are encrypted*/
	for (i = 0; (i + 8) <= msglength; i += 8, message += 8, ciphertext += 8) {

		load64(&msgblk, message);


		//encryption 
		t1 = ~state[1];
		t1 &= ROTR64(state[0], n13);
		tt1 = state[3] ^ state[2];
		keystream = t1 ^ tt1;

		
		*(unsigned long long*)ciphertext = keystream ^ msgblk;
		
		//state update 
		state[0] ^= msgblk;
		nistcipher_stateupdate(state);

	}
	if ((msglength & 7) != 0) {
		t1 = ~state[1];
		t1 &= ROTR64(state[0], n13);
		tt1 = state[3] ^ state[2];
		keystream = t1 ^ tt1;
		for (j = 0; j < (msglength & 7); j++) {
			*(ciphertext + j) = *(message + j) ^ ((unsigned char*)&keystream)[j];
		}
		for (j = 0; j < (msglength & 7); j++){
			load[(msglength & 7) - 1 - j] = *(message + j);
		}
		load[msglength & 7] ^= 0x1;
		load64_2(&tt1, load);
		
		state[0] ^= tt1;
		nistcipher_stateupdate(state);
	}

}

void DecryptMessage(unsigned long long *state, const unsigned char *message, unsigned char *ciphertext, unsigned long long msglength)
{
	unsigned long long i;
	unsigned int j;
	unsigned char load[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

	unsigned long long keystream, t1=0, tt1=0;
	unsigned long long msgblk;
	

	/*decrypt a message, each time 8 bytes are decrypted*/
	for (i = 0; (i + 8) <= msglength; i += 8, message += 8, ciphertext += 8) {

		load64(&msgblk, message);


		//encryption 
		t1 = ~state[1];
		t1 &= ROTR64(state[0], n13);
		tt1 = state[3] ^ state[2];
		keystream = t1 ^ tt1;

		
		*(unsigned long long*)ciphertext = keystream ^ msgblk;
		

		//state update 
		state[0] ^= *(unsigned long long*)ciphertext;
		nistcipher_stateupdate(state);

	}
	if ((msglength & 7) != 0) {
		t1 = ~state[1];
		t1 &= ROTR64(state[0], n13);
		tt1 = state[3] ^ state[2];
		keystream = t1 ^ tt1;
		for (j = 0; j < (msglength & 7); j++) {
			*(ciphertext + j) = *(message + j) ^ ((unsigned char*)&keystream)[j];
		}
		for (j = 0; j < (msglength & 7); j++){
			load[(msglength & 7) - 1 - j] = *(ciphertext + j);
		}
		load[msglength & 7] ^= 0x1;
		load64_2(&tt1, load);
		state[0] ^= tt1;
		nistcipher_stateupdate(state);
	}

}


void Initialization(const unsigned char k[16], const unsigned char iv[12], unsigned long long *state)
{
	unsigned int t;
	unsigned char load[8];
	unsigned long long tt1 = 0, tt2 = 0;

	memset(load, 0x0, 8);
	load[0] = iv[8];
	load[1] = iv[9];
	load[2] = const1; 
	load[3] = iv[10];
	load[4] = iv[11];
	load[5] = const2;
	load[6] = const3;
	load[7] = const4;

	Null_state(state);

	load64(state, k + 0);
	load64(state + 1, iv + 0);
	load64_2(state + 2, load);
	load64(state + 3, k + 8);

	for (t = 0; t < 24; t++){
		nistcipher_stateupdate(state);
		state[3] ^= round_const[t];
	}
	
	load64(&tt1, k + 0);
	state[0] ^= tt1;
	load64(&tt2, k + 8);
	state[1] ^= tt2; 
}




//encrypt a message.
int crypto_aead_encrypt(
	unsigned char *c, unsigned long long *clen,
	const unsigned char *m, unsigned long long mlen,
	const unsigned char *ad, unsigned long long adlen,
	const unsigned char *nsec,
	const unsigned char *npub,
	const unsigned char *k
	)
{
	unsigned long long i;
	unsigned char mac[16];
	unsigned long long state[4];


	memset(mac, 0x0, 16);


	//initialization stage
	Initialization(k, npub, state);
	

	//process the associated data
	for (i = 0; i < adlen; i++)
	{
		state[1] ^= ad[i];
		nistcipher_stateupdate_ad(state);
		
	}

	// middle seperation
	for (i = 0; i < 12; i++)nistcipher_stateupdate(state);
	state[3] ^= 0x1;

	//process the plaintext
	EncryptMessage(state, m, c, mlen);
	

	//finalization stage, we assume that the tag length is a multiple of bytes
	for (i = 0; i < 24; i++)nistcipher_stateupdate_tag(state);

	for (i = 0; i < 2; i++){
		Keystream_generation(state, (unsigned long long*)(mac + 8 * i));
		nistcipher_stateupdate_tag(state);
	}
	for (i = 0; i < 16; i++)mac[i] ^= k[i];

	*clen = mlen + 16;
	memcpy(c + mlen, mac, 16);

	return 0;
}


int crypto_aead_decrypt(
	unsigned char *m, unsigned long long *mlen,
	unsigned char *nsec,
	const unsigned char *c, unsigned long long clen,
	const unsigned char *ad, unsigned long long adlen,
	const unsigned char *npub,
	const unsigned char *k
	)
{
	unsigned long long i;
	unsigned char mac[16], check = 0;
	unsigned long long state[4];

	memset(mac, 0x0, 16);
	check = 0;


	//initialization stage
	Initialization(k, npub, state);
	

	//process the associated data
	for (i = 0; i < adlen; i++)
	{
		state[1] ^= ad[i];
		nistcipher_stateupdate_ad(state);
		
	}

	// middle seperation
	for (i = 0; i < 12; i++)nistcipher_stateupdate(state);
	state[3] ^= 0x1;


	//process the ciphertext
	*mlen = clen - 16;
	DecryptMessage(state, c, m, *mlen);
	

	//finalization stage, we assume that the tag length is a multiple of bytes
	for (i = 0; i < 24; i++)nistcipher_stateupdate_tag(state);

	for (i = 0; i < 2; i++){
		Keystream_generation(state, (unsigned long long*)(mac + 8 * i));
		nistcipher_stateupdate_tag(state);
	}
	for (i = 0; i < 16; i++)mac[i] ^= k[i];
	
	
	for (i = 0; i < 16; i++) check |= (mac[i] ^ c[clen - 16 + i]);
	if (check == 0) return 0;
	else return -1;
}

