#ifndef _HASH_H
#define _HASH_H

#include "parameters_HFE.h"
#include "predicate.h"
#include "init.h"



BEGIN_EXTERNC
    /* For KeccakWidth1600_Sponge */
    #include <libkeccak.a.headers/KeccakSpongeWidth1600.h>
    /* For TupleHash128 and TupleHash256 */
    #include <libkeccak.a.headers/SP800-185.h>
END_EXTERNC


#if (K<=128)
    #define SHA2 SHA256
    #define SHA3 SHA3_256
    #define SIZE_DIGEST 32
    #define SIZE_DIGEST_UINT 4
    #define SIZE_2_DIGEST 64
    #define EQUALHASH ISEQUAL256
    #define COPYHASH COPY256
    #define SPONGE(input,inputByteLen,output,outputByteLen) \
        KeccakWidth1600_Sponge(1088, 512, input, inputByteLen, \
                               0x06, output, outputByteLen)
    #define TUPLEHASH TupleHash128
#else
    #define TUPLEHASH TupleHash256

    #if (K<=192)
        #define SHA2 SHA384
        #define SHA3 SHA3_384
        #define SIZE_DIGEST 48
        #define SIZE_DIGEST_UINT 6
        #define SIZE_2_DIGEST 96
        #define EQUALHASH ISEQUAL384
        #define COPYHASH COPY384
        #define SPONGE(input,inputByteLen,output,outputByteLen) \
            KeccakWidth1600_Sponge(832, 768, input, inputByteLen, \
                                   0x06, output, outputByteLen)
    #else
        #define SHA2 SHA512
        #define SHA3 SHA3_512
        #define SIZE_DIGEST 64
        #define SIZE_DIGEST_UINT 8
        #define SIZE_2_DIGEST 128
        #define EQUALHASH ISEQUAL512
        #define COPYHASH COPY512
        #define SPONGE(input,inputByteLen,output,outputByteLen) \
            KeccakWidth1600_Sponge(576, 1024, input, inputByteLen, \
                                   0x06, output, outputByteLen)
    #endif
#endif




#if 0
    #define SHA2_ENABLED
#endif

#ifdef SHA2_ENABLED
    #include <openssl/sha.h>

    #define HASH(output,m,len) SHA2(m,len,output);
#else
    BEGIN_EXTERNC
        #include <libkeccak.a.headers/SimpleFIPS202.h>
    END_EXTERNC
    #define HASH(output,m,len) SHA3(output,m,len)
#endif



#endif
