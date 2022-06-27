/// @file utils.c
/// @brief Implementations for utils.h
///

#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>




int byte_fdump(FILE * fp, const char * extra_msg , const unsigned char *v, unsigned n_byte)
{
	int r = 0;
	int t = fprintf( fp , "%s = ", extra_msg );
	if( 0 > t ) return t;
	else r += t;

	for(unsigned i=0;i<n_byte;i++) {
		int t = fprintf( fp , "%02x", v[i] );
		if( 0 > t ) return t;
		else r += t;
	}
	return r;
}


static inline
int is_empty( char c )
{
	if( ' ' == c ) return 1;
	if( '\t' == c ) return 1;
	if( '\n' == c ) return 1;
	return 0;
}

unsigned byte_fget( FILE * fp, unsigned char *v , unsigned n_byte )
{
	char c0 = 0, c1 = 0;
	char vv[3]; vv[2] = '\0';

	while( EOF != c0 ) {
		c0 = fgetc( fp );
		if( ('=' == c0) ) break;
	}

	int r = 0;
	while( 1 ) {
		while( is_empty( c0 = fgetc(fp) ) ) ;
		c1 = fgetc( fp );
		if( EOF == c0 ) break;
		if( EOF == c1 ) break;

		vv[0] = c0;
		vv[1] = c1;

		int value = 0;
		int t = sscanf( vv , "%2x" , &value );
		if( 1 == t ) {
			v[r] = value;
			r++;
		}
		else break;
		if( n_byte == (unsigned)r ) return r;
	}
	if( 0 < r ) return r;
	return -1;
}



int byte_from_file( unsigned char *v , unsigned n_byte , const char * f_name )
{
	FILE * fp = fopen( f_name , "r");
	if( NULL == fp ) return -1;
	unsigned r = byte_fget( fp ,  v , n_byte );
	fclose( fp );
	if( r != n_byte ) return -2;
	return 0;
}



int byte_from_binfile( unsigned char *v , unsigned n_byte , const char * f_name )
{
	FILE * fp = fopen( f_name , "r");
	if( NULL == fp ) return -1;
	unsigned r = fread( v,  1 , n_byte , fp );
	fclose( fp );
	if( r != n_byte ) return -2;
	return 0;
}



////////////////////////////////////////////////////////////////////



int byte_read_file( unsigned char ** msg , unsigned long long * len , const char * f_name )
{

	if( NULL == msg[0] ) {
		msg[0] = (unsigned char *)malloc(4096);
		if( NULL == msg[0] ) return -1;
	} else {
		return -2;
	}
	FILE * fp = fopen( f_name , "rb" );
	if( NULL == fp ) return -3;

	unsigned long long size = 4096;
	unsigned long long total_read = 0;

	unsigned n_read = 0;
	while( 0 != (n_read = fread( msg[0]+total_read , 1 , 4096 , fp ) ) ) {
		total_read += n_read;
		if( 4096 != n_read ) break;
		if( total_read + 4096 > size ) {
			size *= 2;
			msg[0] = realloc( msg[0] , size );
			if( NULL == msg[0] ) return -1;
		}
	}
	len[0] = total_read;
	fclose( fp );
	return 0;
}
