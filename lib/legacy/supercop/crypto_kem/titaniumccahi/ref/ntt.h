/* ****************************** *
 * Titanium_CCA_hi                *
 * Implemented by Raymond K. ZHAO *
 *                                *
 * NTT functions                  *
 * ****************************** */
 
#ifndef NTT_H
#define NTT_H

#include <stdint.h>

void intt_2560_2560(uint32_t *a);
void intt_1024_1024(uint32_t *a);
void ntt_2304_2304(uint32_t *a);
void intt_2304_2304_inv(uint32_t *a);
void ntt_1024_1024_inv(uint32_t *a);

#endif
