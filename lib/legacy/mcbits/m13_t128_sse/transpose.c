#include "transpose.h"

void transpose_64x128_sp_asm(vec128 *);

void transpose_64x128_sp(vec128 *in)
{
	transpose_64x128_sp_asm(in);
}

