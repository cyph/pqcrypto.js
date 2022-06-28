/* MPFQ generated file -- do not edit */

#include "mpfq_2_192.h"

#if !(GMP_LIMB_BITS == 64)
#error "Constraints not met for this file: GMP_LIMB_BITS == 64"
#endif
/* Active handler: field */
/* Automatically generated code for GF(2^192) */
/* Definition polynomial P = X^192 + X^7 + X^2 + X + 1 */
/* Active handler: trivialities */
/* Active handler: io */
/* Active handler: linearops */
/* Active handler: inversion */
/* Active handler: reduction */
/* Active handler: mul */
/* Options used: slice=4 coeffs=[192, 7, 2, 1, 0] tag=2_192 helper=/home/mark/mpfq10rc3/gf2n/helper/helper w=64 n=192 table=/home/mark/mpfq10rc3/gf2x/wizard.table output_path=/home/mark/mpfq10rc3/gf2n */

/* Functions operating on the field structure */

/* Element allocation functions */

/* Elementary assignment functions */

/* Assignment of random values */

/* Arithmetic operations on elements */

/* Operations involving unreduced elements */

/* Comparison functions */

/* Vector allocation functions */

/* Montgomery representation conversion functions */

/* Input/output functions */
void mpfq_2_192_asprint(mpfq_2_192_dst_field k, char * * pstr, mpfq_2_192_src_elt x)
{
    int type = k->io_type;
    int i, n; 
    
    // Numerical io.
    if (type <= 16) {
        // allocate enough room for base 2 conversion.
        *pstr = (char *)malloc((192+1)*sizeof(char));
        if (*pstr == NULL)
            MALLOC_FAILED();
    
        mp_limb_t tmp[3 + 1];
        for (i = 0; i < 3; ++i)
            tmp[i] = x[i];
    
        // mpn_get_str() needs a non-zero most significant limb
        int msl = 3 - 1;
        while ((msl > 0) && (tmp[msl] == 0))
            msl--;
        msl++;
        if ((msl == 1) && (tmp[0] == 0)) {
            (*pstr)[0] = '0';
            (*pstr)[1] = '\0';
            return;
        }
        n = mpn_get_str((unsigned char*)(*pstr), type, tmp, msl);
        for (i = 0; i < n; ++i) {
            char c = (*pstr)[i] + '0';
            if (c > '9')
                c = c-'0'+'a'-10;
            (*pstr)[i] = c;
        }
        (*pstr)[n] = '\0';
    
        // Remove leading 0s
        int shift = 0;
        while (((*pstr)[shift] == '0') && ((*pstr)[shift+1] != '\0')) 
            shift++;
        if (shift>0) {
            i = 0;
            while ((*pstr)[i+shift] != '\0') {
                (*pstr)[i] = (*pstr)[i+shift];
                i++;
            }
            (*pstr)[i] = '\0';
        }
    
        // Return '0' instead of empty string for zero element
        if ((*pstr)[0] == '\0') {
            (*pstr)[0] = '0';
            (*pstr)[1] = '\0';
        }
    } 
    // Polynomial io.
    else {
        char c = (char)type;
        // allocate (more than) enough room for polynomial conversion.
        // Warning: this is for exponent that fit in 3 digits
        *pstr = (char *)malloc((8*192+1)*sizeof(char));
        if (*pstr == NULL)
            MALLOC_FAILED();
        {
            unsigned int j;
            int sth = 0;
            char *ptr = *pstr;
            for(j = 0 ; j < 192 ; j++) {
                if (x[j/64] >> (j % 64) & 1UL) {
                	if (sth) {
                        *ptr++ = ' ';
                        *ptr++ = '+';
                        *ptr++ = ' ';
                    }
                	sth = 1;
                	if (j == 0) {
                        *ptr++ = '1';      
                	} else if (j == 1) {
                        *ptr++ = c;      
                	} else {
                        int ret = sprintf(ptr,"%c^%d",c,j);
                        ptr += ret;
                	}
                }
            }
            if (!sth) {
                *ptr++ = '0';
            }
            *ptr = '\0';
        }
    }
}

void mpfq_2_192_fprint(mpfq_2_192_dst_field k, FILE * file, mpfq_2_192_src_elt x)
{
    char *str;
    mpfq_2_192_asprint(k,&str,x);
    fprintf(file,"%s",str);
    free(str);
}

int mpfq_2_192_sscan(mpfq_2_192_dst_field k, mpfq_2_192_dst_elt z, const char * str)
{
    if (k->io_type <= 16) {
        char *tmp;
        int len = strlen(str);
        tmp = (char *)malloc(len+1);
        int i;
        for (i = 0; i < len; ++i) {
            if (str[i] > '9')
                tmp[i] = str[i] + 10 - 'a';
            else 
                tmp[i] = str[i] - '0';
        }
        mp_limb_t *zz;
        // Allocate one limb per byte... very conservative.
        zz = (mp_limb_t *)malloc(len*sizeof(mp_limb_t));
        int ret = mpn_set_str(zz, tmp, len, k->io_type);
        free(tmp);
        if (ret > 3) {
            free(zz);
            return 0;
        }
        for (i = 0; i < ret; ++i)
            z[i] = zz[i];
        for (i = ret; i < 3; ++i)
            z[i] = 0;
        free(zz);
        return 1;
    } else {
        fprintf(stderr, "Polynomial io not implemented for reading\n");
        return 0;
    }
}

int mpfq_2_192_fscan(mpfq_2_192_dst_field k, FILE * file, mpfq_2_192_dst_elt z)
{
    char *tmp;
    int allocated, len=0;
    int c, start=0;
    allocated=100;
    tmp = (char *)malloc(allocated*sizeof(char));
    if (!tmp)
        MALLOC_FAILED();
    for(;;) {
        c = fgetc(file);
        if (c==EOF)
            break;
        if (isspace(c)) {
            if (start==0)
                continue;
            else
                break;
        } else {
            if (len==allocated) {
                allocated+=100;
                tmp = (char*)realloc(tmp, allocated*sizeof(char));
            }
            tmp[len]=c;
            len++;
        }
    }
    if (len==allocated) {
        allocated+=1;
        tmp = (char*)realloc(tmp, allocated*sizeof(char));
    }
    tmp[len]='\0';
    int ret=mpfq_2_192_sscan(k,z,tmp);
    free(tmp);
    return ret;
}


/* vim:set ft=cpp: */
