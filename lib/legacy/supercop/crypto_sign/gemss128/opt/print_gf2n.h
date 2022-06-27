#ifndef _PRINT_GF2N_H
#define _PRINT_GF2N_H


#include <stdio.h>
#include "parameters_HFE.h"
#include "print_gf2x.h"


#define print_gf2n(A) print_gf2x(A,HFEn); puts("");
#define print_gf2nv(A) print_gf2x(A,HFEnv); puts("");
#define print_gf2m(A) print_gf2x(A,HFEm); puts("");
#define print_gf2v(A) print_gf2x(A,HFEv); puts("");




#endif
