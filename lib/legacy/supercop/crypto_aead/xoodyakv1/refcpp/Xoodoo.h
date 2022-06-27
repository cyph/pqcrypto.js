/*
Implementation by Seth Hoffert, hereby denoted as "the implementer".

For more information, feedback or questions, please refer to our websites:
https://keccak.team/xoodoo.html

To the extent possible under law, the implementer has waived all copyright
and related or neighboring rights to the source code in this file.
http://creativecommons.org/publicdomain/zero/1.0/
*/

#ifndef _XOODOO_H_
#define _XOODOO_H_

#include <algorithm>
#include <iostream>
#include <vector>

#include "transformations.h"

typedef UINT32 Lane;

Lane shiftLane(const Lane &a, int dz);
Lane cyclicShiftLane(const Lane &a, int dz);

class XoodooPlane
{
	private:
		std::vector<Lane> lanes;

	public:
		XoodooPlane();
		XoodooPlane(const Lane *state);
		const Lane &operator[](unsigned int i) const;
		Lane &operator[](unsigned int i);
		void write(Lane *state) const;
};

XoodooPlane cyclicShiftPlane(const XoodooPlane &A, int dx, int dz);
XoodooPlane operator^(const XoodooPlane &lhs, const XoodooPlane &rhs);
XoodooPlane operator&(const XoodooPlane &lhs, const XoodooPlane &rhs);
XoodooPlane operator~(const XoodooPlane &A);

class XoodooState
{
	private:
		std::vector<XoodooPlane> planes;

	public:
		XoodooState();
		XoodooState(const UINT8 *state);
		XoodooPlane &operator[](unsigned int y);
		void write(UINT8 *state) const;
		void dump(std::ostream &os) const;
};

enum XoodooLog
{
	LOG_NONE,
	LOG_ROUND,
	LOG_FINAL,
};

class Xoodoo
{
	private:
		std::vector<Lane> rc_s, rc_p;
		unsigned int rounds;

		XoodooLog logType;
		std::ostream *log;

		void calculateRoundConstants();

		void stepTheta(XoodooState &A) const;
		void stepRhoWest(XoodooState &A) const;
		void stepIota(XoodooState &A, int i) const;
		void stepChi(XoodooState &A) const;
		void stepRhoEast(XoodooState &A) const;

		void round(XoodooState &A, int i) const;
		void permute(XoodooState &A) const;

	public:
		Xoodoo(unsigned int width, unsigned int vrounds);
		void operator()(UINT8 *state) const;

		void unsetLog();
		void setLog(XoodooLog type, std::ostream &os);
};

#endif
