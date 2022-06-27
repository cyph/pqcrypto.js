#ifndef _SIGN_OPENHFE_H
#define _SIGN_OPENHFE_H

#include "prefix_name.h"
#include "arch.h"
#include <stdint.h>
#include <stddef.h>


int PREFIX_NAME(sign_openHFE_uncomp_pk)(const unsigned char* m, size_t len,
                                        const unsigned char* sm8,
                                        const uint8_t* pk,
                                        void (*eval_pk)());
#define sign_openHFE_uncomp_pk PREFIX_NAME(sign_openHFE_uncomp_pk)


int PREFIX_NAME(sign_openHFE_huncomp_pk)(const unsigned char* m, size_t len,
                                         const unsigned char* sm8,
                                         const uint8_t* pk, const uint8_t* hpk,
                                         void (*eval_pk)());
#define sign_openHFE_huncomp_pk PREFIX_NAME(sign_openHFE_huncomp_pk)


/* Verify a signature: m is the original message, sm8 is the signed message, 
 * pk is the public-key. */
int PREFIX_NAME(sign_openHFE)(const unsigned char* m, size_t len,
                              const unsigned char* sm8, const uint8_t* pk);
#define sign_openHFE PREFIX_NAME(sign_openHFE)


#endif

