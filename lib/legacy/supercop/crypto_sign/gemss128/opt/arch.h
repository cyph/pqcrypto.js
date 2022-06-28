#ifndef _ARCH_H
#define _ARCH_H

#include <stdint.h>

#define UINT uint64_t
#define NB_BITS_UINT 64U

#define PRINT_UINT(a) printf("0x%lx",a);


#if (NB_BITS_UINT!=64U)
    #define RESERVED_VARIABLE reserved_variable

    #define FOR_LOOP(OP,SIZE) \
        {UINT RESERVED_VARIABLE; \
            for(RESERVED_VARIABLE=0U;RESERVED_VARIABLE<(SIZE);++RESERVED_VARIABLE) \
            { \
                OP;\
            } \
        }

    #define FOR_LOOP_COMPLETE(INIT,CMP,INC,OP) \
        {UINT RESERVED_VARIABLE; \
            for(RESERVED_VARIABLE=INIT;CMP;INC) \
            { \
                OP;\
            } \
        }
#endif



#endif
