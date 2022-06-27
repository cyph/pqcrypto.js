#include <string.h>
#include <stdint.h>
#include "crypto_aead.h"

extern void morus_stateupdate(const uint64_t* msgblk, uint64_t state[][4]);
extern void morus_enc_aut_step_looped(const uint8_t* plaintextblock, uint8_t* ciphertextblock, uint64_t state[5][4], unsigned long long* loop_times);
extern void morus_dec_aut_step_looped(uint8_t* plaintextblock, const uint8_t* ciphertextblock, uint64_t state[5][4], unsigned long long* loop_times);
extern void morus_enc_aut_step_ad(const uint8_t* plaintextblock, uint64_t state[5][4], unsigned long long* loop_times);

void morus_initialization(const uint8_t* key, const uint8_t* iv, uint64_t state[][4])
{
	int i;
	uint64_t temp[4] __attribute__((aligned(256))) = { 0, 0, 0, 0 };
	uint8_t con[32] = { 0x0, 0x1, 0x01, 0x02, 0x03, 0x05, 0x08, 0x0d, 0x15, 0x22, 0x37, 0x59, 0x90, 0xe9, 0x79, 0x62, 0xdb, 0x3d, 0x18, 0x55, 0x6d, 0xc2, 0x2f, 0xf1, 0x20, 0x11, 0x31, 0x42, 0x73, 0xb5, 0x28, 0xdd };

	memcpy(state[0], iv, 16); memset(state[0]+2, 0, 16);
	memcpy(state[1], key, 32);
	memset(state[2], 0xff, 32);
	memset(state[3], 0, 32);
	memcpy(state[4], con, 32);

	for (i = 0; i < 16; i++) morus_stateupdate(temp, state);
	for (i = 0; i < 4;  i++) state[1][i] ^= ((uint64_t*)key)[i];
}

void morus_tag_generation(uint64_t msglen, uint64_t adlen, uint8_t* c, uint64_t state[][4])
{
	int i;
	uint8_t t[32] __attribute__((aligned(256)));

	((uint64_t*)t)[0] = (adlen  << 3);
	((uint64_t*)t)[1] = (msglen << 3);
	((uint64_t*)t)[2] = 0;
	((uint64_t*)t)[3] = 0;

	state[4][0] ^= state[0][0]; state[4][1] ^= state[0][1]; state[4][2] ^= state[0][2]; state[4][3] ^= state[0][3];

	for (i = 0; i < 10; i++) morus_stateupdate((uint64_t*)t, state);

	state[0][0] = state[0][0] ^ state[1][1] ^ (state[2][0] & state[3][0]);
	state[0][1] = state[0][1] ^ state[1][2] ^ (state[2][1] & state[3][1]);

	memcpy(c+msglen, state[0], 16);
}

int morus_tag_verification(uint64_t msglen, uint64_t adlen, const uint8_t* c, uint64_t state[][4])
{
	int i;
	uint8_t t[32] __attribute__((aligned(256)));
	int check = 0;

	((uint64_t*)t)[0] = (adlen << 3);
	((uint64_t*)t)[1] = (msglen << 3);
	((uint64_t*)t)[2] = 0;
	((uint64_t*)t)[3] = 0;

	state[4][0] ^= state[0][0]; state[4][1] ^= state[0][1]; state[4][2] ^= state[0][2]; state[4][3] ^= state[0][3];

	for (i = 0; i < 10; i++) morus_stateupdate((uint64_t*)t, state);

	state[0][0] = state[0][0] ^ state[1][1] ^ (state[2][0] & state[3][0]);
	state[0][1] = state[0][1] ^ state[1][2] ^ (state[2][1] & state[3][1]);

	for (i = 0; i  < 16; i++) check |= (c[msglen + i] ^ ((uint8_t *)state[0])[i]);
	if (0 == check) return 0;
	else return -1;
}

void morus_enc_aut_partialblock(const uint8_t* plaintext, uint8_t* ciphertext, uint64_t len, uint64_t state[][4])
{
	uint8_t plaintextblock[32] __attribute__((aligned(256)));
	uint8_t ciphertextblock[32] __attribute__((aligned(256)));

	memset(plaintextblock, 0, 32);
	memcpy(plaintextblock, plaintext, len);

	((uint64_t*)ciphertextblock)[0] = ((uint64_t*)plaintextblock)[0] ^ state[0][0] ^ state[1][1] ^ (state[2][0] & state[3][0]);
	((uint64_t*)ciphertextblock)[1] = ((uint64_t*)plaintextblock)[1] ^ state[0][1] ^ state[1][2] ^ (state[2][1] & state[3][1]);
	((uint64_t*)ciphertextblock)[2] = ((uint64_t*)plaintextblock)[2] ^ state[0][2] ^ state[1][3] ^ (state[2][2] & state[3][2]);
	((uint64_t*)ciphertextblock)[3] = ((uint64_t*)plaintextblock)[3] ^ state[0][3] ^ state[1][0] ^ (state[2][3] & state[3][3]);
	memcpy(ciphertext, ciphertextblock, len);

	morus_stateupdate(((uint64_t*)plaintextblock), state);
}

void morus_dec_aut_partialblock(uint8_t* plaintext, const uint8_t* ciphertext, unsigned long len, uint64_t state[][4])
{
	uint8_t plaintextblock[32] __attribute__((aligned(256)));
	uint8_t ciphertextblock[32] __attribute__((aligned(256)));

	memset(ciphertextblock, 0, 32);
	memcpy(ciphertextblock, ciphertext, len);

	((uint64_t*)plaintextblock)[0] = ((uint64_t*)ciphertextblock)[0] ^ state[0][0] ^ state[1][1] ^ (state[2][0] & state[3][0]);
	((uint64_t*)plaintextblock)[1] = ((uint64_t*)ciphertextblock)[1] ^ state[0][1] ^ state[1][2] ^ (state[2][1] & state[3][1]);
	((uint64_t*)plaintextblock)[2] = ((uint64_t*)ciphertextblock)[2] ^ state[0][2] ^ state[1][3] ^ (state[2][2] & state[3][2]);
	((uint64_t*)plaintextblock)[3] = ((uint64_t*)ciphertextblock)[3] ^ state[0][3] ^ state[1][0] ^ (state[2][3] & state[3][3]);

	memcpy(plaintext, plaintextblock, len);
	memset(plaintextblock, 0, 32);
	memcpy(plaintextblock, plaintext, len);

	morus_stateupdate(((uint64_t*)plaintextblock), state);
}

int crypto_aead_encrypt(
	unsigned char* c,unsigned long long* clen,
	const unsigned char* m,unsigned long long mlen,
	const unsigned char* ad,unsigned long long adlen,
	const unsigned char* nsec,
	const unsigned char* npub,
	const unsigned char* k
	)
{
	uint8_t ciphertextblock[32] __attribute__((aligned(256)));
	uint64_t morus_state[5][4] __attribute__((aligned(256)));

	// Initialization
	morus_initialization(k, npub, morus_state);

	// Process the associated data
	unsigned long long length __attribute((aligned(64))) = adlen / 32;
	if (length != 0) {
		morus_enc_aut_step_ad(ad, morus_state, &length);
	}

	// Deal with the partial block of associated data
	// In this program, we assume that the message length is a multiple of bytes
	if ((adlen & 0x1f) != 0) {
		morus_enc_aut_partialblock(ad + (length * 32), ciphertextblock, adlen & 0x1f, morus_state);
	}

	// Encrypt the plaintext
	length = mlen / 32;
	if (length != 0) {
		morus_enc_aut_step_looped(m, c, morus_state, &length);
	}

	// Deal with the partial block
	// In this program, we assume that the message length is a multiple of bytes
	if ((mlen & 0x1f) != 0) {
		morus_enc_aut_partialblock(m + (length * 32), c + (length * 32), mlen & 0x1f, morus_state);
	}

	// Finalization stage, we assume that the tag length is a multiple of bytes
	morus_tag_generation(mlen, adlen, c, morus_state);
	*clen = mlen + 16;

	return 0;
}

int crypto_aead_decrypt(
	unsigned char* m,unsigned long long* mlen,
	unsigned char* nsec,
	const unsigned char* c,unsigned long long clen,
	const unsigned char* ad,unsigned long long adlen,
	const unsigned char* npub,
	const unsigned char* k
	)
{
	uint8_t ciphertextblock[32] __attribute__((aligned(256)));
	uint64_t  morus_state[5][4] __attribute__((aligned(256)));

	if (clen < 16) return -1;

	// Initialization
	morus_initialization(k, npub, morus_state);

	// Process the associated data
	unsigned long long length __attribute((aligned(64))) = adlen / 32;
	if (length != 0) {
		morus_enc_aut_step_ad(ad, morus_state, &length);
	}

	// Deal with the partial block of associated data
	// In this program, we assume that the message length is a multiple of bytes
	if ((adlen & 0x1f) != 0) {
		morus_enc_aut_partialblock(ad + (length * 32), ciphertextblock, adlen & 0x1f, morus_state);
	}

	// Decrypt the ciphertext
	*mlen = clen - 16;
	length = *mlen / 32;
	if (length != 0) {
		morus_dec_aut_step_looped(m, c, morus_state, &length);
	}

	// Deal with the partial block
	// In this program, we assume that the message length is a multiple of bytes
	if ((*mlen & 0x1f) != 0) {
		morus_dec_aut_partialblock(m + (length * 32), c + (length * 32), *mlen & 0x1f, morus_state);
	}

	// We assume that the tag length is a multiple of bytes
	// Verification
	return morus_tag_verification(*mlen, adlen, c, morus_state);
}
