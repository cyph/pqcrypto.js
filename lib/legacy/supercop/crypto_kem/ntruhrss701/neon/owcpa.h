/*=============================================================================
This file has been adapted from the implementation 
(available at, CC0-1.0 License https://github.com/jschanck/ntru) 
of "NTRU:A submission to the NIST post-quantum standardization effort"
by : Cong Chen, Oussama Danba, Jeffrey Hoffstein, Andreas Hülsing, 
Joost Rijneveld, Tsunekazu Saito, John M. Schanck, Peter Schwabe, 
William Whyte,Keita Xagawa, Takashi Yamakawa, Zhenfei Zhang.
=============================================================================*/

#ifndef OWCPA_H
#define OWCPA_H

#include "params.h"
#include "poly.h"

#define owcpa_samplemsg CRYPTO_NAMESPACE(owcpa_samplemsg)
void owcpa_samplemsg(unsigned char msg[NTRU_OWCPA_MSGBYTES],
                     const unsigned char seed[NTRU_SEEDBYTES]);

#define owcpa_keypair CRYPTO_NAMESPACE(owcpa_keypair)
void owcpa_keypair(unsigned char *pk,
                   unsigned char *sk,
                   const unsigned char seed[NTRU_SEEDBYTES]);

#define owcpa_enc CRYPTO_NAMESPACE(owcpa_enc)
void owcpa_enc(unsigned char *c,
               poly *r,
               const poly *m,
               const unsigned char *pk);

#define owcpa_dec CRYPTO_NAMESPACE(owcpa_dec)
int owcpa_dec(unsigned char *rm,
              const unsigned char *ciphertext,
              const unsigned char *secretkey);
#endif
