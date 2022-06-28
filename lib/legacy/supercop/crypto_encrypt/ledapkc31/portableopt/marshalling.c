/**
 *
 * <marshalling.c>
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

#include "marshalling.h"

/*----------------------------------------------------------------------------*/

void poly_to_byte_seq(uint8_t bs[NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B], 
                        DIGIT  y[NUM_DIGITS_GF2X_ELEMENT]) {
                            
    for (int i = 0; i < NUM_DIGITS_GF2X_ELEMENT; i++) {
        DIGIT a = y[i]; int v=0;
        for (int u = (int)(DIGIT_SIZE_B-1); u >= 0; u--) {
            bs[i*DIGIT_SIZE_B+v] = a >> (u*8);
            v++;
        }
    }
} // end poly_to_byte_seq

/*----------------------------------------------------------------------------*/

void byte_seq_to_poly(  DIGIT  y[NUM_DIGITS_GF2X_ELEMENT], 
                      uint8_t bs[NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B]) {
    
    for (int b = 0; b< NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B; b+=DIGIT_SIZE_B) {
        DIGIT a = 0; int v=0;
        for (int u = (int)(DIGIT_SIZE_B-1); u >= 0; u--) {
            a += ((DIGIT) bs[b+v]) << (u*8);
            v++;
        }
        y[b/DIGIT_SIZE_B] = a;
    }
} // end byte_seq_to_poly

/*----------------------------------------------------------------------------*/

