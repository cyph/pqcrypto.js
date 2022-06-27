/**
 *  keccak.h
 *
 *  The code below is taken from https://github.com/gvanas/KeccakCodePackage
 *
 *  This file is part of the reference implemention of NTS-KEM
 *  submitted as part of NIST Post-Quantum Cryptography
 *  Standardization Process.
 **/

/*
 Implementation by the Keccak, Keyak and Ketje Teams, namely, Guido Bertoni,
 Joan Daemen, Michaël Peeters, Gilles Van Assche and Ronny Van Keer, hereby
 denoted as "the implementer".
 For more information, feedback or questions, please refer to our websites:
 http://keccak.noekeon.org/
 http://keyak.noekeon.org/
 http://ketje.noekeon.org/
 To the extent possible under law, the implementer has waived all copyright
 and related or neighboring rights to the source code in this file.
 http://creativecommons.org/publicdomain/zero/1.0/
 */

/*
 ================================================================
 The purpose of this source file is to demonstrate a readable and compact
 implementation of all the Keccak instances approved in the FIPS 202 standard,
 including the hash functions and the extendable-output functions (XOFs).
 We focused on clarity and on source-code compactness,
 rather than on the performance.
 The advantages of this implementation are:
 + The source code is compact, after removing the comments, that is. :-)
 + There are no tables with arbitrary constants.
 + For clarity, the comments link the operations to the specifications using
 the same notation as much as possible.
 + There is no restriction in cryptographic features. In particular,
 the SHAKE128 and SHAKE256 XOFs can produce any output length.
 + The code does not use much RAM, as all operations are done in place.
 The drawbacks of this implementation are:
 - There is no message queue. The whole message must be ready in a buffer.
 - It is not optimized for peformance.
 The implementation is even simpler on a little endian platform. Just define the
 LITTLE_ENDIAN symbol in that case.
 For a more complete set of implementations, please refer to
 the Keccak Code Package at https://github.com/gvanas/KeccakCodePackage
 For more information, please refer to:
 * [Keccak Reference] http://keccak.noekeon.org/Keccak-reference-3.0.pdf
 * [Keccak Specifications Summary] http://keccak.noekeon.org/specs_summary.html
 This file uses UTF-8 encoding, as some comments use Greek letters.
 ================================================================
 */

#ifndef __KECCAK_H
#define __KECCAK_H

void sha3_256(const unsigned char *input, size_t inputByteLen, unsigned char *output);
void sha3_384(const unsigned char *input, size_t inputByteLen, unsigned char *output);
void sha3_512(const unsigned char *input, size_t inputByteLen, unsigned char *output);

void shake_256(const unsigned char *input, unsigned int inputByteLen, unsigned char *output, int outputByteLen);

#endif /* __KECCAK_H */
