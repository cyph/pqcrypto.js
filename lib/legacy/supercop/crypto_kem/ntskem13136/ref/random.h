/**
 *  random.h
 *  NTS-KEM
 *
 *  Parameter: NTS-KEM(13, 136)
 *
 *  This file is part of the reference implemention of NTS-KEM
 *  submitted as part of NIST Post-Quantum Cryptography
 *  Standardization Process.
 **/

#ifndef __NTSKEM_RANDOM_H
#define __NTSKEM_RANDOM_H

#include <stdint.h>
#include "randombytes.h"

/**
 *  Generate a 16-bit random number between 0 and `bound-1`
 *  
 *  @param[in]  bound  The limit of the number to be generated
 *  @return a 16-bit random number
 **/
uint16_t random_uint16_bounded(uint16_t bound);

/**
 *  Return a uniform random bit
 *
 *  @return random bit 0 or 1
 **/
uint8_t randombit();

#endif /* __NTSKEM_RANDOM_H */
