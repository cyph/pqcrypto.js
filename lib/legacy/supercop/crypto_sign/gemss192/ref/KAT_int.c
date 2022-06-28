#include "KAT_int.h"


#ifdef KAT_INT
#include "api.h"

int open_files_KAT_INT(FILE** fp)
{
    char fn_int[32];

    sprintf(fn_int, "PQCsignKAT_%d.int", CRYPTO_SECRETKEYBYTES);
    if ( (*fp = fopen(fn_int, "a")) == NULL ) {
        printf("Couldn't open <%s> for write\n", fn_int);
        return KAT_INT_FILE_OPEN_ERROR;
    }

    return 0;
}

/* This function is a modified version of fprintBstr from PQCgenKAT_sign.c */
void fprintBstr_KAT_INT(FILE* fp, const char *S, unsigned char *A, unsigned long long L)
{
    unsigned long long  i;

    fprintf(fp, "%s", S);

    for ( i=0; i<L; i++ )
        fprintf(fp, "%02X", A[i]);

    if ( L == 0 )
        fprintf(fp, "00");

    fprintf(fp, "\n");
}
#endif

