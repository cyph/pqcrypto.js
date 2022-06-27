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


#include "constant_weight_codec.h"
#include "djbsort.h"
#include <string.h>
#include <randombytes.h>

void bitstream_write_construction(unsigned char *output,
                     const unsigned int amount_to_write,
                     unsigned int *output_bit_cursor,
                     uint64_t value_to_write)
{
   if (amount_to_write == 0) return;
   unsigned int bit_cursor_in_char = *output_bit_cursor % 8;
   unsigned int byte_cursor = *output_bit_cursor / 8;
   unsigned int remaining_bits_in_char = 8-bit_cursor_in_char;

   if (amount_to_write <= remaining_bits_in_char) {
      uint64_t cleanup_mask = ~( (((uint64_t)1 << amount_to_write) - 1) <<
                                 (remaining_bits_in_char-amount_to_write) );
      uint64_t buffer = output[byte_cursor];

      buffer = (buffer & cleanup_mask) | (value_to_write << (remaining_bits_in_char
                                          -amount_to_write));
      output[byte_cursor] = (unsigned char) buffer;
      *output_bit_cursor += amount_to_write;
   } else {
      /*copy remaining_bits_in_char, allowing further copies to be byte aligned */
      uint64_t write_buffer = value_to_write >> (amount_to_write -
                              remaining_bits_in_char);
      uint64_t cleanup_mask = ~((1 << remaining_bits_in_char) -1);

      uint64_t buffer = output[byte_cursor];
      buffer = (buffer & cleanup_mask) | write_buffer;
      output[byte_cursor] = buffer;
      *output_bit_cursor += remaining_bits_in_char;
      byte_cursor = *output_bit_cursor /8;

      /*write out as many as possible full bytes*/
      uint64_t still_to_write= amount_to_write - remaining_bits_in_char;
      while(still_to_write >8 ) {
         write_buffer = value_to_write >> (still_to_write - 8) & (uint64_t)0xFF;
         output[byte_cursor] = write_buffer;
         *output_bit_cursor += 8;
         byte_cursor++;
         still_to_write -= 8;
      } // end while
      /*once here, only the still_to_write-LSBs of value_to_write are to be written
       * with their MSB as the MSB of the output[byte_cursor] */
      if(still_to_write > 0) {
         write_buffer = value_to_write &  ((1 << still_to_write) - 1);
         uint64_t cleanup_mask = ~( ((1 << still_to_write) - 1) << (8-still_to_write) );
         write_buffer = write_buffer << (8-still_to_write);

         output[byte_cursor] &= cleanup_mask;
         output[byte_cursor] |= write_buffer;
         *output_bit_cursor += still_to_write;
      } // end if
   } // end else
} // end bitstream_write

/* bits will be written to the output matching the same convention of the
 * bitstream read, i.e., in the same order as they appear in the natural
 * encoding of the uint64_t, with the most significant bit being written
 * as the first one in the output bitstream, starting in the output_bit_cursor
 * position */

void bitstream_write(unsigned char *output,
                     const unsigned int amount_to_write,
                     unsigned int output_bit_cursor,
                     uint64_t value_to_write) {
    if (amount_to_write == 0) return;
    unsigned int bit_cursor_in_char = output_bit_cursor % 8;
    unsigned int byte_cursor = output_bit_cursor / 8;
    unsigned int remaining_bits_in_char = 8 - bit_cursor_in_char;

    if (amount_to_write <= remaining_bits_in_char) {
        uint64_t cleanup_mask = ( ( (uint64_t) 1 << amount_to_write ) - 1);
        cleanup_mask = cleanup_mask << (remaining_bits_in_char - amount_to_write);
        cleanup_mask = ~cleanup_mask;
        uint64_t buffer = output[byte_cursor];

        buffer = (buffer & cleanup_mask) | (value_to_write << (remaining_bits_in_char
                                                               - amount_to_write));
        output[byte_cursor] = (unsigned char) buffer;
        output_bit_cursor += amount_to_write;
    } else {
        /*copy remaining_bits_in_char, allowing further copies to be byte aligned */
        uint64_t write_buffer = value_to_write >> (amount_to_write -
                                                   remaining_bits_in_char);
        uint64_t cleanup_mask = ~((1 << remaining_bits_in_char) - 1);

        uint64_t buffer = output[byte_cursor];
        buffer = (buffer & cleanup_mask) | write_buffer;
        output[byte_cursor] = buffer;
        output_bit_cursor += remaining_bits_in_char;
        byte_cursor = output_bit_cursor / 8;

        /*write out as many as possible full bytes*/
        uint64_t still_to_write = amount_to_write - remaining_bits_in_char;
        while (still_to_write > 8) {
            write_buffer = value_to_write >> (still_to_write - 8) & (uint64_t) 0xFF;
            output[byte_cursor] = write_buffer;
            output_bit_cursor += 8;
            byte_cursor++;
            still_to_write -= 8;
        } // end while
        /*once here, only the still_to_write-LSBs of value_to_write are to be written
         * with their MSB as the MSB of the output[byte_cursor] */
        if (still_to_write > 0) {
            write_buffer = value_to_write & ((1 << still_to_write) - 1);
            uint64_t cleanup_mask = ~(((1 << still_to_write) - 1) << (8 - still_to_write));
            write_buffer = write_buffer << (8 - still_to_write);

            output[byte_cursor] &= cleanup_mask;
            output[byte_cursor] |= write_buffer;
            output_bit_cursor += still_to_write;
        } // end if
    } // end else
#if (defined HIGH_PERFORMANCE_X86_64)
    _mm_mfence();
#endif
} // end bitstream_write


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*
 * Input bitstream read as called by constantWeightEncoding
 * supports reading at most 64 bit at once since the caller will need to add
 * them to the encoding. Given the estimates for log_2(d), this is plentiful
 */
uint64_t bitstream_read(const unsigned char *const stream,
                        const unsigned int bit_amount,
                        unsigned int *bit_cursor)
{
   if(bit_amount == 0) return (uint64_t)0;
   uint64_t extracted_bits=0;
   int bit_cursor_in_char = *bit_cursor % 8;
   int remaining_bits_in_char = 8 - bit_cursor_in_char;

   if (bit_amount <= remaining_bits_in_char) {
      extracted_bits = (uint64_t) (stream[*bit_cursor/8]);
      int slack_bits = remaining_bits_in_char - bit_amount;
      extracted_bits = extracted_bits >> slack_bits;
      extracted_bits = extracted_bits &  ( (((uint64_t) 1) << bit_amount) - 1);

   } else {
      unsigned int byte_cursor = *bit_cursor/8;
      unsigned int still_to_extract = bit_amount;
      if (bit_cursor_in_char !=0) {
         extracted_bits = (uint64_t) (stream[*bit_cursor/8]);
         extracted_bits = extracted_bits &  ( (((uint64_t) 1) << (7-
                                               (bit_cursor_in_char -1)) ) - 1) ;
         still_to_extract = bit_amount-(7-(bit_cursor_in_char -1));
         byte_cursor++;
      }
      while(still_to_extract > 8) {
         extracted_bits = extracted_bits << 8 | ((uint64_t) (stream[byte_cursor]));
         byte_cursor++;
         still_to_extract = still_to_extract -8;
      }
      /* here byte cursor is on the byte where the still_to_extract MSbs are to be
       taken from */
      extracted_bits = (extracted_bits << still_to_extract ) |  ((uint64_t) (
                          stream[byte_cursor])) >> (8 - still_to_extract);
   }
   *bit_cursor = *bit_cursor + bit_amount;
   return extracted_bits;
} // end bitstream_read

#define COND_EXP2(COND,TRUE,FALSE) ( ((COND)*(TRUE)) | (!(COND)*(FALSE)) )
#define CONVTOMASK(x) ((uint32_t)0 -x)
#define COND_EXP(COND,TRUE,FALSE) ( ( CONVTOMASK(COND)&(TRUE)) | (CONVTOMASK(!(COND))&(FALSE)) )


void ct_store(POSITION_T* v, int index, POSITION_T value){
#if (defined HIGH_PERFORMANCE_X86_64)
     _mm_stream_si32 ((int*)(v+index), value);
     _mm_mfence();
#else
    for(int i=0;i< NUM_ERRORS_T ;i++){
         POSITION_T commitmask = (POSITION_T)0 - (i == index);
         v[i] = (value & commitmask) | (v[i] & ~commitmask);
    }
#endif
}

static inline
uint64_t bitstream_read_single_bit(const unsigned char *const stream,
                                   const unsigned int bitCursor) {
    int index = bitCursor /8;
    int posInByte = bitCursor %8;
    unsigned char mask = ((unsigned char) 0x80) >> posInByte;
    return (stream[index] & mask)!=0;
}

static inline
uint64_t rand_range_cwenc(uint64_t s, AES_XOF_struct * cwenc_randomness_gen) {
  uint64_t x;
  seedexpander(cwenc_randomness_gen, (unsigned char*)&x, sizeof(uint64_t));
  __uint128_t m = (__uint128_t) x * (__uint128_t) s;
  uint64_t l = (uint64_t) m;
  if (l < s) {
   uint64_t t = -s % s;
    while (l < t) {
      seedexpander(cwenc_randomness_gen, (unsigned char*)&x, sizeof(uint64_t));
      m = (__uint128_t) x * (__uint128_t) s;
      l = (uint64_t) m;
    }
  }
  return m >> 64;
}


static
int constant_time_bin_to_pos(POSITION_T positionsOut[NUM_ERRORS_T],
                            const unsigned char * const bitstreamIn) {

    unsigned char prng_seed[TRNG_BYTE_LENGTH];
    AES_XOF_struct cwenc_randomness_gen;
    randombytes(prng_seed, TRNG_BYTE_LENGTH);
    seedexpander_from_trng(&cwenc_randomness_gen, prng_seed);


    POSITION_T runLengths[NUM_ERRORS_T]={0};
    POSITION_T remaining_zeroes = N0*P-NUM_ERRORS_T;
    unsigned int bitstreamCursor = 0;
    POSITION_T current_lambda_idx=0;
    POSITION_T read_bit=0;
    POSITION_T quotient=0,quotient_complete=0;

    POSITION_T r=0, r_bit_counter=0, is_r_complete;
    POSITION_T lambda, is_lambda_complete;

    for(int i=0; i<MAX_PREFIX_LEN; i++){

        read_bit = bitstream_read_single_bit(bitstreamIn,bitstreamCursor);
        quotient_complete = quotient_complete | !read_bit;
        bitstreamCursor++;

        quotient = quotient + (read_bit & !(quotient_complete));
        r_bit_counter += quotient_complete;
        is_r_complete = (DIVISOR_POWER_OF_TWO - r_bit_counter) & 0x80000000;
        is_r_complete = (POSITION_T)0 - (is_r_complete >> 31);
        r= (r << 1) | (read_bit & quotient_complete);
        lambda = quotient*DIVISOR+r;
        ct_store(runLengths, current_lambda_idx, lambda);

        is_lambda_complete = (is_r_complete) & (quotient_complete);
        current_lambda_idx = COND_EXP(is_lambda_complete,current_lambda_idx+1,current_lambda_idx);
        remaining_zeroes = COND_EXP(is_lambda_complete,remaining_zeroes-lambda,remaining_zeroes);

        quotient = COND_EXP(is_lambda_complete,0,quotient);
        quotient_complete = COND_EXP(is_lambda_complete,0,quotient_complete);
        r = COND_EXP(is_lambda_complete,0,r);
        r_bit_counter = COND_EXP(is_lambda_complete,0,r_bit_counter);
    }

    DIGIT case_partial_quotient = !(quotient_complete);
    int range;
    range = remaining_zeroes-lambda;
    lambda = COND_EXP(case_partial_quotient,lambda+rand_range_cwenc(range+1,&cwenc_randomness_gen),lambda);

    DIGIT case_full_quotient = (quotient_complete) & (r_bit_counter == 1);
    range = COND_EXP((range > (DIVISOR-1)),(DIVISOR-1),range);
    lambda = COND_EXP(case_full_quotient,lambda+rand_range_cwenc(range+1,&cwenc_randomness_gen),lambda);

    DIGIT case_partial_remainder = (quotient_complete) & !(is_lambda_complete) & (r_bit_counter > 1);
    int missing_remainder_bits = DIVISOR_POWER_OF_TWO+1-r_bit_counter;
    range = (1 << missing_remainder_bits )-1 ;
    lambda = COND_EXP(case_partial_remainder,(r << missing_remainder_bits) + quotient * DIVISOR +rand_range_cwenc(range+1,&cwenc_randomness_gen) ,lambda);

    ct_store(runLengths, current_lambda_idx, lambda);
    current_lambda_idx = COND_EXP(!is_lambda_complete,current_lambda_idx+1,current_lambda_idx);
    remaining_zeroes = COND_EXP(!is_lambda_complete,remaining_zeroes-lambda,remaining_zeroes);

    for(int i = 0; i < NUM_ERRORS_T; i++){
        int need_random_lambda = (i >= current_lambda_idx);
        lambda = runLengths[current_lambda_idx];
        lambda = COND_EXP(need_random_lambda,rand_range_cwenc(remaining_zeroes+1,&cwenc_randomness_gen),lambda);

        ct_store(runLengths, current_lambda_idx, lambda);
        current_lambda_idx = COND_EXP(need_random_lambda,current_lambda_idx+1,current_lambda_idx);
        remaining_zeroes = COND_EXP(need_random_lambda,remaining_zeroes-lambda,remaining_zeroes);
    }

    /*encode ones according to runLengths into constantWeightOut */
    int current_one_position = -1;
    for (int i = 0; i < NUM_ERRORS_T; i++) {
        current_one_position += runLengths[i] + 1;
        positionsOut[i] = current_one_position;
    }
    return 1;
}

static
void constant_time_pos_to_bin(unsigned char * bitstreamOut,
                             const int trimOutLength,
                             POSITION_T positionsIn[NUM_ERRORS_T]){
    unsigned int runLengths[NUM_ERRORS_T] = {0};

    /*compute the array of inter-ones distances. Note that there
     is an implicit one out of bounds to compute the first distance from */
    unsigned int idxDistances = 0;

    /* compute run lengths from one positions */
    runLengths[0]=positionsIn[0]-0;

    for (int i = 1; i<NUM_ERRORS_T; i++){
        runLengths[i] = positionsIn[i] - positionsIn[i-1] /*remove the 1*/ - 1;
    }

    /* perform encoding of distances into binary string*/
    unsigned int outputBitCursor = 0;

    for (idxDistances = 0; idxDistances < NUM_ERRORS_T; idxDistances++) {
        unsigned int quotient = runLengths[idxDistances] / DIVISOR;
        for (int outbit=0; outbit<quotient; outbit++){
        bitstream_write(bitstreamOut,1,
                        outputBitCursor,(uint64_t) 1);
        outputBitCursor++;
        }
        bitstream_write(bitstreamOut,1,
                        outputBitCursor,(uint64_t) 0);
        outputBitCursor++;
        unsigned int remainder = runLengths[idxDistances] % DIVISOR;
        bitstream_write(bitstreamOut, DIVISOR_POWER_OF_TWO, outputBitCursor, remainder);
        outputBitCursor += DIVISOR_POWER_OF_TWO;

    }
    while (outputBitCursor < MAX_COMPRESSED_LEN){
                bitstream_write(bitstreamOut,1,outputBitCursor,(uint64_t) 0);
                outputBitCursor++;
    }
}


/*----------------------------------------------------------------------------*/
void constant_weight_to_binary_approximate(unsigned char *const bitstreamOut,
                                           const DIGIT constantWeightIn[]) {
    POSITION_T positionsIn[NUM_ERRORS_T];
    gf2x_mod_sparsify_error_CT(constantWeightIn,positionsIn,NUM_ERRORS_T);
    int32_sort((int*)positionsIn,NUM_ERRORS_T);

    unsigned char bitstream_buffer[MAX_COMPRESSED_LEN/8] = {0};
    constant_time_pos_to_bin(bitstream_buffer,MAX_PREFIX_LEN,positionsIn);

    memcpy(bitstreamOut,bitstream_buffer,MAX_PREFIX_LEN/8);
} // end constant_weight_to_binary_approximate

/*----------------------------------------------------------------------------*/
/* Encodes a bit string into a constant weight N0 polynomials vector*/
int binary_to_constant_weight_approximate(DIGIT *constantWeightOut,
                                          unsigned char *const bitstreamIn,
                                          const int bitLength){
   POSITION_T positionsOut[NUM_ERRORS_T];
   constant_time_bin_to_pos(positionsOut,bitstreamIn);
   expand_error(constantWeightOut,positionsOut);
   return 1;
} // end binary_to_constant_weight_approximate
