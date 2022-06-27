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
#include "qc_ldpc_parameters.h"
#include "gf2x_limbs.h"

#if (((CATEGORY==1) || (CATEGORY==3) ) && (N0==2))
#define              ITERATIONS_MAX   (6)
#elif (((CATEGORY==1) || (CATEGORY==3) ) && (N0==3))
#define              ITERATIONS_MAX   (5)
#else
#define              ITERATIONS_MAX   (4)
#endif

int bf_decoding_CT(DIGIT out[],
                   const POSITION_T HtrPosOnes[N0][V],
                   const POSITION_T HPosOnes[N0][V],
                   DIGIT privateSyndrome[]);
#if ((CATEGORY==1) && (N0==4))
#define SYNDROME_TRESH_LOOKUP_TABLE { 0, 45},\
{ 1874, 46},\
{ 2325, 47},\
{ 2590, 48},\
{ 2762, 49}
#define SYNDROME_TRESH_TABLE_SIZE 5
#endif
#if ((CATEGORY==1) && (N0==3))
#define SYNDROME_TRESH_LOOKUP_TABLE { 0, 42},\
{ 1296, 43},\
{ 2313, 44},\
{ 2770, 45},\
{ 3030, 46},\
{ 3222, 47}
#define SYNDROME_TRESH_TABLE_SIZE 6
#endif
#if ((CATEGORY==1) && (N0==2))
#define SYNDROME_TRESH_LOOKUP_TABLE { 0, 38},\
{ 2248, 39},\
{ 3292, 40},\
{ 3835, 41},\
{ 4176, 42},\
{ 4391, 43}
#define SYNDROME_TRESH_TABLE_SIZE 6
#endif
#if ((CATEGORY==3) && (N0==4))
#define SYNDROME_TRESH_LOOKUP_TABLE { 0, 65},\
{ 3146, 66},\
{ 4039, 67},\
{ 4551, 68},\
{ 4929, 69},\
{ 5158, 70},\
{ 5332, 71},\
{ 5484, 72}
#define SYNDROME_TRESH_TABLE_SIZE 8
#endif
#if ((CATEGORY==3) && (N0==3))
#define SYNDROME_TRESH_LOOKUP_TABLE { 0, 62},\
{ 3980, 63},\
{ 4961, 64},\
{ 5523, 65},\
{ 5888, 66},\
{ 6145, 67},\
{ 6344, 68},\
{ 6500, 69}
#define SYNDROME_TRESH_TABLE_SIZE 8
#endif
#if ((CATEGORY==3) && (N0==2))
#define SYNDROME_TRESH_LOOKUP_TABLE { 0, 54},\
{ 3930, 55},\
{ 6033, 56},\
{ 7149, 57},\
{ 7834, 58},\
{ 8293, 59},\
{ 8618, 60},\
{ 8863, 61}
#define SYNDROME_TRESH_TABLE_SIZE 8
#endif
#if ((CATEGORY==5) && (N0==4))
#define SYNDROME_TRESH_LOOKUP_TABLE { 0, 85},\
{ 4788, 86},\
{ 6434, 87},\
{ 7379, 88},\
{ 7951, 89},\
{ 8392, 90},\
{ 8701, 91},\
{ 8943, 92},\
{ 9131, 93},\
{ 9301, 94}
#define SYNDROME_TRESH_TABLE_SIZE 10
#endif
#if ((CATEGORY==5) && (N0==3))
#define SYNDROME_TRESH_LOOKUP_TABLE { 0, 81},\
{ 6294, 82},\
{ 8005, 83},\
{ 8998, 84},\
{ 9635, 85},\
{ 10100, 86},\
{ 10440, 87},\
{ 10712, 88},\
{ 10930, 89},\
{ 11104, 90}
#define SYNDROME_TRESH_TABLE_SIZE 10
#endif
#if ((CATEGORY==5) && (N0==2))
#define SYNDROME_TRESH_LOOKUP_TABLE { 0, 71},\
{ 5931, 72},\
{ 9784, 73},\
{ 11717, 74},\
{ 12924, 75},\
{ 13737, 76},\
{ 14307, 77},\
{ 14749, 78},\
{ 15073, 79},\
{ 15343, 80}
#define SYNDROME_TRESH_TABLE_SIZE 10
#endif
