/*
Implementation by Seth Hoffert, hereby denoted as "the implementer".

For more information, feedback or questions, please refer to our websites:
https://keccak.team/xoodoo.html

To the extent possible under law, the implementer has waived all copyright
and related or neighboring rights to the source code in this file.
http://creativecommons.org/publicdomain/zero/1.0/
*/

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

#include "Cyclist.h"

static void ref_assert(bool condition, const std::string &synopsis, const char *fct)
{
	if (!condition)
	{
		throw Exception((std::string(fct) + "(): " + synopsis).data());
	}
}

#undef assert

#if defined(__GNUC__)
#define assert(cond, msg)  ref_assert(cond, msg, __PRETTY_FUNCTION__)
#else
#define assert(cond, msg)  ref_assert(cond, msg, __FUNCTION__)
#endif

/* Cyclist */
Cyclist::Cyclist(BaseIterableTransformation &f,
                 unsigned int Rhash,
                 unsigned int Rkin,
                 unsigned int Rkout,
                 unsigned int lratchet,
                 const BitString &K,
                 const BitString &id,
                 const BitString &counter)
	: f(f), Rkin(Rkin), Rkout(Rkout), lratchet(lratchet)
{
	assert((f.width % 8) == 0,
		"This implementation only supports permutation width that are multiple of 8."); // Limitation of Transformation class

	fbp = f.width / 8;
	phase = PHASE_UP;
	s = BitString::zeroes(8 * fbp);
	mode = MODE_HASH;
	Rabsorb = Rhash;
	Rsqueeze = Rhash;
	if (K.size() != 0) AbsorbKey(K, id, counter);
}

void Cyclist::Absorb(const BitString &X)
{
	AbsorbAny(X, Rabsorb, CONSTANT_ABSORB);
}

BitString Cyclist::Encrypt(const BitString &P)
{
	assert(mode == MODE_KEYED, "Mode must be 'keyed'");

	return Crypt(P, false);
}

BitString Cyclist::Decrypt(const BitString &C)
{
	assert(mode == MODE_KEYED, "Mode must be 'keyed'");

	return Crypt(C, true);
}

BitString Cyclist::Squeeze(unsigned int l)
{
	return SqueezeAny(l, CONSTANT_SQUEEZE);
}

BitString Cyclist::SqueezeKey(unsigned int l)
{
	assert(mode == MODE_KEYED, "Mode must be 'keyed'");

	return SqueezeAny(l, CONSTANT_SQUEEZE_KEY);
}

void Cyclist::Ratchet()
{
	assert(mode == MODE_KEYED, "Mode must be 'keyed'");

	AbsorbAny(SqueezeAny(lratchet, CONSTANT_RATCHET), Rabsorb, CONSTANT_ZERO);
}

void Cyclist::AbsorbAny(const BitString &X, unsigned int r, UINT8 cD)
{
	Blocks Xb = Split(X, r);

	for (unsigned int i = 0; i < Xb.size(); i++)
	{
		if (phase != PHASE_UP) Up(0, CONSTANT_ZERO);
		Down(Xb[i], (i == 0) ? cD : CONSTANT_ZERO);
	}
}

void Cyclist::AbsorbKey(const BitString &K, const BitString &id, const BitString &counter)
{
	assert(K.size() + id.size() <= 8 * Rkin - 1, "|K || id| must be <= R_kin - 1 bytes");

	mode = MODE_KEYED;
	Rabsorb = Rkin;
	Rsqueeze = Rkout;

	if (K.size() != 0)
	{
		AbsorbAny(K || id || BitString(8, (UINT8)(id.size() / 8)), Rabsorb, CONSTANT_ABSORB_KEY);
		if (counter.size() != 0) AbsorbAny(counter, 1, CONSTANT_ZERO);
	}
}

BitString Cyclist::Crypt(const BitString &I, bool decrypt)
{
	Blocks Ib = Split(I, Rkout);
	Blocks Ob(8 * Rkout);

	for (unsigned int i = 0; i < Ib.size(); i++)
	{
		Ob[i] = Ib[i] ^ Up(Ib[i].size() / 8, (i == 0) ? CONSTANT_CRYPT : CONSTANT_ZERO);
		Block Pi = decrypt ? Ob[i] : Ib[i];
		Down(Pi, CONSTANT_ZERO);
	}

	return Ob.bits();
}

BitString Cyclist::SqueezeAny(unsigned int l, UINT8 cU)
{
	BitString Y = Up(std::min(l, Rsqueeze), cU);

	while (Y.size() / 8 < l)
	{
		Down(BitString(), CONSTANT_ZERO);
		Y = Y || Up(std::min(l - Y.size() / 8, Rsqueeze), CONSTANT_ZERO);
	}

	return Y;
}

void Cyclist::Down(const BitString &Xi, UINT8 cD)
{
	phase = PHASE_DOWN;
	s = s ^ (Xi || BitString(8, 0x01) || BitString::zeroes(8 * (fbp - 2) - Xi.size()) || BitString(8, (mode == MODE_HASH) ? (cD & 0x01) : cD));
}

BitString Cyclist::Up(unsigned int Yi, UINT8 cU)
{
	phase = PHASE_UP;
	s = f((mode == MODE_HASH) ? s : (s ^ (BitString::zeroes(8 * (fbp - 1)) || BitString(8, cU))));
	return BitString::substring(s, 0, 8 * Yi);
}

Blocks Cyclist::Split(const BitString &X, unsigned int n)
{
	return Blocks(X, 8 * n);
}
