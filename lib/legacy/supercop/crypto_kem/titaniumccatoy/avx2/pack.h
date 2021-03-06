/* ****************************** *
 * Titanium_CCA_toy               *
 * Implemented by Raymond K. ZHAO *
 *                                *
 * Packing/Unpacking              *
 * ****************************** */

#ifndef PACK_H
#define PACK_H

#include <stdint.h>

void poly_encode(unsigned char *b, const uint64_t *p, uint32_t len);
void poly_decode(uint64_t *p, const unsigned char *b, uint32_t len);
void poly_encode_c2(unsigned char *b, const uint64_t *p, uint32_t len);
void poly_decode_c2(uint64_t *p, const unsigned char *b, uint32_t len);

#endif
