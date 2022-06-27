/**
 * @file cpa-pke.h
 * @author Takuya HAYASHI (t-hayashi@eedept.kobe-u.ac.jp)
 */

#ifndef _CPA_PKE_H
#define _CPA_PKE_H

#include "type.h"

void lotus_cpa_pke_keypair(U16 *pk, U16 *sk);
void lotus_cpa_pke_enc(U16 *ct, const U8 *sigma, const U16 *pk);
void lotus_cpa_pke_dec(U8 *sigma, const U16 *ct, const U16 *sk);

#endif
