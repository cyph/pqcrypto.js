/*---------------------------------------------------------------------
This file has been adapted from the implementation 
(available at, Public Domain https://github.com/pq-crystals/kyber) 
of "CRYSTALS – Kyber: a CCA-secure module-lattice-based KEM"
by : Joppe Bos, Leo Ducas, Eike Kiltz, Tancrede Lepoint, 
Vadim Lyubashevsky, John M. Schanck, Peter Schwabe & Damien stehle
----------------------------------------------------------------------*/

#include <stdio.h>
#include "apiorig.h"
#include "poly.h"
#include "cbd.h"
#include "fips202.h"

void poly_getnoise(uint16_t *r,const unsigned char *seed, unsigned char nonce)
{
  unsigned char buf[SABER_N];

  cshake128_simple(buf,SABER_N,nonce,seed,SABER_NOISESEEDBYTES);

  cbd( r, buf);
}

