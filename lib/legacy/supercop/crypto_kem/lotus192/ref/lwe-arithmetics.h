/**
 * @file lwe-arithmetics.h
 * @author Takuya HAYASHI (t-hayashi@eedept.kobe-u.ac.jp)
 */

#ifndef _LWE_ARITHMETICS_H
#define _LWE_ARITHMETICS_H

#include "type.h"

void submul(U16 *P, const U16 *A, const U16 *S);
void addmul_concat(U16 *c, const U16 *e, const U16 *A, const U16 *P);
void addmul(U16 *m, const U16 *c, const U16 *S);
void redc(U16 *v, const int len);

void add_sigma(U16 *c2, const U8 *sigma);
void reconstruct(U8 *sigma, const U16 *t);

#endif
