/*
Implementation by Seth Hoffert, hereby denoted as "the implementer".

For more information, feedback or questions, please refer to our websites:
https://keccak.team/xoodoo.html

To the extent possible under law, the implementer has waived all copyright
and related or neighboring rights to the source code in this file.
http://creativecommons.org/publicdomain/zero/1.0/
*/

#ifndef _XOODYAK_H_
#define _XOODYAK_H_

#include <iostream>
#include <memory>

#include "bitstring.h"
#include "Cyclist.h"
#include "transformations.h"
#include "types.h"
#include "Xoodoo.h"

class Xoodyak : public Cyclist
{
	public:
		Xoodyak(const BitString &K, const BitString &id, const BitString &counter);
};

#endif
