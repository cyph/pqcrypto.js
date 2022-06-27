#ifndef _MUL_GF2X_H
#define _MUL_GF2X_H

#include <stdint.h>
#include "prefix_name.h"
#include "arch.h"

/* Multiplication in GF(2)[x] */


/* Choice of the best multiplication */
#if ENABLED_GF2X
    /* The gf2x library has the priority when enabled.
       In this case, the chosen muliplication is not necessarily the fastest. */
    #include <gf2x/gf2x_mul1.h>
    #include <gf2x/gf2x_mul2.h>
    #include <gf2x/gf2x_mul3.h>
    #include <gf2x/gf2x_mul4.h>
    #include <gf2x/gf2x_mul5.h>
    #include <gf2x/gf2x_mul6.h>
    #include <gf2x/gf2x_mul7.h>
    #include <gf2x/gf2x_mul8.h>
    #include <gf2x/gf2x_mul9.h>

    #ifdef MODE_64_BITS
        #define best_mul32_gf2x mul32_no_simd_gf2x
        #define best_mul64low_gf2x mul64low_no_simd_gf2x

        #define best_mul64_gf2x(C,A,B) gf2x_mul1((C),*(A),*(B))
        #define best_mul128_gf2x(C,A,B) gf2x_mul2((C),(A),(B))
        #define best_mul192_gf2x(C,A,B) gf2x_mul3((C),(A),(B))
        #define best_mul256_gf2x(C,A,B) gf2x_mul4((C),(A),(B))
        #define best_mul320_gf2x(C,A,B) gf2x_mul5((C),(A),(B))
        #define best_mul384_gf2x(C,A,B) gf2x_mul6((C),(A),(B))
        #define best_mul448_gf2x(C,A,B) gf2x_mul7((C),(A),(B))
        #define best_mul512_gf2x(C,A,B) gf2x_mul8((C),(A),(B))
        #define best_mul576_gf2x(C,A,B) gf2x_mul9((C),(A),(B))

        /* XXX The number of word of C must be even XXX */
        #define best_mul96_gf2x  best_mul128_gf2x
        #define best_mul160_gf2x best_mul192_gf2x
        #define best_mul224_gf2x best_mul256_gf2x
        #define best_mul288_gf2x best_mul320_gf2x
        #define best_mul352_gf2x best_mul384_gf2x
        #define best_mul416_gf2x best_mul448_gf2x
        #define best_mul480_gf2x best_mul512_gf2x
        #define best_mul544_gf2x best_mul576_gf2x
    #elif defined(MODE_32_BITS)
        #define best_mul64low_gf2x mul64low_no_simd_gf2x

        /* The number of word of C can be odd */
        #define best_mul32_gf2x(C,A,B) gf2x_mul1((unsigned long*)(C),\
                                                 (unsigned long)(*(A)),\
                                                 (unsigned long)(*(B)))
        #define best_mul64_gf2x(C,A,B) gf2x_mul2((unsigned long*)(C),\
                                                 (const unsigned long*)(A),\
                                                 (const unsigned long*)(B))
        #define best_mul96_gf2x(C,A,B) gf2x_mul3((unsigned long*)(C),\
                                                 (const unsigned long*)(A),\
                                                 (const unsigned long*)(B))
        #define best_mul128_gf2x(C,A,B) gf2x_mul4((unsigned long*)(C),\
                                                 (const unsigned long*)(A),\
                                                 (const unsigned long*)(B))
        #define best_mul160_gf2x(C,A,B) gf2x_mul5((unsigned long*)(C),\
                                                 (const unsigned long*)(A),\
                                                 (const unsigned long*)(B))
        #define best_mul192_gf2x(C,A,B) gf2x_mul6((unsigned long*)(C),\
                                                 (const unsigned long*)(A),\
                                                 (const unsigned long*)(B))
        #define best_mul224_gf2x(C,A,B) gf2x_mul7((unsigned long*)(C),\
                                                 (const unsigned long*)(A),\
                                                 (const unsigned long*)(B))
        #define best_mul256_gf2x(C,A,B) gf2x_mul8((unsigned long*)(C),\
                                                 (const unsigned long*)(A),\
                                                 (const unsigned long*)(B))
        #define best_mul288_gf2x(C,A,B) gf2x_mul9((unsigned long*)(C),\
                                                 (const unsigned long*)(A),\
                                                 (const unsigned long*)(B))

        /* Prototype of gf2x_mul from the gf2x library */
        extern void gf2x_mul(unsigned long *c,
	        	const unsigned long *a, unsigned long an,
	        	const unsigned long *b, unsigned long bn);

        #define best_mul320_gf2x(C,A,B) gf2x_mul((unsigned long*)(C),\
                                                 (const unsigned long*)(A),10,\
                                                 (const unsigned long*)(B),10)
        #define best_mul352_gf2x(C,A,B) gf2x_mul((unsigned long*)(C),\
                                                 (const unsigned long*)(A),11,\
                                                 (const unsigned long*)(B),11)
        #define best_mul384_gf2x(C,A,B) gf2x_mul((unsigned long*)(C),\
                                                 (const unsigned long*)(A),12,\
                                                 (const unsigned long*)(B),12)
        #define best_mul416_gf2x(C,A,B) gf2x_mul((unsigned long*)(C),\
                                                 (const unsigned long*)(A),13,\
                                                 (const unsigned long*)(B),13)
        #define best_mul448_gf2x(C,A,B) gf2x_mul((unsigned long*)(C),\
                                                 (const unsigned long*)(A),14,\
                                                 (const unsigned long*)(B),14)
        #define best_mul480_gf2x(C,A,B) gf2x_mul((unsigned long*)(C),\
                                                 (const unsigned long*)(A),15,\
                                                 (const unsigned long*)(B),15)
        #define best_mul512_gf2x(C,A,B) gf2x_mul((unsigned long*)(C),\
                                                 (const unsigned long*)(A),16,\
                                                 (const unsigned long*)(B),16)
        #define best_mul544_gf2x(C,A,B) gf2x_mul((unsigned long*)(C),\
                                                 (const unsigned long*)(A),17,\
                                                 (const unsigned long*)(B),17)
        #define best_mul576_gf2x(C,A,B) gf2x_mul((unsigned long*)(C),\
                                                 (const unsigned long*)(A),18,\
                                                 (const unsigned long*)(B),18)
    #endif
#endif



#endif

