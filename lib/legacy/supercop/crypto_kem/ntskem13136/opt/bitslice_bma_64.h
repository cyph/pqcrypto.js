/**
 *
 *  bitslice_bma_64.h
 *  NTS-KEM
 *
 *  Parameter: NTS-KEM(13, 136)
 *  Platform: Intel 64-bit
 *
 *  This file is part of the optimized implemention of NTS-KEM
 *  submitted as part of NIST Post-Quantum Cryptography
 *  Standardization Process.
 **/

#ifndef __NTSKEM_BITSLICE_BMA_64_H
#define __NTSKEM_BITSLICE_BMA_64_H

#include <stdint.h>

void bitslice_bma(uint64_t (*out)[13],  uint64_t (*s)[13], int *xi);

#endif /* __NTSKEM_BITSLICE_BMA_64_H */
