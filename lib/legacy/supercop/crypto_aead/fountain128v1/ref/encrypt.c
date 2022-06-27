#include <string.h>
#include <stdio.h>
#include "crypto_aead.h"

#define const1  0xff
#define const2  0x3f
#define const3  0x00
#define const4  0x80




unsigned char SR[16] = {
	0x1, 0xA, 0x4, 0xC, 0x6, 0xF, 0x3, 0x9, 0x2, 0xD, 0xB, 0x7, 0x5, 0x0, 0x8, 0xE };   // gift S-box

unsigned char SR_kg[16] = {
	0x9, 0x5, 0x6, 0xD, 0x8, 0xA, 0x7, 0x2, 0xE, 0x4, 0xC, 0x1, 0xF, 0x0, 0xB, 0x3 };   // keystream generation S-box

unsigned char SR_ad[16] = {
	0x9, 0xD, 0xE, 0x5, 0x8, 0xA, 0xF, 0x2, 0x6, 0xC, 0x4, 0x1, 0x7, 0x0, 0xB, 0x3 };   // associated data S-box

unsigned char SR_tag[16] = {
	0xB, 0xF, 0xE, 0x8, 0x7, 0xA, 0x2, 0xD, 0x9, 0x3, 0x4, 0xC, 0x5, 0x0, 0x6, 0x1 };   // tag generation S-box


void LFSR1_clock(unsigned char *LFSR1, unsigned char w)
{
	unsigned char fd;
	unsigned char r;

	fd = 0x00;
	fd = LFSR1[31] ^ LFSR1[25] ^ LFSR1[12] ^ LFSR1[0] ^ w;
	for (r = 0; r < 63; r++)LFSR1[r] = LFSR1[r + 1];
	LFSR1[63] = fd;
}


void LFSR2_clock(unsigned char *LFSR2, unsigned char w)
{
	unsigned char fd;
	unsigned char r;

	fd = 0x00;
	fd = LFSR2[31] ^ LFSR2[19] ^ LFSR2[9] ^ LFSR2[0] ^ w;
	for (r = 0; r < 63; r++)LFSR2[r] = LFSR2[r + 1];
	LFSR2[63] = fd;
}




void LFSR3_clock(unsigned char *LFSR3, unsigned char w)
{
	unsigned char fd;
	unsigned char r;

	fd = 0x00;
	fd = LFSR3[31] ^ LFSR3[20] ^ LFSR3[14] ^ LFSR3[0] ^ w;
	for (r = 0; r < 63; r++)LFSR3[r] = LFSR3[r + 1];
	LFSR3[63] = fd;
}




void LFSR4_clock(unsigned char *LFSR4, unsigned char w)
{
	unsigned char fd;
	unsigned char r;

	fd = 0x00;
	fd = LFSR4[31] ^ LFSR4[10] ^ LFSR4[6] ^ LFSR4[0] ^ w;
	for (r = 0; r < 63; r++)LFSR4[r] = LFSR4[r + 1];
	LFSR4[63] = fd;
}



/*MULx*/

unsigned char MULx(unsigned char V, unsigned char c)
{
	if (V & 0x02)
		return(((V << 1) ^ c) & 0x03);
	else
		return((V << 1) & 0x03);
}

unsigned char Matrix(unsigned char V)
{
	unsigned char r0 = 0, r1 = 0;
	unsigned char w0 = 0, w1 = 0;

	r0 = V & 0x03;
	r1 = ((V & 0x0c) >> 2) & 0x03;

	w1 = r1 ^ MULx(r0, 0x03);
	w0 = MULx(r1, 0x03) ^ r0;

	return(((w1 << 2) | w0) & 0x0f);

}

unsigned char filter(unsigned char *LFSR1, unsigned char *LFSR2, unsigned char *LFSR3, unsigned char *LFSR4)
{
	unsigned char bot, out;

	bot = 0x00;
	out = 0x00;
	bot = (LFSR4[2] & LFSR1[5]) ^ (LFSR2[4] & LFSR3[11]) ^ (LFSR4[23] & LFSR3[27]) ^ (LFSR2[24] & LFSR1[29]) ^ (LFSR4[2] & LFSR4[23] & LFSR4[30]);
	out = LFSR1[3] ^ LFSR1[11] ^ LFSR2[20] ^ LFSR3[5] ^ LFSR3[16] ^ LFSR4[7] ^ LFSR4[29] ^ bot;
	return(out);

}



unsigned char Feedback(unsigned char *LFSR1, unsigned char *LFSR2, unsigned char *LFSR3, unsigned char *LFSR4)
{
	unsigned char fdword = 0;

	fdword = 0;
	fdword |= LFSR1[1];
	fdword |= ((LFSR2[1] << 1));
	fdword |= ((LFSR3[1] << 2));
	fdword |= ((LFSR4[1] << 3));
	fdword &= 0x0f;

	return(SR_kg[fdword]);
}


unsigned char Feedback_ad(unsigned char *LFSR1, unsigned char *LFSR2, unsigned char *LFSR3, unsigned char *LFSR4)
{
	unsigned char fdword = 0;

	fdword = 0;
	fdword |= LFSR1[1];
	fdword |= ((LFSR2[1] << 1));
	fdword |= ((LFSR3[1] << 2));
	fdword |= ((LFSR4[1] << 3));
	fdword &= 0x0f;

	return(SR_ad[fdword]);
}


unsigned char Feedback_tag(unsigned char *LFSR1, unsigned char *LFSR2, unsigned char *LFSR3, unsigned char *LFSR4)
{
	unsigned char fdword = 0;

	fdword = 0;
	fdword |= LFSR1[1];
	fdword |= ((LFSR2[1] << 1));
	fdword |= ((LFSR3[1] << 2));
	fdword |= ((LFSR4[1] << 3));
	fdword &= 0x0f;

	return(SR_tag[fdword]);
}



void Stateupdate_kg(unsigned char *LFSR1, unsigned char *LFSR2, unsigned char *LFSR3, unsigned char *LFSR4)
{
	unsigned char fdword = 0;
	unsigned char f1 = 0, f2 = 0, f3 = 0, f4 = 0;

	fdword = Feedback(LFSR1, LFSR2, LFSR3, LFSR4);
	f1 = fdword & 0x1;
	f2 = (fdword >> 1) & 0x1;
	f3 = (fdword >> 2) & 0x1;
	f4 = (fdword >> 3) & 0x1;

	LFSR1_clock(LFSR1, f1);
	LFSR2_clock(LFSR2, f2);
	LFSR3_clock(LFSR3, f3);
	LFSR4_clock(LFSR4, f4);

}



void Stateupdate_Init(unsigned char *LFSR1, unsigned char *LFSR2, unsigned char *LFSR3, unsigned char *LFSR4, unsigned char ks)
{
	unsigned char fdword = 0;
	unsigned char f1 = 0, f2 = 0, f3 = 0, f4 = 0;

	fdword = Feedback(LFSR1, LFSR2, LFSR3, LFSR4);
	f1 = fdword & 0x1;
	f2 = (fdword >> 1) & 0x1;
	f3 = (fdword >> 2) & 0x1;
	f4 = (fdword >> 3) & 0x1;

	LFSR1_clock(LFSR1, f1 ^ ks);
	LFSR2_clock(LFSR2, f2 ^ ks);
	LFSR3_clock(LFSR3, f3 ^ ks);
	LFSR4_clock(LFSR4, f4 ^ ks);

}



void Stateupdate_ad(unsigned char *LFSR1, unsigned char *LFSR2, unsigned char *LFSR3, unsigned char *LFSR4, unsigned char ad)
{
	unsigned char fdword = 0;
	unsigned char f1 = 0, f2 = 0, f3 = 0, f4 = 0;

	fdword = Feedback_ad(LFSR1, LFSR2, LFSR3, LFSR4);
	f1 = fdword & 0x1;
	f2 = (fdword >> 1) & 0x1;
	f3 = (fdword >> 2) & 0x1;
	f4 = (fdword >> 3) & 0x1;

	LFSR1_clock(LFSR1, f1 ^ ad);
	LFSR2_clock(LFSR2, f2 ^ ad);
	LFSR3_clock(LFSR3, f3 ^ ad);
	LFSR4_clock(LFSR4, f4 ^ ad);

}



void Stateupdate_tag(unsigned char *LFSR1, unsigned char *LFSR2, unsigned char *LFSR3, unsigned char *LFSR4, unsigned char ks)
{
	unsigned char fdword = 0;
	unsigned char f1 = 0, f2 = 0, f3 = 0, f4 = 0;

	fdword = Feedback_tag(LFSR1, LFSR2, LFSR3, LFSR4);
	f1 = fdword & 0x1;
	f2 = (fdword >> 1) & 0x1;
	f3 = (fdword >> 2) & 0x1;
	f4 = (fdword >> 3) & 0x1;

	LFSR1_clock(LFSR1, f1 ^ ks);
	LFSR2_clock(LFSR2, f2 ^ ks);
	LFSR3_clock(LFSR3, f3 ^ ks);
	LFSR4_clock(LFSR4, f4 ^ ks);

}




void Stateupdate_message(unsigned char *LFSR1, unsigned char *LFSR2, unsigned char *LFSR3, unsigned char *LFSR4, unsigned char ks)
{
	unsigned char fdword = 0;
	unsigned char f1 = 0, f2 = 0, f3 = 0, f4 = 0;

	fdword = Feedback(LFSR1, LFSR2, LFSR3, LFSR4);
	f1 = fdword & 0x1;
	f2 = (fdword >> 1) & 0x1;
	f3 = (fdword >> 2) & 0x1;
	f4 = (fdword >> 3) & 0x1;

	LFSR1_clock(LFSR1, f1 ^ ks);
	LFSR2_clock(LFSR2, f2 ^ ks);
	LFSR3_clock(LFSR3, f3 ^ ks);
	LFSR4_clock(LFSR4, f4 ^ ks);

}



void Initialization(unsigned char k[128], unsigned char iv[96], unsigned char *LFSR1, unsigned char *LFSR2, unsigned char *LFSR3, unsigned char *LFSR4)
{
	unsigned int t;
	unsigned char d, c1[8], c2[8], c3[8], c4[8];

	d = 0;

	memset(LFSR1, 0x00, 64);
	memset(LFSR2, 0x00, 64);
	memset(LFSR3, 0x00, 64);
	memset(LFSR4, 0x00, 64);

	memset(c1, 0x00, 8);
	memset(c2, 0x00, 8);
	memset(c3, 0x00, 8);
	memset(c4, 0x00, 8);


	for (t = 0; t < 8; t++)c1[t] = (const1 >> t) & 0x1;
	for (t = 0; t < 8; t++)c2[t] = (const2 >> t) & 0x1;
	for (t = 0; t < 8; t++)c3[t] = (const3 >> t) & 0x1;
	for (t = 0; t < 8; t++)c4[t] = (const4 >> t) & 0x1;

	memcpy(LFSR1, k, 8); memcpy(LFSR1 + 8, iv, 8); memcpy(LFSR1 + 16, k + 8, 8); memcpy(LFSR1 + 24, iv + 8, 8);
	memcpy(LFSR1 + 32, k + 16, 8); memcpy(LFSR1 + 40, iv + 16, 8); memcpy(LFSR1 + 48, k + 24, 8); memcpy(LFSR1 + 56, iv + 24, 8);

	memcpy(LFSR2, k + 32, 8); memcpy(LFSR2 + 8, iv + 32, 8); memcpy(LFSR2 + 16, k + 40, 8); memcpy(LFSR2 + 24, iv + 40, 8);
	memcpy(LFSR2 + 32, k + 48, 8); memcpy(LFSR2 + 40, iv + 48, 8); memcpy(LFSR2 + 48, k + 56, 8); memcpy(LFSR2 + 56, iv + 56, 8);

	memcpy(LFSR3, k + 64, 8); memcpy(LFSR3 + 8, iv + 64, 8); memcpy(LFSR3 + 16, k + 72, 8); memcpy(LFSR3 + 24, iv + 72, 8);
	memcpy(LFSR3 + 32, k + 80, 8); memcpy(LFSR3 + 40, iv + 80, 8); memcpy(LFSR3 + 48, k + 88, 8); memcpy(LFSR3 + 56, iv + 88, 8);

	memcpy(LFSR4, k + 96, 8);  memcpy(LFSR4 + 8, k + 104, 8); memcpy(LFSR4 + 16, c1, 8);  memcpy(LFSR4 + 24, k + 112, 8);
	memcpy(LFSR4 + 32, k + 120, 8); memcpy(LFSR4 + 40, c2, 8); memcpy(LFSR4 + 48, c3, 8); memcpy(LFSR4 + 56, c4, 8);

	for (t = 0; t<384; t++)
	{
		d = filter(LFSR1, LFSR2, LFSR3, LFSR4);
		Stateupdate_Init(LFSR1, LFSR2, LFSR3, LFSR4, d);
	}
}


void Keystreamgen(unsigned int n, unsigned char *ks, unsigned char *LFSR1, unsigned char *LFSR2, unsigned char *LFSR3, unsigned char *LFSR4)
{
	unsigned int i;

	for (i = 0; i<n; i++)
	{
		ks[i] = filter(LFSR1, LFSR2, LFSR3, LFSR4);
		Stateupdate_kg(LFSR1, LFSR2, LFSR3, LFSR4);

	}
}

void Keystreamgen_byte(unsigned int n, unsigned char *ks, unsigned char *LFSR1, unsigned char *LFSR2, unsigned char *LFSR3, unsigned char *LFSR4, unsigned char *ksbyte)
{
	unsigned int i;

	for (i = 0; i<n; i++)
	{
		ks[i] = filter(LFSR1, LFSR2, LFSR3, LFSR4);
		Stateupdate_kg(LFSR1, LFSR2, LFSR3, LFSR4);
		*(ksbyte + i / 8) |= (ks[i] << (i % 8));

	}
}




// encrypt one byte
void enc_onebyte(unsigned char *LFSR1, unsigned char *LFSR2, unsigned char *LFSR3, unsigned char *LFSR4, unsigned char plaintextbyte,
	unsigned char *ciphertextbyte)
{
	unsigned char i;
	unsigned char plaintextbit, ciphertextbit, kstem;

	*ciphertextbyte = 0;
	kstem = 0;

	for (i = 0; i < 8; i++)
	{
		plaintextbit = (plaintextbyte >> i) & 1;
		kstem = filter(LFSR1, LFSR2, LFSR3, LFSR4);
		ciphertextbit = kstem ^ plaintextbit;
		Stateupdate_message(LFSR1, LFSR2, LFSR3, LFSR4, plaintextbit);
		*ciphertextbyte |= (ciphertextbit << i);

	}
}


// decrypt one byte
void dec_onebyte(unsigned char *LFSR1, unsigned char *LFSR2, unsigned char *LFSR3, unsigned char *LFSR4, unsigned char *plaintextbyte,
	unsigned char ciphertextbyte)
{
	unsigned char i;
	unsigned char plaintextbit, ciphertextbit, kstem;

	*plaintextbyte = 0;
	kstem = 0;
	for (i = 0; i < 8; i++)
	{
		ciphertextbit = (ciphertextbyte >> i) & 1;
		kstem = filter(LFSR1, LFSR2, LFSR3, LFSR4);
		plaintextbit = kstem ^ ciphertextbit;
		Stateupdate_message(LFSR1, LFSR2, LFSR3, LFSR4, plaintextbit);
		*plaintextbyte |= (plaintextbit << i);
	}
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
	unsigned long long i,j;
	unsigned char ks[128], mac[16], kstem = 0, adbit = 0;
	unsigned char LFSR1[64], LFSR2[64], LFSR3[64], LFSR4[64];
	unsigned char key[128], iv[96];

	memset(key, 0x0, 128);
	memset(iv, 0x0, 96);
	memset(ks, 0x0, 128);
	memset(mac, 0x0, 16);
	kstem = 0;
	adbit = 0;

	for (i = 0; i < 128; i++)key[i] = ((k[i / 8] >> (i % 8))) & 0x1;
	for (i = 0; i < 96; i++)iv[i] = ((npub[i / 8] >> (i % 8))) & 0x1;
	

	//initialization stage
	Initialization(key, iv, LFSR1, LFSR2, LFSR3, LFSR4);


	//process the associated data
	for (i = 0; i < adlen; i++){
		for (j = 0; j < 8; j++)
		{
			adbit = 0;
			adbit = (ad[i] >> j) & 0x1;
			kstem = filter(LFSR1, LFSR2, LFSR3, LFSR4);
			Stateupdate_ad(LFSR1, LFSR2, LFSR3, LFSR4, adbit ^ kstem);
		}
	}

	// middle seperation
	for (i = 0; i < 64; i++){
		kstem = filter(LFSR1, LFSR2, LFSR3, LFSR4);
		Stateupdate_Init(LFSR1, LFSR2, LFSR3, LFSR4, kstem);
	}
	LFSR2[0] ^= 0x1;

	//process the plaintext
	for (i = 0; i < mlen; i++)
	{
		enc_onebyte(LFSR1, LFSR2, LFSR3, LFSR4, m[i], c + i);
	}
	LFSR4[1] ^= 0x1;

	//finalization stage, we assume that the tag length is a multiple of bytes
	for (i = 0; i < 384; i++){
		kstem = filter(LFSR1, LFSR2, LFSR3, LFSR4);
		Stateupdate_tag(LFSR1, LFSR2, LFSR3, LFSR4, kstem);
	}
	Keystreamgen_byte(128, ks, LFSR1, LFSR2, LFSR3, LFSR4, mac);
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
	unsigned long long i,j;
	unsigned char ks[128], tag[16], kstem = 0, adbit = 0, check = 0;
	unsigned char LFSR1[64], LFSR2[64], LFSR3[64], LFSR4[64];
	unsigned char key[128], iv[96];

	memset(key, 0x0, 128);
	memset(iv, 0x0, 96);
	memset(ks, 0x0, 128);
	memset(tag, 0x0, 16);
	kstem = 0;
	adbit = 0;

	for (i = 0; i < 128; i++)key[i] = ((k[i / 8] >> (i % 8))) & 0x1;
	for (i = 0; i < 96; i++)iv[i] = ((npub[i / 8] >> (i % 8))) & 0x1;
	

	//initialization stage
	Initialization(key, iv, LFSR1, LFSR2, LFSR3, LFSR4);

	//process the associated data
	for (i = 0; i < adlen; i++){
		for (j = 0; j < 8; j++)
		{
			adbit = 0;
			adbit = (ad[i] >> j) & 0x1;
			kstem = filter(LFSR1, LFSR2, LFSR3, LFSR4);
			Stateupdate_ad(LFSR1, LFSR2, LFSR3, LFSR4, adbit ^ kstem);
		}
	}

	// middle seperation
	for (i = 0; i < 64; i++){
		kstem = filter(LFSR1, LFSR2, LFSR3, LFSR4);
		Stateupdate_Init(LFSR1, LFSR2, LFSR3, LFSR4, kstem);
	}
	LFSR2[0] ^= 0x1;

	//process the ciphertext
	*mlen = clen - 16;

	for (i = 0; i < *mlen; i++)
	{
		dec_onebyte(LFSR1, LFSR2, LFSR3, LFSR4, m + i, c[i]);
	}
	LFSR4[1] ^= 0x1;

	//finalization stage, we assume that the tag length is a multiple of bytes
	for (i = 0; i < 384; i++){
		kstem = filter(LFSR1, LFSR2, LFSR3, LFSR4);
		Stateupdate_tag(LFSR1, LFSR2, LFSR3, LFSR4, kstem);
	}
	Keystreamgen_byte(128, ks, LFSR1, LFSR2, LFSR3, LFSR4, tag);
	for (i = 0; i < 16; i++)tag[i] ^= k[i];

	for (i = 0; i < 16; i++) check |= (tag[i] ^ c[clen - 16 + i]);
	if (check == 0) return 0;
	else return -1;

}