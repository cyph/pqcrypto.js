#include "print_gf2x.h"
#include "bit.h"
#include <stdio.h>

/* To print words like an element of GF(2^k) */



/* To print "+" excepted if we are going to print the first element */
#define MACRO_PLUS(b) \
    if(b) \
    { \
        printf(" + "); \
    } \
    else \
    { \
        (b)=1; \
    }

/* To print the first word */
static void print_gf2x_init(UINT A, unsigned int k, unsigned char* b)
{
    UINT tmp=A&mask64(k);
    unsigned int i=2;

    if(tmp&1) 
    {
        printf("1");
        *b=1;
    }
    tmp>>=1;

    if(tmp&1)
    {
        MACRO_PLUS(*b);
        printf("a");
    }

    while(tmp!=0)
    {
        tmp>>=1;
        if(tmp&1) 
        {
            MACRO_PLUS(*b);
            printf("a^%u",i);
        }
        ++i;
    }
}

/* To print the others words */
static void print_gf2x_block64(UINT A, unsigned int k, unsigned int j, unsigned char* b)
{
    UINT tmp=A&mask64(k);
    unsigned int i=j*(NB_BITS_UINT);

    while(tmp!=0)
    {
        if(tmp&1) 
        {
            MACRO_PLUS(*b);
            printf("a^%u",i);
        }
        ++i;
        tmp>>=1;
    }
}

/* To print words like an element of GF(2^k) */
void PREFIX_NAME(print_gf2x)(const UINT A[], unsigned int k)
{
    if(!k)
    {
        puts("k=0 for GF(2^k)");
        return;
    }

    /* k = nbword*NB_BITS_UINT + r */
    unsigned int i,nb_word=k/NB_BITS_UINT,r=k%NB_BITS_UINT;
    /* A boolean to know if the first term is printed */
    unsigned char b=0;

    if(r!=0)
    {
        ++nb_word;
    }

    if(nb_word==1) 
    {
        print_gf2x_init(*A,r,&b);
    }
    else 
    {
        print_gf2x_init(*A,0,&b);

        for(i=1;i<nb_word-1;++i)
        {
            print_gf2x_block64(A[i],0,i,&b);
        }
        print_gf2x_block64(A[i],r,i,&b);
    }

    /* if b=0, the number to print is 0 */
    if(!b)
    {
        printf("0");
    }
}

