/*
 * Gage Reference C Implementation
 *
 * Copyright 2018:
 *     Danilo Gligoroski <danilog@ntnu.no>
 *     Daniel Otte <bg@nerilex.org>
 *     Hristina Mihajloska <hristina.mihajloska@finki.ukim.mk>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 */

#include <stdio.h>
#include "crypto_hash.h"
#include "constants.h"
#include "api.h"


void print_state(const struct state *s) {
	int i;
	printf(" [ ");
	for (i = 0; i < INTERNAL_STATE_SIZE_BYTES - 1; ++i) {
		printf("%02x, ", s->a[i]);
	}
	printf("%02x ]\n", s->a[i]);
}

void shuffle_state_bits(struct state *s) {
	struct state t;

	const uint8_t mask[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
	uint8_t i, tmp, bit;

	for (i = 0; i < INTERNAL_STATE_SIZE_BYTES; ++i) {
		t.a[i] = 0;

		bit = 0;
		tmp = ( s->a[(i + bit) % INTERNAL_STATE_SIZE_BYTES] & mask[bit] ) << 4;
		t.a[i] |= tmp;

		bit = 1;
		tmp = ( s->a[(i + bit) % INTERNAL_STATE_SIZE_BYTES] & mask[bit] ) << 2;
		t.a[i] |= tmp;

		bit = 2;
		tmp = ( s->a[(i + bit) % INTERNAL_STATE_SIZE_BYTES] & mask[bit] );
		t.a[i] |= tmp;

		bit = 3;
		tmp = ( s->a[(i + bit) % INTERNAL_STATE_SIZE_BYTES] & mask[bit] ) << 2;
		t.a[i] |= tmp;

		bit = 4;
		tmp = ( s->a[(i + bit) % INTERNAL_STATE_SIZE_BYTES] & mask[bit] ) >> 4;
		t.a[i] |= tmp;

		bit = 5;
		tmp = ( s->a[(i + bit) % INTERNAL_STATE_SIZE_BYTES] & mask[bit] ) << 1;
		t.a[i] |= tmp;

		bit = 6;
		tmp = ( s->a[(i + bit) % INTERNAL_STATE_SIZE_BYTES] & mask[bit] )  >> 5;
		t.a[i] |= tmp;

		bit = 7;
		tmp = ( s->a[(i + bit) % INTERNAL_STATE_SIZE_BYTES] & mask[bit] );
		t.a[i] |= tmp;
	}

	for (i = 0; i < INTERNAL_STATE_SIZE_BYTES; ++i) {
		s->a[i] = t.a[i];
	}
}


void dtransform(const uint8_t l, struct state *s) {
	uint8_t ldr, nextldr;
	uint8_t tmp;
	int i;

	ldr = l;
	for (i = 0; i < INTERNAL_STATE_SIZE_BYTES; ++i) {
		/* compute the new upper 2 bits */
		nextldr = s->a[i] >> 6;
		tmp = 	(Quas[ldr][ nextldr ]) << 6;

		/* compute the next 2 bits */
		ldr = nextldr;
		nextldr = (s->a[i] & 0x30) >> 4;
		tmp |= 	(Quas[ ldr ][ nextldr ]) << 4;

		/* compute the next 2 bits */
		ldr = nextldr;
		nextldr = (s->a[i] & 0x0c) >> 2;
		tmp |=	(Quas[ ldr ][ nextldr ]) << 2;

		/* compute the new lower 2 bits */
		ldr = nextldr;
		nextldr = s->a[i] & 0x03;
		tmp |=	Quas[ ldr ][ nextldr ];

		/* prepare the leader for the next byte in the state to be transformed */
		ldr = nextldr;

		/* in-place replacemet of the old byte with just computed tmp */
		s->a[i] = tmp;
	}
}

void qpermutation(struct state *s, uint8_t rounds) {
	int i;

	dtransform(leader[0], s);
	for (i = 1; i < rounds; ++i) {
		shuffle_state_bits(s);
		dtransform(leader[i], s);
	}
}


void Initialize(struct state *s) {
	int i;

	/* We initialize the state with zeros */
	/* should be with memset() for slightly better performance */
	for (i = 0; i < INTERNAL_STATE_SIZE_BYTES; ++i) {
		s->a[i] = 0;
	}
}

void Padd(unsigned char *mpad, const unsigned char *in, unsigned long long inlen) {
	unsigned long long i, lastbytes;
	int j;

	/* determine how many bytes from the original message go in the mpad block */
	lastbytes = inlen % RATE_BYTES;
	/* and put them in mpad */
	j = 0;
	for (i = inlen - lastbytes; i < inlen; ++i) {
		mpad[j++] = in[i];
	}

	/* append the byte 0x80 */
	mpad[j++] = 0x80;

	/* if there are still some remaining bytes in mpad, set them to 0 */
	while (j<RATE_BYTES) mpad[j++] = 0;
}

void Absorb(struct state *s, const unsigned char *in, unsigned long long inlen, unsigned char *mpad) {
	unsigned long long i, lastblockbyteindex;
	int j;

	lastblockbyteindex = inlen - (inlen % RATE_BYTES);
	i = 0;
	while (i < lastblockbyteindex) {
		for (j = 0; j < RATE_BYTES; ++j)
			s->a[j] ^= in[i++];
		qpermutation(s, ROUNDS);
	}
	/* absorb the final mpad part */
	for (j = 0; j < RATE_BYTES; ++j)
		s->a[j] ^= mpad[j];
	qpermutation(s, ROUNDS);
}

void Squeeze(struct state *s, unsigned char *out) {
	int i, j;

	i = 0;
	while (i < CRYPTO_BYTES) {
		for (j = 0; j < RATE_BYTES; ++j)
			out[i++] = s->a[j];
		qpermutation(s, ROUNDS);
	}
}


int crypto_hash(
unsigned char *out,
const unsigned char *in,
unsigned long long inlen
)
{
	struct state st;
	unsigned char mpad[RATE_BYTES];

	Initialize(&st);
	Padd(mpad, in, inlen);
	Absorb(&st, in, inlen, mpad);
	Squeeze(&st, out);

	return 0;
}
