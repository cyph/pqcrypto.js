/*
  This file is for transpose of the Gao-Mateer FFT
*/

#ifndef FFT_TR_H
#define FFT_TR_H
#define fft_tr CRYPTO_NAMESPACE(fft_tr)

#include "params.h"
#include "vec.h"

void fft_tr(vec out[][GFBITS], vec in[][ GFBITS ]);

#endif

