#ifndef HANMRE_H
#define HANMRE_H

// Rate in bytes
#define HANMRE_rH 64
#define HANMRE_rB 1

// Number of rounds
#define HANMRE_sH 12
#define HANMRE_sB 1
#define HANMRE_sE 6
#define HANMRE_sK 12

// State size in bytes
#define HANMRE_STATE_SZ 40

// Size of rate in bytes
#define HANMRE_rH_SZ ((HANMRE_rH+7)/8)

// Size of zero truncated IV in bytes
#define HANMRE_IV_SZ 8

// Size of tag in bytes
#define HANMRE_TAG_SZ 16

// Security level
#define HANMRE_K 128

void hanmre_mac(
	const unsigned char *ad, const unsigned long long adlen,
	const unsigned char *c, const unsigned long long clen,
	unsigned char *tag
);

void hanmre_rk(
	const unsigned char *k,
	const unsigned char *iv,
	const unsigned char *in,
	const unsigned long long inlen,
	unsigned char *out,
	const unsigned long long outlen
);

void hanmre_enc(
	const unsigned char *k,
	const unsigned char *tag,
	const unsigned char *m, const unsigned long long mlen,
	unsigned char *c
);

#endif
