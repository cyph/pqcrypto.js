#define CATEGORY 3
#define DFR_SL_LEVEL 1
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
#if (CATEGORY == 1) && (N0 == 2) && (DFR_SL_LEVEL ==0)
#define    P  (23371)
#define    V  (71)
#define    NUM_ERRORS_T  (130)
#endif
#if (CATEGORY == 1) && (N0 == 3) && (DFR_SL_LEVEL ==0)
#define    P  (16067)
#define    V  (79)
#define    NUM_ERRORS_T  (83)
#endif
#if (CATEGORY == 1) && (N0 == 4) && (DFR_SL_LEVEL ==0)
#define    P  (13397)
#define    V  (83)
#define    NUM_ERRORS_T  (66)
#endif
#if (CATEGORY == 1) && (N0 == 2) && (DFR_SL_LEVEL ==1)
#define    P  (28277)
#define    V  (69)
#define    NUM_ERRORS_T  (129)
#endif
#if (CATEGORY == 1) && (N0 == 3) && (DFR_SL_LEVEL ==1)
#define    P  (19709)
#define    V  (79)
#define    NUM_ERRORS_T  (82)
#endif
#if (CATEGORY == 1) && (N0 == 4) && (DFR_SL_LEVEL ==1)
#define    P  (16229)
#define    V  (83)
#define    NUM_ERRORS_T  (65)
#endif

#if (CATEGORY == 3) && (N0 == 2) && (DFR_SL_LEVEL ==0)
#define    P  (40787)
#define    V  (103)
#define    NUM_ERRORS_T  (195)
#endif
#if (CATEGORY == 3) && (N0 == 3) && (DFR_SL_LEVEL ==0)
#define    P  (28411)
#define    V  (117)
#define    NUM_ERRORS_T  (124)
#endif
#if (CATEGORY == 3) && (N0 == 4) && (DFR_SL_LEVEL ==0)
#define    P  (22901)
#define    V  (123)
#define    NUM_ERRORS_T  (98)
#endif
#if (CATEGORY == 3) && (N0 == 2) && (DFR_SL_LEVEL ==1)
#define    P  (52667)
#define    V  (103)
#define    NUM_ERRORS_T  (195)
#endif
#if (CATEGORY == 3) && (N0 == 3) && (DFR_SL_LEVEL ==1)
#define    P  (36629)
#define    V  (115)
#define    NUM_ERRORS_T  (123)
#endif
#if (CATEGORY == 3) && (N0 == 4) && (DFR_SL_LEVEL ==1)
#define    P  (30803)
#define    V  (123)
#define    NUM_ERRORS_T  (98)
#endif

#if (CATEGORY == 5) && (N0 == 2) && (DFR_SL_LEVEL ==0)
#define    P  (61717)
#define    V  (137)
#define    NUM_ERRORS_T  (261)
#endif
#if (CATEGORY == 5) && (N0 == 3) && (DFR_SL_LEVEL ==0)
#define    P  (42677)
#define    V  (153)
#define    NUM_ERRORS_T  (165)
#endif
#if (CATEGORY == 5) && (N0 == 4) && (DFR_SL_LEVEL ==0)
#define    P  (35507)
#define    V  (163)
#define    NUM_ERRORS_T  (131)
#endif
#if (CATEGORY == 5) && (N0 == 2) && (DFR_SL_LEVEL ==1)
#define    P  (83579)
#define    V  (135)
#define    NUM_ERRORS_T  (260)
#endif
#if (CATEGORY == 5) && (N0 == 3) && (DFR_SL_LEVEL ==1)
#define    P  (58171)
#define    V  (153)
#define    NUM_ERRORS_T  (165)
#endif
#if (CATEGORY == 5) && (N0 == 4) && (DFR_SL_LEVEL ==1)
#define    P  (48371)
#define    V  (161)
#define    NUM_ERRORS_T  (131)
#endif

#if CATEGORY == 1

#define TRNG_BYTE_LENGTH (24)
#define    HASH_FUNCTION sha3_256
#define HASH_BYTE_LENGTH (32)

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
#else
#error "Unsupported Category"
#endif

// Derived parameters, they are useful for QC-LDPC algorithms
#define HASH_BIT_LENGTH (HASH_BYTE_LENGTH << 3)
#define               K ((N0-1)*P)
#define               N (N0*P)

/*----------------------------------------------------------------------------*/
// Kobara-Imai-Gamma derived parameters
#define MAX_ENCODABLE_BIT_SIZE_CW_ENCODING (HASH_BYTE_LENGTH*8)
#define KOBARA_IMAI_CONSTANT_LENGTH_B (TRNG_BYTE_LENGTH)
static const unsigned char KobaraImaiConstant[KOBARA_IMAI_CONSTANT_LENGTH_B] = {0};
#define KI_LENGTH_FIELD_TYPE uint64_t
#define KI_LENGTH_FIELD_SIZE (sizeof(KI_LENGTH_FIELD_TYPE))

#define MAX_BYTES_IN_IWORD ((K - 8*(KOBARA_IMAI_CONSTANT_LENGTH_B + sizeof(KI_LENGTH_FIELD_TYPE)) )/8)

#define CWENC_EXTRA_RANDOMNESS_LEN 1024
