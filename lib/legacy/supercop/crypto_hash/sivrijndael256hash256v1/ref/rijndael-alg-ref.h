/* rijndael-alg-ref.h   v2.0   August '99
 * Reference ANSI C code
 * authors: Paulo Barreto
 *          Vincent Rijmen
 */
/*
                        ------------------------------
                        Rijndael ANSI C Reference Code
                        ------------------------------

                                October 24, 2000

                                  Disclaimer


This software package was submitted to the National Institute of Standards and
Technology (NIST) during the Advanced Encryption Standard (AES) development
effort by Joan Daemen and Vincent Rijmen, the developers of the Rijndael algorithm.

This software is distributed in compliance with export regulations (see below), and
it is intended for non-commercial use, only.   NIST does not support this software 
and does not provide any guarantees or warranties as to its performance, fitness 
for any particular application, or validation under the Cryptographic Module
Validation Program (CMVP) <http://csrc.nist.gov/cryptval/>.  NIST does not accept 
any liability associated with its use or misuse.  This software is provided as-is. 
By accepting this software the user agrees to the terms stated herein.

                            -----------------------

                              Export Restrictions


Implementations of cryptography are subject to United States Federal
Government export controls.  Export controls on commercial encryption products 
are administered by the Bureau of Export Administration (BXA) 
<http://www.bxa.doc.gov/Encryption/> in the U.S. Department of Commerce. 
Regulations governing exports of encryption are found in the Export 
Administration Regulations (EAR), 15 C.F.R. Parts 730-774.
*/

#ifndef __RIJNDAEL_ALG_H
#define __RIJNDAEL_ALG_H

#define keyBits      128
#define blockBits    256
#define KC          (keyBits/32)
#define BC          (blockBits/32)
#define ROUNDS       14
#define MAXBC				 BC
#define MAXKC				 KC
#define MAXROUNDS		 ROUNDS

typedef unsigned char		word8;	
typedef unsigned short	word16;	
typedef unsigned long		word32;


int rijndaelKeySched (word8 k[4][MAXKC], 
		word8 rk[MAXROUNDS+1][4][MAXBC]);
int rijndaelEncrypt (word8 a[4][MAXBC],
    word8 rk[MAXROUNDS+1][4][MAXBC], word8 domain);

#endif /* __RIJNDAEL_ALG_H */
