//
// Supersingular Isogeny Key Encapsulation Ref. Library
//
// InfoSec Global Inc., 2017-2020
// Basil Hess <basil.hess@infosecglobal.com>
//

/** @file
Encodings for public keys, private keys, field-p, field-p2 and shared secrets.
*/

#include <sike_params.h>
#include <sidh.h>

#ifndef ISOGENY_REF_ENCODING_H
#define ISOGENY_REF_ENCODING_H

#define BITS_TO_BYTES_CEIL(x) (((x) + 7) / 8)

///////////////////////////////////////////////
// Encoding functions for field-p elements
///////////////////////////////////////////////

/**
 * Octet-string-to-integer conversion (encoding)
 *
 * @param to_dec Octet-string to be decoded
 * @param to_decLen Length of the octet string
 * @param dec Decoded integer
 */
void ostoi(const unsigned char *to_dec, size_t to_decLen, mp dec);

/**
 * Octet-string-to-field-p-element conversion
 *
 * @param to_dec
 * @param to_decLen
 * @param dec
 * @param p
 * @return
 */
int ostofp(const unsigned char *to_dec, size_t to_decLen, const mp p, mp dec);



///////////////////////////////////////////////
// Encoding functions for field-p2 elements (shared secrets)
///////////////////////////////////////////////

/**
 * Octet-string-to-field-p^2-element conversion
 * @param to_dec
 * @param to_decLen
 * @param dec
 * @param p
 * @return
 */
int ostofp2(const unsigned char *to_dec, size_t to_decLen, const mp p, fp2* dec);

/**
 * Field-p2-to-octet-string conversion (encoding).
 * Uses for encoding shared secrets.
 *
 * @param params SIKE parameters
 * @param shared_sec Field-p2 element (shared secret) to be encoded.
 * @param enc Octet-encoded field-p2 element (shared secret).
 * @return
 */
void fp2toos(const sike_params_t* params,
            const fp2* shared_sec,
            unsigned char* enc);

/**
 * Encodes the shared secret and allocates memory.
 *
 * @param params SIDH parameters
 * @param shared_sec Fp2 element to be encoded
 * @param enc Encoded shared secret
 * @param encLen Length of the encoded shared secret
 * @return
 */
void fp2toos_alloc(const sike_params_t *params,
                  const fp2 *shared_sec,
                  unsigned char **enc,
                  size_t *encLen);

/**
 * Returns the length of octet-string-encoded field-p2 element (shared secret)
 * given the SIKE parameters
 *
 * @param params SIKE parameters
 * @param party ALICE or BOB
 * @return Length of the octet-string-encoded field-p2 element (shared secret)
 */
size_t fp2toos_len(const sike_params_t* params,
                   party_t party);



///////////////////////////////////////////////
// Encoding functions for private keys
///////////////////////////////////////////////

/**
 * Decodes a private key from its octet-string-encoding.
 *
 * @param params SIDH parameters
 * @param party ALICE or BOB
 * @param sk Octet-string-encoded private key
 * @param s KEM parameter 's'. May be NULL and it won't be decoded.
 * @param skDec Decoded private key part of the octet-encoding
 * @param pkDec Decoded public key part of the octet-encoding. May be NULL and it won't be decoded.
 * @return
 */
int ostosk(const sike_params_t *params,
           party_t party,
           const unsigned char *sk,
           unsigned char* s,
           sike_private_key skDec,
           sike_public_key_t* pkDec);

/**
 * Private-key-to-octet-string conversion (encoding)
 * s | sk | pk
 *
 * @param params SIKE parameters
 * @param party ALICE or BOB
 * @param s KEM parameter 's'
 * @param sk Private key (multi-precision integer)
 * @param pk Public key
 * @param enc Octet-string-encoded private key
 * @return
 */
void sktoos(const sike_params_t *params,
            const party_t party,
            const unsigned char* s,
            const sike_private_key sk,
            const sike_public_key_t* pk,
            unsigned char *enc);

/**
 * Returns the length of octet-string-encoded private keys.
 * given the SIKE parameters
 *
 * @param params SIKE parameters
 * @param party ALICE or BOB
 * @return Length of the octet-string-encoded private keys
 */
size_t sktoos_len(const sike_params_t* params,
                  party_t party);

///////////////////////////////////////////////
// Encoding/Decoding functions for public keys
///////////////////////////////////////////////

/**
 * Decodes a public key from its octet-string-encoding.
 *
 * @param params SIDH parameters
 * @param pk Octet-string-encoded public key
 * @param dec Decoded public key
 * @return
 */
int ostopk(const sike_params_t *params,
           party_t party,
           const unsigned char *pk,
           sike_public_key_t *dec);

/**
 * Public-key-to-octet-string conversion (encoding)
 *
 * @param params SIKE parameters
 * @param pk Montgomery curve parameters to be encoded.
 * @param pkOut Internal representation of the encoded public key (Px, Qx, Rx)
 * @param enc Octet-string encoded public key
 * @param party ALICE or BOB
 * @return
 */
void pktoos(const sike_params_t* params,
            const sike_public_key_t* pk,
            unsigned char* enc,
            party_t party);

/**
 * Public-key-to-octet-string conversion (encoding)
 * Allocates memory for the octet-string
 *
 * @param params SIKE parameters
 * @param pk Montgomery curve parameters to be encoded.
 * @param extraAllocLen Allocate these extra bytes
 * @param enc Octet-string encoded public key. Will be allocated.
 * @param encLen Length of the encoded public key (without extraAllocLen)
 * @param party ALICE or BOB
 * @return
 */
void pktoos_alloc(const sike_params_t *params,
                 const sike_public_key_t *pk,
                 unsigned char **enc,
                 size_t *encLen,
                 party_t party);

/**
 * Returns the length of octet-string-encoded public keys.
 * given the SIKE parameters
 *
 * @param params SIKE parameters
 * @param party ALICE or BOB
 * @return Length of the octet-string-encoded public keys
 */
size_t pktoos_len(const sike_params_t* params,
                  party_t party);



///////////////////////////////////////////////
// Encoding functions for encapsulations (ct)
///////////////////////////////////////////////

/**
 * Octet-string-to-encapsulation conversion (decoding)
 *
 * @param params SIKE parameters
 * @param ct Octet-encoded encapsulation to be decoded.
 * @param c0 Decoded parameter
 * @param c1 Decoded parameter
 * @return
 */
int ostoencaps(const sike_params_t* params,
               const unsigned char* ct,
               sike_public_key_t* c0,
               unsigned char* c1);

/**
 * Encapsulation-to-octet-string conversion (encoding).
 *
 * Consists of a public-key-to-octet-string conversion for c0,
 * and encoding of c1.
 *
 *
 * @param params SIKE parameters
 * @param c0 Montgomery curve to be encoded with a public-key-to-octet-string conversion.
 * @param c1 Parameter to be encoded
 * @param ct Octet-string encoded encapsulation
 * @return
 */
void encapstoos(const sike_params_t* params,
               const sike_public_key_t* c0,
               const unsigned char* c1,
               unsigned char* ct);

/**
 * Encapsulation-to-octet-string conversion (encoding).
 * Allocates memory for the encoding.
 *
 * @param params SIKE parameters
 * @param c0 Montgomery curve to be encoded with a public-key-to-octet-string conversion.
 * @param c1 Parameter to be encoded
 * @param ct Octet-string encoded encapsulation. Will be allocated.
 * @param ctLen Length of allocated memory
 * @return
 */
void encapstoos_alloc(const sike_params_t *params,
                     const sike_public_key_t *c0,
                     const unsigned char *c1,
                     unsigned char **ct,
                     size_t *ctLen);

/**
 * Returns the length of octet-string-encoded encapsulations (ct).
 *
 * @param params SIKE parameters
 * @return Length of the octet-string-encoded encapsulations (ct)
 */
size_t encapstoos_len(const sike_params_t* params);


// memory functions
#define MEM_NOT_FREE 0
#define MEM_FREE 1

/**
 * Clears the memory. Prevents compiler from optimizing it.
 *
 * @param ptr memory location to be cleared
 * @param memLen number of bytes
 * @param free_mem set 1 to free memory
 */
void clear_free( void* ptr, size_t size, int free_mem );

#endif //ISOGENY_REF_ENCODING_H
