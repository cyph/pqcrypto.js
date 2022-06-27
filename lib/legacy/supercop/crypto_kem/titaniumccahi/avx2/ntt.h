/* ****************************** *
 * Titanium_CCA_hi                *
 * Implemented by Raymond K. ZHAO *
 *                                *
 * NTT functions                  *
 * ****************************** */
 
#ifndef NTT_H
#define NTT_H

#include <stdint.h>

void ntt_1536_2560(uint64_t *a);
void intt_2560_1024(uint64_t *a);
void ntt_768_2304(uint64_t *a);
void ntt_1536_2304(uint64_t *a);
void intt_2304_2304_inv(uint64_t *a);
void ntt_1024_1024_inv(uint64_t *a);
void ntt_768_1024(uint64_t *a);
void intt_1024_256(uint64_t *a);
void ntt_2304_2560(uint64_t *a);
void intt_2560_256(uint64_t *a);

#endif
