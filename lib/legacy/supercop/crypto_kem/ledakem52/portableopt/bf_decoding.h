/**
 *
 * <bf_decoding.h>
 *
 * @version 2.0 (March 2019)
 *
 * Reference ISO-C11 Implementation of LEDAcrypt using GCC built-ins.
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
#define              ITERATIONS_MAX   (15)


int bf_decoding(DIGIT err[],
                const POSITION_T HtrPosOnes[N0][DV],
                const POSITION_T QtrPosOnes[N0][M], // N0 vectors containing exp.s of Qtr ones
                DIGIT privateSyndrome[]       //  1 polynomial  -- param. in/out
               );

/******************** START of definitions for N0 = 2 *************************/
#if (CATEGORY == 1) && (N0 == 2)
#define SYNDROME_TRESH_LOOKUP_TABLE {0,       41},\
{2919,       42},\
{4401,       43},\
{5178,       44},\
{5648,       45}
#endif

#if ((CATEGORY == 2) || (CATEGORY == 3)) && (N0 == 2)
#define SYNDROME_TRESH_LOOKUP_TABLE {0,       61},\
{4230,       62},\
{7022,       63},\
{8501,       64},\
{9423,       65},\
{10023,       66},\
{10452,       67},\
{10777,       68}
#endif

#if ((CATEGORY == 4) || (CATEGORY == 5)) && (N0 == 2)
#define SYNDROME_TRESH_LOOKUP_TABLE {0,       74},\
{5521,       75},\
{9830,       76},\
{12004,       77},\
{13363,       78},\
{14241,       79},\
{14915,       80},\
{15384,       81},\
{15770,       82},\
{16069,       83},\
{16300,       84}
#endif

/******************** START of definitions for N0 = 3 *************************/

#if (CATEGORY == 1) && (N0 == 3)
#define SYNDROME_TRESH_LOOKUP_TABLE {0,       43},\
{1089,       44},\
{2212,       45},\
{2717,       46},\
{3003,       47},\
{3214,       48},\
{3355,       49}
#endif

#if ((CATEGORY == 2) || (CATEGORY == 3)) && (N0 == 3)
#define SYNDROME_TRESH_LOOKUP_TABLE  {0,       64},\
{3954,       65},\
{5039,       66},\
{5663,       67},\
{6070,       68},\
{6383,       69},\
{6602,       70},\
{6773,       71}
#endif

#if ((CATEGORY == 4) || (CATEGORY == 5)) && (N0 == 3)
#define SYNDROME_TRESH_LOOKUP_TABLE {0,       86},\
{6369,       87},\
{8307,       88},\
{9436,       89},\
{10162,       90},\
{10692,       91},\
{11113,       92},\
{11419,       93},\
{11664,       94},\
{11861,       95}
#endif

/******************** START of definitions for N0 = 4 *************************/


#if (CATEGORY == 1) && (N0 == 4)
#define SYNDROME_TRESH_LOOKUP_TABLE {0,       49},\
{1840,       50},\
{2333,       51},\
{2648,       52},\
{2850,       53},\
{2978,       54}
#endif

#if ((CATEGORY == 2) || (CATEGORY == 3)) && (N0 == 4)
#define SYNDROME_TRESH_LOOKUP_TABLE {0,       71},\
{3146,       72},\
{4261,       73},\
{4859,       74},\
{5263,       75},\
{5568,       76},\
{5771,       77},\
{5924,       78},\
{6061,       79}
#endif

#if ((CATEGORY == 4) || (CATEGORY == 5)) && (N0 == 4)
#define SYNDROME_TRESH_LOOKUP_TABLE {0,       88},\
{4779,       89},\
{6564,       90},\
{7596,       91},\
{8223,       92},\
{8707,       93},\
{9047,       94},\
{9314,       95},\
{9522,       96},\
{9709,       97}
#endif
