/*=============================================================================
This file has been adapted from the implementation 
(available at, CC0-1.0 License https://github.com/jschanck/ntru) 
of "NTRU:A submission to the NIST post-quantum standardization effort"
by : Cong Chen, Oussama Danba, Jeffrey Hoffstein, Andreas Hülsing, 
Joost Rijneveld, Tsunekazu Saito, John M. Schanck, Peter Schwabe, 
William Whyte,Keita Xagawa, Takashi Yamakawa, Zhenfei Zhang.
=============================================================================*/

#ifndef VERIFY_H
#define VERIFY_H

#include <stdio.h>

/* returns 0 for equal strings, 1 for non-equal strings */
int verify(const unsigned char *a, const unsigned char *b, size_t len);

#define cmov CRYPTO_NAMESPACE(cmov)
/* b = 1 means mov, b = 0 means don't mov*/
void cmov(unsigned char *r, const unsigned char *x, size_t len, unsigned char b);

#endif
