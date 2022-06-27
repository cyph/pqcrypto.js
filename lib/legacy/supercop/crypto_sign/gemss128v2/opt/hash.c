#include "hash.h"


/* A simple way to use SHA3 from OpenSSL */
#define MACRO_SHA3(SHA3_NAME,EVP_SHA3) \
int SHA3_NAME(unsigned char *output, const unsigned char *m, size_t len)\
{\
    unsigned int s;\
    return EVP_Digest((const unsigned char*)m,len,output,&s,EVP_SHA3(),NULL);\
}


/* Here, the code of EVP_Digest is inlined from digest.c */
#define MACRO_SHA3_INLINED(SHA3_NAME,EVP_SHA3) \
int SHA3_NAME(unsigned char *output, const unsigned char *m, size_t len)\
{\
    EVP_MD_CTX *ctx;\
    unsigned int s;\
    int ret;\
\
    ctx=EVP_MD_CTX_new();\
    if (ctx==NULL)\
        return 0;\
    EVP_MD_CTX_set_flags(ctx,EVP_MD_CTX_FLAG_ONESHOT);\
    ret=EVP_DigestInit_ex(ctx,EVP_SHA3(),NULL)\
     && EVP_DigestUpdate(ctx,m,len)\
     && EVP_DigestFinal_ex(ctx,output,&s);\
    EVP_MD_CTX_free(ctx);\
\
    return ret;\
}


#ifdef ENABLED_SHA3_OPENSSL
    #include <openssl/evp.h>
    MACRO_SHA3_INLINED(PREFIX_NAME(sha3_256),EVP_sha3_256);
    MACRO_SHA3_INLINED(PREFIX_NAME(sha3_384),EVP_sha3_384);
    MACRO_SHA3_INLINED(PREFIX_NAME(sha3_512),EVP_sha3_512);
#endif


