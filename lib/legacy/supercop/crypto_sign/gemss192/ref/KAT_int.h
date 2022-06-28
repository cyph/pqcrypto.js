#ifndef _KAT_INT_H
#define _KAT_INT_H


#undef KAT_INT

#ifdef KAT_INT
    #include <stdio.h>

    #define KAT_INT_FILE_OPEN_ERROR -1
    #define KAT_INT_FILE_CLOSE_ERROR -1

    int open_files_KAT_INT(FILE** fp);
    void fprintBstr_KAT_INT(FILE* fp, const char *S, unsigned char *A, unsigned long long L);


    #define OPEN_KAT_INT_FILE\
        FILE* fp;\
        if(open_files_KAT_INT(&fp))\
        {\
            return KAT_INT_FILE_OPEN_ERROR;\
        }

    #define CLOSE_KAT_INT_FILE\
        if(fclose(fp))\
        {\
            return KAT_INT_FILE_CLOSE_ERROR;\
        }

#endif

#endif
