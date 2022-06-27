/*
Implementation by Seth Hoffert, hereby denoted as "the implementer".

For more information, feedback or questions, please refer to our websites:
https://keccak.team/xoodoo.html

To the extent possible under law, the implementer has waived all copyright
and related or neighboring rights to the source code in this file.
http://creativecommons.org/publicdomain/zero/1.0/
*/

#ifndef _CYCLIST_H_
#define _CYCLIST_H_

#include <iostream>
#include <memory>

#include "bitstring.h"
#include "transformations.h"
#include "types.h"

enum CyclistConstant
{
	CONSTANT_ZERO = 0x00,
	CONSTANT_ABSORB_KEY = 0x02,
	CONSTANT_ABSORB = 0x03,
	CONSTANT_RATCHET = 0x10,
	CONSTANT_SQUEEZE_KEY = 0x20,
	CONSTANT_SQUEEZE = 0x40,
	CONSTANT_CRYPT = 0x80,
};

enum CyclistPhase
{
	PHASE_UP,
	PHASE_DOWN,
};

enum CyclistMode
{
	MODE_HASH,
	MODE_KEYED,
};

/**
 * Class implementing the Cyclist construction
 */
class Cyclist
{
	protected:
		BaseIterableTransformation &f;
		unsigned int               fbp, Rkin, Rkout, lratchet;
		CyclistPhase               phase;
		BitString                  s;
		CyclistMode                mode;
		unsigned int               Rabsorb, Rsqueeze;

		void AbsorbAny(const BitString &X, unsigned int r, UINT8 cD);
		void AbsorbKey(const BitString &K, const BitString &id, const BitString &counter);
		BitString Crypt(const BitString &I, bool decrypt);
		BitString SqueezeAny(unsigned int l, UINT8 cU);
		void Down(const BitString &Xi, UINT8 cD);
		BitString Up(unsigned int Yi, UINT8 cU);
		Blocks Split(const BitString &X, unsigned int n);

	public:
		Cyclist(BaseIterableTransformation &f, unsigned int Rhash, unsigned int Rkin, unsigned int Rkout, unsigned int lratchet,
			    const BitString &K, const BitString &id, const BitString &counter);
		void Absorb(const BitString &X);
		BitString Encrypt(const BitString &P);
		BitString Decrypt(const BitString &C);
		BitString Squeeze(unsigned int l);
		BitString SqueezeKey(unsigned int l);
		void Ratchet();
};

#endif
