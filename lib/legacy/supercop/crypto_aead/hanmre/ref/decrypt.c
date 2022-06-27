#include <stdio.h>
#include <string.h>
#include "api.h"
#include "hanmre.h"
#include "crypto_aead.h"

int crypto_aead_decrypt(
	unsigned char *m,unsigned long long *mlen,
	unsigned char *nsec,
	const unsigned char *c,unsigned long long clen,
	const unsigned char *ad,unsigned long long adlen,
	const unsigned char *npub,
	const unsigned char *k
){
	// Plaintext length is clen - tag length
		*mlen = clen-HANMRE_TAG_SZ;

		unsigned char p_tag[HANMRE_TAG_SZ], c_tag[HANMRE_TAG_SZ];
		// decrypt -> P (m)
		for(int i=0;i<HANMRE_TAG_SZ;i++){
			p_tag[i] = c[(*mlen)+i];
		}

		hanmre_enc(k,p_tag,c,*mlen,m);
		// Generate tag
		
		hanmre_mac(ad,adlen,m,*mlen,c_tag);

		// Compare tag
		unsigned long eq_cnt = 0;
		for(size_t i = 0; i < HANMRE_TAG_SZ; i++) {
			eq_cnt += (c_tag[i] == c[(*mlen)+i]);
		}
		// Perform decryption if tag is correct
		if(eq_cnt == (unsigned long)HANMRE_TAG_SZ){
            return 0;
		} 
		else 
		{
			return -1;
		}
	
}
