/** 
 * \file rqc.h
 * \brief Functions of the RQC_PKE IND-CPA scheme
 */

#ifndef RQC_PKE_H
#define RQC_PKE_H

#include "rbc_vec.h"
#include "rbc_qre.h"


void rqc_pke_keygen(uint8_t* pk, uint8_t* sk);
void rqc_pke_encrypt(rbc_qre u, rbc_qre v, const rbc_vec m, uint8_t* theta, const uint8_t* pk);
void rqc_pke_decrypt(rbc_vec m, const rbc_qre u, const rbc_qre v, const uint8_t* sk);

#endif

