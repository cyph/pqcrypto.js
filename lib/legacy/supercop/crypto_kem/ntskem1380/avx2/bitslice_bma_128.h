/**
 *
 *  bitslice_bma_128.h
 *  NTS-KEM
 *
 *  Parameter: NTS-KEM(13, 80)
 *  Platform: AVX2
 *
 *  This file is part of the additional implemention of NTS-KEM
 *  submitted as part of NIST Post-Quantum Cryptography
 *  Standardization Process.
 **/

#ifndef __NTSKEM_BITSLICE_BMA_128_H
#define __NTSKEM_BITSLICE_BMA_128_H

#include <stdint.h>
#include <immintrin.h>

void bitslice_bma(__m128i (*out)[13],  __m128i (*s)[13], int *xi);

#endif /* __NTSKEM_BITSLICE_BMA_128_H */
