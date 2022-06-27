#include <stdio.h>
#include <string.h>
#include "api.h"
#include "hanmre.h"
#include "crypto_aead.h"

int crypto_aead_encrypt(
	unsigned char *c,unsigned long long *clen,
	const unsigned char *m,unsigned long long mlen,
	const unsigned char *ad,unsigned long long adlen,
	const unsigned char *nsec,
	const unsigned char *npub,
	const unsigned char *k
){	
    // Ciphertext length is mlen + tag length
    *clen = mlen+HANMRE_TAG_SZ;
    unsigned char *tag = c+mlen;

    // Generate tag
    hanmre_mac(ad,adlen,m,mlen,tag);
	
    // Encrypt plaintext
    hanmre_enc(k,tag,m,mlen,c);
    
    return 0;
}
