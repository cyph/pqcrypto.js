#include "benes.h"

#include "transpose.h"

static void layer128_x(vec128 *bs, vec128 *cond)
{
	int x;
	vec128 v0, v1;  
	vec128 diff;  

	for (x = 0; x < (1 << 6); x += 2)
	{
		v0 = vec128_unpack_low(bs[x+0], bs[x+1]);
		v1 = vec128_unpack_high(bs[x+0], bs[x+1]);

		diff = vec128_xor(v0, v1);
		diff = vec128_and(diff, *cond++);
		v0 = vec128_xor(v0, diff);			
		v1 = vec128_xor(v1, diff);			

		bs[x+0] = vec128_unpack_low(v0, v1);
		bs[x+1] = vec128_unpack_high(v0, v1);
	}
}

static void layer128_0(vec128 *bs, vec128 *cond)
{
	int x;
	vec128 diff;  

	for (x = 0; x < (1 << 6); x += 2)
	{
		diff = vec128_xor(bs[ x ], bs[ x+1 ]);
		diff = vec128_and(diff, *cond++);
		bs[ x ] = vec128_xor(bs[ x ], diff);			
		bs[ x+1 ] = vec128_xor(bs[ x+1 ], diff);			
	}
}

static void layer128_1(vec128 *bs, vec128 *cond)
{
	int x;
	vec128 diff;  

	for (x = 0; x < (1 << 6); x += 4)
	{
		diff = vec128_xor(bs[ x+0 ], bs[ x+2 ]); 
		diff = vec128_and(diff, cond[0]);
		bs[ x+0 ] = vec128_xor(bs[ x+0 ], diff);			
		bs[ x+2 ] = vec128_xor(bs[ x+2 ], diff);			

		diff = vec128_xor(bs[ x+1 ], bs[ x+3 ]); 
		diff = vec128_and(diff, cond[1]);
		bs[ x+1 ] = vec128_xor(bs[ x+1 ], diff);			
		bs[ x+3 ] = vec128_xor(bs[ x+3 ], diff);			

		cond += 2;
	}
}

static void layer128_2(vec128 *bs, vec128 *cond)
{
	int x;
	vec128 diff;  

	for (x = 0; x < (1 << 6); x += 8)
	{
		diff = vec128_xor(bs[ x+0 ], bs[ x+4 ]); 
		diff = vec128_and(diff, cond[0]);
		bs[ x+0 ] = vec128_xor(bs[ x+0 ], diff);			
		bs[ x+4 ] = vec128_xor(bs[ x+4 ], diff);			

		diff = vec128_xor(bs[ x+1 ], bs[ x+5 ]); 
		diff = vec128_and(diff, cond[1]);
		bs[ x+1 ] = vec128_xor(bs[ x+1 ], diff);			
		bs[ x+5 ] = vec128_xor(bs[ x+5 ], diff);			

		diff = vec128_xor(bs[ x+2 ], bs[ x+6 ]); 
		diff = vec128_and(diff, cond[2]);
		bs[ x+2 ] = vec128_xor(bs[ x+2 ], diff);			
		bs[ x+6 ] = vec128_xor(bs[ x+6 ], diff);			

		diff = vec128_xor(bs[ x+3 ], bs[ x+7 ]); 
		diff = vec128_and(diff, cond[3]);
		bs[ x+3 ] = vec128_xor(bs[ x+3 ], diff);			
		bs[ x+7 ] = vec128_xor(bs[ x+7 ], diff);			

		cond += 4;
	}
}

static void layer128_3(vec128 *bs, vec128 *cond)
{
	int x, s;
	vec128 diff;  

	for (x = 0; x < (1 << 6); x += 16)
	for (s = x; s < x + 8; s += 4)
	{
		diff = vec128_xor(bs[ s+0 ], bs[ s+8 ]); 
		diff = vec128_and(diff, cond[0]);
		bs[ s+0 ] = vec128_xor(bs[ s+0 ], diff);			
		bs[ s+8 ] = vec128_xor(bs[ s+8 ], diff);			

		diff = vec128_xor(bs[ s+1 ], bs[ s+9 ]); 
		diff = vec128_and(diff, cond[1]);
		bs[ s+1 ] = vec128_xor(bs[ s+1 ], diff);			
		bs[ s+9 ] = vec128_xor(bs[ s+9 ], diff);			

		diff = vec128_xor(bs[ s+2 ], bs[ s+10 ]); 
		diff = vec128_and(diff, cond[2]);
		bs[ s+2 ] = vec128_xor(bs[ s+2 ], diff);			
		bs[ s+10 ] = vec128_xor(bs[ s+10 ], diff);			

		diff = vec128_xor(bs[ s+3 ], bs[ s+11 ]); 
		diff = vec128_and(diff, cond[3]);
		bs[ s+3 ] = vec128_xor(bs[ s+3 ], diff);			
		bs[ s+11 ] = vec128_xor(bs[ s+11 ], diff);			

		cond += 4;
	}
}

static void layer128_4(vec128 *bs, vec128 *cond)
{
	int x, s;
	vec128 diff;  

	for (x = 0; x < (1 << 6); x += 32)
	for (s = x; s < x + 16; s += 4)
	{
		diff = vec128_xor(bs[ s+0 ], bs[ s+16 ]); 
		diff = vec128_and(diff, cond[0]);
		bs[ s+0 ] = vec128_xor(bs[ s+0 ], diff);			
		bs[ s+16 ] = vec128_xor(bs[ s+16 ], diff);			

		diff = vec128_xor(bs[ s+1 ], bs[ s+17 ]); 
		diff = vec128_and(diff, cond[1]);
		bs[ s+1 ] = vec128_xor(bs[ s+1 ], diff);			
		bs[ s+17 ] = vec128_xor(bs[ s+17 ], diff);			

		diff = vec128_xor(bs[ s+2 ], bs[ s+18 ]); 
		diff = vec128_and(diff, cond[2]);
		bs[ s+2 ] = vec128_xor(bs[ s+2 ], diff);			
		bs[ s+18 ] = vec128_xor(bs[ s+18 ], diff);			

		diff = vec128_xor(bs[ s+3 ], bs[ s+19 ]); 
		diff = vec128_and(diff, cond[3]);
		bs[ s+3 ] = vec128_xor(bs[ s+3 ], diff);			
		bs[ s+19 ] = vec128_xor(bs[ s+19 ], diff);			

		cond += 4;
	}
}

static void layer128_5(vec128 *bs, vec128 *cond)
{
	int x, s;
	vec128 diff;  

	for (x = 0; x < (1 << 6); x += 64)
	for (s = x; s < x + 32; s += 4)
	{
		diff = vec128_xor(bs[ s+0 ], bs[ s+32 ]); 
		diff = vec128_and(diff, cond[0]);
		bs[ s+0 ] = vec128_xor(bs[ s+0 ], diff);			
		bs[ s+32 ] = vec128_xor(bs[ s+32 ], diff);			

		diff = vec128_xor(bs[ s+1 ], bs[ s+33 ]); 
		diff = vec128_and(diff, cond[1]);
		bs[ s+1 ] = vec128_xor(bs[ s+1 ], diff);			
		bs[ s+33 ] = vec128_xor(bs[ s+33 ], diff);			

		diff = vec128_xor(bs[ s+2 ], bs[ s+34 ]); 
		diff = vec128_and(diff, cond[2]);
		bs[ s+2 ] = vec128_xor(bs[ s+2 ], diff);			
		bs[ s+34 ] = vec128_xor(bs[ s+34 ], diff);			

		diff = vec128_xor(bs[ s+3 ], bs[ s+35 ]); 
		diff = vec128_and(diff, cond[3]);
		bs[ s+3 ] = vec128_xor(bs[ s+3 ], diff);			
		bs[ s+35 ] = vec128_xor(bs[ s+35 ], diff);			

		cond += 4;
	}
}

void benes(vec128 *bs, vec128 *cond, int rev)
{
	int inc;

	//

	if (rev == 0) { inc =  32; cond = &cond[  0]; }
	else          { inc = -32; cond = &cond[768]; }
	
	//

	layer128_5(bs, cond); cond += inc;
	layer128_4(bs, cond); cond += inc;
	layer128_3(bs, cond); cond += inc;
	layer128_2(bs, cond); cond += inc;
	layer128_1(bs, cond); cond += inc;
	layer128_0(bs, cond); cond += inc;
	
	transpose_64x128_sp( bs );
	
	layer128_x(bs, cond); cond += inc;
	layer128_5(bs, cond); cond += inc;
	layer128_4(bs, cond); cond += inc;
	layer128_3(bs, cond); cond += inc;
	layer128_2(bs, cond); cond += inc;
	layer128_1(bs, cond); cond += inc;
	layer128_0(bs, cond); cond += inc;
	layer128_1(bs, cond); cond += inc;
	layer128_2(bs, cond); cond += inc;
	layer128_3(bs, cond); cond += inc;
	layer128_4(bs, cond); cond += inc;
	layer128_5(bs, cond); cond += inc;
	layer128_x(bs, cond); cond += inc;

	transpose_64x128_sp( bs );
	
	layer128_0(bs, cond); cond += inc;
	layer128_1(bs, cond); cond += inc;
	layer128_2(bs, cond); cond += inc;
	layer128_3(bs, cond); cond += inc;
	layer128_4(bs, cond); cond += inc;
	layer128_5(bs, cond); cond += inc;
}

