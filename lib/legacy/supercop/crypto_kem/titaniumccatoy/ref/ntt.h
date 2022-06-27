/* ****************************** *
 * Titanium_CCA_toy               *
 * Implemented by Raymond K. ZHAO *
 *                                *
 * NTT functions                  *
 * ****************************** */
 
#ifndef NTT_H
#define NTT_H

#include <stdint.h>

void intt_1280_1280(uint32_t *a);
void intt_512_512(uint32_t *a);
void ntt_1024_1024(uint32_t *a);
void intt_1024_1024_inv(uint32_t *a);
void ntt_512_512_inv(uint32_t *a);

#endif
