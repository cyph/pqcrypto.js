#define CATEGORY 3
#define N0 3
/**
 *
 * Optimized ISO-C11 Implementation of LEDAcrypt using GCC built-ins.
 *
 * @version 3.0 (May 2020)
 *
 * In alphabetical order:
 *
 * @author Marco Baldi <m.baldi@univpm.it>
 * @author Alessandro Barenghi <alessandro.barenghi@polimi.it>
 * @author Franco Chiaraluce <f.chiaraluce@univpm.it>
 * @author Gerardo Pelosi <gerardo.pelosi@polimi.it>
 * @author Paolo Santini <p.santini@pm.univpm.it>
 *
 * This code is hereby placed in the public domain.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ''AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 **/

#pragma once
// CATEGORY defined in the makefile
#if ((CATEGORY==1) && (N0==4))
#define    P             (  7187)
#define    V             ( 83)
#define    NUM_ERRORS_T  (  67)
#endif

#if ((CATEGORY==1) && (N0==3))
#define    P             (  8237)
#define    V             ( 79)
#define    NUM_ERRORS_T  (  84)
#endif

#if ((CATEGORY==1) && (N0==2))
#define    P             ( 10883)
#define    V             ( 71)
#define    NUM_ERRORS_T  ( 133)
#endif

#if ((CATEGORY==3) && (N0==4))
#define    P             ( 13109)
#define    V             (123)
#define    NUM_ERRORS_T  (  99)
#endif

#if ((CATEGORY==3) && (N0==3))
#define    P             ( 15373)
#define    V             (117)
#define    NUM_ERRORS_T  ( 125)
#endif

#if ((CATEGORY==3) && (N0==2))
#define    P             ( 21011)
#define    V             (103)
#define    NUM_ERRORS_T  ( 198)
#endif

#if ((CATEGORY==5) && (N0==4))
#define    P             ( 21611)
#define    V             (163)
#define    NUM_ERRORS_T  ( 132)
#endif

#if ((CATEGORY==5) && (N0==3))
#define    P             ( 25603)
#define    V             (155)
#define    NUM_ERRORS_T  ( 166)
#endif

#if ((CATEGORY==5) && (N0==2))
#define    P             ( 35339)
#define    V             (137)
#define    NUM_ERRORS_T  ( 263)
#endif


#if CATEGORY == 1
#define TRNG_BYTE_LENGTH (24)
#define    HASH_FUNCTION sha3_256
#define HASH_BYTE_LENGTH (32)
#define   SHAKE_FUNCTION shake_128

/*----------------------------------------------------------------------------*/

// We employ the parameters for Category 3 also in the case where the required
// security level is Category 2, where Category 2 has the following parameters.
//   #define TRNG_BYTE_LENGTH (32)
//   #define    HASH_FUNCTION sha3_256
//   #define HASH_BYTE_LENGTH (32)

/*----------------------------------------------------------------------------*/

#elif (CATEGORY == 2) || (CATEGORY == 3)

#define TRNG_BYTE_LENGTH (32)
#define    HASH_FUNCTION sha3_384
#define HASH_BYTE_LENGTH (48)
#define   SHAKE_FUNCTION shake_256

/*----------------------------------------------------------------------------*/

// We employ the parameters for Category 4 also in the case where the required
// security level is Category 5, where Category 4 has the following parameters.
// #if CATEGORY == 4
//   #define TRNG_BYTE_LENGTH (40)
//   #define    HASH_FUNCTION sha3_384
//   #define HASH_BYTE_LENGTH (48)
// #endif

/*----------------------------------------------------------------------------*/

#elif (CATEGORY == 4) || (CATEGORY == 5)

#define TRNG_BYTE_LENGTH (40)
#define    HASH_FUNCTION  sha3_512
#define HASH_BYTE_LENGTH (64)
#define   SHAKE_FUNCTION shake_256

#else
#error "Unsupported Category"
#endif

// Derived parameters, they are useful for QC-LDPC algorithms
#define HASH_BIT_LENGTH (HASH_BYTE_LENGTH << 3)
#define               K ((N0-1)*P)
#define               N (N0*P)
