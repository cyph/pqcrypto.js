#include "crypto_hash.h"

#include "siv.h"
#include "photon.h"

#define HASH_M_DOMAIN0  0
#define HASH_M_DOMAIN1  1

/* Declaration of basic internal functions */
static void Setup(const unsigned char *k);

static void XOR_State(
	uint8_t *state_inout,
	const uint8_t *data_in,
	const uint64_t dlen_inbytes);

static void Absorb(
	uint8_t *state_inout,
	const uint8_t *data_in,
	const uint64_t dlen_inbytes,
	const uint8_t domain);

static void Squzze(
	uint8_t *tag_out,
	uint8_t *state);

/* Definition of basic internal functions */
unsigned char tweak[TWEAK_INBYTES] = { 0 };

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
	const uint64_t dlen_inbytes)
{
	size_t i;
	for (i = 0; i < dlen_inbytes; i++)
	{
		state_inout[i] ^= data_in[i];
	}
}

static void Absorb(
	uint8_t *state_inout,
	const uint8_t *data_in,
	const uint64_t dlen_inbytes,
	const uint8_t  domain)
{
	size_t i;

	i = dlen_inbytes;
	while (i > RATE_INBYTES)
	{
		XOR_State(state_inout, data_in, RATE_INBYTES);
		TEM_PHOTON_Permutation(state_inout, tweak, HASH_M_DOMAIN0);
		data_in += RATE_INBYTES;
		i -= RATE_INBYTES;
	}
	XOR_State(state_inout, data_in, i);
	TEM_PHOTON_Permutation(state_inout, tweak, domain);
	if (i < RATE_INBYTES) state_inout[i] ^= 0x80; // State ^ pad_32(M[m])
	TEM_PHOTON_Permutation(state_inout, tweak, domain);
}

static void Squzze(
	uint8_t *Tag_out,
	uint8_t *state)
{
	size_t i;

	i = TAG_INBYTES;
	while (i > SQUEEZE_RATE_INBYTES)
	{
		memcpy(Tag_out, state, SQUEEZE_RATE_INBYTES);
		TEM_PHOTON_Permutation(state, tweak, HASH_M_DOMAIN0);
		Tag_out += SQUEEZE_RATE_INBYTES;
		i -= SQUEEZE_RATE_INBYTES;
	}
	memcpy(Tag_out, state, i);
}

int crypto_hash(
	unsigned char *out,
	const unsigned char *in,
	unsigned long long inlen
)
{
	/*
	...
	... the code for the hash function implementation goes here
	... generating a hash value out[0],out[1],...,out[CRYPTO_BYTES-1]
	... from a message in[0],in[1],...,in[in-1] 
	...
	... return 0;
	*/
	uint8_t state[STATE_INBYTES] = { 0 };
	uint8_t domain;

	uint8_t k[16] = { 0 };
	Setup(k);

	if (inlen <= INITIAL_RATE_INBYTES)
	{
		memcpy(state, in, inlen);
		if (inlen < INITIAL_RATE_INBYTES)
		{
			state[inlen] ^= 0x80; // State ^ pad_128(M[m])
			domain = HASH_M_DOMAIN1;
		}
		else
		{
			domain = HASH_M_DOMAIN1 + 1;
		}
		TEM_PHOTON_Permutation(state, tweak, domain);
	}
	else
	{
		memcpy(state, in, INITIAL_RATE_INBYTES);
		TEM_PHOTON_Permutation(state, tweak, HASH_M_DOMAIN0);

		inlen -= INITIAL_RATE_INBYTES;
		domain = ((inlen % RATE_INBYTES) == 0) ? (HASH_M_DOMAIN1 + 1) : HASH_M_DOMAIN1;
		Absorb(state, in + INITIAL_RATE_INBYTES, inlen, domain);
	}
	Squzze(out, state);

	return 0;
}