#include "crypto_aead.h"


/** 
*  Designers: Daniel Penazzi, Miguel Montes
This codification: Daniel Penazzi.
 
* This code is hereby placed in the public domain.
*/

typedef unsigned long long u64;
typedef unsigned int u32;

//number of rounds of initialization, times 11 (12x11)
#define NRINIT 132

//number of rounds of processing ad and message, times 11
#define NRMID 99

//extra number of rounds of finalization (after NRMID rounds are done), times 11
#define NREND 33

//increase of i for the round constants
#define STEP 11

#define ROTR(x,n) (((x)>>(n))|((x)<<(64-(n))))

void Difusion(u64 W[5]){
	W[3]^= ROTR(W[3],57)^ROTR(W[3],63);
	W[4]^= ROTR(W[4],3)^ROTR(W[4],26);
	W[0]^= ROTR(W[0],43)^ROTR(W[0],62);
	W[1]^= ROTR(W[1],21)^ROTR(W[1],46);
	W[2]^= ROTR(W[2],58)^ROTR(W[2],61);
	W[0]^=W[3]^W[4];W[1]^=W[3]^W[4];W[2]^=W[3]^W[4];
	W[3]^=W[0]^W[1]^W[2];W[4]^=W[0]^W[1]^W[2];
	W[0]=ROTR(W[0],8);
	W[1]=ROTR(W[1],24);
	W[2]=ROTR(W[2],40);
	W[3]=ROTR(W[3],56);
}

void Sbox(u64 W[5]){
	u64 x=W[0];
	u64 y=W[1];
	u64 z=W[2];
	u64 u=W[3];
	u64 v=W[4];
	W[0]=v^(x|u)^(y|(z^x));
	W[1]=x^(y|v)^(z|(u^y));
	W[2]=y^(z|x)^(u|(v^z));
	W[3]=z^(u|y)^(v|(x^u));
	W[4]=(~u)^(v|z)^(x|(y^v));
}




//R7R6....R1R0
void LESTOREu64in8bytearray(u64 Rv,unsigned char* R){
	for(u32 i=0;i<8;i++) {
		R[i]=(Rv&0xff);
		Rv=(Rv>>8);
	} 
} 


void PartialLESTOREu64inbytearray(u64 Rv,unsigned char* R,u64 Top){
	for(u32 i=0;i<Top;i++) {
		R[i]=(Rv&0xff);
		Rv=(Rv>>8);
	} 
} 


u64 LELOAD8bytearrayinu64(const unsigned char* R){
	u64 Rv=0;
	for(u32 i=0;i<8;i++) {
		Rv|=((u64)(R[i])<<(8*i));
	}
	return Rv; 
}

//should only be used if Top<8.
u64 PartialLELOADbytearrayinu64(const unsigned char* R,u64 Top){
	u64 Rv=(u64)R[0];
	for(u64 i=1;i<Top;i++) {
		Rv^=((u64)(R[i])<<(8*i));
	}
	return Rv;
}


void Ronda(u64 Rconst,u64 W[5]) {
	W[0]^=Rconst;
	Sbox(W);
	Difusion(W);
}



//aux funcs


void Initialize(u64 W[5],const unsigned char *npub,const unsigned char *k){
	W[0]=LELOAD8bytearrayinu64(k);k+=8;
	W[1]=LELOAD8bytearrayinu64(k);
	W[2]=LELOAD8bytearrayinu64(npub);npub+=8;
	W[3]=LELOAD8bytearrayinu64(npub);
	W[4]=ROTR(W[0]^W[1],32)^0xff;

	for(unsigned int i=0;i<NRINIT;i=i+STEP) Ronda(i^0xff,W);

}

void ProcessAd(u64 W[5],const unsigned char *ad,unsigned long long adlen){
		u64 remain=adlen;		
		while(remain>=16){
			W[3]^=LELOAD8bytearrayinu64(ad);	ad+=8;
			W[2]^=LELOAD8bytearrayinu64(ad);
			for(unsigned int  i=0;i<NRMID;i=i+STEP) Ronda(i^0xad,W);
			remain-=16;
			ad+=8;
		}//endwhile
		if(remain){
			if(remain<8){
				W[3]^=PartialLELOADbytearrayinu64(ad,remain);
				W[3]^=((u64)(1)<<(8*remain));
			}else{
				W[3]^=LELOAD8bytearrayinu64(ad);	ad+=8;remain-=8;
				if(remain) W[2]^=PartialLELOADbytearrayinu64(ad,remain);
				W[2]^=((u64)(1)<<(8*remain));
			}
			W[1]^=1;//distinguisher from remain=0 case
			for(unsigned int  i=0;i<NRMID;i=i+STEP) Ronda(i^0xad,W);
		}//endifadremain

	W[1]^=1;//delimiter ad/mesg

}

unsigned char* Processplaintxt(u64 W[5],unsigned char *c,const unsigned char *m,unsigned long long mlen){
		u64 remain=mlen;
		while(remain>=16){
			W[3]^=LELOAD8bytearrayinu64(m);
			LESTOREu64in8bytearray(W[3],c);m+=8;c+=8;
			W[2]^=LELOAD8bytearrayinu64(m);
			LESTOREu64in8bytearray(W[2],c);

			for(unsigned int  i=0;i<NRMID;i=i+STEP) Ronda(i^0xff,W);
			remain-=16;
			m+=8;c+=8;
		}//endwhile

		if(remain){
			if(remain<8){
				W[3]^=PartialLELOADbytearrayinu64(m,remain);
				PartialLESTOREu64inbytearray(W[3],c,remain);
				W[3]^=((u64)(1)<<(8*remain));
			}
			else{
				W[3]^=LELOAD8bytearrayinu64(m);
				LESTOREu64in8bytearray(W[3],c);m+=8;c+=8;remain-=8;
				if(remain){
					W[2]^=PartialLELOADbytearrayinu64(m,remain);
					PartialLESTOREu64inbytearray(W[2],c,remain);
				}
				W[2]^=((u64)(1)<<(8*remain));
			}
			W[1]^=1;//distinguisher from remain=0 case
			c+=remain;
			for(unsigned int  i=0;i<NRMID;i=i+STEP) Ronda(i^0xff,W);
		}//endifremain
return c;//need it to write the tag into c
}

//must be used with a prior check that clen is >=16
void Processciphertxt(u64 W[5],unsigned char *m,
const unsigned char *c,unsigned long long clen){

		u64 remain=clen-16;
		while(remain>=16){
			LESTOREu64in8bytearray(W[3],m);
			for(u32 sk=0;sk<8;sk++) m[sk]^=c[sk];
			W[3]=LELOAD8bytearrayinu64(c);m+=8;c+=8;

			LESTOREu64in8bytearray(W[2],m);
			for(u32 sk=0;sk<8;sk++) m[sk]^=c[sk];
			W[2]=LELOAD8bytearrayinu64(c);

			for(unsigned int  i=0;i<NRMID;i=i+STEP) Ronda(i^0xff,W);
			remain-=16;
			m+=8;c+=8;
		}//endwhile

		if(remain){
			if(remain<8){
			PartialLESTOREu64inbytearray(W[3],m,remain);//m=trunc(u)
			for(u32 s=0;s<remain;s++) m[s]^=c[s];
			//now m is trunc(u)^c
			W[3]^=PartialLELOADbytearrayinu64(m,remain);/*u^=(u^c) gives u=c in msg bytes ,
																								while u^=0 leaves the other ones untouched*/
			c+=remain;
			W[3]^=((u64)(1)<<(8*remain));
			}
			else{
				LESTOREu64in8bytearray(W[3],m);
				for(u32 sk=0;sk<8;sk++) m[sk]^=c[sk];
				W[3]=LELOAD8bytearrayinu64(c);m+=8;c+=8;remain-=8;
				if(remain){
					PartialLESTOREu64inbytearray(W[2],m,remain);
					for(u32 s=0;s<remain;s++) m[s]^=c[s];
					W[2]^=PartialLELOADbytearrayinu64(m,remain);
				}
				W[2]^=((u64)(1)<<(8*remain));
				c+=remain;
			}

			W[1]^=1;//distinguisher from remain=0 case
			for(unsigned int  i=0;i<NRMID;i=i+STEP) Ronda(i^0xff,W);
		}//endifremain

}







//main functions below






//=======================ENCRYPT======================

int crypto_aead_encrypt(
unsigned char *c,unsigned long long *clen,
const unsigned char *m,unsigned long long mlen,
const unsigned char *ad,unsigned long long adlen,
const unsigned char *nsec,
const unsigned char *npub,
const unsigned char *k
){
/*generating a ciphertext c[0],c[1],...,c[*clen-1]
... from a plaintext m[0],m[1],...,m[mlen-1]
... and associated data ad[0],ad[1],...,ad[adlen-1]
... and nonce npub[0],npub[1],...
... and secret key k[0],k[1],...
... the implementation shall not use nsec*/

		(void)nsec;//avoid warning
//init
	u64 W[5];
	Initialize(W,npub,k);

//ad
		ProcessAd(W,ad,adlen);

//cipher
c=Processplaintxt(W,c,m,mlen);

//finalization
	for(unsigned int i=NRMID;i<NRMID+NREND;i=i+STEP) Ronda(i^0xff,W);

	//tag:
	LESTOREu64in8bytearray(W[0],c);c+=8;
	LESTOREu64in8bytearray(W[1],c);
	*clen = mlen+16;

return 0;
}








//=========================DECRYPT=============================


int crypto_aead_decrypt(
unsigned char *m,unsigned long long *mlen,
unsigned char *nsec,
const unsigned char *c,unsigned long long clen,
const unsigned char *ad,unsigned long long adlen,
const unsigned char *npub,
const unsigned char *k
){
/*
... generating a plaintext m[0],m[1],...,m[*mlen-1]
... and secret message number nsec[0],nsec[1],...
... from a ciphertext c[0],c[1],...,c[clen-1]
... and associated data ad[0],ad[1],...,ad[adlen-1]
... and nonce number npub[0],npub[1],...
... and secret key k[0],k[1],...*/

/*if ciphertext is not valid return -1;*/

	(void)nsec;//avoid warning
//init
	u64 W[5];
	Initialize(W,npub,k);

//ad
		ProcessAd(W,ad,adlen);

//decipher
	if(clen<16) return -1;
	Processciphertxt(W,m,c,clen);
	c+=clen-16;

//finalization
	for(unsigned int i=NRMID;i<NRMID+NREND;i=i+STEP) Ronda(i^0xff,W);
	//tag check:

	unsigned char tagcheck[8];unsigned int flagtagcheck=0;
	LESTOREu64in8bytearray(W[0],tagcheck);
	for(u32 i=0;i<8;i++) if(c[i]!=tagcheck[i]){flagtagcheck=1;}
	LESTOREu64in8bytearray(W[1],tagcheck);
	for(u32 i=8;i<16;i++) if(c[i]!=tagcheck[i-8]){flagtagcheck=1;}
	if(flagtagcheck){*mlen=0; return -1;}
	*mlen=clen-16;


return 0;
}


