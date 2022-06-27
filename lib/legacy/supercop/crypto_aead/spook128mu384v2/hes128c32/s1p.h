/* MIT License
 *
 * Copyright (c) 2019 Gaëtan Cassiers
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef _H_S1P_H_
#define _H_S1P_H_

#include "parameters.h"

// Size of the P parameter
#define P_NBYTES 16

void s1p_encrypt(unsigned char *c, unsigned long long *clen,
                 const unsigned char *ad, unsigned long long adlen,
                 const unsigned char *m, unsigned long long mlen,
                 const unsigned char *k, const unsigned char *p,
                 const unsigned char *n);

int s1p_decrypt(unsigned char *m, unsigned long long *mlen,
                const unsigned char *ad, unsigned long long adlen,
                const unsigned char *c, unsigned long long clen,
                const unsigned char *k, const unsigned char *p,
                const unsigned char *n);

void init_keys(const unsigned char **k, unsigned char p[P_NBYTES],
               const unsigned char *k_glob);

#endif //_H_S1P_H_
