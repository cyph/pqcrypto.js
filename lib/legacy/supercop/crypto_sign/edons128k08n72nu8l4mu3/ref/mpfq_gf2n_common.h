#ifndef MPFQ_GF2N_COMMON_H_
#define MPFQ_GF2N_COMMON_H_

#include "mpfq.h"

#ifdef __cplusplus
extern "C" {
#endif

// type for small field char 2
typedef struct {
    int io_type;
} mpfq_2_field_struct;

typedef mpfq_2_field_struct mpfq_2_field[1];
typedef mpfq_2_field_struct * mpfq_2_dst_field;

#if GNUC_VERSION(4,3,0) || GNUC_VERSION(4,3,1)
#warning "Your GCC version is buggy. Binary fields may fail randomly"
#endif

#ifdef __cplusplus
}
#endif

#endif	/* MPFQ_GF2N_COMMON_H_ */
