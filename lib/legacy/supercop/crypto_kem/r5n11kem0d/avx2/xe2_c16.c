/*
 * Copyright (c) 2019, PQShield
 * Markku-Juhani O. Saarinen
 */

//	Optimized XE2-53 for "2iot" parameter set. Runs well even on 16-bit MCUs.

#include "xef.h"
#include "little_endian.h"

#define XM16_RTLM(r, n) \
	{ r = ((r << (16 % n)) | (r >> (n - (16 % n)))) & ((1 << n) - 1); }
#define XM16_FLDM(r, n) \
	{ r = (r ^ ((r >> n))) & ((1 << n) - 1); }
#define XM16_UNFM(r, n) \
	{ r &= (uint16_t) ((1 << n) - 1); r |= (uint16_t) (r << n); }
#define XM16_ROTR(r, n) \
	{ r = (uint16_t) ((r >> (16 % n)) | (r << (n - (16 % n)))); }

//	== XE5-53 (128-bit payload) ==

void xe2_53_compute(void *block)
{
	int i;
	uint16_t r11, r13, r14, r15;
	uint16_t *p16, x;

	// initialize
	p16 = (uint16_t *) block;
	r11 = r13 = r14 = r15 = LITTLE_ENDIAN16(p16[7]);

	for (i = 7; i >= 0; i--) {
		if (i < 7) {
			XM16_RTLM(r11, 11); XM16_RTLM(r13, 13);
			XM16_RTLM(r14, 14); XM16_RTLM(r15, 15);

			x = LITTLE_ENDIAN16(p16[i]);
			r11 ^= x;	r13 ^= x;
			r14 ^= x;	r15 ^= x;
		}
		XM16_FLDM(r11, 11); XM16_FLDM(r13, 13);
		XM16_FLDM(r14, 14); XM16_FLDM(r15, 15);
	}

	// XE2-53:		r11 r13 r14 r15 end
	// bit offset:	0	11	24	38	53
	p16[8]	^= (uint16_t) LITTLE_ENDIAN16(r11 ^ (r13 << 11));
	p16[9]	^= (uint16_t) LITTLE_ENDIAN16((r13 >> 5) ^ (r14 << 8));
	p16[10] ^= (uint16_t) LITTLE_ENDIAN16((r14 >> 8) ^ (r15 << 6));
	*((uint8_t *) &p16[11]) ^= (uint8_t) ((r15 >> 10));
}

void xe2_53_fixerr(void *block)
{
	int i;
	uint16_t r11, r13, r14, r15;
	uint16_t *p16, x;

	// decode
	p16 = (uint16_t *) block;
	x = (uint16_t) LITTLE_ENDIAN16(p16[8]);
	r11 = x; r13 = x >> 11;
	x = (uint16_t) LITTLE_ENDIAN16(p16[9]);
	r13 ^= (uint16_t) (x << 5); r14 = x >> 8;
	x = (uint16_t) LITTLE_ENDIAN16(p16[10]);
	r14 ^= (uint16_t) (x << 8); r15 = x >> 6;
	x = (uint16_t) *((uint8_t *) &p16[11]);
	r15 ^= (uint16_t) (x << 10);

	XM16_UNFM(r11, 11); XM16_UNFM(r13, 13);
	XM16_UNFM(r14, 14); XM16_UNFM(r15, 15);

	for (i = 0; i < 8; i++) {
		if (i > 0) {
			// rotate
			XM16_ROTR(r11, 11); XM16_ROTR(r13, 13);
			XM16_ROTR(r14, 14); XM16_ROTR(r15, 15);
		}
		x = ((r11 | r13) & r14 & r15) | ((r11 & r13) & (r14 | r15));
		p16[i] ^= LITTLE_ENDIAN16(x);
	}
}
