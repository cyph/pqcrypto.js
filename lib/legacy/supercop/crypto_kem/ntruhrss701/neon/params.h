/*=============================================================================
This file has been adapted from the implementation 
(available at, CC0-1.0 License https://github.com/jschanck/ntru) 
of "NTRU:A submission to the NIST post-quantum standardization effort"
by : Cong Chen, Oussama Danba, Jeffrey Hoffstein, Andreas Hülsing, 
Joost Rijneveld, Tsunekazu Saito, John M. Schanck, Peter Schwabe, 
William Whyte,Keita Xagawa, Takashi Yamakawa, Zhenfei Zhang.
=============================================================================*/

#ifndef PARAMS_H
#define PARAMS_H

#define NTRU_HRSS
#define NTRU_N 701
#define NTRU_LOGQ 13

#ifndef CRYPTO_NAMESPACE
#define CRYPTO_NAMESPACE(s) s
#endif

#define NTRU_N32 704
#define NTRU_N_PAD 704

/* Do not modify below this line */

#define PAD32(X) (((X + 31)/32)*32)

#define NTRU_Q (1 << NTRU_LOGQ)

#define NTRU_SEEDBYTES       32
#define NTRU_PRFKEYBYTES     32
#define NTRU_SHAREDKEYBYTES  32

#define NTRU_SAMPLE_IID_BYTES  (NTRU_N-1)
#define NTRU_SAMPLE_FG_BYTES   (2*NTRU_SAMPLE_IID_BYTES)
#define NTRU_SAMPLE_RM_BYTES   (2*NTRU_SAMPLE_IID_BYTES)

#define NTRU_PACK_DEG (NTRU_N-1)
#define NTRU_PACK_TRINARY_BYTES    ((NTRU_PACK_DEG+4)/5)

#define NTRU_OWCPA_MSGBYTES       (2*NTRU_PACK_TRINARY_BYTES)
#define NTRU_OWCPA_PUBLICKEYBYTES ((NTRU_LOGQ*NTRU_PACK_DEG+7)/8)
#define NTRU_OWCPA_SECRETKEYBYTES (2*NTRU_PACK_TRINARY_BYTES + NTRU_OWCPA_PUBLICKEYBYTES)
#define NTRU_OWCPA_BYTES          ((NTRU_LOGQ*NTRU_PACK_DEG+7)/8)

#define NTRU_PUBLICKEYBYTES  (NTRU_OWCPA_PUBLICKEYBYTES)
#define NTRU_SECRETKEYBYTES  (NTRU_OWCPA_SECRETKEYBYTES + NTRU_PRFKEYBYTES)
#define NTRU_CIPHERTEXTBYTES (NTRU_OWCPA_BYTES)

#endif
