#ifndef _CONFIG_HFE_H
#define _CONFIG_HFE_H



#include "choice_crypto.h"
#include "parameters_HFE.h"
/* Optimization of the representation of the public key when m%64 is small */
#if (HFE&&InnerMode&&(HFEm==324))
    #define HYBRID_REPRESENTATION_PK 1
#else
    #define HYBRID_REPRESENTATION_PK 0
#endif


/* The verification does not need constant time evaluation */
#if HYBRID_REPRESENTATION_PK
    #define evalMQS_gf2_HFE evalMQShybrid_nocst_gf2_m
#else
    #define evalMQS_gf2_HFE evalMQSnocst_gf2
#endif
/* The public and secret keys are the same for encryption and signature */
#define encrypt_keypairHFE sign_keypairHFE

/* To use a right multiplication by T */
#define RIGHT_MULTIPLICATION_BY_T 1

/* To use an affine transformation: pk = (MQS)*T +t */
#define AFFINE_TRANSFORMATION_BY_t 0



/****************** PARAMETERS FOR GENERATION ******************/

/* XXX This part is disabled for the submission XXX */

/* To use precomputed canonical basis and these powers */
#define PRECOMPUTE 0

/* To use precomputed products of elements of canonical basis and these powers */
/* The compilation becomes very slow! */
/* PRECOMPUTE2 removes the effect of PRECOMPUTE */
#define PRECOMPUTE2 0




#endif
