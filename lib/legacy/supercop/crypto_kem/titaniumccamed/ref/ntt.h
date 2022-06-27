/* ****************************** *
 * Titanium_CCA_med               *
 * Implemented by Raymond K. ZHAO *
 *                                *
 * NTT functions                  *
 * ****************************** */
 
#ifndef NTT_H
#define NTT_H

#include <stdint.h>

void intt_2048_2048(uint32_t *a);
void intt_768_768(uint32_t *a);
void ntt_1792_1792(uint32_t *a);
void intt_1792_1792_inv(uint32_t *a);
void ntt_768_768_inv(uint32_t *a);

#endif
