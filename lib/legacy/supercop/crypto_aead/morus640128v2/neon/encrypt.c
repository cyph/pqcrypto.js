#include <string.h>
#include "crypto_aead.h"

extern void morus_stateupdate(unsigned int msgblk[], unsigned int state[][4]);
extern void morus_enc_aut_step_looped(const unsigned char* plaintextblock, unsigned char* ciphertextblock, unsigned int state[][4], unsigned long long* loop_times);
extern void morus_dec_aut_step_looped(unsigned char* plaintextblock, const unsigned char* ciphertextblock, unsigned int state[][4], unsigned long long* loop_times);
extern void morus_enc_aut_step_ad(const unsigned char* plaintextblock, unsigned int state[][4], unsigned long long* loop_times);

void morus_initialization(const unsigned char *key, const unsigned char* iv, unsigned int state[][4])
{
	int i;
	unsigned int temp[4] __attribute__((aligned(128))) = { 0, 0, 0, 0 };
	unsigned char con0[16] = { 0x0, 0x1, 0x01, 0x02, 0x03, 0x05, 0x08, 0x0d, 0x15, 0x22, 0x37, 0x59, 0x90, 0xe9, 0x79, 0x62 };
	unsigned char con1[16] = { 0xdb, 0x3d, 0x18, 0x55, 0x6d, 0xc2, 0x2f, 0xf1, 0x20, 0x11, 0x31, 0x42, 0x73, 0xb5, 0x28, 0xdd };

	memcpy(state[0], iv, 16);
	memcpy(state[1], key, 16);
	memset(state[2], 0xff, 16);
	memcpy(state[3], con0, 16);
	memcpy(state[4], con1, 16);

	for (i = 0; i < 16; i++) morus_stateupdate(temp, state);
	for (i = 0; i < 4; i++) state[1][i] ^= ((unsigned int*)key)[i];
}

void morus_tag_generation(unsigned long long msglen, unsigned long long adlen, unsigned char* c, unsigned int state[][4])
{
	int i, j;
	unsigned char t[16] __attribute__((aligned(128)));

	((unsigned long long*)t)[0] = (adlen << 3);
	((unsigned long long*)t)[1] = (msglen << 3);

	state[4][0] ^= state[0][0]; state[4][1] ^= state[0][1]; state[4][2] ^= state[0][2]; state[4][3] ^= state[0][3];

	for (i = 0; i < 10; i++) morus_stateupdate((unsigned int*)t, state);

	for (j = 0; j < 4; j++) {
		state[0][j] ^= state[1][(j + 1) & 3] ^ (state[2][j] & state[3][j]);
	}

	memcpy(c + msglen, state[0], 16);
}

int morus_tag_verification(unsigned long long msglen, unsigned long long adlen, const unsigned char* c, unsigned int state[][4])
{
	int i, j;
	unsigned char t[16] __attribute__((aligned(128)));
	int check = 0;

	((unsigned long long*)t)[0] = adlen << 3;
	((unsigned long long*)t)[1] = msglen << 3;

	state[4][0] ^= state[0][0]; state[4][1] ^= state[0][1]; state[4][2] ^= state[0][2]; state[4][3] ^= state[0][3];

	for (i = 0; i < 10; i++) morus_stateupdate((unsigned int*)t, state);

	for (j = 0; j < 4; j++) {
		state[0][j] ^= state[1][(j + 1) & 3] ^ (state[2][j] & state[3][j]);
	}

	for (i = 0; i < 16; i++)  check |= (c[msglen + i] ^ ((unsigned char *)state[0])[i]);
	if (check == 0) return 0;
	else return -1;
}

void morus_enc_aut_partialblock(const unsigned char* plaintext, unsigned char* ciphertext, unsigned long long len, unsigned int state[][4])
{
	unsigned char plaintextblock[16] __attribute__((aligned(128)));
	unsigned char ciphertextblock[16] __attribute__((aligned(128)));

	memset(plaintextblock, 0, 16);
	memcpy(plaintextblock, plaintext, len);

	((unsigned int*)ciphertextblock)[0] = ((unsigned int*)plaintextblock)[0] ^ state[0][0] ^ state[1][1] ^ (state[2][0] & state[3][0]);
	((unsigned int*)ciphertextblock)[1] = ((unsigned int*)plaintextblock)[1] ^ state[0][1] ^ state[1][2] ^ (state[2][1] & state[3][1]);
	((unsigned int*)ciphertextblock)[2] = ((unsigned int*)plaintextblock)[2] ^ state[0][2] ^ state[1][3] ^ (state[2][2] & state[3][2]);
	((unsigned int*)ciphertextblock)[3] = ((unsigned int*)plaintextblock)[3] ^ state[0][3] ^ state[1][0] ^ (state[2][3] & state[3][3]);

	morus_stateupdate(((unsigned int*)plaintextblock), state);

	memcpy(ciphertext, ciphertextblock, len);
}

void morus_dec_aut_partialblock(unsigned char* plaintext, const unsigned char* ciphertext, unsigned long long len, unsigned int state[][4])
{
	unsigned char plaintextblock[16] __attribute__((aligned(128)));
	unsigned char ciphertextblock[16] __attribute__((aligned(128)));

	memset(ciphertextblock, 0, 16);
	memcpy(ciphertextblock, ciphertext, len);

	((unsigned int*)plaintextblock)[0] = ((unsigned int*)ciphertextblock)[0] ^ state[0][0] ^ state[1][1] ^ (state[2][0] & state[3][0]);
	((unsigned int*)plaintextblock)[1] = ((unsigned int*)ciphertextblock)[1] ^ state[0][1] ^ state[1][2] ^ (state[2][1] & state[3][1]);
	((unsigned int*)plaintextblock)[2] = ((unsigned int*)ciphertextblock)[2] ^ state[0][2] ^ state[1][3] ^ (state[2][2] & state[3][2]);
	((unsigned int*)plaintextblock)[3] = ((unsigned int*)ciphertextblock)[3] ^ state[0][3] ^ state[1][0] ^ (state[2][3] & state[3][3]);

	memcpy(plaintext, plaintextblock, len);
	memset(plaintextblock, 0, 16);
	memcpy(plaintextblock, plaintext, len);

	morus_stateupdate(((unsigned int*)plaintextblock), state);
}

int crypto_aead_encrypt(
	unsigned char* c, unsigned long long* clen,
	const unsigned char* m, unsigned long long mlen,
	const unsigned char* ad, unsigned long long adlen,
	const unsigned char* nsec,
	const unsigned char* npub,
	const unsigned char* k
	)
{
	unsigned char ciphertextblock[16] __attribute__((aligned(128)));
	unsigned int morus_state[5][4] __attribute__((aligned(128)));

	// Initialization
	morus_initialization(k, npub, morus_state);

	// Process the associated data
	unsigned long long length __attribute__((aligned(64))) = adlen / 16;
	if (length != 0) {
		morus_enc_aut_step_ad(ad, morus_state, &length);
	}

	// Deal with the partial block of associated data
	// In this program, we assume that the message length is a multiple of bytes
	if ((adlen & 0xf) != 0) {
		morus_enc_aut_partialblock(ad + (length * 16), ciphertextblock, adlen & 0xf, morus_state);
	}

	// Encrypt the plaintext
	length = mlen / 16;
	if (length != 0) {
		morus_enc_aut_step_looped(m, c, morus_state, &length);
	}

	// Deal with the partial block
	// In this program, we assume that the message length is a multiple of bytes
	if ((mlen & 0xf) != 0) {
		morus_enc_aut_partialblock(m + (length * 16), c + (length * 16), mlen & 0xf, morus_state);
	}

	// Finalization stage, we assume that the tag length is a multiple of bytes
	morus_tag_generation(mlen, adlen, c, morus_state);
	*clen = mlen + 16;

	return 0;
}

int crypto_aead_decrypt(
	unsigned char* m, unsigned long long* mlen,
	unsigned char* nsec,
	const unsigned char* c, unsigned long long clen,
	const unsigned char* ad, unsigned long long adlen,
	const unsigned char* npub,
	const unsigned char* k
	)
{
	unsigned char ciphertextblock[16] __attribute__((aligned(128)));
	unsigned int morus_state[5][4] __attribute__((aligned(128)));

	if (clen < 16) return -1;

	// Initialization
	morus_initialization(k, npub, morus_state);

	// Process the associated data
	unsigned long long length __attribute__((aligned(64))) = adlen / 16;
	if (length != 0) {
		morus_enc_aut_step_ad(ad, morus_state, &length);
	}

	// Deal with the partial block of associated data
	// In this program, we assume that the message length is a multiple of bytes
	if ((adlen & 0xf) != 0) {
		morus_enc_aut_partialblock(ad + (length * 16), ciphertextblock, adlen & 0xf, morus_state);
	}

	// Decrypt the ciphertext
	*mlen = clen - 16;
	length = *mlen / 16;
	if (length != 0) {
		morus_dec_aut_step_looped(m, c, morus_state, &length);
	}

	// Deal with the partial block
	// In this program, we assume that the message length is a multiple of bytes
	if (((*mlen) & 0xf) != 0) {
		morus_dec_aut_partialblock(m + (length * 16), c + (length * 16), (*mlen) & 0xf, morus_state);
	}

	// We assume that the tag length is a multiple of bytes
	// Verification
	return morus_tag_verification(*mlen, adlen, c, morus_state);
}
