#ifndef __PARAMS_H__
#define __PARAMS_H__

#include <string.h>
#include <stdlib.h>

/*  Block cipher's block size*/ 
#define CRYPTO_BLOCKBYTES (16)



/* Primitive polynomial*/ 
#define PRIM_POLY_MOD_128 (0x87)



/***********************************************************************************
 * 
 * PARTIAL_BLOCK_LEN:	Computes the number of bytes in the (possibly) partial block.
 * 
 ***********************************************************************************/		
#define PARTIAL_BLOCK_LEN(blks_num,byte_len)	((byte_len-((blks_num-1)*CRYPTO_BLOCKBYTES)))

typedef unsigned char u8;
typedef unsigned long long u64;

#endif
