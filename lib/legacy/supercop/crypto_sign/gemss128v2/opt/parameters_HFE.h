#ifndef _PARAMETERS_HFE_H
#define _PARAMETERS_HFE_H

#include "choice_crypto.h"

/****************** PARAMETERS FOR HFE ******************/


/** GF(q) */
#if HFE
    #define GFq 2U
    #define Log2_q 1
    /* For HFE, the previous parameter is necessarily 2. */

    /** This type stores an element of GF(q). */
    typedef unsigned char gf2;
#endif



/* Number of variables of the public-key */
#define HFEnv (HFEn+HFEv)

/* Number of equations of the public-key */
#define HFEm (HFEn-HFEDELTA)



#if (defined(QUARTZ)||defined(QUARTZ_V1))

#if (K==80)

    #define HFEn 103U
    #define HFEv 4U
    #define HFEDELTA 3U

    #define NB_ITE 4

    #define HFEDeg 129U
    #define HFEDegI 7U
    #define HFEDegJ 0U

    #define HFEs 0U

#else
    #error "K is not 80"
#endif


#elif defined(GeMSS)

/* GeMSS128 */
#if (K==128)

    #define HFEn 174U
    #define HFEv 12U
    #define HFEDELTA 12U

    #define NB_ITE 4

    #define HFEDeg 513U
    #define HFEDegI 9U
    #define HFEDegJ 0U

    #define HFEs 0U

/* GeMSS192 */
#elif (K==192)

    #define HFEn 265U
    #define HFEv 20U
    #define HFEDELTA 22U

    #define NB_ITE 4

    #define HFEDeg 513U
    #define HFEDegI 9U
    #define HFEDegJ 0U

    #define HFEs 0U

/* GeMSS256 */
#elif (K==256)

    #define HFEn 354U
    #define HFEv 33U
    #define HFEDELTA 30U

    #define NB_ITE 4

    #define HFEDeg 513U
    #define HFEDegI 9U
    #define HFEDegJ 0U

    #define HFEs 0U

/* GeMSS??? */
#else
    #error "K is not 128, 192 or 256"
#endif


#elif defined(BlueGeMSS)

/* BlueGeMSS128 */
#if (K==128)

    #define HFEn 175U
    #define HFEv 14U
    #define HFEDELTA 13U

    #define NB_ITE 4U

    #define HFEDeg 129U
    #define HFEDegI 7U
    #define HFEDegJ 0U

    #define HFEs 0U

/* BlueGeMSS192 */
#elif (K==192)

    #define HFEn 265U
    #define HFEv 23U
    #define HFEDELTA 22U

    #define NB_ITE 4U

    #define HFEDeg 129U
    #define HFEDegI 7U
    #define HFEDegJ 0U

    #define HFEs 0U

/* BlueGeMSS256 */
#elif (K==256)

    #define HFEn 358U
    #define HFEv 32U
    #define HFEDELTA 34U

    #define NB_ITE 4U

    #define HFEDeg 129U
    #define HFEDegI 7U
    #define HFEDegJ 0U

    #define HFEs 0U

/* BlueGeMSS??? */
#else
    #error "K is not 128, 192 or 256"
#endif


#elif defined(RedGeMSS)

/* RedGeMSS128 */
#if (K==128)

    #define HFEn 177U
    #define HFEv 15U
    #define HFEDELTA 15U

    #define NB_ITE 4U

    #define HFEDeg 17U
    #define HFEDegI 4U
    #define HFEDegJ 0U

    #define HFEs 0U

/* RedGeMSS192 */
#elif (K==192)

    #define HFEn 266U
    #define HFEv 25U
    #define HFEDELTA 23U

    #define NB_ITE 4U

    #define HFEDeg 17U
    #define HFEDegI 4U
    #define HFEDegJ 0U

    #define HFEs 0U

/* RedGeMSS256 */
#elif (K==256)

    #define HFEn 358U
    #define HFEv 35U
    #define HFEDELTA 34U

    #define NB_ITE 4U

    #define HFEDeg 17U
    #define HFEDegI 4U
    #define HFEDegJ 0U

    #define HFEs 0U

/* RedGeMSS??? */
#else
    #error "K is not 128, 192 or 256"
#endif


#elif defined(FGeMSS)

    #define NB_ITE 1U
    #define HFEs 0U

    /* FGeMSS(266) */
    #if (K==128)
        #define HFEn 266U

        #define HFEv 11U
        #define HFEDELTA 10U

        #define HFEDeg 129U
        #define HFEDegI 7U
        #define HFEDegJ 0U
    /* FGeMSS(402) */
    #elif (K==192)
        #define HFEn 402U

        #define HFEv 18U
        #define HFEDELTA 18U

        #define HFEDeg 640U
        #define HFEDegI 9U
        #define HFEDegJ 7U
    /* FGeMSS(537) */
    #elif (K==256)
        #define HFEn 537U

        #define HFEv 26U
        #define HFEDELTA 25U

        #define HFEDeg 1152U
        #define HFEDegI 10U
        #define HFEDegJ 7U
    /* FGeMSS(???) */
    #else
        #error "K is not 128, 192 or 256"
    #endif


#elif defined(DualModeMS)

/* DualModeMS128 */
#if (K==128)

    #define HFEn 266U
    #define HFEv 11U
    #define HFEDELTA 10U

    /* This parameter must not be changed */
    #define NB_ITE 1

    #define HFEDeg 129U
    #define HFEDegI 7U
    #define HFEDegJ 0U

    #define HFEs 0U

/* DualModeMS192 */
#elif (K==192)

    #define HFEn 402U
    #define HFEv 18U
    #define HFEDELTA 18U

    /* This parameter must not be changed */
    #define NB_ITE 1

    #define HFEDeg 129U
    #define HFEDegI 7U
    #define HFEDegJ 0U

    #define HFEs 0U

/* DualModeMS256 */
#elif (K==256)

    #define HFEn 544U
    #define HFEv 32U
    #define HFEDELTA 32U

    /* This parameter must not be changed */
    #define NB_ITE 1

    #define HFEDeg 129U
    #define HFEDegI 7U
    #define HFEDegJ 0U

    #define HFEs 0U

#else
    /* DualModeMS??? */
    #error "K is not 128, 192 or 256"
#endif


#elif defined(TEST_LIBRARY)

#if (!TEST_LIBRARY)
    /* The user can modify these values to test the library */

    /* Level of security of the cryptosystems 
       (2K is the level of security of the hash functions) */
    #define K 128U

    /* Degree of the extension GF(2^n) */
    #define HFEn 174U
    /* Number of vinegar variable */
    #define HFEv 12U
    /* Number of removed equations (the minus) */
    #define HFEDELTA 12U

    /* Number of iterations in the Feistel-Patarin scheme, different from 0 */
    #define NB_ITE 4

    /* Degree of the HFE polynomial: Deg = 2^DegI + 2^DegJ 
     * or Deg = 2^0 (with DegI==DegJ==0) */
    #define HFEDeg 513U
    #define HFEDegI 9U
    /* Requirement : DegI>=DegJ */
    #define HFEDegJ 0U

    /* Number of removed odd degree terms in the HFEv polynomial */
    #define HFEs 0U
#elif (TEST_LIBRARY==1)
    #define K 128U

    #define HFEn 29U
    #define HFEv 12U
    #define HFEDELTA 12U

    #define NB_ITE 4

    #define HFEDeg 513U
    #define HFEDegI 9U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==2)
    #define K 128U

    #define HFEn 62U
    #define HFEv 12U
    #define HFEDELTA 12U

    #define NB_ITE 4

    #define HFEDeg 513U
    #define HFEDegI 9U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==3)
    #define K 128U

    #define HFEn 95U
    #define HFEv 12U
    #define HFEDELTA 12U

    #define NB_ITE 4

    #define HFEDeg 513U
    #define HFEDegI 9U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==4)
    #define K 128U

    #define HFEn 126U
    #define HFEv 12U
    #define HFEDELTA 12U

    #define NB_ITE 4

    #define HFEDeg 513U
    #define HFEDegI 9U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==5)
    #define K 128U

    #define HFEn 156U
    #define HFEv 12U
    #define HFEDELTA 12U

    #define NB_ITE 4

    #define HFEDeg 513U
    #define HFEDegI 9U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==6)
    #define K 128U

    #define HFEn 186U
    #define HFEv 12U
    #define HFEDELTA 12U

    #define NB_ITE 4

    #define HFEDeg 513U
    #define HFEDegI 9U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==7)
    #define K 128U

    #define HFEn 220U
    #define HFEv 12U
    #define HFEDELTA 12U

    #define NB_ITE 4

    #define HFEDeg 513U
    #define HFEDegI 9U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==8)
    #define K 128U

    #define HFEn 252U
    #define HFEv 12U
    #define HFEDELTA 12U

    #define NB_ITE 4

    #define HFEDeg 513U
    #define HFEDegI 9U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==9)
    #define K 192U

    #define HFEn 278U
    #define HFEv 12U
    #define HFEDELTA 12U

    #define NB_ITE 4

    #define HFEDeg 513U
    #define HFEDegI 9U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==10)
    #define K 192U

    #define HFEn 314U
    #define HFEv 12U
    #define HFEDELTA 12U

    #define NB_ITE 4

    #define HFEDeg 513U
    #define HFEDegI 9U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==11)
    #define K 192U

    #define HFEn 345U
    #define HFEv 12U
    #define HFEDELTA 12U

    #define NB_ITE 4

    #define HFEDeg 513U
    #define HFEDegI 9U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==12)
    #define K 192U

    #define HFEn 375U
    #define HFEv 12U
    #define HFEDELTA 12U

    #define NB_ITE 4

    #define HFEDeg 513U
    #define HFEDegI 9U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==13)
    #define K 256U

    #define HFEn 414U
    #define HFEv 12U
    #define HFEDELTA 12U

    #define NB_ITE 4

    #define HFEDeg 513U
    #define HFEDegI 9U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==14)
    #define K 256U

    #define HFEn 441U
    #define HFEv 12U
    #define HFEDELTA 12U

    #define NB_ITE 4

    #define HFEDeg 513U
    #define HFEDegI 9U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==15)
    #define K 256U

    #define HFEn 476U
    #define HFEv 12U
    #define HFEDELTA 12U

    #define NB_ITE 4

    #define HFEDeg 513U
    #define HFEDegI 9U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==16)
    #define K 256U

    #define HFEn 508U
    #define HFEv 12U
    #define HFEDELTA 12U

    #define NB_ITE 4

    #define HFEDeg 513U
    #define HFEDegI 9U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==17)
    #define K 256U

    #define HFEn 543U
    #define HFEv 12U
    #define HFEDELTA 31U

    #define NB_ITE 4

    #define HFEDeg 513U
    #define HFEDegI 9U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==18)
    #define K 256U

    #define HFEn 574U
    #define HFEv 12U
    #define HFEDELTA 62U

    #define NB_ITE 4

    #define HFEDeg 513U
    #define HFEDegI 9U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==100)
    #define K 128U

    #define HFEn 268U
    #define HFEv 12U
    #define HFEDELTA 12U

    #define NB_ITE 1U

    #define HFEDeg 17U
    #define HFEDegI 4U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==101)
    #define K 128U

    #define HFEn 204U
    #define HFEv 15U
    #define HFEDELTA 12U

    #define NB_ITE 2U

    #define HFEDeg 17U
    #define HFEDegI 4U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==102)
    #define K 128U

    #define HFEn 186U
    #define HFEv 15U
    #define HFEDELTA 15U

    #define NB_ITE 3U

    #define HFEDeg 17U
    #define HFEDegI 4U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==103)
    #define K 128U

    #define HFEn 177U
    #define HFEv 15U
    #define HFEDELTA 15U

    #define NB_ITE 4U

    #define HFEDeg 17U
    #define HFEDegI 4U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==104)
    #define K 128U

    #define HFEn 268U
    #define HFEv 12U
    #define HFEDELTA 12U

    #define NB_ITE 1U

    #define HFEDeg 33U
    #define HFEDegI 5U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==105)
    #define K 128U

    #define HFEn 204U
    #define HFEv 15U
    #define HFEDELTA 12U

    #define NB_ITE 2U

    #define HFEDeg 33U
    #define HFEDegI 5U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==106)
    #define K 128U

    #define HFEn 186U
    #define HFEv 15U
    #define HFEDELTA 15U

    #define NB_ITE 3U

    #define HFEDeg 33U
    #define HFEDegI 5U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==107)
    #define K 128U

    #define HFEn 177U
    #define HFEv 15U
    #define HFEDELTA 15U

    #define NB_ITE 4U

    #define HFEDeg 33U
    #define HFEDegI 5U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==108)
    #define K 128U

    #define HFEn 266U
    #define HFEv 11U
    #define HFEDELTA 10U

    #define NB_ITE 1U

    #define HFEDeg 129U
    #define HFEDegI 7U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==109)
    #define K 128U

    #define HFEn 266U
    #define HFEv 11U
    #define HFEDELTA 10U

    #define NB_ITE 1U

    #define HFEDeg 130U
    #define HFEDegI 7U
    #define HFEDegJ 1U

    #define HFEs 3U
#elif (TEST_LIBRARY==110)
    #define K 128U

    #define HFEn 204U
    #define HFEv 12U
    #define HFEDELTA 12U

    #define NB_ITE 2U

    #define HFEDeg 129U
    #define HFEDegI 7U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==111)
    #define K 128U

    #define HFEn 204U
    #define HFEv 12U
    #define HFEDELTA 12U

    #define NB_ITE 2U

    #define HFEDeg 130U
    #define HFEDegI 7U
    #define HFEDegJ 1U

    #define HFEs 3U
#elif (TEST_LIBRARY==112)
    #define K 128U

    #define HFEn 185U
    #define HFEv 13U
    #define HFEDELTA 14U

    #define NB_ITE 3U

    #define HFEDeg 129U
    #define HFEDegI 7U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==113)
    #define K 128U

    #define HFEn 185U
    #define HFEv 13U
    #define HFEDELTA 14U

    #define NB_ITE 3U

    #define HFEDeg 130U
    #define HFEDegI 7U
    #define HFEDegJ 1U

    #define HFEs 3U
#elif (TEST_LIBRARY==114)
    #define K 128U

    #define HFEn 175U
    #define HFEv 14U
    #define HFEDELTA 13U

    #define NB_ITE 4U

    #define HFEDeg 129U
    #define HFEDegI 7U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==115)
    #define K 128U

    #define HFEn 175U
    #define HFEv 14U
    #define HFEDELTA 13U

    #define NB_ITE 4U

    #define HFEDeg 130U
    #define HFEDegI 7U
    #define HFEDegJ 1U

    #define HFEs 3U
#elif (TEST_LIBRARY==116)
    #define K 128U

    #define HFEn 265U
    #define HFEv 9U
    #define HFEDELTA 9U

    #define NB_ITE 1U

    #define HFEDeg 513U
    #define HFEDegI 9U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==117)
    #define K 128U

    #define HFEn 265U
    #define HFEv 9U
    #define HFEDELTA 9U

    #define NB_ITE 1U

    #define HFEDeg 514U
    #define HFEDegI 9U
    #define HFEDegJ 1U

    #define HFEs 3U
#elif (TEST_LIBRARY==118)
    #define K 128U

    #define HFEn 202U
    #define HFEv 11U
    #define HFEDELTA 10U

    #define NB_ITE 2U

    #define HFEDeg 513U
    #define HFEDegI 9U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==119)
    #define K 128U

    #define HFEn 202U
    #define HFEv 11U
    #define HFEDELTA 10U

    #define NB_ITE 2U

    #define HFEDeg 514U
    #define HFEDegI 9U
    #define HFEDegJ 1U

    #define HFEs 3U
#elif (TEST_LIBRARY==120)
    #define K 128U

    #define HFEn 183U
    #define HFEv 12U
    #define HFEDELTA 12U

    #define NB_ITE 3U

    #define HFEDeg 513U
    #define HFEDegI 9U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==121)
    #define K 128U

    #define HFEn 183U
    #define HFEv 12U
    #define HFEDELTA 12U

    #define NB_ITE 3U

    #define HFEDeg 514U
    #define HFEDegI 9U
    #define HFEDegJ 1U

    #define HFEs 3U
#elif (TEST_LIBRARY==122)
    #define K 128U

    #define HFEn 174U
    #define HFEv 12U
    #define HFEDELTA 12U

    #define NB_ITE 4U

    #define HFEDeg 513U
    #define HFEDegI 9U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==123)
    #define K 128U

    #define HFEn 174U
    #define HFEv 12U
    #define HFEDELTA 12U

    #define NB_ITE 4U

    #define HFEDeg 514U
    #define HFEDegI 9U
    #define HFEDegJ 1U

    #define HFEs 3U
#elif (TEST_LIBRARY==124)
    #define K 192U

    #define HFEn 404U
    #define HFEv 19U
    #define HFEDELTA 20U

    #define NB_ITE 1U

    #define HFEDeg 17U
    #define HFEDegI 4U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==125)
    #define K 192U

    #define HFEn 310U
    #define HFEv 23U
    #define HFEDELTA 22U

    #define NB_ITE 2U

    #define HFEDeg 17U
    #define HFEDegI 4U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==126)
    #define K 192U

    #define HFEn 279U
    #define HFEv 25U
    #define HFEDELTA 23U

    #define NB_ITE 3U

    #define HFEDeg 17U
    #define HFEDegI 4U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==127)
    #define K 192U

    #define HFEn 266U
    #define HFEv 25U
    #define HFEDELTA 23U

    #define NB_ITE 4U

    #define HFEDeg 17U
    #define HFEDegI 4U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==128)
    #define K 192U

    #define HFEn 404U
    #define HFEv 19U
    #define HFEDELTA 20U

    #define NB_ITE 1U

    #define HFEDeg 33U
    #define HFEDegI 5U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==129)
    #define K 192U

    #define HFEn 310U
    #define HFEv 23U
    #define HFEDELTA 22U

    #define NB_ITE 2U

    #define HFEDeg 33U
    #define HFEDegI 5U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==130)
    #define K 192U

    #define HFEn 279U
    #define HFEv 25U
    #define HFEDELTA 23U

    #define NB_ITE 3U

    #define HFEDeg 33U
    #define HFEDegI 5U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==131)
    #define K 192U

    #define HFEn 266U
    #define HFEv 25U
    #define HFEDELTA 23U

    #define NB_ITE 4U

    #define HFEDeg 33U
    #define HFEDegI 5U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==132)
    #define K 192U

    #define HFEn 402U
    #define HFEv 18U
    #define HFEDELTA 18U

    #define NB_ITE 1U

    #define HFEDeg 129U
    #define HFEDegI 7U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==133)
    #define K 192U

    #define HFEn 402U
    #define HFEv 18U
    #define HFEDELTA 18U

    #define NB_ITE 1U

    #define HFEDeg 130U
    #define HFEDegI 7U
    #define HFEDegJ 1U

    #define HFEs 3U
#elif (TEST_LIBRARY==134)
    #define K 192U

    #define HFEn 402U
    #define HFEv 18U
    #define HFEDELTA 18U

    #define NB_ITE 1U

    #define HFEDeg 640U
    #define HFEDegI 9U
    #define HFEDegJ 7U

    #define HFEs 0U
#elif (TEST_LIBRARY==135)
    #define K 192U

    #define HFEn 402U
    #define HFEv 18U
    #define HFEDELTA 18U

    #define NB_ITE 1U

    #define HFEDeg 640U
    #define HFEDegI 9U
    #define HFEDegJ 7U

    #define HFEs 3U
#elif (TEST_LIBRARY==136)
    #define K 192U

    #define HFEn 308U
    #define HFEv 22U
    #define HFEDELTA 20U

    #define NB_ITE 2U

    #define HFEDeg 129U
    #define HFEDegI 7U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==137)
    #define K 192U

    #define HFEn 308U
    #define HFEv 22U
    #define HFEDELTA 20U

    #define NB_ITE 2U

    #define HFEDeg 130U
    #define HFEDegI 7U
    #define HFEDegJ 1U

    #define HFEs 3U
#elif (TEST_LIBRARY==138)
    #define K 192U

    #define HFEn 278U
    #define HFEv 23U
    #define HFEDELTA 22U

    #define NB_ITE 3U

    #define HFEDeg 129U
    #define HFEDegI 7U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==139)
    #define K 192U

    #define HFEn 278U
    #define HFEv 23U
    #define HFEDELTA 22U

    #define NB_ITE 3U

    #define HFEDeg 130U
    #define HFEDegI 7U
    #define HFEDegJ 1U

    #define HFEs 3U
#elif (TEST_LIBRARY==140)
    #define K 192U

    #define HFEn 265U
    #define HFEv 23U
    #define HFEDELTA 22U

    #define NB_ITE 4U

    #define HFEDeg 129U
    #define HFEDegI 7U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==141)
    #define K 192U

    #define HFEn 265U
    #define HFEv 23U
    #define HFEDELTA 22U

    #define NB_ITE 4U

    #define HFEDeg 130U
    #define HFEDegI 7U
    #define HFEDegJ 1U

    #define HFEs 3U
#elif (TEST_LIBRARY==142)
    #define K 192U

    #define HFEn 399U
    #define HFEv 18U
    #define HFEDELTA 15U

    #define NB_ITE 1U

    #define HFEDeg 513U
    #define HFEDegI 9U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==143)
    #define K 192U

    #define HFEn 399U
    #define HFEv 18U
    #define HFEDELTA 15U

    #define NB_ITE 1U

    #define HFEDeg 514U
    #define HFEDegI 9U
    #define HFEDegJ 1U

    #define HFEs 3U
#elif (TEST_LIBRARY==144)
    #define K 192U

    #define HFEn 308U
    #define HFEv 19U
    #define HFEDELTA 20U

    #define NB_ITE 2U

    #define HFEDeg 513U
    #define HFEDegI 9U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==145)
    #define K 192U

    #define HFEn 308U
    #define HFEv 19U
    #define HFEDELTA 20U

    #define NB_ITE 2U

    #define HFEDeg 514U
    #define HFEDegI 9U
    #define HFEDegJ 1U

    #define HFEs 3U
#elif (TEST_LIBRARY==146)
    #define K 192U

    #define HFEn 276U
    #define HFEv 22U
    #define HFEDELTA 20U

    #define NB_ITE 3U

    #define HFEDeg 513U
    #define HFEDegI 9U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==147)
    #define K 192U

    #define HFEn 276U
    #define HFEv 22U
    #define HFEDELTA 20U

    #define NB_ITE 3U

    #define HFEDeg 514U
    #define HFEDegI 9U
    #define HFEDegJ 1U

    #define HFEs 3U
#elif (TEST_LIBRARY==148)
    #define K 192U

    #define HFEn 265U
    #define HFEv 20U
    #define HFEDELTA 22U

    #define NB_ITE 4U

    #define HFEDeg 513U
    #define HFEDegI 9U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==149)
    #define K 192U

    #define HFEn 265U
    #define HFEv 20U
    #define HFEDELTA 22U

    #define NB_ITE 4U

    #define HFEDeg 514U
    #define HFEDegI 9U
    #define HFEDegJ 1U

    #define HFEs 3U
#elif (TEST_LIBRARY==150)
    #define K 256U

    #define HFEn 540U
    #define HFEv 29U
    #define HFEDELTA 28U

    #define NB_ITE 1U

    #define HFEDeg 17U
    #define HFEDegI 4U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==151)
    #define K 256U

    #define HFEn 415U
    #define HFEv 32U
    #define HFEDELTA 31U

    #define NB_ITE 2U

    #define HFEDeg 17U
    #define HFEDegI 4U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==152)
    #define K 256U

    #define HFEn 375U
    #define HFEv 33U
    #define HFEDELTA 33U

    #define NB_ITE 3U

    #define HFEDeg 17U
    #define HFEDegI 4U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==153)
    #define K 256U

    #define HFEn 358U
    #define HFEv 35U
    #define HFEDELTA 34U

    #define NB_ITE 4U

    #define HFEDeg 17U
    #define HFEDegI 4U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==154)
    #define K 256U

    #define HFEn 540U
    #define HFEv 29U
    #define HFEDELTA 28U

    #define NB_ITE 1U

    #define HFEDeg 33U
    #define HFEDegI 5U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==155)
    #define K 256U

    #define HFEn 415U
    #define HFEv 32U
    #define HFEDELTA 31U

    #define NB_ITE 2U

    #define HFEDeg 33U
    #define HFEDegI 5U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==156)
    #define K 256U

    #define HFEn 375U
    #define HFEv 33U
    #define HFEDELTA 33U

    #define NB_ITE 3U

    #define HFEDeg 33U
    #define HFEDegI 5U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==157)
    #define K 256U

    #define HFEn 358U
    #define HFEv 35U
    #define HFEDELTA 34U

    #define NB_ITE 4U

    #define HFEDeg 33U
    #define HFEDegI 5U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==158)
    #define K 256U

    #define HFEn 540U
    #define HFEv 26U
    #define HFEDELTA 28U

    #define NB_ITE 1U

    #define HFEDeg 129U
    #define HFEDegI 7U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==159)
    #define K 256U

    #define HFEn 540U
    #define HFEv 26U
    #define HFEDELTA 28U

    #define NB_ITE 1U

    #define HFEDeg 130U
    #define HFEDegI 7U
    #define HFEDegJ 1U

    #define HFEs 3U
#elif (TEST_LIBRARY==160)
    #define K 256U

    #define HFEn 414U
    #define HFEv 30U
    #define HFEDELTA 30U

    #define NB_ITE 2U

    #define HFEDeg 129U
    #define HFEDegI 7U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==161)
    #define K 256U

    #define HFEn 414U
    #define HFEv 30U
    #define HFEDELTA 30U

    #define NB_ITE 2U

    #define HFEDeg 130U
    #define HFEDegI 7U
    #define HFEDegJ 1U

    #define HFEs 3U
#elif (TEST_LIBRARY==162)
    #define K 256U

    #define HFEn 372U
    #define HFEv 33U
    #define HFEDELTA 30U

    #define NB_ITE 3U

    #define HFEDeg 129U
    #define HFEDegI 7U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==163)
    #define K 256U

    #define HFEn 372U
    #define HFEv 33U
    #define HFEDELTA 30U

    #define NB_ITE 3U

    #define HFEDeg 130U
    #define HFEDegI 7U
    #define HFEDegJ 1U

    #define HFEs 3U
#elif (TEST_LIBRARY==164)
    #define K 256U

    #define HFEn 358U
    #define HFEv 32U
    #define HFEDELTA 34U

    #define NB_ITE 4U

    #define HFEDeg 129U
    #define HFEDegI 7U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==165)
    #define K 256U

    #define HFEn 358U
    #define HFEv 32U
    #define HFEDELTA 34U

    #define NB_ITE 4U

    #define HFEDeg 130U
    #define HFEDegI 7U
    #define HFEDegJ 1U

    #define HFEs 3U
#elif (TEST_LIBRARY==166)
    #define K 256U

    #define HFEn 537U
    #define HFEv 26U
    #define HFEDELTA 25U

    #define NB_ITE 1U

    #define HFEDeg 513U
    #define HFEDegI 9U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==167)
    #define K 256U

    #define HFEn 537U
    #define HFEv 26U
    #define HFEDELTA 25U

    #define NB_ITE 1U

    #define HFEDeg 514U
    #define HFEDegI 9U
    #define HFEDegJ 1U

    #define HFEs 3U
#elif (TEST_LIBRARY==168)
    #define K 256U

    #define HFEn 537U
    #define HFEv 26U
    #define HFEDELTA 25U

    #define NB_ITE 1U

    #define HFEDeg 1152U
    #define HFEDegI 10U
    #define HFEDegJ 7U

    #define HFEs 0U
#elif (TEST_LIBRARY==169)
    #define K 256U

    #define HFEn 537U
    #define HFEv 26U
    #define HFEDELTA 25U

    #define NB_ITE 1U

    #define HFEDeg 1152U
    #define HFEDegI 10U
    #define HFEDegJ 7U

    #define HFEs 3U
#elif (TEST_LIBRARY==170)
    #define K 256U

    #define HFEn 414U
    #define HFEv 27U
    #define HFEDELTA 30U

    #define NB_ITE 2U

    #define HFEDeg 513U
    #define HFEDegI 9U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==171)
    #define K 256U

    #define HFEn 414U
    #define HFEv 27U
    #define HFEDELTA 30U

    #define NB_ITE 2U

    #define HFEDeg 514U
    #define HFEDegI 9U
    #define HFEDegJ 1U

    #define HFEs 3U
#elif (TEST_LIBRARY==172)
    #define K 256U

    #define HFEn 372U
    #define HFEv 30U
    #define HFEDELTA 30U

    #define NB_ITE 3U

    #define HFEDeg 513U
    #define HFEDegI 9U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==173)
    #define K 256U

    #define HFEn 372U
    #define HFEv 30U
    #define HFEDELTA 30U

    #define NB_ITE 3U

    #define HFEDeg 514U
    #define HFEDegI 9U
    #define HFEDegJ 1U

    #define HFEs 3U
#elif (TEST_LIBRARY==174)
    #define K 256U

    #define HFEn 354U
    #define HFEv 33U
    #define HFEDELTA 30U

    #define NB_ITE 4U

    #define HFEDeg 513U
    #define HFEDegI 9U
    #define HFEDegJ 0U

    #define HFEs 0U
#elif (TEST_LIBRARY==175)
    #define K 256U

    #define HFEn 354U
    #define HFEv 33U
    #define HFEDELTA 30U

    #define NB_ITE 4U

    #define HFEDeg 514U
    #define HFEDegI 9U
    #define HFEDegJ 1U

    #define HFEs 3U
#else
    #error "The value of TEST_LIBRARY is not implemented."
#endif


#else
    #error "Parameters must be chosen."
#endif



/****************** VERIFICATION OF THE PARAMETERS ******************/

#if (!HFEn)
    #error "HFEn must be different from zero"
#endif

#if (!HFEm)
    #error "HFEm must be different from zero"
#endif

#if (HFEm>HFEn)
    #error "HFEm>HFEn is not possible for this implementation"
#endif

#if (HFEm>(K<<1))
    #error "HFEm>2K is not possible for this implementation"
#endif

#if (!NB_ITE)
    #error "NB_ITE must be different from zero"
#endif


#if (!HFEDeg)
    #error "HFEDeg must be different from zero"
#endif

#if (HFEDeg==1)
    #if(HFEDegI||HFEDegJ)
        #error "For HFEDeg==1, put HFEDegI==HFEDegJ==0"
    #endif
#else
    #if (HFEDeg!=((1U<<HFEDegI)+(1U<<HFEDegJ)))
        #error "HFEDeg!=2^(HFEDegI) + 2^(HFEDegJ)"
    #endif
#endif

#if (HFEDegI<HFEDegJ)
    #error "Requirement: HFEDegI>=HFEDegJ"
#endif

/* X^(2^i + 2^j) >= X^(2^n) */
#if ((HFEDegI>=HFEn)||((HFEDegI==(HFEn-1))&&(HFEDegI==HFEDegJ)))
    #error "X^(2^n)==X so the degree of HFE polynomial is biased"
#endif




#endif

