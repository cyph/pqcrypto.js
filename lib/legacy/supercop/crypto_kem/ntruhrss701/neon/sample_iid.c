/*=============================================================================
This file has been adapted from the implementation 
(available at, CC0-1.0 License https://github.com/jschanck/ntru) 
of "NTRU:A submission to the NIST post-quantum standardization effort"
by : Cong Chen, Oussama Danba, Jeffrey Hoffstein, Andreas Hülsing, 
Joost Rijneveld, Tsunekazu Saito, John M. Schanck, Peter Schwabe, 
William Whyte,Keita Xagawa, Takashi Yamakawa, Zhenfei Zhang.
=============================================================================*/

#include "sample.h"

static uint16_t mod3(uint16_t a)
{
  uint16_t r;
  int16_t t, c;

  r = (a >> 8) + (a & 0xff); // r mod 255 == a mod 255
  r = (r >> 4) + (r & 0xf); // r' mod 15 == r mod 15
  r = (r >> 2) + (r & 0x3); // r' mod 3 == r mod 3
  r = (r >> 2) + (r & 0x3); // r' mod 3 == r mod 3

  t = r - 3;
  c = t >> 15;

  return (c&r) ^ (~c&t);
}

void sample_iid(poly *r, const unsigned char uniformbytes[NTRU_SAMPLE_IID_BYTES])
{
  int i;
  /* {0,1,...,255} -> {0,1,2}; Pr[0] = 86/256, Pr[1] = Pr[-1] = 85/256 */
  for(i=0; i<NTRU_N-1; i++)
    r->coeffs[i] = mod3(uniformbytes[i]);

  r->coeffs[NTRU_N-1] = 0;
}
