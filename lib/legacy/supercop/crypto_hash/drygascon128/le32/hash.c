#include "crypto_hash.h"
#include "drysponge.h"

int crypto_hash(
    unsigned char *out,
    const unsigned char *in,
    unsigned long long inlen
){
    (void) DRYSPONGE_enc; //avoid warning
    (void) DRYSPONGE_dec; //avoid warning
    DRYSPONGE_hash(
        in,     // message,
        inlen,  // mlen,
        out     //digest
    );
    return 0;
}
