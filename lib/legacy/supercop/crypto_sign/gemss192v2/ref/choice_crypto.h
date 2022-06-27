#ifndef _CHOICE_CRYPTO_H
#define _CHOICE_CRYPTO_H


/* To test the library. */
/* Value 0: the user can modify parameters to test the library. */
/* The other values are the number of 32-bit words of an element of GF(2^n),
   i.e. ceil(n/32), between 1 and 18. */
/* #define TEST_LIBRARY (0) */



/** Level of security of the cryptosystem and the hash functions. */
#ifndef TEST_LIBRARY
    #define K 192U
#else
/*        #define K 256U */
#endif



/** Choice of the current cryptosystem. */
#ifndef TEST_LIBRARY
    #define GeMSS
/* To choose parameters of GeMSS */
/* #define GeMSS */
/* To choose parameters of BlueGeMSS */
/* #define BlueGeMSS */
/* To choose parameters of RedGeMSS */
/* #define RedGeMSS */
/* To choose parameters of FGeMSS */
/* #define FGeMSS */
/* To choose parameters of DualModeMS */
/* #define DualModeMS */
/* To use QUARTZ (version 2, is broken) */
/* #define QUARTZ */
/* To use QUARTZ_V1 (is broken) */
/* #define QUARTZ_V1 */


#endif


#define INNER_DualModeMS 1




/** Choice of a HFE-based multivariate scheme. */
#if (defined(GeMSS)||defined(BlueGeMSS)||defined(RedGeMSS)||defined(FGeMSS)||\
     defined(QUARTZ)||defined(QUARTZ_V1)||\
     defined(DualModeMS)||defined(TEST_LIBRARY))
    #define HFE 1
#else
    #define HFE 0
#endif



#if (defined(GeMSS)||defined(BlueGeMSS)||defined(RedGeMSS)||defined(FGeMSS)||\
    defined(QUARTZ)||defined(QUARTZ_V1)||\
    INNER_DualModeMS||defined(TEST_LIBRARY))
    /** Boolean for the inner mode. */
    #define InnerMode 1
    /** Boolean for the dual mode. */
    #define DualMode 0
#else
    /** Boolean for the inner mode. */
    #define InnerMode 0
    /** Boolean for the dual mode. */
    #define DualMode 1
#endif

#if InnerMode
    /** Boolean enabling the code of the dual mode. */
    #define ENABLED_DUALMODE 0
#else
    /** Boolean enabling the code of the dual mode, necessarily True here. */
    #define ENABLED_DUALMODE 1
#endif




/** Prefix which is the name of the current used cryptosystem. */
#ifdef GeMSS
    #define CRYPTO_NAME GeMSS_
#elif defined(BlueGeMSS)
    #define CRYPTO_NAME BlueGeMSS_
#elif defined(RedGeMSS)
    #define CRYPTO_NAME RedGeMSS_
#elif defined(FGeMSS)
    #define CRYPTO_NAME FGeMSS_
#elif defined(DualModeMS)
    #if INNER_DualModeMS
        #define CRYPTO_NAME InnerMode_
    #else
        #define CRYPTO_NAME DualModeMS_
    #endif
#elif defined(QUARTZ)
    #define CRYPTO_NAME QUARTZ_
#elif defined(QUARTZ_V1)
    #define CRYPTO_NAME QUARTZ_V1_
#else
    #define CRYPTO_NAME MQsoft_
#endif



/** Set the EUF-CMA property of the cryptosystem. */
#define EUF_CMA_PROPERTY 0





#endif

