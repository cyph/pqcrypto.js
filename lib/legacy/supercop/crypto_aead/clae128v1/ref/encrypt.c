#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "crypto_aead.h"
#include "api.h"

typedef unsigned char BYTE;
#define CRYPTO_TAGBYTES 8

void search(const BYTE k[16], BYTE b, BYTE o[4]){
    BYTE f = k[b>>4]+k[b&0x0F];
    b = (f>>3|f<<5)^b; 
    BYTE b0 = (b&0x1E)>>1; 
    BYTE b1 = (b&0x01)<<3|b>>5;
    BYTE b2 = (b&0x3C)>>2; 
    BYTE b3 = (b&0x03)<<2|b>>6;
    o[0] = k[b0]+(b2<<1); 
    o[1] = k[b1]+(b3<<3);  
    o[2] = k[b2]+(b1<<1); 
    o[3] = k[b3]+(b0<<3);

}

void vecIV(const BYTE k[16], const BYTE *iv, unsigned long long ivlen, BYTE s0, BYTE s1, BYTE o[2]){\
	BYTE i, b0, b1;

    o[0] = s0;
    o[1] = s1;
    for(i=0; i< ivlen; i++){
        b0 = iv[i]&0x0F;
		o[0] = (o[0]+k[o[1]>>4]+k[o[0]&0x0F]+k[b0])^b0;
        b1 = iv[ivlen-i-1]>>4; 
		o[1] = (o[1]+k[o[0]>>4]+k[o[1]&0x0F]+k[b1])^b0^b1;	

   }
    	
}

void spill(const BYTE k[16], BYTE* c, unsigned long long clen, unsigned long long i){ 
            BYTE o[4]; 
			unsigned long long l, r0, r1; 
            search(k, c[i], o); 
            if (i>0) l=i-1; else l=clen-1; 
            if(i<clen-1) r0= i +1; else r0=0; 
 
            r1 = r0+((o[0]+o[3])&0x0F);
			if(r1>=clen) r1 = r1-clen;
            if(r1==i) 
            {if(r0<clen-1) r1=r0+1; else r1=0;}
                 
            c[l] = c[l]^(o[0]+o[1]); 
            c[r1] = c[r1]^(o[1]+o[2]);
            c[r0] = c[r0]^(o[2]+o[3]);
}

BYTE spillA(const BYTE k[16], BYTE s, BYTE* c, unsigned long long clen, unsigned long long i, BYTE b){ 
            BYTE o[4]; 
			unsigned long long l0, r0;
            search(k, (k[b>>4]+k[b&0xF]+s)^(b>>4), o);

			l0 = i+(s&0x0F); 
            if(l0>=clen) l0 = l0-clen;
			r0 = i+((o[0]+o[1])&0x0F); 
			if(r0>=clen) r0 = r0-clen;

            c[l0] = c[l0]^(o[0]+o[1]+s)^(b&0x55); 
			c[r0] = c[r0]^(o[2]+o[3]+s)^(b&0xAA);

			return (o[0]+o[2]+o[3]);
}


BYTE enc_byte_p(const BYTE k[16], BYTE s0, BYTE s1, BYTE m){
    BYTE o[4];
    search(k, s0^s1, o);
    BYTE c0 = (s1*o[3]+s0*o[2])^(m&0x0F);
    BYTE c1 = (s0*o[1]+s1*o[0])^(m&0xF0);
	return c0^c1;
}

BYTE dec_byte_p(const BYTE k[16], BYTE s0, BYTE s1, BYTE c){    
    BYTE o[4]; 
    search(k, s0^s1, o); 
    BYTE m = c^(s1*o[3]+s0*o[2])^(s0*o[1]+s1*o[0]);
    return m;
}

void enc_byte(const BYTE k[16], BYTE s[2], const BYTE iv[CRYPTO_NPUBBYTES], BYTE m, BYTE c[2]){
    BYTE o[4], b[2];
    search(k, s[0], o);

    vecIV(k, iv, CRYPTO_NPUBBYTES/2, s[0], s[1], b);
    c[0] = (o[0]*b[0]+o[1]*b[1])^(m&0x0F);  
    vecIV(k, iv+CRYPTO_NPUBBYTES/2, CRYPTO_NPUBBYTES/2, s[1], c[0], b);
    c[1] = (o[2]*b[0]+o[3]*b[1])^(m>>4);

    c[0] = c[0]+(c[1]>>3|c[1]<<5);
    memcpy(o, c, 2);

    c[0] = c[0]+s[1];
    c[1] = c[1]+s[0];

    memcpy(s, o, 2);
}

int dec_byte(const BYTE k[16], BYTE s[2], const BYTE iv[CRYPTO_NPUBBYTES], BYTE c[2]){    
    BYTE o[4], b[2], c0; 

    c[0] = c[0]-s[1];
    c[1] = c[1]-s[0];

    c0 = c[0]-(c[1]>>3|c[1]<<5);
    search(k, s[0], o); 
    vecIV(k, iv+CRYPTO_NPUBBYTES/2, CRYPTO_NPUBBYTES/2, s[1], c0, b);
    BYTE m1 = c[1]^(o[2]*b[0]+o[3]*b[1]);
    vecIV(k, iv, CRYPTO_NPUBBYTES/2, s[0], s[1], b);
    BYTE m0 = c0^(o[0]*b[0]+o[1]*b[1]);

    memcpy(s, c, 2);
    if ( m0>0x0F || m1>0x0F){  
        return -1;
    }
	
    return m0+(m1<<4);
}

int crypto_aead_encrypt(
	unsigned char *c, unsigned long long *clen,
	const unsigned char *m, unsigned long long mlen,
	const unsigned char *ad, unsigned long long adlen,
	const unsigned char *nsec,
	const unsigned char *npub,
	const unsigned char *k
){
    unsigned long long i=0, j=0; BYTE plen=0;

	if(mlen<CRYPTO_TAGBYTES)
		plen = CRYPTO_TAGBYTES-mlen;

    *clen = CRYPTO_TAGBYTES+mlen+plen;

    BYTE s[2];  s[0]=0; 
    for(i=0; i < 16; i++) s[0] = s[0]+k[i]; 

    vecIV(k, npub, CRYPTO_NPUBBYTES, s[0], s[0]>>3|s[0]<<5, s);

    for(i=0; i< CRYPTO_TAGBYTES; i++){
		if(i<mlen)
       		enc_byte(k, s, npub, m[i], c+i*2);
        else
			enc_byte(k, s, npub, 0xFF, c+i*2);
	}

	for(i=0; i <CRYPTO_TAGBYTES-1; i++){                  	
			c[i*2+0] = c[i*2+0]+c[(i+1)*2+0];
            c[i*2+1] = c[i*2+1]+c[(i+1)*2+1];			
	} 

    for(i=CRYPTO_TAGBYTES; i <mlen; i++){
			c[CRYPTO_TAGBYTES*2+i-CRYPTO_TAGBYTES] =  enc_byte_p(k, c[i-CRYPTO_TAGBYTES], c[CRYPTO_TAGBYTES*2+i-CRYPTO_TAGBYTES-1], m[i]);
			c[i-CRYPTO_TAGBYTES+1] = (c[i-CRYPTO_TAGBYTES+1]+(c[CRYPTO_TAGBYTES*2+i-CRYPTO_TAGBYTES]))^(m[i]&0xAA);
			c[i-CRYPTO_TAGBYTES+2] = (c[i-CRYPTO_TAGBYTES+2]+(c[CRYPTO_TAGBYTES*2+i-CRYPTO_TAGBYTES]))^(m[i]&0x55);
	}

    s[0]=0; j=0;
    for(i=0; i < 16; i++) s[0] = s[0]+k[i];  
    for(i=0; i < adlen;i++){
		s[0] = spillA(k, s[0], c, *clen, j++, ad[i]);
		if(j>*clen) j=0;	
	}

    for(i=1; i < *clen; i++)
		c[*clen-i-1] = c[*clen-i-1]+(c[*clen-i]>>5|c[*clen-i]<<3);

    for(i=0; i < *clen; i++)
		spill(k, c, *clen, i);

    for(i=0; i < *clen; i++)
		spill(k, c, *clen, i);

    return 0;
}

int crypto_aead_decrypt(
	unsigned char *m, unsigned long long *mlen,
	unsigned char *nsec,
	const unsigned char *c, unsigned long long clen,
	const unsigned char *ad, unsigned long long adlen,
	const unsigned char *npub,
	const unsigned char *k
){
    unsigned long long i=0, j=0; int l;     BYTE s[2]; 

    BYTE* cp = (BYTE*) malloc(clen);
    memcpy(cp, c, clen);
    *mlen = clen - CRYPTO_TAGBYTES;

    for(i=0; i < clen; i++)  		
		spill(k, cp, clen, clen-1 - i);

    for(i=0; i < clen; i++)  		
		spill(k, cp, clen, clen-1 - i);	

    for(i=1; i < clen; i++)
		cp[i-1] = cp[i-1]-(cp[i]>>5|cp[i]<<3);	


    s[0] =0; j=0;
    for(i=0; i < 16; i++) s[0] = s[0]+k[i]; 
	for(i=0; i < adlen; i++){
		s[0] = spillA(k, s[0], cp, clen, j++, ad[i]);
		if(j>clen) j=0;
	}


    for(l=*mlen-1; l >= CRYPTO_TAGBYTES; l--){
			m[l] = dec_byte_p(k, cp[l-CRYPTO_TAGBYTES], cp[CRYPTO_TAGBYTES*2+l-CRYPTO_TAGBYTES-1], cp[CRYPTO_TAGBYTES*2+l-CRYPTO_TAGBYTES]);
            cp[l-CRYPTO_TAGBYTES+1] = (cp[l-CRYPTO_TAGBYTES+1]^(m[l]&0xAA))-(cp[CRYPTO_TAGBYTES*2+l-CRYPTO_TAGBYTES]);
            cp[l-CRYPTO_TAGBYTES+2] = (cp[l-CRYPTO_TAGBYTES+2]^(m[l]&0x55))-(cp[CRYPTO_TAGBYTES*2+l-CRYPTO_TAGBYTES]);
	}

    for(l=CRYPTO_TAGBYTES-2; l >=0; l--){  
			cp[l*2+0] = cp[l*2+0]-cp[(l+1)*2+0];
			cp[l*2+1] = cp[l*2+1]-cp[(l+1)*2+1];			
	}

    s[0]=0; j=0;
    for(i=0; i < 16; i++) s[0] = s[0]+k[i]; 
    vecIV(k, npub, CRYPTO_NPUBBYTES, s[0], s[0]>>3|s[0]<<5, s);

    for(i=0; i < CRYPTO_TAGBYTES; i++){
      int res = dec_byte(k, s, npub, cp+i*2);
      j=j+(res==-1);
      m[i] = (BYTE) res;
    }

	if(*mlen==CRYPTO_TAGBYTES){
		for(i=0; i < CRYPTO_TAGBYTES; i++)
			if(m[CRYPTO_TAGBYTES-i-1]==0xFF)
				*mlen = *mlen -1; 
			else 
				break;
	}

    free(cp);
    return (-1*(j>0));

}
