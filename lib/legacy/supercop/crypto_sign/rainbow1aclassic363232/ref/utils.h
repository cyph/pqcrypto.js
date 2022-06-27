/// @file utils.h
/// @brief Utilities for IO.
///
///
#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdio.h>
#include <stdlib.h>


#ifdef  __cplusplus
extern  "C" {
#endif



int byte_fdump(FILE * fp, const char * extra_msg , const unsigned char *v, unsigned n_byte);

unsigned byte_fget( FILE * fp, unsigned char *v , unsigned n_byte );

int byte_from_file( unsigned char *v , unsigned n_byte , const char * f_name );

int byte_from_binfile( unsigned char *v , unsigned n_byte , const char * f_name );


int byte_read_file( unsigned char ** msg , unsigned long long * len , const char * f_name );


#ifdef  __cplusplus
}
#endif



#endif // _UTILS_H_


