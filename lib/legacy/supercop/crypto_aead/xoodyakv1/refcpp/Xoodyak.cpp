/*
Implementation by Seth Hoffert, hereby denoted as "the implementer".

For more information, feedback or questions, please refer to our websites:
https://keccak.team/xoodoo.html

To the extent possible under law, the implementer has waived all copyright
and related or neighboring rights to the source code in this file.
http://creativecommons.org/publicdomain/zero/1.0/
*/

#include "Xoodyak.h"

/* Xoodyak instantiation parameters */
namespace XoodyakParams
{
	IterableTransformation<Xoodoo>   f(384, 12);

	unsigned int                     param_Rhash = 16;
	unsigned int                     param_Rkin = 44;
	unsigned int                     param_Rkout = 24;
	unsigned int                     param_lratchet = 16;
};

/* Xoodyak */
Xoodyak::Xoodyak(const BitString &K, const BitString &id, const BitString &counter)
	: Cyclist(XoodyakParams::f, XoodyakParams::param_Rhash, XoodyakParams::param_Rkin, XoodyakParams::param_Rkout, XoodyakParams::param_lratchet,
			  K, id, counter)
{
}
