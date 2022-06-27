/**
 * qarma128_14.h
 *
 * @version 1.0 (March 2019)
 *
 * Reference ANSI C code for the Qarma tweakable block cipher
 *
 * @author Roberto Avanzi <roberto.avanzi@gmail.com>, <roberto.avanzi@arm.com>
 * @author Subhadeep Banik <subhadeep.banik@epfl.ch>
 * @author Francesco Regazzoni <regazzoni@alari.ch>
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
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define ROTATION_A  1
#define ROTATION_B  4
#define ROTATION_C  5

#define ROTATION_A_INV  1
#define ROTATION_B_INV  4
#define ROTATION_C_INV  5

#define ROTATION_A_MID  1
#define ROTATION_B_MID  4
#define ROTATION_C_MID  5

typedef unsigned char cell_t;

typedef unsigned char u8;

typedef unsigned long long u64;

// In the internal state matrix the data is stored as follows
//
//  0  1  2  3
//  4  5  6  7
//  8  9 10 11
// 12 13 14 15

// Definition of the 4-bit Sbox upon which the 8-bit ones are built

const cell_t sbox_4[16] =  { 10, 13, 14, 6, 15, 7, 3, 5, 9, 8, 0, 12, 11, 1, 2, 4 };
const cell_t *sbox_4_inv = sbox_4;

// ShuffleCells permutation (we use the INVERSE MIDORI permutation)

const int SHUFFLE_P[16]     = {0,11, 6,13,  10, 1,12, 7,   5,14, 3, 8,  15, 4, 9, 2};
const int SHUFFLE_P_inv[16] = {0, 5,15,10,  13, 8, 2, 7,  11,14, 4, 1,   6, 3, 9,12};

// INVERSE Tweak permutation

const int TWEAKEY_P[16]     = {6,5,14,15,0,1,2,3,7,12,13,4,8,9,10,11};
const int TWEAKEY_P_inv[16] = {4,5,6,7,11,1,0,8,12,13,14,15,9,10,2,3};

// round constants

const cell_t RC[15][16] = 
{{0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00},
 {0xA4,0x09, 0x38,0x22, 0x29,0x9F, 0x31,0xD0, 0x08,0x2E, 0xFA,0x98, 0xEC,0x4E, 0x6C,0x89},
 {0x45,0x28, 0x21,0xE6, 0x38,0xD0, 0x13,0x77, 0xBE,0x54, 0x66,0xCF, 0x34,0xE9, 0x0C,0x6C},
 {0xC0,0xAC, 0x29,0xB7, 0xC9,0x7C, 0x50,0xDD, 0x3F,0x84, 0xD5,0xB5, 0xB5,0x47, 0x09,0x17},
 {0x92,0x16, 0xD5,0xD9, 0x89,0x79, 0xFB,0x1B, 0xD1,0x31, 0x0B,0xA6, 0x98,0xDF, 0xB5,0xAC},
 {0x2F,0xFD, 0x72,0xDB, 0xD0,0x1A, 0xDF,0xB7, 0xB8,0xE1, 0xAF,0xED, 0x6A,0x26, 0x7E,0x96},
 {0xBA,0x7C, 0x90,0x45, 0xF1,0x2C, 0x7F,0x99, 0x24,0xA1, 0x99,0x47, 0xB3,0x91, 0x6C,0xF7},
 {0x08,0x01, 0xF2,0xE2, 0x85,0x8E, 0xFC,0x16, 0x63,0x69, 0x20,0xD8, 0x71,0x57, 0x4E,0x69},
 {0xA4,0x58, 0xFE,0xA3, 0xF4,0x93, 0x3D,0x7E, 0x0D,0x95, 0x74,0x8F, 0x72,0x8E, 0xB6,0x58},
 {0x71,0x8B, 0xCD,0x58, 0x82,0x15, 0x4A,0xEE, 0x7B,0x54, 0xA4,0x1D, 0xC2,0x5A, 0x59,0xB5},
 {0x9C,0x30, 0xD5,0x39, 0x2A,0xF2, 0x60,0x13, 0xC5,0xD1, 0xB0,0x23, 0x28,0x60, 0x85,0xF0},
 {0xCA,0x41, 0x79,0x18, 0xB8,0xDB, 0x38,0xEF, 0x8E,0x79, 0xDC,0xB0, 0x60,0x3A, 0x18,0x0E},
 {0x6C,0x9E, 0x0E,0x8B, 0xB0,0x1E, 0x8A,0x3E, 0xD7,0x15, 0x77,0xC1, 0xBD,0x31, 0x4B,0x27},
 {0x78,0xAF, 0x2F,0xDA, 0x55,0x60, 0x5C,0x60, 0xE6,0x55, 0x25,0xF3, 0xAA,0x55, 0xAB,0x94},
 {0x57,0x48, 0x98,0x62, 0x63,0xE8, 0x14,0x40, 0x55,0xCA, 0x39,0x6A, 0x2A,0xAB, 0x10,0xB6}};

const cell_t ALPHA[16] =
 {0x24,0x3F, 0x6A,0x88, 0x85,0xA3, 0x08,0xD3, 0x13,0x19, 0x8A,0x2E, 0x03,0x70, 0x73,0x44};

FILE* file;

void display_cells(const cell_t state[16])
{
    int i;
    unsigned char input[16];

    for(i = 0; i < 16; i++) input[i] = state[i] & 0xFF;
    for(i = 0; i < 8; i++) fprintf(file,"%02x", input[i]);
    fprintf(file," ");
    for(     ; i < 16; i++) fprintf(file,"%02x", input[i]);
}

void display_cipher_state(unsigned char state[16], unsigned char tweakCells[16])
{
    fprintf(file,"S = "); display_cells(state);
    fprintf(file," - T = "); display_cells(tweakCells);
}

// XOR Cells to the state

void Add(cell_t state[16], const cell_t Cells[16])
{
    for(int i = 0; i < 16; i++) state[i] ^= Cells[i];
}

// Apply the tweak update

void apply_perm(cell_t cells[16], const int permutation[16])
{
    int i;
    cell_t tmp[16];

    // Apply the TWEAK permutation, store in temporary array

    for(i = 0; i < 16; i++) tmp[i] = cells[permutation[i]];

    // copy back, destructing input

    for(i = 0; i < 16; i++) cells[i] = tmp[i];
}

// Apply the tweak update, its inverse

// maps (b_7,...,b_2,b_1,b_0) to ( b_0 + b_2 ,b_7,...,b_2,b_1)

int lfsr(int v)
{
    return (v >> 1) ^ (((v & 1) ^ ((v >> 2) & 1)) << 7);
}

// maps  ( b_8 = b_0 + b_2 ,b_7,...b_3,b_2,b_1) to (b_7,...,b_3,b_2,b_1,b_8 + b_2)

int lfsr_inv(int v)
{
    return ((v << 1) & 0xFF) ^ (((v >> 1) & 1) ^ ((v >> 7) & 1));
}

void updateTweak(cell_t cells[16])
{
    apply_perm(cells, TWEAKEY_P);

    cells[ 0] = lfsr(cells[ 0]);
    cells[ 1] = lfsr(cells[ 1]);
    cells[ 3] = lfsr(cells[ 3]);
    cells[ 4] = lfsr(cells[ 4]);
    cells[ 8] = lfsr(cells[ 8]);
    cells[11] = lfsr(cells[11]);
    cells[13] = lfsr(cells[13]);
}

void updateTweak_inv(cell_t cells[16])
{
    cells[ 0] = lfsr_inv(cells[ 0]);
    cells[ 1] = lfsr_inv(cells[ 1]);
    cells[ 3] = lfsr_inv(cells[ 3]);
    cells[ 4] = lfsr_inv(cells[ 4]);
    cells[ 8] = lfsr_inv(cells[ 8]);
    cells[11] = lfsr_inv(cells[11]);
    cells[13] = lfsr_inv(cells[13]);

    apply_perm(cells, TWEAKEY_P_inv);
}


// Apply the ShuffleCells function, its inverse

void ShuffleCells(cell_t cells[16])
{
    apply_perm(cells, SHUFFLE_P);
}

void ShuffleCells_inv(cell_t cells[16])
{
    apply_perm(cells, SHUFFLE_P_inv);
}

// Add a round constant

void AddConstants(cell_t cells[16], int r)
{
    for(int i = 0; i < 16; i++) cells[i] ^= RC[r][i];
}

// Apply alpha to cells state

void AddAlpha(cell_t cells[16])
{
    for(int i = 0; i < 16; i++) cells[i] ^= ALPHA[i];
}

// Apply the 8-bit Sbox

unsigned char bit_permute[256] = {
  0,   1,   4,   5,  16,  17,  20,  21,  64,  65,  68,  69,  80,  81,  84,  85, 
  2,   3,   6,   7,  18,  19,  22,  23,  66,  67,  70,  71,  82,  83,  86,  87, 
  8,   9,  12,  13,  24,  25,  28,  29,  72,  73,  76,  77,  88,  89,  92,  93, 
 10,  11,  14,  15,  26,  27,  30,  31,  74,  75,  78,  79,  90,  91,  94,  95, 
 32,  33,  36,  37,  48,  49,  52,  53,  96,  97, 100, 101, 112, 113, 116, 117, 
 34,  35,  38,  39,  50,  51,  54,  55,  98,  99, 102, 103, 114, 115, 118, 119, 
 40,  41,  44,  45,  56,  57,  60,  61, 104, 105, 108, 109, 120, 121, 124, 125, 
 42,  43,  46,  47,  58,  59,  62,  63, 106, 107, 110, 111, 122, 123, 126, 127, 
128, 129, 132, 133, 144, 145, 148, 149, 192, 193, 196, 197, 208, 209, 212, 213, 
130, 131, 134, 135, 146, 147, 150, 151, 194, 195, 198, 199, 210, 211, 214, 215, 
136, 137, 140, 141, 152, 153, 156, 157, 200, 201, 204, 205, 216, 217, 220, 221, 
138, 139, 142, 143, 154, 155, 158, 159, 202, 203, 206, 207, 218, 219, 222, 223, 
160, 161, 164, 165, 176, 177, 180, 181, 224, 225, 228, 229, 240, 241, 244, 245, 
162, 163, 166, 167, 178, 179, 182, 183, 226, 227, 230, 231, 242, 243, 246, 247, 
168, 169, 172, 173, 184, 185, 188, 189, 232, 233, 236, 237, 248, 249, 252, 253, 
170, 171, 174, 175, 186, 187, 190, 191, 234, 235, 238, 239, 250, 251, 254, 255, 
};

unsigned char bit_permute_inv[256] = {
  0,   1,  16,  17,   2,   3,  18,  19,  32,  33,  48,  49,  34,  35,  50,  51, 
  4,   5,  20,  21,   6,   7,  22,  23,  36,  37,  52,  53,  38,  39,  54,  55, 
 64,  65,  80,  81,  66,  67,  82,  83,  96,  97, 112, 113,  98,  99, 114, 115, 
 68,  69,  84,  85,  70,  71,  86,  87, 100, 101, 116, 117, 102, 103, 118, 119, 
  8,   9,  24,  25,  10,  11,  26,  27,  40,  41,  56,  57,  42,  43,  58,  59, 
 12,  13,  28,  29,  14,  15,  30,  31,  44,  45,  60,  61,  46,  47,  62,  63, 
 72,  73,  88,  89,  74,  75,  90,  91, 104, 105, 120, 121, 106, 107, 122, 123, 
 76,  77,  92,  93,  78,  79,  94,  95, 108, 109, 124, 125, 110, 111, 126, 127, 
128, 129, 144, 145, 130, 131, 146, 147, 160, 161, 176, 177, 162, 163, 178, 179, 
132, 133, 148, 149, 134, 135, 150, 151, 164, 165, 180, 181, 166, 167, 182, 183, 
192, 193, 208, 209, 194, 195, 210, 211, 224, 225, 240, 241, 226, 227, 242, 243, 
196, 197, 212, 213, 198, 199, 214, 215, 228, 229, 244, 245, 230, 231, 246, 247, 
136, 137, 152, 153, 138, 139, 154, 155, 168, 169, 184, 185, 170, 171, 186, 187, 
140, 141, 156, 157, 142, 143, 158, 159, 172, 173, 188, 189, 174, 175, 190, 191, 
200, 201, 216, 217, 202, 203, 218, 219, 232, 233, 248, 249, 234, 235, 250, 251, 
204, 205, 220, 221, 206, 207, 222, 223, 236, 237, 252, 253, 238, 239, 254, 255, 
};

cell_t sbox_8(cell_t v)
{
    cell_t vh = (v >> 4) & 0xF;
    cell_t vl =  v       & 0xF;
    cell_t u;

    u = (sbox_4[vh] << 4) | sbox_4[vl];

    u = bit_permute[u];
    
    return u;
}

cell_t sbox_8_inv(cell_t v)
{
    cell_t u;

    u = bit_permute_inv[v];

    cell_t vh = (u >> 4) & 0xF;
    cell_t vl =  u       & 0xF;

    u = (sbox_4_inv[vh] << 4) | sbox_4_inv[vl];

    return u;
}

void SubCell8(cell_t cells[16])
{
    for(int i = 0; i < 16; i++) cells[i] = sbox_8(cells[i]);
}

// Apply the 4-bit inverse Sbox

void SubCell8_inv(cell_t cells[16])
{
    for(int i = 0; i < 16; i++) cells[i] = sbox_8_inv(cells[i]);
}

// Apply the linear diffusion matrix
//
// M =
// 0 a b c  a,b,c means rotate by a,b,c
// c 0 a b
// b c 0 a
// a b c 0
//
// 0 a b c     s 0 s 1 s 2 s 3    
// c 0 a b     s 4 s 5 s 6 s 7    
// b c 0 a  X  s 8 s 9 s10 s11
// a b c 0     s12 s13 s14 s15    

cell_t RotCell(cell_t val, int amount)
{
    return ( ((val << amount) | (val >> (8-amount))) & 0xFF );
}

void MixColumns(cell_t state[16], int a, int b, int c)
{
    int j;
    cell_t temp0, temp1, temp2, temp3;

    for(j = 0; j < 4; j++) // for each column, that has 0,1,2,3 at the top
    {
        temp0 =                       RotCell(state[j+4],a) ^ RotCell(state[j+8],b) ^ RotCell(state[j+12],c);
        temp1 = RotCell(state[j],c) ^                         RotCell(state[j+8],a) ^ RotCell(state[j+12],b);
        temp2 = RotCell(state[j],b) ^ RotCell(state[j+4],c) ^                         RotCell(state[j+12],a);
        temp3 = RotCell(state[j],a) ^ RotCell(state[j+4],b) ^ RotCell(state[j+8],c);

        state[j]    = temp0;
        state[j+4]  = temp1;
        state[j+8]  = temp2;
        state[j+12] = temp3;
    }
}

// ////////////////////////////////////////////////////////////////////

// encryption function of qarma128_R
 

void qarma128(const cell_t * input,
             const cell_t W[16], const cell_t W_p[16],
             const cell_t core_key[16], const cell_t middle_key[16],
             const cell_t tweak[16],
             int R, cell_t *output)
{
    cell_t state[16];
    cell_t T[16];
    cell_t K[16];

    int i;

    for(i = 0; i < 16; i++) {
    	state[i] = input[i];
    	T[i]     = tweak[i];
    	K[i]     = core_key[i];
    }

 

    // Initial whitening

    Add(state, W);					 

    // The R forward rounds

    for(i = 0; i < R; i++)
    {
    	// A round is Add(constant+key+tweak), then S-Box, MixColumns, and Shuffle
    
    	AddConstants(state, i);
    	Add(state, K);
    	Add(state, T);				 
    	if (i != 0)
    	{
    		ShuffleCells(state);	 
    		MixColumns(state,ROTATION_A,ROTATION_B,ROTATION_C);	
    								 
    	}
    	SubCell8(state);			 
    	updateTweak(T);
    } 

    // The pseudo-reflector

       // first whitening key addition

    Add(state, W_p);
    Add(state, T);					 

    // full forward diffusion layer

    ShuffleCells(state);			 
    MixColumns(state,ROTATION_A,ROTATION_B,ROTATION_C);	
    								 
    SubCell8(state);

    // pseudo reflector

    ShuffleCells(state);

    MixColumns(state,ROTATION_A_MID,ROTATION_B_MID,ROTATION_C_MID);	
    Add(state, middle_key);

    ShuffleCells_inv(state);

    // backward round with whitening key in place of core key

    SubCell8_inv(state);			 

    MixColumns(state,ROTATION_A_INV,ROTATION_B_INV,ROTATION_C_INV);	
    								 
    ShuffleCells_inv(state);		 

    // second whitening key addition

    Add(state, W);
    Add(state, T);					 

    // The R backward rounds

    for(i = 0; i < R; i++)
    {
        updateTweak_inv(T);

    	SubCell8_inv(state);		 

    	if (i != R-1)
    	{
    		MixColumns(state,ROTATION_A_INV,ROTATION_B_INV,ROTATION_C_INV);	
    									 
    		ShuffleCells_inv(state);	 
    	}
    	Add(state, T);				
    	Add(state, K);				
    	AddAlpha(state); // inverse has + alpha
    	AddConstants(state, R-1-i);	 
    }

    // Final whitening with W_p 

    Add(state, W_p);				 

    for(i = 0; i < 16; i++)
    	output[i] = state[i] & 0xFF;
}

 
// in: core and whitening keys w0 and k0
// out: center core keys k1 and k1_M, second whitening key w1

void Orthomorphism(const cell_t w0[16], cell_t * w1)
{
    cell_t oldtemp;
    cell_t newtemp;
    int i;

    // create w1 by expansion
    // first w0 >>> 1


    oldtemp = 0;
    for (i = 0; i<16; i++)
    {
    	newtemp = w0[i] & 0x1;
    	w1[i]   = w0[i] >>  1;
    	w1[i]   = w1[i] ^ oldtemp;
    	oldtemp = (newtemp << 7) & 0xff;
    }
    w1[0]  ^= oldtemp;

    // then add w0 >> 127

    w1[15] ^= (w0[0] >> 7);
}

void KeySpecialisation( const cell_t k0[16], const  cell_t w0[16],
    cell_t * k1, cell_t * k1_M, cell_t * w1)
{
    int i;
    
    Orthomorphism(w0, w1); 
        for (i = 0; i<16; i++)
                k1[i] = k0[i];
 

 	for (i = 0; i<16; i++)
    	k1_M[i] = k1[i];

    MixColumns(k1_M,ROTATION_A_MID,ROTATION_B_MID,ROTATION_C_MID);	
}

void KeySpecialisation_dec(const  cell_t k0[16],  const cell_t w0[16],
    cell_t * k1, cell_t * k1_M, cell_t * w1, cell_t * w0d,cell_t * w1d, cell_t *k0d)
{
    int i;
 
    Orthomorphism(w0, w1);

        for (i = 0; i<16; i++)
                {          
    	   k1[i] = k0[i];
                   w0d[i]=w1[i];  
                   w1d[i]=w0[i];
                }
 	for (i = 0; i<16; i++)
    	k1_M[i] = k1[i];

    MixColumns(k1_M,ROTATION_A_MID,ROTATION_B_MID,ROTATION_C_MID);	
        for (i = 0; i<16; i++)
            k0d[i]=k0[i] ^ ALPHA[i];

}
