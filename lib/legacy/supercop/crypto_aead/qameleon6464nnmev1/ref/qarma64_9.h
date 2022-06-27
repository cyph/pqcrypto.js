/**
 * qarma64_9.h
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

#define ROTATION_A      1
#define ROTATION_B      2
#define ROTATION_C      1

#define ROTATION_A_INV  1
#define ROTATION_B_INV  2
#define ROTATION_C_INV  1

#define ROTATION_A_MID  1
#define ROTATION_B_MID  2
#define ROTATION_C_MID  1

typedef unsigned char cell_t;
typedef unsigned char u8;
typedef unsigned long long u64;

/* In the internal state matrix the data is stored as follows */
/* 0  1  2  3   */
/* 4  5  6  7   */
/* 8  9 10 11   */
/* 12 13 14 15   */

/* Definition of the 4 - bit Sbox(es) */
const cell_t	sigma_1[16] = {10, 13, 14, 6, 15, 7, 3, 5, 9, 8, 0, 12, 11, 1, 2, 4};
const cell_t	sigma_1_inv[16] = {10, 13, 14, 6, 15, 7, 3, 5, 9, 8, 0, 12, 11, 1, 2, 4};
const cell_t   *sbox_4 = sigma_1;
const cell_t   *sbox_4_inv = sigma_1_inv;

/* ShuffleCells permutation (we use the INVERSE MIDORI permutation) */
/* 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 */
const int	SHUFFLE_P[16] = {0, 11, 6, 13, 10, 1, 12, 7, 5, 14, 3, 8, 15, 4, 9, 2};
const int	SHUFFLE_P_inv[16] = {0, 5, 15, 10, 13, 8, 2, 7, 11, 14, 4, 1, 6, 3, 9, 12};
/* 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 */

/* INVERSE Tweak permutation */

const int	TWEAKEY_P[16] = {6, 5, 14, 15, 0, 1, 2, 3, 7, 12, 13, 4, 8, 9, 10, 11};
const int	TWEAKEY_P_inv[16] = {4, 5, 6, 7, 11, 1, 0, 8, 12, 13, 14, 15, 9, 10, 2, 3};

const cell_t	RC [16][16] = {
	{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	{0x1, 0x3, 0x1, 0x9, 0x8, 0xA, 0x2, 0xE, 0x0, 0x3, 0x7, 0x0, 0x7, 0x3, 0x4, 0x4},
	{0xA, 0x4, 0x0, 0x9, 0x3, 0x8, 0x2, 0x2, 0x2, 0x9, 0x9, 0xF, 0x3, 0x1, 0xD, 0x0},
	{0x0, 0x8, 0x2, 0xE, 0xF, 0xA, 0x9, 0x8, 0xE, 0xC, 0x4, 0xE, 0x6, 0xC, 0x8, 0x9},
	{0x4, 0x5, 0x2, 0x8, 0x2, 0x1, 0xE, 0x6, 0x3, 0x8, 0xD, 0x0, 0x1, 0x3, 0x7, 0x7},
	{0xB, 0xE, 0x5, 0x4, 0x6, 0x6, 0xC, 0xF, 0x3, 0x4, 0xE, 0x9, 0x0, 0xC, 0x6, 0xC},
	{0x3, 0xF, 0x8, 0x4, 0xD, 0x5, 0xB, 0x5, 0xB, 0x5, 0x4, 0x7, 0x0, 0x9, 0x1, 0x7},
	{0x9, 0x2, 0x1, 0x6, 0xD, 0x5, 0xD, 0x9, 0x8, 0x9, 0x7, 0x9, 0xF, 0xB, 0x1, 0xB},
	{0xD, 0x1, 0x3, 0x1, 0x0, 0xB, 0xA, 0x6, 0x9, 0x8, 0xD, 0xF, 0xB, 0x5, 0xA, 0xC},
	{0x2, 0xF, 0xF, 0xD, 0x7, 0x2, 0xD, 0xB, 0xD, 0x0, 0x1, 0xA, 0xD, 0xF, 0xB, 0x7},
	{0xB, 0x8, 0xE, 0x1, 0xA, 0xF, 0xE, 0xD, 0x6, 0xA, 0x2, 0x6, 0x7, 0xE, 0x9, 0x6},
	{0xB, 0xA, 0x7, 0xC, 0x9, 0x0, 0x4, 0x5, 0xF, 0x1, 0x2, 0xC, 0x7, 0xF, 0x9, 0x9},
	{0x2, 0x4, 0xA, 0x1, 0x9, 0x9, 0x4, 0x7, 0xB, 0x3, 0x9, 0x1, 0x6, 0xC, 0xF, 0x7},
	{0x0, 0x8, 0x0, 0x1, 0xF, 0x2, 0xE, 0x2, 0x8, 0x5, 0x8, 0xE, 0xF, 0xC, 0x1, 0x6},
	{0x6, 0x3, 0x6, 0x9, 0x2, 0x0, 0xD, 0x8, 0x7, 0x1, 0x5, 0x7, 0x4, 0xE, 0x6, 0x9},
	{0xA, 0x4, 0x5, 0x8, 0xF, 0xE, 0xA, 0x3, 0xF, 0x4, 0x9, 0x3, 0x3, 0xD, 0x7, 0xE}
};

const cell_t	ALPHA[16] =
{0xC, 0x0, 0xA, 0xC, 0x2, 0x9, 0xB, 0x7, 0xC, 0x9, 0x7, 0xC, 0x5, 0x0, 0xD, 0xD};

/* XOR Cells to the state */

void
Add(cell_t state[16], const cell_t Cells[16])
{
	for (int i = 0; i < 16; i++)
		state[i] ^= Cells[i];
}

void
apply_perm(cell_t cells[16], const int permutation[16])
{
	int		i;
	cell_t		tmp     [16];

	//Apply the TWEAK permutation, store in temporary array

		for (i = 0; i < 16; i++)
		tmp[i] = cells[permutation[i]];

	//copy back, destructing input

		for (i = 0; i < 16; i++)
		cells[i] = tmp[i];
}

//Apply the tweak update, its inverse

/* maps(b_3, b_2, b_1, b_0) to(b_0 + b_1, b_3, b_2, b_1) */
int
lfsr(int v)
{
	return (v >> 1) ^ (((v & 1) ^ ((v >> 1) & 1)) << 3);
}

/* maps(b_4 = b_0 + b_1, b_3, b_2, b_1) to(b_3, b_2, b_1, b_1 + b_4) */
int
lfsr_inv(int v)
{
	return ((v << 1) & 0xF) ^ ((v & 1) ^ ((v >> 3) & 1));
}

void
updateTweak(cell_t cells[16])
{
	apply_perm(cells, TWEAKEY_P);

	cells[0] = lfsr(cells[0]);
	cells[1] = lfsr(cells[1]);
	cells[3] = lfsr(cells[3]);
	cells[4] = lfsr(cells[4]);
	cells[8] = lfsr(cells[8]);
	cells[11] = lfsr(cells[11]);
	cells[13] = lfsr(cells[13]);
}

void
updateTweak_inv(cell_t cells[16])
{
	cells[0] = lfsr_inv(cells[0]);
	cells[1] = lfsr_inv(cells[1]);
	cells[3] = lfsr_inv(cells[3]);
	cells[4] = lfsr_inv(cells[4]);
	cells[8] = lfsr_inv(cells[8]);
	cells[11] = lfsr_inv(cells[11]);
	cells[13] = lfsr_inv(cells[13]);

	apply_perm(cells, TWEAKEY_P_inv);
}

//Apply the ShuffleCells function, its inverse

void
ShuffleCells(cell_t cells[16])
{
	apply_perm(cells, SHUFFLE_P);
}

void
ShuffleCells_inv(cell_t cells[16])
{
	apply_perm(cells, SHUFFLE_P_inv);
}

//Add a round constant

void
AddConstants(cell_t cells[16], int r)
{
	for (int i = 0; i < 16; i++)
		cells[i] ^= RC[r][i];
}

//Apply alpha to cells state

void
AddAlpha(cell_t cells[16])
{
	for (int i = 0; i < 16; i++)
		cells[i] ^= ALPHA[i];
}

//Apply the 4 - bit Sbox

void
SubCell4(cell_t cells[16])
{
	for (int i = 0; i < 16; i++)
		cells[i] = sbox_4[cells[i]];
}

//Apply the 4 - bit inverse Sbox

void
SubCell4_inv(cell_t cells[16])
{
	for (int i = 0; i < 16; i++)
		cells[i] = sbox_4_inv[cells[i]];
}

/*
 * Apply the linear diffusion matrix
 */

cell_t
RotCell(cell_t val, int amount)
{
	return (((val << amount) | (val >> (4 - amount))) & 0xF);
}

void
MixColumns(cell_t state[16], int a, int b, int c)
{
	int		j;
	cell_t		temp0  , temp1, temp2, temp3;

	for (j = 0; j < 4; j++)
		/* for each column, that has 0, 1, 2, 3 at the top */
	{
		temp0 = RotCell(state[j + 4], a) ^ RotCell(state[j + 8], b) ^ RotCell(state[j + 12], c);
		temp1 = RotCell(state[j], c) ^ RotCell(state[j + 8], a) ^ RotCell(state[j + 12], b);
		temp2 = RotCell(state[j], b) ^ RotCell(state[j + 4], c) ^ RotCell(state[j + 12], a);
		temp3 = RotCell(state[j], a) ^ RotCell(state[j + 4], b) ^ RotCell(state[j + 8], c);

		state[j] = temp0;
		state[j + 4] = temp1;
		state[j + 8] = temp2;
		state[j + 12] = temp3;
	}
}

void
qarma64(const cell_t * input,
	const cell_t W[16], const cell_t W_p[16],
	const cell_t core_key[16], const cell_t middle_key[16],
	const cell_t tweak[16],
	int R, cell_t * output)
{
	cell_t		state   [16];
	cell_t		T       [16];
	cell_t		K       [16];

	int		i;

	for (i = 0; i < 16; i++) {
		state[i] = input[i];
		T[i] = tweak[i];
		K[i] = core_key[i];
	}

	//Initial whitening qarma64(c, w0, w1, k0, k1, t, r);

	Add(state, W);

	//The R forward rounds

		for (i = 0; i < R; i++) {
		//A round is Add(constant + key + tweak), then S - Box, MixColumns, and Shuffle

			AddConstants(state, i);
		Add(state, K);
		Add(state, T);
		if (i != 0) {
			ShuffleCells(state);
			MixColumns(state, ROTATION_A, ROTATION_B, ROTATION_C);
		}
		SubCell4(state);
		updateTweak(T);
	}

	/* The pseudo - reflector */

	/* first whitening key addition */

	Add(state, W_p);
	Add(state, T);

	/* full forward diffusion layer */

	ShuffleCells(state);
	MixColumns(state, ROTATION_A, ROTATION_B, ROTATION_C);

	SubCell4(state);

	/* bridge */

	ShuffleCells(state);

	MixColumns(state, ROTATION_A_MID, ROTATION_B_MID, ROTATION_C_MID);
	Add(state, middle_key);

	ShuffleCells_inv(state);

	/* backward round with whitening key in place of core key */

	SubCell4_inv(state);

	MixColumns(state, ROTATION_A_INV, ROTATION_B_INV, ROTATION_C_INV);

	ShuffleCells_inv(state);

	/* second whitening key addition */

	Add(state, W);
	Add(state, T);

	/* The R backward rounds */

	for (i = 0; i < R; i++) {
		updateTweak_inv(T);

		SubCell4_inv(state);

		if (i != R - 1) {
			MixColumns(state, ROTATION_A_INV, ROTATION_B_INV, ROTATION_C_INV);

			ShuffleCells_inv(state);
		}
		Add(state, T);
		Add(state, K);
		AddAlpha(state);
		/* inverse has + alpha */
		AddConstants(state, R - 1 - i);
	}

	/* Final whitening with W_p */

	Add(state, W_p);

	for (i = 0; i < 16; i++)
		output[i] = state[i] & 0x0F;
}

void
Orthomorphism(const cell_t w0[16], cell_t * w1)
{
	cell_t		oldtemp;
	cell_t		newtemp;
	int		i;

	/* create w1 by expansion */
	/* first w0 >>> 1 */

	oldtemp = 0;
	for (i = 0; i < 16; i++) {
		newtemp = w0[i] & 0x1;
		w1[i] = (w0[i] >> 1) & 0xf;
		w1[i] = w1[i] ^ oldtemp;
		oldtemp = (newtemp << 3);
	}
	w1[0] ^= oldtemp;

	/* then add w0 >> 63 */

	w1[15] ^= (w0[0] >> 3);
}

void
KeySpecialisation(const cell_t k0[16], const cell_t w0[16],
		  cell_t * k1, cell_t * k1_M, cell_t * w1)
{
	int		i;

	Orthomorphism(w0, w1);

	for (i = 0; i < 16; i++)
		k1[i] = k0[i];

	for (i = 0; i < 16; i++)
		k1_M[i] = k1[i];

	MixColumns(k1_M, ROTATION_A_MID, ROTATION_B_MID, ROTATION_C_MID);
}

void
KeySpecialisation_dec(const cell_t k0[16], const cell_t w0[16],
		      cell_t * k1, cell_t * k1_M, cell_t * w1, cell_t * w0d, cell_t * w1d, cell_t * k0d)
{
	int		i;

	Orthomorphism(w0, w1);

	for (i = 0; i < 16; i++) {
		k1[i] = k0[i];
		w0d[i] = w1[i];
		w1d[i] = w0[i];
	}
	for (i = 0; i < 16; i++)
		k1_M[i] = k1[i];

	MixColumns(k1_M, ROTATION_A_MID, ROTATION_B_MID, ROTATION_C_MID);
	for (i = 0; i < 16; i++)
		k0d[i] = k0[i] ^ ALPHA[i];

}
