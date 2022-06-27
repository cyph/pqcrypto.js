/// @file utils_hash.h
/// @brief the interface for adapting hash functions.
///
///
#ifndef _UTILS_HASH_H_
#define _UTILS_HASH_H_


// for the definition of _HASH_LEN.
#include "hash_len_config.h"


#ifdef  __cplusplus
extern  "C" {
#endif


int hash_msg( unsigned char * digest , unsigned len_digest , const unsigned char * m , unsigned long long mlen );



#ifdef  __cplusplus
}
#endif



#endif // _UTILS_HASH_H_

