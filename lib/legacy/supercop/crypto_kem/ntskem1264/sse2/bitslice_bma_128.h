/**
 *
 *  bitslice_bma_128.h
 *  NTS-KEM
 *
 *  Parameter: NTS-KEM(12, 64)
 *  Platform: SSE2/SSE4.1
 *
 *  This file is part of the additional implemention of NTS-KEM
 *  submitted as part of NIST Post-Quantum Cryptography
 *  Standardization Process.
 **/

#ifndef __NTSKEM_BITSLICE_BMA_128_H
#define __NTSKEM_BITSLICE_BMA_128_H

#include <stdint.h>
#include <immintrin.h>

void bitslice_bma(uint64_t (*out)[12], __m128i (*s)[12], int *xi);

#endif /* __NTSKEM_BITSLICE_BMA_128_H */
