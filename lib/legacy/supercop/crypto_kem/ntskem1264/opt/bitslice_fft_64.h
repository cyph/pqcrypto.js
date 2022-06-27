/**
 *
 *  bitslice_fft_64.h
 *  NTS-KEM
 *
 *  Parameter: NTS-KEM(12, 64)
 *  Platform: Intel 64-bit
 *
 *  The implementation here is based on the work of McBit's, see
 *  https://tungchou.github.io/mcbits/
 *
 *  This file is part of the optimized implemention of NTS-KEM
 *  submitted as part of NIST Post-Quantum Cryptography
 *  Standardization Process.
 **/

#ifndef __NTSKEM_BITSLICE_FFT_64_H
#define __NTSKEM_BITSLICE_FFT_64_H

#include <stdint.h>

void bitslice_fft12_64(uint64_t out[][12], uint64_t (*in)[12], uint64_t mask);

#endif /* __NTSKEM_BITSLICE_FFT_64_H */
