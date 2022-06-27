/*=============================================================================
This file has been adapted from the implementation 
(available at, CC0-1.0 License https://github.com/jschanck/ntru) 
of "NTRU:A submission to the NIST post-quantum standardization effort"
by : Cong Chen, Oussama Danba, Jeffrey Hoffstein, Andreas Hülsing, 
Joost Rijneveld, Tsunekazu Saito, John M. Schanck, Peter Schwabe, 
William Whyte,Keita Xagawa, Takashi Yamakawa, Zhenfei Zhang.
=============================================================================*/

#include <string.h>
#include <stdint.h>
#include "verify.h"

/* returns 0 for equal strings, 1 for non-equal strings */
int verify(const unsigned char *a, const unsigned char *b, size_t len)
{
  uint64_t r;
  size_t i;
  r = 0;
  
  for(i=0;i<len;i++)
    r |= a[i] ^ b[i];
 
  r = (-r) >> 63;
  return r;
}

/* b = 1 means mov, b = 0 means don't mov*/
void cmov(unsigned char *r, const unsigned char *x, size_t len, unsigned char b)
{
  size_t i;

  b = -b;
  for(i=0;i<len;i++)
    r[i] ^= b & (x[i] ^ r[i]);
}
