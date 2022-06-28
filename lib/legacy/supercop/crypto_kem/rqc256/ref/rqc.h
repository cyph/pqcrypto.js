/** 
 * \file rqc.h
 * \brief Functions of the RQC_PKE IND-CPA scheme
 */

#ifndef RQC_H
#define RQC_H

#include "ffi_vec.h"


void rqc_pke_keygen(unsigned char* pk, unsigned char* sk);
void rqc_pke_encrypt(ffi_vec& u, ffi_vec& v, const ffi_vec& m, unsigned char* theta, const unsigned char* pk);
void rqc_pke_decrypt(ffi_vec& m, const ffi_vec& u, const ffi_vec& v, const unsigned char* sk);

#endif

