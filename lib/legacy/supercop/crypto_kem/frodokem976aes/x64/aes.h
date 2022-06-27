/********************************************************************************************
* Header defining the APIs for AES
*
* If USE_OPENSSL flag is defined it uses OpenSSL's AES implementation. 
* Otherwise, it uses a standalone implementation
*********************************************************************************************/

#ifndef __AES_H
#define __AES_H

#include <stdint.h>
#include <stdlib.h>
#include "config.h"

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
void handleErrors(void);
void AES128_free_schedule(EVP_CIPHER_CTX *schedule);
void AES256_free_schedule(EVP_CIPHER_CTX *schedule);

#endif
