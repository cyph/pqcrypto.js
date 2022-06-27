#ifndef _HASH_H
#define _HASH_H

#include "arch.h"
#include "choice_crypto.h"
#include "parameters_HFE.h"
#include "predicate.h"
#include "init.h"


/******************************************************************/
/****************** Choice of the hash functions ******************/
/******************************************************************/


/* Choice of the hash function */
/* The user is allowed to switch between SHA2 and SHA3 */
#if (defined(QUARTZ)||defined(QUARTZ_V1))
    #define CHOICE_HASH_SHA1
#elif 0
    #define CHOICE_HASH_SHA2
#else
    #define CHOICE_HASH_SHA3
#endif


/******************************************************************/
/******************** Enable the hash functions *******************/
/******************************************************************/


/* Use of third libraries */
/* The user is allowed to switch between OpenSSL and XKCP */
/* The user can define several macros, while several SHA3 are not defined
   (if several SHA3 are defined, XKCP has priority). */
#ifdef CHOICE_HASH_SHA1
    #define ENABLED_SHA1_OPENSSL
#endif
#if defined(CHOICE_HASH_SHA2)
    #define ENABLED_SHA2_OPENSSL
#endif
#if defined(CHOICE_HASH_SHA3)
    /* XKCP is constant-time and faster than OpenSSL */
    #define ENABLED_SHA3_XKCP
    /* #define ENABLED_SHA3_OPENSSL */
#endif


#define ENABLED_SHAKE_XKCP
/* #define ENABLED_TUPLEHASH_XKCP XXX Disabled XXX */


/******************************************************************/
/***************** Include for the hash functions *****************/
/******************************************************************/


/* We minimize the numbers of #include to decrease the dependencies with the
   third libraries. */
#if (defined(ENABLED_SHA1_OPENSSL)||defined(ENABLED_SHA2_OPENSSL))
    #include <openssl/sha.h>
#endif


#ifdef ENABLED_SHA2_OPENSSL
    #include "randombytes.h"

    #if ENABLED_OPENSSL_FIPS
        #include <openssl/err.h>
        #include <openssl/crypto.h>
    #endif
#endif


#ifdef ENABLED_SHA3_OPENSSL
    #include <stddef.h>
    #include "prefix_name.h"
    int PREFIX_NAME(sha3_256)(unsigned char *output, const unsigned char *m,
                                                     size_t len);
    int PREFIX_NAME(sha3_384)(unsigned char *output, const unsigned char *m,
                                                     size_t len);
    int PREFIX_NAME(sha3_512)(unsigned char *output, const unsigned char *m,
                                                     size_t len);
    #define sha3_256 PREFIX_NAME(sha3_256)
    #define sha3_384 PREFIX_NAME(sha3_384)
    #define sha3_512 PREFIX_NAME(sha3_512)
#endif


#if (defined(ENABLED_SHA3_XKCP)||defined(ENABLED_SHAKE_XKCP))
    BEGIN_EXTERNC
        #include <libkeccak.a.headers/SimpleFIPS202.h>
    END_EXTERNC
#endif


#ifdef ENABLED_SHAKE_XKCP
    BEGIN_EXTERNC
        #include <libkeccak.a.headers/KeccakHash.h>
    END_EXTERNC
#endif


#ifdef ENABLED_TUPLEHASH_XKCP
    BEGIN_EXTERNC
        #include <libkeccak.a.headers/SP800-185.h>
    END_EXTERNC
#endif


/******************************************************************/
/**************** Macro to call the hash functions ****************/
/******************************************************************/


#define SHA1_OPENSSL(output,m,len) SHA1(m,len,output)
#define SHA256_OPENSSL(output,m,len) SHA256(m,len,output)
#define SHA384_OPENSSL(output,m,len) SHA384(m,len,output)
#define SHA512_OPENSSL(output,m,len) SHA512(m,len,output)

#define SHA256_OPENSSL_FIPS(output,m,len) \
            if(FIPS_mode()) \
            {\
                /* Set to off the FIPS mode */\
                if(FIPS_mode_set(0)!=1)\
                {\
                    exit(ERR_get_error());\
                }\
            }\
            SHA256_OPENSSL(output,m,len);
#define SHA384_OPENSSL_FIPS(output,m,len) \
            if(FIPS_mode()) \
            {\
                /* Set to off the FIPS mode */\
                if(FIPS_mode_set(0)!=1)\
                {\
                    exit(ERR_get_error());\
                }\
            }\
            SHA384_OPENSSL(output,m,len);
#define SHA512_OPENSSL_FIPS(output,m,len) \
            if(FIPS_mode()) \
            {\
                /* Set to off the FIPS mode */\
                if(FIPS_mode_set(0)!=1)\
                {\
                    exit(ERR_get_error());\
                }\
            }\
            SHA512_OPENSSL(output,m,len);

/* Format: SHA3_*(output,m,len) */
#if 0
    #define SHA3_256_XKCP SHA3_256
    #define SHA3_384_XKCP SHA3_384
    #define SHA3_512_XKCP SHA3_512
#else
    /* SHA3_* is inlined from SimpleFIPS202.c */
    #define SHA3_256_XKCP(output,m,len) \
        KeccakWidth1600_Sponge(1088, 512, m, len, 0x06, output, 32)
    #define SHA3_384_XKCP(output,m,len) \
        KeccakWidth1600_Sponge(832, 768, m, len, 0x06, output, 48)
    #define SHA3_512_XKCP(output,m,len) \
        KeccakWidth1600_Sponge(576, 1024, m, len, 0x06, output, 64)
#endif

/* Format: SHAKE*(output,outputByteLen,input,inputByteLen) */
#if 0
    #define SHAKE128_XKCP SHAKE128
    #define SHAKE256_XKCP SHAKE256
#else
    /* SHAKE* is inlined from SimpleFIPS202.c */
    #define SHAKE128_XKCP(output,outputByteLen,m,len) \
        KeccakWidth1600_Sponge(1344, 256, m, len, 0x1F, output, outputByteLen)
    #define SHAKE256_XKCP(output,outputByteLen,m,len) \
        KeccakWidth1600_Sponge(1088, 512, m, len, 0x1F, output, outputByteLen)
#endif

/* To call with:
        Keccak_HashInstance hashInstance;
        Keccak_HashIUF_SHAKE*_XKCP(&hashInstance,data,databitlen);
   And after a call to Keccak_HashIUF_SHAKE*_XKCP, to use one or several times:
        Keccak_HashSqueeze(&hashInstance,output,outputbitlen);
  XXX Here, length in bits XXX
*/
#define Keccak_HashIUF_SHAKE128_XKCP(hashInstance,data,databitlen) \
    Keccak_HashInitialize_SHAKE128(hashInstance);\
    Keccak_HashUpdate(hashInstance,data,databitlen);\
    Keccak_HashFinal(hashInstance,0);
#define Keccak_HashIUF_SHAKE256_XKCP(hashInstance,data,databitlen) \
    Keccak_HashInitialize_SHAKE256(hashInstance);\
    Keccak_HashUpdate(hashInstance,data,databitlen);\
    Keccak_HashFinal(hashInstance,0);

#define TUPLEHASH128_XKCP TupleHash128
#define TUPLEHASH256_XKCP TupleHash256


/************************************************************************/
/* Macro to call the hash functions corresponding to the security level */
/************************************************************************/


/* Choice of the hash function */
#if (K<=128)
    #if ENABLED_OPENSSL_FIPS
        #define SHA2 SHA256_OPENSSL_FIPS
    #else
        #define SHA2 SHA256_OPENSSL
    #endif

    #ifdef ENABLED_SHA3_XKCP
        #define SHA3 SHA3_256_XKCP
    #elif defined(ENABLED_SHA3_OPENSSL)
        #define SHA3 sha3_256
    #endif
#elif (K<=192)
    #if ENABLED_OPENSSL_FIPS
        #define SHA2 SHA384_OPENSSL_FIPS
    #else
        #define SHA2 SHA384_OPENSSL
    #endif

    #ifdef ENABLED_SHA3_XKCP
        #define SHA3 SHA3_384_XKCP
    #elif defined(ENABLED_SHA3_OPENSSL)
        #define SHA3 sha3_384
    #endif
#else
    #if ENABLED_OPENSSL_FIPS
        #define SHA2 SHA512_OPENSSL_FIPS
    #else
        #define SHA2 SHA512_OPENSSL
    #endif

    #ifdef ENABLED_SHA3_XKCP
        #define SHA3 SHA3_512_XKCP
    #elif defined(ENABLED_SHA3_OPENSSL)
        #define SHA3 sha3_512
    #endif
#endif


/* Choice of SHAKE and TupleHash */
#if (K<=128)
    #define SHAKE SHAKE128_XKCP
    #define Keccak_HashIUF_SHAKE Keccak_HashIUF_SHAKE128_XKCP
    #define TUPLEHASH TUPLEHASH128_XKCP
#else
    #define SHAKE SHAKE256_XKCP
    #define Keccak_HashIUF_SHAKE Keccak_HashIUF_SHAKE256_XKCP
    #define TUPLEHASH TUPLEHASH256_XKCP
#endif


/******************************************************************/
/******** Macro to call the chosen hash function of MQsoft ********/
/******************************************************************/


#ifdef CHOICE_HASH_SHA1
    #define HASH SHA1
#elif defined(CHOICE_HASH_SHA2)
    #define HASH SHA2
#else
    #define HASH SHA3
#endif


/******************************************************************/
/************************** Other tools ***************************/
/******************************************************************/


BEGIN_EXTERNC
    /* For KeccakWidth1600_Sponge */
    #include <libkeccak.a.headers/KeccakSpongeWidth1600.h>
END_EXTERNC


#if (K<=80)
    #define SIZE_DIGEST 20
    #define SIZE_DIGEST_UINT 3
#elif (K<=128)
    #define SIZE_DIGEST 32
    #define SIZE_DIGEST_UINT 4
    #define SIZE_2_DIGEST 64
    #define EQUALHASH_NOCST ISEQUAL4_NOCST
    #define COPYHASH COPY4
#elif (K<=192)
    #define SIZE_DIGEST 48
    #define SIZE_DIGEST_UINT 6
    #define SIZE_2_DIGEST 96
    #define EQUALHASH_NOCST ISEQUAL6_NOCST
    #define COPYHASH COPY6
#else
    #define SIZE_DIGEST 64
    #define SIZE_DIGEST_UINT 8
    #define SIZE_2_DIGEST 128
    #define EQUALHASH_NOCST ISEQUAL8_NOCST
    #define COPYHASH COPY8
#endif


#define EQUALHASH(a,b) f_ISEQUAL(a,b,SIZE_DIGEST_UINT)


/* XXX Bytes XXX */
#define expandSeed(output,outputByteLen,seed,seedByteLen) \
        SHAKE(output,outputByteLen,seed,seedByteLen)

#define expandSeedCxtDeclaration Keccak_HashInstance hashInstance
#define expandSeedIUF Keccak_HashIUF_SHAKE
#define expandSeedSqueeze Keccak_HashSqueeze


#endif

