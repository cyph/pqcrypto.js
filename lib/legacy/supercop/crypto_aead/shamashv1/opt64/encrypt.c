#include "crypto_aead.h"


/** 
*  Designers: Daniel Penazzi, Miguel Montes
This codification: Daniel Penazzi.
 
* This code is hereby placed in the public domain.
*/

typedef unsigned long long u64;
typedef unsigned int u32;


#define Sbox(S0,S1,S2,S3,S4) {\
	unsigned long long xx=S0;\
	unsigned long long yy=S1;\
	unsigned long long zz=S2;\
	unsigned long long uu=S3;\
	unsigned long long vv=S4;\
	unsigned long long or0=(zz|(uu^yy));\
	unsigned long long or1=(uu|(vv^zz));\
	unsigned long long or2=(vv|(xx^uu));\
	unsigned long long or3=(xx|(yy^vv));\
	unsigned long long or4=(yy|(zz^xx));\
	S1=xx^or0;S2=yy^or1;S3=zz^or2;S4=(~uu)^or3;S0=vv^or4;\
	or0=(yy|vv);\
	or1=(zz|xx);\
	or2=(uu|yy);\
	or3=(vv|zz);\
	or4=(xx|uu);\
S1^=or0;S2^=or1;S3^=or2;S4^=or3;S0^=or4;\
}



#define ROTR(x,n) (((x)>>(n))|((x)<<(64-(n))))


#define Difusion(x,y,z,u,v) {\
	u^= ROTR(u,57)^ROTR(u,63);\
	v^= ROTR(v,3)^ROTR(v,26);\
	x^= ROTR(x,43)^ROTR(x,62);\
	y^= ROTR(y,21)^ROTR(y,46);\
	z^= ROTR(z,58)^ROTR(z,61);\
	x^=u^v;y^=u^v;z^=u^v;\
	u^=z^x^y;v^=z^x^y;\
	x=ROTR(x,8);\
	y=ROTR(y,24);\
	z=ROTR(z,40);\
	u=ROTR(u,56);\
}


//number of rounds of initialization, times 11 (12x11)
#define NRINIT 132

//number of rounds of processing ad and message, times 11
#define NRMID 99

//extra number of rounds of finalization (after NRMID rounds are done), times 11
#define NREND 33

//increase of i for the round constants
#define STEP 11






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


#define Ronda(Rconst,S0,S1,S2,S3,S4) {\
	S0^=Rconst;\
	Sbox(S0,S1,S2,S3,S4)\
	Difusion(S0,S1,S2,S3,S4)\
}




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
	u64 x,y,z,u,v;
	x=LELOAD8bytearrayinu64(k);k+=8;
	y=LELOAD8bytearrayinu64(k);
	z=LELOAD8bytearrayinu64(npub);npub+=8;
	u=LELOAD8bytearrayinu64(npub);
	v=((x<<32)^(x>>32))^((y<<32)^(y>>32))^0xff;

	
	for(unsigned int i=0;i<NRINIT;i=i+STEP) Ronda(i^0xff,x,y,z,u,v);
	
//ad
		u64 remain=adlen;		
		while(remain>=16){
			u^=LELOAD8bytearrayinu64(ad);	ad+=8;
			z^=LELOAD8bytearrayinu64(ad);
			for(unsigned int  i=0;i<NRMID;i=i+STEP) Ronda(i^0xad,x,y,z,u,v);
			remain-=16;
			ad+=8;
		}//endwhile
		if(remain){
			if(remain<8){
			u^=PartialLELOADbytearrayinu64(ad,remain);
			u^=((u64)(1)<<(8*remain));
			}
			else{
				u^=LELOAD8bytearrayinu64(ad);	ad+=8;remain-=8;
				if(remain) z^=PartialLELOADbytearrayinu64(ad,remain);
				z^=((u64)(1)<<(8*remain));
			}
			y^=1;//distinguisher from remain=0 case
			for(unsigned int  i=0;i<NRMID;i=i+STEP) Ronda(i^0xad,x,y,z,u,v);
		}//endifremain

	y^=1;//delimiter ad/mesg

//cipher
		remain=mlen;
		while(remain>=16){
			u^=LELOAD8bytearrayinu64(m);
			LESTOREu64in8bytearray(u,c);m+=8;c+=8;
			z^=LELOAD8bytearrayinu64(m);
			LESTOREu64in8bytearray(z,c);

			for(unsigned int  i=0;i<NRMID;i=i+STEP) Ronda(i^0xff,x,y,z,u,v);
			remain-=16;
			m+=8;c+=8;
		}//endwhile

		if(remain){
			if(remain<8){
				u^=PartialLELOADbytearrayinu64(m,remain);
				PartialLESTOREu64inbytearray(u,c,remain);
				u^=((u64)(1)<<(8*remain));
			}
			else{
				u^=LELOAD8bytearrayinu64(m);
				LESTOREu64in8bytearray(u,c);m+=8;c+=8;remain-=8;
				if(remain){
					z^=PartialLELOADbytearrayinu64(m,remain);
					PartialLESTOREu64inbytearray(z,c,remain);
				}
				z^=((u64)(1)<<(8*remain));
			}
			c+=remain;
			y^=1;//distinguisher from remain=0 case
			
			for(unsigned int  i=0;i<NRMID;i=i+STEP) Ronda(i^0xff,x,y,z,u,v);
		}//endifremain


	for(unsigned int  i=NRMID;i<NRMID+NREND;i=i+STEP) Ronda(i^0xff,x,y,z,u,v);

	//tag:
	LESTOREu64in8bytearray(x,c);c+=8;
	LESTOREu64in8bytearray(y,c);
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
	u64 x,y,z,u,v;
	x=LELOAD8bytearrayinu64(k);k+=8;
	y=LELOAD8bytearrayinu64(k);
	z=LELOAD8bytearrayinu64(npub);npub+=8;
	u=LELOAD8bytearrayinu64(npub);
	v=((x<<32)^(x>>32))^((y<<32)^(y>>32))^0xff;

	for(unsigned int  i=0;i<NRINIT;i=i+STEP) Ronda(i^0xff,x,y,z,u,v);
//ad
		u64 remain=adlen;		
		while(remain>=16){
			u^=LELOAD8bytearrayinu64(ad);	ad+=8;
			z^=LELOAD8bytearrayinu64(ad);
			for(unsigned int  i=0;i<NRMID;i=i+STEP) Ronda(i^0xad,x,y,z,u,v);
			remain-=16;
			ad+=8;
		}//endwhile
		if(remain){
			if(remain<8){
			u^=PartialLELOADbytearrayinu64(ad,remain);
			u^=((u64)(1)<<(8*remain));
			}
			else{
				u^=LELOAD8bytearrayinu64(ad);	ad+=8;remain-=8;
				if(remain) z^=PartialLELOADbytearrayinu64(ad,remain);
				z^=((u64)(1)<<(8*remain));
			}
			y^=1;//distinguisher from remain=0 case
			for(unsigned int  i=0;i<NRMID;i=i+STEP) Ronda(i^0xad,x,y,z,u,v);
		}//endifremain

	y^=1;//delimiter ad/mesg

//decipher
	if(clen<16) return -1;
	remain=clen-16;
		while(remain>=16){
			LESTOREu64in8bytearray(u,m);
			for(u32 sk=0;sk<8;sk++) m[sk]^=c[sk];
			u=LELOAD8bytearrayinu64(c);m+=8;c+=8;

			LESTOREu64in8bytearray(z,m);
			for(u32 sk=0;sk<8;sk++) m[sk]^=c[sk];
			z=LELOAD8bytearrayinu64(c);

			for(unsigned int  i=0;i<NRMID;i=i+STEP) Ronda(i^0xff,x,y,z,u,v);
			remain-=16;
			m+=8;c+=8;
		}//endwhile

		if(remain){
			if(remain<8){
				PartialLESTOREu64inbytearray(u,m,remain);//m=trunc(u)
				for(u32 s=0;s<remain;s++) m[s]^=c[s];
				//now m is trunc(u)^c
				u^=PartialLELOADbytearrayinu64(m,remain);/*u^=(u^c) gives u=c in msg bytes ,
																									while u^=0 leaves the other ones untouched*/
				c+=remain;
				u^=((u64)(1)<<(8*remain));
			}
			else{
				LESTOREu64in8bytearray(u,m);
				for(u32 sk=0;sk<8;sk++) m[sk]^=c[sk];
				u=LELOAD8bytearrayinu64(c);m+=8;c+=8;remain-=8;
				if(remain){
					PartialLESTOREu64inbytearray(z,m,remain);
					for(u32 s=0;s<remain;s++) m[s]^=c[s];
					z^=PartialLELOADbytearrayinu64(m,remain);
				}
				z^=((u64)(1)<<(8*remain));
				c+=remain;
			}
			y^=1;//distinguisher from remain=0 case
			for(unsigned int  i=0;i<NRMID;i=i+STEP) Ronda(i^0xff,x,y,z,u,v);
		}//endifremain


	for(unsigned int i=NRMID;i<NRMID+NREND;i=i+STEP) Ronda(i^0xff,x,y,z,u,v);
	//tag check:

	unsigned char tagcheck[8];unsigned int flagtagcheck=0;
	LESTOREu64in8bytearray(x,tagcheck);
	for(u32 i=0;i<8;i++) if(c[i]!=tagcheck[i]){flagtagcheck=1;}
	LESTOREu64in8bytearray(y,tagcheck);
	for(u32 i=8;i<16;i++) if(c[i]!=tagcheck[i-8]){flagtagcheck=1;}
  	if(flagtagcheck){*mlen=0; return -1;}

	*mlen=clen-16;

	

return 0;
}


