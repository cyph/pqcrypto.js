#include "printHFE.h"
#include "sizes_HFE.h"
#include <stdio.h>
#include "print_gf2x.h"


void PREFIX_NAME(printDocument)(const unsigned char* doc, unsigned long long len)
{
    unsigned long long int i;
    printf("Document: ");
    for(i=0;i<len;++i)
    {
        printf("%02X",doc[i]);
    }
    puts("");
}


/* Print a signature */
void PREFIX_NAME(printSignature)(const unsigned char* sm)
{
    unsigned int i;
    for(i=0;i<SIZE_SIGN_HFE;++i)
    {
        printf("%02X",sm[i]);
    }
}


/* Print a vector of n+v bits + (NB_ITE-1) vectors of (n+v-m) bits */
void PREFIX_NAME(printSignature_uncompressed)(const UINT* sm, const char* name)
{
    unsigned int i;

    printf("%s:=[",name);
    PRINT_UINT(sm[0]);
    for(i=1;i<NB_WORD_GF2nv;++i)
    {
        printf(",");
        PRINT_UINT(sm[i]);
    }
    puts("];");

    #if (NB_ITE>1)
        unsigned int j;
        for(j=0;j<(NB_ITE-1);++j)
        {
            printf("X%u:=[",NB_ITE-2-j);
            PRINT_UINT(sm[NB_WORD_GF2nvm*j + NB_WORD_GF2nv]);
            for(i=1;i<NB_WORD_GF2nvm;++i)
            {
                printf(",");
                PRINT_UINT(sm[NB_WORD_GF2nvm*j + NB_WORD_GF2nv+i]);
            }
            puts("];");
        }
    #endif
}


