
#ifndef SYCON_H
#define SYCON_H

#include<math.h>
#include<stdlib.h>


#define STATEBYTES		40 //SIZE of state in bytes
#define NUMROUNDS		14 //Number of Rounds
#define NUMROUNDSR96              9 //Number of Rounds

typedef unsigned long long u64;

unsigned char rotl8 ( const unsigned char x, const unsigned char y, const unsigned char shift );

void sycon_perm( unsigned char *input );

void sycon_perm_r96( unsigned char *input );

void print_sycon_state( const unsigned char *state );

void sycon_perm_ALLZERO ( unsigned char *state );

void sycon_perm_ALLONE ( unsigned char *state );

#endif
