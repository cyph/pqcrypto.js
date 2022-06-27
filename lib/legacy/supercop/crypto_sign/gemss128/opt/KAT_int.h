#ifndef _KAT_INT_H
#define _KAT_INT_H


/** Define KAT_INT to enable the creation of intermediate KAT (Known Answer 
 *  Tests) files. */
#undef KAT_INT

#ifdef KAT_INT
    #include <stdio.h>
    #include "prefix_name.h"

    /** Error value from fopen. */
    #define KAT_INT_FILE_OPEN_ERROR -1
    /** Error value from fclose. */
    #define KAT_INT_FILE_CLOSE_ERROR -1

    int PREFIX_NAME(open_files_KAT_INT)(FILE** fp);
    void PREFIX_NAME(fprintBstr_KAT_INT)(FILE* fp, const char *S, \
                                       unsigned char *A, unsigned long long L);
    #define open_files_KAT_INT PREFIX_NAME(open_files_KAT_INT)
    #define fprintBstr_KAT_INT PREFIX_NAME(fprintBstr_KAT_INT)

    /** Macro to open a intermediate KAT file. */
    #define OPEN_KAT_INT_FILE\
        FILE* fp;\
        if(open_files_KAT_INT(&fp))\
        {\
            return KAT_INT_FILE_OPEN_ERROR;\
        }

    /** Macro to close a intermediate KAT file. */
    #define CLOSE_KAT_INT_FILE\
        if(fclose(fp))\
        {\
            return KAT_INT_FILE_CLOSE_ERROR;\
        }

#endif

#endif
