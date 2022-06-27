#ifndef _SIMD_H
#define _SIMD_H

/* This file is unused for the moment */
/* The code should use generic macros for SIMD, in the way to choose these 
   macros in a specific file as simd_intel.h. For the moment, the 
   implementation sometimes uses directly Intel vectorial instructions. */
/* TODO: to replace all Intel instructions by macros from simd_intel.h. */


#include "arch.h"

#if INTEL_PROCESSOR
    #include "simd_intel.h"
#endif



#endif
