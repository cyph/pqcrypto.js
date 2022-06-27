/**
 *  utils.h
 *  NTS-KEM
 *
 *  Parameter: NTS-KEM(13, 80)
 *  Platform: SSE2/SSE4.1
 *
 *  This file is part of the additional implemention of NTS-KEM
 *  submitted as part of NIST Post-Quantum Cryptography
 *  Standardization Process.
 **/

#ifndef _NTSKEM_UTILS_H
#define _NTSKEM_UTILS_H

#include <stdint.h>

uint64_t cpucycles(void);

uint64_t cpucycles_persecond(void);

#endif /* _NTSKEM_UTILS_H */
