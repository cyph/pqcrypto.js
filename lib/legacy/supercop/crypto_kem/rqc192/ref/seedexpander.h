//
//  rng.h
//
//  Created by Bassham, Lawrence E (Fed) on 8/29/17.
//  Copyright © 2017 Bassham, Lawrence E (Fed). All rights reserved.
//

#ifndef rng_h
#define rng_h

#include <stdio.h>

#define RNG_SUCCESS      0
#define RNG_BAD_MAXLEN  -1
#define RNG_BAD_OUTBUF  -2
#define RNG_BAD_REQ_LEN -3


typedef struct {
    unsigned char   buffer[16];
    size_t          buffer_pos;
    size_t          length_remaining;
    unsigned char   key[32];
    unsigned char   ctr[16];
} AES_XOF_struct;


int seedexpander_init(AES_XOF_struct *ctx, unsigned char *seed, unsigned char *diversifier, size_t maxlen);
int seedexpander(AES_XOF_struct *ctx, unsigned char *x, size_t xlen);

#endif /* rng_h */
