/*
Implementation by Seth Hoffert, hereby denoted as "the implementer".

For more information, feedback or questions, please refer to our websites:
https://keccak.team/xoodoo.html

To the extent possible under law, the implementer has waived all copyright
and related or neighboring rights to the source code in this file.
http://creativecommons.org/publicdomain/zero/1.0/
*/

#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "Xoodoo.h"

static const int RowSize = 4;
static const int ColSize = 3;
static const int LaneSize = 32;

static void reduceX(int &x)
{
	x = ((x % RowSize) + RowSize) % RowSize;
}

static void reduceZ(int &z)
{
	z = ((z % LaneSize) + LaneSize) % LaneSize;
}

Lane shiftLane(const Lane &a, int dz)
{
	reduceZ(dz);
	return a << dz;
}

Lane cyclicShiftLane(const Lane &a, int dz)
{
	reduceZ(dz);

	if (dz == 0)
	{
		return a;
	}
	else
	{
		return ((a << dz) | (a >> (LaneSize - dz)));
	}
}

XoodooPlane::XoodooPlane() : lanes(RowSize)
{
}

XoodooPlane::XoodooPlane(const Lane *state)
{
	lanes.assign(state, state + RowSize);
}

const Lane &XoodooPlane::operator[](unsigned int i) const
{
	return lanes[i];
}

Lane &XoodooPlane::operator[](unsigned int i)
{
	return lanes[i];
}

void XoodooPlane::write(Lane *state) const
{
	std::copy(lanes.begin(), lanes.end(), state);
}

XoodooPlane cyclicShiftPlane(const XoodooPlane &A, int dx, int dz)
{
	XoodooPlane p;

	for (unsigned int i = 0; i < RowSize; i++)
	{
		int index = i - dx;
		reduceX(index);
		p[i] = cyclicShiftLane(A[index], dz);
	}

	return p;
}

XoodooPlane operator^(const XoodooPlane &lhs, const XoodooPlane &rhs)
{
	XoodooPlane p;

	for (unsigned int i = 0; i < RowSize; i++)
	{
		p[i] = lhs[i] ^ rhs[i];
	}

	return p;
}

XoodooPlane operator&(const XoodooPlane &lhs, const XoodooPlane &rhs)
{
	XoodooPlane p;

	for (unsigned int i = 0; i < RowSize; i++)
	{
		p[i] = lhs[i] & rhs[i];
	}

	return p;
}

XoodooPlane operator~(const XoodooPlane &A)
{
	XoodooPlane p;

	for (unsigned int i = 0; i < RowSize; i++)
	{
		p[i] = ~A[i];
	}

	return p;
}

XoodooState::XoodooState() : planes(ColSize)
{
}

XoodooState::XoodooState(const UINT8 *state)
{
	const Lane *l = reinterpret_cast<const Lane *>(state);

	for (unsigned int i = 0; i < ColSize; i++)
	{
		planes.push_back(XoodooPlane(l + i * RowSize));
	}
}

XoodooPlane &XoodooState::operator[](unsigned int y)
{
	return planes[y];
}

void XoodooState::write(UINT8 *state) const
{
	Lane *l = reinterpret_cast<Lane *>(state);

	for (unsigned int i = 0; i < ColSize; i++)
	{
		planes[i].write(l + i * RowSize);
	}
}

void XoodooState::dump(std::ostream &os) const
{
	std::ios::fmtflags f(os.flags());

	os << std::hex << std::noshowbase << std::setfill('0');

	for (unsigned int y = 0; y < ColSize; y++)
	{
		for (unsigned int x = 0; x < RowSize; x++)
		{
			os << "0x" << std::setw(8) << planes[y][x] << ' ';
		}
	}

	os << '\n';
	os.flags(f);
}

void Xoodoo::calculateRoundConstants()
{
	Lane s = 1;
	Lane p = 1;

	for (unsigned int i = 0; i < 6; i++)
	{
		rc_s.push_back(s);
		s = (s * 5) % 7;
	}

	for (unsigned int i = 0; i < 7; i++)
	{
		rc_p.push_back(p);
		p = p ^ (p << 2);
		if ((p & 16) != 0) p ^= 22;
		if ((p &  8) != 0) p ^= 11;
	}
}

void Xoodoo::stepTheta(XoodooState &A) const
{
	XoodooPlane P = A[0] ^ A[1] ^ A[2];
	XoodooPlane E = cyclicShiftPlane(P, 1, 5) ^ cyclicShiftPlane(P, 1, 14);

	for (unsigned int i = 0; i < ColSize; i++)
	{
		A[i] = A[i] ^ E;
	}
}

void Xoodoo::stepRhoWest(XoodooState &A) const
{
	A[1] = cyclicShiftPlane(A[1], 1, 0);
	A[2] = cyclicShiftPlane(A[2], 0, 11);
}

void Xoodoo::stepIota(XoodooState &A, int i) const
{
	Lane rc = (rc_p[-i % 7] ^ 8) << rc_s[-i % 6];
	A[0][0] = A[0][0] ^ rc;
}

void Xoodoo::stepChi(XoodooState &A) const
{
	XoodooState B;
	B[0] = ~A[1] & A[2];
	B[1] = ~A[2] & A[0];
	B[2] = ~A[0] & A[1];

	for (unsigned int i = 0; i < ColSize; i++)
	{
		A[i] = A[i] ^ B[i];
	}
}

void Xoodoo::stepRhoEast(XoodooState &A) const
{
	A[1] = cyclicShiftPlane(A[1], 0, 1);
	A[2] = cyclicShiftPlane(A[2], 2, 8);
}

void Xoodoo::round(XoodooState &A, int i) const
{
	stepTheta(A);
	stepRhoWest(A);
	stepIota(A, i);
	stepChi(A);
	stepRhoEast(A);
}

void Xoodoo::permute(XoodooState &A) const
{
	for (int i = 1 - rounds; i <= 0; i++)
	{
		round(A, i);

		if (logType == LOG_ROUND)
		{
			std::ios::fmtflags f(log->flags());
			*log << "(Round " << std::setfill('0') << std::setw(2) << (i + rounds) << ") ";
			log->flags(f);
			A.dump(*log);
		}
	}

	if (logType == LOG_FINAL)
	{
		A.dump(*log);
	}
}

Xoodoo::Xoodoo(unsigned int width, unsigned int vrounds)
	: rounds(vrounds), logType(LOG_NONE), log(NULL)
{
	if (width != 384) throw Exception("Unsupported width");
	if (rounds > 42) throw Exception("Unsupported number of rounds");

	calculateRoundConstants();
}

void Xoodoo::operator()(UINT8 *state) const
{
	XoodooState A(state);
	permute(A);
	A.write(state);
}

void Xoodoo::unsetLog()
{
	logType = LOG_NONE;
	log = NULL;
}

void Xoodoo::setLog(XoodooLog type, std::ostream &os)
{
	logType = type;
	log = &os;
}
