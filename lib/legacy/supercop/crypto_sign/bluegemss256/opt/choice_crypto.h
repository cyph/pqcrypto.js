#ifndef _CHOICE_CRYPTO_H
#define _CHOICE_CRYPTO_H




/* To test the library. */
/* Value 0: the user can modify parameters to test the library. */
/* The other values are the number of words of an element of GF(2^n). */
/* Value -1 is for 0.5 word. */
/* Is okay: -1,1,2,3,4,5,6,7,8,9 */
/* #define TEST_LIBRARY (9) */



/** Level of security of the cryptosystem and the hash functions. */
#ifndef TEST_LIBRARY
    #define K 256U
#else
/*        #define K 256U */
#endif



/** Choice of the current cryptosystem. */
#ifndef TEST_LIBRARY
    #define BlueGeMSS
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


#endif


    #define INNER_DualModeMS 1




/** Choice of a HFE-based multivariate scheme. */
#define HFE 1


/** Boolean for the inner mode. */
#define InnerMode 1
/** Boolean for the dual mode. */
#define DualMode 0



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
    #define CRYPTO_NAME InnerMode_
#else
    #define CRYPTO_NAME MQsoft_
#endif



/** Set the EUF-CMA property of the cryptosystem. */
#if 0
    #define EUF_CMA_PROPERTY 1
#else
    #define EUF_CMA_PROPERTY 0
#endif





#endif
