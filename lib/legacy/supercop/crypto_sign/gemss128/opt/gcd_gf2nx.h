#ifndef _GCD_GF2NX_H
#define _GCD_GF2NX_H

#include "prefix_name.h"
#include "arch.h"
#include "gf2nx.h"


unsigned int PREFIX_NAME(gcd_gf2nx)(unsigned int* b, gf2nx A, unsigned int da, \
                                        gf2nx B, unsigned int db);
#define gcd_gf2nx PREFIX_NAME(gcd_gf2nx)


#endif

