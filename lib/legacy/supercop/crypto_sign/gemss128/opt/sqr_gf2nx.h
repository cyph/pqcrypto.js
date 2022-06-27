#ifndef _SQR_GF2NX_H
#define _SQR_GF2NX_H

#include "prefix_name.h"
#include "gf2nx.h"


void PREFIX_NAME(sqr_gf2nx)(gf2nx poly, unsigned int d);
void PREFIX_NAME(sqr_HFE_gf2nx)(gf2nx poly);

#define sqr_gf2nx PREFIX_NAME(sqr_gf2nx)
#define sqr_HFE_gf2nx PREFIX_NAME(sqr_HFE_gf2nx)

#endif

