/**
 *
 * <constant_weight_codec.c>
 *
 * @version 2.0 (March 2019)
 *
 * Reference ISO-C11 Implementation of the LEDAcrypt PKC cipher using GCC built-ins.
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

#include <assert.h>
#include <string.h>

/* bits will be written to the output matching the same convention of the
 * bitstream read, i.e., in the same order as they appear in the natural
 * encoding of the uint64_t, with the most significant bit being written
 * as the first one in the output bitstream, starting in the output_bit_cursor
 * position */

void bitstream_write(unsigned char *output,
                     const unsigned int amount_to_write,
                     unsigned int *output_bit_cursor,
                     uint64_t value_to_write)
{
   if (amount_to_write == 0) return;
   assert(amount_to_write <= 64);
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
   assert(bit_amount <=64);
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

/*----------------------------------------------------------------------------*/
/* returns the portion of the bitstream read, padded with zeroes if the
   bitstream has less bits than required. Updates the value of the bit cursor */
static
uint64_t bitstream_read_padded (const unsigned char *const stream,
                                const unsigned int bitAmount,
                                const unsigned int bitstreamLength,
                                unsigned int *const bitCursor)
{
   uint64_t readBitstreamFragment;
   if ( (*bitCursor+bitAmount) < bitstreamLength) {
      readBitstreamFragment = bitstream_read(stream, bitAmount, bitCursor);
   } else {
      /*if remaining bits are not sufficient, pad with enough zeroes */
      unsigned int available_bits =  bitstreamLength-*bitCursor;
      if (available_bits) {
         readBitstreamFragment = bitstream_read(stream, available_bits, bitCursor);
         readBitstreamFragment = readBitstreamFragment << (bitAmount - available_bits);
      } else {
         readBitstreamFragment = 0;
      }
   }
   return readBitstreamFragment;
} // end bitstream_read_padded

/*----------------------------------------------------------------------------*/

static inline
void estimate_d_u(unsigned *d, unsigned *u, const unsigned n, const unsigned t)
{

   *d= 0.69315 * ((double)n - ( (double)t - 1.0)/2.0) /((double) t);
   *u = 0;
   unsigned tmp = *d;
   while(tmp) {
      tmp >>= 1;
      *u = *u +1;
   }
} //end bitstream_read_padded

/*----------------------------------------------------------------------------*/
/* Encodes a bit string into a constant weight N0 polynomials vector*/
void constant_weight_to_binary_approximate(unsigned char *const bitstreamOut,
      const DIGIT constantWeightIn[])
{
   unsigned int distancesBetweenOnes[NUM_ERRORS_T] = {0};

   /*compute the array of inter-ones distances. Note that there
    is an implicit one out of bounds to compute the first distance from */
   unsigned int last_one_position=-1;
   unsigned int idxDistances=0;

   for(unsigned int current_inspected_position = 0;
         current_inspected_position< N0*P; current_inspected_position++) {
      unsigned int current_inspected_exponent, current_inspected_poly;
      current_inspected_exponent = current_inspected_position %P;
      current_inspected_poly = current_inspected_position / P;
      if( gf2x_get_coeff(constantWeightIn
                         +current_inspected_poly*NUM_DIGITS_GF2X_ELEMENT,
                         current_inspected_exponent) == 1) {
         distancesBetweenOnes[idxDistances]= current_inspected_position -
                                             last_one_position - 1;
         last_one_position = current_inspected_position;
         idxDistances++;
      }
   }
   assert(idxDistances == NUM_ERRORS_T);

   /* perform encoding of distances into binary string*/
   unsigned int onesStillToPlaceOut = NUM_ERRORS_T;
   unsigned int inPositionsStillAvailable = N0*P;

   unsigned int outputBitCursor=0;
   unsigned int d;
   unsigned int u;

   for (idxDistances = 0; idxDistances< NUM_ERRORS_T; idxDistances++) {
      estimate_d_u(&d, &u, inPositionsStillAvailable, onesStillToPlaceOut);

      unsigned int quotient;
      if(d != 0) {
        quotient = distancesBetweenOnes[idxDistances] / d;
      } else {
          return;
      }
      /* write out quotient in unary, with the trailing 0, i.e., 1^*0 */
      bitstream_write(bitstreamOut,
                      quotient+1,
                      &outputBitCursor,
                      ( ((((uint64_t) 1) << quotient ) -1) << 1));

      unsigned int remainder = distancesBetweenOnes[idxDistances] % d;
      if(remainder < ((1 << u) - d)) {
         u = u>0 ? u-1 : 0; // clamp u-minus-one to zero
         bitstream_write(bitstreamOut,u,&outputBitCursor,remainder);
      } else {
         bitstream_write(bitstreamOut,
                         u,
                         &outputBitCursor,
                         remainder + ((1 << u) - d));
      }
      inPositionsStillAvailable -= distancesBetweenOnes[idxDistances]+1;
      onesStillToPlaceOut--;
   }
} // end constant_weight_to_binary_approximate

/*----------------------------------------------------------------------------*/

int binary_to_constant_weight_approximate(DIGIT *constantWeightOut,
      const unsigned char *const bitstreamIn,
      const int bitLength)
{
   uint32_t distancesBetweenOnes[NUM_ERRORS_T];
   uint32_t idxDistances = 0;
   uint32_t onesStillToPlaceOut = NUM_ERRORS_T;
   uint32_t outPositionsStillAvailable = N0*P;
   unsigned int bitstreamInCursor = 0; /* assuming trailing slack bits in the input
   stream. In case the slack bits in the input stream are leading, change to
   8- (bitLength %8) - 1 */

   for(idxDistances = 0; idxDistances< NUM_ERRORS_T &&
         outPositionsStillAvailable > onesStillToPlaceOut; idxDistances++) {
      /* lack of positions should not be possible */
      if ( outPositionsStillAvailable < onesStillToPlaceOut ||
            outPositionsStillAvailable < 0) {
         return 0;
      }
      /*estimate d and u */
      unsigned int d;
      unsigned int u;
      estimate_d_u(&d,&u,outPositionsStillAvailable,onesStillToPlaceOut);
      /* read unary-encoded quotient, i.e. leading 1^* 0 */
      unsigned int quotient = 0;
      while ((uint64_t) 1 == bitstream_read_padded(bitstreamIn,
             1,
             bitLength,
             &bitstreamInCursor)) {
         quotient++;
      }
      /* decode truncated binary encoded integer */
      uint32_t distanceToBeComputed = (u > 0) ? bitstream_read_padded(bitstreamIn,
                                      u-1,
                                      bitLength,
                                      &bitstreamInCursor) : 0;
      if(distanceToBeComputed >= ((1 << u) - d) ) {
         distanceToBeComputed *= 2;
         distanceToBeComputed += bitstream_read_padded(bitstreamIn,
                                 1,
                                 bitLength,
                                 &bitstreamInCursor);
         distanceToBeComputed -= ((1 << u) - d);
      }
      distancesBetweenOnes[idxDistances] = distanceToBeComputed + quotient* d;
      outPositionsStillAvailable -= distancesBetweenOnes[idxDistances] + 1;
      onesStillToPlaceOut--;
   }

   if (outPositionsStillAvailable == onesStillToPlaceOut) {
      for ( ; idxDistances< NUM_ERRORS_T; idxDistances++) {
         distancesBetweenOnes[idxDistances] = 0;
      }
   }
   if (outPositionsStillAvailable < onesStillToPlaceOut) {
      return 0;
   }

   if (bitstreamInCursor < HASH_BYTE_LENGTH*8){
       return 0;
   }
   /*encode ones according to distancesBetweenOnes into constantWeightOut */
   int current_one_position = -1;
   for (int i=0; i < NUM_ERRORS_T; i++) {
      current_one_position += distancesBetweenOnes[i] + 1;
      if (current_one_position >= N0*P) {
         return 0;
      }
      unsigned int polyIndex = current_one_position / P;
      unsigned int exponent = current_one_position % P;
      gf2x_set_coeff( constantWeightOut + NUM_DIGITS_GF2X_ELEMENT*polyIndex, exponent,
                      ( (DIGIT) 1));
   }
   return 1;
} // end binary_to_constant_weight_approximate
