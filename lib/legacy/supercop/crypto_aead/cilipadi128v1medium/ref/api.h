/*
 * api.h
 *
 *  Created on: 22 Feb 2019
 *      Author: mrz
 */



/*
This indicates that for this variant of the MyAEAD algorithm,
the key is 32 bytes, the nonce is 12 bytes,
and that the ciphertext is at most 16 bytes longer than the plaintext.
(A typical AEAD algorithm has a constant gap between plaintext length and
ciphertext length, but the requirement here is to have a constant limit on the gap.)

The definition CRYPTO_NSECBYTES shall always be set to zero.
The last definition CRYPTO_NOOVERLAP is an optional definition in SUPERCOP API
and indicates whether the implementation can handle overlapping input and output buffers.
To ensure compatibility with the SUPERCOP API, api.h file shall
contain "#define CRYPTO_NOOVERLAP 1".
Regardless of whether this flag is needed in the SUPERCOP framework,
it clarifies how the API is intended to be used;
the implementation is not expected to handle overlapping input and output buffers.
(Note that if CRYPTO_NOOVERLAP is not defined, the SUPERCOP framework assumes that
inputs and outputs can overlap, and returns an error if this behavior is not supported.)
*/

#define CRYPTO_KEYBYTES 16 // key size in bytes
#define CRYPTO_NSECBYTES 0 // shall always be set to zero
#define CRYPTO_NPUBBYTES 16 // nonce size in bytes
#define CRYPTO_ABYTES 12 // the ciphertext is at most CRYPTO_ABYTES bytes longer than the plaintext
#define CRYPTO_NOOVERLAP 1

