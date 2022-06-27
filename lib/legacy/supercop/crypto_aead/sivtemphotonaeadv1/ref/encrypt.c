#include "crypto_aead.h"

#include "siv.h"
#include "photon.h"

#define AUTH_A_DOMAIN  0
#define AUTH_M_DOMAIN  1
#define AUTH_N_DOMAIN  3
#define ENC_M_DOMAIN   7

/* Declaration of basic internal functions */
static void Setup(const unsigned char *k);

static void AUTH(
	uint8_t *state_inout,
	const unsigned char *m, unsigned long long mlen,
	const unsigned char *ad, unsigned long long adlen,
	const unsigned char *npub);

static void ENC(
	uint8_t *state_inout,
	unsigned char *c,
	const unsigned char *m, unsigned long long mlen);

static void AUTH_AD(
	uint8_t *state_inout,
	uint8_t *next_domain_bit,
	const unsigned char *ad, unsigned long long adlen);

static void AUTH_MsgNonce(
	uint8_t *state_inout,
	const unsigned char *m, unsigned long long mlen,
	const unsigned char *npub, uint8_t domain);

static void XOR_State(
	uint8_t *state_inout,
	const uint8_t *data_in,
	const size_t dlen_inbytes);


/* Definition of basic internal functions */
static void Setup(const unsigned char *k)
{
	int i;
	for (i = 0; i < (D / 2) * D; i++)
	{
		PHOTON_key_state[i / D][i % D] = (k[i / 2] >> (4 * (i & 1))) & 0xf;
	}
}

static void XOR_State(
	uint8_t *state_inout,
	const uint8_t *data_in,
	const size_t dlen_inbytes)
{
	size_t i;
	for (i = 0; i < dlen_inbytes; i++)
	{
		state_inout[i] ^= data_in[i];
	}
}

static void AUTH_AD(
	uint8_t *state_inout,
	uint8_t *next_domain,
	const unsigned char *ad, unsigned long long adlen)
{
	size_t i;
	unsigned char last_tweak_block[TWEAK_INBYTES] = { 0 };

	i = adlen;
	while (i > (STATE_INBYTES + TWEAK_INBYTES))
	{
		XOR_State(state_inout, ad, STATE_INBYTES);
		TEM_PHOTON_Permutation(state_inout, ad + STATE_INBYTES, AUTH_A_DOMAIN);
		ad = ad + (STATE_INBYTES + TWEAK_INBYTES);
		i = i - (STATE_INBYTES + TWEAK_INBYTES);
	}

	if (i < (STATE_INBYTES + TWEAK_INBYTES))
	{			
		*next_domain = AUTH_M_DOMAIN;
		
		if (i < STATE_INBYTES)
		{
			XOR_State(state_inout, ad, i);
			state_inout[i] ^= 0x80; // State ^ padn(A[a])
		}
		else
		{
			XOR_State(state_inout, ad, STATE_INBYTES);
			memcpy(last_tweak_block, ad + STATE_INBYTES, i - STATE_INBYTES);
			last_tweak_block[i - STATE_INBYTES] = 0x80; // State ^ padn(A[a])
		}
		TEM_PHOTON_Permutation(state_inout, last_tweak_block, *next_domain);
	}
	else // i == (STATE_INBYTES + TWEAK_INBYTES)
	{
		*next_domain = AUTH_M_DOMAIN + 1;
		XOR_State(state_inout, ad, STATE_INBYTES);
		TEM_PHOTON_Permutation(state_inout, ad + STATE_INBYTES, *next_domain);
	}
}

static void AUTH_MsgNonce(
	uint8_t *state_inout,
	const unsigned char *m, unsigned long long mlen,
	const unsigned char *npub, uint8_t domain)
{
	size_t i;
	unsigned char auth_n_domain;
	unsigned char last_tweak_block[TWEAK_INBYTES] = { 0 };

	i = mlen;
	while (i > (STATE_INBYTES + TWEAK_INBYTES))
	{
		XOR_State(state_inout, m, STATE_INBYTES);
		TEM_PHOTON_Permutation(state_inout, m + STATE_INBYTES, domain);
		m = m + (STATE_INBYTES + TWEAK_INBYTES);
		i = i - (STATE_INBYTES + TWEAK_INBYTES);
	}

	if (i < STATE_INBYTES)
	{
		XOR_State(state_inout, m, i);
		state_inout[i] ^= 0x80; // State ^ padn(M[m])
		auth_n_domain = AUTH_N_DOMAIN;
	}
	else if (i == STATE_INBYTES)
	{
		XOR_State(state_inout, m, STATE_INBYTES);
		auth_n_domain = AUTH_N_DOMAIN + 1;
	}
	else if (i < STATE_INBYTES + TWEAK_INBYTES)
	{
		XOR_State(state_inout, m, STATE_INBYTES);
		memcpy(last_tweak_block, m + STATE_INBYTES, i - STATE_INBYTES);
		last_tweak_block[i - STATE_INBYTES] = 0x80; // State ^ padn(M[m])
		TEM_PHOTON_Permutation(state_inout, last_tweak_block, domain);
		auth_n_domain = AUTH_N_DOMAIN + 2;
	}
	else // i == (STATE_INBYTES + TWEAK_INBYTES)
	{
		XOR_State(state_inout, m, STATE_INBYTES);
		TEM_PHOTON_Permutation(state_inout, m + STATE_INBYTES, domain);
		auth_n_domain = AUTH_N_DOMAIN + 3;
	}
	TEM_PHOTON_Permutation(state_inout, npub, auth_n_domain);
}

static void AUTH(
	uint8_t *state_inout,
	const unsigned char *m, unsigned long long mlen,
	const unsigned char *ad, unsigned long long adlen,
	const unsigned char *npub)
{
	unsigned char auth_m_domain;

	memset(state_inout, 0, STATE_INBYTES);
	AUTH_AD(state_inout, &auth_m_domain, ad, adlen);
	AUTH_MsgNonce(state_inout, m, mlen, npub, auth_m_domain);
}

static void ENC(
	uint8_t *state_inout,
	unsigned char *c,
	const unsigned char *m, unsigned long long mlen)
{
	size_t i;
	unsigned char tweak[TWEAK_INBYTES] = { 0 };
	unsigned char enc_m_domain = ENC_M_DOMAIN;
	i = mlen;
	while(i > STATE_INBYTES)
	{
		TEM_PHOTON_Permutation(state_inout, tweak, enc_m_domain);
		memcpy(c, state_inout, STATE_INBYTES);
		XOR_State(c, m, STATE_INBYTES);
		i -= STATE_INBYTES;
		c += STATE_INBYTES;
		m += STATE_INBYTES;
	}
	TEM_PHOTON_Permutation(state_inout, tweak, enc_m_domain);
	memcpy(c, state_inout, i);
	XOR_State(c, m, i);
}

int crypto_aead_encrypt(
	unsigned char *c, unsigned long long *clen,
	const unsigned char *m, unsigned long long mlen,
	const unsigned char *ad, unsigned long long adlen,
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
	... and nonce npub[0],npub[1],..
	... and secret key k[0],k[1],...
	... the implementation shall not use nsec
	...
	... return 0;
	*/
	uint8_t state[STATE_INBYTES] = { 0 };

	Setup(k);
	AUTH(state, m, mlen, ad, adlen, npub);
	memcpy(c+mlen, state, TAG_INBYTES);
	ENC(state, c, m, mlen);

	*clen = mlen + TAG_INBYTES;
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
	/*
	...
	... the code for the AEAD implementation goes here,
	... generating a plaintext m[0],m[1],...,m[*mlen-1]
	... and secret message number nsec[0],nsec[1],...
	... from a ciphertext c[0],c[1],...,c[clen-1]
	... and associated data ad[0],ad[1],...,ad[adlen-1]
	... and nonce number npub[0],npub[1],...
	... and secret key k[0],k[1],...
	...
	... return 0;
	*/

	uint8_t state[STATE_INBYTES] = { 0 };
	uint64_t cmtlen;

	if (clen < TAG_INBYTES) return TAG_UNMATCH;
	cmtlen = clen - TAG_INBYTES;

	Setup(k);
	memcpy(state, c + cmtlen, TAG_INBYTES);
	ENC(state, m, c, cmtlen);
	AUTH(state, m, cmtlen, ad, adlen, npub);

	if (memcmp(state, c + cmtlen, TAG_INBYTES) != 0)
	{
		return TAG_UNMATCH;
	}

	*mlen = cmtlen;
	return TAG_MATCH;
}