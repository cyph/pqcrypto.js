#ifndef _PRINTHFE_H
#define _PRINTHFE_H

#include "prefix_name.h"
#include "matrix_gf2.h"
#include <stdio.h>


#define printParameters_HFE()\
    puts("(K,D,n,Delta,v,nb_ite,s)");\
    printf("(%u,%u,%u,%u,%u,%u,%u)\n",K,HFEDeg,HFEn,HFEDELTA,HFEv,NB_ITE,HFEs);


void PREFIX_NAME(printDocument)(const unsigned char* doc, unsigned long long len);
void PREFIX_NAME(printSignature)(const unsigned char* sm);
void PREFIX_NAME(printSignature_uncompressed)(const UINT* sm, const char* name);

#define printDocument PREFIX_NAME(printDocument)
#define printSignature PREFIX_NAME(printSignature)
#define printSignature_uncompressed PREFIX_NAME(printSignature_uncompressed)

#endif
