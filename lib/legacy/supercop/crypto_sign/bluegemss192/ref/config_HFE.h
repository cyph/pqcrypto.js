#ifndef _CONFIG_HFE_H
#define _CONFIG_HFE_H

#include "arch.h"
#include "bit.h"
#include "types_HFE.h"
#include "KAT_int.h"
#include <stdlib.h>


/* Level of security of hash functions */
#define K 192U

/** Choice of the current cryptosystem. */
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



/* The verification don't need constant time evaluation */
#define verifHFE evaluateMQSnoconst
/* The public and secret keys are the same for encryption and signature */
#define encrypt_keypairHFE sign_keypairHFE

/* To use a right multiplication by T */
#define RIGHT_MULTIPLICATION_BY_T 1


/****************** C++ compatibility ******************/

#ifdef __cplusplus
    #define BEGIN_EXTERNC extern "C" {
    #define END_EXTERNC }
#else
    #define BEGIN_EXTERNC
    #define END_EXTERNC
#endif


/****************** PARAMETERS FOR HFE ******************/



/* This parameter is necessarily 2 */
/* GF(q) */
#define HFEq 2U

/* Number of variables of the public key */
#define HFEnv (HFEn+HFEv)

/* Number of equations of the public key */
#define HFEm (HFEn-HFEDELTA)






#ifdef GeMSS

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


#else
    /* To test an other choice of parameters */
    /* The user can modify these values to test the library */

    /* Degree of the extension GF(2^n) */
    #define HFEn 174U
    /* Number of vinegar variable */
    #define HFEv 12U
    /* Number of removed equations (the minus) */
    #define HFEDELTA 12U

    /* Number of iterations of the signature */
    #define NB_ITE 4

    /* Degree of HFE polynomial: Deg = 2^DegI + 2^DegJ 
     * or Deg = 2^0 (with DegI==DegJ==0) */
    #define HFEDeg 513U
    #define HFEDegI 9U
    /* Requirement : DegI>=DegJ */
    #define HFEDegJ 0U

    /* Number of removed odd degree terms in the HFE polynomial */
    #define HFEs 0U
#endif


/****************** VERIFICATION OF THE PARAMETERS ******************/

#if (!HFEn)
    #error "HFEn must be different of zero"
#endif

#if (!HFEm)
    #error "HFEm must be different of zero"
#endif

#if (HFEm>HFEn)
    #error "HFEm>HFEn is not possible for this implementation"
#endif

#if (HFEm>(K<<1))
    #error "HFEm>2K is not possible for this implementation"
#endif


#if (!HFEDeg)
    #error "HFEDeg must be different of zero"
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

/****************** MACRO USEFUL FOR HASH ******************/

#if (K<=128)
    #define HASH SHA3_256
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
        #define HASH SHA3_384
        #define SIZE_DIGEST 48
        #define SIZE_DIGEST_UINT 6
        #define SIZE_2_DIGEST 96
        #define EQUALHASH ISEQUAL384
        #define COPYHASH COPY384
        #define SPONGE(input,inputByteLen,output,outputByteLen) \
            KeccakWidth1600_Sponge(832, 768, input, inputByteLen, \
                                   0x06, output, outputByteLen)
    #else
        #define HASH SHA3_512
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

/****************** MACRO USEFUL FOR SIZE ******************/


/* Optimization of the representation of the public key when m%64 is small */
#if (HFEm==324)
    #define HYBRID_REPRESENTATION_PK 1
#else
    #define HYBRID_REPRESENTATION_PK 0
#endif


/* Public key (words) */
#if HYBRID_REPRESENTATION_PK
    #define SIZE_PK NB_WORD_HYBRID_EQUATIONS
#else
    #define SIZE_PK MQ_GFqm_SIZE
#endif
/* Public key (bytes) */
#define SIZE_PK_BYTES (SIZE_PK<<3)

/* Secret key: HFE polynomial + matrix S_inv + matrix T_inv */

/* Secret key (words) */
#define SIZE_SK (NB_UINT_HFEPOLY+MATRIXnv_SIZE+MATRIXn_SIZE)
/* Secret key (bytes) */
#define SIZE_SK_BYTES (SIZE_SK<<3)


/* Signature: NB_WORD_GFqnv for signature NB_ITE, NB_WORD_GFqnvm for others */

/* Size of signature (words): */
#define SIZE_SIGN_UNCOMPRESSED (NB_WORD_GFqnv+(NB_ITE-1)*NB_WORD_GFqnvm)

#define SIZE_SIGN_THEORETICAL_HFE (HFEnv+(NB_ITE-1)*(HFEnv-HFEm))

/* Size of signature (bytes): */
/* (x+7)/8 = Ceiling(x/8) */ 
#define SIZE_SIGN_HFE ((SIZE_SIGN_THEORETICAL_HFE+7)>>3)



/****************** MACRO USEFUL FOR HFE ******************/


/* n = NB_BITS_UINT*quo + rem */
#define HFEnq (HFEn/NB_BITS_UINT)
#define HFEnr (HFEn%NB_BITS_UINT)
/* The number of word that an element of GF(2^n) needs */
#if (HFEnr)
    #define NB_WORD_GFqn (HFEnq+1)
#else
    #define NB_WORD_GFqn HFEnq
#endif

/* Mask for arithmetic in GF(2^n) */
#define HFE_MASKn mask64(HFEnr)

#define HFEnr8 (HFEn&7)
#define HFE_MASKn8 ((1U<<HFEnr8)-1)
/* Number of bytes that an element of GF(2^n) needs */
#define NB_BYTES_GFqn ((HFEn>>3)+((HFEnr8)?1:0))


/* v = NB_BITS_UINT*quo + rem */
#define HFEvq (HFEv/NB_BITS_UINT)
#define HFEvr (HFEv%NB_BITS_UINT)
/* The number of word that an element of GF(2^v) needs */
#if (HFEvr)
    #define NB_WORD_GFqv (HFEvq+1)
#else
    #define NB_WORD_GFqv HFEvq
#endif

/* Mask for arithmetic in GF(2^v) */
#define HFE_MASKv mask64(HFEvr)

#define HFEvr8 (HFEv&7)
/* Number of bytes that an element of GF(2^(n+v)) needs */
#define NB_BYTES_GFqv ((HFEv>>3)+((HFEvr8)?1:0))


/* n+v = NB_BITS_UINT*quo + rem */
#define HFEnvq (HFEnv/NB_BITS_UINT)
#define HFEnvr (HFEnv%NB_BITS_UINT)
/* The number of word that an element of GF(2^(n+v)) needs */
#if (HFEnvr)
    #define NB_WORD_GFqnv (HFEnvq+1)
#else
    #define NB_WORD_GFqnv HFEnvq
#endif

/* Mask for arithmetic in GF(2^(n+v)) */
#define HFE_MASKnv mask64(HFEnvr)

#define HFEnvr8 (HFEnv&7)
#define HFE_MASKnv8 ((1U<<HFEnvr8)-1)
/* Number of bytes that an element of GF(2^(n+v)) needs */
#define NB_BYTES_GFqnv ((HFEnv>>3)+((HFEnvr8)?1:0))



/* Number of monom */
#if (HFEq==2)
    #define NB_MONOM (((HFEn*(HFEn+1))>>1)+1)
    #define NB_MONOM_VINEGAR (((HFEv*(HFEv+1))>>1)+1)
    #define NB_MONOM_PK (((HFEnv*(HFEnv+1))>>1)+1)
#else
    #define NB_MONOM (((HFEn*(HFEn+3))>>1)+1)
    #define NB_MONOM_VINEGAR (((HFEv*(HFEv+3))>>1)+1)
    #define NB_MONOM_PK (((HFEnv*(HFEnv+3))>>1)+1)
#endif


/* NB_MONOM_PK = NB_BITS_UINT*quo + rem */
#define HFENq (NB_MONOM_PK/NB_BITS_UINT)
#define HFENr (NB_MONOM_PK%NB_BITS_UINT)
#if (HFENr)
    #define NB_WORD_EQUATION (HFENq+1)
#else
    #define NB_WORD_EQUATION HFENq
#endif
#define NB_WORD_mEQUATIONS (NB_WORD_EQUATION*HFEm)


/* Size of a MQ polynom with n+v variables and with coefficients in GF(2^n) */
#define MQnv_GFqn_SIZE (NB_MONOM_PK*NB_WORD_GFqn)
/* Size of a MQ polynom with v variables and with coefficients in GF(2^n) */
#define MQv_GFqn_SIZE (NB_MONOM_VINEGAR*NB_WORD_GFqn)
/* Size of a multivariate linear polynomial with v variables and with 
 * coefficients in GF(2^n) */
#define MLv_GFqn_SIZE ((HFEv+1)*NB_WORD_GFqn)

#define MALLOC_MQSn (UINT*)malloc(MQnv_GFqn_SIZE*sizeof(UINT))
#define CALLOC_MQSn (UINT*)calloc(MQnv_GFqn_SIZE,sizeof(UINT))


/* Size for an uncompressed equation */
#define NB_WORD_ONE_EQUATION \
    ((((HFEnvq*(HFEnvq+1))>>1)*NB_BITS_UINT)+(HFEnvq+1)*HFEnvr)

/* Remove the last word of each coefficient in GF(2^m), \
   excepted for the constant */
#define ACCESS_last_equations ((NB_MONOM_PK-1)*HFEmq+NB_WORD_GFqm)
/* It is padded to avoid to load data outside of memory 
   during the public key evaluation */
/* XXX We remove the padding here XXX */
#define NB_WORD_HYBRID_EQUATIONS (ACCESS_last_equations+\
            NB_WORD_ONE_EQUATION*HFEmr/*+((4-(NB_WORD_GFqnv&3))&3)*/)


/* Number of UINT of matrix n*n in GF(2) */
#define MATRIXn_SIZE (HFEn*NB_WORD_GFqn)
/* Number of UINT of matrix (n+v)*(n+v) in GF(2) */
#define MATRIXnv_SIZE (HFEnv*NB_WORD_GFqnv)

#define MALLOC_MATRIXn (UINT*)malloc(MATRIXn_SIZE*sizeof(UINT));
#define CALLOC_MATRIXn (UINT*)calloc(MATRIXn_SIZE,sizeof(UINT));

#define MALLOC_MATRIXnv (UINT*)malloc(MATRIXnv_SIZE*sizeof(UINT));
#define CALLOC_MATRIXnv (UINT*)calloc(MATRIXnv_SIZE,sizeof(UINT));


/* Malloc lower triangular matrix n*n (in words) */
#define LTRIANGULAR_N_SIZE (((HFEnq*(HFEnq+1))>>1)*NB_BITS_UINT+\
                            NB_WORD_GFqn*HFEnr)
/* Malloc lower triangular matrix (n+v)*(n+v) (in words) */
#define LTRIANGULAR_NV_SIZE (((HFEnvq*(HFEnvq+1))>>1)*NB_BITS_UINT+\
                            NB_WORD_GFqnv*HFEnvr)



/* m = NB_BITS_UINT*quo + rem */
#define HFEmq (HFEm/NB_BITS_UINT)
#define HFEmr (HFEm%NB_BITS_UINT)
/* The number of word that an element of GF(2^m) needs */
#if (HFEmr)
    #define NB_WORD_GFqm (HFEmq+1)
#else
    #define NB_WORD_GFqm HFEmq
#endif

/* Mask to truncate the last word */
#define HFE_MASKm mask64(HFEmr)

#define HFEmq8 (HFEm>>3)
#define HFEmr8 (HFEm&7U)

/* Number of bytes that an element of GF(2^m) needs */
#define NB_BYTES_GFqm (HFEmq8+((HFEmr8)?1:0))

/* Size of a MQ polynom with coefficients in GF(2^m) */
#define MQ_GFqm_SIZE (NB_MONOM_PK*NB_WORD_GFqm)

#define MALLOC_MQSm (UINT*)malloc(MQ_GFqm_SIZE*sizeof(UINT))
#define CALLOC_MQSm (UINT*)calloc(MQ_GFqm_SIZE,sizeof(UINT))

/* Number of UINT of matrix m*m in GF(2) */
#define MATRIXm_SIZE (HFEm*NB_WORD_GFqm)


#if ((HFEn==HFEm)&&(!HFEv))
    /* This case is never used */
    #define NB_WORD_GFqnvm 0U
#else
    #define NB_WORD_GFqnvm (NB_WORD_GFqnv-NB_WORD_GFqm+(HFEmr?1:0))
#endif

#define HFE_MASKnvm (~HFE_MASKm)


/****************** MACRO FOR HFE POLYNOMIAL ******************/

#if HFEs
    /* Set to 1 to remove terms which have an odd degree strictly greater than \
       HFE_odd_degree */
    #define ENABLED_REMOVE_ODD_DEGREE 1
#else
    #define ENABLED_REMOVE_ODD_DEGREE 0
#endif

#if ENABLED_REMOVE_ODD_DEGREE
    #define LOG_odd_degree (HFEDegI-HFEs)
    /* HFE_odd_degree = 1 + 2^LOG_odd_degree */
    #define HFE_odd_degree ((1U<<(LOG_odd_degree))+1)

    #if (HFEDeg&1)
        #error "HFEDeg is odd, so to remove the leading term would decrease \
                the degree."
    #endif

    #if (HFE_odd_degree>HFEDeg)
        #error "It is useless to remove 0 term."
    #endif

    #if (HFE_odd_degree<=1)
        #error "The case where the term X^3 is removing is not implemented."
    #endif
#endif





/* Number of coefficients of HFE polynomial */
#if (HFEDeg==1)
    /* when the vinegars variables are evaluated */
    #define NB_COEFS_HFEPOLY_EVAL 1U
    /* do not store the leading term == only constant coefficient */
    #define NB_COEFS_HFEPOLY NB_MONOM_VINEGAR

#else
    #if (HFEq==2)
        /* when the vinegars variables are evaluated */
        /* set i=HFEDegI and j=HFEDegJ, the number of no zero monom is:
           1 (constant) + 1 (X^1) + i(i+1)/2 (X^2 to X^(2*2^(i-1)) + 
  (j+1) (X^(2^i + 2^0) to X^(2^i + 2^j)) -1 (leading term == 1 is not stored) */

        #if ENABLED_REMOVE_ODD_DEGREE
            #define NB_COEFS_HFEPOLY_EVAL (2+HFEDegJ+((HFEDegI*(HFEDegI-1))>>1)+LOG_odd_degree)
        #else
            #define NB_COEFS_HFEPOLY_EVAL (2+HFEDegJ+((HFEDegI*(HFEDegI+1))>>1))
        #endif

        #define NB_COEFS_HFEPOLY (NB_COEFS_HFEPOLY_EVAL+(NB_MONOM_VINEGAR-1)+\
                                  (HFEDegI+1)*HFEv)

    #else
        /* Not implemented */
    #endif
#endif

#define NB_UINT_HFEPOLY (NB_COEFS_HFEPOLY*NB_WORD_GFqn)
#define NB_UINT_HFEPOLY_EVAL (NB_COEFS_HFEPOLY_EVAL*NB_WORD_GFqn)

#define MALLOC_HFEPOLY_EVAL (UINT*)malloc(NB_UINT_HFEPOLY_EVAL*sizeof(UINT));


/****************** ARITHMETIC FUNCTIONS FOR HFE ******************/

/* Always true because the implementation only support 64 bits */
#if (NB_BITS_UINT==64U)

#if (NB_WORD_GFqn == 1)
    #define HFEADD ADD64_TAB
    #define HFEADD2 ADD64_TAB2
    #define HFECOPY COPY64
    #define HFEPERMUTATION PERMUTATION64_TAB
    #define HFESET0 SET0_64
    #define HFECMP_LT CMP_LT64
    #define HFECMP_GT CMP_GT64
    #define HFEDOTPRODUCT DOTPRODUCT64
    #define HFEDOTPRODUCTN DOTPRODUCT64M

#elif (NB_WORD_GFqn == 2)
    #define HFEADD ADD128
    #define HFEADD2 ADD128_2
    #define HFECOPY COPY128
    #define HFEPERMUTATION PERMUTATION128
    #define HFESET0 SET0_128
    #define HFECMP_LT CMP_LT128
    #define HFECMP_GT CMP_GT128
    #define HFEDOTPRODUCT DOTPRODUCT128
    #define HFEDOTPRODUCTN DOTPRODUCT128M

#elif (NB_WORD_GFqn == 3)
    #define HFEADD ADD192
    #define HFEADD2 ADD192_2
    #define HFECOPY COPY192
    #define HFEPERMUTATION PERMUTATION192
    #define HFESET0 SET0_192
    #define HFECMP_LT CMP_LT192
    #define HFECMP_GT CMP_GT192
    #define HFEDOTPRODUCT DOTPRODUCT192
    #define HFEDOTPRODUCTN DOTPRODUCT192M

#elif (NB_WORD_GFqn == 4)
    #define HFEADD ADD256
    #define HFEADD2 ADD256_2
    #define HFECOPY COPY256
    #define HFEPERMUTATION PERMUTATION256
    #define HFESET0 SET0_256
    #define HFECMP_LT CMP_LT256
    #define HFECMP_GT CMP_GT256
    #define HFEDOTPRODUCT DOTPRODUCT256
    #define HFEDOTPRODUCTN DOTPRODUCT256M

#elif (NB_WORD_GFqn == 5)
    #define HFEADD ADD320
    #define HFEADD2 ADD320_2
    #define HFECOPY COPY320
    #define HFEPERMUTATION PERMUTATION320
    #define HFESET0 SET0_320
    #define HFECMP_LT CMP_LT320
    #define HFECMP_GT CMP_GT320
    #define HFEDOTPRODUCT DOTPRODUCT320
    #define HFEDOTPRODUCTN DOTPRODUCT320M

#elif (NB_WORD_GFqn == 6)
    #define HFEADD ADD384
    #define HFEADD2 ADD384_2
    #define HFECOPY COPY384
    #define HFEPERMUTATION PERMUTATION384
    #define HFESET0 SET0_384
    #define HFECMP_LT CMP_LT384
    #define HFECMP_GT CMP_GT384
    #define HFEDOTPRODUCT DOTPRODUCT384
    #define HFEDOTPRODUCTN DOTPRODUCT384M

#elif (NB_WORD_GFqn == 7)
    #define HFEADD ADD448
    #define HFEADD2 ADD448_2
    #define HFECOPY COPY448
    #define HFEPERMUTATION PERMUTATION448
    #define HFESET0 SET0_448
    #define HFECMP_LT CMP_LT448
    #define HFECMP_GT CMP_GT448
    #define HFEDOTPRODUCT DOTPRODUCT448
    #define HFEDOTPRODUCTN DOTPRODUCT448M

#elif (NB_WORD_GFqn == 8)
    #define HFEADD ADD512
    #define HFEADD2 ADD512_2
    #define HFECOPY COPY512
    #define HFEPERMUTATION PERMUTATION512
    #define HFESET0 SET0_512
    #define HFECMP_LT CMP_LT512
    #define HFECMP_GT CMP_GT512
    #define HFEDOTPRODUCT DOTPRODUCT512
    #define HFEDOTPRODUCTN DOTPRODUCT512M

#elif (NB_WORD_GFqn == 9)
    #define HFEADD ADD576
    #define HFEADD2 ADD576_2
    #define HFECOPY COPY576
    #define HFEPERMUTATION PERMUTATION576
    #define HFESET0 SET0_576
    #define HFECMP_LT CMP_LT576
    #define HFECMP_GT CMP_GT576
    #define HFEDOTPRODUCT DOTPRODUCT576
    #define HFEDOTPRODUCTN DOTPRODUCT576M
#endif


#if (NB_WORD_GFqm == 1)
    #define HFEDOTPRODUCTM DOTPRODUCT64M
    #define HFECOPY_M COPY64
    #define HFEADDm ADD64_TAB
    #define HFEADD2m ADD64_TAB2
    #define HFEISEQUALm ISEQUAL64
#elif (NB_WORD_GFqm == 2)
    #define HFEDOTPRODUCTM DOTPRODUCT128M
    #define HFECOPY_M COPY128
    #define HFEADDm ADD128
    #define HFEADD2m ADD128_2
    #define HFEISEQUALm ISEQUAL128
#elif (NB_WORD_GFqm == 3)
    #define HFEDOTPRODUCTM DOTPRODUCT192M
    #define HFECOPY_M COPY192
    #define HFEADDm ADD192
    #define HFEADD2m ADD192_2
    #define HFEISEQUALm ISEQUAL192
#elif (NB_WORD_GFqm == 4)
    #define HFEDOTPRODUCTM DOTPRODUCT256M
    #define HFECOPY_M COPY256
    #define HFEADDm ADD256
    #define HFEADD2m ADD256_2
    #define HFEISEQUALm ISEQUAL256
#elif (NB_WORD_GFqm == 5)
    #define HFEDOTPRODUCTM DOTPRODUCT320M
    #define HFECOPY_M COPY320
    #define HFEADDm ADD320
    #define HFEADD2m ADD320_2
    #define HFEISEQUALm ISEQUAL320
#elif (NB_WORD_GFqm == 6)
    #define HFEDOTPRODUCTM DOTPRODUCT384M
    #define HFECOPY_M COPY384
    #define HFEADDm ADD384
    #define HFEADD2m ADD384_2
    #define HFEISEQUALm ISEQUAL384
#elif (NB_WORD_GFqm == 7)
    #define HFEDOTPRODUCTM DOTPRODUCT448M
    #define HFECOPY_M COPY448
    #define HFEADDm ADD448
    #define HFEADD2m ADD448_2
    #define HFEISEQUALm ISEQUAL448
#elif (NB_WORD_GFqm == 8)
    #define HFEDOTPRODUCTM DOTPRODUCT512M
    #define HFECOPY_M COPY512
    #define HFEADDm ADD512
    #define HFEADD2m ADD512_2
    #define HFEISEQUALm ISEQUAL512
#endif


#if (NB_WORD_GFqnv == 1)
    #define HFEDOTPRODUCTNV DOTPRODUCT64M
    #define HFESET0_NV SET0_64
    #define HFEPERMUTATIONNV PERMUTATION64_TAB
#elif (NB_WORD_GFqnv == 2)
    #define HFEDOTPRODUCTNV DOTPRODUCT128M
    #define HFESET0_NV SET0_128
    #define HFEPERMUTATIONNV PERMUTATION128
#elif (NB_WORD_GFqnv == 3)
    #define HFEDOTPRODUCTNV DOTPRODUCT192M
    #define HFESET0_NV SET0_192
    #define HFEPERMUTATIONNV PERMUTATION192
#elif (NB_WORD_GFqnv == 4)
    #define HFEDOTPRODUCTNV DOTPRODUCT256M
    #define HFESET0_NV SET0_256
    #define HFEPERMUTATIONNV PERMUTATION256
#elif (NB_WORD_GFqnv == 5)
    #define HFEDOTPRODUCTNV DOTPRODUCT320M
    #define HFESET0_NV SET0_320
    #define HFEPERMUTATIONNV PERMUTATION320
#elif (NB_WORD_GFqnv == 6)
    #define HFEDOTPRODUCTNV DOTPRODUCT384M
    #define HFESET0_NV SET0_384
    #define HFEPERMUTATIONNV PERMUTATION384
#elif (NB_WORD_GFqnv == 7)
    #define HFEDOTPRODUCTNV DOTPRODUCT448M
    #define HFESET0_NV SET0_448
    #define HFEPERMUTATIONNV PERMUTATION448
#elif (NB_WORD_GFqnv == 8)
    #define HFEDOTPRODUCTNV DOTPRODUCT512M
    #define HFESET0_NV SET0_512
    #define HFEPERMUTATIONNV PERMUTATION512
#elif (NB_WORD_GFqnv == 9)
    #define HFEDOTPRODUCTNV DOTPRODUCT576M
    #define HFESET0_NV SET0_576
    #define HFEPERMUTATIONNV PERMUTATION576
#endif


#if (NB_WORD_GFqnvm == 1)
    #define HFECOPYnvm COPY64
#elif (NB_WORD_GFqnvm == 2)
    #define HFECOPYnvm COPY128
    #define HFECOPYnvm1 COPY64
#elif (NB_WORD_GFqnvm == 3)
    #define HFECOPYnvm COPY192
    #define HFECOPYnvm1 COPY128
#elif (NB_WORD_GFqnvm == 4)
    #define HFECOPYnvm COPY256
    #define HFECOPYnvm1 COPY192
#elif (NB_WORD_GFqnvm == 5)
    #define HFECOPYnvm COPY320
    #define HFECOPYnvm1 COPY256
#elif (NB_WORD_GFqnvm == 6)
    #define HFECOPYnvm COPY384
    #define HFECOPYnvm1 COPY320
#elif (NB_WORD_GFqnvm == 7)
    #define HFECOPYnvm COPY448
    #define HFECOPYnvm1 COPY384
#elif (NB_WORD_GFqnvm == 8)
    #define HFECOPYnvm COPY512
    #define HFECOPYnvm1 COPY448
#endif


#endif
#endif
