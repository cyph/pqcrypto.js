/* Copyright (c) 2014, NEC Corporation. All rights reserved.
*
* LICENSE
*
* 1. NEC Corporation ("NEC") hereby grants users to use and reproduce 
*AES-OTR program ("Software") for testing and evaluation purpose for 
*CAESAR (Competition for Authenticated Encryption: Security, Applicability, 
*and Robustness). The users must not use the Software for any other purpose 
*or distribute it to any third party.
*
* 2. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
*OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
*FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL NEC 
*BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, OR CONSEQUENTIAL 
*DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
*ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OF THE 
*SOFTWARE.
*
* 3. In the event of an user's failure to comply with any term of this License 
*or wrongful act, NEC may terminate such user's use of the Software. Upon such 
*termination the user must cease all use of the Software and destroy all copies 
*of the Software.
*
* 4. The users shall comply with all applicable laws and regulations, including 
*export and import control laws, which govern the usage of the Software.
*/
/* 
* A reference C code for AES-OTR v3, for combined associated data processing (ADP=C) 
*/

#include "crypto_aead.h" /* for SUPERCOP */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "t-aes_define.h"
#include "otr.h"
#include "api.h"

#define XORWORD //word-wise XOR of blocks
#define UNVERIFIEDP //unverified plaintext remained in the buffer
//#define MULWORD //word-wise doubling for little endian environment

/* AES functions */
extern int KeySchedule(int skeylen, int mode, const uint8 *skey);
extern int Encryption(int skeylen, uint8 *plain, uint8 *cipher);

/* basic internal functions */
static void ozp(uint32 length, const uint8 *in, uint8 *out);
static void fmt(uint32 nlength, uint32 tlength, const uint8 *nonce, uint8 *fnonce);
static void mul2(uint8 *in, uint8 *out);
static void mul3(uint8 *in, uint8 *out);
static void xor(uint8 *x, uint8 *y, uint8 *z);
static void xorp(uint32 length, const uint8 *x, const uint8 *y, uint8 *z);

/*
 EFunc : OTRC Core Encryption Function
*/
int EFunc(
		  const uint8 *V, //Initial vector of BLOCK bytes
		  const uint8 *plaintext,
		  uint32 pl_len,
		  uint32 t_len,
		  uint8 *ciphertext,
		  uint8 *blocktag //one-block tag
		  )
{
	uint32 i = 0;
	uint32 ell=0; //number of 2BLOCK-byte chunks, excl. last one
	uint32 last=0; //number of bytes in the last chunks

	uint8 tmp[BLOCK] = { 0 }, Z[BLOCK] = { 0 };
	uint8 Sum[BLOCK]={0};
	uint8 *La; //uint8 La[BLOCK]; 
	uint8 *p=(uint8*) plaintext;
	uint8 *c = ciphertext;
	uint8	L[BLOCK] = { 0 }, Ls[BLOCK] = { 0 };

	if(pl_len != 0){
		last = pl_len % DBLOCK;
		if(last == 0) last = DBLOCK;
		ell = (pl_len-last) / DBLOCK; // plaintext length = 2BLOCK*ell + last (non-zero)
	}
	Encryption(KeyLen, (uint8*)V, L); // L <- E(V)
	mul3(L, Ls); // Ls <- 3L
	/* 2-round Feistel for the full chunks */
	for (i = 0; i < ell; i++,p+=DBLOCK,c+=DBLOCK){
		xor(L, p, tmp);
		Encryption(KeyLen, tmp, tmp);
		xor(tmp, p+BLOCK,c);
		xor(Ls, c, tmp);
		Encryption(KeyLen, tmp, tmp);
		xor(tmp, p, c + BLOCK);
		xor(Sum, p+BLOCK, Sum);
		xor(L, Ls, L); //L <- L + Ls
		mul2(Ls, Ls); // Ls <- 2Ls
	}
	/* Last chunk */
	if(last <= BLOCK){ 	//odd block, including the case pl_len = 0 (no plaintext)
		Encryption(KeyLen, L, Z);
		xorp(last, Z, p, c);
		ozp(last, p, tmp);
		xor(tmp, Sum, Sum);
		La = L;
	}
	else{//even blocks, last > BLOCK always holds. 2-round Feistel with last swap
		xor(L, p, tmp);
		Encryption(KeyLen, tmp, Z);
		xorp(last - BLOCK, Z, p+BLOCK, c+BLOCK);
		ozp(last - BLOCK, c+BLOCK, tmp);
		xor(Sum, Z, Sum);
		xor(Sum, tmp, Sum);
		xor(Ls, tmp, tmp);
		Encryption(KeyLen, tmp, tmp);
		xor(tmp, p, c);
		La = Ls;
	}
	/* TE generation */
	mul2(La, tmp);
	if (last == BLOCK || last == DBLOCK){//[last = 16 or 32] = integral block
		mul3(tmp, tmp); 
	}
	else {//partial block
		mul2(tmp,tmp);
	}
	xor(La, tmp, La);	
	xor(Sum, La, Sum);	//Sum = (3^2 or 7)La xor Sum
	Encryption(KeyLen, Sum, blocktag);
	return SUCCESS;
}

/*
 DFunc : OTR Core Decryption Function
 */
int DFunc(
		  int adproc, //switch for AUTHF or DECF 
		  const uint8 *V, //BLOCK-byte IV
		  const uint8 *ciphertext, //or header when AUTHF
		  uint32 ci_len, //or h_len when AUTHF
		  uint32 t_len, //BLOCK when AUTHF
		  uint8 *plaintext, //unused when AUTHF
		  uint8 *blocktag //one-block tag
		  )
{
	uint32 i = 0;
	uint32 ell=0; //number of 2BLOCK-byte chunks, excl. last one
	uint32 last=0; //number of bytes in the last chunks

	uint8 tmp[BLOCK] = { 0 }, Z[BLOCK] = { 0 };
	uint8 Sum[BLOCK]={0};
	uint8	L[BLOCK] = { 0 }, Ls[BLOCK] = { 0 };
	uint8 *La;
	uint8 *p = plaintext;
	uint8 *c = (uint8*)ciphertext;
	int dd = DBLOCK;

	if(ci_len != 0){
		last = ci_len % DBLOCK;
		if(last == 0) last = DBLOCK;
		ell = (ci_len-last) / DBLOCK; // plaintext length = 2BLOCK*ell + last (non-zero)
	}
	if (adproc == AUTHF){//Associated data processing (AUTHF)
		dd = 0;
		p = (uint8 *)malloc(DBLOCK);
	}
	Encryption(KeyLen, (uint8*)V, L);	// L <- E(V)
	mul3(L, Ls); // Ls <- 3L
	/* 2-round Feistel for the full chunks */
	for (i = 0; i < ell; i++, p += dd, c += DBLOCK){
		xor(Ls, c, tmp); //xor(Ls, ciphertext + j, tmp);
		Encryption(KeyLen, tmp, tmp);
		xor(tmp, c + BLOCK, p);//xor(tmp, ciphertext + (j + BLOCK), plaintext + j);
		xor(L, p, tmp);//xor(L, plaintext + j, tmp);
		Encryption(KeyLen, tmp, tmp);
		xor(tmp, c, p + BLOCK); //xor(tmp, ciphertext + j, plaintext + (j + BLOCK));
		xor(Sum, p + BLOCK, Sum);//xor(Sum, plaintext + (j + BLOCK), Sum);
		xor(L, Ls, L); //L <- L + Ls
		mul2(Ls, Ls); // Ls <- 2Ls
	}
	/* Last chunk */
	if(last <= BLOCK){ 	//odd block, including the case ci_len = 0 (no plaintext)
		Encryption(KeyLen, L, Z);
		xorp(last, Z, c, p);//xorp(last, Z, ciphertext + j, plaintext + j);
		ozp(last, p, tmp); //ozp(last, plaintext + j, tmp);
		xor(tmp,Sum,Sum);
		La = L;
	}
	else{//even blocks, last > BLOCK always holds. 2-round Feistel with last swap
		ozp(last - BLOCK, c + BLOCK, tmp); 
		xor(Sum,tmp,Sum);
		xor(Ls,tmp,tmp);
		Encryption(KeyLen,tmp,tmp);
		xor(tmp, c, p); //xor(tmp, ciphertext + j, plaintext + j);
		xor(L, p, tmp); //xor(L, plaintext + j, tmp);
		Encryption(KeyLen,tmp,Z);
		xorp(last - BLOCK, Z, c + BLOCK, p + BLOCK); //xorp(last - BLOCK, Z, ciphertext + (j + BLOCK), plaintext + (j + BLOCK));
		xor(Sum,Z,Sum);
		La = Ls;
	}
	/* TE generation */
	mul2(La, tmp);
	if (last == BLOCK || last == DBLOCK){//[last = 16 or 32] = integral block
		mul3(tmp, tmp);
	}
	else {//partial block
		mul2(tmp, tmp);
	}
	xor(La, tmp, La);
	xor(Sum, La, Sum);	//Sum = (3^2 or 7)La xor Sum
	Encryption(KeyLen, Sum, blocktag);
	if (adproc == AUTHF){//case AUTHF
		free(p);
	}
	return SUCCESS;
}//end of DFunc

/*
 ozp: 100...0 padding for length (<=16) bytes (when n = 16 nothing is done)
*/
static void ozp(uint32 length, const uint8 *in, uint8 *out){
	uint8 tmp[BLOCK + 1] = { 0 };
	memcpy(tmp, in, length);
	tmp[length] = 0x80;
	memcpy(out, tmp, BLOCK);
}
/*
 Format function : Format(tau,N)=number2string(tau mod n,7)||0^{n-8-|N|}||1||N
 */
static void fmt(uint32 nlength, uint32 tlength, const uint8 *nonce, uint8 *fnonce){
		memcpy(&fnonce[BLOCK - nlength], nonce, nlength);
		fnonce[0] = (uint8)(((tlength * 8) % (BLOCK * 8)) << 1);
		fnonce[BLOCK - nlength - 1] |= 0x01;
}
 /*
 mul2 : mutiply 2 over GF(2^n)
*/
#ifdef MULWORD
/* (crazy) block-wise mul2 for little endian environment */
static void mul2(uint8 *in, uint8 *out){
	int a0 = ((signed char)in[0]) >> 7; //assumes arithmetic shift
	uint8 a8 = in[8] >> 7;
	block* x = (block*)in;
	block* y = (block*)out;
	block car;
	car.l = (x->l & 0x8080808080808080) >> 15;
	car.r = (x->r & 0x8080808080808080) >> 15;
	y->l = ((x->l & 0x7f7f7f7f7f7f7f7f) << 1) ^ car.l;
	y->r = ((x->r & 0x7f7f7f7f7f7f7f7f) << 1) ^ car.r;
	y->l |= ((uint64)(a8)) << 56;
	y->r ^= (uint64)(a0 & 0x87) << 56;
}
#else
static const uint8 cst_mul[2] = {0x00, 0x87};
static void mul2(uint8 *in, uint8 *out)
{
	int i, t = in[0] >> 7;
    for(i = 0; i < 15; i++)
        out[i] = (in[i] << 1) | (in[i + 1] >> 7);
    out[15] = (in[15] << 1) ^ cst_mul[t];

}
#endif //MULWORD

static void mul3(uint8 *in, uint8 *out) //3X = 2X + X
{
	uint8	tmp[BLOCK];
	mul2(in, tmp);
	xor(in,tmp,out);
}

#ifdef XORWORD
static void xor(uint8 *x, uint8 *y, uint8 *z){
	((block*)z)->l = ((block*)x)->l ^ ((block*)y)->l;
	((block*)z)->r = ((block*)x)->r ^ ((block*)y)->r;
}
#else
static void xor(uint8 *x, uint8 *y, uint8 *z)
	{
		uint32 i;
		for (i = 0; i < BLOCK; i++){
			z[i] = x[i] ^ y[i];
		}
	}
#endif //XORWORD

static void xorp(uint32 length, const uint8 *x, const uint8 *y, uint8 *z){
	uint32 i;
    for(i = 0; i < length; i++){
		z[i] = x[i]^y[i];
	}
}

int crypto_aead_encrypt(
	unsigned char *c,unsigned long long *clen,
	const unsigned char *m,unsigned long long mlen,
	const unsigned char *ad,unsigned long long adlen,
	const unsigned char *nsec,
	const unsigned char *npub,
	const unsigned char *k
	)
{
	const uint8 cst[BLOCK] = {0};
	uint8 fnonce[BLOCK] = { 0 }; 
	uint8 blocktag[BLOCK] = { 0 }; 

	KeySchedule(KeyLen, ENC, k);
	if(adlen>0){
		DFunc(AUTHF, cst, ad, (uint32)adlen, BLOCK, NULL, blocktag);
	}
	fmt(CRYPTO_NPUBBYTES, CRYPTO_ABYTES, npub, fnonce); 
	xor(blocktag, fnonce, fnonce); //V = fmt(N) xor TA
	EFunc(fnonce, m, (uint32)mlen, CRYPTO_ABYTES, c, blocktag);//Compute ciphertext and full-block tag 
	memcpy(c + mlen, (uint8*)&blocktag, CRYPTO_ABYTES);
	*clen = mlen + CRYPTO_ABYTES;

	return 0;
}

int crypto_aead_decrypt(
	unsigned char *m,unsigned long long *mlen,
	unsigned char *nsec,
	const unsigned char *c,unsigned long long clen,
	const unsigned char *ad,unsigned long long adlen,
	const unsigned char *npub,
	const unsigned char *k
	)
{
	const uint8 cst[BLOCK] = {0};
	uint8 fnonce[BLOCK] = {0};
	uint8 blocktag[BLOCK] = { 0 }; 

	*mlen = clen - CRYPTO_ABYTES;
	KeySchedule(KeyLen, ENC, k);
	if (adlen>0){
		DFunc(AUTHF, cst, ad, (uint32)adlen, BLOCK, NULL, blocktag);
	}
	fmt(CRYPTO_NPUBBYTES, CRYPTO_ABYTES, npub, fnonce); 
	xor(blocktag, fnonce, fnonce); //V = fmt(N) xor TA
	DFunc(DECF, fnonce, c, (uint32)*mlen, CRYPTO_ABYTES, m, blocktag);//compute plaintext and tag (in blocktag)
	if (memcmp(blocktag, c + (uint32)*mlen, CRYPTO_ABYTES) != 0){//non-constant-time compare
		return TAG_UNMATCH;
	}
	return TAG_MATCH;
}
