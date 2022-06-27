/* ****************************** *
 * Titanium_CCA_super             *
 * Implemented by Raymond K. ZHAO *
 *                                *
 * Packing/Unpacking              *
 * ****************************** */

#include "pack.h"
#include "param.h"
#include "littleendian.h"
#include <stdint.h>

/* convert a polynomial to a binary string */
void poly_encode(unsigned char *b, const uint64_t *p, uint32_t len)
{
	uint32_t i;
	unsigned char *bb;
	uint32_t pp[8];
	
	/* pack 8 21-bit coordinates to 21 bytes */
	for (i = 0; i < len; i += 8)
	{
		/* make sure each coordinate is smaller than Q */
		pp[0] = p[i] % Q;
		pp[1] = p[i + 1] % Q;
		pp[2] = p[i + 2] % Q;
		pp[3] = p[i + 3] % Q;
		pp[4] = p[i + 4] % Q;
		pp[5] = p[i + 5] % Q;
		pp[6] = p[i + 6] % Q;
		pp[7] = p[i + 7] % Q;
		
		bb = b + (i / 8) * Q_BITS;
		bb[0] = pp[0];
		bb[1] = pp[0] >> 8;
		bb[2] = (pp[0] >> 16) | ((pp[1] & 0x07) << 5);
		bb[3] = pp[1] >> 3;
		bb[4] = pp[1] >> 11;		
		bb[5] = (pp[1] >> 19) | ((pp[2] & 0x3f) << 2);
		bb[6] = pp[2] >> 6;
		bb[7] = (pp[2] >> 14) | ((pp[3] & 0x01) << 7);
		bb[8] = pp[3] >> 1;
		bb[9] = pp[3] >> 9;
		bb[10] = (pp[3] >> 17) | ((pp[4] & 0x0f) << 4);
		bb[11] = pp[4] >> 4;
		bb[12] = pp[4] >> 12;
		bb[13] = (pp[4] >> 20) | ((pp[5] & 0x7f) << 1);
		bb[14] = pp[5] >> 7;
		bb[15] = (pp[5] >> 15) | ((pp[6] & 0x03) << 6);
		bb[16] = pp[6] >> 2;
		bb[17] = pp[6] >> 10;
		bb[18] = (pp[6] >> 18) | ((pp[7] & 0x1f) << 3);
		bb[19] = pp[7] >> 5;
		bb[20] = pp[7] >> 13;
	}
}

/* convert a binary string to a polynomial */
void poly_decode(uint64_t *p, const unsigned char *b, uint32_t len)
{
	uint32_t i;
	const unsigned char *bb;
	
	/* unpack 21 bytes to 8 21-bit coordinates */
	for (i = 0; i < len; i += 8)
	{
		bb = b + (i / 8) * Q_BITS;

		p[i] = ((uint64_t)bb[0]) | (((uint64_t)bb[1]) << 8) | ((((uint64_t)bb[2]) & 0x1f) << 16);
		p[i + 1] = (((uint64_t)bb[2]) >> 5) | (((uint64_t)bb[3]) << 3) | (((uint64_t)bb[4]) << 11) | ((((uint64_t)bb[5]) & 0x03) << 19);
		p[i + 2] = (((uint64_t)bb[5]) >> 2) | (((uint64_t)bb[6]) << 6) | ((((uint64_t)bb[7]) & 0x7f) << 14);
		p[i + 3] = (((uint64_t)bb[7]) >> 7) | (((uint64_t)bb[8]) << 1) | (((uint64_t)bb[9]) << 9) | ((((uint64_t)bb[10]) & 0x0f) << 17);
		p[i + 4] = (((uint64_t)bb[10]) >> 4) | (((uint64_t)bb[11]) << 4) | (((uint64_t)bb[12]) << 12) | ((((uint64_t)bb[13]) & 0x01) << 20);
		p[i + 5] = (((uint64_t)bb[13]) >> 1) | (((uint64_t)bb[14]) << 7) | ((((uint64_t)bb[15]) & 0x3f) << 15);
		p[i + 6] = (((uint64_t)bb[15]) >> 6) | (((uint64_t)bb[16]) << 2) | (((uint64_t)bb[17]) << 10) | ((((uint64_t)bb[18]) & 0x07) << 18);
		p[i + 7] = (((uint64_t)bb[18]) >> 3) | (((uint64_t)bb[19]) << 5) | (((uint64_t)bb[20]) << 13);
	}
}

/* convert a polynomial to a binary string (with compression) */
void poly_encode_c2(unsigned char *b, const uint64_t *p, uint32_t len)
{
	uint32_t i;

	for (i = 0; i < len; i++)
	{
		/* each coordinate will become 2 bytes after compression */
		STORE_C2(b + i * C2_COMPRESSION_BYTE, (p[i] % Q) >> C2_COMPRESSION_BITS);
	}
}

/* convert a binary string to a polynomial (with compression) */
void poly_decode_c2(uint64_t *p, const unsigned char *b, uint32_t len)
{
	uint32_t i;
	
	for (i = 0; i < len; i++)
	{
		/* shift the compressed coordinates back */
		p[i] = ((uint64_t)(LOAD_C2(b + i * C2_COMPRESSION_BYTE))) << C2_COMPRESSION_BITS;
	}
}

