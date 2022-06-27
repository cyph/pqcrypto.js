/* ****************************** *
 * Titanium_CCA_lite              *
 * Implemented by Raymond K. ZHAO *
 *                                *
 * NTT functions                  *
 * ****************************** */
 
#ifndef NTT_H
#define NTT_H

#include <stdint.h>

void ntt_1024_1536(uint64_t *a);
void intt_1536_768(uint64_t *a);
void ntt_512_1280(uint64_t *a);
void ntt_1024_1280(uint64_t *a);
void intt_1280_1280_inv(uint64_t *a);
void ntt_768_768_inv(uint64_t *a);
void ntt_512_768(uint64_t *a);
void intt_768_256(uint64_t *a);
void ntt_1280_1536(uint64_t *a);
void intt_1536_256(uint64_t *a);

#endif
