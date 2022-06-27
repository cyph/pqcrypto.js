#ifndef RAINBOW_H
#define RAINBOW_H

#include "GFpow.h"
#define DEBUG 0

#ifdef OUTFILE
void display(GFpow *ap, int const n, int const m, char *ch = " ");
#endif

const int ovn = 42; // oil + vinegar variables
const int v1 = 18;  // first set of vinegar variables: o < v1 < v2 <...< ovn=n
const int un = 3;   // number of entries into Slist, first entry always v1, last always ovn
const int Slist[un] = {v1, 30, ovn};

const int nv1 = ovn - v1;
const int ovn1 = ovn + 1;
const int nv2 = 2 * nv1;
const int ovn2 = 2 * ovn;

int gauss(GFpow ab[], int const, int const); //Definition in Gauss.cpp

#endif /* RAINBOW_H  */