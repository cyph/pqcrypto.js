/*=============================================================================
This file has been adapted from the implementation 
(available at, CC0-1.0 License https://github.com/jschanck/ntru) 
of "NTRU:A submission to the NIST post-quantum standardization effort"
by : Cong Chen, Oussama Danba, Jeffrey Hoffstein, Andreas Hülsing, 
Joost Rijneveld, Tsunekazu Saito, John M. Schanck, Peter Schwabe, 
William Whyte,Keita Xagawa, Takashi Yamakawa, Zhenfei Zhang.
=============================================================================*/

#ifndef SAMPLE_H
#define SAMPLE_H

#include "params.h"
#include "poly.h"

#define sample_fg CRYPTO_NAMESPACE(sample_fg)
#define sample_rm CRYPTO_NAMESPACE(sample_rm)
void sample_fg(poly *f, poly *g, const unsigned char uniformbytes[NTRU_SAMPLE_FG_BYTES]);
void sample_rm(poly *r, poly *m, const unsigned char uniformbytes[NTRU_SAMPLE_RM_BYTES]);

#define sample_iid CRYPTO_NAMESPACE(sample_iid)
void sample_iid(poly *r, const unsigned char uniformbytes[NTRU_SAMPLE_IID_BYTES]);

#define sample_fixed_type CRYPTO_NAMESPACE(sample_fixed_type)
void sample_fixed_type(poly *r, const unsigned char uniformbytes[NTRU_SAMPLE_FT_BYTES]);


#endif
