#include "api.h"
#include "params.h"
#include "crypto_aead.h"
#include "casb.h"



/**********************************************************************
 * 
 * xor_bytes: XORs "num" many bytes of "in" to "out".
 * 
 **********************************************************************/		
void xor_bytes(u8 *dest, const u8 *src, u8 num)
{
	for(u8 i=0; i < num; i++)
	{
		dest[i] ^= src[i];
	}
}

/**********************************************************************
 * 
 * memcpy_ozp:	Copies in bytes to out and pads with 10* to create
 * 		CRYPTO_BLOCKBYTES-oriented data.
 * 
 **********************************************************************/
void memcpy_ozp(u8* dest, const u8 *src, u8 len)
{
	memset(dest, 0, CRYPTO_BLOCKBYTES);
	memcpy(dest, src, len);
	dest[len] ^= 0x01;
}

/**********************************************************************
 * 
 * mult_2: Field Multiplication by $2$.
 * 
 **********************************************************************/

void mult_2(u8 *out, u8 *in)
{
	u8 mask = 0b00000000;
	if(in[CRYPTO_KEYBYTES-1] & 0b10000000){
		mask = PRIM_POLY_MOD_128;
	}
	for(u8 i=CRYPTO_KEYBYTES-1; i>0; i--){
		out[i] = in[i]<<1 | in[i-1]>>7;
	}
	out[0] = in[0]<<1;
	out[0] ^= mask;
}

/**********************************************************************
 * 
 * mac: mac processing of inut with some iv. 
 * 
 **********************************************************************/
void mac(u8 *tag, u8 *cs, const u8 (*round_keys)[32], const u8 *in, const u64 inlen, const u64 in_blocks)
{
        u8 temp[CRYPTO_BLOCKBYTES],temp1[CRYPTO_BLOCKBYTES];
	for(u64 i=0; i<in_blocks-1; i++)
	{
		memcpy(temp, tag, CRYPTO_BLOCKBYTES);
		xor_bytes(temp, &in[i*CRYPTO_BLOCKBYTES], CRYPTO_BLOCKBYTES);
                xor_bytes(cs, &in[i*CRYPTO_BLOCKBYTES], CRYPTO_BLOCKBYTES);
		casb_enc(tag, &round_keys[0],temp);
	}
	if(PARTIAL_BLOCK_LEN(in_blocks,inlen) != CRYPTO_BLOCKBYTES)
	{
		memcpy_ozp(temp, &in[(in_blocks-1)*CRYPTO_BLOCKBYTES], PARTIAL_BLOCK_LEN(in_blocks, inlen));
                xor_bytes(cs, temp, CRYPTO_BLOCKBYTES);
		xor_bytes(temp, tag, CRYPTO_BLOCKBYTES);
                mult_2(temp1, temp);
                mult_2(temp, temp1);
		casb_enc(tag, &round_keys[0], temp);
	}
	else
	{
		memcpy(temp, tag, CRYPTO_BLOCKBYTES);
		xor_bytes(temp, &in[(in_blocks-1)*CRYPTO_BLOCKBYTES], CRYPTO_BLOCKBYTES);
		xor_bytes(cs, &in[(in_blocks-1)*CRYPTO_BLOCKBYTES], CRYPTO_BLOCKBYTES);
                mult_2(temp1, temp);
		casb_enc(tag, &round_keys[0], temp1);
	}
}


/**********************************************************************
 * 
 * hash: Tag generator.
 * 
 **********************************************************************/
void hash(u8 *tag, const u8 (*round_keys)[32], const u8 *nonce, const u8 *ad, const u64 adlen, const u64 ad_blocks, const u8 *pt, const u64 ptlen, const u64 pt_blocks)
{
	u8 cs[CRYPTO_BLOCKBYTES]; 
	u8 const1 = 0x03;
	u8 const2 = 0x02;
	u8 const3 = 0x01;
	u8 temp[CRYPTO_BLOCKBYTES] = { 0 };
 	memset(tag, 0x00, CRYPTO_ABYTES);
	memset(cs, 0x00, CRYPTO_ABYTES);
	memcpy(temp, nonce, CRYPTO_BLOCKBYTES);
        if ((adlen > 0) && (ptlen > 0))
 		cs[0] ^= const1;
        if ((adlen > 0) && (ptlen == 0))
                cs[0] ^= const3;
        if ((adlen == 0) && (ptlen > 0))
                cs[0] ^= const2;
	casb_enc(&tag[0], &round_keys[0], &cs[0]);
	xor_bytes(&tag[0], &temp[0], CRYPTO_BLOCKBYTES);
	xor_bytes(&cs[0], &temp[0], CRYPTO_BLOCKBYTES);
	casb_enc(&tag[0], &round_keys[0], &tag[0]);
	
	if(adlen > 0)
	{
		
		mac(&tag[0], &cs[0],&round_keys[0], ad, adlen, ad_blocks);
		xor_bytes(&tag[0], &cs[0], CRYPTO_BLOCKBYTES);
		casb_enc(&tag[0], &round_keys[0], &tag[0]);
	}
	if(ptlen > 0)
	{
	
		mac(&tag[0], &cs[0],&round_keys[0], pt, ptlen, pt_blocks);
		xor_bytes(&tag[0], &cs[0], CRYPTO_BLOCKBYTES);
		casb_enc(&tag[0], &round_keys[0], &tag[0]);
	}
	if(adlen == 0 && ptlen == 0)
	{
		xor_bytes(&tag[0], &cs[0], CRYPTO_BLOCKBYTES);		
		casb_enc(&tag[0], &round_keys[0], &tag[0]);
	}
}

/**********************************************************************
 * 
 * ofb:	Ciphertext generation using the tag.
 * 
 **********************************************************************/
void ofb(u8 *out, u64 *outlen, const u8 (*round_keys)[32], const u8 *iv, const u8 *in, const u64 inlen, const u64 in_blocks)
{
	
	
	u8 iv_[CRYPTO_BLOCKBYTES];
	u8 temp[CRYPTO_BLOCKBYTES];

	*outlen = 0;

	// process non-last blocks
	memcpy(iv_, iv, CRYPTO_BLOCKBYTES);
	for(u8 i=0; i<in_blocks-1; i++)
	{
		memcpy(temp, iv_, CRYPTO_BLOCKBYTES);
		casb_enc(iv_, &round_keys[0],temp);
		memcpy(&out[i*CRYPTO_BLOCKBYTES], &in[i*CRYPTO_BLOCKBYTES], CRYPTO_BLOCKBYTES);
		xor_bytes(&out[i*CRYPTO_BLOCKBYTES], iv_, CRYPTO_BLOCKBYTES);
		*outlen += CRYPTO_BLOCKBYTES;
	}
	
	// process last block (may be partial)
	memcpy(temp, iv_, CRYPTO_BLOCKBYTES);
	casb_enc(iv_, &round_keys[0],temp);
	memcpy(&out[(in_blocks-1)*CRYPTO_BLOCKBYTES], &in[(in_blocks-1)*CRYPTO_BLOCKBYTES], PARTIAL_BLOCK_LEN(in_blocks, inlen));
	xor_bytes(&out[(in_blocks-1)*CRYPTO_BLOCKBYTES], iv_, PARTIAL_BLOCK_LEN(in_blocks, inlen));
	*outlen += PARTIAL_BLOCK_LEN(in_blocks, inlen);
}

/**********************************************************************
 * 
 * crypto_aead_encrypt:	Main encryption function.
 * 
 **********************************************************************/
int crypto_aead_encrypt(
	unsigned char *ct, unsigned long long *ctlen,
	const unsigned char *pt, unsigned long long ptlen,
	const unsigned char *ad, unsigned long long adlen,
	const unsigned char *nsec,
	const unsigned char *npub,
	const unsigned char *k
)
{
	// to bypass unused warning on nsec 
	nsec = nsec;

	
	u8 tag[CRYPTO_ABYTES] = { 0 };
	
	// initialize and generate round keys
	u64 pt_blocks = ptlen%CRYPTO_BLOCKBYTES ? ((ptlen/CRYPTO_BLOCKBYTES)+1) : (ptlen/CRYPTO_BLOCKBYTES);
	u64 ad_blocks = adlen%CRYPTO_BLOCKBYTES ? ((adlen/CRYPTO_BLOCKBYTES)+1) : (adlen/CRYPTO_BLOCKBYTES);
	
	u8 round_keys[CRYPTO_BC_NUM_ROUNDS][32];
	_casb_ENC_ROUND_KEY_GEN(&round_keys[0], &k[0]);
	
	// generate tag
	hash(&tag[0], &round_keys[0], npub, ad, adlen, ad_blocks, pt, ptlen, pt_blocks);
	// generate ciphertext
	if(pt_blocks != 0)
	{
		ofb(ct, ctlen, &round_keys[0], &tag[0], pt, ptlen, pt_blocks);
	}
	else
	{
		*ctlen = 0;
	}
	// append tag to ciphertext
	memcpy(&ct[*ctlen],&tag[0],CRYPTO_ABYTES);
	*ctlen += CRYPTO_ABYTES;
	
	return 0;
}

/**********************************************************************
 * 
 * crypto_aead_decrypt:	Main decryption function. 
 * 
 **********************************************************************/
int crypto_aead_decrypt(
	unsigned char *pt, unsigned long long *ptlen,
	unsigned char *nsec,
	const unsigned char *ct, unsigned long long ctlen,
	const unsigned char *ad, unsigned long long adlen,
	const unsigned char *npub,
	const unsigned char *k
)
{
	int pass = 0;
	
	// to bypass unused warning on nsec
	nsec = nsec;
	
	u8 tag[CRYPTO_ABYTES] = { 0 };
	
	// extract tag (to be used as IV for OFB mode) from ciphertext, reflect the change in ciphertext length
	memcpy(&tag[0], &ct[ctlen-CRYPTO_ABYTES], CRYPTO_ABYTES);
	ctlen -= CRYPTO_ABYTES;
	
	// initialize and generate round keys
	u64 ct_blocks = ctlen%CRYPTO_BLOCKBYTES ? ((ctlen/CRYPTO_BLOCKBYTES)+1) : (ctlen/CRYPTO_BLOCKBYTES);
	u64 ad_blocks = adlen%CRYPTO_BLOCKBYTES ? ((adlen/CRYPTO_BLOCKBYTES)+1) : (adlen/CRYPTO_BLOCKBYTES);
	
	u8 round_keys[CRYPTO_BC_NUM_ROUNDS][32];
	_casb_ENC_ROUND_KEY_GEN(&round_keys[0], &k[0]);
	
	// generate plaintext
	if(ct_blocks != 0)
	{
		ofb(pt, ptlen, &round_keys[0], &tag[0], ct, ctlen, ct_blocks);
	}
	else
	{
		*ptlen = 0;
	}
	
	// generate tag
	u64 pt_blocks = *ptlen%CRYPTO_BLOCKBYTES ? ((*ptlen/CRYPTO_BLOCKBYTES)+1) : (*ptlen/CRYPTO_BLOCKBYTES);
	hash(&tag[0], &round_keys[0], npub, ad, adlen, ad_blocks, pt, *ptlen, pt_blocks);

	// check computed tag =? received tag (0 if equal)
	pass = memcmp(tag, &ct[*ptlen], CRYPTO_ABYTES);
	
	if(!pass)
	{
		return pass;
	}
	else
	{
		return -1;
	}
}
