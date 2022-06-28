#ifndef _CONFIG_HFE_H
#define _CONFIG_HFE_H

#include "arch.h"
#include "bit.h"
#include "types_HFE.h"
#include "KAT_int.h"



/* The verification don't need constant time evaluation */
#define verifHFE evaluateMQSnoconst
/* The public and secret keys are the same for encryption and signature */
#define sign_keypairHFE encrypt_keypairHFE


/****************** C++ compatibility ******************/

#ifdef __cplusplus
    #define BEGIN_EXTERNC extern "C" {
    #define END_EXTERNC }
#else
    #define BEGIN_EXTERNC
    #define END_EXTERNC
#endif


/****************** PARAMETERS FOR HFE ******************/


/* Level of security of hash functions */
#define K 256U

/* This parameter is necessarily 2 */
/* GF(q) */
#define HFEq 2U

/* Degree of the extension GF(2^n) */
#define HFEn 354U

#if (!HFEn)
    #error "HFEn must be different of zero"
#endif

/* Number of vinegar variable */
#define HFEv 33U
/* Number of variable of the public key */
#define HFEnv (HFEn+HFEv)

/* Number of equations */
#define HFEm 324U

#if (!HFEm)
    #error "HFEm must be different of zero"
#endif

#if (HFEm>HFEn)
    #error "HFEm>HFEn is not possible for this implementation"
#endif

#if (HFEm>(K<<1))
    #error "HFEm>2K is not possible for this implementation"
#endif

/* Number of iterations of the signature */
#define NB_ITE 4

/* Degree of HFE polynom : Deg = 2^DegI + 2^DegJ 
 * or Deg = 2^0 (with DegI==DegJ==0) */
#define HFEDeg 512U
#define HFEDegI 8U
/* Requirement : DegI>=DegJ */
#define HFEDegJ 8U


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
    #error "Requirement : HFEDegI>=HFEDegJ"
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

/* Public key (words) */
#define SIZE_PK MQ_GFqm_SIZE
/* Public key (bytes) */
#define SIZE_PK_BYTES (SIZE_PK<<3)

/* Secret key: HFE polynomial + matrix S_inv + matrix T_inv */

/* Secret key (words) */
#define SIZE_SK (NB_UINT_HFEPOLY+MATRIXnv_SIZE+MATRIXn_SIZE)
/* Secret key (bytes) */
#define SIZE_SK_BYTES (SIZE_SK<<3)


/* Signature: NB_WORD_GFqnv for signature NB_ITE, NB_WORD_GFqnvm for others */

/* Size of signature (words): */
#define SIZE_SIGNATURE (NB_WORD_GFqnv+(NB_ITE-1)*NB_WORD_GFqnvm)
/* Size of signature (bytes): */
#define SIZE_SIGNATURE_BYTES (SIZE_SIGNATURE<<3)


/* XXX Useless for signature XXX */

/* Plaintext (words) */
#define SIZE_PLAIN NB_WORD_GFqnv
/* Plaintext (bytes) */
#define SIZE_PLAIN_BYTES (SIZE_PLAIN<<3)

/* Ciphertext: encrypted plaintext + SHA3 of plaintext */

/* In the ciphertext, the NB_BITS_HASH_PLAIN of SHA3(plaintext) are taken */
/* Heuristic: 2*log_2((2^(HFEv)) * HFEDeg) */
#define NB_BITS_HASH_PLAIN ((HFEv+HFEDegI+1)<<1)

#if (NB_BITS_HASH_PLAIN>(SIZE_DIGEST<<3))
    #error "The length of the hash is too short"
#endif

#define HASH_PLAIN_QUO (NB_BITS_HASH_PLAIN/NB_BITS_UINT)
#define HASH_PLAIN_REM (NB_BITS_HASH_PLAIN%NB_BITS_UINT)
#if HASH_PLAIN_REM
    #define NB_WORD_HASH_PLAIN (HASH_PLAIN_QUO+1)
#else
    #define NB_WORD_HASH_PLAIN HASH_PLAIN_QUO
#endif
#define HFE_MASK_PH mask64(HASH_PLAIN_REM)

/* Size of cipher (words): */
#define SIZE_CIPHER (NB_WORD_GFqm+NB_WORD_HASH_PLAIN)
/* Size of cipher (bytes): */
#define SIZE_CIPHER_BYTES (SIZE_CIPHER<<3)


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

/* Size of a MQ polynom with coefficients in GF(2^m) */
#define MQ_GFqm_SIZE (NB_MONOM_PK*NB_WORD_GFqm)

#define MALLOC_MQSm (UINT*)malloc(MQ_GFqm_SIZE*sizeof(UINT))
#define CALLOC_MQSm (UINT*)calloc(MQ_GFqm_SIZE,sizeof(UINT))

/* Number of UINT of matrix m*m in GF(2) */
#define MATRIXm_SIZE (HFEm*NB_WORD_GFqm)

/* Number of bytes that an element of GF(2^m) needs */
#define NB_BYTES_GFqm ((HFEm>>3)+((HFEm&7)?1:0))


#if ((HFEn==HFEm)&&(!HFEv))
    /* This case is never used */
    #define NB_WORD_GFqnvm 0U
#else
    #define NB_WORD_GFqnvm (NB_WORD_GFqnv-NB_WORD_GFqm+(HFEmr?1:0))
#endif

#define HFE_MASKnvm (~HFE_MASKm)


/****************** MACRO FOR HFE POLYNOMIAL ******************/


/* Number of coefficients of HFE polynomial */
#if (HFEq==2)
    #if (HFEDeg==1)
        /* when the vinegars variables are evaluated */
        #define NB_COEFS_HFEPOLY_EVAL 1U
        /* do not store the leading term == only constant coefficient */
        #define NB_COEFS_HFEPOLY NB_MONOM_VINEGAR
    #else
        /* when the vinegars variables are evaluated */
        /* set i=HFEDegI and j=HFEDegJ, the number of no zero monom is:
           1 (constant) + 1 (X^1) + i(i+1)/2 (X^2 to X^(2*2^(i-1)) + 
  (j+1) (X^(2^i + 2^0) to X^(2^i + 2^j)) -1 (leading term == 1 is not stored) */
        #define NB_COEFS_HFEPOLY_EVAL (2+HFEDegJ+((HFEDegI*(HFEDegI+1))>>1))
        #define NB_COEFS_HFEPOLY (NB_COEFS_HFEPOLY_EVAL+(NB_MONOM_VINEGAR-1)+\
                                  (HFEDegI+1)*HFEv)
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
#endif


#endif
#endif
