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
#include "gf2x_arith_mod_xPplusOne.h"

#if ((CATEGORY==1) && (N0==2) && (DFR_SL_LEVEL==0))
/* log_2 (n-choose-t): 1286*/
#define MAX_PREFIX_LEN 256
#define DIVISOR_POWER_OF_TWO 7
#endif
#if ((CATEGORY==1) && (N0==3) && (DFR_SL_LEVEL==0))
/* log_2 (n-choose-t): 877*/
#define MAX_PREFIX_LEN 256
#define DIVISOR_POWER_OF_TWO 7
#endif
#if ((CATEGORY==1) && (N0==4) && (DFR_SL_LEVEL==0))
/* log_2 (n-choose-t): 728*/
#define MAX_PREFIX_LEN 256
#define DIVISOR_POWER_OF_TWO 7
#endif
#if ((CATEGORY==1) && (N0==2) && (DFR_SL_LEVEL==1))
/* log_2 (n-choose-t): 1313*/
#define MAX_PREFIX_LEN 256
#define DIVISOR_POWER_OF_TWO 7
#endif
#if ((CATEGORY==1) && (N0==3) && (DFR_SL_LEVEL==1))
/* log_2 (n-choose-t): 892*/
#define MAX_PREFIX_LEN 256
#define DIVISOR_POWER_OF_TWO 7
#endif
#if ((CATEGORY==1) && (N0==4) && (DFR_SL_LEVEL==1))
/* log_2 (n-choose-t): 737*/
#define MAX_PREFIX_LEN 256
#define DIVISOR_POWER_OF_TWO 7
#endif
#if ((CATEGORY==3) && (N0==2) && (DFR_SL_LEVEL==0))
/* log_2 (n-choose-t): 1974*/
#define MAX_PREFIX_LEN 384
#define DIVISOR_POWER_OF_TWO 7
#endif
#if ((CATEGORY==3) && (N0==3) && (DFR_SL_LEVEL==0))
/* log_2 (n-choose-t): 1342*/
#define MAX_PREFIX_LEN 384
#define DIVISOR_POWER_OF_TWO 7
#endif
#if ((CATEGORY==3) && (N0==4) && (DFR_SL_LEVEL==0))
/* log_2 (n-choose-t): 1103*/
#define MAX_PREFIX_LEN 384
#define DIVISOR_POWER_OF_TWO 7
#endif
#if ((CATEGORY==3) && (N0==2) && (DFR_SL_LEVEL==1))
/* log_2 (n-choose-t): 2046*/
#define MAX_PREFIX_LEN 384
#define DIVISOR_POWER_OF_TWO 8
#endif
#if ((CATEGORY==3) && (N0==3) && (DFR_SL_LEVEL==1))
/* log_2 (n-choose-t): 1378*/
#define MAX_PREFIX_LEN 384
#define DIVISOR_POWER_OF_TWO 8
#endif
#if ((CATEGORY==3) && (N0==4) && (DFR_SL_LEVEL==1))
/* log_2 (n-choose-t): 1145*/
#define MAX_PREFIX_LEN 384
#define DIVISOR_POWER_OF_TWO 8
#endif

#if ((CATEGORY==5) && (N0==2) && (DFR_SL_LEVEL==0))
/* log_2 (n-choose-t): 2689*/
#define MAX_PREFIX_LEN 512
#define DIVISOR_POWER_OF_TWO 7
#endif
#if ((CATEGORY==5) && (N0==3) && (DFR_SL_LEVEL==0))
/* log_2 (n-choose-t): 1816*/
#define MAX_PREFIX_LEN 512
#define DIVISOR_POWER_OF_TWO 7
#endif
#if ((CATEGORY==5) && (N0==4) && (DFR_SL_LEVEL==0))
/* log_2 (n-choose-t): 1504*/
#define MAX_PREFIX_LEN 512
#define DIVISOR_POWER_OF_TWO 8
#endif
#if ((CATEGORY==5) && (N0==2) && (DFR_SL_LEVEL==1))
/* log_2 (n-choose-t): 2794*/
#define MAX_PREFIX_LEN 512
#define DIVISOR_POWER_OF_TWO 8
#endif
#if ((CATEGORY==5) && (N0==3) && (DFR_SL_LEVEL==1))
/* log_2 (n-choose-t): 1890*/
#define MAX_PREFIX_LEN 512
#define DIVISOR_POWER_OF_TWO 8
#endif
#if ((CATEGORY==5) && (N0==4) && (DFR_SL_LEVEL==1))
/* log_2 (n-choose-t): 1563*/
#define MAX_PREFIX_LEN 512
#define DIVISOR_POWER_OF_TWO 8
#endif

#define DIVISOR (1 << DIVISOR_POWER_OF_TWO)
#define MAX_COMPRESSED_LEN (((N0*P-NUM_ERRORS_T)/(DIVISOR))+ NUM_ERRORS_T*(1+DIVISOR_POWER_OF_TWO))

/*----------------------------------------------------------------------------*/
void constant_weight_to_binary_approximate(unsigned char *const bitstreamOut,
      const DIGIT constantWeightIn[]);
int binary_to_constant_weight_approximate(DIGIT *constantWeightOut,
      unsigned char *const bitstreamIn,
      const int bitLength);

uint64_t bitstream_read(const unsigned char *const stream,
                        const unsigned int bit_amount,
                        unsigned int *bit_cursor);
void bitstream_write_construction(unsigned char *output,
                     const unsigned int amount_to_write,
                     unsigned int *output_bit_cursor,
                     uint64_t value_to_write);
