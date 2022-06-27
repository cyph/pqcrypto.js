// Radix.cpp: a fast floating-point radix sort demo
//
//   Copyright (C) Herf Consulting LLC 2001.  All Rights Reserved.
//   Use for anything you want, just tell me what you do with it.
//   Code provided "as-is" with no liabilities for anything that goes wrong.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "crypto_int32.h"
#include "crypto_uint32.h"
#include "crypto_sort.h"

// ------------------------------------------------------------------------------------------------
// ---- Basic types

typedef crypto_int32 int32;
typedef crypto_uint32 uint32;
typedef const char *cpointer;

// ---- use SSE prefetch (needs compiler support), not really a problem on non-SSE machines.
//		need http://msdn.microsoft.com/vstudio/downloads/ppack/default.asp
//		or recent VC to use this

#define PREFETCH 1

#if PREFETCH
#include <xmmintrin.h>	// for prefetch
#define pfval	64
#define pfval2	128
#define pf(x)	_mm_prefetch((cpointer)(x + i + pfval), 0)
#define pf2(x)	_mm_prefetch((cpointer)(x + i + pfval2), 0)
#else
#define pf(x)
#define pf2(x)
#endif

// ---- utils for accessing 11-bit quantities
#define _0(x)	(x & 0x7FF)
#define _1(x)	(x >> 11 & 0x7FF)
#define _2(x)	(x >> 22 )

// ================================================================================================
// Main radix sort
// ================================================================================================
static void RadixSort11(uint32 *array, uint32 *sort, uint32 elements)
{
	uint32 i;

	// 3 histograms on the stack:
	const uint32 kHist = 2048;
	uint32 b0[kHist * 3];

	uint32 *b1 = b0 + kHist;
	uint32 *b2 = b1 + kHist;

	for (i = 0; i < kHist * 3; i++) {
		b0[i] = 0;
	}
	//memset(b0, 0, kHist * 12);

	// 1.  parallel histogramming pass
	//
	for (i = 0; i < elements; i++) {
		
		pf(array);

		uint32 fi = 0x80000000 ^ array[i];

		b0[_0(fi)] ++;
		b1[_1(fi)] ++;
		b2[_2(fi)] ++;
	}
	
	// 2.  Sum the histograms -- each histogram entry records the number of values preceding itself.
	{
		uint32 sum0 = 0, sum1 = 0, sum2 = 0;
		uint32 tsum;
		for (i = 0; i < kHist; i++) {

			tsum = b0[i] + sum0;
			b0[i] = sum0 - 1;
			sum0 = tsum;

			tsum = b1[i] + sum1;
			b1[i] = sum1 - 1;
			sum1 = tsum;

			tsum = b2[i] + sum2;
			b2[i] = sum2 - 1;
			sum2 = tsum;
		}
	}

	// byte 0: flip entire value, read/write histogram, write out flipped
	for (i = 0; i < elements; i++) {

		uint32 fi = array[i];
		fi = 0x80000000 ^ fi;
		uint32 pos = _0(fi);
		
		pf2(array);
		sort[++b0[pos]] = fi;
	}

	// byte 1: read/write histogram, copy
	//   sorted -> array
	for (i = 0; i < elements; i++) {
		uint32 si = sort[i];
		uint32 pos = _1(si);
		pf2(sort);
		array[++b1[pos]] = si;
	}

	// byte 2: read/write histogram, copy & flip out
	//   array -> sorted
	for (i = 0; i < elements; i++) {
		uint32 ai = array[i];
		uint32 pos = _2(ai);

		pf2(array);
		sort[++b2[pos]] = 0x80000000 ^ ai;
	}

	// to write original:
	memcpy(array, sort, elements * 4);
}

void crypto_sort(void *x,long long n)
{
  int32 y[n];
  RadixSort11(x,y,n);
}
