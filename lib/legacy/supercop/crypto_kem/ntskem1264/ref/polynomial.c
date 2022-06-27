/**
 *  polynomial.c
 *  NTS-KEM
 *
 *  Parameter: NTS-KEM(12, 64)
 *
 *  This file is part of the reference implemention of NTS-KEM
 *  submitted as part of NIST Post-Quantum Cryptography
 *  Standardization Process.
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "polynomial.h"
#include "bits.h"
#include "random.h"
#include "additive_fft.h"

#define COPY_POLY(a, b)  (b)->degree = (a)->degree;\
                         memcpy((b)->coeff, (a)->coeff, (a)->size*sizeof(ff_unit))

poly* init_poly(int size)
{
    poly* px = (poly *)malloc(sizeof(poly));
    if (!px)
        return NULL;
    
    px->size = size;
    px->degree = -1; /* Indicate a zero polynomial */
    px->coeff = (ff_unit *)calloc(size, sizeof(ff_unit));
    if (!px->coeff)
        return NULL;
    
    return px;
}

void free_poly(poly* px)
{
    if (px) {
        if (px->coeff)
            free(px->coeff);
        px->coeff = NULL;
        px->degree = -1;
        px->size = 0;
        free(px);
    }
}

void zero_poly(poly* px)
{
    memset(px->coeff, 0, px->size*sizeof(ff_unit));
}

poly* clone_poly(const poly *px)
{
    poly *qx = init_poly(px->size);
    if (!qx)
        return NULL;
    
    memcpy(qx->coeff, px->coeff, px->size*sizeof(ff_unit));
    qx->degree = px->degree;
    
    return qx;
}

int is_equal_poly(const poly* ax, const poly* bx)
{
    if (ax->degree != bx->degree)
        return 0;
    return (0 == memcmp(ax->coeff, bx->coeff, (ax->degree+1)*sizeof(ff_unit)));
}

void update_poly_degree(poly *px)
{
    px->degree = px->size-1;
    while (px->degree > 0 && !px->coeff[px->degree]) --px->degree;
}

poly* create_random_poly(const FF2m* ff2m, int degree)
{
    int i;
    ff_unit mask = (1 << ff2m->m)-1;
    poly* px = init_poly(degree+1);
    if (!px)
        return NULL;
    
    px->degree = degree;
    randombytes((uint8_t *)px->coeff, px->degree*sizeof(ff_unit));
    px->coeff[px->degree] = 1;
    
    /* Make sure that the leading coefficient is non zero */
    while (!px->coeff[0])
        randombytes((uint8_t *)px->coeff, sizeof(ff_unit));
    
    /**
     * Mask the coefficient so that it does not go over 
     * the highest finite field value 
     **/
    i = px->size; do { --i;
        px->coeff[i] &= mask;
    } while (i);
    
    return px;
}

ff_unit evaluate_poly(const FF2m* ff2m, const poly* px, ff_unit a)
{
    int i;
    ff_unit x, y;
    if (!a) { /* a is zero */
        return px->coeff[0];
    }
    /* ... a is a non zero */
    x = (ff_unit)a;
    y = px->coeff[0];
    for (i=1; i<=px->degree; i++)
    {
        y = ff2m->ff_add(ff2m, y,
                         ff2m->ff_mul(ff2m, px->coeff[i], x));
        x = ff2m->ff_mul(ff2m, x, a);
    }
    return y;
}

int formal_derivative_poly(const poly* fx, poly *dx)
{
    int i;
    
    /* Make sure that d(x) has enough buffer space */
    if (dx->size < fx->size-1)
        return 0;
    
    for (i=0; i<fx->degree; ++i)
    {
        dx->coeff[i] = 0;
        if ((i & 1) == 0)
            dx->coeff[i] = fx->coeff[i+1];
    }
    dx->degree = fx->degree-1;
    for (i=0; i<fx->degree; ++i)
    {
        /* in case coefficients are zero */
        if (dx->coeff[fx->degree-1-i])
            break;
        --dx->degree;
    }
    
    return 1;
}

void modulo_reduce_poly(const FF2m* ff2m, const poly *mx, poly *ax)
{
    int i, j;
    ff_unit a;
    
    while (ax->degree >= mx->degree) {
        a = ff2m->ff_mul(ff2m, ax->coeff[ax->degree],
                         ff2m->ff_inv(ff2m, mx->coeff[mx->degree]));
        j = ax->degree - mx->degree;
        for (i=0; i<mx->degree; ++i,++j) {
            if (mx->coeff[i])
                ax->coeff[j] = ff2m->ff_add(ff2m, ax->coeff[j],
                                            ff2m->ff_mul(ff2m, mx->coeff[i], a));
        }
        ax->coeff[j] = 0;
        while ((ax->degree >= 0) && !ax->coeff[ax->degree])
            --ax->degree;
    }
}

int gcd_poly(const FF2m* ff2m, const poly* ax, const poly *bx, poly *gx)
{
    poly *sx, *tx;
    
    sx = clone_poly(ax);
    tx = clone_poly(bx);
    if (!sx || !tx)
        return 0;
    
    while (tx->degree >= 0) {
        /* g(x) = s(x) */
        COPY_POLY(sx, gx);
        
        /* g(x) = g(x) mod t(x) */
        modulo_reduce_poly(ff2m, tx, gx);
        
        /* s(x) = t(x) */
        COPY_POLY(tx, sx);
        
        /* t(x) = g(x) */
        COPY_POLY(gx, tx);
    }
    COPY_POLY(sx, gx);
    
    free_poly(sx);
    free_poly(tx);
    
    return 1;
}

poly* poly_from_roots(const FF2m* ff2m, const ff_unit* roots, size_t root_size)
{
    int i = 0, j;
    poly* ax = init_poly((int)root_size + 1);
    
    ax->coeff[0] = roots[i];
    ax->coeff[1] = 1;
    ax->degree = 1;
    
    for (i=1; i<(int)root_size; i++) {
        ax->coeff[ax->degree+1] = ax->coeff[ax->degree];
        for (j=ax->degree-1; j>=0; j--) {
            ax->coeff[j+1] = ff2m->ff_add(ff2m, ax->coeff[j],
                                          ff2m->ff_mul(ff2m, ax->coeff[j+1], roots[i]));
        }
        ax->coeff[0] = ff2m->ff_mul(ff2m, ax->coeff[0], roots[i]);
        
        ax->degree++;
        update_poly_degree(ax);
    }
    
    return ax;
}

#if defined(INTERMEDIATE_VALUES)
char* string_from_poly(const FF2m *ff2m, const poly *ax)
{
    int i;
    size_t size = (8 + 9)*ax->size + 1;
	char *str = NULL, *start_ptr = NULL;
	const char *end_ptr = NULL;
    
	if (!(str = (char *)calloc(size, sizeof(char))))
		return NULL;	
	start_ptr = str;
	end_ptr = str + size;

    for (i=0; i<=ax->degree; i++) {
        if (!ax->coeff[i])
            continue;
        switch (ax->coeff[i]) {
            case 1:
                if (!i)
                    start_ptr += snprintf(start_ptr, end_ptr-start_ptr, "1");
                break;
            case 2:
				start_ptr += snprintf(start_ptr, end_ptr-start_ptr, "a");
                break;
            default:
				start_ptr += snprintf(start_ptr, end_ptr-start_ptr, "a^%d", ff2m->poly2log[ax->coeff[i]]);
                break;
        }
        switch (i) {
            case 0: break;
            case 1: start_ptr += snprintf(start_ptr, end_ptr-start_ptr, "x"); break;
            default: start_ptr += snprintf(start_ptr, end_ptr-start_ptr, "x^%d", i); break;
        }
        if (i < ax->degree)
            start_ptr += snprintf(start_ptr, end_ptr-start_ptr, " + ");
    }

    return str;
}

void poly_fprintf(FILE* fp,
                  const char *prefix,
                  const FF2m* ff2m,
                  const poly* ax,
                  const char *suffix)
{
    char *str = string_from_poly(ff2m, ax);
    if (!str)
        return;
    if (prefix) {
        fprintf(fp, "%s", prefix);
    }
    fprintf(fp, "%s", str);
    if (suffix) {
        fprintf(fp, "%s", suffix);
    }
    free(str);
}
#endif /* INTERMEDIATE_VALUES */

