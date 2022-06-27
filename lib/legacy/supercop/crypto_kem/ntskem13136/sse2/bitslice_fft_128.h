/**
 *
 *  bitslice_fft_128.h
 *  NTS-KEM
 *
 *  Parameter: NTS-KEM(13, 136)
 *  Platform: SSE2/SSE4.1
 *
 *  The implementation here is based on the work of McBit's, see
 *  https://tungchou.github.io/mcbits/
 *
 *  This file is part of the additional implemention of NTS-KEM
 *  submitted as part of NIST Post-Quantum Cryptography
 *  Standardization Process.
 **/

#ifndef __NTSKEM_BITSLICE_FFT_128_H
#define __NTSKEM_BITSLICE_FFT_128_H

#include <stdint.h>
#include <immintrin.h>

void bitslice_fft13_128(__m128i out[][13], __m128i (*in)[13]);

#endif /* __NTSKEM_BITSLICE_FFT_128_H */
