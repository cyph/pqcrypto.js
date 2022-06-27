/* ****************************** *
 * Titanium_CCA_std               *
 * Implemented by Raymond K. ZHAO *
 *                                *
 * Naive Product/MP               *
 * ****************************** */

#ifndef PRODUCT_H
#define PRODUCT_H

#include <stdint.h>

void product(uint32_t *out, const uint32_t *a, uint32_t alen, const uint32_t *b, uint32_t blen);
void mp(uint32_t *out, uint32_t outlen, const uint32_t *a, uint32_t alen, const uint32_t *b, uint32_t blen);

#endif
