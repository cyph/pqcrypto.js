/********************************************************************************************
* Functions for AES
*
* If USE_OPENSSL flag is defined it uses OpenSSL's AES implementation. 
* Otherwise, it uses a standalone implementation
*********************************************************************************************/

#include <assert.h>
#include <string.h>
#include "aes.h"
#include "aes_local.h"

void handleErrors(void)
{
    ERR_print_errors_fp(stderr);
    abort();
}


void AES128_free_schedule(EVP_CIPHER_CTX *schedule) {
    EVP_CIPHER_CTX_free(schedule);
}


void AES256_free_schedule(EVP_CIPHER_CTX *schedule) {
    EVP_CIPHER_CTX_free(schedule);
}

